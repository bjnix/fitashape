/** Example 004 Movement

This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.  We'll demonstrate framerate
independent movement, which means moving by an amount dependent
on the duration of the last run of the Irrlicht loop.

Example 19.MouseAndJoystick shows how to handle those kinds of input.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
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
	CircleNode* nextNode;
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

//TODO delete once we have body tested
//	scene::ISceneNode * LShoulderN;
//	scene::ISceneNode * RShoulderN;
//	scene::ISceneNode * LHipN;
//	scene::ISceneNode * RHipN;

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
This method will detect where the persons body is and the relative shape of the arms and legs.
uses the first 4 locations stored in initLoc[] and sets the last 5
TODO remove last part when done with testing body
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
	
	//TODO ~~~~~delete once we are done testing body~~~~~~~~
	/*LShoulderN = smgr->addSphereSceneNode(.5);
	if (LShoulderN)
	{
		LShoulderN->setPosition(LShoulder);
		LShoulderN->setMaterialTexture(0, NULL);
		LShoulderN->setMaterialFlag(video::EMF_LIGHTING, false);
	}
	RShoulderN = smgr->addSphereSceneNode(.5);
	if (RShoulderN)
	{
		RShoulderN->setPosition(RShoulder);
		RShoulderN->setMaterialTexture(0, NULL);
		RShoulderN->setMaterialFlag(video::EMF_LIGHTING, false);
	}
	LHipN = smgr->addSphereSceneNode(.5);
	if (LHipN)
	{
		LHipN->setPosition(LHip);
		LHipN->setMaterialTexture(0, NULL);
		LHipN->setMaterialFlag(video::EMF_LIGHTING, false);
	}
	RHipN = smgr->addSphereSceneNode(.5);
	if (RHipN)
	{
		RHipN->setPosition(RHip);
		RHipN->setMaterialTexture(0, NULL);
		RHipN->setMaterialFlag(video::EMF_LIGHTING, false);
	}*/
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

	//srand (time(0));
	spin = (f32) (rand() % 90 - 45);
	f32 bodySpin = spin;

	//rotate the body randomly to one side or the other
	temp = initLoc[4];// LShoulderN->getPosition();
	temp.rotateXYBy(spin,initLoc[8]);//centerBody);
	LShoulder = temp;
	//LShoulderN->setPosition(temp);

	temp = initLoc[5];//RShoulderN->getPosition();
	temp.rotateXYBy(spin,initLoc[8]);//centerBody);
	RShoulder = temp;
	//RShoulderN->setPosition(temp);

	temp = initLoc[6];//LHipN->getPosition();
	temp.rotateXYBy(spin,initLoc[8]);//centerBody);
	LHip = temp;
	//LHipN->setPosition(temp);

	temp = initLoc[7];//RHipN->getPosition();
	temp.rotateXYBy(spin,initLoc[8]);//centerBody);
	RHip = temp;
	//RHipN->setPosition(temp);	

	if(bodySpin < 0){//Right foot down!
		spin = (f32) (rand() % 50 - 25);
		temp = vector3df(RHip.X,RHip.Y - RLeg,RHip.Z);
		temp.rotateXYBy(spin,RHip);
		RFTarget->setPosition(temp);

		//put the left foot somewhere
		temp = vector3df(LHip.X,LHip.Y - LLeg,LHip.Z);
		temp.rotateXYBy(spin,LHip);
		LFTarget->setPosition(temp);

		//get the amout to move up
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

		//get the amout to move up
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
	//LShoulderN->setPosition(vector3df(LShoulderN->getPosition().X,LShoulderN->getPosition().Y + shift,LShoulderN->getPosition().Z));
	//RShoulderN->setPosition(vector3df(RShoulderN->getPosition().X,RShoulderN->getPosition().Y + shift,RShoulderN->getPosition().Z));
	//LHipN->setPosition(vector3df(LHipN->getPosition().X,LHipN->getPosition().Y + shift,LHipN->getPosition().Z));
	//RHipN->setPosition(vector3df(RHipN->getPosition().X,RHipN->getPosition().Y + shift,RHipN->getPosition().Z));
	LShoulder = vector3df(LShoulder.X,LShoulder.Y + shift, LShoulder.Z);
	RShoulder = vector3df(RShoulder.X,RShoulder.Y + shift, RShoulder.Z);
	LHip = vector3df(LHip.X,LHip.Y + shift, LHip.Z);
	RHip = vector3df(RHip.X,RHip.Y + shift, RHip.Z);
	centerBody = vector3df(centerBody.X,centerBody.Y + shift, centerBody.Z);
}

