//
//  CUAccelerationPoller.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a polling interface for the Accelerometer.  Cocos2d decided
//  to make all of its input event-driven. This is a major problem, because it is hard
//  to associate the events with the current animation frame, potentially causing
//  lag.  A poller is an alternative input interface that allows us to query the
//  current state of the keyboard when we need it.
//
//  Author: Walker White
//  Version: 12/5/15
//
#include "CUAccelerationPoller.h"

NS_CC_BEGIN

/** Initialization of static reference */
AccelerationPoller* AccelerationPoller::_gAcceleration = nullptr;

#pragma mark -
#pragma mark Initializers
/**
 * Creates a new (inactive) input controller for the accelerometer.
 */
AccelerationPoller::AccelerationPoller() : _active(false) {
    // Create the accelerometer listener. This is an autorelease object.
    _accelListener = EventListenerAcceleration::create(CC_CALLBACK_2(AccelerationPoller::accleromCB, this));
    _accelListener->retain();
}

/**
 * Disposes of this input controller, releasing all listeners.
 */
AccelerationPoller::~AccelerationPoller() {
    if (_active) {
        dispose();
    }
    _accelListener->release();
    _accelListener = nullptr;
}

/**
 * Initializes this listener, registering it to receive events.
 *
 * The listener has fixed priority.
 *
 * @param  priority The input priority for the listener.
 */
void AccelerationPoller::init(int priority) {
    CCASSERT(!_active, "Listener is already active");
    // This makes the accelerometer A LOT less laggy (if only we could do this for touch)
    Device::setAccelerometerEnabled(true);
    Device::setAccelerometerInterval(1.0f/60);
    
    // Register the listeners
    // BUG: Cocos2D always prioritizes touch
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithFixedPriority(_accelListener,priority);
    _active = true;
}

/**
 * Initializes this listener, registering it to receive events.
 *
 * The listener has scene priority.
 *
 * @param  node     The node for scene priority.
 */
void AccelerationPoller::init(Node* node) {
    CCASSERT(!_active, "Listener is already active");
    // This makes the accelerometer A LOT less laggy (if only we could do this for touch)
    Device::setAccelerometerEnabled(true);
    Device::setAccelerometerInterval(1.0f/60);
    
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(_accelListener,node);
    _active = true;
}

/**
 * Stops this listener, so that it no longer receives events
 */
void AccelerationPoller::dispose() {
    CCASSERT(_active, "Listener is not active");
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeEventListener(_accelListener);
    Device::setAccelerometerEnabled(false);
    _active = false;
}


#pragma mark -
#pragma mark Static Methods
/**
 * Starts the input processing for this poller.
 *
 * This method will activate the singleton and assign it a priority.
 * The listener has fixed priority.
 *
 * @param  priority The input priority for the keyboard.
 */
void AccelerationPoller::start(int priority) {
    if (_gAcceleration == nullptr) {
        _gAcceleration = new AccelerationPoller();
    }
    _gAcceleration->init(priority);
}

/**
 * Starts the input processing for this poller.
 *
 * This method will activate the singleton and assign it a priority.
 * The listener has scene priority.
 *
 * @param  node     The node for scene priority.
 */
void AccelerationPoller::start(Node* node) {
    if (_gAcceleration == nullptr) {
        _gAcceleration = new AccelerationPoller();
    }
    _gAcceleration->init(node);
}

/**
 * Stop the input processing for this input controller.
 *
 * This will deallocate the singleton, freeing memory.
 */
void AccelerationPoller::stop() {
    _gAcceleration->dispose();
    if (_gAcceleration != nullptr) {
        delete _gAcceleration;
        _gAcceleration = nullptr;
    }
}

/**
 * Returns the singleton interface for the keyboard poller.
 *
 * It is unsafe to make your own keyboard poller.  Just use this static method
 * to access the singleton object.
 *
 * This method will return nullptr if the poller is not yet started.
 */
AccelerationPoller* AccelerationPoller::getInstance() {
    return _gAcceleration;
}


#pragma mark -
#pragma mark Acceleration State

/**
 * Callback for a change in accelerometer state
 *
 * @param acc   The accelerometer information
 * @param event The associated event
 */
void AccelerationPoller::accleromCB(Acceleration *acc, Event *event) {
    _acceleration = *acc;
}

NS_CC_END