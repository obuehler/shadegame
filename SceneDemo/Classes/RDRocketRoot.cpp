//
//  RDRocketRoot.cpp
//  RocketDemo
//
//  This class is a reimagining of the RocketDemo.  Instead of splitting the layer and the
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
#include "RDRocketRoot.h"
#include "RDRocketInput.h"
#include "RDRocketModel.h"
#include <cornell.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>

#include <string>
#include <iostream>
#include <sstream>

using namespace cocos2d;
using namespace std;


#pragma mark -
#pragma mark Level Geography

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f
/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -4.9f

/** To automate the loading of crate files */
#define NUM_CRATES 2

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
static float WALL1[] = { 0.0f, 18.0f, 16.0f, 18.0f, 16.0f, 17.0f,
                         8.0f, 15.0f,  1.0f, 17.0f,  2.0f,  7.0f,
                         3.0f,  5.0f,  3.0f,  1.0f, 16.0f,  1.0f,
                        16.0f,  0.0f,  0.0f,  0.0f};
static float WALL2[] = {32.0f, 18.0f, 32.0f,  0.0f, 16.0f,  0.0f,
                        16.0f,  1.0f, 31.0f,  1.0f, 30.0f, 10.0f,
                        31.0f, 16.0f, 16.0f, 17.0f, 16.0f, 18.0f};
static float WALL3[] = { 4.0f, 10.5f,  8.0f, 10.5f,
                         8.0f,  9.5f,  4.0f,  9.5f};

/** The positions of the crate pyramid */
static float BOXES[] = { 14.5f, 14.25f,
                         13.0f, 12.00f, 16.0f, 12.00f,
                         11.5f,  9.75f, 14.5f,  9.75f, 17.5f, 9.75f,
                         13.0f,  7.50f, 16.0f,  7.50f,
                         11.5f,  5.25f, 14.5f,  5.25f, 17.5f, 5.25f,
                         10.0f,  3.00f, 13.0f,  3.00f, 16.0f, 3.00f, 19.0f, 3.0f};

/** The initial rocket position */
static float ROCK_POS[] = {24,  4};
/** The goal door position */
static float GOAL_POS[] = { 6, 12};


#pragma mark Assset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE       "earth"
/** The key for the rocket texture in the asset manager */
#define ROCK_TEXTURE        "rocket"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE        "goal"
/** The key prefix for the multiple crate assets */
#define CRATE_PREFIX        "crate"
/** The key for the fire textures in the asset manager */
#define MAIN_FIRE_TEXTURE   "flames"
#define RGHT_FIRE_TEXTURE   "flames-right"
#define LEFT_FIRE_TEXTURE   "flames-left"

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color3B::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192

/** The key for collisions sounds */
#define COLLISION_SOUND     "bump"
/** The key for the main afterburner sound */
#define MAIN_FIRE_SOUND     "burn"
/** The key for the right afterburner sound */
#define RGHT_FIRE_SOUND     "sounds/sideburner-left.mp3"
/** The key for the left afterburner sound */
#define LEFT_FIRE_SOUND     "sounds/sideburner-right.mp3"

/** The key for the font reference */
#define PRIMARY_FONT        "retro"
#define DEFAULT_FONT_SIZE   64.0f

#pragma mark Physics Constants

// Physics constants for initialization
/** Density of non-crate objects */
#define BASIC_DENSITY       0.0f
/** Density of the crate objects */
#define CRATE_DENSITY       1.0f
/** Friction of non-crate objects */
#define BASIC_FRICTION      0.1f
/** Friction of the crate objects */
#define CRATE_FRICTION      0.2f
/** Angular damping of the crate objects */
#define CRATE_DAMPING       1.0f
/** Collision restitution for all objects */
#define BASIC_RESTITUTION   0.1f
/** Threshold for generating sound on collision */
#define SOUND_THRESHOLD     3


