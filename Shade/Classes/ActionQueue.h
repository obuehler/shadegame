#ifndef __ACTION_QUEUE_H__
#define __ACTION_QUEUE_H__

#include <memory>
#include <cassert>
#include <iostream>
#include "cocos2d.h"

#define DEFAULT_ACTION_LENGTH 1
#define DEFAULT_BEARING 0.0f

using namespace std;

/** Linked list of ActionNodes. Manipulated by the AI Controller. */

template <class T>
class OurMovingObject;

template <class T>
class ActionQueue : public Ref {

	CC_DISALLOW_COPY_AND_ASSIGN(ActionQueue<T>);

private:
	class ActionNode;

public:

	typedef typename T::ActionType ActionType;

	friend class AIController;

	friend class OurMovingObject<T>;
	friend class GameController;

/*
	void printContents() {
		cout << "head is ";
		if (_head == nullptr) {
			cout << "nullptr " << endl;
		}
		else {
			cout << _head->_type << endl;
		}
		cout << "tail is ";
		if (_tail == nullptr) {
			cout << "nullptr " << endl;
		}
		else {
			cout << _tail->_type << endl;
		}
		cout << "initialHead is ";
		if (_initialHead == nullptr) {
			cout << "nullptr " << endl;
		}
		else {
			cout << _initialHead->_type << endl;
		}
		shared_ptr<ActionNode> action(_head);
		while (action != _tail) {
			cout << action->_type << "  ";
			action = action->_next;
		}
		if (action != nullptr) {
			assert(action->_next == nullptr || action->_next == _initialHead);
			cout << action->_type << endl;
		}
		cout << "-------------------------------" << endl;
	}
*/
	
	ActionQueue<T>() : _head(nullptr), _tail(nullptr), _initialHead(nullptr) {}

	static ActionQueue<T> * create() {
		ActionQueue<T>* q = new (std::nothrow) ActionQueue<T>();
		if (q && q->init()) {
			q->autorelease();
			return q;
		}
		CC_SAFE_DELETE(q);
		return nullptr;
	}

	static ActionQueue<T> * create(ActionNode& action) {
		ActionQueue<T>* q = new (std::nothrow) ActionQueue<T>();
		if (q && q->initialize(action)) {
			q->autorelease();
			return q;
		}
		CC_SAFE_DELETE(q);
		return nullptr;
	}

	static ActionQueue<T> * create(shared_ptr<ActionNode> actionPtr) {
		ActionQueue<T>* q = new (std::nothrow) ActionQueue<T>();
		if (q && q->initialize(actionPtr)) {
			q->autorelease();
			return q;
		}
		CC_SAFE_DELETE(q);
		return nullptr;
	}

	/** Creates a copy of queue with new copies of all action nodes */
	static ActionQueue<T> * create(ActionQueue<T>& queue) {
		ActionQueue<T>* q = new (std::nothrow) ActionQueue<T>();
		if (q && q->init(queue)) {
			q->autorelease();
			return q;
		}
		CC_SAFE_DELETE(q);
		return nullptr;
	}

