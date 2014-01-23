#ifndef PLAYER_VIEW_H
#define PLAYER_VIEW_H

#include <osg/Camera>
#include <osg/Light>
#include <osg/Node>
#include <osg/Vec3>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>

#include "CyberWall.h"

/* Because sometimes, OSG has its head up ... you know */
class OSGSphere {
public:
	OSGSphere(osg::Vec3f position, float radius, osg::Group* parent);
	~OSGSphere();
	
	osg::Vec3f getPosition();
	osg::Vec3f setPosition(osg::Vec3f newpos);
private:
	osg::Group* parent;
	osg::Geode* local;
	osg::Vec3f position;
	osg::Vec4f color;
	osg::Sphere* sphereDef;
	osg::ShapeDrawable* drawableSphere;
	osg::PositionAttitudeTransform* transform;
};

/**
 * \brief Handles graphics and collision detection for a single player including cycle and wall, also holds the camera for the player.
 */
class PlayerView {
public:
	PlayerView(int player, osg::ref_ptr<osg::Group> parent, osg::Vec3 initPos, osg::Vec3 viewDirection, osg::Vec3 up, osg::ref_ptr<osg::Camera> camera);
	void setCycleTransform(osg::Vec3 position, std::vector<osg::Vec3f> limbs, osg::Vec3 viewDirection, osg::Vec3 moveDirection, osg::Vec3 up);
	osg::ref_ptr<osg::Camera> getCamera();
	CyberWall& getWall();
	osg::Vec3 getWallPoint(osg::Vec3f moveDirection);
	bool intersects(Player *willHeDie);
	void reset();
	bool isOKStartingPlace(osg::Vec3 location);
	
private:
	osg::PositionAttitudeTransform *cycle; /** The root node for the actual cycle model. */
	osg::ref_ptr<osg::Camera> camera;      /** The camera for one player. */
	CyberWall wall;                        /** This player's CyberWall, which is unique to this player */
	osg::ref_ptr<osg::Group> playerRoot;   /** The root of this whole player in the scene graph */
	osgViewer::Viewer viewer;              /** The viewer that renders this player's viewport */
	int player;                            /** This player's id */
	
	OSGSphere* lFootSphere; /* TEMP */
};

#endif
