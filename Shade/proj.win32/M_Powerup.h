/* The Powerup class. All powerups should be the subclass of this. */

#ifndef __M_POWERUP_H__
#define __M_POWERUP_H__

#include <cornell/CUCapsuleObstacle.h>

using namespace cocos2d;

class Powerup : public CapsuleObstacle {
	
private:
	/** Position on the list of powerups, used during collision processing to
	mark the powerups removed. */
	int _index;

	/** Initializes a new power-up object with the supplied position and index. */
	virtual bool init(const Vec2& pos, int i) = 0;

public:
	/** Returns the index */
	int getIndex() const { return _index; };

	/** Sets the index */
	void setIndex(int i) { _index = i; };

#pragma mark Physics Methods
	/**
	* Create new fixtures for this body, defining the shape
	*
	* This is the primary method to override for custom physics objects
	*/
	virtual void createFixtures() override = 0;

	/**
	* Release the fixtures for this body, reseting the shape
	*
	* This is the primary method to override for custom physics objects.
	*/
	virtual void releaseFixtures() override = 0;
};

#endif  /* __M_POWERUP_H__ */