#include <cstdio>
#include "Game.h"

using namespace std;

Game* Game::instance = NULL;

/**
 * \brief Creates a game, ONLY CALL THIS ONCE, game is a singleton.
 * After calling this Game::getInstance() will return this instance.
 * 
 * \param n the number of players (fixed).
 * \param setup the way the screens will be split up for each player.
 * \param mapx the width of the map (units could pass for meters).
 * \param mapy the height of the map (units could pass for meters).
 */
Game::Game(int n, SplitScreenSetup *setup, int mapx, int mapy):numPlayers(n),width(mapx),height(mapy) {
	if(instance != NULL) {
		fprintf(stderr, "Game is a singleton class.\n");
		exit(1);
	}
	instance = this;
	srand(time(NULL));

	for(int i = 0; i < numPlayers; i++){
		float angle = (((float)i)/numPlayers)*6.2830;
		float dx = cos(angle);
		float dz = sin(angle);
		osg::Vec3 position(dx, 0, dz);
		osg::Vec3 moveDirection(-dx, 0, -dz);
		dx = cos(-angle/2);
		dz = sin(-angle/2);
		osg::Vec3 viewDirection(-dx, 0, -dz);
		osg::Vec3 normal(0, 1, 0);
		players.push_back(new Player(i, position, moveDirection, viewDirection, normal));
	}
	view = new View(players, setup, mapx, mapy);
}

/**
 * \brief advances the game one tick of a provided size with the provided input state.
 * 
 * \param keys the input state set.
 * \param dTime the length of time to advance forward (seconds).
 */
void Game::input(Keys *keys, float dTime){
	if(keys->numPlayers != numPlayers){
		printf("Bad keys");
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
	
	for(int i = 0; i < keys->numPlayers; i++){
		float dTheta = keys->getTheta(i);
		players[i]->updatePlayer(dTheta, keys->players[i]->limbs, dTime);
	}
	
	bool *died = new bool[keys->numPlayers];
	int *diedTo = new int[keys->numPlayers];
	
	for(int i = 0; i < keys->numPlayers; i++){
		died[i] = false;
	}
	
	for(int i = 0; i < keys->numPlayers; i++){
		for(int j = 0; j < keys->numPlayers; j++){
			if(players[i]->intersects(players[j])){
				printf("Player %d died to player %d\n", j, i);
				died[j] = true;
				diedTo[j] = i;
			}
		}
	}
	
	for(int i = 0; i < keys->numPlayers; i++){
		if(!players[i]->isInBounds(width, height)){
			died[i] = true;
			diedTo[i] = i;
		}
	}
	
	for(int i = 0; i < keys->numPlayers; i++){
		if(died[i]){
			int to = diedTo[i];
			if(!died[to]){ // Kill, +1 point to killer
				players[to]->setScore(players[to]->getScore()+1);
			} else if(to == i){ // Suicide, -1pt
				players[i]->setScore(players[i]->getScore()-1);
			}
			players[i]->reset();
		}
	}
	
	delete[] died;
	delete[] diedTo;
}

/**
 * \brief Gets the singleton instance of this Game, the game object must have been previously constructed.
 */
Game* Game::getInstance() {
	if(instance == NULL){
		printf("No game was instantiated, main should have done that!\n");
		exit(1);
	}
	return instance;
}

/**
 * \brief Selects starting conditions for a player that needs to respawn.
 *
 * This method uses output parameters, which we normally avoid in this project, because to avoid them would be even worse looking.
 * 
 * \param forPlayer the player that is respawning (to avoid self collision check).
 * \param position the position of the player after respawn, output parameter.
 * \param viewDirection the new direction of the player's view after respawn, output parameter.
 * \param moveDirection the new direction of the player's movement after respawn, output parameter.
 * \param normal the new up vector of the player after respawn, output parameter.
 */
void Game::selectReasonableStartingPosition(int forPlayer, osg::Vec3 &position, osg::Vec3 &viewDirection, osg::Vec3 &moveDirection, osg::Vec3 &normal){
	osg::Vec3 newPosition(0, 0, 0);
	osg::Vec3 newViewDirection(1, 0, 0);
	osg::Vec3 newMoveDirection(1, 0, 0);
	osg::Vec3 newNormal(0, 1, 0);
	
	//TODO: Yes, this is bad, but they get inexplicably corrupted
	width = 200;
	height = 200;
	
	int x = 0;
	int y = 0;
	
	for(int tries = 0; tries < 100; tries++){
		x = (rand() % (width-40)) - (width/2) + 20;
		y = (rand() % height - 40) - (height/2) + 20;
		bool ok = true;
		for(int p = 0; p < numPlayers; p++){
			if(p == forPlayer){
				continue;
			}
			if(!players[p]->isOKStartingPlace(osg::Vec3(x, 0, y))){
				ok = false;
				break;
			}
		}
		if(ok){
			break;
		}
	}
	
	newPosition[0] = x;
	newPosition[2] = y;
	
	position = newPosition;
	viewDirection = newViewDirection;
	moveDirection = newMoveDirection;
	normal = newNormal;
}

/**
 * Retrieves the whole Game view that handles all the game graphics.
 */
View* Game::getView(){
	return view;
}
