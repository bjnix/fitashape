/**
Class for the Game object
*/

#include "fitashape/Game.h"



std::string hostname = "c07-0510-01.ad.mtu.edu";//"141.219.28.17:801";//was 141.219.28.107:801
ViconDataStreamSDK::CPP::Client MyClient;


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

Game::Game(bool isLocal){
	gameOver = true;
	zen = 50;
	timesUp = 5;
	score = 0;
	toExit = false;
	pause = true;
	local = isLocal;
	run();
}

Game::~Game(void){
	delete p1;
}

/*
	Method that uses keyboard input with local running. 
	W,S,A,D to move an orb, H,J,K,L to switch between them
*/

int Game::run(){	

	// ask user for driver
	//video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
	if (driverType==video::EDT_COUNT)
		return 1;

	std::cout << "creating the event reciever for KB \n"<< std::flush;
	
	// create reciever and device
	//device = createDevice(driverType,core::dimension2d<u32>(1920*6, 1080*4), 16, false, false, false, &receiver);
	device = createDevice(driverType,core::dimension2d<u32>(1440, 540), 16, false, false, false, &receiver);
	if (device == 0)
		return 1; // could not create selected device.
	
	//get the driver and the scene manager
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	
	std::cout << "made a scene manager at location:"<<&smgr << "\n"<< std::flush;



	//ICameraSceneNode *myCamera;
	//irr::core::matrix4 MyMatrix;
	//MyMatrix.buildProjectionMatrixOrthoLH(16.0f,12.0f,-1.5f,32.5f);
	//myCamera = smgr->addCameraSceneNode(0,irr::core::vector3df(-14.0f,14.0f,-14.0f),irr::core::vector3df(0,0,0));
	//myCamera = smgr->addCameraSceneNode();
	//myCamera->setProjectionMatrix(MyMatrix);

	smgr->addCameraSceneNode();


	
	//creates the clock.
	createClock();

	std::cout << "creating the player object \n"<< std::flush;
	//create player and draw the limbs
	p1 = new Player(driver,smgr);
	p1->drawLimbs();

	if(!local){
		//using the tracking system
	
		std::cout << "calling viconInit() \n"<< std::flush;
		//get the initial setup for the player if using tracking system
		if(viconInit() != 0)
		{ gameOver = true;}


		//sets up the player's body and stuff - Now done in the menu 
		//startLocation();	
		//printf("Done calibrating\n");
		p1->localInitPos();
		//then sets up the body, arms, and legs
		p1->initializePosition();

		std::cout << "Just finished Method Calls \n"<< std::flush;
	}else{
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
	

	//reset the clock for the start of the game!
	myClock->setTime(0);
	p1->setTargetVisible(false, gameOver);
	
	ITexture* background = driver->getTexture("../assets/Background(small).png");

	while(device->run() && !toExit)
	{
		//move the orbs around
		if(local)
			moveKeyboard(receiver);
		else
			motionTracking();

		if(p1->jump() && !gameOver && !pause){
			printf("JUMPED\n");
			myClock->stop();
			pause = true;
			p1->setTargetVisible(false, gameOver);
		}
		//normal scoring while the game runs
		if(!pause){
			//update the clock and check for win/lose
			updateClock();
		}
		//menu for game overness
		else{
			pauseMenu();
		}

		//puts the stuff on the screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		smgr->drawAll(); // draw the 3d scene
		driver->endScene();
		//end the current session, asking if want to play again.
		
			

	}

	/*
	In the end, delete the Irrlicht device.
	*/
	viconExit();
	device->drop();
	return 0;
}

void Game::moveKeyboard(MyEventReceiver receiver){
	
	//check if the user want to switch nodes
	if(receiver.IsKeyDown(KEY_KEY_H))// left hand
		p1->setCurrentLH();
	else if(receiver.IsKeyDown(KEY_KEY_J))// right hand
		p1->setCurrentRH();
	else if(receiver.IsKeyDown(KEY_KEY_K))// left foot
		p1->setCurrentLF();
	else if(receiver.IsKeyDown(KEY_KEY_L))// right foot
		p1->setCurrentRF();

	
	/* Check if keys W, S, A or D are being held down, and move the
	sphere node around respectively. */
	core::vector3df nodePosition = p1->currentNode()->getPosition();
	
	if(receiver.IsKeyDown(KEY_KEY_W))
		nodePosition.Y += .5;
	else if(receiver.IsKeyDown(KEY_KEY_S))
		nodePosition.Y -= .5;
	
	if(receiver.IsKeyDown(KEY_KEY_A))
		nodePosition.X -= .5;
	else if(receiver.IsKeyDown(KEY_KEY_D))
		nodePosition.X += .5;

	p1->currentNode()->setPosition(nodePosition);
	
	p1->updateBody();

}


void Game::motionTracking(){
	vector3df temp[4];
	bool OccludedMarker = true;	
	while(OccludedMarker){
		OccludedMarker = false;
		while(MyClient.GetFrame().Result != Result::Success) 
		{
			sleep(1);
			std::cout << ".";
		}	
		std::cout<<std::flush;
		for(int i = 0; i < 4; i++)
		{
			ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation Output = 
				MyClient.GetSegmentGlobalTranslation(names[i],names[i]);
			//std::cout<<names[i]<<": "<<Output.Translation << std::endl;
			if(!Output.Occluded)
		    	{ 
				//std::cout<<names[i]<<" NOT occluded!"<< std::endl;
				/*std::cout<<names[i]<<": ("<<Output.Translation[0]<<", "
							  <<Output.Translation[1]<<", "
							  <<Output.Translation[2]<<") " 
							  <<Output.Occluded << std::endl;*/

				temp[i] = vector3df(Output.Translation[0]/100,Output.Translation[2]/100,30);		
			}
			else
			{ 
				//std::cout<<names[i]<<" occluded!"<< std::endl; 
				OccludedMarker = true;
				break;
			}
		}
	}
	p1->setPositions(temp);

	p1->updateBody();

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


void Game::retryMenu(){
	switch(p1->restartCollide()){
		//if yes is selected, reinitialize the variables
		case 1:
			gameOver = false;
			zen = 50;
			timesUp = 5;
			score = 0;
			p1->setTargetVisible(true, gameOver);
			myClock->setTime(0);
			break;
		//if no selected, set exit bool to be true
		case 2: 
			toExit = true;
			break;
	}
}

void Game::pauseMenu(){
	wchar_t tmp[100];
	p1->setMenu();
	switch(p1->pauseCollide()){
		case 1:
			pause = false;
			myClock->start();
			p1->setTargetVisible(true, gameOver);
			break;
		case 2:
			p1->setMenuInvis();
			if(!local){
				swprintf(tmp, 100, L"Assume The Position!");
				text->setText(tmp);
				startLocation();
				printf("Done calibrating\n");
			}
			gameOver = false;
			zen = 50;
			timesUp = 5;
			score = 0;
			p1->setTargetVisible(true, gameOver);
			myClock->setTime(0);
			pause = false;
			p1->randomTargets();
			break;
		
		case 3:
			toExit = true;
			break;
		case 4:
			swprintf(tmp, 100, L"Resume Game");
			text->setText(tmp);
			break;
		case 5:
			swprintf(tmp, 100, L"New Game");
			text->setText(tmp);
			break;
		case 6:
			swprintf(tmp, 100, L"Exit Game");
			text->setText(tmp);
			break;
		default:
			swprintf(tmp, 100, L"Hover Over With Left Hand To Choose Option");
			text->setText(tmp);
			break;
	}
}







/*
This method updates our clock in the title bar 
and also checks to see if we have won depending on the time
*/
void Game::updateClock(){ 
	
	wchar_t tmp[255] = {}; //string to display in the tital bar
	int seconds = (myClock->getTime() / 1000) % 60; //current time

	//display the clock with latest seconds
	swprintf(tmp, 255, L"CLOCK: Seconds: %d \t\t Zen: %d \t\t Score: %d",timesUp - seconds, zen, score);
 	
	//device->setWindowCaption(tmp);
	text->setText(tmp);
	
	//check to see if we have ran out of time
	if(seconds >= timesUp){ 
		//checks to see if all the nodes match their targets
		//bool win = p1->collideAll();
		/*int match = p1->collideNum();
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
			timesUp = 10 - (score/2);//*/
		
		//get number of orbs matched, then score based on that

		switch(p1->collideNum()){
			
			case 0:
				zen -= 25;
				break;
			case 1:
				score += 5;
				zen -= 15;
				break;
			case 2:
				score += 15;
				zen -= 5;
				break;
			case 3:
				score += 25;
				break;
			case 4:
				score += 50;
				zen += 10;
				break;
			default:
				break;
		}
		if(zen > 100) // so zen cant get above 100%
			zen = 100;
		if(zen <= 0){ // if zen <= 0 player loses and game quits
			zen = 0;
			gameOver = true;
			pause = true;
			p1->setTargetVisible(false, gameOver);
			//p1->setTargetVisible(false);
		} 
		p1->randomTargets();
		//reset the clock
		myClock->setTime(0);

	}
	else{//else we are not at the end time so just update any collisions
		p1->collideAll();
	}

	//display the clock with updated score
	//if(!gameOver)
		swprintf(tmp, 255, L"CLOCK: Seconds: %d \t\t Zen: %d \t\t Score: %d",timesUp - seconds,zen, score);
	/*else
		swprintf(tmp, 255, L)//*/
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

	ITexture* background = driver->getTexture("../assets/Calibration(small).png");


	//int temp = 0;// keeps track of which group we are going to update
	bool one = false;
	bool two = false;
	bool three = false;
	myClock->start();// start a clock to keep track of time

	//loop to keep checking the locations of the nodes till they come close to stopping
	while(moving){
		myClock->tick();//move the clock
		printf("Finding body... Stand still! at time %d\n", (myClock->getTime() / 500) % 60);		

		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		smgr->drawAll(); 
		driver->endScene();

		//call the motion tracking method to get up to date locaitons
		motionTracking();
		if(0 == ((myClock->getTime() / 500) % 60) % 3){ //check if we want to store this pos
			printf("CHECK 1\n");
			LHpos1 = p1->LH.node->getPosition();
			RHpos1 = p1->RH.node->getPosition();
			LFpos1 = p1->LF.node->getPosition();
			RFpos1 = p1->RF.node->getPosition();
			one = true;
		}

		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		smgr->drawAll();
		driver->endScene();
	
		//call the motion tracking method to get up to date locaitons
		motionTracking();
		if(1 == ((myClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 2\n");
			LHpos2 = p1->LH.node->getPosition();
			RHpos2 = p1->RH.node->getPosition();
			LFpos2 = p1->LF.node->getPosition();
			RFpos2 = p1->RF.node->getPosition();
			two = true;
		}
		
		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		smgr->drawAll();
		driver->endScene();

		//call the motion tracking method to get up to date locaitons
		motionTracking();
		if(2 == ((myClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 3\n");
			LHpos3 = p1->LH.node->getPosition();
			RHpos3 = p1->RH.node->getPosition();
			LFpos3 = p1->LF.node->getPosition();
			RFpos3 = p1->RF.node->getPosition();
			three = true;
		}

		//make stuff appear on screen
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		smgr->drawAll();
		driver->endScene();
		
		if(one && two && three){
			printf("Found all three\n");
			//check to see if the player is close to staying still
			double close = .5; //number to define how close is enough
			//double min = 1;
			if(LHpos1.getDistanceFrom(LHpos2) < close && LHpos2.getDistanceFrom(LHpos3) < close && LHpos3.getDistanceFrom(LHpos1) < close &&
				RHpos1.getDistanceFrom(RHpos2) < close && RHpos2.getDistanceFrom(RHpos3) < close && RHpos3.getDistanceFrom(RHpos1) < close &&
				LFpos1.getDistanceFrom(LFpos2) < close && LFpos2.getDistanceFrom(LFpos3) < close && LFpos3.getDistanceFrom(LFpos1) < close &&
				RFpos1.getDistanceFrom(RFpos2) < close && RFpos2.getDistanceFrom(RFpos3) < close && RFpos3.getDistanceFrom(RFpos1) < close){
					std::cout<<"stood still!"<<std::endl;
					//check to see if they look like they are in the right possition
					//TODO add more restrictions if necessary
					//check to see that they are an actual size, not just a dot
					//if(LHpos3.getDistanceFrom(LFpos3) > min && RHpos3.getDistanceFrom(RFpos3) > min && LHpos3.getDistanceFrom(LHpos3) > min){
						//std::cout<<"real size person!"<<std::endl;
						//makes sure the arms are at about the same hight and  that the arms are about the same length
						if((LHpos3.Y-RHpos3.Y > -.5 && LHpos3.Y-RHpos3.Y < .5) 
							&& ((LHpos3.Y-LFpos3.Y)-(RHpos3.Y-RFpos3.Y) > -.5 && (LHpos3.Y-LFpos3.Y)-(RHpos3.Y-RFpos3.Y) < .5)){
								moving = false; //they have stopped moving!
							std::cout<<"T position"<<std::endl;
						}
					//}
			}
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
int Game::viconInit()
{
    // Connect to a server
    std::cout << "Connecting to " << hostname.c_str() << " ..." << std::flush;
	int attemptConnectCount = 0;
	const int MAX_CONNECT_ATTEMPTS=2;
    while( !MyClient.IsConnected().Connected && attemptConnectCount < MAX_CONNECT_ATTEMPTS)
    {
		attemptConnectCount++;
		bool ok = false;
		ok =( MyClient.Connect( hostname ).Result == Result::Success );
		if(!ok)
			std::cout << "Warning - connect failed..." << std::endl;
		std::cout << ".";
		sleep(1);
    }
	if(attemptConnectCount == MAX_CONNECT_ATTEMPTS)
	{
		printf("Giving up making connection to Vicon system\n");
		return 1;
	}
    std::cout << std::endl;
/* TODO: bjnix at mtu dot edu | 11.13.2013
	add enumeration for input, so as to let user input what types of data to enable
*/
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
	return 0;
}
void Game::viconExit()
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
/*
// an atexit() callback:
void exitCallback()
{
	viconExit();
	return;
}
*/
