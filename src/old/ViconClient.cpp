//#include <GL/glew.h>
//#include <GL/glut.h>

#include <math.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>

#ifdef WIN32
#include <conio.h>   // For _kbhit()
#include <cstdio>   // For getchar()
#include <windows.h> // For Sleep()
#endif // WIN32

#include "ViconInputClient.h"

const float SCREEN_WIDTH = (1920*6)/8.0;  
const float SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;
 
ViconInputClient::ViconInputClient(int players, Game* g):InputClient(players, g) {
	g->getView()->getOSGViewer()->getView(0)->addEventHandler(this);
	this->viconInitialized = false;
	timerclear(&startTime);
	gettimeofday(&startTime, 0);
	timerclear(&totalTime);
	this->viconInit();
}

ViconInputClient::~ViconInputClient() {
	this->viconExit();
}

void ViconInputClient::viconInit() {
	// After everything's started up
	viconInitialized = true;
	
	// Cleanly send both players forward
	keys->setTheta(0, 0);
	keys->setTheta(1, 0);
	
	// Variables needed for connection
	using namespace ViconDataStreamSDK::CPP;
	char* HostName = "141.219.28.107:801";
	
	// Connect to a server
    printf("Connecting to Vicon server at %s...\n", HostName);
	int attemptConnectCount = 0;
	const int MAX_CONNECT_ATTEMPTS=2;
    while( !viconClient.IsConnected().Connected && attemptConnectCount < MAX_CONNECT_ATTEMPTS)
    {
		attemptConnectCount++;
		bool ok = false;
		ok =( viconClient.Connect( HostName ).Result == Result::Success );
		if(!ok)
			printf("Vicon connection failed. Trying again.\n");
		sleep(1);
    }
	if(attemptConnectCount == MAX_CONNECT_ATTEMPTS)
	{
		printf("Giving up making connection to Vicon system\n");
		return;
	}
	
	printf("Connected to Vicon server.\n");

    // Enable some different data types
    viconClient.EnableSegmentData();
    //viconClient.EnableMarkerData();
    //viconClient.EnableUnlabeledMarkerData();
    //viconClient.EnableDeviceData();

    printf("  Segment Data Enabled: %s\n",          viconClient.IsSegmentDataEnabled().Enabled? "True":"False");
    //printf("Marker Data Enabled: %s\n",           viconClient.IsMarkerDataEnabled().Enabled? "True":"False");
    //printf("Unlabeled Marker Data Enabled: %s\n", viconClient.IsUnlabeledMarkerDataEnabled().Enabled? "True":"False");
    //printf("Device Data Enabled: %s\n",           viconClient.IsDeviceDataEnabled().Enabled? "True":"False");

    // Set the streaming mode
    //viconClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );
    //viconClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPullPreFetch );
    viconClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ServerPush );

    // Set the global up axis
    viconClient.SetAxisMapping( Direction::Forward, 
                             Direction::Left, 
                             Direction::Up ); // Z-up
    // viconClient.SetGlobalUpAxis( Direction::Forward, 
    //                           Direction::Up, 
    //                           Direction::Right ); // Y-up

    Output_GetAxisMapping _Output_GetAxisMapping = viconClient.GetAxisMapping();
    std::cout << "  Axis Mapping: X-" << Adapt( _Output_GetAxisMapping.XAxis ) 
			  << " Y-" << Adapt( _Output_GetAxisMapping.YAxis ) 
			  << " Z-" << Adapt( _Output_GetAxisMapping.ZAxis ) << std::endl;

    // Discover the version number
    Output_GetVersion _Output_GetVersion = viconClient.GetVersion();
    std::cout << "  Vicon Version: " << _Output_GetVersion.Major << "." 
			  << _Output_GetVersion.Minor << "." 
			  << _Output_GetVersion.Point << std::endl;
}

void ViconInputClient::viconExit() {
	if(!viconInitialized) return;
	
	viconClient.DisableSegmentData();
	//viconClient.DisableMarkerData();
	//viconClient.DisableUnlabeledMarkerData();
	//viconClient.DisableDeviceData();

	// TODO: Disconnect seems to cause a hang. -Scott Kuhl
    // Disconnect and dispose
    int t = clock();
    std::cout << "Disconnecting Vicon system..." << std::endl;
    viconClient.Disconnect();
    int dt = clock() - t;
    double secs = (double) (dt)/(double)CLOCKS_PER_SEC;
    std::cout << "  Success. Disconnect time = " << secs << " sec" << std::endl;
    viconInitialized = false;
}

bool ViconInputClient::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa) {
	return false;
}

void ViconInputClient::accept(osgGA::GUIEventHandlerVisitor& v){
	v.visit(*this);
}

void ViconInputClient::advance(struct timeval *tv, int usec) {
	tv->tv_usec += usec;
	while(tv->tv_usec >= 1000000){
		tv->tv_sec++;
		tv->tv_usec -= 1000000;
	}
}

