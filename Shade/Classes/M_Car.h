#ifndef __M_CAR_H__
#define __M_CAR_H__

#include <map>
#include <string>
#include <cocos2d.h>
#include <cornell.h>
//#include "M_MovingObject.h"

#define CAR_SCALE_DOWN 5.0f

#define CAR_DENSITY 100.0f
#define CAR_FRICTION 0.5f
#define CAR_RESTITUTION 0.0f // TODO increase this when latching is implemented

/** Animation constants */
#define CAR_ROWS 5
#define CAR_COLS 2

using namespace cocos2d;

struct Car {
	typedef enum ActionType { GO, STOP, TURN_LEFT, TURN_RIGHT } ActionType;
	static const map<std::string, ActionType> actionMap;
	ActionType actionType;

	static const std::string name;

	static void act(ActionType action, int actionLength, int actionCounter, BoxObstacle* object, BoxObstacle* shadow);
};

#endif /* __M_CAR_H__ */