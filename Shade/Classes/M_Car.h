#ifndef __M_CAR_H__
#define __M_CAR_H__

#include <map>
#include <string>
#include <cocos2d.h>
#include <cornell.h>
//#include "M_MovingObject.h"

#define CAR_SCALE_DOWN 5.0f

using namespace cocos2d;
using namespace std;

struct Car {
	typedef enum ActionType { GO, STOP, TURN_LEFT, TURN_RIGHT } ActionType;
	static const map<string, ActionType> actionMap;
	ActionType actionType;

	static void act(ActionType, BoxObstacle*, BoxObstacle*);
};

#endif /* __M_CAR_H__ */