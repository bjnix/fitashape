/** fitashape.cpp
Author: Brent Nix : bjnix at mtu dot edu
Description:Contains methods needed specifically for fitashape

*/

/* get the current node
*/
scene::ISceneNode * currentNode(){
	return current.node;
}

/* set the current node
*/
void setCurrent(CircleNode node){
	current = node;
}

/* This method will detect where the persons body is and the relative shape of the arms and legs
i have an idea, have to code it! - kyle
*/
void initializePosition(){

}

/* method to draw the target location for the limb orbs
*/
void drawTargets(scene::ISceneManager* smgr){
	LHTarget = smgr->addSphereSceneNode(1.5);
	if (LHTarget)
	{
		LHTarget->setPosition(core::vector3df(-20,10,30));
		LHTarget->setMaterialTexture(0, NULL);
		LHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RHTarget = smgr->addSphereSceneNode(1.5);
	if (RHTarget)
	{
		RHTarget->setPosition(core::vector3df(20,10,30));
		RHTarget->setMaterialTexture(0, NULL);
		RHTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	LFTarget = smgr->addSphereSceneNode(1.5);
	if (LFTarget)
	{
		LFTarget->setPosition(core::vector3df(-20,-10,30));
		LFTarget->setMaterialTexture(0, NULL);
		LFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RFTarget = smgr->addSphereSceneNode(1.5);
	if (RFTarget)
	{
		RFTarget->setPosition(core::vector3df(20,-10,30));
		RFTarget->setMaterialTexture(0, NULL);
		RFTarget->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}
/*
method to draw the limb orbs
*/
void drawLimbs(scene::ISceneManager* smgr){
	LH.node = smgr->addSphereSceneNode(1);
	if (LH.node)
	{
		
		LH.target = LHTarget;
		LH.nextNode = &RH;
		LH.node->setPosition(core::vector3df(-10,5,30));
		LH.node->setMaterialTexture(0, driver->getTexture("../assets/fire.bmp"));
		LH.node->setMaterialFlag(video::EMF_LIGHTING, false);
		setCurrent(LH);
	}

	RH.node = smgr->addSphereSceneNode(1);
	if (RH.node)
	{
		RH.target = RHTarget;
		RH.nextNode = &LF;
		RH.node->setPosition(core::vector3df(10,5,30));
		RH.node->setMaterialTexture(0, driver->getTexture("../assets/lightFalloff.png"));
		RH.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	LF.node = smgr->addSphereSceneNode(1);
	if (LF.node)
	{
		LF.target = LFTarget;
		LF.nextNode = &RF;
		LF.node->setPosition(core::vector3df(-10,-5,30));
		LF.node->setMaterialTexture(0, driver->getTexture("../assets/particlegreen.jpg"));
		LF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	RF.node = smgr->addSphereSceneNode(1);
	if (RF.node)
	{
		RF.target = RFTarget;
		RF.nextNode = &LH;
		RF.node->setPosition(vector3df(10,-5,30));
		RF.node->setMaterialTexture(0, driver->getTexture("../assets/portal7.bmp"));
		RF.node->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}

/*
method to determine if the passed node is close to its targert
*/
bool collide (CircleNode node){
	vector3df nodeLocation = node.node->getAbsolutePosition();
	vector3df targetLocation = node.target->getAbsolutePosition();
	double collideDist = 2.5; // determins how close the orbs have to be. easy to change
	
	if(nodeLocation.getDistanceFrom(targetLocation) > collideDist)
		return false;
	return true;
}
