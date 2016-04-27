#include "M_Powerup.h"
#include <cornell.h>

/**
* Static constructor with position and index. Objects should be created with
* this method once dynamic level loading is implemented.
*
* @param	pos	The initial position in world coordinates
* @param	i	The index in the list of power-up metadata
*
* @return  An autoreleased physics object
*/
Powerup* Powerup::create(const Vec2& pos, const PowerupType& t, int i) {
	Powerup* p = new (std::nothrow) Powerup();
	if (p && p->init(pos, t, i, true)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return nullptr;
};

/** Initializes a new power-up object with the supplied position and index. */
bool Powerup::init(const Vec2& pos, const PowerupType& t, int i, bool resetDrawScale) {
	if (CapsuleObstacle::init(pos, resetDrawScale)) {
		_type = t;
		_index = i;
		setDensity(0.0f);
		setFriction(0.0f);
		setFixedRotation(true);
		return true;
	}
	return false;
};