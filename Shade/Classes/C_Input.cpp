//
//  PFInputController.cpp
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
#include "C_Input.h"


#pragma mark -
#pragma mark Input Settings
/** The key to use for reseting the game */
#define RESET_KEY EventKeyboard::KeyCode::KEY_R
/** The key for toggling the debug display */
#define DEBUG_KEY EventKeyboard::KeyCode::KEY_D
/** The key for exitting the game */
#define EXIT_KEY  EventKeyboard::KeyCode::KEY_ESCAPE
/** The key for firing a bullet */
#define FIRE_KEY EventKeyboard::KeyCode::KEY_SPACE
/** The key for jumping up */
#define JUMP_KEY EventKeyboard::KeyCode::KEY_UP_ARROW

/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How fast we must swipe left or right for a gesture */
#define EVENT_SWIPE_TIME    1000
/** How far we must swipe left or right for a gesture (as ratio of screen) */
#define EVENT_SWIPE_LENGTH  0.05f

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

/** The portion of the screen used for the left zone */
#define LEFT_ZONE       0.2f
/** The portion of the screen used for the right zone */
#define RIGHT_ZONE      0.2f
/** The portion of the screen used for the bottom zone */
#define BOTTOM_ZONE     0.2f


#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_touchListener(nullptr)
//_mouseListener(nullptr)
{
    _keyReset = false;
    _keyDebug = false;
    _keyExit  = false;
    
    _horizontal = 0.0f;
    _keyFire  = false;
    
    _keyMove  = false;
    
    // Initialize the touch values.
    _touch.touchid = -1;
    _touch.count = 0;
    
    
}


/**
 * Disposes of this input controller, releasing all listeners.
 */
InputController::~InputController() {
    if (_touchListener != nullptr) {
        _touchListener->release();
        _touchListener = nullptr;
    }
    /* if (_mouseListener != nullptr) {
     _mouseListener->release();
     _mouseListener = nullptr;
     } */
}

/**
 * Initializes the input control for the given drawing scale and bounds.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * The meaning of touch events depends on the screen size. The parameter
 * bounds allows the input controller to identify the bounds of the screen.
 *
 * @param  bounds   the bounds of the touch device
 *
 * @return true if the controller was initialized successfully
 */
bool InputController::init(const Rect& bounds) {
    
    // Create the touch listener. This is an autorelease object.
    _touchListener = TouchListener::create();
    if (_touchListener != nullptr) {
        _touchListener->retain();
        _touchListener->onTouchBegan = [this](Touch* t, timestamp_t time) {
            return touchBeganCB(t,time);
        };
        _touchListener->onTouchMoved = [=](Touch* t, timestamp_t time) {
            return this->touchMovedCB(t,time);
        };
        _touchListener->onTouchEnded = [=](Touch* t, timestamp_t time) {
            return this->touchEndedCB(t,time);
        };
        _touchListener->onTouchCancelled = [=](Touch* t, timestamp_t time) {
            return this->touchCancelCB(t,time);
        };
        return true;
    }
    return false;
}

/**
 * Starts the input processing for this input controller.
 *
 * This method must be called AFTER the input controller is initialized
 */
void InputController::start() {
    if (!_active) {
        _active = true;
        // Prioritize the input
        // BUG: Cocos2D always prioritizes touch
        KeyboardPoller::start(1);
        _touchListener->start(2);
    }
}

/**
 * Stops the input processing for this input controller.
 *
 * This method will not dispose of the input controller. It can be restarted.
 */
void InputController::stop() {
    if (_active) {
        _active = false;
        _touchListener->stop();
        KeyboardPoller::stop();
    }
}


#pragma mark -
#pragma mark Input Handling

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
void InputController::update(float dt) {
    if (!_active) {
        return;
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // DESKTOP CONTROLS
    KeyboardPoller* keys = KeyboardPoller::getInstance();
    keys->update();
    
    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
    
    _keyFire   = keys->keyPressed(FIRE_KEY);
    _keyJump   = keys->keyPressed(JUMP_KEY);
    
    _keyLeft  = keys->keyDown(EventKeyboard::KeyCode::KEY_LEFT_ARROW);
    _keyRight = keys->keyDown(EventKeyboard::KeyCode::KEY_RIGHT_ARROW);
    _keyUp = keys->keyDown(EventKeyboard::KeyCode::KEY_UP_ARROW);
    _keyDown = keys->keyDown(EventKeyboard::KeyCode::KEY_DOWN_ARROW);
    
    
#endif
    // Nothing to do for MOBILE CONTROLS
    
    // Capture the current state
    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    _firePressed  = _keyFire;
    _jumpPressed  = _keyJump;
    
    // Directional controls
    _horizontal = 0.0f;
    _vertical = 0.0f;
    if (_keyRight) {
        _horizontal += 1.0f;
    }
    if (_keyLeft) {
        _horizontal -= 1.0f;
    }
    if (_keyUp) {
        _vertical += 1.0f;
    }
    if (_keyDown) {
        _vertical -= 1.0f;
    }
    
    
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // Need to clear keys in the mobile state
    _keyDebug = false;
    _keyReset = false;
    _keyExit  = false;
    
    _keyMove  = false;
#endif
}




/**
 * Returns true if this is a tap to move.
 *
 *
 * @param  target    the target position of the candidate tap
 * @param  current  the current timestamp of the gesture
 *
 * @return true if the tapped position is not the current position.
 */
bool InputController::checkTap(const Vec2& start, const Vec2& stop) {
    // Look for swipes up that are "long enough"
    float xidff = stop.x - start.x;
    float yidff = stop.y - start.y;
    if(xidff != 0 || yidff != 0 ){
        return true;
    }
    return false;
}



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
bool InputController::touchBeganCB(Touch* t, timestamp_t current) {
    assert(!_isTouching);
    _isTouching = true;
    Vec2 pos = t->getLocation();
    if(_touch.touchid == -1){
        _touch.position = pos;
        _touch.touchid = t->getID();
        
    }
    _touch.count++;
    return true;
}

/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchEndedCB(Touch* t, timestamp_t current) {
    assert(_isTouching);
    _isTouching = false;
    // Reset all keys that might have been set
    _touch.count--;
    if(_touch.count == 0){
        _touch.touchid = -1;
        
    }
    
    
}


/**
 * Callback for a touch movement event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchMovedCB(Touch* t, timestamp_t current) {
    assert(_isTouching);
    Vec2 pos = t->getLocation();
    if(t->getID() == _touch.touchid ){
        _keyMove = checkTap(_touch.position, t->getLocation());
    }
}
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
void InputController::touchCancelCB(Touch* t, timestamp_t current) {
    // Update the timestamp
    assert(_isTouching);
    _isTouching = false;
    
    _touch.touchid = -1;
    _touch.count = 0;
    
}
