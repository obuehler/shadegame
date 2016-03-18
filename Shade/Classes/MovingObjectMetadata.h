#ifndef __MOVING_OBJECT_METADATA_H__
#define __MOVING_OBJECT_METADATA_H__

#include <cocos2d>
#include <vector>
#include <string>

/** Holds the metadata to construct a MovingObject from a supplied level.
 * An instance is a field of a Level object. */
struct MovingObjectMetaData {

	/** Very similar in structure to ActionQueue::ActionNode, except it does
	 * not hold information for the next action. */
	struct ActionData {

		/** The individual moving object classes contain a map structure.
		 * This structure holds strings as keys and ActionType enum values
		 * as the associated values. This variable, hence its name, is the
		 * key value associated with the ActionType of the given action. */
		string typeKey;

		/** The length of the action in number of game frames */
		int length;

		/** The execution of the action will start with this many frames
		 * remaining. */
		int counter;

		/** The heading in which the action is completed */
		float bearing;
	};

	/** The initial position of the object in physics world coordinates */
	Vec2 position;
	
	// int assetIndex;  (the 3rd pedestrian image, 8th car image etc.)
	// TODO uncomment this when implemented

	/** Initial heading of the object. */
	float bearing;

	/** The vector object to be used to create the initial actionQueue */
	vector<ActionData> initialActions;
};

#endif /* __MOVING_OBJECT_METADATA_H__ */