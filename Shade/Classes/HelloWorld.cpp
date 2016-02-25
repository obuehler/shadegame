//
//  HelloWorld.cpp
//  HelloWorld
//
//  This is the root class for a single game scene.  It shows how to subclass RootLayer to
//  make a new game.  This is a standard design pattern that we have used in all our samples.
//
#include "HelloWorld.h"


USING_NS_CC;

/**
 * Starts the layer, allocating initial resources
 *
 * This method is used to bootstrap the game.  It should start up an asset manager
 * and load initial assets.
 */
void HelloWorld::start() {
    // Use the Cocos2d, not XNA, color
    setColor(Color3B::BLACK);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program

    // Add a "close" icon to exit the progress. It's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "textures/CloseNormal.png",
                                           "textures/CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // Create menu; it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // Add a label to show "Hello World"
    // Create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // Position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // Add the label as a child to this layer
    this->addChild(label, 1);

    // Add "HelloWorld" splash screen"
    auto sprite = Sprite::create("textures/HelloWorld.png");

    // Position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // Add the sprite as a child to this layer
    this->addChild(sprite, 0);
}

/**
 * Updates the game for a single animation frame
 *
 * This method is called every animation frame.  There is no draw() or render()
 * counterpoint to this method; drawing is done automatically in the scene graph.
 * However, this method is responsible for updating any transforms in the scene graph.
 *
 * @param  dt   the time in seconds since last update
 */
void HelloWorld::update(float dt) {
    // Since there is no animation, there is nothing to do
}

/**
 * Stops the layer, releasing all resources
 *
 * This method is used to clean-up any allocation that occurred in either start or
 * update.  While Cocos2d does have rudimentary garbage collection, you still have
 * to release any objects that you have retained.
 */
void HelloWorld::stop() {
    // Since nothing was retained, there is nothing to do
}

/**
 * Callback function for the UI widgit in this demo.
 *
 * This function is called when the close button is pressed.  It quits the application.
 *
 * @param  pSender the widgit issuing the command.
 */
void HelloWorld::menuCloseCallback(Ref* pSender) {
    // This RootLayer command closes this application.
    shutdown();
}
