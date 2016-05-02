//
//  RGRagdollInput.cpp
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
#include "RGRagdollInput.h"

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


#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
RagdollInput::RagdollInput() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_touchListener(nullptr)
{
    _keyReset = false;
    _keyDebug = false;
    _keyExit  = false;
    _select   = false;
    _tcount   = 0;
}

/**
 * Disposes of this input controller, releasing all listeners.
 */
RagdollInput::~RagdollInput() {
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
 * The drawing scale is the difference in size between the layer (which
 * is receiving the touch) and the physics world.  This allows us to better
 * associate a touch with an object.
 *
 * @param  scale    the drawing scale
 *
 * @return true if the controller was initialized successfully
 */
bool RagdollInput::init(const Vec2& scale) {
    // Invert scale to convert back
    _scale.set(1/scale.x,1/scale.y);
    _timestamp = current_time();
    
    // Create the touch listener. This is an autorelease object.
    _touchListener = MultiTouchListener::create();
    if (_touchListener != nullptr) {
        _touchListener->retain();
        _touchListener->onTouchesBegan = [this](const std::vector<Touch*>& touches, timestamp_t time) {
            return touchesBeganCB(touches,time);
        };
        _touchListener->onTouchesMoved = [this](const std::vector<Touch*>& touches, timestamp_t time) {
            return touchesMovedCB(touches,time);
        };
        _touchListener->onTouchesEnded = [this](const std::vector<Touch*>& touches, timestamp_t time) {
            return touchesEndedCB(touches,time);
        };
        _touchListener->onTouchesCancelled = [this](const std::vector<Touch*>& touches, timestamp_t time) {
            return touchesCancelCB(touches,time);
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
void RagdollInput::start() {
    if (!_active) {
        _active = true;
        // Priority the input
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
void RagdollInput::stop() {
    if (_active) {
        _tcount = 0;
        _select = false;
        _active = false;
        _keyReset = false;
        _keyDebug = false;
        _keyExit  = false;
        _touchListener->stop();
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
void RagdollInput::update(float dt) {
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
#endif

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // Reset simulated keyboard
    _keyDebug = false;
    _keyReset = false;
    _keyDebug = false;
#endif
}


#pragma mark -
#pragma mark Touch Callbacks
/**
 * Called when a multitouch event moves.
 *
 * If this event handler runs on a desktop with a mouse, this event
 * will only register if the mouse is moved while a button is held down.
 *
 * @param  touches  the collective touch state
 * @param  time     the time of the event
 */
 bool RagdollInput::touchesBeganCB(const std::vector<Touch*>& touches, timestamp_t current) {
     _tcount += touches.size();
     _select = (_tcount == 1);
     
     // Time how long it has been since last start touch.
     _keyDebug = _select && (elapsed_millis(_timestamp,current) <= EVENT_DOUBLE_CLICK);
     _timestamp = current;
     
     // Anchor the location if we have one touch.
     if (_select) {
         _ctouch.set(touches[0]->getLocation());
         _ctouch.scale(_scale);
         return true;
     } else if (_tcount > 1) {
         // Anchor the second if we have two
         int pos = (touches.size() > 1 ? 1 : 0);
         _dtouch.set(touches[pos]->getLocation());
     }
     return false;
 }
 
/**
 * Called when a multitouch event completes
 *
 * @param  touches  the collective touch state
 * @param  time     the time of the event
 */
 void RagdollInput::touchesEndedCB(const std::vector<Touch*>& touches, timestamp_t current) {
     // Multitouch gesture has ended.  Give it meaning.
     if (_tcount > 1) {
         int pos = (touches.size() > 1 ? 1 : 0);
         Touch* t = touches[pos];
         float xdiff = (t->getLocation().x-_dtouch.x);
         bool fast = (elapsed_millis(_timestamp,current) < EVENT_SWIPE_TIME);
         _keyReset = fast && xdiff < -EVENT_SWIPE_LENGTH;
         _keyExit  = fast && xdiff > EVENT_SWIPE_LENGTH;
     }
     _tcount -= touches.size();
     _select = false;
 }
 
 
/**
 * Called when a multitouch event moves.
 *
 * If this event handler runs on a desktop with a mouse, this event
 * will only register if the mouse is moved while a button is held down.
 *
 * @param  touches  the collective touch state
 * @param  time     the time of the event
 */
 void RagdollInput::touchesMovedCB(const std::vector<Touch*>& touches, timestamp_t current) {
     // Move the physics body pointer if just one touch.
     if (_select) {
         _ctouch.set(touches[0]->getLocation());
         _ctouch.scale(_scale);
     }
 }
 
/**
 * Called when a multitouch event is aborted
 *
 * Cancellation occurs when an external event—for example, an incoming phone
 * call—disrupts the current app’s event processing.
 *
 * @param  touches  the collective touch state
 * @param  time     the time of the event
 */
 void RagdollInput::touchesCancelCB(const std::vector<Touch*>& touches, timestamp_t current) {
     // Reset settings
     _dtouch.set(-1.0f,-1.0f);
     _ctouch.set(-1.0f,-1.0f);
     _select = false;
 }
