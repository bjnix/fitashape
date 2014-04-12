/**
Class for the Game object
*/
#include "fitashape/Game.h"


template<>
char * MapNode<Player>::getDataString(){
	std::vector<vector3df> dataPos = data->getPosition();

	float float_array[12];
	dataPos[0].getAs3Values( &( float_array[0] ) );
	dataPos[1].getAs3Values( &( float_array[3] ) );
	dataPos[2].getAs3Values( &( float_array[6] ) );
	dataPos[3].getAs3Values( &( float_array[9] ) );

    char * data_array = new char[dataLength];
    memcpy(data_array, float_array, dataLength);        
    return data_array;
}
template<>
void MapNode<Player>::setData(char * data_array){
	float float_array[12];
    memcpy(float_array, data_array, dataLength);
    std::vector<vector3df> dataPos;
    dataPos.push_back(vector3df(float_array[0],float_array[1],float_array[2]));
    dataPos.push_back(vector3df(float_array[3],float_array[4],float_array[5]));
    dataPos.push_back(vector3df(float_array[6],float_array[7],float_array[8]));
    dataPos.push_back(vector3df(float_array[9],float_array[10],float_array[11]));
    data->setPosition(dataPos);
}

char * RELAY_IP;
std::string hostname = "c07-0510-01.ad.mtu.edu";//"141.219.28.17:801";//was 141.219.28.107:801
ViconDataStreamSDK::CPP::Client MyClient;

double frustum_left,frustum_right,frustum_bottom,frustum_top;
int screen_width,screen_height;

DGR_framework * myDGR;

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

int viconInit()
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
void viconExit(void)
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

Game::Game(bool isLocal, char* relay_ip){
	myDGR = new DGR_framework(relay_ip);

	gameOver = true;
	zen = 50;
	zenBarSize = 50;
	timesUp = 10;
	score = 0;
	toExit = false;
	pause = true;
	local = isLocal;
	run();
}

Game::Game(
	bool isLocal, 
	char* f_left, char* f_right, char* f_bottom, char* f_top,   //frustum
	char* s_width, char* s_height)  							//dimentions
{
	myDGR = new DGR_framework();
	frustum_left = atof(f_left);
	frustum_right = atof(f_right);
	frustum_bottom = atof(f_bottom);
	frustum_top = atof(f_top);

	screen_width = atof(s_width);
	screen_height = atof(s_height);


	gameOver = true;
	zen = 50;
	zenBarSize = 50;
	timesUp = 5;
	score = 0;
	toExit = false;
	pause = true;
	local = isLocal;
	run();
}



