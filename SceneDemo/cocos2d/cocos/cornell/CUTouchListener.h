//
//  CUTouchListener.h
//  Cornell Extensions to Cocos2D
//
//  Module provides a time sensitive listener for the touch input.  This class simplifies
//  a lot of the overhead for EventListenerTouchOneByOne.  More importantly, it adds a
//  timestamp to all of the callback functions so that the user can precisely time the
//  callbacks.
//
//  Author: Walker White
//  Version: 12/5/15
//
#ifndef __CU_TOUCH_LISTENER_H__
#define __CU_TOUCH_LISTENER_H__

#include <cocos2d.h>
#include "CUTimestamp.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Touch Listener

/**
 * Class provides a time senstive touch listener.
 *
 * When created, this class initializes but does not activate a touch listener.
 * You must use the start and stop methods to activate/deactivate the listener.
 * Furthermore, all of the callback functions are originally undefined.  They
 * must be defined before this listener performs any useful functionality.
 *
 * This class simplifies a lot of the overhead for EventListenerTouchOneByOne.  
 * More importantly,it adds a timestamp to all of the callback functions so that 
 * the user can precisely time the callbacks.
 *
 * While this listener is designed for mobile devices, it will register mouse
 * events as well.  It is preferable to MouseListener when full mouse motion 
 * is not required, because it is more efficient.
 */
class CC_DLL TouchListener : public Ref {
private:
    /** This macro disables the copy constructor (not allowed on listeners) */
    CC_DISALLOW_COPY_AND_ASSIGN(TouchListener);
    
protected:
    /** The event dispatching handling input for this controller */
    EventDispatcher* _dispatcher;
    /** Listener to process touch events */
    EventListenerTouchOneByOne* _touchListener;
    /** Whether this listener is active and receiving events */
    bool _active;
    
    
public:
#pragma mark Static Constructors
    /**
     * Creates a new input controller for the default dispatcher.
     *
     * @retain  a reference to the default event dispatcher
     * @return  An autoreleased touch listener
     */
    static TouchListener* create();
    
    /**
     * Creates a new input controller for the given dispatcher.
     *
     * @param dispatcher Event handler for callbacks.
     *
     * @retain  a reference to the event dispatcher
     * @return  An autoreleased touch listener
     */
    static TouchListener* create(EventDispatcher* dispatcher);
    
    
#pragma mark Event Handling
    /**
     * Starts this listener, registering it to receive events.
     *
     * The listener has fixed priority.
     *
     * @param  priority The input priority for the listener.
     */
    void start(int priority);
    
    /**
     * Starts this listener, registering it to receive events.
     *
     * The listener has scene priority.
     *
     * @param  node     The node for scene priority.
     */
    void start(Node* node);
    
    /**
     * Stops this listener, so that it no longer receives events
     */
    void stop();
    
    /**
     * Returns whether this listener is active and receiving events
     *
     * Returns whether this listener is active and receiving events
     */
    bool isActive() const { return _active; }
    
    /**
     * Called when a touch first starts
     *
     * While this interface is designed for single touches, it can handle 
     * multitouch events. Look at the id in the touch state for touch
     * identification when multiple touches happen.
     *
     * @param  touch    the touch state
     * @param  time     the time of the event
     *
     * @return true if the touch is consumed; false otherwise
     */
    std::function<bool(Touch* touch, timestamp_t time)> onTouchBegan;
    
    /**
     * Called when a touch moves.
     *
     * While this interface is designed for single touches, it can handle
     * multitouch events. Look at the id in the touch state for touch
     * identification when multiple touches happen.
     *
     * If this event handler runs on a desktop with a mouse, this event
     * will only register if the mouse is moved while a button is held down.
     *
     * @param  touch    the touch state
     * @param  time     the time of the event
     */
    std::function<void(Touch* touch, timestamp_t time)> onTouchMoved;
    
    /**
     * Called when a touch completes
     *
     * While this interface is designed for single touches, it can handle
     * multitouch events. Look at the id in the touch state for touch
     * identification when multiple touches happen.
     *
     * @param  touch    the touch state
     * @param  time     the time of the event
     */
    std::function<void(Touch* touch, timestamp_t time)> onTouchEnded;
    
    /**
     * Called when a touch is aborted
     *
     * While this interface is designed for single touches, it can handle
     * multitouch events. Look at the id in the touch state for touch
     * identification when multiple touches happen.
     *
     * @param  touch    the touch state
     * @param  time     the time of the event
     */
    std::function<void(Touch* touch, timestamp_t time)> onTouchCancelled;
    
    
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new touch listener, but does not initialize it.
     */
    TouchListener() : _active(false) {}
    
    /**
     * Deallocates the touch listener, releasing all memory.
     */
    virtual ~TouchListener();
    
    /**
     * Initializes a new input controller for the default dispatcher.
     *
     * @return  true if the listener is initialized properly, false otherwise.
     */
    bool init() { return init(Director::getInstance()->getEventDispatcher()); }
    
    /**
     * Creates a new input controller for the given dispatcher.
     *
     * @param dispatcher Event handler for callbacks.
     *
     * @retain a reference to the event dispatcher
     * @return true if the listener is initialized properly, false otherwise.
     */
    bool init(EventDispatcher* dispatcher);
    
};

NS_CC_END
#endif /* defined(__CU_TOUCH_LISTENER_H__) */
