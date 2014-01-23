#include <stdio.h>

#include "InputClient.h"
#include "Game.h"

using namespace std;

bool InputClient::handleInput(string next)
{
	char op = next[0];
	bool on = false;
	if(op == '+') on = true;
	if(op == 'e' || op == 'E') {
		cout << "Quit key pressed. Quitting." << endl;
		exit(0);
	}
	char ch = next[1];

	// Old NES controller input. Legacy CyberCycles code that got dropped.
	switch(ch) {
		case 'w':
			keys->setKey(0, UP, on);break;
		case 'a':
			keys->setKey(0, LEFT, on);break;
		case 's':
			keys->setKey(0, DOWN, on);break;
		case 'd':
			keys->setKey(0, RIGHT, on);break;
		case 'A':
			keys->setKey(0, A, on);break;
		case 'B':
			keys->setKey(0, B, on);break;
		case 'T':
			keys->setKey(0, START, on);break;
		case 'E':
			keys->setKey(0, SELECT, on);

			// For now, Select exits the game
			cout << "Quit key pressed. Quitting." << endl;
			exit(0);
			break;
		case 'e':
			cout << "Quit key pressed. Quitting." << endl;
			exit(0);
		default:
			cout << "Unrecognized key: " << ch << endl;
			return false;
	}
	return true;
}

InputClient::InputClient(int players, Game *g) {
	this->game = g;
	this->view = g->getView();
	this->keys = new Keys(players);
}

InputClient::~InputClient() {
	//cout << "Stopping an input receiver" << endl;
}

void InputClient::run() {
	// Time since last frame (plus any leftover time not "consumed" from previous frames)
	// Used to keep the game loop ticking at a relatively constant rate
	long delta = 0;
	struct timeval lastTime;
	gettimeofday(&lastTime, 0);
	struct timeval nextTime;
	
	fd_set readset;
	struct timeval tv;
	while( !this->view->done() )
	{	
		// Keyboard input forwarded from Python (CyberCycles NES controller code)
		while(true) { // Check stdin until it doesn't have anything on it.
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			FD_ZERO(&readset);
			FD_SET(fileno(stdin), &readset);
			select(fileno(stdin)+1, &readset, NULL, NULL, &tv);
			
			if(FD_ISSET(fileno(stdin), &readset))
			{
				string next;
				cin >> next;
				cout << "Have input: " << next << endl;
				handleInput(next);
			}
			else break;
		}

		// Get the time since last loop, and add the delta
		gettimeofday(&nextTime, 0);
		delta += (nextTime.tv_sec-lastTime.tv_sec)*1000000 + nextTime.tv_usec-lastTime.tv_usec;
		lastTime.tv_sec = nextTime.tv_sec;
		lastTime.tv_usec = nextTime.tv_usec;

		// While that delta is greater than our game loop's time step,
		// run the game loop and decrease the delta (to say we've used that time)
		while(delta > 25000) { // 40 updates/sec, or once every 25,000 uSec
			//cout << "Sending present key structure to game" << endl;
			this->game->input(this->keys, 0.025);
			delta -= 25000;
		}

		this->view->frame();
	}
	cout << "Run loop finished. Should probably close connections and such." << endl;
}
