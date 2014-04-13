/**
Class for the player object
**/

#include "fitashape/Player.h"
Player::Player(IVideoDriver * d, ISceneManager * s){
	driver = d;
	smgr = s;
	ground = 0;
}
Player::~Player(void){}
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

	ground = mid(initLoc[2].Y,initLoc[3].Y); //sets ground to how the average height of the feet are

	LShoulder = core::vector3df(initLoc[2].X,initLoc[0].Y,initLoc[0].Z);//(leftfoot.x,lefthand.y,lefthand.x)
	RShoulder = core::vector3df(initLoc[3].X,initLoc[1].Y,initLoc[1].Z);//(rightfoot.x,righthand.y,righthand.x)
	
	LArm = initLoc[0].getDistanceFrom(LShoulder);//from left hand to left shoulder
	RArm = initLoc[1].getDistanceFrom(RShoulder);//from right hand to right shoulder

	//FUN FACT: your hips are about your arms's length below your shoulders
	LHip = core::vector3df(LShoulder.X,(LShoulder.Y - LArm),LShoulder.Z);
	RHip = core::vector3df(RShoulder.X,(RShoulder.Y - RArm),RShoulder.Z);

	LLeg = initLoc[2].getDistanceFrom(LHip);//left foot to left hip
	RLeg = initLoc[3].getDistanceFrom(RHip);//right foot to right hip

	centerBody = core::vector3df(mid(LHip.X,RHip.X),mid(LHip.Y,LShoulder.Y),LHip.Z);
	
	initLoc[4] = LShoulder;
	initLoc[5] = RShoulder;
	initLoc[6] = LHip;
	initLoc[7] = RHip;
	initLoc[8] = centerBody;
	

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
		RFTarget.setPosition(temp);

		//put the left foot somewhere
		temp = vector3df(LHip.X,LHip.Y - LLeg,LHip.Z);
		temp.rotateXYBy(spin,LHip);
		LFTarget.setPosition(temp);

		//get the amout to move up
		shift = ground - RFTarget.getPosition().Y;
	}else{//left foot down!
		spin = (f32) (rand() % 50 - 25);
		temp = vector3df(LHip.X,LHip.Y - LLeg,LHip.Z);
		temp.rotateXYBy(spin,LHip);
		LFTarget.setPosition(temp);

		//put the right foot somewhere
		spin =  90 - 45 + bodySpin;
		temp = vector3df(RHip.X,RHip.Y - RLeg,RHip.Z);
		temp.rotateXYBy(spin,RHip);
		RFTarget.setPosition(temp);

		//get the amout to move up
		shift = ground - LFTarget.getPosition().Y;
	}

	//move the arms around!
	//left hand first
	spin = rand() % 160 - 80 + bodySpin;
	temp = vector3df(LShoulder.X - LArm,LShoulder.Y ,LShoulder.Z);
	temp.rotateXYBy(spin,LShoulder);
	LHTarget.setPosition(temp);
	//right hand now
	spin = rand() % 160 - 80 + bodySpin;
	temp = vector3df(RShoulder.X + RArm,RShoulder.Y ,RShoulder.Z);
	temp.rotateXYBy(spin,RShoulder);
	RHTarget.setPosition(temp);

	//shift everything to line up with the ground
	LHTarget.setPosition(vector3df(LHTarget.getPosition().X,LHTarget.getPosition().Y + shift,LHTarget.getPosition().Z));
	RHTarget.setPosition(vector3df(RHTarget.getPosition().X,RHTarget.getPosition().Y + shift,RHTarget.getPosition().Z));
	LFTarget.setPosition(vector3df(LFTarget.getPosition().X,LFTarget.getPosition().Y + shift,LFTarget.getPosition().Z));
	RFTarget.setPosition(vector3df(RFTarget.getPosition().X,RFTarget.getPosition().Y + shift,RFTarget.getPosition().Z));
	//LShoulder = vector3df(LShoulder.X,LShoulder.Y + shift, LShoulder.Z);
	//RShoulder = vector3df(RShoulder.X,RShoulder.Y + shift, RShoulder.Z);
	//LHip = vector3df(LHip.X,LHip.Y + shift, LHip.Z);
	//RHip = vector3df(RHip.X,RHip.Y + shift, RHip.Z);
	//centerBody = vector3df(centerBody.X,centerBody.Y + shift, centerBody.Z);
}

