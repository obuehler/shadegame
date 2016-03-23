#ifndef __M_ACTION_QUEUE_H__
#define __M_ACTION_QUEUE_H__

#include <memory>
#include <cassert>
#include <iostream>

#define DEFAULT_ACTION_LENGTH 0

using namespace std;

/** Linked list of ActionNodes. Manipulated by the AI Controller. */
class ActionQueue {
private:
	class ActionNode;

public:

	//friend class AIController;    UNCOMMENT
	friend class MovingObject;
	friend class Runner; // DELETE

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

	ActionQueue() : _head(nullptr), _tail(nullptr), _initialHead(nullptr) {}

	/** Creates a new ActionQueue from the given action chain. */
	ActionQueue(ActionNode& action) { initialize(action); }

	/** Creates a new ActionQueue from the given action chain that already
	* survives in a shared pointer. */
	ActionQueue(shared_ptr<ActionNode> actionPtr) {	initialize(actionPtr); }

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
	shared_ptr<ActionNode> next();

	/** Pushes a series of actions onto the queue. */
	void concat(const ActionQueue& actions);

	inline void pushOne(ActionNode& action) {
		setTailNext(make_shared<ActionNode>(action));
		bumpTail();
	}

	/** Pushes a copy of an action onto the queue. The copy does not
	* preserve the _next attribute, it sets it to nullptr. */
	void pushCopy(ActionNode action) {
		pushOne(action);
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(char type, int length) {  // TODO replace char with T
		pushNode(ActionNode(type, length));
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(char type) { // TODO replace char with T
		pushNode(ActionNode(type));
	}

	/** Constructs a new ActionNode with the given arguments and pushes it
	* onto the queue. */
	void push(char type, int length, int counter) { // TODO replace char with T
		pushNode(ActionNode(type, length, counter));
	}

	/** Pushes the given node onto the queue. */
	void pushNode(ActionNode& action);

	/** Reinitializes the queue from the queue supplied. */
	void reinitialize(const ActionQueue& actions) {
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
	void force(const ActionQueue& actions, bool fromBeginning);

	/** Returns to the default action pattern. */
	void reset();

	/** Sets whether the queue cycles back around or not. */
	void setCycling(bool cycle);

private:

	shared_ptr<ActionNode> _head;
	shared_ptr<ActionNode> _tail;

	// The head of the default cycle
	shared_ptr<ActionNode> _initialHead;

	/** Sets tail of queue to the correct node */
	void resetTail();

	/**
	 * Checks if _tail->_next is _head.
	 *
	 * @friendof ActionNode
	 */
	bool tailLinksTo(shared_ptr<ActionNode> action) { 
		return _tail != nullptr &&_tail->_next == action;
	}

	/** Empties the queue and reinitializes it with the supplied action.
	*
	* @param	actionPtr	Points to the action to reinitialize the queue with
	*/
	void initialize(shared_ptr<ActionNode> actionPtr);

	/** Empties the queue and reinitializes it with the supplied action.
	*
	* @param	action		The action to reinitialize the queue with
	*/
	void initialize(ActionNode& action);

	/**
	* Bump the head forward by 1 and return the original head.
	* Useful when calling next().
	*
	* @friendof ActionNode
	*/
	shared_ptr<ActionNode> bumpHead();

	/**
	 * Bump the tail forward by 1. Useful when calling next() on a cyclic queue
	 * and repointing the tail to the proper ActionNode using resetTail().
	 *
	 * @friendof ActionNode
	 */
	void bumpTail() { _tail = _tail->_next;	}

	/** 
	 * Sets _tail->_next for cycling purposes. 
	 *
	 * @friendof ActionNode
	 */
	void setTailNext(shared_ptr<ActionNode> next) const { _tail->_next = next; }

	/** Returns whether _tail points to another ActionNode. */
	const bool tailHasNext() const;

	class ActionNode {

	private:
		shared_ptr<ActionNode> _next;
	public:
		friend shared_ptr<ActionNode> ActionQueue::bumpHead();
		friend void ActionQueue::bumpTail();
		friend void ActionQueue::setTailNext(shared_ptr<ActionNode> next) const;
		friend bool ActionQueue::tailLinksTo(shared_ptr<ActionNode> action);
		friend const bool ActionQueue::tailHasNext() const;
		friend void ActionQueue::printContents();  // TODO remove this

		char _type;//T _type;   UNCOMMENT
		int _length;
		int _counter;

		ActionNode(char type) : _length(DEFAULT_ACTION_LENGTH), _counter(DEFAULT_ACTION_LENGTH), _type(type), _next(nullptr) { cout << _next << endl; }     // DELETE
		ActionNode(char type, int length, int counter) : _length(length), _counter(counter), _type(type), _next(nullptr) { }     // DELETE
		/*ActionNode(T type) : _length(0), _counter(
			0), _type(type), _next(nullptr) {}     UNCOMMENT */
		/* ActionNode(T type, int length, int counter) : _type(
			type), _length(length), _counter(counter), _next(nullptr) {}
		UNCOMMENT */

		ActionNode(char type, int length) : _counter(length
			), _type(type), _length(length), _next(nullptr) {}  // DELETE

		ActionNode(const ActionNode& action) : _counter(action._counter
			), _type(action._type), _length(action._length), _next(nullptr) {}


		/** Empties all fields and returns the pointer to this node for
		 * chaining. Use when deleting the node. */
		~ActionNode() {
			_type = NULL;
			_length = NULL;
			_counter = NULL;
			// Do not delete _next, set it to nullptr instead.
			// This is what shared_ptrs are for - the object pointed to will be
			// destroyed automatically if there is nothing pointing to it.
			_next = nullptr;  // THIS SHOULD COME LAST FOR TAIL RECURSION.
		}
	};

};

#endif /* __M_ACTION_QUEUE_H__ */
