//
//  RDRocketInput.cpp
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
#include "RDRocketInput.h"

/** The key to use for reseting the game */
#define RESET_KEY EventKeyboard::KeyCode::KEY_R
/** The key for toggling the debug display */
#define DEBUG_KEY EventKeyboard::KeyCode::KEY_D
/** The key for exitting the game */
#define EXIT_KEY  EventKeyboard::KeyCode::KEY_ESCAPE

/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How far we must swipe left or right for a gesture */
#define EVENT_SWIPE_LENGTH  200
/** How fast we must swipe left or right for a gesture */
#define EVENT_SWIPE_TIME   1000
/** How far we must turn the tablet for the accelerometer to register */
#define EVENT_ACCEL_THRESH  M_PI/10.0f


#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
RocketInput::RocketInput() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_touchListener(nullptr)
{
    _keyUp    = false;
    _keyDown  = false;
    _keyReset = false;
    _keyDebug = false;
    _keyExit  = false;
    
    _horizontal = 0.0f;
    _vertical   = 0.0f;
}

/**
 * Disposes of this input controller, releasing all listeners.
 */
RocketInput::~RocketInput() {
    if (_touchListener != nullptr) {
        _touchListener->release();
        _touchListener = nullptr;
    }
}

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @return true if the controller was initialized successfully
 */
bool RocketInput::init() {
    _timestamp = current_time();
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
void RocketInput::start() {
    if (!_active) {
        _active = true;
        // Priority the input
        // BUG: Cocos2D always prioritizes touch
        KeyboardPoller::start(1);
        _touchListener->start(2);
        AccelerationPoller::start(3);
    }
}

/**
 * Stops the input processing for this input controller.
 *
 * This method will not dispose of the input controller. It can be restarted.
 */
void RocketInput::stop() {
    if (_active) {
        _active = false;
        _touchListener->stop();
        AccelerationPoller::stop();
        KeyboardPoller::stop();
    }
}

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
void RocketInput::update(float dt) {
    if (!_active) {
        return;
    }

    int left = false;
    int rght = false;
    int up   = false;
    int down = false;

    // WIN32, WINRT, MAC, and LINUX are allowed to have desktop controls and everything else uses touch
    // However, WINRT is also allowed to have both keyboard and touch controls ...
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // DESKTOP CONTROLS
    KeyboardPoller* keys = KeyboardPoller::getInstance();
    keys->update();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
    
    left = keys->keyDown(EventKeyboard::KeyCode::KEY_LEFT_ARROW);
    rght = keys->keyDown(EventKeyboard::KeyCode::KEY_RIGHT_ARROW);
    up   = keys->keyDown(EventKeyboard::KeyCode::KEY_UP_ARROW);
    down = keys->keyDown(EventKeyboard::KeyCode::KEY_DOWN_ARROW);
#endif
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) && (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)
    // MOBILE CONTROLS
    Acceleration acc = AccelerationPoller::getInstance()->getAcceleration();
    // Pitch allows us to treat the tablet like a steering wheel
    float pitch = atan2(-acc.x, sqrt(acc.y*acc.y + acc.z*acc.z));
    
    // Check if we turned left or right
    left |= (pitch > EVENT_ACCEL_THRESH);
    rght |= (pitch < -EVENT_ACCEL_THRESH);
    up   |= _keyUp;
#endif

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    
    // Directional controls
    _horizontal = 0.0f;
    if (rght) {
        _horizontal += 1.0f;
    }
    if (left) {
        _horizontal -= 1.0f;
    }

    _vertical = 0.0f;
    if (up) {
        _vertical += 1.0f;
    }
    if (down) {
        _vertical -= 1.0f;
    }


// If it does not support keyboard, we must reset "virtual" keyboard
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _keyDebug = false;
    _keyReset = false;
    _keyDebug = false;
#endif
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
 bool RocketInput::touchBeganCB(Touch* t, timestamp_t current) {
     // All touches correspond to key up
     _keyUp = true;
     
     // Update the touch location for later gestures
     _timestamp = current;
     _dtouch.set(t->getLocation());
     return true;
 }
 
 /**
  * Callback for the end of a touch event
  *
  * @param t     The touch information
  * @param event The associated event
  */
 void RocketInput::touchEndedCB(Touch* t, timestamp_t current) {
     CCLOG("Touch ended");
     // Gesture has ended.  Give it meaning.
     float xdiff = (t->getLocation().x-_dtouch.x);
     float ydiff = (t->getLocation().y-_dtouch.y);
     bool fast = (elapsed_millis(_timestamp,current) < EVENT_SWIPE_TIME);
     _keyReset = fast && xdiff < -EVENT_SWIPE_LENGTH;
     _keyExit  = fast && xdiff > EVENT_SWIPE_LENGTH;
     _keyDebug = fast && ydiff > EVENT_SWIPE_LENGTH;
     _keyUp = false;
 }
 
 
 /**
  * Callback for a touch movement event
  *
  * @param t     The touch information
  * @param event The associated event
  */
 void RocketInput::touchMovedCB(Touch* t, timestamp_t current) {
     // This example only has gesture support.  Nothing to do here.
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
 void RocketInput::touchCancelCB(Touch* t, timestamp_t current) {
     // Update the timestamp
     _timestamp = current;
     _dtouch.set(-1.0f,-1.0f);
 }