/*
method to set up the targets for each limb
*/
void Player::drawTargets(){
	LHTarget.init(smgr, 1);
	
	LHTarget.setTarget(&LH);
	LHTarget.node->setPosition(LH.node->getPosition()); // set it's position to a temp spot
	LHTarget.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	LHTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	LHTarget.node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}

	//target for right hand
	//RHTarget = smgr->addSphereSceneNode(1);
	RHTarget.init(smgr, 1);
	//if (RHTarget){
		//RH.target = RHTarget;
	RHTarget.setTarget(&RH);
	RHTarget.node->setPosition(RH.node->getPosition());
	RHTarget.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
	RHTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	RHTarget.node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}

	//target for left foot
	//LFTarget = smgr->addSphereSceneNode(1);
	LFTarget.init(smgr, 1);
	//if (LFTarget){
		//LF.target = LFTarget;
	LFTarget.setTarget(&LF);
	LFTarget.node->setPosition(LF.node->getPosition());
	LFTarget.node->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
	LFTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	LFTarget.node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}

	//target for right foot
	//RFTarget = smgr->addSphereSceneNode(1);
	RFTarget.init(smgr, 1);
	//if (RFTarget){		
		//RF.target = RFTarget;
	RFTarget.setTarget(&RF);
	RFTarget.node->setPosition(RF.node->getPosition());
	RFTarget.node->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
	RFTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	RFTarget.node->setMaterialFlag(video::EMF_LIGHTING, false);

	RestartYes.init(smgr, 1);
	RestartYes.setTarget(&LH);
	RestartYes.node->setPosition(core::vector3df(-20, 10, 30));
	RestartYes.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	RestartYes.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	RestartYes.node->setMaterialFlag(video::EMF_LIGHTING, false);
	RestartYes.node->setVisible(false);

	RestartNo.init(smgr, 1);
	RestartNo.setTarget(&RH);
	RestartNo.node->setPosition(core::vector3df(20,10,30));
	RestartNo.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
	RestartNo.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	RestartNo.node->setMaterialFlag(video::EMF_LIGHTING, false);
	RestartNo.node->setVisible(false);

	NewGame.init(smgr, 1);
	NewGame.setTarget(&LH);
	NewGame.node->setPosition(core::vector3df(-10, 10, 30));
	NewGame.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	NewGame.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	NewGame.node->setMaterialFlag(video::EMF_LIGHTING, false);
	NewGame.node->setVisible(false);

	ResumeGame.init(smgr, 1);
	ResumeGame.setTarget(&LH);
	ResumeGame.node->setPosition(core::vector3df(-10, 13, 30));
	ResumeGame.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	ResumeGame.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	ResumeGame.node->setMaterialFlag(video::EMF_LIGHTING, false);
	ResumeGame.node->setVisible(false);

	ExitGame.init(smgr, 1);
	ExitGame.setTarget(&LH);
	ExitGame.node->setPosition(core::vector3df(-10, 8, 30));
	ExitGame.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	ExitGame.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	ExitGame.node->setMaterialFlag(video::EMF_LIGHTING, false);
	ExitGame.node->setVisible(false);

	Select.init(smgr, 1);
	Select.setTarget(&RH);
	Select.node->setPosition(core::vector3df(0,10,30));
	Select.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
	Select.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	Select.node->setMaterialFlag(video::EMF_LIGHTING, false);
	Select.node->setVisible(false);
}
/*
method to draw the limb orbs
*/
void Player::drawLimbs(){

	LH.init(smgr, 1);
	//if (LH.node){
		LH.node->setPosition(core::vector3df(-13,10,30)); //set its position
		LH.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp")); //set the texture
		LH.node->setMaterialFlag(video::EMF_LIGHTING, false); //turn of the emf lighting flag
	//}

	//right hand
	//RH.node = smgr->addSphereSceneNode(1);
	RH.init(smgr, 1);
	//if (RH.node){
		RH.node->setPosition(core::vector3df(13,10,30));
		RH.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
		RH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}

	//left foot
	//LF.node = smgr->addSphereSceneNode(1);
	LF.init(smgr, 1);
	//if (LF.node){
		LF.node->setPosition(core::vector3df(-3,-7,30));
		LF.node->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
		LF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}

	//right foot
	//RF.node = smgr->addSphereSceneNode(1);
	RF.init(smgr, 1);
	//if (RF.node){
		RF.node->setPosition(vector3df(3,-7,30));
		RF.node->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
		RF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}
}

