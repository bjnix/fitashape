#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <stdio.h>

#include "CyberWall.h"
#include "Player.h"

std::vector<osg::Vec4*> CyberWall::playerColors;

/**
 * \brief Creates a CyberWall, create one of these per player.
 * 
 * \param player the player id.
 * \param initial the initial anchor point for the wall.
 */
CyberWall::CyberWall(int player, osg::Vec3f initial) : old(initial), firstTime(true), player(player){

	// Initialize player color options
	if(playerColors.empty()) {
		playerColors.push_back(new osg::Vec4(0.396, 0.642, 0.702, 0.70));
		playerColors.push_back(new osg::Vec4(0.682, 0.375, 0.346, 0.70));
	}

	wallRoot = new osg::Geode();
	totalDist = 0;
}

/**
 * \brief Inserts a new wall segment spanning between two points.
 * 
 * \param from the starting point
 * \param to the ending point
 */
void CyberWall::insertBox(osg::Vec3f from, osg::Vec3f to){
	osg::Vec3f direction = to - from;
	float dist = direction.length();
	direction.normalize();
	osg::Vec3 center = (from+to)/2;
	osg::Box* wall = new osg::Box(center + osg::Vec3(0, WALL_Y_HEIGHT/2, 0), WALL_X_WIDTH, WALL_Y_HEIGHT, dist);
	osg::Quat q;
	q.makeRotate(osg::Vec3(0, 0, 1), direction);
	wall->setRotation(q);
	osg::ShapeDrawable* drawableWall = new osg::ShapeDrawable(wall);
	drawableWall->setColor(*(playerColors[player]));
	drawableWall->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	wallRoot->addDrawable(drawableWall);
}

/**
 * \brief Extends this wall to a new point, a segment will be added between the supplied point and the last point given to this method. 
 * 
 * \param next the next point.
 */
void CyberWall::extend(osg::Vec3f next){
	if(firstTime){
		firstTime = false;
		old = next;
		points.push_back(next);
		return;
	}
	osg::Vec3f direction = next - old;
	float dist = direction.length();
	totalDist += dist;
	if(dist < 0.2){
		return;
	}
	points.push_back(next);
	insertBox(old, next);
	old = next;
}

/**
 * \brief Supplies a node suitable for inserting into the scene graph that contains the complete CyberWall.
 */
osg::ref_ptr<osg::Node> CyberWall::asNode(){
	return wallRoot;
}

/**
 * \brief Prints the dimensions of a bounding box to stdout.
 * 
 * \param box the box to print.
 */
void printBdd(osg::BoundingBox box){
	printf("Bounding box [<%f,%f>;<%f,%f>;<%f,%f>]\n", box.xMin(), box.xMax(), box.yMin(), box.yMax(), box.zMin(), box.zMax());
}

/**
 * \brief Determines if this cyberwall intersects the given player.
 * 
 * \param player the player to check.
 */
bool CyberWall::intersects(Player *player){
	osg::BoundingBox playerBounds = player->boundingBox();
	for(unsigned i = 0; i < points.size(); i++){
		if(playerBounds.xMin() < points[i].x() && points[i].x() < playerBounds.xMax()
				&& playerBounds.yMin() < points[i].y() && points[i].y() < playerBounds.yMax()
				&& playerBounds.zMin() < points[i].z() && points[i].z() < playerBounds.zMax()){
			return true;
		}
	}
	return false;
}

/**
 * \brief Remove all the segments from this cyberwall and cause the last anchor point to be erased.
 *        After this, you need to call extend twice to get the first segment.
 */
void CyberWall::reset(){
	wallRoot->removeDrawables(0, wallRoot->getNumDrawables());
	points.clear();
	firstTime = true;
}

/**
 * \brief Determines if putting a player at the specified location would put them in imminent danger from this CyberWall.
 * 
 * \param location the location a player might spawn at.
 */
bool CyberWall::isOKStartingPlace(osg::Vec3 location){
	for(unsigned i = 0; i < points.size(); i++){
		if((location - points[i]).length() < 20){
			return false;
		}
	}
	return true;
}
