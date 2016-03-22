//
//  PFInputController.h
//  PlatformerDemo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/15
//
#ifndef __C_INPUT_H__
#define __C_INPUT_H__

#include <cocos2d.h>
#include <cornell/CUKeyboardPoller.h>
#include <cornell/CUAccelerationPoller.h>
#include <cornell/CUTouchListener.h>


using namespace cocos2d;

#pragma mark -
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
class InputController {
private:
    // KEYBOARD EMULATION
    /** Whether the jump key is down */
    bool  _keyJump;
    /** Whether the fire key is down */
    bool  _keyFire;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    bool  _keyLeft;
    bool  _keyRight;
    bool  _keyUp;
    bool  _keyDown;
    
    
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
    /** Whether the fire action was chosen. */
    bool _firePressed;
    /** Whether the jump action was chosen. */
    bool _jumpPressed;
    /** How much did we move horizontally? */
    float _horizontal;
    /** How much did we move vertically? */
    float _vertical;
    
    
#pragma mark Internal Touch Management
    // The screen is divided into four zones: Left, Bottom, Right and Main/
    // These are all shown in the diagram below.
    //
    //   |---------------|
    //   |   |       |   |
    //   | L |   M   | R |
    //   |   |       |   |
    //   -----------------
    //   |       B       |
    //   -----------------
    //
    // The meaning of any touch depends on the zone it begins in.
    
    /** Information representing a single "touch" (possibly multi-finger) */
    struct TouchInstance {
        /** The current touch position */
        Vec2 position;
        /** The touch id for future reference */
        int  touchid;
        /** The number of fingers for this touch */
        int  count;
    };
    
    /** Enumeration identifying a zone for the current touch */
    enum class Zone {
        /** The touch was not inside the screen bounds */
        UNDEFINED,
        /** The touch was in the left zone (as shown above) */
        LEFT,
        /** The touch was in the right zone (as shown above) */
        RIGHT,
        /** The touch was in the bottom zone (as shown above) */
        BOTTOM,
        /** The touch was in the main zone (as shown above) */
        MAIN
    };
    
    /** The bounds of the entire game screen */
    Rect _bounds;
    /** The bounds of the left touch zone */
    Rect _lzone;
    /** The bounds of the right touch zone */
    Rect _rzone;
    /** The bounds of the bottom touch zone */
    Rect _bzone;
    
    // Each zone can have only one touch
    /** The current touch location for the left zone */
    TouchInstance _ltouch;
    /** The current touch location for the right zone */
    TouchInstance _rtouch;
    /** The current touch location for the main zone */
    TouchInstance _btouch;
    /** The current touch location for the bottom zone */
    TouchInstance _mtouch;
    
    TouchInstance _oneFinger;
    TouchInstance _twoFingers;
    
    /** The timestamp for the beginning of the current swipe gesture */
    timestamp_t _swipetime;
    /** The timestamp for a double tap (main zone only) */
    timestamp_t _dbtaptime;
    
    /**
     * Defines the zone boundaries, so we can quickly categorize touches.
     */
    void createZones();
    
    /**
     * Returns the correct zone for the given position.
     *
     * See the comments above for a description of how zones work.
     *
     * @param  pos  a position in screen coordinates
     *
     * @return the correct zone for the given position.
     */
    //Zone getZone(const Vec2& pos);
    
    /**
     * Returns true if this is a jump swipe.
     *
     * A jump swipe is a quick swipe up in either the left or right zone.
     *
     * @param  start    the start position of the candidate swipe
     * @param  stop     the end position of the candidate swipe
     * @param  current  the current timestamp of the gesture
     *
     * @return true if this is a jump swipe.
     */
    bool checkJump(const Vec2& start, const Vec2& stop, timestamp_t current);
    
    /**
     * Returns a nonzero value if this is a quick left or right swipe
     *
     * The function returns -1 if it is left swipe and 1 if it is a right swipe.
     *
     * @param  start    the start position of the candidate swipe
     * @param  stop     the end position of the candidate swipe
     * @param  current  the current timestamp of the gesture
     *
     * @return a nonzero value if this is a quick left or right swipe
     */
    int  checkSwipe(const Vec2& start, const Vec2& stop, timestamp_t current);
    
    
#pragma mark -
#pragma mark Input Control
public:
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    InputController(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController();
    
    /**
     * Initializes the input control for the given drawing scale and bounds.
     *
     * This method works like a proper constructor, initializing the input
     * controller and allocating memory.  However, it still does not activate
     * the listeners.  You must call start() do that.
     *
     * The drawing scale is the difference in size between the layer (which
     * is receiving the touch) and the physics world.  This allows us to better
     * associate a touch with an object.
     *
     * @param  bounds   the bounds of the touch device
     * @param  scale    the drawing scale
     *
     * @return true if the controller was initialized successfully
     */
    bool init(const Rect& bounds);
    
    void setZero() {
        _vertical = 0;
        _horizontal = 0;
    }
    
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
     * -1 = down, 1 = up, 0 = still
     * @return the amount of vertical movement.
     */
    float getVertical() const { return _vertical; }
    
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getHorizontal() const { return _horizontal; }
    
    /**
     * Returns if the jump button was pressed.
     *
     * @return if the jump button was pressed.
     */
    float didJump() const { return _jumpPressed; }
    
    /**
     * Returns true if the fire button was pressed.
     *
     * @return true if the fire button was pressed.
     */
    bool didFire() const { return _firePressed; }
    
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
     * Cancellation occurs when an external eventâ€”for example,
     * an incoming phone callâ€”disrupts the current appâ€™s event
     * processing.
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void    touchCancelCB(Touch* t, timestamp_t time);
};

#endif /* defined(__C_INPUT_H__) */