#ifndef __M_CASTER_H__
#define __M_CASTER_H__

#include <map>
#include <string>
#include <cocos2d.h>
#include <cornell.h>
//#include "M_MovingObject.h"

using namespace cocos2d;
using namespace std;

struct Caster {
	typedef enum ActionType { GO, STOP, TURN_LEFT, TURN_RIGHT } ActionType;
	static map<string, ActionType> actionMap;
	ActionType actionType;

	static void act(ActionType, BoxObstacle*, BoxObstacle*);
};

#endif /* __M_CASTER_H__ */