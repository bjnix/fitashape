/**
	FIT A SHAPE!
*/
#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

//USED FOR DEBUGING MODE
#define DEBUG 1
#define IFDEBUG if(DEBUG)

#include <irrlicht.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "vector3d.h"
#include "time.h"
#include "ViconClient.h"
#include "ViconSegment.h"


using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace ViconDataStreamSDK::CPP;

ViconDataStreamSDK::CPP::Client MyClient;
std::string HostName = "141.219.28.17:801";//was 141.219.28.107:801


struct CircleNode{
	scene::ISceneNode* node;
	scene::ISceneNode* target;
};

CircleNode current;

CircleNode LH;
CircleNode RH;
CircleNode LF;
CircleNode RF;

scene::ISceneNode * LHTarget;
scene::ISceneNode * RHTarget;
scene::ISceneNode * LFTarget;
scene::ISceneNode * RFTarget;

video::IVideoDriver* driver;

//0=LeftHand, 1=RightHand, 2=LeftFoot 3=RightFoot
//4=LeftShoulder, 5=RightShoulder 6=LeftHip
//7=RightHip, 8=CenterOfBody
vector3df initLoc[9] = {};

f32 LArm; //estimated length of left arm
f32 RArm; //estimated length of right arm
f32 LLeg; //estimated length of left leg
f32 RLeg; //estimated length of right leg
f32 ground; //estimated height of the ground

//create a array char pointers, which we will be using in our game
const char * objects[4] = {"HandL","HandR","FootL","FootR"};

//clock to race and score to keep track of
ITimer* myClock;
int score = 0;
int timesUp = 10;

scene::ITextSceneNode * text;

/*
To receive events like mouse and keyboard input, or GUI events like "the OK
button has been clicked", we need an object which is derived from the
irr::IEventReceiver object. There is only one method to override:
irr::IEventReceiver::OnEvent(). This method will be called by the engine once
when an event happens. What we really want to know is whether a key is being
held down, and so we will remember the current state of each key.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement

	
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


/*
get the current node
*/
scene::ISceneNode * currentNode(){
	return current.node;
}

/*
set the current node
*/
void setCurrent(CircleNode& node){
IFDEBUG std::cout << "in setCurrent\n"<< std::flush;
	current = node;
IFDEBUG std::cout << "leaving setCurrent\n"<< std::flush;
}

/*
This method will detect the relative shape of the body, arms, and legs.
uses the first 4 locations stored in initLoc[] and sets the last 5
*/
void initializePosition(scene::ISceneManager* smgr){
	vector3df LShoulder;
	vector3df RShoulder;
	vector3df LHip;
	vector3df RHip;
	vector3df centerBody;

	ground = initLoc[3].Y; //sets ground to how high right foot is

	LShoulder = core::vector3df(initLoc[2].X,initLoc[0].Y,initLoc[0].Z);//(leftfoot.x,lefthand.y,lefthand.x)
	RShoulder = core::vector3df(initLoc[3].X,initLoc[1].Y,initLoc[1].Z);//(rightfoot.x,righthand.y,righthand.x)
	
	LArm = initLoc[0].getDistanceFrom(LShoulder);//from left hand to left shoulder
	RArm = initLoc[1].getDistanceFrom(RShoulder);//from right hand to right shoulder

	//FUN FACT: your hips are about your arms's length below your shoulders
	LHip = core::vector3df(LShoulder.X,(LShoulder.Y - LArm),LShoulder.Z);
	RHip = core::vector3df(RShoulder.X,(RShoulder.Y - RArm),RShoulder.Z);

	LLeg = initLoc[2].getDistanceFrom(LHip);//left foot to left hip
	RLeg = initLoc[3].getDistanceFrom(RHip);//right foot to right hip

	centerBody = core::vector3df((LHip.X+RHip.X)/2,(LHip.Y + LShoulder.Y)/2,LHip.Z);
	
	initLoc[4] = LShoulder;
	initLoc[5] = RShoulder;
	initLoc[6] = LHip;
	initLoc[7] = RHip;
	initLoc[8] = centerBody;
	
}


