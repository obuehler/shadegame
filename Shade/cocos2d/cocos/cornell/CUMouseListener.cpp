//
//  CUMouseListener.h
//  Cornell Extensions to Cocos2D
//
//  Module provides a time sensitive listener for the mouse.  This class simplifies a
//  lot of the overhead for EventMouseLister.  More importantly, it adds a timestamp
//  to all of the callback functions so that the user can precisely time the callbacks.
//
//  Author: Walker White
//  Version: 12/5/15
//
#include "CUMouseListener.h"

NS_CC_BEGIN


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new input controller for the default dispatcher.
 *
 * @return  An autoreleased mouse listener
 */
MouseListener* MouseListener::create() {
    MouseListener* listener = new (std::nothrow) MouseListener();
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
 * @retain a reference to the event dispatcher
 * @return  An autoreleased mouse listener
 */
MouseListener* MouseListener::create(EventDispatcher* dispatcher) {
    MouseListener* listener = new (std::nothrow) MouseListener();
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
void MouseListener::start(int priority) {
    CCASSERT(!_active, "Listener is already active");
    _dispatcher->addEventListenerWithFixedPriority(_mouseListener,priority);
    _active = true;
}

/**
 * Starts this listener, registering it to receive events.
 *
 * The listener has scene priority.
 *
 * @param  node     The node for scene priority.
 */
void MouseListener::start(Node* node) {
    CCASSERT(!_active, "Listener is already active");
    _dispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, node);
    _active = true;
}

/**
 * Stops this listener, so that it no longer receives events
 */
void MouseListener::stop() {
    CCASSERT(_active, "Listener is not active");
    _dispatcher->removeEventListener(_mouseListener);
    _active = false;
}


#pragma mark -
#pragma mark Initializers
/**
 * Deallocates the mouse listener, releasing all memory.
 */
MouseListener::~MouseListener() {
    if (_active) {
        stop();
    }
    _mouseListener->release();
    _mouseListener = nullptr;
    _dispatcher->release();
    _dispatcher = nullptr;
}

/**
 * Creates a new input controller for the given dispatcher.
 *
 * @param dispatcher Event handler for callbacks.
 *
 * @retain a reference to the event dispatcher
 * @return true if the listener is initialized properly, false otherwise.
 */
bool MouseListener::init(EventDispatcher* dispatcher) {
    _dispatcher = dispatcher;
    _dispatcher->retain();

    // The mouse listener
    _mouseListener = cocos2d::EventListenerMouse::create();
    _mouseListener->retain();
    _mouseListener->onMouseDown = [this](EventMouse* event) {
        if (onMouseDown !=  nullptr) {
            onMouseDown(event,current_time());
        }
    };
    _mouseListener->onMouseUp = [this](EventMouse* event) {
        if (onMouseUp !=  nullptr) {
            onMouseUp(event,current_time());
        }
    };
    _mouseListener->onMouseMove = [this](EventMouse* event) {
        if (onMouseMove !=  nullptr) {
            onMouseMove(event,current_time());
        }
    };
    _mouseListener->onMouseScroll = [this](EventMouse* event) {
        if (onMouseScroll !=  nullptr) {
            onMouseScroll(event,current_time());
        }
    };
    
    return true;
}


NS_CC_END