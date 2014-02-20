#ifndef FITASHAPE_PLAYER_H
#define FITASHAPE_PLAYER_H 


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "irrlicht/irrlicht.h"
#include "irrlicht/vector3d.h"
#include "time.h"
//#include "fitashape/Game.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

struct CircleNode{
	scene::ISceneNode* node;
	CircleNode* nextNode;
	scene::ISceneNode* target;
};



class Player
{
	friend class Game;
private:
	
	video::IVideoDriver* driver;
	scene::ISceneManager* smgr;

	CircleNode current;
	
	CircleNode LH;
	CircleNode RH;
	CircleNode LF;
	CircleNode RF;

	scene::ISceneNode * LHTarget;
	scene::ISceneNode * RHTarget;
	scene::ISceneNode * LFTarget;
	scene::ISceneNode * RFTarget;

	

	//0=LeftHand, 1=RightHand, 2=LeftFoot 3=RightFoot
	//4=LeftShoulder, 5=RightShoulder 6=LeftHip
	//7=RightHip, 8=CenterOfBody
	vector3df initLoc[9];

	f32 LArm; //estimated length of left arm
	f32 RArm; //estimated length of right arm
	f32 LLeg; //estimated length of left leg
	f32 RLeg; //estimated length of right leg
	f32 ground; //estimated height of the ground


public:
	scene::ISceneNode * currentNode();
	
	virtual~Player(void);	
	Player(IVideoDriver * d, ISceneManager * s);

	void setCurrent(CircleNode& node);
	void initializePosition();
	void randomTargets();
	void drawTargets();
	void drawLimbs();
	bool collide (CircleNode node);
	void setPosition(std::vector<vector3df> vec);
	bool collideAll();
	void setCurrentLH();
	void setCurrentRH();
	void setCurrentLF();
	void setCurrentRF();
	void setPositions(vector3df pos[4]);
	void localInitPos();	
	void addCameraScene();


};

#endif
