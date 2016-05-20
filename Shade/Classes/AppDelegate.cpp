//
//  AppDelegate.cpp
//  PlatformerDemo
//
//  This is the true root class.  Anything that has to be allocated across all scenes
//  (e.g. asset management, the sound engine) must be handled here.  Everything else
//  should be delegated to the game root.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/15
//
#include "AppDelegate.h"
#include "PFGameRoot.h"


using namespace cocos2d;

/** Modern iPhone resolutions */
static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 576);
static cocos2d::Size smallResolutionSize  = cocos2d::Size(480, 270);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize  = cocos2d::Size(2048, 1536);

static cocos2d::Size testResolutionSize = cocos2d::Size(1920, 1080);
//static cocos2d::Size testResolutionSize = cocos2d::Size(1024, 576);


/**
 * Constructs a new AppDelegate
 *
 * This method defers to the parent class, as true initialization
 * happens in applicationDidFinishLaunching().
 */
AppDelegate::AppDelegate() {
}

/**
 * Disposes a new AppDelegate
 *
 * This method should release any global resources that were allocated
 * at application start-up.
 */
AppDelegate::~AppDelegate() {
    // If you started sound or an asset manager, it must be stopped here
    AssetManager::shutdown();
    SoundEngine::stop();
}

/**
 * Initializes the OpenGL settings.
 *
 * If you want a different context, just modify the value of glContextAttrs. It
 * will take effect on all platforms
 */
void AppDelegate::initGLContextAttrs() {
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

/**
 * Initializes the application after it has finished bootstrap loading.
 *
 * Your application startup code goes here.  However, you should
 * not need to make too many modifications to this method.  Just
 * create a new game layer.
 *
 * @return true if initialization succesful; otherwise false
 */
bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("Shade",
//                                            Rect(0, 0, designResolutionSize.width,designResolutionSize.height));
                                            Rect(0, 0, testResolutionSize.width,testResolutionSize.height));
#else
        glview = GLViewImpl::create("Rocket Demo");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    ResolutionPolicy::NO_BORDER);
    Size frameSize = glview->getFrameSize();
    
    // Rescale the window to align with the design resolution
    if (frameSize.height > designResolutionSize.height) {
        director->setContentScaleFactor(MIN(frameSize.height/designResolutionSize.height,
                                            frameSize.width/designResolutionSize.width));
    }
    
    // Start any global asset managers (Sound, etc...)
    SoundEngine::start();
    AssetManager::init();
    
    // MODIFY this line to use your root class
    auto scene = GameRoot::createScene<PlatformRoot>();

    // Run the game
    director->runWithScene(scene);

    return true;
}

/**
 * Moves the application to the background.
 *
 * This method is invoked when you return to the home screen or take a phone
 * call. You should add page-out code to your game root and invoke it here.
 *
 * If your app uses A LOT of memory, you might want to release all that
 * memory here.  We will have examples of this in later demos.
 */
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SoundEngine, it must be paused here
    SoundEngine::getInstance()->pauseAll();
}

/**
 * Moves the application to the foreground.
 *
 * This is the inverse of the function applicationDidEnterBackground().
 * You should add page-in code to your game root and invoke it here.
 *
 * If you released memory, you will need to restore all that state here.
 * We will have examples of this in later demos.
 */
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SoundEngine, it must resume here
    SoundEngine::getInstance()->resumeAll();
}