/*
method to draw the target location for the limb orbs
*/
void drawTargets(scene::ISceneManager* smgr){
	LHTarget = smgr->addSphereSceneNode(1);
	if (LHTarget)
	{
		LH.target = LHTarget;
		LHTarget->setPosition(LH.node->getPosition());
		LHTarget->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
		LHTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		LHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RHTarget = smgr->addSphereSceneNode(1);
	if (RHTarget)
	{
		RH.target = RHTarget;
		RHTarget->setPosition(RH.node->getPosition());
		RHTarget->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
		RHTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		RHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	LFTarget = smgr->addSphereSceneNode(1);
	if (LFTarget)
	{
		LF.target = LFTarget;
		LFTarget->setPosition(LF.node->getPosition());
		LFTarget->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
		LFTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		LFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RFTarget = smgr->addSphereSceneNode(1);
	if (RFTarget)
	{		
		RF.target = RFTarget;
		RFTarget->setPosition(RF.node->getPosition());
		RFTarget->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
		RFTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		RFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}
/*
method to draw the limb orbs
*/
void drawLimbs(scene::ISceneManager* smgr){
IFDEBUG std::cout << "In drawLimbs\n"<< std::flush;

IFDEBUG std::cout << "LH is: " << &LH << "\n" << std::flush;
IFDEBUG std::cout << "LH.node is: "<< &LH.node << "\n" << std::flush;
IFDEBUG std::cout << "smgr is: "<< &smgr << "\n" << std::flush;
	LH.node = smgr->addSphereSceneNode(1);
IFDEBUG std::cout << "after LH.node is: "<< &LH.node << "\n" << std::flush;
	if (LH.node)
	{
		LH.nextNode = &RH;
		LH.node->setPosition(core::vector3df(-13,10,30));
		LH.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
		LH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

IFDEBUG std::cout << "about to RH node add sphere\n"<< std::flush;
	RH.node = smgr->addSphereSceneNode(1);
	if (RH.node)
	{
		RH.nextNode = &LF;
		RH.node->setPosition(core::vector3df(13,10,30));
		RH.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
		RH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

IFDEBUG std::cout << "about to LF node add sphere\n"<< std::flush;
	LF.node = smgr->addSphereSceneNode(1);
	if (LF.node)
	{
		LF.nextNode = &RF;
		LF.node->setPosition(core::vector3df(-3,-7,30));
		LF.node->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
		LF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

IFDEBUG std::cout << "about to RF node add sphere\n"<< std::flush;
	RF.node = smgr->addSphereSceneNode(1);
	if (RF.node)
	{
		RF.nextNode = &LH;
		RF.node->setPosition(vector3df(3,-7,30));
		RF.node->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
		RF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}

/*
method to determin if the passed node is close to its targert
*/
bool collide (CircleNode node){
	vector3df nodeLocation = node.node->getPosition();
	vector3df targetLocation = node.target->getPosition();
	double collideDist = 2; // determins how close the orbs have to be. easy to change
	
	if(nodeLocation.getDistanceFrom(targetLocation) > collideDist){
		node.target->setMaterialTexture(0, node.node->getMaterial(0).getTexture(0));
		node.target->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);		
		return false;
	}
	node.target->setMaterialTexture(0, driver->getTexture("../assets/particlered.bmp"));
	node.target->setMaterialType(video::EMT_SOLID);	
	return true;
}

/*
	Method that uses keyboard input with local running
*/
void moveKeyboard(MyEventReceiver receiver, const f32 frameDeltaTime ){
	
	//developer keys to show different things
	//TODO remove when we no longer want them
	/*if(receiver.IsKeyDown(irr::KEY_KEY_Z)){
		LShoulderN->setVisible(false);
		RShoulderN->setVisible(false);
		LHipN->setVisible(false);
		RHipN->setVisible(false);
		LHTarget->setVisible(false);
		RHTarget->setVisible(false);
		LFTarget->setVisible(false);
		RFTarget->setVisible(false);
	}
	if(receiver.IsKeyDown(irr::KEY_KEY_X)){
		LShoulderN->setVisible(true);
		RShoulderN->setVisible(true);
		LHipN->setVisible(true);
		RHipN->setVisible(true);
		LHTarget->setVisible(true);
		RHTarget->setVisible(true);
		LFTarget->setVisible(true);
		RFTarget->setVisible(true);
		randomTargets();
	}*/


	//check if the user want to switch nodes
	//if(receiver.IsKeyDown(irr::KEY_SPACE))
	//	current = *current.nextNode;
	if(receiver.IsKeyDown(irr::KEY_KEY_H))
		current = LH;
	else if(receiver.IsKeyDown(irr::KEY_KEY_J))
		current = RH;
	else if(receiver.IsKeyDown(irr::KEY_KEY_K))
		current = LF;
	else if(receiver.IsKeyDown(irr::KEY_KEY_L))
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
	standing when trying to get the initial locations
	*/
void startLocation(ViconSegment **segment, scene::ISceneManager* smgr){
	bool moving = true;
	vector3df LHpos1;
	vector3df RHpos1;
	vector3df LFpos1;
	vector3df RFpos1;

	vector3df LHpos2;
	vector3df RHpos2;
	vector3df LFpos2;
	vector3df RFpos2;

	vector3df LHpos3;
	vector3df RHpos3;
	vector3df LFpos3;
	vector3df RFpos3;

	int temp = -1;//keep track of which one we are going to update
	myClock->start();//start a clock to keep track of time

	//loop to keep checking the locations of the nodes till they come close to stopping
	while(moving){
		myClock->tick();//move the clock
		printf("Finding body... Stand still!\n");		
		
		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll(); // draw the 3d scene
		//device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)
		driver->endScene();


		motionTracking(segment);
		if(temp != 0 && 0 == ((myClock->getTime() / 500) % 60) % 3){ //check if we want to store this pos
			printf("CHECK 1\n");
			LHpos1 = LH.node->getPosition();
			RHpos1 = RH.node->getPosition();
			LFpos1 = LF.node->getPosition();
			RFpos1 = RF.node->getPosition();
			temp = 0;
		}

		//make stuff appear on screen		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll(); // draw the 3d scene
		//device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)
		driver->endScene();
	

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
		smgr->drawAll(); // draw the 3d scene
		//device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)
		driver->endScene();


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
		smgr->drawAll(); // draw the 3d scene
		//device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)
		driver->endScene();

		//printf("LH1: %f,%f,%f, LH2: %f,%f,%f, LH3: %f,%f,%f\n", LHpos1.X,LHpos1.Y,LHpos1.Z,LHpos2.X,LHpos2.Y,LHpos2.Z,LHpos3.X,LHpos3.Y,LHpos3.Z);
		//printf("RH1: %f,%f,%f, RH2: %f,%f,%f, RH3: %f,%f,%f\n", RHpos1.X,RHpos1.Y,RHpos1.Z,RHpos2.X,RHpos2.Y,RHpos2.Z,RHpos3.X,RHpos3.Y,RHpos3.Z);
		//printf("LF1: %f,%f,%f, LF2: %f,%f,%f, LF3: %f,%f,%f\n", LFpos1.X,LFpos1.Y,LFpos1.Z,LFpos2.X,LFpos2.Y,LFpos2.Z,LFpos3.X,LFpos3.Y,LFpos3.Z);
		//printf("RF1: %f,%f,%f, RF2: %f,%f,%f, RF3: %f,%f,%f\n", RFpos1.X,RFpos1.Y,RFpos1.Z,RFpos2.X,RFpos2.Y,RFpos2.Z,RFpos3.X,RFpos3.Y,RFpos3.Z);		
		
		//check to see if they are close to staying still
		double close = .5;
		if(LHpos1.getDistanceFrom(LHpos2) < close && LHpos2.getDistanceFrom(LHpos3) < close && LHpos3.getDistanceFrom(LHpos1) < close &&
			RHpos1.getDistanceFrom(RHpos2) < close && RHpos2.getDistanceFrom(RHpos3) < close && RHpos3.getDistanceFrom(RHpos1) < close &&
			LFpos1.getDistanceFrom(LFpos2) < close && LFpos2.getDistanceFrom(LFpos3) < close && LFpos3.getDistanceFrom(LFpos1) < close &&
			RFpos1.getDistanceFrom(RFpos2) < close && RFpos2.getDistanceFrom(RFpos3) < close && RFpos3.getDistanceFrom(RFpos1) < close){
				//check to see if they look like they are in the right possition
				//TODO add more restrictions if necessary
				if((LHpos3.Y-RHpos3.Y > -.5 && LHpos3.Y-RHpos3.Y < .5) && ((LHpos3.Y-LFpos3.Y)-(RHpos3.Y-RFpos3.Y) > -.5 && (LHpos3.Y-LFpos3.Y)-(RHpos3.Y-RFpos3.Y) < .5))
						moving = false;
				
		}
	}
	initLoc[0] = LHpos3;
	initLoc[1] = RHpos3;
	initLoc[2] = LFpos3;
	initLoc[3] = RFpos3;
	
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
This method creates our clock object and puts it in the title bar
*/
void createClock(IrrlichtDevice* device, ISceneManager* smgr){
	myClock = device->getTimer();
	wchar_t tmp[255] = {};
	swprintf(tmp,255, L"CLOCK: %d",myClock->getTime());
	device->setWindowCaption(tmp);
}

/*
This method updates our clock in the title bar and also checks to see if we have won
depending on the time
*/
void updateClock(IrrlichtDevice* device){ 
	wchar_t tmp[255] = {}; 
	int seconds = (myClock->getTime() / 1000) % 60;

	swprintf(tmp, 255, L"CLOCK: Seconds: %d \t\t Score: %d",timesUp - seconds,score);
 	device->setWindowCaption(tmp);
	//check to see if we have ran out of time
	if(seconds >= timesUp){ //checks to see if all the nodes match their targets
		bool win = collide(LH);
		win = collide(RH) && win; 
		win = collide(LF) && win; 
		win = collide(RF) && win; 
		if(win){ 
			myClock->setTime(0);
			randomTargets(); 
			score++;
		}else{ 
			score--;			
			if(score < -10){
				exit(0);
			}else{
				myClock->setTime(0);
				randomTargets(); 
			}
		}
		if(score/2 < 10)
			timesUp = 10 - (score/2);
		printf("timesUp: %d score: %d\n",timesUp,score);

	}else{
		collide(LH);
		collide(RH);
		collide(LF);
		collide(RF);
	}
}



/*
The event receiver for keeping the pressed keys is ready, the actual responses
will be made inside the render loop, right before drawing the scene. So lets
just create an irr::IrrlichtDevice and the scene node we want to move. We also
create some other additional scene nodes, to show that there are also some
different possibilities to move and animate scene nodes.
*/
/**
 * \param argc currently takes upto one input
 * \param argv expects a string, if "Local" then will run with keyboard inputs.
 */
int main(int argc, char* argv[]){
std::cout << "Starting main \n"<< std::flush;

	//initialize our ViconSegments
	ViconSegment * viSegments[4]; //this is how we could create multiple segments and store them in a single array.

IFDEBUG	std::cout << "creating bool local \n"<< std::flush;

	bool local = false;
	if(argc !=1 && (strcmp(argv[1],"Local") == 0 || strcmp(argv[1],"local") == 0))
		local = true;

	// ask user for driver
	//video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	video::E_DRIVER_TYPE driverType=irr::video::EDT_OPENGL;
	if (driverType==video::EDT_COUNT)
		return 1;

IFDEBUG std::cout << "creating the event reciever for KB \n"<< std::flush;
	// create device
	MyEventReceiver receiver;

	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(1280, 1024), 16, false, false, false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.

	driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
IFDEBUG std::cout << "made a scene manager at location:"<<&smgr << "\n"<< std::flush;

	smgr->addCameraSceneNode();

	createClock(device, smgr);
	//myClock->start();

	//draws the limbs
	drawLimbs(smgr);



IFDEBUG std::cout << "returned from drawLimbs \n"<< std::flush;

	//get the initial position if the person is using the tracking system
	if(!local){
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



		startLocation(viSegments,smgr);	

IFDEBUG std::cout << "Just finished Method Calls \n"<< std::flush;
	}
	else{
		//gets the initial position of the person
		initLoc[0]=LH.node->getPosition();
		initLoc[1]=RH.node->getPosition();
		initLoc[2]=LF.node->getPosition();
		initLoc[3]=RF.node->getPosition();
		initializePosition(smgr);
	}


IFDEBUG std::cout << "about to draw targets \n"<< std::flush;
	//set up the target
	drawTargets(smgr);
		

	srand (time(0));
IFDEBUG std::cout << "calling randomTargets\n"<< std::flush;
	randomTargets();
IFDEBUG std::cout << "calling setCurrent\n"<< std::flush;
	//needed for keyboard input
	setCurrent(LH);
IFDEBUG std::cout << "just setCurrent\n"<< std::flush;

	/*
	Sets the camera view at 0,0,0
	*/
	//smgr->addCameraSceneNode();
	//smgr->addCameraSceneNodeFPS();	
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
		{
			motionTracking(viSegments);
		}

		//update the clock and check for win/lose
		updateClock(device);

		driver->beginScene(true, true, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene
		//device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

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
