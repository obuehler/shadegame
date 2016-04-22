//
//  RGRagdollRoot.cpp
//  RagdollDemo
//
//  This class is a reimagining of the RagdollDemo.  Instead of splitting the layer and the
//  the controller, we combine them into a single class.  This is because we are now using
//  scenes instead of controllers to alter game functionality.
//
//  The most important thing to understand this time is the difference between init() and
//  start().  The method init() is called as soon as we create the layer and its associated
//  scene, even if it is not immediately active  The method start() is called it is activated
//  (and stop() is called when it is deactivated).
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 4/2/16
//
#include "RGRagdollRoot.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>

using namespace cocos2d;

#pragma mark -
#pragma mark Level Geography

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
static float WALL1[] = {16.0f, 18.0f, 16.0f, 17.0f,  1.0f, 17.0f,
    1.0f,  1.0f, 16.0f,  1.0f, 16.0f,  0.0f,
    0.f,  0.0f,  0.0f, 18.0f};
static float WALL2[] = {32.0f, 18.0f, 32.0f,  0.0f, 16.0f,  0.0f,
    16.0f,  1.0f, 31.0f,  1.0f, 31.0f, 17.0f,
    16.0f, 17.0f, 16.0f, 18.0f};

/** The initial position of the ragdoll head */
static float DOLL_POS[] = {16, 10};


#pragma mark -
#pragma mark Physics Constants

/** The density for all of (external) objects */
#define BASIC_DENSITY       0.0f
/** The friction for all of (external) objects */
#define BASIC_FRICTION      0.1f
/** The restitution for all of (external) objects */
#define BASIC_RESTITUTION   0.1f
/** How big to make the crosshairs */
#define CROSSHAIR_SIZE      0.1f
/** The new lessened gravity for this world */
#define WATER_GRAVITY   -0.25f


#pragma mark -
#pragma mark Asset Constants

/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The key for the rocket texture in the asset manager */
#define BKGD_TEXTURE    "background"
/** The key for the win door texture in the asset manager */
#define FRGD_TEXTURE    "foreground"
/** The key prefix for the multiple crate assets */
#define SOUND_PREFIX    "bubble"
/** The number of bubble sounds available */
#define NUM_BUBBLES     4

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color3B::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192
/** Opacity of the foreground mask */
#define FRGD_OPACITY    64


#pragma mark -
#pragma mark Initialization

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 */
RagdollRoot::RagdollRoot() :
_foreground(nullptr),
_worldnode(nullptr),
_debugnode(nullptr),
_background(nullptr),
_world(nullptr),
_ragdoll(nullptr),
_selector(nullptr),
_active(false),
_debug(false),
_counter(0)
{
}

/**
 * Created the demo contents. but does NOT start the game
 *
 * This constructor creates any objects that are to be reused across multiple
 * instances of the demo, such as the Box2D world, or any child nodes.  However,
 * it does not assume that any assets have finised loading.  Initialization
 * that requires assets is delayed to start().
 *
 * @return  An autoreleased root layer
 */
