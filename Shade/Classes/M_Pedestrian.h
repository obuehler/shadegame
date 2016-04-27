#ifndef __M_PEDESTRIAN_H__
#define __M_PEDESTRIAN_H__

#include <map>
#include <string>
#include <cocos2d.h>
#include <cornell.h>

#define PEDESTRIAN_SCALE_DOWN 8.0f

using namespace std;
using namespace cocos2d;

struct Pedestrian {
	typedef enum ActionType { WALK_FAST, WALK_SLOW, STAND, LOOK_AROUND } ActionType;

	static const map<string, ActionType> actionMap;

	static void act(ActionType, BoxObstacle*, BoxObstacle*) {} // TODO define this
};

#endif /* __M_PEDESTRIAN_H__ */
