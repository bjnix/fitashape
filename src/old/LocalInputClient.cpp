#include <stdio.h>

#include <osgGA/GUIEventAdapter>

#include "LocalInputClient.h"
#include "Keys.h"

LocalInputClient::LocalInputClient(int players, Game *g):InputClient(players, g) {
	static bool singleton = true;
	if(singleton == false){
		printf("Cannot create multiple instances of the local input client!\n");
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
	singleton = false;
	osg::ref_ptr<osgViewer::CompositeViewer> cViewer = g->getView()->getOSGViewer();
	if(cViewer->getNumViews() == 0){
		printf("No viewers attached\n");
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
	g->getView()->getOSGViewer()->getView(0)->addEventHandler(this); 
}

LocalInputClient::~LocalInputClient() {}

void LocalInputClient::accept(osgGA::GUIEventHandlerVisitor& v){
	v.visit(*this);
}

void LocalInputClient::handleKeyDown(int key){
	switch(key){
	case 'w':
		keys->setKey(0, UP, true);
		break;
	case 's':
		keys->setKey(0, DOWN, true);
		break;
	case 'a':
		keys->setKey(0, LEFT, true);
		keys->setTheta(0, -1);
		break;
	case 'd':
		keys->setKey(0, RIGHT, true);
		keys->setTheta(0, 1);
		break;
	
	case osgGA::GUIEventAdapter::KEY_Up:
		keys->setKey(1, UP, true);
		break;
	case osgGA::GUIEventAdapter::KEY_Down:
		keys->setKey(1, DOWN, true);
		break;
	case osgGA::GUIEventAdapter::KEY_Left:
		keys->setKey(1, LEFT, true);
		keys->setTheta(1, -1);
		break;
	case osgGA::GUIEventAdapter::KEY_Right:
		keys->setKey(1, RIGHT, true);
		keys->setTheta(1, 1);
		break;
	
	case ' ':
		keys->setKey(0, START, true);
		break;
	}
}

void LocalInputClient::handleKeyUp(int key){
		switch(key){
	case 'w':
		keys->setKey(0, UP, false);
		break;
	case 's':
		keys->setKey(0, DOWN, false);
		break;
	case 'a':
		keys->setKey(0, LEFT, false);
		keys->setTheta(0, 0);
		break;
	case 'd':
		keys->setKey(0, RIGHT, false);
		keys->setTheta(0, 0);
		break;
	
	case osgGA::GUIEventAdapter::KEY_Up:
		keys->setKey(1, UP, false);
		break;
	case osgGA::GUIEventAdapter::KEY_Down:
		keys->setKey(1, DOWN, false);
		break;
	case osgGA::GUIEventAdapter::KEY_Left:
		keys->setKey(1, LEFT, false);
		keys->setTheta(1, 0);
		break;
	case osgGA::GUIEventAdapter::KEY_Right:
		keys->setKey(1, RIGHT, false);
		keys->setTheta(1, 0);
		break;
	
	case ' ':
		keys->setKey(0, START, false);
		break;
	}
}

bool LocalInputClient::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa){
	switch(ea.getEventType()){
	case(osgGA::GUIEventAdapter::KEYDOWN):
		handleKeyDown(ea.getKey());
		break;
	case(osgGA::GUIEventAdapter::KEYUP):
		handleKeyUp(ea.getKey());
		break;
	default:
		break;
	}
	return false;
}

void advance(struct timeval *tv, int usec){
	tv->tv_usec += usec;
	while(tv->tv_usec >= 1000000){
		tv->tv_sec++;
		tv->tv_usec -= 1000000;
	}
}

void diff(struct timeval *targetTime, struct timeval *curTime, struct timeval *tv){
	long us = 0;
	us += 1000000 * (targetTime->tv_sec - curTime->tv_sec);
	us += (targetTime->tv_usec - curTime->tv_usec);
	
	tv->tv_sec = us / 1000000;
	us %= 1000000;
	tv->tv_usec = us;
}

void LocalInputClient::run() {
	// Time since last frame (plus any leftover time not "consumed" from previous frames)
    // Used to keep the game loop ticking at a relatively constant rate
    struct timeval curTime;
    struct timeval targetTime;
    gettimeofday(&targetTime, 0);
    struct timeval tv;
    if(!this->view->isRealized()){
		this->view->realize();
	}
	if(!this->view->isRealized()){
		printf("Could not realize viewer\n");
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
	while( !this->view->done() )
	{
		advance(&targetTime, 25000);
		gettimeofday(&curTime, 0);
		diff(&targetTime, &curTime, &tv);
		select(0, NULL, NULL, NULL, &tv);//wait
		game->input(this->keys, 0.025);
		view->frame();
	}
	
}
