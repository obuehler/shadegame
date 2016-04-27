#ifndef __M_POWERUP_H__
#define __M_POWERUP_H__

#include "cocos2d.h"
#include <cornell/CUCapsuleObstacle.h>
using namespace cocos2d;

enum PowerupType {};  // TODO fill this

class Powerup : public CapsuleObstacle {
	
private:
	/** Position on the list of powerups, used during collision processing to
	mark the powerups removed. */
	int _index;

	/** 
	* The type of the powerup. Used during collision processing to apply the
	* correct effect to the game.
	*/
	PowerupType _type;

	/**
	* Static constructor with position and index. Objects should be created with
	* this method once dynamic level loading is implemented.
	*
	* @param	pos	The initial position in world coordinates
	* @param	i	The index in the list of power-up metadata
	*
	* @return  An autoreleased physics object
	*/
	static Powerup* create(const Vec2& pos, const PowerupType& t, int i=0);

	/** Initializes a new power-up object with the supplied position and index. */
	bool init(const Vec2& pos, const PowerupType& t, int i, bool resetDrawScale);

public:
	/** Returns the type */
	PowerupType getType() const { return _type; };

	/** Sets the type */
	void setType(PowerupType t) { _type = t; };

	/** Returns the type */
	int getIndex() const { return _index; };

	/** Sets the type */
	void setIndex(int i) { _index = i; };
};

#endif  /* __M_POWERUP_H__ */