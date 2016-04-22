//
//  CUMultiTouchListener.h
//  Cornell Extensions to Cocos2D
//
//  Module provides a time sensitive listener for the touch input.  This class simplifies
//  a lot of the overhead for EventListenerTouchAllAtOnce.  More importantly, it adds a
//  timestamp to all of the callback functions so that the user can precisely time the
//  callbacks.
//
//  Author: Walker White
//  Version: 12/5/15
//
#include "CUMultiTouchListener.h"

NS_CC_BEGIN


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new input controller for the default dispatcher.
 *
 * @retain  a reference to the default event dispatcher
 * @return  An autoreleased touch listener
 */
MultiTouchListener* MultiTouchListener::create() {
    MultiTouchListener* listener = new (std::nothrow) MultiTouchListener();
    if (listener && listener->init()) {
        listener->autorelease();
        return listener;
    }
    CC_SAFE_DELETE(listener);
    return nullptr;
}

/**
 * Creates a new input controller for the given dispatcher.
 *
 * @param dispatcher Event handler for callbacks.
 *
 * @retain  a reference to the event dispatcher
 * @return  An autoreleased touch listener
 */
MultiTouchListener* MultiTouchListener::create(EventDispatcher* dispatcher) {
    MultiTouchListener* listener = new (std::nothrow) MultiTouchListener();
    if (listener && listener->init(dispatcher)) {
        listener->autorelease();
        return listener;
    }
    CC_SAFE_DELETE(listener);
    return nullptr;
}


#pragma mark -
#pragma mark Event Handling
/**
 * Starts this listener, registering it to receive events.
 *
 * The listener has fixed priority.
 *
 * @param  priority The input priority for the listener.
 */
void MultiTouchListener::start(int priority) {
    CCASSERT(!_active, "Listener is already active");
    _dispatcher->addEventListenerWithFixedPriority(_touchListener,priority);
    _active = true;
}

/**
 * Starts this listener, registering it to receive events.
 *
 * The listener has scene priority.
 *
 * @param  node     The node for scene priority.
 */
void MultiTouchListener::start(Node* node) {
    CCASSERT(!_active, "Listener is already active");
    _dispatcher->addEventListenerWithSceneGraphPriority(_touchListener, node);
    _active = true;
}

/**
 * Stops this listener, so that it no longer receives events
 */
void MultiTouchListener::stop() {
    CCASSERT(_active, "Listener is not active");
    _dispatcher->removeEventListener(_touchListener);
    _active = false;
}


#pragma mark -
#pragma mark Initializers
/**
 * Deallocates the touch listener, releasing all memory.
 */
MultiTouchListener::~MultiTouchListener() {
    if (_active) {
        stop();
    }
    _touchListener->release();
    _touchListener = nullptr;
    _dispatcher->release();
    _dispatcher = nullptr;
}

/**
 * Creates a new input controller for the given dispatcher.
 *
 * @param dispatcher Event handler for callbacks.
 *
 * @retain  a reference to the event dispatcher
 * @return true if the listener is initialized properly, false otherwise.
 */
bool MultiTouchListener::init(EventDispatcher* dispatcher) {
    _dispatcher = dispatcher;
    _dispatcher->retain();
    
    // The mouse listener
    _touchListener = cocos2d::EventListenerTouchAllAtOnce::create();
    _touchListener->retain();
    _touchListener->onTouchesBegan = [this](const std::vector<Touch*>& touches, Event* event) {
        if (onTouchesBegan !=  nullptr) {
            onTouchesBegan(touches,current_time());
        }
    };
    _touchListener->onTouchesMoved = [this](const std::vector<Touch*>& touches, Event* event) {
        if (onTouchesMoved !=  nullptr) {
            onTouchesMoved(touches,current_time());
        }
    };
    _touchListener->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event* event) {
        if (onTouchesEnded !=  nullptr) {
            onTouchesEnded(touches,current_time());
        }
    };
    _touchListener->onTouchesCancelled = [this](const std::vector<Touch*>& touches, Event* event) {
        if (onTouchesCancelled !=  nullptr) {
            onTouchesCancelled(touches,current_time());
        }
    };
    
    return true;
}

NS_CC_END