RagdollRoot* RagdollRoot::create() {
    RagdollRoot *node = new (std::nothrow) RagdollRoot();
    if (node && node->init()) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Initializes the demo contents. but does NOT start the game
 *
 * This initializer creates any objects that are to be reused across multiple
 * instances of the demo, such as the Box2D world, or any child nodes.  However,
 * it does not assume that any assets have finised loading.  Initialization
 * that requires assets is delayed to start().
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool RagdollRoot::init() {
    Rect rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT);
    Vec2 gravity(0,WATER_GRAVITY);

    if (RootLayer::init()) {
        // Determine the center of the screen
        Size dimen  = getContentSize();
        
        // Create the scale and notify the input handler
        _scale.set(dimen.width/rect.size.width,dimen.height/rect.size.height);
        
        _input.init(_scale);
        
        // Create the world; there are no listeners this time.
        _world = WorldController::create(rect,gravity);
        _world->retain();
        
        // Instead of a listener, create a mouse selector.
        _selector = ObstacleSelector::create(_world);
        _selector->retain();
        
        // Create the scene graph.  First the background
        _background = PolygonNode::create();
        _foreground = PolygonNode::create();
        
        // Placeholders for the unpopulated physics objects
        _worldnode = Node::create();
        _debugnode = Node::create();

        addChild(_background,0);
        addChild(_worldnode,1);
        addChild(_debugnode,2);
        addChild(_foreground,3);

        // Create localized scene manager
        _sceneidx = AssetManager::getInstance()->createScene();
        _assets = AssetManager::getInstance()->at(_sceneidx);

        _assets->attach<Texture2D>(TextureLoader::create());
        _assets->attach<Sound>(SoundLoader::create());
        _assets->start();
        
        return true;
    }
    
    return false;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void RagdollRoot::dispose() {
    if (_selector != nullptr) {
        _selector->release();
    }
    if (_world != nullptr) {
        _world->clear();
        _world->release();
    }
    removeAllChildren();
    if (_assets != nullptr) {
        _assets->unloadAll();
        AssetManager::getInstance()->deleteScene(_sceneidx);
    }
}


#pragma mark -
#pragma mark Gameplay Control

/**
 * Starts the layer, allocating initial resources
 *
 * This method is used to bootstrap the game.  It should access the asset manager
 * and perform any intialization that requires the assets.
 *
 * IMPORTANT: This method does not start up any global singletons.  That is because
 * it is not attached to the root scene of the application.
 */
void RagdollRoot::start() {
    _input.start();
    
    // Determine the center of the screen
    Size dimen  = getContentSize();
    Vec2 center(dimen.width/2.0f,dimen.height/2.0f);

    float cscale = Director::getInstance()->getContentScaleFactor();
    Texture2D* image = _assets->get<Texture2D>(BKGD_TEXTURE);
    _background->setTexture(image);
    _background->setPolygon(Rect(Vec2::ZERO,image->getContentSizeInPixels()));
    _background->setPosition(center);
    _background->setScale(cscale);
    
    image  = _assets->get<Texture2D>(FRGD_TEXTURE);
    _foreground->setTexture(image);
    _foreground->setPolygon(Rect(Vec2::ZERO,image->getContentSizeInPixels()));
    _foreground->setPosition(center);
    _foreground->setScale(cscale);
    _foreground->setOpacity(FRGD_OPACITY);
    
    // Now populate the physics objects
    populate();
    _active = true;
    setDebug(false);

    RootLayer::start(); // YOU MUST END with call to parent
}

/**
 * Stops the layer, releasing all non-reusable resources
 *
 * This method is used to clean-up any allocation that occurred in either start or
 * update.  While Cocos2d does have rudimentary garbage collection, you still have
 * to release any objects that you have retained.
 *
 * IMPORTANT: This method does not stop any global singletons.  That is because
 * it is not attached to the root scene of the application.
 */
void RagdollRoot::stop() {
    _input.stop();
    
    _selector->deselect();
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();

    RootLayer::stop();  // YOU MUST BEGIN with call to parent
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
void RagdollRoot::update(float dt) {
    RootLayer::update(dt);  // YOU MUST BEGIN with call to parent
    _input.update(dt);
    
    // Process the toggled key commands
    if (_input.didDebug()) { setDebug(!isDebug()); }
    if (_input.didReset()) { reset(); }
    if (_input.didExit())  {
        // Have a cross-fade transition
        Director::getInstance()->popScene([=](Scene* scene) { return TransitionCrossFade::create(0.5,scene); });
        // Have NO transition
        // Director::getInstance()->popScene();
    }
    
    // Move an object if touched
    if (_input.didSelect() && _selector->isSelected()) {
        _selector->moveTo(_input.getSelection());
    } else if (_input.didSelect()) {
        _selector->select(_input.getSelection());
    } else if (_selector->isSelected()) {
        _selector->deselect();
    }
    
    // Turn the physics engine crank.
    _world->update(dt);
    
    // Play a sound for each bubble
    if (_ragdoll->getBubbleGenerator()->didBubble()) {
        // Pick a sound
        int indx = 1+ (int)(CCRANDOM_0_1()*NUM_BUBBLES) % NUM_BUBBLES;
        std::string asset = std::string(SOUND_PREFIX) + (indx < 10 ? "0" : "" ) + cocos2d::to_string(indx);
        std::string key = std::string(SOUND_PREFIX) + cocos2d::to_string(_counter);
        _counter++;
        Sound* source = _assets->get<Sound>(asset);
        SoundEngine::getInstance()->playEffect(key, source);
    }
}


#pragma mark -
#pragma mark State Management

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void RagdollRoot::reset() {
    _selector->deselect();
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    
    populate();
}

/**
 * Lays out the game geography.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void RagdollRoot::populate() {
    // Make sure that we are currently using the correct scene manager
    AssetManager::getInstance()->setCurrentIndex(_sceneidx);
    
#pragma mark : Ragdoll
    _ragdoll = RagdollModel::create(DOLL_POS, _scale);
    // Just give it a blank node for its scene and debug nodes
    // The methods of ragdoll will fill it with contents
    _ragdoll->setSceneNode(Node::create());
    _ragdoll->setDebugNode(WireNode::create());
    addObstacle(_ragdoll,0);
    
    // All walls share the same texture
    Texture2D* image = _assets->get<Texture2D>("earth");
    PolygonNode* sprite;
    WireNode* draw;
    string wname = "wall";
    
#pragma mark : Wall polygon 1
    PolygonObstacle* wallobj;
    Poly2 wall1(WALL1,16);
    wall1.triangulate();
    wallobj = PolygonObstacle::create(wall1);
    wallobj->setDrawScale(_scale.x, _scale.y);
    wallobj->setName(wname);
    
    // Set the physics attributes
    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);
    
    // Add the scene graph nodes to this object
    wall1 *= _scale;
    sprite = PolygonNode::createWithTexture(image,wall1);
    wallobj->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    wallobj->setDebugNode(draw);
    addObstacle(wallobj,2);
    
    
#pragma mark : Wall polygon 2
    Poly2 wall2(WALL2,16);
    wall2.triangulate();
    wallobj = PolygonObstacle::create(wall2);
    wallobj->setDrawScale(_scale.x, _scale.y);
    wallobj->setName(wname);
    
    // Set the physics attributes
    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);
    
    // Add the scene graph nodes to this object
    wall2 *= _scale;
    sprite = PolygonNode::createWithTexture(image,wall2);
    wallobj->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    wallobj->setDebugNode(draw);
    addObstacle(wallobj,2);
}

/**
 * Immediately adds the object to the physics world
 *
 * Objects have a z-order.  This is the order they are drawn in the scene
 * graph node.  Objects with the different textures should have different
 * z-orders whenever possible.  This will cut down on the amount of drawing done
 *
 * param obj The object to add
 * param zOrder The drawing order
 *
 * @retain a reference to the obstacle
 */
void RagdollRoot::addObstacle(Obstacle* obj, int zOrder) {
    _world->addObstacle(obj);  // Implicit retain
    if (obj->getSceneNode() != nullptr) {
        _worldnode->addChild(obj->getSceneNode(),zOrder);
    }
    if (obj->getDebugNode() != nullptr) {
        _debugnode->addChild(obj->getDebugNode(),zOrder);
    }
}

/**
 * Preloads the assets needed for the game.
 */
void RagdollRoot::preload() {
    // Load the textures (Autorelease objects)
    Texture2D::TexParams params;
    params.wrapS = GL_REPEAT;
    params.wrapT = GL_REPEAT;
    params.magFilter = GL_LINEAR;
    params.minFilter = GL_NEAREST;
    
    TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();
    tloader->loadAsync(EARTH_TEXTURE, "textures/earthtile.png", params);
    tloader->loadAsync(BKGD_TEXTURE,  "textures/ragdoll/fishtank.png");
    tloader->loadAsync(FRGD_TEXTURE,  "textures/ragdoll/foreground.png");
    tloader->loadAsync(HEAD_TEXTURE,  "textures/ragdoll/ProfWhite.png");
    tloader->loadAsync(BODY_TEXTURE,  "textures/ragdoll/tux_body.png");
    tloader->loadAsync(ARM_TEXTURE,   "textures/ragdoll/tux_arm.png");
    tloader->loadAsync(FOREARM_TEXTURE,  "textures/ragdoll/tux_forearm.png");
    tloader->loadAsync(THIGH_TEXTURE,  "textures/ragdoll/tux_thigh.png");
    tloader->loadAsync(SHIN_TEXTURE,   "textures/ragdoll/tux_shin.png");
    tloader->loadAsync(BUBBLE_TEXTURE, "textures/ragdoll/bubble.png");
    SoundLoader* sloader = (SoundLoader*)_assets->access<Sound>();
    for(int ii = 1; ii <= NUM_BUBBLES; ii++) {
        std::string file = std::string("sounds/ragdoll/bubble") + (ii < 10 ? "0" : "" ) + cocos2d::to_string(ii) + ".mp3";
        std::string key  = std::string(SOUND_PREFIX) + (ii < 10 ? "0" : "" ) + cocos2d::to_string(ii);
        sloader->loadAsync(key, file);
    }
    
    
    
}
