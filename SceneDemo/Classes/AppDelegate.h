//
//  AppDelegate.h
//  SceneDemo
//
//  This is the true root class.  Anything that has to be allocated across all scenes
//  (e.g. asset management, the sound engine) must be handled here.  Everything else
//  should be delegated to the game root.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 4/2/16
//
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include <cocos2d.h>

/**
 * Class that represents cocos2d Application.
 *
 * This is implemented as private inheritance is to hide some interface calls
 * by the Director.
 */
class  AppDelegate : private cocos2d::Application {
public:
    /**
     * Constructs a new AppDelegate 
     *
     * This method defers to the parent class, as true initialization
     * happens in applicationDidFinishLaunching().
     */
    AppDelegate();
    
    /**
     * Disposes a new AppDelegate 
     *
     * This method should release any global resources that were allocated
     * at application start-up.
     */
    virtual ~AppDelegate();
    
    /**
     * Initializes the OpenGL settings. 
     *
     * If you want a different context, just modify the value of glContextAttrs. It 
     * will take effect on all platforms
     */
    virtual void initGLContextAttrs();
    
    /**
     * Initializes the application after it has finished bootstrap loading.
     *
     * Your application startup code goes here.  However, you should
     * not need to make too many modifications to this method.  Just
     * create a new game layer.
     *
     * @return true if initialization succesful; otherwise false
     */
    virtual bool applicationDidFinishLaunching();
    
    /**
     * Moves the application to the background.
     *
     * This method is invoked when you return to the home screen or take a phone 
     * call. You should add page-out code to your game root and invoke it here.
     *
     * If your app uses A LOT of memory, you might want to release all that
     * memory here.  We will have examples of this in later demos.
     */
    virtual void applicationDidEnterBackground();
    
    /**
     * Moves the application to the foreground.
     *
     * This is the inverse of the function applicationDidEnterBackground().
     * You should add page-in code to your game root and invoke it here.
     *
     * If you released memory, you will need to restore all that state here.
     * We will have examples of this in later demos.
     */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

