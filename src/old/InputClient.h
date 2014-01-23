#ifndef __INPUT_CLIENT_H
#define __INPUT_CLIENT_H

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <osgViewer/Viewer>
#include "Keys.h"
#include "Game.h"

/**
 * \brief Represents an input client that get its input from a socket. This is a work in progress.
 * 
 * InputClient's act as adapters to outside sources of input for the game as well as driving the game timer.
 */
class InputClient {
public:
	InputClient(int players, Game *g);
	virtual ~InputClient();
	virtual void run();
protected:
	bool handleInput(std::string next);
	Game *game; /** The game object to drive. */
	View *view; /** The game view. */
	Keys* keys; /** The current input state. */
};

#endif
