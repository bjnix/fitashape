/**
	FIT A SHAPE!
*/
//USED FOR DEBUGING MODE
#define DEBUG 1
#define IFDEBUG if(DEBUG)


#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "fitashape/Game.h"




/*
	Main method for the game.
	if argv[1] is "Local" then will run with keyboard inputs.
 */
int main(int argc, char* argv[])
{

	std::cout << "Starting main \n"<< std::flush;

	bool local = false;
	//check to see if we are testing on local or with tracking system
	if(argc !=1 && (strcmp(argv[1],"Local") == 0 || strcmp(argv[1],"local") == 0))
		local = true;
	Game fit_Game = new Game(local);
	return 0;
}

/*
That's it. Compile and play around with the program.
**/
