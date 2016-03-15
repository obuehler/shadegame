/* Template for a moving object. */

#ifndef __MOVING_OBJECT_H__
#define __MOVING_OBJECT_H__

#include "cocos2d.h"
#include <tuple>
#include <cornell/CUPolygonObstacle.h>
#include <memory>

using namespace std;

template <class T>
class MovingObject : public PolygonObstacle {
public:
	struct ActionNode {
		int _id;
		T::ActionType _type;
		int _length;
		int _counter;
		shared_ptr<ActionNode> _next;

		/** Empties all fields and returns the pointer to this node for
		 * chaining. Use when deleting the node. */
		~ActionNode() {
			_id = NULL;
			_type = NULL;
			_length = NULL;
			_counter = NULL;

			// Do not delete _next, set it to nullptr instead.
			// This is what shared_ptrs are for - the object pointed to will be
			// destroyed automatically if there is nothing pointing to it.
			_next = nullptr;  // THIS SHOULD COME LAST FOR TAIL RECURSION.
		}
	};

	/** Linked list of ActionNodes. Manipulated by the AI Controller. */
	class ActionQueue {
	private:
		shared_ptr<ActionNode> _head;
		shared_ptr<ActionNode> _tail;

		// The head of the default cycle
		shared_ptr<ActionNode> _initialHead;

		ActionQueue() {}

	public:
		/** Creates a new ActionQueue from the given action chain. */
		static ActionQueue* create(ActionNode* action) {
			ActionQueue* queue = new (nothrow) ActionQueue();
			
			// Initialize the shared pointers
			queue->_head(action);
			// Copy these from queue_head so they point to the same shared pointer
			queue->_tail(queue->_head);
			queue->_initialHead(queue->_head);

			// Correct the tail
			queue->resetTail();

			return queue;
		}

		/** Creates a new ActionQueue from the given action chain that already
		 * survives in a shared pointer. */
		static ActionQueue* create(shared_ptr<ActionNode> actionPtr) {
			ActionQueue* queue = new (nothrow) ActionQueue();

			// Initialize the shared pointers
			queue->_head(actionPtr);
			queue->_tail(actionPtr);
			queue->_initialHead(actionPtr);

			// Correct the tail
			queue->resetTail();

			return queue;
		}

		/** Returns the head. */
		shared_ptr<ActionNode> const getHead() {
			return _head;
		}

		/** Sets tail to the correct node */
		void resetTail() {
			while (_tail->_next && _tail->_next != _head && _tail->_next != _initialHead) _tail = _tail->_next;
		}

		/** To be used if actions cycle back around. */
		void next() {
			_tail->_next = _head;
			_head = _head->_next;
			_tail = _tail->_next;
			_tail->_next = nullptr;
		}

		/** If the returned action is useless, it must be deleted by caller. */
		ActionNode* pop() {
			ActionNode* action = _head;
			_head = action->_next;
			return action;
		}

		void push(ActionNode* action) {
			_tail->_next = action;
			_tail = _tail->_next;
		}

		/** For use by the AI controller. Pushes another series of actions
		 * to the front of the queue. Does not link _tail back to the new head even
		 * if the list is cyclic, as that is not desired behavior. We want the
		 * default cycle (if one exists) to continue in the same way after the
		 * inserted actions are executed. If there are other actions to be
		 * executed before the default cycle, they are purged. */
		void force(ActionQueue& actions) {
			_head = actions->_head;

			// Link the added actions to the default cycle
			if (!(actions->_tail->_next)) actions->_tail->_next = _initialHead;
		}

		/** Returns to the default action pattern. */
		void reset() {
			_head = _initialHead;
		}

		/** Empties the queue and reinitializes it with the supplied action.
		 *
		 * @param	action		The action to reinitialize the queue with
		 */
		void reinitialize(ActionNode* action) {
			_head = action;
			_tail = action;
			resetTail();
		}

	};

	ActionQueue* actionQueue;

	/** Whether the actions in actionQueue cycle back */
	bool cycleActions;


	/**
	* Executes the next move in the actionQueue.
	*/
	void act() {
		if (actionQueue->_head) {
			ActionNode* initialHead = actionQueue->_head;
			bool headProcessed = false;
			while (actionQueue->_head->_counter <= 0) {
				if (cycleActions) {

					// No action is taken if all action lengths are somehow 0 or less.
					// Added to avoid being stuck in a while loop
					if (headProcessed && actionQueue->_head == initialHead) return;

					headProcessed = true;
					actionQueue->_head->_counter = actionQueue->_head->_length;
					actionQueue->next();
				}
				else {
					CC_SAFE_DELETE(actionQueue->pop());
				}
			}
			/* The head of the queue */
			ActionNode* action = actionQueue->_head;
			// TODO the act() method of action types take the current and remaining
			// number of frames as arguments
			action->_type->act(action->_length - action->_counter, action->_counter);
			action->_counter--;
		}
	}

	/**
	* Static constructor with position and index. Objects should be created with
	* this method once dynamic level loading is implemented.
	*
	* @param	pos	The initial position in world coordinates
	* @param	i	The index in the list of power-up metadata
	*
	* @return  An autoreleased physics object
	*/
	static Powerup* create(const Vec2& pos, const PowerupType& t, int i = 0);

	/** Initializes a new power-up object with the supplied position and index. */
	bool init(const Vec2& pos, const PowerupType& t, int i);

	/** Returns the type */
	PowerupType getType() const { return _type; };

	/** Sets the type */
	void setType(PowerupType t) { _type = t; };

};

#endif /*__MOVING_OBJECT_H__*/