/*
This method builds a random shape for the player based off thier initLoc[]
*/
void randomTargets(){
	f32 spin;
	f32 shift;
	vector3df temp;
	vector3df LShoulder;
	vector3df RShoulder;
	vector3df LHip;
	vector3df RHip;
	vector3df centerBody;

	srand (time(0));
	
	spin = (f32) (rand() % 90 - 45);
	f32 bodySpin = spin;

	//rotate the body randomly to one side or the other
	temp = initLoc[4];// position of left shoulder
	temp.rotateXYBy(spin,initLoc[8]);
	LShoulder = temp;

	temp = initLoc[5];// position of right shoulder
	temp.rotateXYBy(spin,initLoc[8]);
	RShoulder = temp;

	temp = initLoc[6];// position of left hip
	temp.rotateXYBy(spin,initLoc[8]);
	LHip = temp;

	temp = initLoc[7];// position of right hip
	temp.rotateXYBy(spin,initLoc[8]);
	RHip = temp;

	//figure out which foot will be on the ground
	if(bodySpin < 0){//Right foot down!
		spin = (f32) (rand() % 50 - 25);
		temp = vector3df(RHip.X,RHip.Y - RLeg,RHip.Z);
		temp.rotateXYBy(spin,RHip);
		RFTarget->setPosition(temp);

		//put the left foot somewhere
		temp = vector3df(LHip.X,LHip.Y - LLeg,LHip.Z);
		temp.rotateXYBy(spin,LHip);
		LFTarget->setPosition(temp);

		//get the amount we have to move up for the foot to be on the ground
		shift = ground - RFTarget->getPosition().Y;
	}else{//left foot down!
		spin = (f32) (rand() % 50 - 25);
		temp = vector3df(LHip.X,LHip.Y - LLeg,LHip.Z);
		temp.rotateXYBy(spin,LHip);
		LFTarget->setPosition(temp);

		//put the right foot somewhere
		spin =  90 - 45 + bodySpin;
		temp = vector3df(RHip.X,RHip.Y - RLeg,RHip.Z);
		temp.rotateXYBy(spin,RHip);
		RFTarget->setPosition(temp);

		//get the amount we have to move up for the foot to be on the ground
		shift = ground - LFTarget->getPosition().Y;
	}

	//move the arms around!
	//left hand first
	spin = rand() % 160 - 80 + bodySpin;
	temp = vector3df(LShoulder.X - LArm,LShoulder.Y ,LShoulder.Z);
	temp.rotateXYBy(spin,LShoulder);
	LHTarget->setPosition(temp);
	//right hand now
	spin = rand() % 160 - 80 + bodySpin;
	temp = vector3df(RShoulder.X + RArm,RShoulder.Y ,RShoulder.Z);
	temp.rotateXYBy(spin,RShoulder);
	RHTarget->setPosition(temp);

	//shift everything to line up with the ground
	LHTarget->setPosition(vector3df(LHTarget->getPosition().X,LHTarget->getPosition().Y + shift,LHTarget->getPosition().Z));
	RHTarget->setPosition(vector3df(RHTarget->getPosition().X,RHTarget->getPosition().Y + shift,RHTarget->getPosition().Z));
	LFTarget->setPosition(vector3df(LFTarget->getPosition().X,LFTarget->getPosition().Y + shift,LFTarget->getPosition().Z));
	RFTarget->setPosition(vector3df(RFTarget->getPosition().X,RFTarget->getPosition().Y + shift,RFTarget->getPosition().Z));
	
}

