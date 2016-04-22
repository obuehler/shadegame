//
//  RGRagdollInput.h
//  RagdollDemo
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
#ifndef __RG_RAGDOLL_INPUT_H__
#define __RG_RAGDOLL_INPUT_H__

#include <cocos2d.h>
#include <cornell/CUKeyboardPoller.h>
#include <cornell/CUAccelerationPoller.h>
#include <cornell/CUMultiTouchListener.h>

using namespace cocos2d;

#pragma mark Polled Input
/**
 * Class to represent player input in the ragdoll demo.
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
class RagdollInput {
private:
    // KEYBOARD EMULATION
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;

    // TOUCH SUPPORT
    /** The drawing scale for this input handler (necessary to scale touch input) */
    Vec2 _scale;
    /** The UNSCALED touch location for the current (two-finger) gesture */
    Vec2 _dtouch;
    /** The SCALED touch location for the current (one-finger) selection */
    Vec2 _ctouch;
    /** Touches may not be simultaneous, so we should count them */
    int _tcount;
    /** The timestamp for the beginning of the current gesture */
    timestamp_t _timestamp;    

protected:
    // EVENT LISTENERS
    /** Listener to process touch events */
    MultiTouchListener* _touchListener;
    
    /** Whether or not this controller is currently active. */
    bool _active;

    // Input results
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** Are we registering an object selection? */
    bool _select;


public:
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    RagdollInput(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~RagdollInput();
    
    /**
     * Initializes the input control for the given drawing scale.
     *
     * This method works like a proper constructor, initializing the input
     * controller and allocating memory.  However, it still does not activate
     * the listeners.  You must call start() do that.
     *
     * The drawing scale is the difference in size between the layer (which
     * is receiving the touch) and the physics world.  This allows us to better
     * associate a touch with an object.
     *
     * @param  scale    the drawing scale
     *
     * @return true if the controller was initialized successfully
     */
    bool init(const Vec2& scale);
    
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
     * Returns the amount of vertical movement.
     *
     * -1 = down, 1 = up, 0 = still
     *
     * @return the amount of vertical movement.
     */
    bool didSelect() const { return _select; }
    
    /**
     * Returns the location (in scaled space) of the selection.
     *
     * @return the location (in scaled space) of the selection.
     */
    const Vec2& getSelection() const { return _ctouch; }
    
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
     * Called when a multitouch event first starts
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     *
     * @return true if the touch is consumed; false otherwise
     */
    bool    touchesBeganCB(const std::vector<Touch*>& touches, timestamp_t time);
    
    /**
     * Called when a multitouch event completes
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     */
    void    touchesEndedCB(const std::vector<Touch*>& touches, timestamp_t time);
    
    /**
     * Called when a multitouch event moves.
     *
     * If this event handler runs on a desktop with a mouse, this event
     * will only register if the mouse is moved while a button is held down.
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     */
    void    touchesMovedCB(const std::vector<Touch*>& touches, timestamp_t time);
    
    /**
     * Called when a multitouch event is aborted
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     */
    void    touchesCancelCB(const std::vector<Touch*>& touches, timestamp_t time);
};

#endif /* defined(__RG_RAGDOLL_INPUT_H__) */