//Class header for Circle Node

#ifndef CIRCLENODE_H
#define CIRCLENODE_H

#include "irrlicht/irrlicht.h"
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

class CircleNode
{

private:
	CircleNode* targ;
	scene::ISceneManager* smgr;
	

public:
	scene::ISceneNode* node;
	
	CircleNode();
	void init(scene::ISceneManager*, irr::f32);
	void init(scene::ISceneManager*);
	~CircleNode();
	void setTarget(CircleNode*);
	CircleNode* target();
	void setPosition(vector3df);
	vector3df getPosition(); //maybe const?

	/* data */
};

#endif