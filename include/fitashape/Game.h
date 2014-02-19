#ifndef FITASHAPE_GAME_H
#define FITASHAPE_GAME_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "time.h"

#include "irrlicht/irrlicht.h"
#include "irrlicht/vector3d.h"
#include "vicon/ViconSegment.h"
#include "vicon/ViconInputClient.h"
#include "fitashape/player.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

class Game
{

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


private:
//clock to race and score to keep track of
	ITimer* myClock;
	int score = 0;
	int timesUp = 10;

	scene::ITextSceneNode * text;

public:
	video::IVideoDriver* driver;
	scene::ISceneManager* smgr;
	Game();
	~Game();

	void moveKeyboard(MyEventReceiver receiver, const f32 frameDeltaTime );
	void motionTracking(ViconInputClient * vClient);
	void createClock(IrrlichtDevice * device, ISceneManager * smgr);
	void updateClock(IrrlichtDevice * device)

};

#endif
