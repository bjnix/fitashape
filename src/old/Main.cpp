#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>
#include <iostream>
#include <sys/resource.h>
#include <cassert>

#include "Game.h"
#include "InputClient.h"
#include "LocalInputClient.h"
#include "ViconInputClient.h"
#include "View.h"

namespace Constants {
	bool FAS_DEBUGGING;
}
using namespace std;

/**
 * \brief Entry point for execution.
 * 
 * The expected format of input arguments is "num_players global_width global_height local_x local_y local_width local_height [local]"
 * where widths and heights are in pixels of the "whole screen" and "this screen" respectively. These only differ on the display wall.
 * On a single machine, a proper invocation would look like "2 1920 1080 0 0 1920 1080 local".
 *
 * \param argc expects 8 or 9 arguments.
 * \param argv expects a string,  7 integers and an optional string.
 */
int main(int argc, char **argv) {

	Constants::FAS_DEBUGGING = getenv("FITASHAPE_DEBUG");

	if(Constants::FAS_DEBUGGING) {
		// Enable core dumps
		rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
		assert( setrlimit( RLIMIT_CORE, &core_limit ) == 0 );
		
		// Argument checks
		printf("%d arguments: [", argc);
		if(argc > 0) {
			printf("\"%s\"", argv[0]);
		}
		for(int i = 1; i < argc; i++) {
			printf(", \"%s\"", argv[i]);
		}
		printf("]\n");
	}
	
	// Ensure proper startup parameters
	if(argc != 8 && argc != 9) {
		printf("Wrong number of args, expected FitAShape players global_width global_height local_x local_y local_width local_height\n");
		printf("If you're running FitAShape directly, try using ./run.sh [local] instead. See the README for details.\n");
		exit(1);
	} else {
		if(Constants::FAS_DEBUGGING) printf("Correct number of arguments\n");
	}
	fflush(stdout);
	
	int players = atoi(argv[1]);
	int gwidth = atoi(argv[2]);
	int gheight = atoi(argv[3]);
	int x = atoi(argv[4]);
	int y = atoi(argv[5]);
	int width = atoi(argv[6]);
	int height = atoi(argv[7]);
	
	Screen *globalScreen = new Screen(gwidth, gheight);
	LocalGeometry *localScreen = new LocalGeometry(x, y, width, height);
	SplitScreenSetup *setup = new SplitScreenSetup(players, localScreen, globalScreen);
	
	// Prepare the game and input method, then get running
	Game g(players, setup, 200, 200);
	InputClient* ipc;
	
	char *arg = argv[argc-1];
	if(strncmp(arg, "local", 6) == 0) {
		// Local: keyboard
		printf("Using local (keyboard) input\n");
		ipc = new LocalInputClient(players, &g);
	}
	else {
		// Default: Vicon tracking system
		printf("Using default (Vicon tracking system) input\n");
		ipc = new ViconInputClient(players, &g);
	}
	
	ipc->run(); // Blocks until game is over
	delete ipc;
	exit(0);
}
