#ifndef __M_PEDESTRIAN_H__
#define __M_PEDESTRIAN_H__

#include <map>
#include <string>
#include <cocos2d.h>
#include <cornell.h>

using namespace std;
using namespace cocos2d;

struct Pedestrian {
	typedef enum ActionType { GO, STOP, TURN_LEFT, TURN_RIGHT, LOOK_AROUND, TO_SHADOW } ActionType;

	static map<string, ActionType> actionMap;
	static void act(ActionType, BoxObstacle*, BoxObstacle*);
};

#endif /* __M_PEDESTRIAN_H__ */