#pragma mark -
#pragma mark Initialization

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 */
RocketRoot::RocketRoot() :
_worldnode(nullptr),
_debugnode(nullptr),
_world(nullptr),
_goalDoor(nullptr),
_rocket(nullptr),
_assets(nullptr),
_active(false),
_complete(false),
_debug(false)
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
RocketRoot* RocketRoot::create() {
    RocketRoot *node = new (std::nothrow) RocketRoot();
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
bool RocketRoot::init() {
    Rect rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT);
    Vec2 gravity(0,DEFAULT_GRAVITY);
    
    if (RootLayer::init()) {
        _input.init();
    
        // Create the world and attach the listeners.
        _world = WorldController::create(rect,gravity);
        _world->retain();
        _world->activateCollisionCallbacks(true);
        _world->onBeginContact = [this](b2Contact* contact) {
            beginContact(contact);
        };
        _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
            beforeSolve(contact,oldManifold);
        };
    
        _scale.set(getContentSize().width/rect.size.width,
                   getContentSize().height/rect.size.height);
        
        _active = true;
        _complete = false;
        
        // Create the scene graph
        _worldnode = Node::create();
        _debugnode = Node::create();
        _winnode = Label::create();
        
        addChild(_worldnode,0);
        addChild(_debugnode,1);
        addChild(_winnode,3);
        
        // Create localized scene manager
        _sceneidx = AssetManager::getInstance()->createScene();
        _assets = AssetManager::getInstance()->at(_sceneidx);
            
        FontLoader* fonts = FontLoader::create();
        fonts->setDefaultSize(DEFAULT_FONT_SIZE);
        _assets->attach<TTFont>(fonts);
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
void RocketRoot::dispose() {
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
void RocketRoot::start() {
    _input.start();

    _winnode->setTTFConfig(_assets->get<TTFont>(PRIMARY_FONT)->getTTF());
    _winnode->setString("VICTORY!");
    
    _winnode->setPosition(getContentSize().width/2.0f,getContentSize().height/2.0f);
    _winnode->setColor(DEBUG_COLOR);
    _winnode->setVisible(false);

    setDebug(false);
    populate();
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
void RocketRoot::stop() {
    _input.stop();
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    setComplete(false);
    RootLayer::stop();  // YOU MUST BEGIN with call to parent
}

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
void RocketRoot::update(float dt) {
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
    
    // Apply the force to the rocket
    _rocket->setFX(_input.getHorizontal() * _rocket->getThrust());
    _rocket->setFY(_input.getVertical() * _rocket->getThrust());
    _rocket->applyForce();
    
    // Animate the three burners
    updateBurner(RocketModel::Burner::MAIN, _rocket->getFY() > 1);
    updateBurner(RocketModel::Burner::LEFT, _rocket->getFX() > 1);
    updateBurner(RocketModel::Burner::RIGHT, _rocket->getFX() < -1);
    
    // Turn the physics engine crank.
    _world->update(dt);
}

/**
 * Updates that animation for a single burner
 *
 * This method is here instead of the the rocket model because of our philosophy
 * that models should always be lightweight.  Animation includes sounds and other
 * assets that we do not want to process in the model
 *
 * @param  burner   The rocket burner to animate
 * @param  on       Whether to turn the animation on or off
 */
void RocketRoot::updateBurner(RocketModel::Burner burner, bool on) {
    string sound = _rocket->getBurnerSound(burner);
    if (on) {
        _rocket->animateBurner(burner, true);
        if (!SoundEngine::getInstance()->isActiveEffect(sound) && sound.size() > 0) {
            Sound* source = _assets->get<Sound>(sound);
            SoundEngine::getInstance()->playEffect(sound,source,true);
        }
    } else {
        _rocket->animateBurner(burner, false);
        if (SoundEngine::getInstance()->isActiveEffect(sound)) {
            SoundEngine::getInstance()->stopEffect(sound);
        }
    }
    
}


#pragma mark -
#pragma mark State Management
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void RocketRoot::reset() {
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    
    setComplete(false);
    populate();
}

/**
 * Lays out the game geography.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void RocketRoot::populate() {
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    // To convert from design resolution to real, divide positions by cscale
    float cscale = Director::getInstance()->getContentScaleFactor();
    // Note that this is different from _scale, which is the physics scale
    
    // THIS DOES NOT FIX ASPECT RATIO PROBLEMS
    // If you are using a device with a 3:2 aspect ratio, you will need to
    // completely redo the level layout.  We can help if this is an issue.
    
    // Make sure the asset manager is at the right place
    AssetManager::getInstance()->setCurrentIndex(_sceneidx);
    
#pragma mark : Goal door
    Texture2D* image = _assets->get<Texture2D>(GOAL_TEXTURE);
    PolygonNode* sprite;
    WireNode* draw;
    
    // Create obstacle
    Vec2 goalPos = ((Vec2)GOAL_POS);
    sprite = PolygonNode::createWithTexture(image);
    Size goalSize(image->getContentSize().width/_scale.x,
                  image->getContentSize().height/_scale.y);
    _goalDoor = BoxObstacle::create(goalPos,goalSize);
    _goalDoor->setDrawScale(_scale.x, _scale.y);
    
    // Set the physics attributes
    _goalDoor->setBodyType(b2_staticBody);
    _goalDoor->setDensity(0.0f);
    _goalDoor->setFriction(0.0f);
    _goalDoor->setRestitution(0.0f);
    _goalDoor->setSensor(true);
    
    // Add the scene graph nodes to this object
    sprite = PolygonNode::createWithTexture(image);
    sprite->setScale(cscale);
    _goalDoor->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    _goalDoor->setDebugNode(draw);
    addObstacle(_goalDoor, 0); // Put this at the very back
    
    
#pragma mark : Wall polygon 1
    // Create ground pieces
    // All walls share the same texture
    image  = _assets->get<Texture2D>(EARTH_TEXTURE);
    string wname = "wall";
    
    PolygonObstacle* wallobj;
    Poly2 wall1(WALL1,22);
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
    addObstacle(wallobj,1);  // All walls share the same texture
    
    
#pragma mark : Wall polygon 2
    Poly2 wall2(WALL2,18);
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
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    wallobj->setDebugNode(draw);
    addObstacle(wallobj,1);
    
    
#pragma mark : Wall polygon 3
    Poly2 wall3(WALL3,8);
    wall3.triangulate();
    wallobj = PolygonObstacle::create(wall3);
    wallobj->setDrawScale(_scale.x, _scale.y);
    wallobj->setName(wname);
    
    // Set the physics attributes
    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);
    
    // Add the scene graph nodes to this object
    wall3 *= _scale;
    sprite = PolygonNode::createWithTexture(image,wall3);
    wallobj->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    wallobj->setDebugNode(draw);
    addObstacle(wallobj,1);
    
    
#pragma mark : Crates
    for (int ii = 0; ii < 15; ii++) {
        // Pick a crate and random and generate the key
        int indx = (CCRANDOM_0_1() > 0.5f ? 2 : 1);
        stringstream ss;
        ss << CRATE_PREFIX << (indx < 10 ? "0" : "" ) << indx;
        
        // Create the sprite for this crate
        image  = _assets->get<Texture2D>(ss.str());
        sprite = PolygonNode::createWithTexture(image);
        sprite->setScale(cscale);
        
        Vec2 boxPos(BOXES[2*ii], BOXES[2*ii+1]);
        Size boxSize(image->getContentSize().width*cscale/_scale.x,image->getContentSize().height*cscale/_scale.y);
        BoxObstacle* crate = BoxObstacle::create(boxPos,boxSize);
        crate->setDrawScale(_scale.x, _scale.y);
        crate->setName(ss.str());
        crate->setAngleSnap(0);     // Snap to the nearest degree
        
        // Set the physics attributes
        crate->setDensity(CRATE_DENSITY);
        crate->setFriction(CRATE_FRICTION);
        crate->setAngularDamping(CRATE_DAMPING);
        crate->setRestitution(BASIC_RESTITUTION);
        
        // Add the scene graph nodes to this object
        crate->setSceneNode(sprite);
        
        draw = WireNode::create();
        draw = WireNode::create();
        draw->setColor(DEBUG_COLOR);
        draw->setOpacity(DEBUG_OPACITY);
        crate->setDebugNode(draw);
        addObstacle(crate,1+indx);  // PUT SAME TEXTURES IN SAME LAYER!!!
    }
    
#pragma mark : Rocket
    Vec2 rockPos = ((Vec2)ROCK_POS);
    image  = _assets->get<Texture2D>(ROCK_TEXTURE);
    Size rockSize(image->getContentSize().width*cscale/_scale.x,image->getContentSize().height*cscale/_scale.y);
    
    _rocket = RocketModel::create(rockPos,rockSize);
    _rocket->setDrawScale(_scale.x, _scale.y);
    _rocket->setShipTexture(ROCK_TEXTURE);
    _rocket->setBurnerStrip(RocketModel::Burner::MAIN,  MAIN_FIRE_TEXTURE);
    _rocket->setBurnerStrip(RocketModel::Burner::LEFT,  LEFT_FIRE_TEXTURE);
    _rocket->setBurnerStrip(RocketModel::Burner::RIGHT, RGHT_FIRE_TEXTURE);
    _rocket->setBurnerSound(RocketModel::Burner::MAIN,  MAIN_FIRE_SOUND);
    _rocket->setBurnerSound(RocketModel::Burner::LEFT,  LEFT_FIRE_SOUND);
    _rocket->setBurnerSound(RocketModel::Burner::RIGHT, RGHT_FIRE_SOUND);
    
    // Create the polygon node (empty, as the model will initialize)
    sprite = PolygonNode::create();
    sprite->setScale(cscale);
    _rocket->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    _rocket->setDebugNode(draw);
    addObstacle(_rocket,3);
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
 */
void RocketRoot::addObstacle(Obstacle* obj, int zOrder) {
    _world->addObstacle(obj);
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
void RocketRoot::preload() {
    // Load the textures (Autorelease objects)
    Texture2D::TexParams params;
    params.wrapS = GL_REPEAT;
    params.wrapT = GL_REPEAT;
    params.magFilter = GL_LINEAR;
    params.minFilter = GL_NEAREST;
    
    TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();
    _assets->loadAsync<TTFont>(PRIMARY_FONT, "fonts/RetroGame.ttf");
    tloader->loadAsync(EARTH_TEXTURE,       "textures/earthtile.png", params);
    tloader->loadAsync(GOAL_TEXTURE,        "textures/rocket/goaldoor.png");
    tloader->loadAsync(ROCK_TEXTURE,        "textures/rocket/rocket.png");
    tloader->loadAsync(MAIN_FIRE_TEXTURE,   "textures/rocket/flames.png");
    tloader->loadAsync(LEFT_FIRE_TEXTURE,   "textures/rocket/flames-left.png");
    tloader->loadAsync(RGHT_FIRE_TEXTURE,   "textures/rocket/flames-right.png");
    for(int ii = 1; ii <= NUM_CRATES; ii++) {
        stringstream ss, tt;
        ss << "textures/rocket/crate" << (ii < 10 ? "0" : "" ) << ii << ".png";
        tt << CRATE_PREFIX << (ii < 10 ? "0" : "" ) << ii;
        tloader->loadAsync(tt.str(), ss.str());
    }
    _assets->loadAsync<Sound>(MAIN_FIRE_SOUND,  "sounds/rocket/afterburner.mp3");
    _assets->loadAsync<Sound>(LEFT_FIRE_SOUND,  "sounds/rocket/sideburner-left.mp3");
    _assets->loadAsync<Sound>(RGHT_FIRE_SOUND,  "sounds/rocket/sideburner-right.mp3");
    _assets->loadAsync<Sound>(COLLISION_SOUND,  "sounds/rocket/bump.mp3");
}


#pragma mark -
#pragma mark Collision Handling

/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */
void RocketRoot::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    
    // If we hit the "win" door, we are done
    if((body1->GetUserData() == _rocket && body2->GetUserData() == _goalDoor) ||
       (body1->GetUserData() == _goalDoor && body2->GetUserData() == _rocket)) {
        setComplete(true);
    }
}

/**
 * Handles any modifications necessary before collision resolution
 *
 * This method is called just before Box2D resolves a collision.  We use this method
 * to implement sound on contact, using the algorithms outlined in Ian Parberry's
 * "Introduction to Game Physics with Box2D".
 *
 * @param  contact  	The two bodies that collided
 * @param  oldManfold  	The collision manifold before contact
 */
void RocketRoot::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    float speed = 0;
    
    // Use Ian Parberry's method to compute a speed threshold
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    for(int ii =0; ii < 2; ii++) {
        if (state2[ii] == b2_addState) {
            b2Vec2 wp = worldManifold.points[0];
            b2Vec2 v1 = body1->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 v2 = body2->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 dv = v1-v2;
            speed = b2Dot(dv,worldManifold.normal);
        }
    }
    
    // Play a sound if above threshold
    if (speed > SOUND_THRESHOLD) {
        string key = ((Obstacle*)body1->GetUserData())->getName()+((Obstacle*)body2->GetUserData())->getName();
        Sound* source = _assets->get<Sound>(COLLISION_SOUND);
        SoundEngine::getInstance()->playEffect(key, source, false, 0.5);
    }
}
