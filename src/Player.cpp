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

double Player::mid(double a, double b){
	return (a/2 + b/2);
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
	
	//add the body
	IMesh* mesh = smgr->getMesh("../assets/circle-stick.3ds");
	if (!mesh)
	{
		printf("mesh did not work\n");
		exit(-1);
		//device->drop();
		//return 1;
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
	int scale = (LShoulder.Y - LArm)/5;
	body->setScale(vector3df(scale,scale,scale));
}


/*
	Method to determine where and when the person is 
	standing when trying to get the initial locations.
	Takes in instance every second and checks to see if the
	play has stood still for the last three seconds
*/



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
	LHTarget.init(smgr, 1);
	
	LHTarget.setTarget(&LH);
	LHTarget.node->setPosition(LH.node->getPosition()); // set it's position to a temp spot
	LHTarget.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	//LHTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
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
	//RHTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
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
	//LFTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
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
	//RFTarget.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	RFTarget.node->setMaterialFlag(video::EMF_LIGHTING, false);

	RestartYes.init(smgr, 1);
	RestartYes.setTarget(&LH);
	RestartYes.node->setPosition(core::vector3df(-20, 10, 30));
	RestartYes.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	//RestartYes.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	RestartYes.node->setMaterialFlag(video::EMF_LIGHTING, false);
	RestartYes.node->setVisible(false);

	RestartNo.init(smgr, 1);
	RestartNo.setTarget(&RH);
	RestartNo.node->setPosition(core::vector3df(20,10,30));
	RestartNo.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
	//RestartNo.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	RestartNo.node->setMaterialFlag(video::EMF_LIGHTING, false);
	RestartNo.node->setVisible(false);

	NewGame.init(smgr, 1);
	NewGame.setTarget(&LH);
	NewGame.node->setPosition(core::vector3df(-10, 10, 30));
	NewGame.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	//NewGame.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	NewGame.node->setMaterialFlag(video::EMF_LIGHTING, false);
	NewGame.node->setVisible(false);

	ResumeGame.init(smgr, 1);
	ResumeGame.setTarget(&LH);
	ResumeGame.node->setPosition(core::vector3df(-10, 13, 30));
	ResumeGame.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	//ResumeGame.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	ResumeGame.node->setMaterialFlag(video::EMF_LIGHTING, false);
	ResumeGame.node->setVisible(false);

	ExitGame.init(smgr, 1);
	ExitGame.setTarget(&LH);
	ExitGame.node->setPosition(core::vector3df(-10, 8, 30));
	ExitGame.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
	//ExitGame.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //make it transarent
	ExitGame.node->setMaterialFlag(video::EMF_LIGHTING, false);
	ExitGame.node->setVisible(false);

	Select.init(smgr, 1);
	Select.setTarget(&RH);
	Select.node->setPosition(core::vector3df(0,10,30));
	Select.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
	//Select.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
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
		node.node->setMaterialTexture(0, node.node->getMaterial(0).getTexture(0));
		//node.node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
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
method to set the the positions of all the nodes. Pass an array of the positions with 
0 = LH, 1 = RH, 2 = LF, 3 = RF
*/
void Player::setPositions(vector3df pos[4]){
	LH.setPosition(pos[0]);
	RH.setPosition(pos[1]);
	LF.setPosition(pos[2]);
	RF.setPosition(pos[3]);
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
	smgr->addCameraSceneNode(0, core::vector3df(initLoc[8].X,initLoc[8].Y+5,0), core::vector3df(initLoc[8].X,initLoc[8].Y+5,initLoc[8].Z));
}

void Player::setTargetVisible(bool visibility, bool resume){
	RFTarget.node->setVisible(visibility);
	LFTarget.node->setVisible(visibility);
	RHTarget.node->setVisible(visibility);
	LHTarget.node->setVisible(visibility);
	NewGame.node->setVisible(!visibility);
	ResumeGame.node->setVisible(!visibility && !resume);
	ExitGame.node->setVisible(!visibility);
	Select.node->setVisible(!visibility);
}

void Player::setMenuInvis(){
	NewGame.node->setVisible(false);
	ResumeGame.node->setVisible(false);
	ExitGame.node->setVisible(false);
	Select.node->setVisible(false);
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

int Player::pauseCollide(){
	if(ResumeGame.node->isVisible() && collide(ResumeGame)){
		if(collide(Select))
			return 1;
		else
			return 4;
	} 
		
	else if(collide(NewGame)){
		if(collide(Select))
			return 2;
		else
			return 5;
	}
	else if(collide (ExitGame)){
		if(collide(Select))
			return 3;
		else
			return 6;
	}
	return 0;
}

bool Player::jump(){
	printf("ground = %f, LF = %f, RF = %f\n",ground,LF.getPosition().Y,RF.getPosition().Y);
	if(LF.getPosition().Y > ground + .75 && RF.getPosition().Y > ground + .75)
		return true;

	return false;
}

void Player::setMenu(){
	f32 px, py;
	px = LF.getPosition().X;
	py = LF.getPosition().Y;
	Select.setPosition(vector3df(px + 3, py + 12, 30));
	NewGame.setPosition(vector3df(px -5, py + 12, 30));
	ResumeGame.setPosition(vector3df(px - 5, py + 15, 30));
	ExitGame.setPosition(vector3df(px - 5, py + 9, 30));
}

void Player::updateBody(){
	//make sure the body exists already
	if(body){

		body->setPosition(vector3df(mid(RF.getPosition().X,LF.getPosition().X), body->getPosition().Y, body->getPosition().Z));

		printf("POS: RF = %f, LF = %f, Mid = %f\n", RF.getPosition().X, LF.getPosition().X,body->getPosition().X);

		printf("ROTATION: LH.X = %f LH Bound = %f \n",LH.getPosition().X, (abs(initLoc[0].X - initLoc[8].X) - body->getPosition().X));
		printf("ROTATION: RH.X = %f RH Bound = %f \n",RH.getPosition().X, (abs(initLoc[1].X - initLoc[8].X) + body->getPosition().X));

		if(LH.getPosition().X < (abs(initLoc[0].X - initLoc[8].X) - body->getPosition().X))
			body->setRotation(vector3df(0,0, 3 * abs((abs(initLoc[0].X - initLoc[8].X) - body->getPosition().X) - LH.getPosition().X)));

		if(RH.getPosition().X > (abs(initLoc[1].X - initLoc[8].X) + body->getPosition().X))
			body->setRotation(vector3df(0,0, -3 * abs((abs(initLoc[1].X - initLoc[8].X) + body->getPosition().X) - RH.getPosition().X)));
	}
}

double Player::abs(double number){
	if(number < 0)
		return number * -1;
	return number;
}
