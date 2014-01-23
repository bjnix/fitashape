#include <stdio.h>
#include "Keys.h"
#include "Player.h"
#include "Game.h"

/**
 * \brief Creates a player, does not set up its graphics.
 * 
 * \param id his players id, these need to be contiguous integers from 0 to numPlayers-1.
 * \param position the starting position of this player in world coords.
 * \param direction the starting direction of this player in world coords.
 * \param normal this players up vector, probably <0,1,0>.
 */
Player::Player(int id, osg::Vec3 position, osg::Vec3 moveDirection, osg::Vec3 viewDirection, osg::Vec3 normal){
	this->speed = 20;
	this->speedMultiplier = 0;
	this->time = 0;
	this->position = position;
	this->moveDirection = moveDirection;
	this->moveDirection.normalize();
	this->viewDirection = viewDirection;
	this->viewDirection.normalize();
	this->normal = normal;
	this->normal.normalize();
	this->id = id;
	this->turnSpeed = 0.15;

}

/**
 * \brief Empty destructor for now
 */
Player::~Player(){
}

/**
 * \brief Sets the PlayerView which will do the graphics for this player. This must be set for every player before trying to render.
 * 
 * \param v the PlayerView for this player.
 */
void Player::setView(PlayerView *v){
	view = v;
}

/**
 * \brief Retrieves the PlayerView previously set with setView()
 */
PlayerView* Player::getView(){
	return view;
}

/**
 * \brief Gets the player's current position in world coords.
 */
osg::Vec3 Player::getPosition(){
	return position;
}

/**
 * \brief Gets the player's current direction of movement in world coords.
 */
osg::Vec3 Player::getMoveDirection(){
	return moveDirection;
}

/**
 * \brief Gets the player's current direction in model coords.
 */
osg::Vec3 Player::getViewDirection(){
	return viewDirection;
}

/**
 * \brief Gets the player's up vector in world coords.
 */
osg::Vec3 Player::getNormal(){
	return normal;
}

/**
 * \brief The player's current score, can be negative inf a player kills him/herself.
 */
int Player::getScore(){
	return score;
}

/**
 * \brief Sets this players score.
 * 
 * \param s the new score, may be negative. 
 */
void Player::setScore(int s){
	score = s;
}

/**
 * \brief Retrieves this players id, guaranteed to be in the range 0 to numPlayers-1.
 */
int Player::getID(){
	return id;
}

/**
 * \brief Updates this player one game step forward in time.
 * 
 * \param dTheta the amoumt this player should be turning (-1 to 1).
 * \param dTime the size of the time step to simulate forward.
 */
void Player::updatePlayer(float dTheta, std::vector<osg::Vec3f> limbs, float dTime){
	static osg::Vec3 z(0, 1, 0);

	this->limbs = limbs;
	
	dTheta *=  turnSpeed;
	dTheta *= -1;// Inverted controls
	
	osg::Quat moveRot(dTheta, z);
	osg::Quat viewRot(dTheta / 2, z);
	
	moveDirection = moveRot * moveDirection;
	viewDirection = viewRot * viewDirection;
	
	time += dTime;
	speedMultiplier = 1.0 / (1.0 + exp(-time * 0.2));
	
	position += moveDirection * speed * speedMultiplier * dTime;
	
	//printf("Player %d, dTheta %f, dTime %f, position = <%f, %f, %f, >, moveDirection = <%f, %f, %f>\n", id, dTheta, dTime, position[0], position[1], position[2], moveDirection[0], moveDirection[1], moveDirection[2]);
	
	if(view){
		view->setCycleTransform(position, this->limbs, viewDirection, moveDirection, normal);
		view->getWall().extend(view->getWallPoint(moveDirection));
	}
}

/**
 * \brief Determines if this player should kill the passed in player with his cycle or light wall this turn.
 * 
 * \param p the player that might be destroyed this turn.
 */
bool Player::intersects(Player *p){
	return view->intersects(p);
}

/**
 * \brief Resets this player, typically called after death. This player will lose all CyberWalls and be placed in a random location.
 * 
 * The random location this player is placed in will attempt to avoid puting the player too close to a wall or a CyberWall so they don't just die again.
 * If this fails, they will just be placed wherever regardless.
 * 
 */
void Player::reset(){
	time = 0;
	if(view){
		view->reset();
		Game::getInstance()->selectReasonableStartingPosition(id, this->position, this->viewDirection, this->moveDirection, this->normal);
		view->setCycleTransform(position, this->limbs, viewDirection, moveDirection, normal);
	}
}

/**
 * \brief Gets the bounding box for this player, used for locision detection.
 * 
 * This is just a cube of size 0.5, axis aligned and centered on the players current position.
 */
osg::BoundingBox Player::boundingBox(){
	double cubeSize = 0.5;
	osg::BoundingBox box(position[0]-cubeSize, position[1]-cubeSize, position[2]-cubeSize, position[0]+cubeSize, position[1]+cubeSize, position[2]+cubeSize);
	return box;
}

/**
 * \brief Determines if this player would put a player in imminent danger if the other player were placed at the location provided.
 * 
 * \param location a candidate location fo a player to respawn.
 */
bool Player::isOKStartingPlace(osg::Vec3 location){
	if((location - position).length() < 20){
		return false;
	}
	return view->isOKStartingPlace(location);
}

bool Player::isInBounds(int width, int height){
	if(position[0] < -(width/2 - 10) || position[0] > (width/2 + 10) || position[2] < -(height/2 - 10) || position[2] > (height/2 + 10)){
		return false;
	}
	return true;
}
