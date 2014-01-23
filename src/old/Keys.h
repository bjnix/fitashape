#ifndef __KEYS_H
#define __KEYS_H

#include <vector>
#include <osg/Vec3f>

/**
 * \brief All the valid keys we use on a NES controller.
 */
enum Key {
	UP = 0, DOWN, LEFT, RIGHT, A, B, START, SELECT
};

enum Limb {
	HAND_LEFT = 0, FOOT_LEFT, HAND_RIGHT, FOOT_RIGHT
};

/**
 * \brief Represents input state for our program in terms of NES buttons and delta theta's (for turning).
 *        Currently the NES controller inputs are ignored, though left and right should be hooked up.
 *        Only dTheta is used for now.
 */

class Keys {
private:
	/**
	 * Represents the input state of one player.
	 */
	typedef struct _player {
		// Generic NES input
		bool keys[8];

		// Specific to CyberCycles
		float dTheta;
		
		std::vector<osg::Vec3f> limbs;
	} Player;
public:
	Keys(int numplayers);
	~Keys();

	int numPlayers;                                  /** The number of players, yes this is public, don't write to it! */
	Player** players;                                /** All the players' input */
	float getTheta(int player);                      
	float setTheta(int player, float value);
	osg::Vec3f setLimb(int player, int limb, float x, float y);
	osg::Vec3f getLimb(int player, int limb);
	bool getKey(int player, Key which);
	bool setKey(int player, Key which, bool value);



	
};

#endif
