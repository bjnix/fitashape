//CircleNode class definition
#include "fitashape/CircleNode.h"

void CircleNode::setTarget(CircleNode* add){
	targ = add;
}


void CircleNode::init(scene::ISceneManager* scn, irr::f32 size){
	smgr = scn;
	node = smgr->addSphereSceneNode(size);
}

void CircleNode::init(scene::ISceneManager* scn){
	smgr = scn;
	node = smgr->addSphereSceneNode();
}

CircleNode* CircleNode::target(){
	return targ;
}

CircleNode::CircleNode(){
	//Does nothing
}
CircleNode::~CircleNode(){
	//Does nothing
}

void CircleNode::setPosition(vector3df pos){
	node->setPosition(vector3df(pos.X,pos.Y - 13 ,pos.Z));
}

vector3df CircleNode::getPosition(){
	vector3df temp = node->getPosition();
	return vector3df(temp.X,temp.Y+13,temp.Z);
	//return node->getPosition();
}

void CircleNode::saveTexture(ITexture* color){
	texture = color;
	node->setMaterialTexture(0, texture);
}

void CircleNode::collideTexture(ITexture* color){
	node->setMaterialTexture(0, color);
}

void CircleNode::resetTexture(){
	node->setMaterialTexture(0, texture);
}
