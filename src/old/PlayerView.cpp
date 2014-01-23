#include "PlayerView.h"
#include "Player.h"
#include "Keys.h"
#include <osgDB/ReadFile>

/**
 * \brief Constructs the player view object, creating a player geode that has a cycle geode.
 *
 * \param player the id of the player.
 * \param root the root node that the player node will be added to.
 * \param initPos the initial position of the player.
 * \param direction the initial direction the player will be pointed.
 * \param up the up direction from the arena.
 * \param camera a reference to the games camera object that will contain the view of the players screen.
 *
 */
PlayerView::PlayerView(int player, osg::ref_ptr<osg::Group> root, osg::Vec3 initPos, osg::Vec3 direction, osg::Vec3 up, osg::ref_ptr<osg::Camera> camera) : wall(CyberWall(player, initPos)), player(player){
	playerRoot = new osg::Group;
	this->camera = camera;
	
	//create cycle
	cycle = new osg::PositionAttitudeTransform();
	cycle->setPosition(initPos);
	osg::Quat q;
	q.makeRotate(osg::Vec3f(1,0,0), direction);
	cycle->setAttitude(q);//TODO: add up vector
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("./assets/Light Cycle/HQ_Movie cycle.obj");
	cycle->addChild(model);
	playerRoot->addChild(cycle);
	
	camera->setClearColor(osg::Vec4(0, 0, 255, 1)); // black background
	camera->setProjectionMatrixAsPerspective(30, 4.0 / 3.0, 0.1, 100);
	
	// Track player limbs
	osg::ref_ptr<osg::Group> limbNodes = new osg::Group();
	playerRoot->addChild(limbNodes);
	
	// Create Left Foot
	lFootSphere = new OSGSphere(osg::Vec3f(0.0, 0.0, 0.0), (float)50.0, limbNodes);

	//setup camera
	camera->setViewMatrixAsLookAt(
		osg::Vec3(-7.5, 7.5, 0), // eye above xy-plane
		cycle->getPosition() + osg::Vec3(0, 3, 0),    // gaze at cycle
		osg::Vec3(0, 1, 0));   // usual up vector
	
	playerRoot->addChild(wall.asNode());
	root->addChild(playerRoot);
}

/**
 * \brief Sets the transformation on the light cycle in terms of attitude and position.
 * 
 * \param position position in world coords
 * \param viewDirection direction in model coords
 * \param moveDirection direction in world coords
 * \param up up in world coords
 */
void PlayerView::setCycleTransform(osg::Vec3 position, std::vector<osg::Vec3f> limbs, osg::Vec3 viewDirection, osg::Vec3 moveDirection, osg::Vec3 up){
	if(cycle){
		lFootSphere->setPosition(limbs[FOOT_LEFT]);
		cycle->setPosition(position);
	}
	cycle->setAttitude(osg::Quat(M_PI, viewDirection));//TODO: add up
	
	camera->setViewMatrixAsLookAt(
				position - moveDirection * 10 + osg::Vec3(0, 4, 0), // eye above xy-plane
				position + osg::Vec3(0, 2, 0),    // gaze at cycle
				osg::Vec3(0, 1, 0));   // usual up vector
}


/**
 * \brief Determines if the player represented by this PlayerView will kill the supplied player either with the light cycle or the CyberWall.
 *        It is safe to call this with the player of this PlayerView (i.e. willHeDie->getView() == this) and only the CyberWall will be checked.
 * 
 * \param willHeDie the player that might be killed by this one.
 * 
 */
bool PlayerView::intersects(Player *willHeDie){
	osg::BoundingSphere theirCycle = willHeDie->getView()->cycle->getBound();
	osg::BoundingSphere myCycle = cycle->getBound();
	if((this->player != willHeDie->getView()->player) && myCycle.contains(theirCycle.center())){
		//within half a radius
		return true;
	}
	return wall.intersects(willHeDie);
}

void PlayerView::reset(){
	wall.reset();
}

/**
 * \brief checks if the location is an ok place for the player to respawn.
 *
 * \param location the location where the player is trying to respawn.
 *
 * \return true if the location is ok, false otherwise.
 */
bool PlayerView::isOKStartingPlace(osg::Vec3 location){
	return wall.isOKStartingPlace(location);
}

/**
 * \brief Get the camera associated with this PlayerView, each player has one camera attached to a different viewport.
 */
osg::ref_ptr<osg::Camera> PlayerView::getCamera(){
	return camera;
}

/**
 * \brief Retrieves this players CyberWall.
 */
CyberWall& PlayerView::getWall(){
	return wall;
}

/**
 * \brief Calculates the point in world coords that the wall should spawn at, this is outside the cycle's bounding box.
 * 
 * \param moveDirection The direction of movement in world coords of this light cycle.
 */
osg::Vec3 PlayerView::getWallPoint(osg::Vec3f moveDirection){
	return cycle->getPosition() - (moveDirection*3.5);
}

/**
 * OSGSphere: Because osg::Sphere literally can't set its own position without
 *            someone holding its hand.
 */
OSGSphere::OSGSphere(osg::Vec3f position, float radius, osg::Group* parent) {
	this->position = position;
	this->parent = parent;
	this->color = osg::Vec4f(1.0, 1.0, 1.0, 0.5);
	
	local = new osg::Geode();
	sphereDef = new osg::Sphere(position, 5);
	drawableSphere = new osg::ShapeDrawable(sphereDef);
	drawableSphere->setColor(this->color);
	drawableSphere->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	transform = new osg::PositionAttitudeTransform();
	
	transform->addChild(local);
	local->addDrawable(drawableSphere);
	parent->addChild(local);
}

OSGSphere::~OSGSphere() {
	//parent.removeChild(drawableSphere);
}

osg::Vec3f OSGSphere::getPosition() {
	return position;
}

osg::Vec3f OSGSphere::setPosition(osg::Vec3f newpos) {
	osg::Vec3f old = position;
	position = newpos;
	
	transform->setPosition(newpos);
	
	return old;
}
