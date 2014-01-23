#ifndef CYBER_WALL_H
#define CYBER_WALL_H

#include <osg/ShapeDrawable>


class Player;

#define WALL_Y_HEIGHT  1.0
#define WALL_X_WIDTH   0.05

/**
 * \brief Does collision detection and graphics for one player's CyberWall.
 */
class CyberWall {
private:
	osg::Vec3f old;
	static std::vector<osg::Vec4*> playerColors;

public:
	CyberWall(int player, osg::Vec3f initial);
	virtual ~CyberWall(){}
	void extend(osg::Vec3f next);
	osg::ref_ptr<osg::Node> asNode();
	bool intersects(Player *player);
	void reset();
	bool isOKStartingPlace(osg::Vec3 location);

private:
	osg::ref_ptr<osg::Geode> wallRoot;
	bool firstTime;
	std::vector<osg::Vec3> points;
	double totalDist;
	void insertBox(osg::Vec3f from, osg::Vec3f to);
	int player;
};

#endif
