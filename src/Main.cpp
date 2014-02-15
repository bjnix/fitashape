/**
	FIT A SHAPE!
*/
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
#include "Player.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace ViconDataStreamSDK::CPP;

ViconDataStreamSDK::CPP::Client MyClient;
std::string HostName = "141.219.28.17:801";//was 141.219.28.107:801


Player* p1;

video::IVideoDriver* driver;
scene::ISceneManager* smgr;

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
IEventReceiver object. There is only one method to override:
IEventReceiver::OnEvent(). This method will be called by the engine once
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
		if (event.EventType == EET_KEY_INPUT_EVENT)
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
	Method that uses keyboard input with local running. 
	W,S,A,D to move an orb, H,J,K,L to switch between them
*/
void moveKeyboard(MyEventReceiver receiver, const f32 frameDeltaTime ){
	
	//check if the user want to switch nodes
	if(receiver.IsKeyDown(KEY_KEY_H))// left hand
		p1->setCurrentLH();
	else if(receiver.IsKeyDown(KEY_KEY_J))// right hand
		p1->setCurrentRH();
	else if(receiver.IsKeyDown(KEY_KEY_K))// left foot
		p1->setCurrentLF();
	else if(receiver.IsKeyDown(KEY_KEY_L))// right foot
		p1->setCurrentRF();

	// This is the movemen speed in units per second.
	const f32 MOVEMENT_SPEED = 5.f;

	/* Check if keys W, S, A or D are being held down, and move the
	sphere node around respectively. */
	core::vector3df nodePosition = p1->currentNode()->getPosition();
	
	if(receiver.IsKeyDown(KEY_KEY_W))
		nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
	else if(receiver.IsKeyDown(KEY_KEY_S))
		nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;
	
	if(receiver.IsKeyDown(KEY_KEY_A))
		nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
	else if(receiver.IsKeyDown(KEY_KEY_D))
		nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;

	p1->currentNode()->setPosition(nodePosition);
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

	vector3df temp[4] ={vector3df(segment[0]->getX()/100,segment[0]->getZ()/100,30),
				vector3df(segment[1]->getX()/100,segment[1]->getZ()/100,30),
				vector3df(segment[2]->getX()/100,segment[2]->getZ()/100,30),
				vector3df(segment[3]->getX()/100,segment[3]->getZ()/100,30)};

	p1->setPositions(temp);

	
		//std::cout << "Translation Data:" << std::endl;
		//std::cout << "( " << ", " << segment[0]->getX()/100/2 << ", " << segment[0]->getZ()/100/2 << ", " << "30" << " )" << std::endl;

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
	text = smgr->addTextSceneNode(device->getGUIEnvironment()->getFont("../assets/bigfont.png"),tmp,video::SColor(255,0,0,0),0,core::vector3df(0,25,30));
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
		bool win = p1->collideAll();
		//if we won then we gain a point and get a new target
		if(win){ 
			p1->randomTargets(); 
			score++;
		}else{ 
			//if we lost then we lose a point
			score--;
			//if we hit a score of -10 the game is over, other wise we just get a new target
			if(score < -10){
				exit(0);
			}else{
				p1->randomTargets(); 
			}
		}
		//reset the clock
		myClock->setTime(0);
		
		//sets what the time is for the next round
		if(score/2 < 10)
			timesUp = 10 - (score/2);

	}else{//else we are not at the end time so just update any collisions
		p1->collideAll();
	}

	//display the clock with updated score
	swprintf(tmp, 255, L"CLOCK: Seconds: %d \t\t Score: %d",timesUp - seconds,score);
 	device->setWindowCaption(tmp);
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
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
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

	//creates the clock.
	createClock(device, smgr);


	p1 = new Player(device, smgr);

IFDEBUG std::cout << "creating the player object \n"<< std::flush;

	//create and draw the limbs
	p1->drawLimbs();

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

printf("GOING IN!!!\n");
		//sets up the player's body and stuff
		p1->startLocation(viSegments,smgr);	
printf("Done calibrating\n");
IFDEBUG std::cout << "Just finished Method Calls \n"<< std::flush;
	}
	else{
		//manually set the initial position of the limbs
		p1->localInitPos();
		//then sets up the body, arms, and legs
		p1->initializePosition();
	}

IFDEBUG std::cout << "about to draw targets \n"<< std::flush;

	//set up the target
	p1->drawTargets();
		

IFDEBUG std::cout << "calling randomTargets\n"<< std::flush;

	//gets a new target for the player to play with
	p1->randomTargets();
	
IFDEBUG std::cout << "calling setCurrent\n"<< std::flush;
	
	//needed for keyboard input
	p1->setCurrentLH();
	
IFDEBUG std::cout << "just setCurrent\n"<< std::flush;

	
	//centers the camera on the players position	
	p1->addCameraScene();
	//smgr->addCameraSceneNode(0, core::vector3df(p1->initLoc[8].X,p1->initLoc[8].Y+5,0), core::vector3df(p1->initLoc[8].X,p1->initLoc[8].Y+5,p1->initLoc[8].Z));

	/*
	//this is how to make different cameras at different points. might use for dgr...
	ICameraSceneNode *camera[3]={0,0,0};
	camera[0] = smgr->addCameraSceneNode(0, core::vector3df(0,0,0), core::vector3df(0,0,100));
	camera[1] = smgr->addCameraSceneNode(0, core::vector3df(30,0,0), core::vector3df(30,0,100));
	camera[2] = smgr->addCameraSceneNode(0, core::vector3df(-30,0,0), core::vector3df(-30,0,100));
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
