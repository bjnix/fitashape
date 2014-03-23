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
#include "dgr/DGR_framework.h"

//std::string HostName = "141.219.28.17:801";//was 141.219.28.107:801

/*
	Main method for the game.
	if argv[1] is "Local" then will run with keyboard inputs.
 */
int main(int argc, char* argv[])
{

	std::cout << "Starting main \n"<< std::flush;

	bool local = false;
	//check to see if we are testing on local or with tracking system
	
	#ifdef DGR_MASTER
	
	if(argc < 2)
	{    
        printf("USAGE: %s  relay-ip-address\n", argv[0]);
        return 1;
    }
    else if(argc == 3 && ( (strcmp(argv[1],"Local") == 0) || (strcmp(argv[1],"local") == 0) ) )
	{	
		local = true;
	}
	else
	{
		printf("Trailing Symbols ignored \n")
	}

	Game * fit_Game = new Game(local,argv[2]);

	#else
	Game * fit_Game = new Game(local,fl,fr,fb,ft,fw,fh);
	delete fit_Game;
	return 0;
}

/*
That's it. Compile and play around with the program.
**/
