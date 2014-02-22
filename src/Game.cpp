/**
Class for the Game object
*/

#include "fitashape/Game.h"


std::string HostName = "141.219.28.17:801";//was 141.219.28.107:801

template<typename T, size_t N>
T * end(T (&ra)[N]) {
    return ra + N;
}
const char *nameList[] = {
	"HandL",
	"HandR",
	"FootL",
	"FootR"
	};

std::vector<std::string> names(nameList,end(nameList));

Game::Game(bool local){
	run(local);
}

Game::~Game(void){}

/*
	Method that uses keyboard input with local running. 
	W,S,A,D to move an orb, H,J,K,L to switch between them
*/

int Game::run(bool local){	

	// ask user for driver
	//video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
	if (driverType==video::EDT_COUNT)
		return 1;

	std::cout << "creating the event reciever for KB \n"<< std::flush;
	
	// create reciever and device
	device = createDevice(driverType,
			core::dimension2d<u32>(1280, 1024), 16, false, false, false, &receiver);
	if (device == 0)
		return 1; // could not create selected device.
	
	//get the driver and the scene manager
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	
	std::cout << "made a scene manager at location:"<<&smgr << "\n"<< std::flush;

	//adds a camera scene node 
	smgr->addCameraSceneNode();

	//creates the clock.
	createClock();

	std::cout << "creating the player object \n"<< std::flush;
	//create player and draw the limbs
	p1 = new Player(driver,smgr);
	p1->drawLimbs();

	if(!local)
	{
	//using the tracking system
	
	std::cout << "calling viconInit() \n"<< std::flush;
	//get the initial setup for the player if using tracking system
	vClient = new ViconInputClient(&HostName,&names,&names);

	printf("GOING IN!!!\n");
	//sets up the player's body and stuff
	startLocation();	
	printf("Done calibrating\n");
	std::cout << "Just finished Method Calls \n"<< std::flush;
	}
	else{
		//manually set the initial position of the limbs
		p1->localInitPos();
		//then sets up the body, arms, and legs
		p1->initializePosition();
	}

	std::cout << "about to draw targets \n"<< std::flush;

	//set up the target
	p1->drawTargets();
		

	std::cout << "calling randomTargets\n"<< std::flush;

	//gets a new target for the player to play with
	p1->randomTargets();
	
	std::cout << "calling setCurrent\n"<< std::flush;
	
	//needed for keyboard input
	p1->setCurrentLH();
	
	std::cout << "just setCurrent\n"<< std::flush;

	
	//centers the camera on the players position	
	p1->addCameraScene();
	

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
			motionTracking();

		//update the clock and check for win/lose
		updateClock();

		//puts the stuff on the screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll(); // draw the 3d scene
		driver->endScene();

	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
}

void Game::moveKeyboard(MyEventReceiver receiver, const f32 frameDeltaTime ){
	
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
void Game::motionTracking(){

	std::vector<ViconSegment> segment = vClient->GetRigidBodies();
	vector3df temp[4] ={vector3df(segment[0].getX()/100,segment[0].getZ()/100,30),
				vector3df(segment[1].getX()/100,segment[1].getZ()/100,30),
				vector3df(segment[2].getX()/100,segment[2].getZ()/100,30),
				vector3df(segment[3].getX()/100,segment[3].getZ()/100,30)};

	p1->setPositions(temp);

}

/*
This method creates our clock object and displays it in the title bar
*/
void Game::createClock(){
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
void Game::updateClock(){ 
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

void Game::startLocation(){

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
		motionTracking();
		if(temp != 0 && 0 == ((myClock->getTime() / 500) % 60) % 3){ //check if we want to store this pos
			printf("CHECK 1\n");
			LHpos1 = p1->LH.node->getPosition();
			RHpos1 = p1->RH.node->getPosition();
			LFpos1 = p1->LF.node->getPosition();
			RFpos1 = p1->RF.node->getPosition();
			temp = 0;
		}

		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll();
		driver->endScene();
	
		//call the motion tracking method to get up to date locaitons
		motionTracking();
		if(temp != 1 && 1 == ((myClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 2\n");
			LHpos2 = p1->LH.node->getPosition();
			RHpos2 = p1->RH.node->getPosition();
			LFpos2 = p1->LF.node->getPosition();
			RFpos2 = p1->RF.node->getPosition();
			temp = 1;
		}
		
		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		smgr->drawAll();
		driver->endScene();

		//call the motion tracking method to get up to date locaitons
		motionTracking();
		if(temp != 2 && 2 == ((myClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 3\n");
			LHpos3 = p1->LH.node->getPosition();
			RHpos3 = p1->RH.node->getPosition();
			LFpos3 = p1->LF.node->getPosition();
			RFpos3 = p1->RF.node->getPosition();
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
	p1->initLoc[0] = LHpos3; //left hand
	p1->initLoc[1] = RHpos3; //right hand
	p1->initLoc[2] = LFpos3; //left foot
	p1->initLoc[3] = RFpos3; //right foot
	
	//calls the method that determines what the body looks like
	p1->initializePosition();
	return;
}