/*
method to create the target for the limbs and place them in the scene
*/
void drawTargets(scene::ISceneManager* smgr){
	//target for left hand
	LHTarget = smgr->addSphereSceneNode(1);
	if (LHTarget){
		LH.target = LHTarget; //assign the object it's target
		LHTarget->setPosition(LH.node->getPosition()); // set it's position to a temp spot
		LHTarget->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
		LHTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
		LHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	//target for right hand
	RHTarget = smgr->addSphereSceneNode(1);
	if (RHTarget){
		RH.target = RHTarget;
		RHTarget->setPosition(RH.node->getPosition());
		RHTarget->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
		RHTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		RHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	//target for left foot
	LFTarget = smgr->addSphereSceneNode(1);
	if (LFTarget){
		LF.target = LFTarget;
		LFTarget->setPosition(LF.node->getPosition());
		LFTarget->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
		LFTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		LFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	//target for right foot
	RFTarget = smgr->addSphereSceneNode(1);
	if (RFTarget){		
		RF.target = RFTarget;
		RFTarget->setPosition(RF.node->getPosition());
		RFTarget->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
		RFTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		RFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}
/*
method to create the limb orbs and place them in the scene
*/
void drawLimbs(scene::ISceneManager* smgr){
	//left hand
	LH.node = smgr->addSphereSceneNode(1);
	if (LH.node){
		LH.node->setPosition(core::vector3df(-13,10,30)); //set its position
		LH.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp")); //set the texture
		LH.node->setMaterialFlag(video::EMF_LIGHTING, false); //turn of the emf lighting flag
	}

	//right hand
	RH.node = smgr->addSphereSceneNode(1);
	if (RH.node){
		RH.node->setPosition(core::vector3df(13,10,30));
		RH.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
		RH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	//left foot
	LF.node = smgr->addSphereSceneNode(1);
	if (LF.node){
		LF.node->setPosition(core::vector3df(-3,-7,30));
		LF.node->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
		LF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	//right foot
	RF.node = smgr->addSphereSceneNode(1);
	if (RF.node){
		RF.node->setPosition(vector3df(3,-7,30));
		RF.node->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
		RF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}

/*
method to determin if the passed node is close to its targert returns true if collided
*/
bool collide (CircleNode node){
	vector3df nodeLocation = node.node->getPosition();
	vector3df targetLocation = node.target->getPosition();
	double collideDist = 2; // determins how close the orbs have to be. easy to change
	
	//check to see of the distance between the two nodes is less than the required distance
	if(nodeLocation.getDistanceFrom(targetLocation) > collideDist){
		//did not collide so make sure it is the normal texture and transparent
		node.target->setMaterialTexture(0, node.node->getMaterial(0).getTexture(0));
		node.target->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		return false;
	}
	//did collide, change texture to a different one and make it look solid
	node.target->setMaterialTexture(0, driver->getTexture("../assets/particlered.bmp"));
	node.target->setMaterialType(video::EMT_SOLID);	
	return true;
}

/*
	Method that uses keyboard input with local running. 
	W,S,A,D to move an orb, H,J,K,L to switch between them
*/
void moveKeyboard(MyEventReceiver receiver, const f32 frameDeltaTime ){
	
	//check if the user want to switch nodes
	if(receiver.IsKeyDown(irr::KEY_KEY_H))// left hand
		current = LH;
	else if(receiver.IsKeyDown(irr::KEY_KEY_J))// right hand
		current = RH;
	else if(receiver.IsKeyDown(irr::KEY_KEY_K))// left foot
		current = LF;
	else if(receiver.IsKeyDown(irr::KEY_KEY_L))// right foot
		current = RF;

	// This is the movemen speed in units per second.
	const f32 MOVEMENT_SPEED = 5.f;

	/* Check if keys W, S, A or D are being held down, and move the
	sphere node around respectively. */
	core::vector3df nodePosition = currentNode()->getPosition();
	
	if(receiver.IsKeyDown(irr::KEY_KEY_W))
		nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
	else if(receiver.IsKeyDown(irr::KEY_KEY_S))
		nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;
	
	if(receiver.IsKeyDown(irr::KEY_KEY_A))
		nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
	else if(receiver.IsKeyDown(irr::KEY_KEY_D))
		nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;

	currentNode()->setPosition(nodePosition);
}

/*
	TODO - matt
	Placeholder Method that will use the tracking system to move 
	nodes to the current location of the hardbody
	move each node once per method call
*/
void motionTracking(ViconSegment **segment){

	//TODO I think we are better off making a seperate client for the vicon and call its
		//GetFrame function then we will only have to talk to our object not the vicon one.
	if(MyClient.GetFrame().Result != Result::Success ) printf("WARNING: Inside display() and there is no data from Vicon...\n");


	LH.node->setPosition(vector3df(segment[0]->getX()/100,segment[0]->getZ()/100,30));
	RH.node->setPosition(vector3df(segment[1]->getX()/100,segment[1]->getZ()/100,30));
	LF.node->setPosition(vector3df(segment[2]->getX()/100,segment[2]->getZ()/100,30));
	RF.node->setPosition(vector3df(segment[3]->getX()/100,segment[3]->getZ()/100,30));


		//std::cout << "Translation Data:" << std::endl;
		//std::cout << "( " << ", " << segment[0]->getX()/100/2 << ", " << segment[0]->getZ()/100/2 << ", " << "30" << " )" << std::endl;

}

/*
	Method to determine where and when the person is 
	standing when trying to get the initial locations.
	Takes in instance every second and checks to see if the
	play has stood still for the last three seconds
*/
void startLocation(ViconSegment **segment, scene::ISceneManager* smgr){
	bool moving = true;// boolean for when they are still moving
	//positons for the first second
	vector3df LHpos1;
	vector3df RHpos1;
	vector3df LFpos1;
	vector3df RFpos1;

	//positions for the second second
	vector3df LHpos2;
	vector3df RHpos2;
	vector3df LFpos2;
	vector3df RFpos2;

	//position for the third second
	vector3df LHpos3;
	vector3df RHpos3;
	vector3df LFpos3;
	vector3df RFpos3;

	int temp = -1;// keeps track of which group we are going to update
	myClock->start();// start a clock to keep track of time

	//loop to keep checking the locations of the nodes till they come close to stopping
	while(moving){
		myClock->tick();//move the clock
		printf("Finding body... Stand still!\n");		
		
		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll(); 
		driver->endScene();

		//call the motion tracking method to get up to date locaitons
		motionTracking(segment);
		if(temp != 0 && 0 == ((myClock->getTime() / 500) % 60) % 3){ //check if we want to store this pos
			printf("CHECK 1\n");
			LHpos1 = LH.node->getPosition();
			RHpos1 = RH.node->getPosition();
			LFpos1 = LF.node->getPosition();
			RFpos1 = RF.node->getPosition();
			temp = 0;
		}

		//make stuff appear on screen		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll();
		driver->endScene();
	
		//call the motion tracking method to get up to date locaitons
		motionTracking(segment);
		if(temp != 1 && 1 == ((myClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 2\n");
			LHpos2 = LH.node->getPosition();
			RHpos2 = RH.node->getPosition();
			LFpos2 = LF.node->getPosition();
			RFpos2 = RF.node->getPosition();
			temp = 1;
		}
		
		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll();
		driver->endScene();

		//call the motion tracking method to get up to date locaitons
		motionTracking(segment);
		if(temp != 2 && 2 == ((myClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 3\n");
			LHpos3 = LH.node->getPosition();
			RHpos3 = RH.node->getPosition();
			LFpos3 = LF.node->getPosition();
			RFpos3 = RF.node->getPosition();
			temp = 2;
		}

		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll();
		driver->endScene();
		
		//check to see if the player is close to staying still
		double close = .5; //number to define how close is enough
		if(LHpos1.getDistanceFrom(LHpos2) < close && LHpos2.getDistanceFrom(LHpos3) < close && LHpos3.getDistanceFrom(LHpos1) < close &&
			RHpos1.getDistanceFrom(RHpos2) < close && RHpos2.getDistanceFrom(RHpos3) < close && RHpos3.getDistanceFrom(RHpos1) < close &&
			LFpos1.getDistanceFrom(LFpos2) < close && LFpos2.getDistanceFrom(LFpos3) < close && LFpos3.getDistanceFrom(LFpos1) < close &&
			RFpos1.getDistanceFrom(RFpos2) < close && RFpos2.getDistanceFrom(RFpos3) < close && RFpos3.getDistanceFrom(RFpos1) < close){
				//check to see if they look like they are in the right possition
				//makes sure the arms are at about the same hight and  that the arms are about the same length
				//TODO add more restrictions if necessary
				if((LHpos3.Y-RHpos3.Y > -.5 && LHpos3.Y-RHpos3.Y < .5) 
					&& ((LHpos3.Y-LFpos3.Y)-(RHpos3.Y-RFpos3.Y) > -.5 && (LHpos3.Y-LFpos3.Y)-(RHpos3.Y-RFpos3.Y) < .5))
						moving = false; //they have stopped moving!
		}
	}
	//store this position for later use
	initLoc[0] = LHpos3; //left hand
	initLoc[1] = RHpos3; //right hand
	initLoc[2] = LFpos3; //left foot
	initLoc[3] = RFpos3; //right foot
	
	//calls the method that determines what the body looks like
	initializePosition(smgr);
	return;
}
/*VICON code============================================================*/
namespace
{
	std::string Adapt( const bool i_Value )
	{
		return i_Value ? "True" : "False";
	}

	std::string Adapt( const Direction::Enum i_Direction )
	{
		switch( i_Direction )
		{
			case Direction::Forward:
				return "Forward";
			case Direction::Backward:
				return "Backward";
			case Direction::Left:
				return "Left";
			case Direction::Right:
				return "Right";
			case Direction::Up:
				return "Up";
			case Direction::Down:
				return "Down";
			default:
				return "Unknown";
		}
	}

	std::string Adapt( const DeviceType::Enum i_DeviceType )
	{
		switch( i_DeviceType )
		{
			case DeviceType::ForcePlate:
				return "ForcePlate";
			case DeviceType::Unknown:
			default:
				return "Unknown";
		}
	}

	std::string Adapt( const Unit::Enum i_Unit )
	{
		switch( i_Unit )
		{
			case Unit::Meter:
				return "Meter";
			case Unit::Volt:
				return "Volt";
			case Unit::NewtonMeter:
				return "NewtonMeter";
			case Unit::Newton:
				return "Newton";
			case Unit::Kilogram:
				return "Kilogram";
			case Unit::Second:
				return "Second";
			case Unit::Ampere:
				return "Ampere";
			case Unit::Kelvin:
				return "Kelvin";
			case Unit::Mole:
				return "Mole";
			case Unit::Candela:
				return "Candela";
			case Unit::Radian:
				return "Radian";
			case Unit::Steradian:
				return "Steradian";
			case Unit::MeterSquared:
				return "MeterSquared";
			case Unit::MeterCubed:
				return "MeterCubed";
			case Unit::MeterPerSecond:
				return "MeterPerSecond";
			case Unit::MeterPerSecondSquared:
				return "MeterPerSecondSquared";
			case Unit::RadianPerSecond:
				return "RadianPerSecond";
			case Unit::RadianPerSecondSquared:
				return "RadianPerSecondSquared";
			case Unit::Hertz:
				return "Hertz";
			case Unit::Joule:
				return "Joule";
			case Unit::Watt:
				return "Watt";
			case Unit::Pascal:
				return "Pascal";
			case Unit::Lumen:
				return "Lumen";
			case Unit::Lux:
				return "Lux";
			case Unit::Coulomb:
				return "Coulomb";
			case Unit::Ohm:
				return "Ohm";
			case Unit::Farad:
				return "Farad";
			case Unit::Weber:
				return "Weber";
			case Unit::Tesla:
				return "Tesla";
			case Unit::Henry:
				return "Henry";
			case Unit::Siemens:
				return "Siemens";
			case Unit::Becquerel:
				return "Becquerel";
			case Unit::Gray:
				return "Gray";
			case Unit::Sievert:
				return "Sievert";
			case Unit::Katal:
				return "Katal";

			case Unit::Unknown:
			default:
				return "Unknown";
		}
	}
}


void viconExit()
{
    MyClient.DisableSegmentData();
//    MyClient.DisableMarkerData();
//    MyClient.DisableUnlabeledMarkerData();
//    MyClient.DisableDeviceData();

	// TODO: Disconnect seems to cause a hang. -Scott Kuhl
    // Disconnect and dispose
    int t = clock();
    std::cout << " Disconnecting..." << std::endl;
    MyClient.Disconnect();
    int dt = clock() - t;
    double secs = (double) (dt)/(double)CLOCKS_PER_SEC;
    std::cout << " Disconnect time = " << secs << " secs" << std::endl;
}

void viconInit()
{
    // Connect to a server
    std::cout << "Connecting to " << HostName << " ..." << std::flush;
	int attemptConnectCount = 0;
	const int MAX_CONNECT_ATTEMPTS=2;
    while( !MyClient.IsConnected().Connected && attemptConnectCount < MAX_CONNECT_ATTEMPTS)
    {
		attemptConnectCount++;
		bool ok = false;
		ok =( MyClient.Connect( HostName ).Result == Result::Success );
		if(!ok)
			std::cout << "Warning - connect failed..." << std::endl;
		std::cout << ".";
		sleep(1);
    }
	if(attemptConnectCount == MAX_CONNECT_ATTEMPTS)
	{
		printf("Giving up making connection to Vicon system\n");
		return;
	}
    std::cout << std::endl;

    // Enable some different data types
    MyClient.EnableSegmentData();
    //MyClient.EnableMarkerData();
    //MyClient.EnableUnlabeledMarkerData();
    //MyClient.EnableDeviceData();

    std::cout << "Segment Data Enabled: "          << Adapt( MyClient.IsSegmentDataEnabled().Enabled )         << std::endl;
    std::cout << "Marker Data Enabled: "           << Adapt( MyClient.IsMarkerDataEnabled().Enabled )          << std::endl;
    std::cout << "Unlabeled Marker Data Enabled: " << Adapt( MyClient.IsUnlabeledMarkerDataEnabled().Enabled ) << std::endl;
    std::cout << "Device Data Enabled: "           << Adapt( MyClient.IsDeviceDataEnabled().Enabled )          << std::endl;

    // Set the streaming mode
    //MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );
    // MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPullPreFetch );
    MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ServerPush );

    // Set the global up axis
    MyClient.SetAxisMapping( Direction::Forward, 
                             Direction::Left, 
                             Direction::Up ); // Z-up
    // MyClient.SetGlobalUpAxis( Direction::Forward, 
    //                           Direction::Up, 
    //                           Direction::Right ); // Y-up

    Output_GetAxisMapping _Output_GetAxisMapping = MyClient.GetAxisMapping();
    std::cout << "Axis Mapping: X-" << Adapt( _Output_GetAxisMapping.XAxis ) 
			  << " Y-" << Adapt( _Output_GetAxisMapping.YAxis ) 
			  << " Z-" << Adapt( _Output_GetAxisMapping.ZAxis ) << std::endl;

    // Discover the version number
    Output_GetVersion _Output_GetVersion = MyClient.GetVersion();
    std::cout << "Version: " << _Output_GetVersion.Major << "." 
			  << _Output_GetVersion.Minor << "." 
			  << _Output_GetVersion.Point << std::endl;

}
// an atexit() callback:
void exitCallback()
{
	viconExit();
	return;
}

/**
*	This function will print the vicon segment objects' coordinates
*
*	@param *segment is a pointer to a ViconSegment, which we will print.
*	
*/
void printViconData(ViconSegment *segment){
		//double * tester = segment->getTranslation();
		//segment->printSubjectName();
		//std::cout << "Translation Data:" << std::endl;
		//std::cout << "( " << ", " << tester[0]/1000/2 << ", " << segment->getY()/1000/2 << ", " << segment->getZ()/1000/2 << " )" << std::endl;
		//X is left/right
		//Y is forwards and backwards
		//Z is	up/down.
}


/*
This method creates our clock object and displays it in the title bar
*/
void createClock(IrrlichtDevice* device, ISceneManager* smgr){
	myClock = device->getTimer(); //gets the clock from the device
	//set up the string that will go into the title bar
	wchar_t tmp[255] = {};
	swprintf(tmp,255, L"CLOCK: %d",myClock->getTime());
	
	//put the text in the window caption
	//device->setWindowCaption(tmp);
	
	//text that will be displayed on the screen
	text = smgr->addTextSceneNode(device->getGUIEnvironment()->getFont("../assets/fitashape.xml"),tmp,irr::video::SColor(255,0,0,0),0,irr::core::vector3df(0,25,30));
}

/*
This method updates our clock in the title bar 
and also checks to see if we have won depending on the time
*/
void updateClock(IrrlichtDevice* device){ 
	wchar_t tmp[255] = {}; //string to display in the tital bar
	int seconds = (myClock->getTime() / 1000) % 60; //current time

	//display the clock with latest seconds
	swprintf(tmp, 255, L"CLOCK: Seconds: %d \t\t Score: %d",timesUp - seconds,score);
 	
	//device->setWindowCaption(tmp);
	text->setText(tmp);
	
	//check to see if we have ran out of time
	if(seconds >= timesUp){ 
		//checks to see if all the nodes match their targets
		bool win = collide(LH);
		win = collide(RH) && win; 
		win = collide(LF) && win; 
		win = collide(RF) && win; 
		//if we won then we gain a point and get a new target
		if(win){ 
			randomTargets(); 
			score++;
		}else{ 
			//if we lost then we lose a point
			score--;
			//if we hit a score of -10 the game is over, other wise we just get a new target
			if(score < -10){
				exit(0);
			}else{
				randomTargets(); 
			}
		}
		//reset the clock
		myClock->setTime(0);
		
		//sets what the time is for the next round
		if(score/2 < 10)
			timesUp = 10 - (score/2);

	}else{//else we are not at the end time so just update any collisions
		collide(LH);
		collide(RH);
		collide(LF);
		collide(RF);
	}

	//display the clock with updated score
	swprintf(tmp, 255, L"CLOCK: Seconds: %d \t\t Score: %d",timesUp - seconds,score);
 	device->setWindowCaption(tmp);
}


void makeBackground () {

	driver->beginScene();
	ITexture* background = driver->getTexture("../assets/Background(Fitashape).png");
	if ( background == 0 ) printf("Failed to read texture");
	else driver->draw2DImage(background, core::position2d<s32>(0.0f,0.0f));
	driver->endScene();

}

/*
	Main method for the game.
	if argv[1] is "Local" then will run with keyboard inputs.
 */
int main(int argc, char* argv[]){
std::cout << "Starting main \n"<< std::flush;

	//initialize our ViconSegments
	ViconSegment * viSegments[4]; //this is how we could create multiple segments and store them in a single array.

IFDEBUG	std::cout << "creating bool local \n"<< std::flush;

	bool local = false;
	//check to see if we are testing on local or with tracking system
	if(argc !=1 && (strcmp(argv[1],"Local") == 0 || strcmp(argv[1],"local") == 0))
		local = true;

	// ask user for driver
	//video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	video::E_DRIVER_TYPE driverType=irr::video::EDT_OPENGL;
	if (driverType==video::EDT_COUNT)
		return 1;

IFDEBUG std::cout << "creating the event reciever for KB \n"<< std::flush;
	
	// create reciever and device
	MyEventReceiver receiver;
	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(1280, 1024), 16, false, false, false, &receiver);
	if (device == 0)
		return 1; // could not create selected device.
	
	//get the driver and the scene manager
	driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	
IFDEBUG std::cout << "made a scene manager at location:"<<&smgr << "\n"<< std::flush;

	//adds a camera scene node 
	smgr->addCameraSceneNode();

	//makes the background
	makeBackground();

	//creates the clock.
	createClock(device, smgr);

	//create and draw the limbs
	drawLimbs(smgr);

IFDEBUG std::cout << "returned from drawLimbs \n"<< std::flush;


	if(!local){
	//using the tracking system
	
IFDEBUG std::cout << "calling viconInit() \n"<< std::flush;
		//get the initial setup for the player if using tracking system
		viconInit();

		//GetFrame function then we will only have to talk to our object not the vicon one.
		if(MyClient.GetFrame().Result != Result::Success ) printf("WARNING: Inside display() and there is no data from Vicon...\n");


//------------------Creating  Segments!-------------//
	std::string subject_0("HandL");
	std::string segment_0("HandL");
	std::string subject_1("HandR");
	std::string segment_1("HandR");
	std::string subject_2("FootL");
	std::string segment_2("FootL");
	std::string subject_3("FootR");
	std::string segment_3("FootR");
	viSegments[0]= new ViconSegment(subject_0,segment_0,&MyClient);
	viSegments[1]= new ViconSegment(subject_1,segment_1,&MyClient);
	viSegments[2]= new ViconSegment(subject_2,segment_2,&MyClient);
	viSegments[3]= new ViconSegment(subject_3,segment_3,&MyClient);
//------------------Done Creating  Segments!-------------//
IFDEBUG std::cout << objects[0] << std::endl;


		//sets up the player's body and stuff
		startLocation(viSegments,smgr);	

IFDEBUG std::cout << "Just finished Method Calls \n"<< std::flush;
	}
	else{
		//manually set the initial position of the limbs
		initLoc[0]=LH.node->getPosition();
		initLoc[1]=RH.node->getPosition();
		initLoc[2]=LF.node->getPosition();
		initLoc[3]=RF.node->getPosition();
		//then sets up the body, arms, and legs
		initializePosition(smgr);
	}

IFDEBUG std::cout << "about to draw targets \n"<< std::flush;

	//set up the target
	drawTargets(smgr);
		

IFDEBUG std::cout << "calling randomTargets\n"<< std::flush;

	//gets a new target for the player to play with
	randomTargets();
	
IFDEBUG std::cout << "calling setCurrent\n"<< std::flush;
	
	//needed for keyboard input
	setCurrent(LH);
	
IFDEBUG std::cout << "just setCurrent\n"<< std::flush;

	
	//centers the camera on the players position	
	smgr->addCameraSceneNode(0, irr::core::vector3df(initLoc[8].X,initLoc[8].Y+5,0), irr::core::vector3df(initLoc[8].X,initLoc[8].Y+5,initLoc[8].Z));

	/*
	//this is how to make different cameras at different points. might use for dgr...
	ICameraSceneNode *camera[3]={0,0,0};
	camera[0] = smgr->addCameraSceneNode(0, irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,100));
	camera[1] = smgr->addCameraSceneNode(0, irr::core::vector3df(30,0,0), irr::core::vector3df(30,0,100));
	camera[2] = smgr->addCameraSceneNode(0, irr::core::vector3df(-30,0,0), irr::core::vector3df(-30,0,100));
	smgr->setActiveCamera(camera[2]);
	*/

	// In order to do framerate independent movement, we have to know
	// how long it was since the last frame
	u32 then = device->getTimer()->getTime();
	
	//reset the clock for the start of the game!
	myClock->setTime(0);
	while(device->run())
	{

		
		//This code is to test one object at a time//
		//if(DEBUG){printViconData(WandSeg);}//print the debug info for vicon segments
		//if(DEBUG){printViconData(HandOR);}//print the debug info for vicon segments
		//if(DEBUG){printViconData(FootOR);}//print the debug info for vicon segments
		//if(DEBUG){printViconData(HandR);}//print the debug info for vicon segments
		//if(DEBUG){printViconData(FootR);}//print the debug info for vicon segments


		// Work out a frame delta time.
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;
		
		//move the orbs around
		if(local)
			moveKeyboard(receiver, frameDeltaTime);
		else
			motionTracking(viSegments);

		//update the clock and check for win/lose
		updateClock(device);

		//puts the stuff on the screen
		
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		ITexture* background = driver->getTexture("../assets/Background(Fitashape).png");
	if ( background == 0 ) printf("Failed to read texture");
	else driver->draw2DImage(background, core::rect<s32>(core::position2d<s32>(0.0f,0.0f),core::position2d<s32>(800.0f,600.0f)),core::rect<s32>(core::position2d<s32>(0.0f,0.0f),core::position2d<s32>(11520.0f,4320.0f)),0,0,false);
		smgr->drawAll(); // draw the 3d scene
		driver->endScene();

	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	
	return 0;
}

/*
That's it. Compile and play around with the program.
**/
