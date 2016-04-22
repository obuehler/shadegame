//
//  CUKeyboardPoller.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a polling interface for the Keyboard.  Cocos2d decided to
//  make all of its input event-driven. This is a major problem, because it is hard
//  to associate the events with the current animation frame, potentially causing
//  lag.  A poller is an alternative input interface that allows us to query the
//  current state of the keyboard when we need it.
//
//  Author: Walker White
//  Version: 12/5/15
//
#include "CUKeyboardPoller.h"

NS_CC_BEGIN

/** Initialization of static reference */
KeyboardPoller* KeyboardPoller::_gKeyboard = nullptr;

#pragma mark -
#pragma mark Initializers
/**
 * Creates a new (inactive) input controller for the keyboard.
 */
KeyboardPoller::KeyboardPoller() : _active(false) {
    // Create the keyboard listener. This is an autorelease object.
    _keybdListener = EventListenerKeyboard::create();
    _keybdListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        _changed[keyCode] = true;
    };
    _keybdListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        _changed[keyCode] = false;
    };
    _keybdListener->retain();
}

/**
 * Disposes of this input controller, releasing all listeners.
 */
KeyboardPoller::~KeyboardPoller() {
    if (_active) {
        dispose();
    }
    _keybdListener->release();
    _keybdListener = nullptr;
}

/**
 * Initializes this listener, registering it to receive events.
 *
 * The listener has fixed priority.
 *
 * @param  priority The input priority for the listener.
 */
void KeyboardPoller::init(int priority) {
    CCASSERT(!_active, "Listener is already active");
    // Register the listeners
    // BUG: Cocos2D always prioritizes touch
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithFixedPriority(_keybdListener,priority);
    _active = true;
}

/**
 * Initializes this listener, registering it to receive events.
 *
 * The listener has scene priority.
 *
 * @param  node     The node for scene priority.
 */
void KeyboardPoller::init(Node* node) {
    CCASSERT(!_active, "Listener is already active");
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(_keybdListener,node);
    _active = true;
}

/**
 * Stops this listener, so that it no longer receives events
 */
void KeyboardPoller::dispose() {
    CCASSERT(_active, "Listener is not active");
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeEventListener(_keybdListener);
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
void KeyboardPoller::start(int priority) {
    if (_gKeyboard == nullptr) {
        _gKeyboard = new KeyboardPoller();
    }
    _gKeyboard->init(priority);
}

/**
 * Starts the input processing for this poller.
 *
 * This method will activate the singleton and assign it a priority.
 * The listener has scene priority.
 *
 * @param  node     The node for scene priority.
 */
void KeyboardPoller::start(Node* node) {
    if (_gKeyboard == nullptr) {
        _gKeyboard = new KeyboardPoller();
    }
    _gKeyboard->init(node);
}

/**
 * Stop the input processing for this input controller.
 *
 * This will deallocate the singleton, freeing memory.
 */
void KeyboardPoller::stop() {
    _gKeyboard->dispose();
    if (_gKeyboard != nullptr) {
        delete _gKeyboard;
        _gKeyboard = nullptr;
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
KeyboardPoller* KeyboardPoller::getInstance() {
    return _gKeyboard;
}


#pragma mark -
#pragma mark Keyboard State
/**
 * Gather the input for the current animation frame.
 *
 * This method is used to take any cached key events and add them to
 * the poller state.  This should be called just before any state
 * queries in the current animation frame.
 */
void KeyboardPoller::update() {
    _previous.clear();
    _previous.insert(_current.begin(),_current.end());
 
    for(auto it = _changed.begin(); it != _changed.end(); ++it) {
        if (it->second) {
            _current.insert(it->first);
        } else {
            _current.erase(it->first);
        }
    }
    _changed.clear();
}

/**
 * Returns a list of the keys currently held down.
 *
 * This list contains the codes for all of the keys currently held down. This
 * list is a copy; modifying it has not effect on the poller.
 *
 * @return a list of the keys currently held down.
 */
const std::vector<EventKeyboard::KeyCode> KeyboardPoller::keySet() const {
    return std::vector<EventKeyboard::KeyCode>(_current.begin(),_current.end());
}


NS_CC_END