Game::~Game(void){
	delete p1;
	delete myDGR;
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
	#ifdef DGR_MASTER
	device = createDevice(driverType,core::dimension2d<u32>(1440, 540), 16, false, false, false, &receiver);
	#else
	device = createDevice(driverType,core::dimension2d<u32>(11520,4320), 16, false, false, false, &receiver);
	//device = createDevice(driverType,core::dimension2d<u32>(5760,1080), 16, false, false, false, &receiver);
	#endif

	if (device == 0)
		return 1; // could not create selected device.
	
	//get the driver and the scene manager
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	
	std::cout << "made a scene manager at location:"<<&smgr << "\n"<< std::flush;

	zenBackgrounds = new ITexture*[6]();
	if(!local){
		zenBackgrounds[0] = driver->getTexture("../assets/Zen-lvl-1.png");
		zenBackgrounds[1] = driver->getTexture("../assets/Zen-lvl-2.png");
		zenBackgrounds[2] = driver->getTexture("../assets/Zen-lvl-3.png");
		zenBackgrounds[3] = driver->getTexture("../assets/Zen-lvl-4.png");
		zenBackgrounds[4] = driver->getTexture("../assets/Zen-lvl-5.png");
		zenBackgrounds[5] = driver->getTexture("../assets/Zen-lvl-6.png");
	}


	//ICameraSceneNode *myCamera;
	//irr::core::matrix4 MyMatrix;
	//MyMatrix.buildProjectionMatrixOrthoLH(16.0f,12.0f,-1.5f,32.5f);
	//myCamera = smgr->addCameraSceneNode(0,irr::core::vector3df(-14.0f,14.0f,-14.0f),irr::core::vector3df(0,0,0));
	//myCamera->setProjectionMatrix(MyMatrix);

	//create basic camera
	float x = 0;
	float y = 0;
	float z = 1.5;

	ICameraSceneNode *myCamera;
	irr::core::CMatrix4<float> MyMatrix;
	#ifdef DGR_MASTER
	myCamera = smgr->addCameraSceneNode();
	#else
	myCamera = smgr->addCameraSceneNode();
	//printf("*****view port 1: UL(%d,%d) and LR(%d,%d)\n", driver->getViewPort().UpperLeftCorner.X, driver->getViewPort().UpperLeftCorner.Y, driver->getViewPort().LowerRightCorner.X,driver->getViewPort().LowerRightCorner.Y);
	//myCamera = smgr->addCameraSceneNode(0, core::vector3df(25,0,0), core::vector3df(25,0,30));
	//int width = 11520;
	//int height = 4320;
	//driver->setViewPort(rect<s32>(-width/2,0,width,height/2));
	//printf("*****view port 2: UL(%d,%d) and LR(%d,%d)\n", driver->getViewPort().UpperLeftCorner.X, driver->getViewPort().UpperLeftCorner.Y, driver->getViewPort().LowerRightCorner.X,driver->getViewPort().LowerRightCorner.Y);

	//myCamera = smgr->addCameraSceneNode(/*0,vector3df(screen_width, screen_height, 0),vector3df(screen_width, screen_height, 30)*/);
	#endif
	//creates the clock.
	createClock();
	zenBar = driver->getTexture("../assets/Scroll.png");

	std::cout << "creating the player object \n"<< std::flush;
	//create player and draw the limbs
	p1 = new Player(driver,smgr);
	p1->drawLimbs();

	if(!local){
		//using the tracking system
		#ifdef DGR_MASTER//master
		std::cout << "calling viconInit() \n"<< std::flush;
		//get the initial setup for the player if using tracking system
		if(viconInit() != 0)
			{ gameOver = true;}
		atexit(viconExit);
		#else //slave
		#endif

		//sets up the player's body and stuff - Now done in the menu 
		//startLocation();	
		//printf("Done calibrating\n");
		p1->localInitPos();
		//then sets up the body, arms, and legs
		p1->initializePosition();
		myDGR->addNode<Player>("Player1",p1,sizeof(float)*12);


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

	p1->createBody();

	//reset the clock for the start of the game!
	myClock->setTime(0);
	p1->setTargetVisible(false, gameOver);
	
	if(!local){
		#ifdef DGR_MASTER
		background = driver->getTexture("../assets/Background(small).png");
		#else
		background = driver->getTexture("../assets/Background(Fitashape).png");
		#endif
	}

	while(device->run() && !toExit)
	{

		//printf("(x,y,z): (%f,%f,%f)\n",p1->LH.node->getPosition().X,p1->LH.node->getPosition().Y,p1->LH.node->getPosition().Z);

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
		drawObjects();
		//end the current session, asking if want to play again.
		
			

	}

	/*
	In the end, delete the Irrlicht device.
	*/
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
	
#ifdef DGR_MASTER
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
				//std::cout<<names[i]<<" IS occluded!"<< std::endl; 
				OccludedMarker = true;
				break;
			}
		}
	}

	p1->updateBody();
	p1->setPosition(temp);
#endif
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
	text = smgr->addTextSceneNode(device->getGUIEnvironment()->getFont("../assets/bigfont.png"),tmp,video::SColor(255,0,0,0),0,core::vector3df(0,18,35));
}

/*
void retryMenu - I think this is depreciated in favor of pauseMenu, and no longer is needed - Will most likely delete later
*/
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