osg::Vec3 ViconInputClient::findObject(std::string objName, std::string segName, int player) {
	Output_GetSegmentGlobalTranslation obj = viconClient.GetSegmentGlobalTranslation(objName, segName);

	if(player == 0) {
		float x = obj.Translation[0]/1000 + 2.2;
		// Ensure player 1 stays to the left
		// That is, -1..0 is now -1..1 for p1, 0..1 is now -1..1 for p2
		if(x < -1) x = -1;
		if(x > 1) x = 1;
	
		// Make a buffer around 0, so players can straigthen out easily
		if(x > -0.10 && x < 0.10) x = 0;
		
		float y = obj.Translation[2]/1000 + 0.0;
		float z = obj.Translation[1]/1000 + 0.0;
		return osg::Vec3(x, y, z);
	}
	else if(player == 1) {
		float x = obj.Translation[0]/1000 - 1.0;
		// Ensure player 2 stays to the right
		// That is, -1..0 is now -1..1 for p1, 0..1 is now -1..1 for p2
		if(x < -1) x = -1;
		if(x > 1) x = 1;
	
		// Make a buffer around 0, so players can straigthen out easily
		if(x > -0.10 && x < 0.10) x = 0;
		
		float y = obj.Translation[2]/1000 + 0.0;
		float z = obj.Translation[1]/1000 + 0.0;
		return osg::Vec3(x, y, z);
	}
	else {
		printf("Player should be 0 or 1 when calling fix()\n");
		return osg::Vec3f(0.0, 0.0, 0.0);
	}
}

void ViconInputClient::viconUpdateKeys() {
	if(viconClient.GetFrame().Result != Result::Success ) {
		printf("WARNING: Vicon missed a frame!\n");
		return;
	}
	
	// Get the location of all of p1's limbs
	osg::Vec3 p1lh  = findObject("P1_HandL", "HandOR", 0);
	osg::Vec3 p1lf  = findObject("P1_FootL", "FootOR", 0);
	osg::Vec3 p1rh  = findObject("P1_HandR", "HandR",  0);
	osg::Vec3 p1rf  = findObject("P1_FootR", "FootR",  0);
	
	// Set all of p1's limbs
	this->keys->setLimb(0, 0, p1lh.x(), p1lh.y());
	this->keys->setLimb(0, 1, p1lf.x(), p1lf.y());
	this->keys->setLimb(0, 2, p1rh.x(), p1rh.y());
	this->keys->setLimb(0, 3, p1rf.x(), p1rf.y());
	
	// When debugging, this will track everything we're looking for at once,
	// including the wand and the box (just in case we need really-well-defined bodies)
	/*
	osg::Vec3 wand  = findObject("Wand", "Wand", 1);
	osg::Vec3 box   = findObject("Box",  "Box",  1);
	printf("\33[2K\r p1lh(%2.2f, %2.2f, %2.2f), p1lf(%2.2f, %2.2f, %2.2f), p1rh(%2.2f, %2.2f, %2.2f), p1rf(%2.2f, %2.2f, %2.2f), Wand(%2.2f, %2.2f, %2.2f), Box(%2.2f, %2.2f, %2.2f)",
		p1lh.x(),p1lh.y(),p1lh.z(), p1lf.x(),p1lf.y(),p1lf.z(), p1rh.x(),p1rh.y(),p1rh.z(), p1rf.x(),p1rf.y(),p1rf.z(), wand.x(),wand.y(),wand.z(), box.x(),box.y(),box.z());
	*/
	
	/* Commented out until we have physical objects to represent these * /
	// Get the location of all of p2's limbs
	osg::Vec3 p2lh  = findObject("P2_HandL", "???", 1);
	osg::Vec3 p2lf  = findObject("P2_FootL", "???", 1);
	osg::Vec3 p2rh  = findObject("P2_HandR", "???", 1);
	osg::Vec3 p2rf  = findObject("P2_FootR", "???", 1);
	/* End player 2 inputs */
	
	
	// Divide by 1000 to convert to meters, then divide by 2 to get the
	// approximate boundaries of the room, then shift a little to get the
	// correct edges.
	
	// TODO: Once we have player 2's objects, stop using p1's other hand!!
	// WARN: P1_HandL's definition in Tracker is a little spotty, it gets lost sometimes
	//       as such, we're using Wand instead.
	float p1x = findObject("Wand", "Wand", 0).x();
	float p2x = findObject("P1_HandR", "HandR",  1).x();
	
	this->keys->setTheta(0, p1x);
	this->keys->setTheta(1, p2x);
}

void ViconInputClient::run() {
	// Time since last frame (plus any leftover time not "consumed" from previous frames)
    // Used to keep the game loop ticking at a relatively constant rate
    struct timeval curTime;
    struct timeval targetTime;
    gettimeofday(&targetTime, 0);
    struct timeval tv;
    if(!this->view->isRealized()){
    	fflush(stdout);
		this->view->realize();
	}
	if(!this->view->isRealized()){
		printf("Could not realize viewer\n");
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
	while( !this->view->done() )
	{
		fflush(stdout);
		
		// Add 25ms to the time we SHOULD be at for the current frame
		advance(&targetTime, 25000);
		
		// Determine how far away from that target time we really are
		gettimeofday(&curTime, 0);
		timersub(&targetTime, &curTime, &tv);
		
		// Figure out how long we've been running grand total
		timersub(&curTime, &startTime, &totalTime);
		
		// Ask Vicon to update our keys object
		viconUpdateKeys();
		
		// DEBUG: Quit game after it's been running for 30sec
		/*
		printf("  Time on clock: %ld.%06ldsec\n", (long int)totalTime.tv_sec, (long int)totalTime.tv_usec);
		if(totalTime.tv_sec >= 30) {this->view->getOSGViewer()->setDone(true);printf("DONE!");}
		else {
			printf("  Waiting for %ld.%06ldsec before next frame\n", (long int)tv.tv_sec, (long int)tv.tv_usec);
			printf("  Target time %ld.%06ldsec, current time %ld.%06ldsec\n",
				(long int)targetTime.tv_sec, (long int)targetTime.tv_usec, (long int)curTime.tv_sec, (long int)curTime.tv_usec
			);
		}
		*/
		
		select(0, NULL, NULL, NULL, &tv); // If we're ahead of schedule, wait
		game->input(this->keys, 0.025);
		view->frame();
	}
	
}
