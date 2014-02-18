/**
Class for the Game object
*/

#include "Game.h"


Game::Game(int numplayers){
	if( numplayers == 1 )
		Player* p1;
	else if (numplayers == 2)
		Player* p1;
		Player* p2;
	else
}


Game::~Game(void){
}


/*
	Method that uses keyboard input with local running. 
	W,S,A,D to move an orb, H,J,K,L to switch between them
*/
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
void Game::motionTracking(std::vector<ViconSegment> * segment){
	
	vector3df temp[4] ={vector3df(segment[0]->getX()/100,segment[0]->getZ()/100,30),
				vector3df(segment[1]->getX()/100,segment[1]->getZ()/100,30),
				vector3df(segment[2]->getX()/100,segment[2]->getZ()/100,30),
				vector3df(segment[3]->getX()/100,segment[3]->getZ()/100,30)};

	p1->setPositions(temp);

}

/*
This method creates our clock object and displays it in the title bar
*/
void Game::createClock(IrrlichtDevice* device, ISceneManager* smgr){
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
void Game::updateClock(IrrlichtDevice* device){ 
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


