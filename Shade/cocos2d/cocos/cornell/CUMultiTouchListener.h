//
//  CUMultiTouchListener.h
//  Cornell Extensions to Cocos2D
//
//  Module provides a time sensitive listener for the touch input.  This class simplifies
//  a lot of the overhead for EventListenerTouchAllAtOnce.  More importantly, it adds a
//  timestamp to all of the callback functions so that the user can precisely time the
//  callbacks.
//
//  Author: Walker White
//  Version: 12/5/15
//
#ifndef __CU_MULTITOUCH_LISTENER_H__
#define __CU_MULTITOUCH_LISTENER_H__

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
 * This class simplifies a lot of the overhead for EventListenerTouchAllAtOnce.
 * More importantly,it adds a timestamp to all of the callback functions so that
 * the user can precisely time the callbacks.
 *
 * While TouchListener can handle multitouch events, this listener is preferred
 * if you are trying to coordinate the touches, such as in gesture recognition.
 */
class CC_DLL MultiTouchListener : public Ref {
private:
    /** This macro disables the copy constructor (not allowed on listeners) */
    CC_DISALLOW_COPY_AND_ASSIGN(MultiTouchListener);
    
protected:
    /** The event dispatching handling input for this controller */
    EventDispatcher* _dispatcher;
    /** Listener to process touch events */
    EventListenerTouchAllAtOnce* _touchListener;
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
    static MultiTouchListener* create();
    
    /**
     * Creates a new input controller for the given dispatcher.
     *
     * @param dispatcher Event handler for callbacks.
     *
     * @retain  a reference to the event dispatcher
     * @return  An autoreleased touch listener
     */
    static MultiTouchListener* create(EventDispatcher* dispatcher);
    
    
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
     * Called when a multitouch event first starts
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     *
     * @return true if the touch is consumed; false otherwise
     */
    std::function<void(const std::vector<Touch*>& touches, timestamp_t time)> onTouchesBegan;
    
    /**
     * Called when a multitouch event moves.
     *
     * If this event handler runs on a desktop with a mouse, this event
     * will only register if the mouse is moved while a button is held down.
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     */
    std::function<void(const std::vector<Touch*>& touches, timestamp_t time)> onTouchesMoved;
    
    /**
     * Called when a multitouch event completes
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     */
    std::function<void(const std::vector<Touch*>& touches, timestamp_t time)> onTouchesEnded;
    
    /**
     * Called when a multitouch event is aborted
     *
     * Cancellation occurs when an external event—for example, an incoming phone 
     * call—disrupts the current app’s event processing.
     *
     * @param  touches  the collective touch state
     * @param  time     the time of the event
     */
    std::function<void(const std::vector<Touch*>& touches, timestamp_t time)> onTouchesCancelled;
    
    
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new touch listener, but does not initialize it.
     */
    MultiTouchListener() : _active(false) {}
    
    /**
     * Deallocates the touch listener, releasing all memory.
     */
    virtual ~MultiTouchListener();
    
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
     * @retain  a reference to the event dispatcher
     * @return true if the listener is initialized properly, false otherwise.
     */
    bool init(EventDispatcher* dispatcher);
    
};

NS_CC_END
#endif /* defined(__CU_MULTITOUCH_LISTENER_H__) */