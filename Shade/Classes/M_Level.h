#ifndef __M_LEVEL_H__
#define __M_LEVEL_H__

#include <tuple>

using namespace std;

struct Level {
	int levelIndex;
	tuple<float, float, float> _shadowPos;
	tuple<float, float, float> _casterPos;
	//MovingObjectMetadata* _movingObjects;  TODO uncomment this
	tuple<float, float, float> _coins;
	// TODO add powerups
};



#endif /* __M_LEVEL_H__ */