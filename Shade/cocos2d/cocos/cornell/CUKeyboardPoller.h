//
//  CUKeyboardPoller.h
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
#ifndef __CU_KEYBOARD_POLLER_H__
#define __CU_KEYBOARD_POLLER_H__

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cocos2d.h>

NS_CC_BEGIN

/**
 * Hash code function for keyboard key codes
 */
struct KeyCodeHasher{
    std::size_t operator()(const EventKeyboard::KeyCode& k) const {
        return (std::hash<int>()(static_cast<int>(k)));
    }
};


#pragma mark -
#pragma mark Keyboard Poller

/**
 * Class to providing a polling interface for the Keyboard.
 *
 * Cocos2d decided to make all of its input event-driven. This is a major problem,
 * because it is hard to associate the events with the current animation frame,
 * potentially causing lag.  A poller is an alternative input interface that allows 
 * us to query the current state of the keyboard when we need it.
 *
 * Because there is (theoretically) only one keyboard, this class provides a singleton
 * interface.  You can start and stop the interface with static methods.
 */
class CC_DLL KeyboardPoller {
private:
    /** This macro disables the copy constructor (not allowed on input controllers) */
    CC_DISALLOW_COPY_AND_ASSIGN(KeyboardPoller);

protected:
    /** Singleton keyboard controller */
    static KeyboardPoller* _gKeyboard;
    /** Listener to process keyboard events */
    EventListenerKeyboard* _keybdListener;
    /** Whether this listener is active and receiving events */
    bool _active;

    /** The previously pressed keys */
    std::unordered_set<EventKeyboard::KeyCode,KeyCodeHasher> _previous;
    /** The currently pressed keys */
    std::unordered_set<EventKeyboard::KeyCode,KeyCodeHasher> _current;
    /** The keys that will change in the next animation frame */
    std::unordered_map<EventKeyboard::KeyCode,bool,KeyCodeHasher> _changed;

    
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
     * Returns the singleton interface for the keyboard poller.
     *
     * It is unsafe to make your own keyboard poller.  Just use this static method
     * to access the singleton object.
     *
     * This method will return nullptr if the poller is not yet started.
     */
    static KeyboardPoller* getInstance();

    
#pragma mark Keyboard State
    /**
     * Returns whether this listener is active and receiving events
     *
     * Returns whether this listener is active and receiving events
     */
    bool isActive() const { return _active; }
    
    /**
     * Gather the input for the current animation frame.
     *
     * This method is used to take any cached key events and add them to
     * the poller state.  This should be called just before any state
     * queries in the current animation frame.
     */
    void update();

    /**
     * Returns true if the key is currently held down.
     *
     * @param  code the keyboard key to test
     *
     * @return true if the key is currently held down.
     */
    bool keyDown(EventKeyboard::KeyCode code) const {
        return _current.find(code) != _current.end();
    }

    /**
     * Returns true if the key was pressed this animation frame.
     *
     * A key press occurs if the key is down this animation frame, but was not
     * down the previous animation frame.
     *
     * @param  code the keyboard key to test
     *
     * @return true if the key is currently held down.
     */
    bool keyPressed(EventKeyboard::KeyCode code) const {
        return _current.find(code) != _current.end() && _previous.find(code) == _previous.end();
    }

    /**
     * Returns true if the key was released this animation frame.
     *
     * A key press occurs if the key is up this animation frame, but was not
     * up the previous animation frame.
     *
     * @param  code the keyboard key to test
     *
     * @return true if the key is currently held down.
     */
    bool keyReleased(EventKeyboard::KeyCode code) {
        return _current.find(code) == _current.end() && _previous.find(code) != _previous.end();
    }
    
    /**
     * Returns the number of keys currently held down.
     *
     * @return the number of keys currently held down.
     */
    int keyCount() const { return (int)_current.size(); }
    
    /**
     * Returns a list of the keys currently held down.
     *
     * This list contains the codes for all of the keys currently held down. This
     * list is a copy; modifying it has not effect on the poller.
     *
     * @return a list of the keys currently held down.
     */
    const std::vector<EventKeyboard::KeyCode> keySet() const;
    
    
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new (inactive) input controller for the keyboard.
     */
    KeyboardPoller();
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~KeyboardPoller();
    
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
#endif /* defined(__CU_KEYBOARD_POLLER_H__) */