	/** Returns whether the queue is empty by checking if _head is nullptr. */
	bool isEmpty() {
		return _head == nullptr;
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
	shared_ptr<ActionNode> next() {
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
	void concat(const ActionQueue<T>& actions) {
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

	/** Pushes a copy of an action onto the queue. The copy does not
	* preserve the _next attribute, it sets it to nullptr. */
	void pushCopy(ActionNode action) {
		pushNode(make_shared<ActionNode>(action));
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(ActionType type, int length) {
		pushNode(make_shared<ActionNode>(ActionNode(type, length)));
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(ActionType type) {
		pushNode(make_shared<ActionNode>(ActionNode(type)));
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(ActionType type, int length, int counter) {
		pushNode(make_shared<ActionNode>(ActionNode(type, length, counter)));
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(ActionType type, int length, int counter, float heading) {
		pushNode(make_shared<ActionNode>(ActionNode(type, length, counter, heading)));
	}

	/** Pushes the given node onto the queue. */
	void pushNode(shared_ptr<ActionNode> action) {
		if (isEmpty()) {
			initialize(action);
		}
		else {
			setTailNext(action);
			bumpTail();
		}
	}

	/** Reinitializes the queue from the queue supplied. */
	void reinitialize(const ActionQueue<T>& actions) {
		_head = actions._head;
		_tail = actions._tail;
		_initialHead = actions._initialHead;
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
	void force(const ActionQueue<T>& queue, bool fromBeginning) {
		ActionQueue<T> actions;
		actions.init(queue);
		if (_head == nullptr) { // This queue is empty
			reinitialize(actions);
		}
		else if (actions.tailHasNext()) { // actions is cyclic

			// Retain _initialHead to check for circular references. Wrap a copy in
			// a new action queue in order to manipulate _next without declaring an
			// additional friend function or changing _next's access specifier,
			// through the setTailNext() function which is a friend of ActionNode.
			ActionQueue<T> temp;
			temp.pushNode(_initialHead);

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

	/** Returns to the default action pattern. */
	void reset() {
		_head = _initialHead;
		resetTail();
	}

	/** Sets whether the queue cycles back around or not. */
	void setCycling(bool cycle) {
		if (cycle) {
			setTailNext(_head);
		}
		else {
			setTailNext(nullptr);
		}
	}

private:

	shared_ptr<ActionNode> _head;
	shared_ptr<ActionNode> _tail;

	// The head of the default cycle
	shared_ptr<ActionNode> _initialHead;

	/** Sets tail of queue to the correct node */
	void resetTail() {
		assert(_head == _initialHead);
		if (_tail != nullptr) { // If _tail == nullptr, _head must be nullptr
			_tail = _head;
		}
		while (tailHasNext() && !tailLinksTo(_head)) bumpTail();
	}

	/**
	* Checks if _tail->_next is _head.
	*
	* @friendof ActionNode
	*/
	bool tailLinksTo(shared_ptr<ActionNode> action) {
		return _tail != nullptr &&_tail->getNext() == action;
	}

	// INITIALIZATION

	bool init() {
		_head = nullptr;
		_tail = nullptr;
		_initialHead = nullptr;
		return true;
	}

	bool init(const ActionQueue<T>& actions) {
		assert((actions._initialHead == nullptr) == (actions._head == nullptr));
		if (init()) {
			shared_ptr<ActionNode> current = actions._head;
			bool initialHeadSet = false;
			while (current != nullptr) {
				if (!initialHeadSet) {
					pushCopy(*current);
					if (current == actions._initialHead) {
						_initialHead = _tail;
						initialHeadSet = true;
					}
				}
				else if (current == actions._initialHead) {
					setTailNext(_initialHead);
					return true;
				}
				else {
					pushCopy(*current);
				}
				current = current->getNext();
			}
			return true;
		}
		return false;
	}

	/** Empties the queue and reinitializes it with the supplied action.
	*
	* @param	actionPtr	Points to the action to reinitialize the queue with
	*/
	bool initialize(shared_ptr<ActionNode> actionPtr) {
		// Initialize the shared pointers
		_head = shared_ptr<ActionNode>(actionPtr);  // copies actionPtr
		_tail = shared_ptr<ActionNode>(actionPtr);  // copies actionPtr
		_initialHead = shared_ptr<ActionNode>(actionPtr);  // copies actionPtr
		//
		// Correct the tail
		resetTail();
		return true;
	}

	/** Empties the queue and reinitializes it with the supplied action.
	*
	* @param	action		The action to reinitialize the queue with
	*/
	bool initialize(ActionNode& action) {
		// Initialize the shared pointers
		_head = make_shared<ActionNode>(action);
		_tail = shared_ptr<ActionNode>(_head);  // copies _head
		_initialHead = shared_ptr<ActionNode>(_head);  // copies _head
		//
		// Correct the tail
		resetTail();
		return true;
	}

	/**
	* Bump the head forward by 1 and return the original head.
	* Useful when calling next().
	*
	* @friendof	ActionNode
	* @return	the first element in the queue
	*/
	shared_ptr<ActionNode> bumpHead() {
		shared_ptr<ActionNode> action(_head);
		_head = _head->getNext();
		// Reset initialHead if about to be popped from the queue
		if (_initialHead == action && !tailLinksTo(action)) {
			_initialHead = _head;
		}
		return action;
	}

	/**
	* Bump the tail forward by 1. Useful when calling next() on a cyclic queue
	* and repointing the tail to the proper ActionNode using resetTail().
	*
	* @friendof ActionNode
	*/
	void bumpTail() { _tail = _tail->getNext(); }

	/**
	* Sets _tail->_next for cycling purposes.
	*
	* @friendof ActionNode
	*/
	void setTailNext(shared_ptr<ActionNode> next) { 
		if (_tail != nullptr) {
			_tail->_next = next;
		}
		else {
			initialize(next);
		}
	}

	/** Returns whether _tail points to another ActionNode. */
	const bool tailHasNext() const {
		if (_tail != nullptr && _tail->getNext() != nullptr) {
			/* cout << (_tail->_next == _head) << endl;
			cout << (_tail->_next == _initialHead) << endl; */
			//assert(_tail->_next == _initialHead);  TODO uncomment
			return true;
		}
		return false;
	}

	class ActionNode {

	private:
		shared_ptr<ActionNode> _next;
	public:

		friend void ActionQueue<T>::setTailNext(shared_ptr<ActionNode> next);

		ActionType _type;
		int _length;
		int _counter;
		float _bearing;

		// CONSTRUCTORS

		ActionNode(ActionType type) : _length(DEFAULT_ACTION_LENGTH), _counter(
			DEFAULT_ACTION_LENGTH), _type(type), _next(nullptr
				), _bearing(0.0f) {}

		ActionNode(ActionType type, int length, int counter) : _type(type
			), _length(length), _counter(counter), _next(nullptr), _bearing(
				DEFAULT_BEARING) {}

		ActionNode(ActionType type, int length) : _counter(length), _type(
			type), _length(length), _next(nullptr), _bearing(DEFAULT_BEARING) {}

		ActionNode(ActionType type, int length, float bearing) : _counter(length
			), _type(type), _length(length), _next(nullptr), _bearing(bearing) {}

		ActionNode(ActionType type, int length, int counter, float bearing
			) : _counter(counter), _type(type), _length(length), _next(nullptr
				), _bearing(bearing) {}

		ActionNode(const ActionNode& action) : _counter(action._counter
			), _type(action._type), _length(action._length), _next(nullptr
				), _bearing(action._bearing) {}


		/** Empties all fields and returns the pointer to this node for
		* chaining. Use when deleting the node. */
		~ActionNode() {
			//_type = NULL;
			_length = NULL;
			_counter = NULL;
			_bearing = NULL;
			// Do not delete _next, set it to nullptr instead.
			// This is what shared_ptrs are for - the object pointed to will be
			// destroyed automatically if there is nothing pointing to it.
			_next = nullptr;  // THIS SHOULD COME LAST FOR TAIL RECURSION.
		}

		const shared_ptr<ActionNode> getNext() const { return _next; }
	};

};

#endif /* __ACTION_QUEUE_H__ */
