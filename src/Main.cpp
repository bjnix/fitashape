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
#include "dgr_framework/DGR_framework.h"

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
	Game * fit_Game;
	
    if( (strcmp(argv[1],"Local") == 0) || (strcmp(argv[1],"local") == 0) )
	{	
		local = true;
	}

	#ifdef DGR_MASTER
	
	if( (local && (argc < 3)) || (!local && (argc < 2)))
	{    
        printf("USAGE: %s  relay-ip-address\n", argv[0]);
        printf("   OR: %s local relay-ip-address\n", argv[0]);
        return 1;
    }
	else if( (local && (argc > 3)) || (!local && (argc > 2 )) )
	{
		printf("Trailing Symbols ignored \n");
	}
	if(local){ fit_Game = new Game(local,argv[2]); }
	else{ fit_Game = new Game(local,argv[1]); }
	#else //SLAVE
	if( (local && argc != 8) || (!local && argc != 7) )
	{   
		printf("Please enter frustum and screen constraints:\n"); 
        printf("USAGE: %s f_left<float>, f_right<float>, f_bottom<float>, f_top, s_width<int>, s_height<int>\n", argv[0]);
        printf("   OR: %s local f_left<float>, f_right<float>, f_bottom<float>, f_top, s_width<int>, s_height<int>\n", argv[0]);
        return 1;
    }
	if(local){ fit_Game = new Game(local,argv[2],argv[3],argv[4],argv[5],argv[6],argv[7]); }
	else{  fit_Game = new Game(local,argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]); }
	#endif

	delete fit_Game;
	return 0;
}

/*
That's it. Compile and play around with the program.
**/
