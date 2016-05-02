//
//  CUAccelerationPoller.h
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
#ifndef __CU_ACCELERATION_POLLER_H__
#define __CU_ACCELERATION_POLLER_H__

#include <cocos2d.h>

NS_CC_BEGIN

/**
 * Class to providing a polling interface for the Accelerometer.
 *
 * Cocos2d decided to make all of its input event-driven. This is a major problem,
 * because it is hard to associate the events with the current animation frame,
 * potentially causing lag.  A poller is an alternative input interface that allows
 * us to query the current state of the keyboard when we need it.
 *
 * Because there is (theoretically) only one accelerometer, this class provides a 
 * singleton interface.  You can start and stop the interface with static methods.
 */
class CC_DLL AccelerationPoller {
protected:
    /** Singleton accleraton controller */
    static AccelerationPoller* _gAcceleration;
    /** Listener to process keyboard events */
    EventListenerAcceleration* _accelListener;
    /** The most recent acceleration value */
    Acceleration _acceleration;
    /** Whether this listener is active and receiving events */
    bool _active;
    
    
public:
#pragma mark Static Methods
    /**
     * Starts the input processing for this poller.
     *
     * This method will activate the singleton and assign it a priority.
     * The listener has fixed priority.
     *
     * @param  priority The input priority for the keyboard.
     */
    static void start(int priority);
    
    /**
     * Starts the input processing for this poller.
     *
     * This method will activate the singleton and assign it a priority.
     * The listener has scene priority.
     *
     * @param  node     The node for scene priority.
     */
    static void start(Node* node);
    
    /**
     * Stop the input processing for this input controller.
     *
     * This will deallocate the singleton, freeing memory.
     */
    static void stop();
    
    /**
     * Returns the singleton interface for the acceleration poller.
     *
     * It is unsafe to make your own acceleration poller.  Just use this static
     * method to access the singleton object.
     *
     * This method will return nullptr if the poller is not yet started.
     */
    static AccelerationPoller* getInstance();
    
    
#pragma mark Acceleration State
    /**
     * Returns whether this listener is active and receiving events
     *
     * Returns whether this listener is active and receiving events
     */
    bool isActive() const { return _active; }

    /**
     * Returns the most recent acceleration value.
     *
     * @return the most recent acceleration value.
     */
    const Acceleration& getAcceleration() const { return _acceleration; }
    
    /**
     * Callback for a change in accelerometer state
     *
     * @param acc   The accelerometer information
     * @param event The associated event
     */
    void accleromCB(Acceleration *acc, Event *event);


#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new (inactive) input controller for the accelerometer.
     */
    AccelerationPoller();
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~AccelerationPoller();

    /**
     * Initializes this listener, registering it to receive events.
     *
     * The listener has fixed priority.
     *
     * @param  priority The input priority for the listener.
     */
    void init(int priority);
    
    /**
     * Initializes this listener, registering it to receive events.
     *
     * The listener has scene priority.
     *
     * @param  node     The node for scene priority.
     */
    void init(Node* node);
    
    /**
     * Stops this listener, so that it no longer receives events
     */
    void dispose();
};


NS_CC_END

#endif /* defined(__CU_ACCELERATION_POLLER_H__) */
