/**
Class for the player object
**/

#include "fitashape/Player.h"



Player::Player(IrrlichtDevice* d, scene::ISceneManager* s, Game * g){
	playerDevice = d;
	playerSmgr = s;
	playerClock = playerDevice->getTimer();
	playerDriver = playerDevice->getVideoDriver();
	fit_Game = g;
}


Player::~Player(void){
}


/*
get the current node
*/
scene::ISceneNode * Player::currentNode(){
	return current.node;
}

/*
set the current node
*/
void Player::setCurrent(CircleNode& node){
	current = node;
}

/*
This method will detect where the persons body is and the relative shape of the arms and legs.
uses the first 4 locations stored in initLoc[] and sets the last 5
*/
void Player::initializePosition(){
	vector3df LShoulder;
	vector3df RShoulder;
	vector3df LHip;
	vector3df RHip;
	vector3df centerBody;

	ground = (initLoc[3].Y + initLoc[2].Y) / 2; //sets ground to how the average height of the feet are

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
	Method to determine where and when the person is 
	standing when trying to get the initial locations.
	Takes in instance every second and checks to see if the
	play has stood still for the last three seconds
*/
void Player::startLocation(ViconInputClient * vClient, scene::ISceneManager* playerSmgr){

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
	playerClock->start();// start a clock to keep track of time

	//loop to keep checking the locations of the nodes till they come close to stopping
	while(moving){
		playerClock->tick();//move the clock
		printf("Finding body... Stand still!\n");		

		//make stuff appear on screen
		playerDriver->beginScene(true, true, video::SColor(255,113,113,133));
		playerSmgr->drawAll(); 
		playerDriver->endScene();

		//call the motion tracking method to get up to date locaitons
		fit_Game->motionTracking(vClient->GetSolidBodies());
		if(temp != 0 && 0 == ((playerClock->getTime() / 500) % 60) % 3){ //check if we want to store this pos
			printf("CHECK 1\n");
			LHpos1 = LH.node->getPosition();
			RHpos1 = RH.node->getPosition();
			LFpos1 = LF.node->getPosition();
			RFpos1 = RF.node->getPosition();
			temp = 0;
		}

		//make stuff appear on screen
		playerDriver->beginScene(true, true, video::SColor(255,113,113,133));
		playerSmgr->drawAll();
		playerDriver->endScene();
	
		//call the motion tracking method to get up to date locaitons
		fit_Game->motionTracking(vClient->GetSolidBodies());
		if(temp != 1 && 1 == ((playerClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 2\n");
			LHpos2 = LH.node->getPosition();
			RHpos2 = RH.node->getPosition();
			LFpos2 = LF.node->getPosition();
			RFpos2 = RF.node->getPosition();
			temp = 1;
		}
		
		//make stuff appear on screen
		playerDriver->beginScene(true, true, video::SColor(255,113,113,133));
		playerSmgr->drawAll();
		playerDriver->endScene();

		//call the motion tracking method to get up to date locaitons
		fit_Game->motionTracking(vClient->GetSolidBodies());
		if(temp != 2 && 2 == ((playerClock->getTime() / 500) % 60) % 3){//check if we want to store this pos
			printf("CHECK 3\n");
			LHpos3 = LH.node->getPosition();
			RHpos3 = RH.node->getPosition();
			LFpos3 = LF.node->getPosition();
			RFpos3 = RF.node->getPosition();
			temp = 2;
		}

		//make stuff appear on screen
		playerDriver->beginScene(true, true, video::SColor(255,113,113,133));
		playerSmgr->drawAll();
		playerDriver->endScene();
		
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
	initializePosition();
	return;
}


/*
This method builds a random shape for the player based off thier initLoc[]
*/
void Player::randomTargets(){
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

	//figure out where the feet should go
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
	LShoulder = vector3df(LShoulder.X,LShoulder.Y + shift, LShoulder.Z);
	RShoulder = vector3df(RShoulder.X,RShoulder.Y + shift, RShoulder.Z);
	LHip = vector3df(LHip.X,LHip.Y + shift, LHip.Z);
	RHip = vector3df(RHip.X,RHip.Y + shift, RHip.Z);
	centerBody = vector3df(centerBody.X,centerBody.Y + shift, centerBody.Z);
}

/*
method to set up the targets for each limb
*/
void Player::drawTargets(){
	LHTarget = playerSmgr->addSphereSceneNode(1);
	if (LHTarget){
		LH.target = LHTarget;
		LHTarget->setPosition(LH.node->getPosition());
		LHTarget->setMaterialTexture(0, playerDriver->getTexture("../assets/fire.bmp"));
		LHTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		LHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RHTarget = playerSmgr->addSphereSceneNode(1);
	if (RHTarget){
		RH.target = RHTarget;
		RHTarget->setPosition(RH.node->getPosition());
		RHTarget->setMaterialTexture(0, playerDriver->getTexture("../assets/lightFalloff.png"));
		RHTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		RHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	LFTarget = playerSmgr->addSphereSceneNode(1);
	if (LFTarget){
		LF.target = LFTarget;
		LFTarget->setPosition(LF.node->getPosition());
		LFTarget->setMaterialTexture(0, playerDriver->getTexture("../assets/particlegreen.jpg"));
		LFTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		LFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RFTarget = playerSmgr->addSphereSceneNode(1);
	if (RFTarget){		
		RF.target = RFTarget;
		RFTarget->setPosition(RF.node->getPosition());
		RFTarget->setMaterialTexture(0, playerDriver->getTexture("../assets/portal7.bmp"));
		RFTarget->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		RFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}
/*
method to draw the limb orbs
*/
void Player::drawLimbs(){

	LH.node = playerSmgr->addSphereSceneNode(1);
	if (LH.node){
		LH.nextNode = &RH;
		LH.node->setPosition(core::vector3df(-13,10,30));
		LH.node->setMaterialTexture(0, playerDriver->getTexture("../assets/fire.bmp"));
		LH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}


	RH.node = playerSmgr->addSphereSceneNode(1);
	if (RH.node){
		RH.nextNode = &LF;
		RH.node->setPosition(core::vector3df(13,10,30));
		RH.node->setMaterialTexture(0, playerDriver->getTexture("../assets/lightFalloff.png"));
		RH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	LF.node = playerSmgr->addSphereSceneNode(1);
	if (LF.node){
		LF.nextNode = &RF;
		LF.node->setPosition(core::vector3df(-3,-7,30));
		LF.node->setMaterialTexture(0, playerDriver->getTexture("../assets/particlegreen.jpg"));
		LF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RF.node = playerSmgr->addSphereSceneNode(1);
	if (RF.node){
		RF.nextNode = &LH;
		RF.node->setPosition(vector3df(3,-7,30));
		RF.node->setMaterialTexture(0, playerDriver->getTexture("../assets/portal7.bmp"));
		RF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}

/*
method to determin if the passed node is close to its targert
*/
bool Player::collide (CircleNode node){
	vector3df nodeLocation = node.node->getPosition();
	vector3df targetLocation = node.target->getPosition();
	double collideDist = 2; // determins how close the orbs have to be. easy to change
	
	if(nodeLocation.getDistanceFrom(targetLocation) > collideDist){
		node.target->setMaterialTexture(0, node.node->getMaterial(0).getTexture(0));
		node.target->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);		
		return false;
	}
	node.target->setMaterialTexture(0, playerDriver->getTexture("../assets/particlered.bmp"));
	node.target->setMaterialType(video::EMT_SOLID);	
	return true;
}

/*
method to update the position of the limbs. pass a vector of vector3df 
with left hand, right hand, left foot, right foot.
*/
void Player::setPosition(std::vector<vector3df> vec){
	LH.node->setPosition(vec[0]);
	RH.node->setPosition(vec[1]);
	LF.node->setPosition(vec[2]);
	RF.node->setPosition(vec[3]);
}

/*
method to update/check if all the orbs are touching their targets
*/
bool Player::collideAll(){

	bool result = collide(LH);
	result = collide(RH) && result ; 
	result = collide(LF) && result ; 
	result = collide(RF) && result ; 

	return result;
}

/*
method to set the current node to LH
*/
void Player::setCurrentLH(){
	current = LH;
}

/*
method to set the current node to RH
*/
void Player::setCurrentRH(){
	current = RH;
}

/*
method to set the current node to LF
*/
void Player::setCurrentLF(){
	current = LF;
}

/*
method to set the current node to RF
*/
void Player::setCurrentRF(){
	current = RF;
}

/*
method to set the the positions of all the nodes. Pass an array of the positions with 
0 = LH, 1 = RH, 2 = LF, 3 = RF
*/
void Player::setPositions(vector3df pos[4]){
	LH.node->setPosition(pos[0]);
	RH.node->setPosition(pos[1]);
	LF.node->setPosition(pos[2]);
	RF.node->setPosition(pos[3]);
}

/*
method to store the initial positions into the array for local use
*/
void Player::localInitPos(){
	initLoc[0]=LH.node->getPosition();
	initLoc[1]=RH.node->getPosition();
	initLoc[2]=LF.node->getPosition();
	initLoc[3]=RF.node->getPosition();
}

/*
method to add a camera scene node that is centered on the player
*/
void Player::addCameraScene(){
	playerSmgr->addCameraSceneNode(0, core::vector3df(initLoc[8].X,initLoc[8].Y+5,0), core::vector3df(initLoc[8].X,initLoc[8].Y+5,initLoc[8].Z));
}
