#ifndef __GAME_H
#define __GAME_H

#include <vector>
#include "Keys.h"
#include "View.h"
#include "Player.h"
#include "SplitScreenSetup.h"

/**
 * \brief The Game class handles everything but input for the game.
 * 
 * The Game class handles setting up openscenegraph, managing the graphics, enforcing the game rules and such.
 * The InputClient class and its derivatives will keep the game updating at regular intervals and give the input to the Game class.
 */
class Game{
public:
	Game(int n, SplitScreenSetup *setup, int mapx, int mapy);
	View* getView();
	void input(struct Keys *keys, float dTime);
	static Game* getInstance();
	void selectReasonableStartingPosition(int forPlayer, osg::Vec3 &position, osg::Vec3 &viewDirection, osg::Vec3 &moveDirection, osg::Vec3 &normal);
	const int numPlayers;         /** The number of players in the game. */
private:
	static Game* instance;        /** Game is implemented as a singleton. */
	View *view;                   /** The View which handles all the graphics. */
	std::vector<Player*> players; /** The list of players in the game. */
	int width;                    /** The width of the map (could pass for meters). */
	int height;                   /** The height of the map (could pass for meters). */
};

#endif
