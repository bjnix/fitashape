#include "Keys.h"

/**
 * \brief Creates a Keys object, only one should ever be needed.
 *
 * \param numPlayers the number of players to allocate state for.
 */
Keys::Keys(int numplayers) {
	players = new Player*[numplayers];
	for(int i = 0; i < numplayers; i++){
		players[i] = new Player();
		players[i]->limbs.push_back(osg::Vec3f(0.0, 0.0, 0.0)); // Left Hand
		players[i]->limbs.push_back(osg::Vec3f(0.0, 0.0, 0.0)); // Left Foot
		players[i]->limbs.push_back(osg::Vec3f(0.0, 0.0, 0.0)); // Right Hand
		players[i]->limbs.push_back(osg::Vec3f(0.0, 0.0, 0.0)); // Right Foot
	}
	numPlayers = numplayers;
}

/**
 * \brief Destroy this Keys, does nothing.
 *
 */
Keys::~Keys() {
	// Cleanup
}

/**
 * \brief Gets the requested rotation of the given player form -1 to 1 (in game units per second).
 * 
 * \prama player the player to get the dTheta for.
 */
float Keys::getTheta(int player) {
	return players[player]->dTheta;
}

/**
 * \brief Sets the requested rotation of the given player form -1 to 1 (in game units per second).
 * 
 * \param player the player to set dTheta for.
 * \param value the vlaue of dTheta -1 to 1.
 */
float Keys::setTheta(int player, float value) {
	float ret = players[player]->dTheta;
	players[player]->dTheta = value;
	return ret;
}

osg::Vec3f Keys::getLimb(int player, int limb) {
	if(limb < 0 || limb > FOOT_RIGHT) {
		printf("Invalid value for limb!!\n");
		exit(0);
	}
	return players[player]->limbs[limb];
}

osg::Vec3f Keys::setLimb(int player, int limb, float x, float y) {
	if(limb < 0 || limb > FOOT_RIGHT) {
		printf("Invalid value for limb!!\n");
		exit(0);
	}
	osg::Vec3f old = players[player]->limbs[limb];
	players[player]->limbs[limb] = osg::Vec3f(x, y, 0.0);
	return old;
}

/**
 * \brief Gets the requested NES key of the given player.
 * 
 * \prama player the player to get the key for.
 * \param which which key to get.
 */
bool Keys::getKey(int player, Key which) {
	Player *p = players[player];
	return p->keys[which];
}

/**
 * \brief Sets the requested NES key of the given player to a given value.
 * 
 * \param player the player to set the NES key for.
 * \param which the NES key to set.
 * \param value the value of the key (true == depressed).
 */
bool Keys::setKey(int player, Key which, bool value) {
	Player *p = players[player];
	bool ret = p->keys[which];
	p->keys[which] = value;
	return ret;
}
