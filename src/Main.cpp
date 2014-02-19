/**
	FIT A SHAPE!
*/
//USED FOR DEBUGING MODE
#define DEBUG 1
#define IFDEBUG if(DEBUG)


#include <stdlib.h>
#include <stdio.h>
#include <iostream>


#include "time.h"
#include "irrlicht/vector3d.h"
#include "irrlicht/irrlicht.h"
#include "vicon/ViconInputClient.h"
#include "fitashape/Player.h"
#include "fitashape/Game.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace ViconDataStreamSDK::CPP;

std::string HostName = "141.219.28.17:801";//was 141.219.28.107:801

std::vector<std::string> names = {
	"HandL",
	"HandR",
	"FootL",
	"FootR"
	};

ViconInputClient * vClient;

Player* p1;

video::IVideoDriver* driver;
scene::ISceneManager* smgr;

//create a array char pointers, which we will be using in our game

//clock to race and score to keep track of
ITimer* myClock;
int score = 0;
int timesUp = 10;

scene::ITextSceneNode * text;



/*
	Main method for the game.
	if argv[1] is "Local" then will run with keyboard inputs.
 */
int main(int argc, char* argv[])
{

std::cout << "Starting main \n"<< std::flush;

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


	if(!local)
	{
	//using the tracking system
	
IFDEBUG std::cout << "calling viconInit() \n"<< std::flush;
		//get the initial setup for the player if using tracking system
	vClient = new ViconInputClient("141.219.28.17:801",names,names);

printf("GOING IN!!!\n");
		//sets up the player's body and stuff
		p1->startLocation(vClient,smgr);	
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


		// Work out a frame delta time.
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;
		
		//move the orbs around
		if(local)
			moveKeyboard(receiver, frameDeltaTime);
		else
			motionTracking(vClient->getSolidBodies());

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
