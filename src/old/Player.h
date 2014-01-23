#ifndef __PLAYER_H
#define __PLAYER_H

#include <osg/Vec3f>
#include <osg/Group>

#include "PlayerView.h"

/**
 * \brief represents a player and their came logic, also responsible for the graphics for one player through a held PlayerView.
 */
class Player {
public:
	Player(int id, osg::Vec3 position, osg::Vec3 moveDirection, osg::Vec3 viewDirection, osg::Vec3 normal);
	~Player();
	std::vector<osg::Vec3f> limbs;
	void updatePlayer(float dTheta, std::vector<osg::Vec3f> limbs, float dTime);
	void setView(PlayerView *v);
	PlayerView* getView();
	osg::Vec3 getPosition();
	osg::Vec3 getMoveDirection();
	osg::Vec3 getViewDirection();
	osg::Vec3 getNormal();
	int getScore();
	void setScore(int s);
	int getID();
	bool intersects(Player *p);
	void reset();
	osg::BoundingBox boundingBox();
	bool isOKStartingPlace(osg::Vec3 location);
	bool isInBounds(int width, int height);
	
private:
	osg::Vec3 position;      /** This players current position in world coords. */
	osg::Vec3 moveDirection; /** This players direction in world coords. */
	osg::Vec3 viewDirection; /** This players direction in model coords. */
	osg::Vec3 normal;        /** This players up vector in world coords. */
	float speed;             /** This players max speed in world units per second. */
	PlayerView *view;        /** This players view, handles graphics for a single player. */
	int id;                  /** This players id, ranges form 0 to numPlayers-1. */
	float turnSpeed;         /** The turn speed in radians/second. */
	int score;               /** This players score in the game. */
	double speedMultiplier;  /** The speed multiplier to simulate acceleration */
	double time;             /** The total time since this player got reset */
};

#endif