/*
void pauseMenu - setup the pause menu and interactions, buttons, etc
*/
void Game::pauseMenu(){
	wchar_t tmp[100]; //buffer to set display text as
	p1->setMenu();
	switch(p1->pauseCollide()){ //figure out which selection is pressed
		case 1: //resume game if paused
			pause = false;
			p1->setTargetVisible(true, gameOver);
			myClock->start();
			break;
		case 2: //create new game
			p1->setMenuInvis();
			if(!local){
				swprintf(tmp, 100, L"Assume The Position!");
				text->setText(tmp);
				startLocation();
				printf("Done calibrating\n");
			}
			gameOver = false;
			zen = 50;
			zenBarSize = 50;
			timesUp = 5;
			score = 0;
			p1->setTargetVisible(true, gameOver);
			myClock->setTime(0);
			pause = false;
			p1->randomTargets();
			break;
		
		case 3://exit the game
			toExit = true;
			break;
		case 4://display text for resume
			swprintf(tmp, 100, L"Resume Game");
			text->setText(tmp);
			break;
		case 5://display text for new
			swprintf(tmp, 100, L"New Game");
			printf("new game shoudl be here\n");
			text->setText(tmp);
			break;
		case 6: // display text for exit
			swprintf(tmp, 100, L"Exit Game");
			text->setText(tmp);
			break;
		default://display text for menu help
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
	printf("time is up\n");
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

		//update background
		if(!local){
			if(zen <= 16){
				background = zenBackgrounds[0];
			}else if(zen >= 17 && zen <= 33){
				background = zenBackgrounds[1];
			}else if(zen >= 33 && zen <= 50){
				background = zenBackgrounds[2];
			}else if(zen >= 50 && zen <= 66){
				background = zenBackgrounds[3];
			}else if(zen >= 67 && zen <= 83){
				background = zenBackgrounds[4];
			}else if(zen >= 84){
				background = zenBackgrounds[5];
			}
		}


		if(zen > 100) // so zen cant get above 100%
			zen = 100;
		if(zen <= 0){ // if zen <= 0 player loses and game quits
			zen = 0;
			gameOver = true;
			pause = true;
			p1->setTargetVisible(false, gameOver);

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


/*
	Method to determine where and when the person is 
	standing when trying to get the initial locations.
	Takes in instance every second and checks to see if the
	play has stood still for the last three seconds
*/
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

	if(!local)
		background = driver->getTexture("../assets/Calibration.png");


	// keeps track of which group we are going to update, and when to move on
	bool one = false;
	bool two = false;
	bool three = false;
	int next = 0;
	int last = 0;

	myClock->start();// start a clock to keep track of time

	//loop to keep checking the locations of the nodes till they come close to stopping
	while(moving){
		myClock->tick();//move the clock
		//printf("Finding body... Stand still! at time %d\n", ((myClock->getTime() / 500) % 60) % 3);		

		//make stuff appear on screen
		drawObjects();

		//update the signal to the current time
		next = ((myClock->getTime() / 500) % 10) % 3;

		//call the motion tracking method to get up-to-date locaitons
		motionTracking();

		if(!one && last != next){ //check if we want to store this pos
			printf("CHECK 1\n");
			LHpos1 = p1->LH.node->getPosition();
			RHpos1 = p1->RH.node->getPosition();
			LFpos1 = p1->LF.node->getPosition();
			RFpos1 = p1->RF.node->getPosition();
			one = true;
			last = next;
		}

		//make stuff appear on screen
		drawObjects();
	
		//call the motion tracking method to get up-to-date locaitons
		motionTracking();

		if(!two && last != next){ //check if we want to store this pos
			printf("CHECK 2\n");
			LHpos2 = p1->LH.node->getPosition();
			RHpos2 = p1->RH.node->getPosition();
			LFpos2 = p1->LF.node->getPosition();
			RFpos2 = p1->RF.node->getPosition();
			two = true;
			last = next;
		}
		
		//make stuff appear on screen
		drawObjects();

		//call the motion tracking method to get up-to-date locaitons
		motionTracking();

		if(!three && last != next){ //check if we want to store this pos
			printf("CHECK 3\n");
			LHpos3 = p1->LH.node->getPosition();
			RHpos3 = p1->RH.node->getPosition();
			LFpos3 = p1->LF.node->getPosition();
			RFpos3 = p1->RF.node->getPosition();
			three = true;
			last = next;
		}

		//make stuff appear on screen
		drawObjects();
		
		if(one && two && three){
			one = false;
			two = false;
			three = false;
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
							std::cout<<"good position"<<std::endl;
						}
						else{std::cout<<"level out hands and feet"<<std::endl;}
					//}
			}
			else{printf("Stand still...\n");}
		}
	}
	//store this position for later use
	p1->initLoc[0] = LHpos3; //left hand
	p1->initLoc[1] = RHpos3; //right hand
	p1->initLoc[2] = LFpos3; //left foot
	p1->initLoc[3] = RFpos3; //right foot
	
	//calls the method that determines what the body looks like
	p1->initializePosition();

	//centers the camera on the players position	
	//p1->addCameraScene();

	p1->bodyScale();

	if(!local)
		background = zenBackgrounds[0];

	return;
}

/*
// an atexit() callback:
void exitCallback()
{
	viconExit();
	return;
}
*/

void Game::drawObjects(){
	
	driver->beginScene(true, true, video::SColor(255,113,113,133));
	
	int x1 = 520, y1 = 0, x2 = 920, y2 = 70;
	int color;
	
	//draw the background
	if(!local){
		#ifdef DGR_MASTER
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		#else
		driver->draw2DImage(background,position2d<s32>(0.0f,0.0f));
		#endif
	}
	
	
	

	//driver->enableMaterial2D();
	if(!gameOver){//zen bar stuff
		if(zenBarSize < zen - 1)
			zenBarSize++;
		else if(zenBarSize > zen)
			zenBarSize--;
		driver->draw2DImage(zenBar, rect<s32>(x1, y1, x2, y2), rect<s32>(0, 0, 1780, 300), NULL, NULL, true);
		color = 255 - (zenBarSize * 2.55);
		driver->draw2DRectangle(SColor(255,color,255 - color,0), rect<s32>(x1+37, y1+22, x1 +37 + (zenBarSize * 3.3), y2-27), NULL);
	}
	smgr->drawAll(); // draw the 3d scene
	driver->endScene();
}
