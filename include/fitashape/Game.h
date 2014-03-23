#ifndef FITASHAPE_GAME_H
#define FITASHAPE_GAME_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "time.h"

#include "irrlicht/irrlicht.h"
#include "irrlicht/vector3d.h"
#include "vicon/ViconClient.h"
#include "fitashape/Player.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace ViconDataStreamSDK::CPP;
namespace
{
	std::string Adapt( const bool i_Value )
	{
		return i_Value ? "True" : "False";
	}

	std::string Adapt( const Direction::Enum i_Direction )
	{
		switch( i_Direction )
		{
			case Direction::Forward:
				return "Forward";
			case Direction::Backward:
				return "Backward";
			case Direction::Left:
				return "Left";
			case Direction::Right:
				return "Right";
			case Direction::Up:
				return "Up";
			case Direction::Down:
				return "Down";
			default:
				return "Unknown";
		}
	}
}

class Game
{
private:
	bool gameOver;
	bool toExit;
	bool pause;
	bool local;
	ITexture * zenBar;
	


public:

	/**	To receive events like mouse and keyboard input, or GUI events like "the OK
	 *	button has been clicked", we need an object which is derived from the
	 *	IEventReceiver object. There is only one method to override:
	 *	IEventReceiver::OnEvent(). This method will be called by the engine once
	 *	when an event happens. What we really want to know is whether a key is being
	 *	held down, and so we will remember the current state of each key.
	 *
	 */

	class MyEventReceiver : public IEventReceiver
	{
	public:
		// This is the one method that we have to implement


		virtual bool OnEvent(const SEvent& event)
		{
			// Remember whether each key is down or up
			if (event.EventType == EET_KEY_INPUT_EVENT)
				KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

			return false;
		}

		// This is used to check whether a key is being held down
		virtual bool IsKeyDown(EKEY_CODE keyCode) const
		{
			return KeyIsDown[keyCode];
		}

		MyEventReceiver()
		{
			for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
				KeyIsDown[i] = false;
		}

	private:
		// We use this array to store the current state of each key
		bool KeyIsDown[KEY_KEY_CODES_COUNT];
	};
	
	
	Player * p1;
	video::IVideoDriver * driver;
	scene::ISceneManager * smgr;
	IrrlichtDevice * device;
	ITimer * myClock;
	int zen;
	int score;
	int timesUp;
	scene::ITextSceneNode * text;
	MyEventReceiver receiver;

	Game(bool);
	~Game();
	
	int run();
	void moveKeyboard(MyEventReceiver receiver);
	void motionTracking();
	void createClock();
	void updateClock();
	void startLocation();
	void retryMenu(); //delete this later
	void pauseMenu();
	void drawObjects();
	
	int viconInit();
	void viconExit();

};

#endif
