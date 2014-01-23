#ifndef __VIEW_H
#define __VIEW_H

#include <vector>

#include <osg/Camera>
#include <osg/Light>
#include <osg/Node>
#include <osg/Vec3>

#include <osgViewer/CompositeViewer>

#include "Player.h"
#include "HUD.h"
#include "SplitScreenSetup.h"

/**
 * \brief Handles all the graphics for the whole game.
 */
class View {
public:
	View(std::vector<Player*> players, SplitScreenSetup *setup, int mapx, int mapy);
	~View();
	void movePlayer(int player, float dTheta, float dTime);
	void createPlayers(std::vector<Player*> players);
	void realize();
	bool isRealized();
	bool done();
	void frame();
	osg::ref_ptr<osgViewer::CompositeViewer> getOSGViewer();
	
private:
	void createArena(int width, int height);
	void createPlayers();
	void createWalls(int width, int height);
	void createFloor(int width, int height);
	
	osg::ref_ptr<osg::Group> root;                   /** The root of the whole scene graph */
	std::vector<PlayerView*> playerViews;            /** All the PlayerViews which render each player individually */
	osg::ref_ptr<osg::Group> arena;                  /** The root of the arena containing the floor and walls */
	osg::ref_ptr<osgViewer::CompositeViewer> viewer; /** The master viewer which composites the individual players together */
	std::vector<osg::ref_ptr<osg::Geode> > walls;    /** The walls of the arena */
	osg::ref_ptr<osg::Geode> floor;                  /** The floor */
	osg::ref_ptr<osg::GraphicsContext> gc;           /** The current graphics context */
	HUD *hud;                                        /** The HUD overlay */
	SplitScreenSetup *setup;                         /** The way we split the screen for players */
};

#endif
