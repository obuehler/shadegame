#include "M_ActionQueue.h"

/** Sets tail of queue to the correct node */
void ActionQueue::resetTail() {
	assert(_head == _initialHead);
	if (_tail != nullptr) { // If _tail == nullptr, _head must be nullptr
		_tail = _head;
	}
	while (tailHasNext() && !tailLinksTo(_head)) bumpTail();
}

/**
* Moves the head to the next ActionNode, and the tail to the head
* if and only if tail has a neighbor. Ownership of the original head
* is transferred to the caller.
*
* @friendof ActionNode
* @release	The original head of the queue.
* @return	The original head of the queue.
*/
shared_ptr<ActionQueue::ActionNode> ActionQueue::next() {
	if (_head == nullptr) {
		return _head;
	}
	else {
		shared_ptr<ActionNode> action(_head);

		// make the head of the default cycle the tail
		if (tailLinksTo(_head)) {
			bumpTail();
		}
		// set _tail to null if _head was the only element and did
		// not point to itself
		else if (_tail == _head) {
			_tail = nullptr;
		}
		return bumpHead();
	}
}

/** Pushes a series of actions onto the queue. */
void ActionQueue::concat(const ActionQueue& actions) {
	if (_head == nullptr) {
		reinitialize(actions);
	}
	else {
		if (actions.tailHasNext()) {
			_initialHead = actions._head;
		}
		setTailNext(actions._head);
		_tail = actions._tail;
	}
}

/** Pushes the given node onto the queue. */
void ActionQueue::pushNode(ActionNode& action) {
	if (_head == nullptr) {
		initialize(action);
	}
	else {
		pushOne(action);
	}
}

/** For use by the AI controller. Pushes a series of actions
* to the front of the queue. Does not link _tail back to the new head even
* if the list is cyclic, as that is not desired behavior. We want the
* default cycle (if one exists) to continue in the same way after the
* inserted actions are executed. If there are other actions to be
* executed before the default cycle, they are purged.
*
* @param fromBeginning	Whether the queue should return to the initial head
*						after finishing the forced section or continue from
*						where it left off.
*/
void ActionQueue::force(const ActionQueue& actions, bool fromBeginning) {
	if (_head == nullptr) { // This queue is empty
		reinitialize(actions);
	}
	else if (actions.tailHasNext()) { // actions is cyclic

									  // Retain _initialHead to check for circular references. Wrap a copy in
									  // a new ActionQueue in order to manipulate _next without declaring an
									  // additional friend function or changing _next's access specifier,
									  // through the setTailNext() function which is a friend of ActionNode.
		ActionQueue temp(_initialHead);

		reinitialize(actions);
		int useCount = temp._head.use_count();
		if (useCount > 3) {
			// There is a cycle.
			// Make sure the implementation is correct and the manager object
			// for the original _initialHead is only being pointed to by temp
			// and the ActionNode at the end of the cycle.
			assert(useCount == 4);

			// Point _next elsewhere. Since no two different queues can
			// have the same ActionNode instance, all descendants of temp's
			// ActionNode will be destroyed. temp's ActionNode will then be
			// destroyed itself when temp goes out of scope.
			temp.setTailNext(nullptr);
		}
	}
	else {
		if (!fromBeginning) {  // Do not replace already forced actions
			_initialHead = _head;
		}
		actions.setTailNext(_initialHead);
		_head = actions._head;
		if (_tail == nullptr) _tail = actions._tail;
	}
}

/** Returns whether _tail points to another ActionNode. */
const bool ActionQueue::tailHasNext() const {
	if (_tail != nullptr && _tail->_next != nullptr) {
		/* cout << (_tail->_next == _head) << endl;
		cout << (_tail->_next == _initialHead) << endl; */
		//assert(_tail->_next == _initialHead);  TODO uncomment
		return true;
	}
	return false;
}

/** Returns to the default action pattern. */
void ActionQueue::reset() {
	_head = _initialHead;
	resetTail();
}

/** Empties the queue and reinitializes it with the supplied action.
*
* @param	actionPtr	Points to the action to reinitialize the queue with
*/
void ActionQueue::initialize(shared_ptr<ActionNode> actionPtr) {
	// Initialize the shared pointers
	_head = shared_ptr<ActionNode>(actionPtr);  // copies actionPtr
	_tail = shared_ptr<ActionNode>(actionPtr);  // copies actionPtr
	_initialHead = shared_ptr<ActionNode>(actionPtr);  // copies actionPtr

													   // Correct the tail
	resetTail();
}

/** Empties the queue and reinitializes it with the supplied action.
*
* @param	action		The action to reinitialize the queue with
*/
void ActionQueue::initialize(ActionNode& action) {
	// Initialize the shared pointers
	_head = make_shared<ActionNode>(action);
	_tail = shared_ptr<ActionNode>(_head);  // copies _head
	_initialHead = shared_ptr<ActionNode>(_head);  // copies _head

												   // Correct the tail
	resetTail();
}

/** Sets whether the queue cycles back around or not. */
void ActionQueue::setCycling(bool cycle) {
	if (cycle) {
		setTailNext(_head);
	}
	else {
		setTailNext(nullptr);
	}
}

/**
* Bump the head forward by 1 and return the original head.
* Useful when calling next().
*
* @friendof ActionNode
*/
shared_ptr<ActionQueue::ActionNode> ActionQueue::bumpHead() {
	shared_ptr<ActionNode> action(_head);
	_head = _head->_next;
	// Reset initialHead if about to be popped from the queue
	if (_initialHead == action && !tailLinksTo(action)) {
		_initialHead = _head;
	}
	return action;
}