//
//  RDRocketInput.h
//  RocketDemo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 4/2/16 (Renamed to prevent name collision)
//
#ifndef __RD_ROCKET_INPUT_H__
#define __RD_ROCKET_INPUT_H__

#include <cocos2d.h>
#include <cornell/CUKeyboardPoller.h>
#include <cornell/CUAccelerationPoller.h>
#include <cornell/CUTouchListener.h>

using namespace cocos2d;

#pragma mark Polled Input
/**
 * Class to represent player input in the rocket demo.
 *
 * This input handler uses the polling input API provided by the Cornell Extensions
 * (in contrast to the callback API required by Cocos2d).  However, there is some
 * callback functionality for touch support.  This allows us to handle gestures.
 *
 * Unlike handlers like the Keyboard poller, this class is not a singleton.  It
 * must be allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this controller
 * as a field without using pointers. We simply add the class to the header file
 * of its owner, and delay initialization (via the method start()) until later.
 * This is one of the main reasons we like to avoid initialization in the constructor.
 */
class RocketInput {
private:
    // KEYBOARD EMULATION
    /** Whether the up arrow key is down */
    bool  _keyUp;
    /** Whether the down arrow key is down */
    bool  _keyDown;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;

    // TOUCH SUPPORT
    /** The initial touch location for the current gesture */
    Vec2 _dtouch;
    /** The timestamp for the beginning of the current gesture */
    timestamp_t _timestamp;    

protected:
    // EVENT LISTENERS
    /** Listener to process touch events */
    TouchListener* _touchListener;
    
    /** Whether or not this controller is currently active. */
    bool _active;

    // Input results
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** How much did we move horizontally? */
    float _horizontal;
    /** How much did we move vertically? */
    float _vertical;
    
public:
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    RocketInput(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~RocketInput();
    
    /**
     * Initializes the input control for the given drawing scale.
     *
     * This method works like a proper constructor, initializing the input
     * controller and allocating memory.  However, it still does not activate
     * the listeners.  You must call start() do that.
     *
     * @return true if the controller was initialized successfully
     */
    bool init();
    
    /**
     * Starts the input processing for this input controller.
     *
     * This method must be called AFTER the input controller is initialized
     */
    void    start();
    
    /**
     * Stops the input processing for this input controller.
     *
     * This method will not dispose of the input controller. It can be restarted.
     */
    void    stop();
    
    /**
     * Processes the currently cached inputs.
     *
     * This method is used to to poll the current input state.  This will poll the
     * keyboad and accelerometer.
     * 
     * This method also gathers the delta difference in the touches. Depending on 
     * the OS, we may see multiple updates of the same touch in a single animation
     * frame, so we need to accumulate all of the data together.
     */
    void  update(float dt);

    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getHorizontal() const { return _horizontal; }
    
    /**
     * Returns the amount of vertical movement.
     *
     * -1 = down, 1 = up, 0 = still
     *
     * @return the amount of vertical movement.
     */
    float getVertical() const { return _vertical; }
    
    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
    bool didReset() const { return _resetPressed; }
    
    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didDebug() const { return _debugPressed; }
    
    /**
     * Returns true if the exit button was pressed.
     *
     * @return true if the exit button was pressed.
     */
    bool didExit() const { return _exitPressed; }
    
    
#pragma mark -
#pragma mark Touch Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     *
     * @return True if the touch was processed; false otherwise.
     */
    bool    touchBeganCB(Touch* t, timestamp_t time);
    
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void    touchEndedCB(Touch* t, timestamp_t time);
    
    /**
     * Callback for a touch movement event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void    touchMovedCB(Touch* t, timestamp_t time);
    
    /**
     * Callback for the cancellation of a touch event
     *
     * Cancellation occurs when an external event—for example, 
     * an incoming phone call—disrupts the current app’s event 
     * processing.
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void    touchCancelCB(Touch* t, timestamp_t time);
};

#endif /* defined(__RD_ROCKET_INPUT_H__) */