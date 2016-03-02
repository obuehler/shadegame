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
#include "PFInputController.h"


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
    _keyJump  = false;
    
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
    _keyJump  = false;
    _keyFire  = false;
#endif
}


/**
 * Defines the zone boundaries, so we can quickly categorize touches.
 */
void InputController::createZones() {
    _lzone = _bounds;
    _lzone.size.width *= LEFT_ZONE;
    _rzone = _bounds;
    _rzone.size.width *= RIGHT_ZONE;
    _rzone.origin.x = _bounds.origin.x+_bounds.size.width-_rzone.size.width;
    _bzone = _bounds;
    _bzone.size.height *= BOTTOM_ZONE;
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
InputController::Zone InputController::getZone(const Vec2& pos) {
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
}

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
    float ydiff = (stop.y-start.y);
    if (elapsed_millis(_swipetime,current) < EVENT_SWIPE_TIME) {
        return (ydiff > EVENT_SWIPE_LENGTH*_bounds.size.height);
    }
    return false;
}

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
int InputController::checkSwipe(const Vec2& start, const Vec2& stop, timestamp_t current) {
    // Look for swipes up that are "long enough"
    float xdiff = (stop.x-start.x);
    if (elapsed_millis(_swipetime,current) < EVENT_SWIPE_TIME) {
        float thresh = EVENT_SWIPE_LENGTH*_bounds.size.width;
        if (xdiff > thresh) {
            return 1;
        } else if (xdiff < thresh) {
            return -1;
        }
    }
    return 0;
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
    Zone zone = getZone(pos);
    switch (zone) {
        case Zone::LEFT:
            CCLOG("Zone left");
            // Only process if no touch in zone
            if (_ltouch.touchid == -1) {
                _ltouch.position = pos;
                _ltouch.touchid = t->getID();
                // Cannot do both.
                _keyLeft = _rtouch.touchid == -1;
            }
            break;
        case Zone::RIGHT:
            CCLOG("Zone right");
            // Only process if no touch in zone
            if (_rtouch.touchid == -1) {
                _rtouch.position = pos;
                _rtouch.touchid = t->getID();
                _keyRight = _ltouch.touchid == -1;
            }
            break;
        case Zone::BOTTOM:
            CCLOG("Zone bottom");
            // Only process if no touch in zone
            if (_btouch.touchid == -1) {
                _btouch.position = pos;
                _btouch.touchid = t->getID();
            }
            _keyFire = true;
            break;
        case Zone::MAIN:
            // Only check for double tap in Main if nothing else down
            if (_ltouch.touchid == -1 && _rtouch.touchid == -1 && _btouch.touchid == -1 && _mtouch.touchid == -1) {
                _keyDebug = (elapsed_millis(_dbtaptime,current) <= EVENT_DOUBLE_CLICK);
            }
            
            // Keep count of touches in Main zone.
            if (_mtouch.touchid == -1) {
                _mtouch.position = pos;
                _mtouch.touchid = t->getID();
            }
            _mtouch.count++;
            break;
        default:
            CCASSERT(false, "Touch is out of bounds");
            break;
    }
    _swipetime = current;
    return true;
}

/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchEndedCB(Touch* t, timestamp_t current) {
    // Reset all keys that might have been set
    CCLOG("Touch is up %d", t->getID());
    if (_ltouch.touchid == t->getID()) {
        _ltouch.touchid = -1;
        _ltouch.count = 0;
        _keyLeft = false;
    } else if (_rtouch.touchid == t->getID()) {
        _rtouch.touchid = -1;
        _rtouch.count = 0;
        _keyRight = false;
    } else if (_btouch.touchid == t->getID()) {
        _btouch.touchid = -1;
        _btouch.count = 0;
        // Fire is made false by update
    } else if (_mtouch.touchid == t->getID()) {
        _mtouch.count--;
        if (_mtouch.count == 0) {
            _mtouch.touchid = -1;
        }
        // Reset, debug is made false by update
    }
    _dbtaptime = current;
}


/**
 * Callback for a touch movement event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchMovedCB(Touch* t, timestamp_t current) {
    if (t->getID() == _ltouch.touchid && getZone(t->getLocation()) == Zone::LEFT)  {
        _keyJump = checkJump(_ltouch.position, t->getLocation(), current);
    } else if (t->getID() == _rtouch.touchid && getZone(t->getLocation()) == Zone::RIGHT)  {
        _keyJump = checkJump(_rtouch.position, t->getLocation(), current);
    } else if (t->getID() == _btouch.touchid && getZone(t->getLocation()) == Zone::BOTTOM)  {
        // Allow the fire "key" to be held down
        _keyFire = true;
    } else if (t->getID() == _mtouch.touchid && _mtouch.count > 1) {
        int swipe = checkSwipe(_mtouch.position, t->getLocation(), current);
        if (swipe == -1) {
            _keyReset = true;
        } else if (swipe == 1) {
            _keyExit = true;
        }
    }
    // More complex checks go here
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