/*
method to determin if the passed node is close to its targert
*/
bool Player::collide (CircleNode node){
	vector3df nodeLocation = node.node->getPosition();
	vector3df targetLocation = node.target()->node->getPosition();
	double collideDist = 2; // determins how close the orbs have to be. easy to change
	
	//check to see of the distance between the two nodes is less than the required distance
	if(nodeLocation.getDistanceFrom(targetLocation) > collideDist){
		//did not collide so make sure it is the normal texture and transparent
		node.node->setMaterialTexture(0, node.target()->node->getMaterial(0).getTexture(0));
		node.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		return false;
	}
	node.node->setMaterialTexture(0, driver->getTexture("../assets/particlewhite.bmp"));
	node.node->setMaterialType(video::EMT_SOLID);	
	//did collide, change texture to a different one and make it look solid
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
void Player::setPosition(vector3df pos[4]){
	LH.setPosition(pos[0]);
	RH.setPosition(pos[1]);
	LF.setPosition(pos[2]);
	RF.setPosition(pos[3]);
}
/*
Medthod to get the position of all if the limbs. returns a vector of vector3df's
with left hand, right hand, left foot, right foot.
*/
std::vector<vector3df> Player::getPosition(){
	std::vector<vector3df> position;
		position.push_back(LH.node->getPosition());
		position.push_back(RH.node->getPosition());
		position.push_back(LF.node->getPosition());
		position.push_back(RF.node->getPosition());
	return position;
}

/*
method to update/check if all the orbs are touching their targets
*/
bool Player::collideAll(){
	bool result = collide(LHTarget);
	result = collide(RHTarget) && result ;
	result = collide(LFTarget) && result ; 
	result = collide(RFTarget) && result ; 
	
	return result;
}

/*
returns the number of collided nodes
*/
int Player::collideNum(){
	return collide(LHTarget) + collide(RHTarget) + collide(LFTarget) + collide(RFTarget);
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
method to store the initial positions into the array for local use
*/
void Player::localInitPos(){
	initLoc[0]=LH.getPosition();
	initLoc[1]=RH.getPosition();
	initLoc[2]=LF.getPosition();
	initLoc[3]=RF.getPosition();
}

/*
method to add a camera scene node that is centered on the player
*/
void Player::addCameraScene(){

	//ICameraSceneNode *myCamera;
	//irr::core::matrix4 MyMatrix;
	//MyMatrix.buildProjectionMatrixOrthoLH(16.0f,12.0f,-1.5f,32.5f);
	//myCamera = smgr->addCameraSceneNode(0, core::vector3df(initLoc[8].X,initLoc[8].Y+5,0), core::vector3df(initLoc[8].X,initLoc[8].Y+5,initLoc[8].Z));
	//myCamera = smgr->addCameraSceneNode(0,irr::core::vector3df(-14.0f,14.0f,-14.0f),irr::core::vector3df(0,0,0));
	//myCamera->setProjectionMatrix(MyMatrix);


	printf("initLoc[8] (%f,%f,%f)",initLoc[8].X,initLoc[8].Y,initLoc[8].Z);
	smgr->addCameraSceneNode(0, core::vector3df(initLoc[8].X,initLoc[8].Y+5,0), core::vector3df(initLoc[8].X,initLoc[8].Y+5,initLoc[8].Z));
}

/*
void setTargetVisible - detirmine whether the game targets or menu targets are visible.
bool visibility - true for targets vis, menu invis. false for inverse
bool resume - whether or not a game is currently running, allowing for the resume game option to be shown.
*/
void Player::setTargetVisible(bool visibility, bool resume){
	RFTarget.node->setVisible(visibility);
	LFTarget.node->setVisible(visibility);
	RHTarget.node->setVisible(visibility);
	LHTarget.node->setVisible(visibility);
	NewGame.node->setVisible(!visibility);
	bNewGame->setVisible(!visibility);
	ResumeGame.node->setVisible(!visibility && !resume);
	bResumeGame->setVisible(!visibility && !resume);
	ExitGame.node->setVisible(!visibility);
	bExitGame->setVisible(!visibility);
	Select.node->setVisible(!visibility);
	bSelect->setVisible(!visibility);

	body->setVisible(visibility);
}

/*
void setMenuInvis - make the menu nodes all invisible
*/
void Player::setMenuInvis(){
	NewGame.node->setVisible(false);
	bNewGame->setVisible(false);
	ResumeGame.node->setVisible(false);
	bResumeGame->setVisible(false);
	ExitGame.node->setVisible(false);
	bExitGame->setVisible(false);
	Select.node->setVisible(false);
	bSelect->setVisible(false);
}

//figure out which resart button is pressed
int Player::restartCollide(){
	if(collide(RestartYes))
		return 1;
	else if(collide(RestartNo))
		return 2;
	else 
		return 0;
}

/*
int pauseCollide - return which combination of buttons is pressed for the menu
returns:
	1 - resume and select button pressed
	2 - newgame and select pressed
	3 - exit game and select pressed
	4 - only resume selected
	5 - only newgame selected
	6 - only exit selected
*/
int Player::pauseCollide(){
	int ret = 0;
	if(ResumeGame.node->isVisible() && collide(ResumeGame)){ // only count resume if visible
		if(collide(Select))
			ret = 1;
		else
			ret = 4;
	} 
		
	if(collide(NewGame)){
		if(collide(Select))
			ret = 2;
		else
			ret = 5;
	}
	if(collide (ExitGame)){
		if(collide(Select))
			ret = 3;
		else
			ret = 6;
	}
	return ret;
}

bool Player::jump(){
	//printf("mid: %f, left: %f, right: %f\n", ground, LF.getPosition().Y, RF.getPosition().Y);
	if(LF.getPosition().Y > ground + .5 && RF.getPosition().Y > ground + .5)
		return true;

	return false;
}

/*
void setmenu - sets the menu buttons to relativly follow the left foot node.
*/
void Player::setMenu(){
	f32 px, py;
	px = LF.getPosition().X;
	py = LF.getPosition().Y;
	Select.setPosition(vector3df(px + 3, py + 12, 30));
	bSelect->setPosition(vector3df(px + 3, py + 11.85, 29));
	NewGame.setPosition(vector3df(px -5, py + 12, 30));
	bNewGame->setPosition(vector3df(px -5, py + 11.85, 29));
	ResumeGame.setPosition(vector3df(px - 5, py + 15, 30));
	bResumeGame->setPosition(vector3df(px -5, py + 14.85, 29));
	ExitGame.setPosition(vector3df(px - 5, py + 9, 30));
	bExitGame->setPosition(vector3df(px -5, py + 8.85, 29));
}

/*
  update the location and rotation of the body object to make it look like it is accurate
*/
void Player::updateBody(){
	//make sure the body exists already
	if(body){
		//set the position to the middle of the feet
		body->setPosition(vector3df(mid(RF.getPosition().X,LF.getPosition().X), body->getPosition().Y, body->getPosition().Z));

		//set the rotation based off of the hands
		if(LH.getPosition().X < (body->getPosition().X - abs(initLoc[0].X - initLoc[8].X)))
			body->setRotation(vector3df(0,0, 3 * abs((abs(initLoc[0].X - initLoc[8].X) - body->getPosition().X) + LH.getPosition().X)));

		if(RH.getPosition().X > (body->getPosition().X + abs(initLoc[1].X - initLoc[8].X)))
			body->setRotation(vector3df(0,0, -3 * abs((abs(initLoc[1].X - initLoc[8].X) + body->getPosition().X) - RH.getPosition().X)));
	}
}

/*
   helper to find the middle of 2 numbers
*/
double Player::mid(double a, double b){
	return (a/2 + b/2);
}

/*
   helper to find the absolute value of a number
*/
double Player::abs(double number){
	if(number < 0)
		return number * -1;
	return number;
}

void Player::createBody(){
	//add the body
	IMesh* mesh = smgr->getMesh("../assets/Character.3ds");
	if (!mesh)
	{
		printf("mesh did not work\n");
		exit(-1);
	}
	printf("mesh worked\n");
	body = smgr->addMeshSceneNode( mesh,0,0,vector3df(initLoc[8].X,initLoc[8].Y-2,initLoc[8].Z+10 ));
	if (body)
	{
		body->setMaterialFlag(EMF_LIGHTING, false);
		printf("body worked\n");	
	}
	else{
		printf("body did not work\n");
		exit(-1);
	}
}

void Player::createButtons(){
	f32 px, py;
	px = LF.getPosition().X;
	py = LF.getPosition().Y;
	IMesh* mesh = smgr->getMesh("../assets/New.3ds");
	bNewGame = smgr->addMeshSceneNode(mesh, 0, 0, vector3df(NewGame.getPosition().X, NewGame.getPosition().Y, 29));
	bNewGame->setMaterialFlag(EMF_LIGHTING, false);
	bNewGame->setScale(vector3df(4, 4, 4));
	mesh = smgr->getMesh("../assets/Exit.3ds");
	bExitGame = smgr->addMeshSceneNode(mesh, 0, 0, vector3df(ExitGame.getPosition().X, ExitGame.getPosition().Y, 29));
	bExitGame->setMaterialFlag(EMF_LIGHTING, false);
	bExitGame->setScale(vector3df(4, 4, 4));
	mesh = smgr->getMesh("../assets/Resume.3ds");
	bResumeGame = smgr->addMeshSceneNode(mesh, 0, 0, vector3df(ResumeGame.getPosition().X, ResumeGame.getPosition().Y, 29));
	bResumeGame->setMaterialFlag(EMF_LIGHTING, false);
	bResumeGame->setScale(vector3df(4, 4, 4));
	mesh = smgr->getMesh("../assets/Select.3ds");
	bSelect = smgr->addMeshSceneNode(mesh, 0, 0, vector3df(Select.getPosition().X, Select.getPosition().Y, 29));
	bSelect->setMaterialFlag(EMF_LIGHTING, false);
	bSelect->setScale(vector3df(4, 4, 4));

	/*Select.setPosition(vector3df(px + 3, py + 12, 30));
	NewGame.setPosition(vector3df(px -5, py + 12, 30));
	ResumeGame.setPosition(vector3df(px - 5, py + 15, 30));
	ExitGame.setPosition(vector3df(px - 5, py + 9, 30));*/
}

void Player::bodyScale(){
	if (body){
		body->setPosition(vector3df(initLoc[8].X,initLoc[8].Y-2,initLoc[8].Z+10 ));
		double scale = (initLoc[4].Y - initLoc[0].getDistanceFrom(initLoc[4]))/5.0;
		body->setScale(vector3df(scale,scale,scale));
	}
}
