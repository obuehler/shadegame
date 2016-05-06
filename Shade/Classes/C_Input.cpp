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
#define EVENT_DOUBLE_CLICK  200
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

#define CENTER_ZONE       0.01f


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
	_pausePressed(false),
	_touchListener(nullptr)
	//_mouseListener(nullptr)
{
	_keyReset = false;
	_keyDebug = false;
	_keyExit = false;

	_keyDoubleTap = false;

	_horizontal = 0.0f;
	_vertical = 0.0f;
	_keyFire = false;
	_keyJump = false;

	// Initialize the touch values.
	_ltouch.touchid = -1;
	_rtouch.touchid = -1;
	_btouch.touchid = -1;
	_mtouch.touchid = -1;
	_ltouch.count = 0;
	_rtouch.count = 0;
	_btouch.count = 0;
	_mtouch.count = 0;
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
	_bounds = bounds;
	createZones();

	_swipetime = current_time();
	_dbtaptime = current_time();
	// Create the touch listener. This is an autorelease object.
	_touchListener = TouchListener::create();
	if (_touchListener != nullptr) {
		_touchListener->retain();
		_touchListener->onTouchBegan = [this](Touch* t, timestamp_t time) {
			return touchBeganCB(t, time);
		};
		_touchListener->onTouchMoved = [=](Touch* t, timestamp_t time) {
			return this->touchMovedCB(t, time);
		};
		_touchListener->onTouchEnded = [=](Touch* t, timestamp_t time) {
			return this->touchEndedCB(t, time);
		};
		_touchListener->onTouchCancelled = [=](Touch* t, timestamp_t time) {
			return this->touchCancelCB(t, time);
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
	_keyReset = keys->keyPressed(RESET_KEY);
	_keyDebug = keys->keyPressed(DEBUG_KEY);
	_keyExit = keys->keyPressed(EXIT_KEY);

	_keyFire = keys->keyPressed(FIRE_KEY);
	_keyJump = keys->keyPressed(JUMP_KEY);

	_keyLeft = keys->keyDown(EventKeyboard::KeyCode::KEY_LEFT_ARROW);
	_keyRight = keys->keyDown(EventKeyboard::KeyCode::KEY_RIGHT_ARROW);
	_keyUp = keys->keyDown(EventKeyboard::KeyCode::KEY_UP_ARROW);
	_keyDown = keys->keyDown(EventKeyboard::KeyCode::KEY_DOWN_ARROW);


#endif
	// Nothing to do for MOBILE CONTROLS

	// Capture the current state
	_resetPressed = _keyReset;
	_debugPressed = _keyDebug;
	_exitPressed = _keyExit;
	_firePressed = _keyFire;
	_jumpPressed = _keyJump;
	_pausePressed = _keyDoubleTap;

	// Directional controls
	//_horizontal = 0.0f;
	//_vertical = 0.0f;
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

	_keyDoubleTap = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	// Need to clear keys in the mobile state
	_keyDebug = false;
	_keyReset = false;
	_keyExit = false;
	_keyJump = false;
	_keyFire = false;
#endif
}


/**
* Defines the zone boundaries, so we can quickly categorize touches.
*/
void InputController::createZones() {
	_mzone = _bounds;
	_mzone.size.width *= CENTER_ZONE;
	_mzone.size.height *= CENTER_ZONE;
}

/**
* Returns the correct zone for the given position.
*
* See the comments above for a description of how zones work.
*
* @param  pos  a position in screen coordinates
*
* @return the correct zone for the given position.
*/
/*InputController::Zone InputController::getZone(const Vec2& pos) {
if (_lzone.containsPoint(pos)) {
return Zone::LEFT;
} else if (_rzone.containsPoint(pos)) {
return Zone::RIGHT;
} else if (_bzone.containsPoint(pos)) {
return Zone::BOTTOM;
} else if (_bounds.containsPoint(pos)) {
return Zone::MAIN;
}
return Zone::UNDEFINED;
} */

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
bool InputController::checkJump(const Vec2& start, const Vec2& stop, timestamp_t current) {
	// Look for swipes up that are "long enough"
	float ydiff = (stop.y - start.y);
	if (elapsed_millis(_swipetime, current) < EVENT_SWIPE_TIME) {
		return (ydiff > EVENT_SWIPE_LENGTH*_bounds.size.height);
	}
	return false;
}

/**
* Check if it touched the center of the screen
*
*
* @param  pos  a position in screen coordinates
*
* @return true if it touched the center screen
*/
bool InputController::isCenter(const Vec2& pos) {
	if (_mzone.containsPoint(pos)) {
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
	Vec2 pos = t->getLocation();
	_swipetime = current;
	_keyDoubleTap = (elapsed_millis(_dbtaptime, current) <= EVENT_DOUBLE_CLICK);

	if (isCenter(pos)) {
		_vertical = 0;
		_horizontal = 0;
	}
	_vertical = (pos.y - _bounds.getMidY()) / (_bounds.size.height / 2.0f);
	_horizontal = (pos.x - _bounds.getMidX()) / (_bounds.size.width / 2.0f);
	return true;
}

/**
* Callback for the end of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchEndedCB(Touch* t, timestamp_t current) {
	_dbtaptime = current;
}


/**
* Callback for a touch movement event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchMovedCB(Touch* t, timestamp_t current) {
	
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
	_dbtaptime = current;
	_swipetime = current;
	_ltouch.touchid = -1;
	_rtouch.touchid = -1;
	_btouch.touchid = -1;
	_mtouch.touchid = -1;
	_ltouch.count = 0;
	_rtouch.count = 0;
	_btouch.count = 0;
	_mtouch.count = 0;

}