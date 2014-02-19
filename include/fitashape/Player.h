#ifndef PLAYER_H
#define PLAYER_H 


#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "irrlicht/irrlicht.h"
#include "vector3d.h"
#include "time.h"
#include "vicon/ViconInputClient.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace ViconDataStreamSDK::CPP;

struct CircleNode{
	scene::ISceneNode* node;
	CircleNode* nextNode;
	scene::ISceneNode* target;
};

class Player
{
private:

	ITimer* playerClock;

	CircleNode current;
	
	CircleNode LH;
	CircleNode RH;
	CircleNode LF;
	CircleNode RF;

	scene::ISceneNode * LHTarget;
	scene::ISceneNode * RHTarget;
	scene::ISceneNode * LFTarget;
	scene::ISceneNode * RFTarget;

	IrrlichtDevice* playerDevice;
	video::IVideoDriver* playerDriver;
	scene::ISceneManager* playerSmgr;

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

	virtual~Player(void);	
	Player(IrrlichtDevice* d, scene::ISceneManager* s);
	scene::ISceneNode * currentNode();
	void setCurrent(CircleNode& node);
	void initializePosition();
	void randomTargets();
	void drawTargets();
	void drawLimbs();
	bool collide (CircleNode node);
	void setPosition(std::vector<vector3df> vec);
	void startLocation(ViconSegment **segment, scene::ISceneManager* smgr);
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
