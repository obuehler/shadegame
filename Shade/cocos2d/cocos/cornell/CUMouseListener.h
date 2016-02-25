//
//  CUMouseListener.h
//  Cornell Extensions to Cocos2D
//
//  Module provides a time sensitive listener for the mouse.  This class simplifies a
//  lot of the overhead for EventListenerMouse.  More importantly, it adds a timestamp
//  to all of the callback functions so that the user can precisely time the callbacks.
//
//  Author: Walker White
//  Version: 12/5/15
//
#ifndef __CU_MOUSE_LISTENER_H__
#define __CU_MOUSE_LISTENER_H__

#include <cocos2d.h>
#include "CUTimestamp.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Mouse Listener

/**
 * Class provides a time senstive mouse listener.
 *
 * When created, this class initializes but does not activate a mouse listener.
 * You must use the start and stop methods to activate/deactivate the listener.
 * Furthermore, all of the callback functions are originally undefined.  They
 * must be defined before this listener performs any useful functionality.
 *
 * This class simplifies a lot of the overhead for EventListenerMouse.  More 
 * importantly,it adds a timestamp to all of the callback functions so that the 
 * user can precisely time the callbacks.
 *
 * This is a very expensive listener.  onMouseMove is called every single animation
 * frame, registering a mouse move regardless of whether the mouse button is 
 * held down.  If you only want to register events on a button press, use a 
 * touch listener instead (which works on PCs too).
 */
class CC_DLL MouseListener : public Ref {
private:
    /** This macro disables the copy constructor (not allowed on listeners) */
    CC_DISALLOW_COPY_AND_ASSIGN(MouseListener);

protected:
    /** The event dispatching handling input for this controller */
    EventDispatcher* _dispatcher;
    /** Listener to process mouse events */
    EventListenerMouse* _mouseListener;
    /** Whether this listener is active and receiving events */
    bool _active;
    
    
public:
#pragma mark Static Constructors
    /**
     * Creates a new input controller for the default dispatcher.
     *
     * @return  An autoreleased mouse listener
     */
    static MouseListener* create();

    /**
     * Creates a new input controller for the given dispatcher.
     *
     * @param dispatcher Event handler for callbacks.
     *
     * @retain a reference to the event dispatcher
     * @return  An autoreleased mouse listener
     */
    static MouseListener* create(EventDispatcher* dispatcher);
    

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
     * Called when a mouse button is initially pressed
     *
     * @param  event    the event storing the mouse state
     * @param  time     the time of the event
     */
    std::function<void(EventMouse* event, timestamp_t time)> onMouseDown;

    /**
     * Called when a mouse button is released
     *
     * @param  event    the event storing the mouse state
     * @param  time     the time of the event
     */
    std::function<void(EventMouse* event, timestamp_t time)> onMouseUp;

    /**
     * Called when the mouse moves
     *
     * This event registers ALL THE TIME. It does not care whether the mouse
     * button is held down or not. This makes this a very expensive listener
     *
     * @param  event    the event storing the mouse state
     * @param  time     the time of the event
     */
    std::function<void(EventMouse* event, timestamp_t time)> onMouseMove;
    
    /**
     * Called when the mouse scroll wheel moves
     *
     * This event only registers when the mouse button is down.
     *
     * @param  event    the event storing the mouse state
     * @param  time     the time of the event
     */
    std::function<void(EventMouse* event, timestamp_t time)> onMouseScroll;

    
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new mouse listener, but does not initialize it.
     */
    MouseListener() : _active(false) {}

    /**
     * Deallocates the mouse listener, releasing all memory.
     */
    virtual ~MouseListener();
    
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
#endif /* defined(__CU_MOUSE_LISTENER_H__) */
