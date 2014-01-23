#ifndef HUD_H
#define HUD_H

#include <osg/Group>
#include <osgViewer/Viewer>

#include <vector>

#include "Player.h"
#include "SplitScreenSetup.h"

struct HUDForPlayer;

/**
 * \brief Creates a per player heads up display.
 */
class HUD{
public:
	HUD(std::vector<Player*> players);	
	void update();	
	osg::Node* getHudRoot();
	
private:
	std::vector<Player*> players;       /** The players in the HUD. */
	std::vector<HUDForPlayer*> huds;    /** The individual HUDs. */
	osg::ref_ptr<osgViewer::View> view; /** The master viewer. */
	osg::ref_ptr<osg::Group> hudRoot;   /** The root of the HUD in the scene graph. */
	
	void setupPlayers();
	void setupPlayer(int player);
};

#endif
