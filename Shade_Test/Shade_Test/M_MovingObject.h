#ifndef __MOVING_OBJECT_H__
#define __MOVING_OBJECT_H__

//#include "cocos2d.h"
#include <tuple>
//#include <cornell/CUPolygonObstacle.h>     UNCOMMENT
#include "ActionQueuew.h"
#include "Runner.h"   // DELETE

using namespace std;

// T must be an enum, whose values all have a method named
// act(SimpleObstacle object, SimpleObstacle shadow).
//template <class T>    UNCOMMENT
class MovingObject {    // TODO friend of AIController

protected:

	// SimpleObstacle object;     UNCOMMENT
	// SimpleObstacle shadow;      UNCOMMENT

public:

	ActionQueue* _actionQueue;

	//MovingObject(MovingObjectMetadata* data) {} // TODO implement this    UNCOMMENT
	MovingObject() : _actionQueue(new ActionQueue()) {}

	/**
	* Executes the next move in the _actionQueue.
	*/
	void act() {
		if (!_actionQueue->isEmpty()) {
			while (!_actionQueue->isEmpty() && _actionQueue->_head->_counter <= 0) {
				//assert(_actionQueue->_head->_length > 0);    UNCOMMENT
				if (_actionQueue->tailHasNext()) {
					_actionQueue->_head->_counter = _actionQueue->_head->_length;
				}
				_actionQueue->next(); // The head of the queue gets popped if not cyclic
			}
			// Check to see if we are left with an empty queue
			if (!_actionQueue->isEmpty()) {
				/* The head of the queue */
				shared_ptr<ActionQueue::ActionNode> action(_actionQueue->_head);
				// TODO the act() method of action types take the current and remaining
				// number of frames as arguments
				//action->_type->act(action->_length - action->_counter, action->_counter);     UNCOMMENT
				action->_counter--;
			}
		}
	}

};

#endif /*__MOVING_OBJECT_H__*/

