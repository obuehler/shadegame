//
//  PFGameController.cpp
//  PlatformerDemo
//
//  This is the most important class in this demo.  This class manages the gameplay
//  for this demo.  It also handles collision detection. There is not much to do for
//  collisions; our WorldController class takes care of all of that for us.  This
//  controller mainly transforms input into gameplay.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/15
//
// This is the root, so there are a lot of includes
#include <string>
#include <cornell.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include "C_Gameplay.h"
#include "C_Input.h"
#include "M_Shadow.h"


using namespace cocos2d;
using namespace std;

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
#define WALL_VERTS 12
#define WALL_COUNT  2

float WALL[WALL_COUNT][WALL_VERTS] = {
    {16.0f, 18.0f, 16.0f, 17.0f,  1.0f, 17.0f,
      1.0f,  0.0f,  0.0f,  0.0f,  0.0f, 18.0f},
    {32.0f, 18.0f, 32.0f,  0.0f, 31.0f,  0.0f,
     31.0f, 17.0f, 16.0f, 17.0f, 16.0f, 18.0f}
};

/** The number of platforms */
#define PLATFORM_VERTS  8
#define PLATFORM_COUNT  10

/** The number of buildings */
#define BUILDING_COUNT 5   // TODO Update this with the actual number
/** The number of types of buildings */
#define BUILDING_TYPES 11

/** The outlines of all of the platforms */
float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
    { 1.0f, 3.0f, 6.0f, 3.0f, 6.0f, 2.5f, 1.0f, 2.5f},
    { 6.0f, 4.0f, 9.0f, 4.0f, 9.0f, 2.5f, 6.0f, 2.5f},
    {23.0f, 4.0f,31.0f, 4.0f,31.0f, 2.5f,23.0f, 2.5f},
    {26.0f, 5.5f,28.0f, 5.5f,28.0f, 5.0f,26.0f, 5.0f},
    {29.0f, 7.0f,31.0f, 7.0f,31.0f, 6.5f,29.0f, 6.5f},
    {24.0f, 8.5f,27.0f, 8.5f,27.0f, 8.0f,24.0f, 8.0f},
    {29.0f,10.0f,31.0f,10.0f,31.0f, 9.5f,29.0f, 9.5f},
    {23.0f,11.5f,27.0f,11.5f,27.0f,11.0f,23.0f,11.0f},
    {19.0f,12.5f,23.0f,12.5f,23.0f,12.0f,19.0f,12.0f},
    { 1.0f,12.5f, 7.0f,12.5f, 7.0f,12.0f, 1.0f,12.0f}
};

/** The goal door position */
float GOAL_POS[] = { 4.0f,14.0f};
/** The position of the spinning barrier */
float SPIN_POS[] = {13.0f,12.5f};
/** The initial position of the dude */
float DUDE_POS[] = { 2.5f, 5.0f};
/** The position of the rope bridge */
float BRIDGE_POS[] = {9.0f, 3.8f};


#pragma mark -
#pragma mark Physics Constants
/** The new heavier gravity for this world (so it is not so floaty) */
#define DEFAULT_GRAVITY 0.0f  //-14.7f
/** The density for most physics objects */
#define BASIC_DENSITY   0.0f
/** The density for a bullet */
#define HEAVY_DENSITY   10.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.0f
/** The width of the rope bridge */
#define BRIDGE_WIDTH    14.0f
/** Offset for bullet when firing */
#define BULLET_OFFSET   0.5f
/** The speed of the bullet after firing */
#define BULLET_SPEED   20.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants

/* ***********************************************************
******************** CODE ADDED FOR SHADE ********************
************************************************************ */

const string buildingTextures[] = {
	"b1",	"B1.png",	"s1",	"S1.png" ,
	"b2",	"B2.png",	"s2",	"S2.png" ,
	"b3",	"B3.png",	"s3",	"S3.png" ,
	"b4",	"B4.png",	"s4",	"S4.png" ,
	"b5",	"B5.png",	"s5",	"S5.png" ,
	"b6",	"B6.png",	"s6",	"S6.png" ,
	"b7",	"B7.png",	"s7",	"S7.png" ,
	"b8",	"B8.png",	"s8",	"S8.png" ,
	"b9",	"B9.png",	"s9",	"S9.png" ,
	"b10",	"B10.png",	"s10",	"S10.png" ,
	"b11",	"B11.png",	"s11",	"S11.png" 
};

/* ***********************************************************
***************** END OF CODE ADDED FOR SHADE ****************
************************************************************ */

#define EXPOSURE_LIMIT 5.0f // seconds before you die

/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE    "goal"
/** The key for the win door texture in the asset manager */
#define BULLET_TEXTURE  "bullet"
/** The name of a bullet (for object identification) */
#define BULLET_NAME     "bullet"
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"
/** The name of a shadow (for object identification) */
#define SHADOW_NAME "shadow"
/** The name of a building (for object identification) */
#define BUILDING_NAME "building"
/** The font for victory/failure messages */
#define MESSAGE_FONT    "retro"
/** The message for winning the game */
#define WIN_MESSAGE     "VICTORY!"
/** The color of the win message */
#define WIN_COLOR       Color3B::YELLOW
/** The message for losing the game */
#define LOSE_MESSAGE    "FAILURE!"
/** The color of the lose message */
#define LOSE_COLOR      Color3B::RED
/** The key the basic game music */
#define GAME_MUSIC      "game"
/** The key the victory game music */
#define WIN_MUSIC       "win"
/** The key the failure game music */
#define LOSE_MUSIC      "lose"
/** The sound effect for firing a bullet */
#define PEW_EFFECT      "pew"
/** The sound effect for a bullet collision */
#define POP_EFFECT      "pop"
/** The sound effect for jumping */
#define JUMP_EFFECT     "jump"
/** The volume for the music */
#define MUSIC_VOLUME    0.7f
/** The volume for sound effects */
#define EFFECT_VOLUME   0.8f

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color3B::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192


#pragma mark -
#pragma mark Initialization

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameController::GameController() :
_rootnode(nullptr),
_worldnode(nullptr),
_debugnode(nullptr),
_world(nullptr),
_avatar(nullptr),
_active(false),
_debug(false)
{
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @retain a reference to the root layer
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameController::init(RootLayer* root) {
    return init(root,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param bounds The game bounds in Box2d coordinates
 * @param scale  The difference between screen and Box2d coordinates
 * @param gravity The gravitational force on this Box2d world
 *
 * @retain a reference to the root layer
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameController::init(RootLayer* root, const Rect& rect) {
    return init(root,rect,Vec2(0,DEFAULT_GRAVITY));
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param bounds The game bounds in Box2d coordinates
 * @param scale  The difference between screen and Box2d coordinates
 * @param gravity The gravitational force on this Box2d world
 *
 * @retain a reference to the root layer
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameController::init(RootLayer* root, const Rect& rect, const Vec2& gravity) {
    // Determine the center of the screen
    Size dimen  = root->getContentSize();
    Vec2 center(dimen.width/2.0f,dimen.height/2.0f);
    
    // Create the scale and notify the input handler
    _scale.set(root->getContentSize().width/rect.size.width,
               root->getContentSize().height/rect.size.height);
    Rect screen = rect;
    screen.origin.x *= _scale.x;    screen.origin.y *= _scale.y;
    screen.size.width *= _scale.x;  screen.size.height *= _scale.y;

    _input.init(screen);
    _input.start();
    
    // Create the world; there are no listeners this time.
    _world = WorldController::create(rect,gravity);
    _world->retain();
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact* contact) {
        endContact(contact);
    };

    // Create the scene graph
    _worldnode = Node::create();
    _debugnode = Node::create();
    _winnode = Label::create();
    
    _winnode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
    _winnode->setString(WIN_MESSAGE);
    
    _winnode->setPosition(root->getContentSize().width/2.0f,
                          root->getContentSize().height/2.0f);
    _winnode->setColor(WIN_COLOR);
    setComplete(false);

    _losenode = Label::create();
    
    _losenode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
    _losenode->setString(LOSE_MESSAGE);
    
    _losenode->setPosition(root->getContentSize().width/2.0f,
                           root->getContentSize().height/2.0f);
    _losenode->setColor(LOSE_COLOR);
    setFailure(false);

	// Starting exposure is 0
	_exposure = 0.0f;

	// TODO Aaron: Declare some sprite that will act as a display for exbuildposure in PFGameController.h.
	// TODO Aaron: Initialize the sprite for exposure here.
	_timernode = Label::create();
	_timernode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
	_timernode->setString("");
	_timernode->setPosition(60, root->getContentSize().height - 30);
	_timernode->setColor(WIN_COLOR);
	_timernode->setVisible(true);

	_exposurenode = Label::create();
	_exposurenode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
	_exposurenode->setString("");
	_exposurenode->setPosition(root->getContentSize().width - 50, root->getContentSize().height - 30);
	_exposurenode->setColor(WIN_COLOR);
	_exposurenode->setVisible(true);
	// TODO Aaron: Update the sprite to display the current exposure in the update() method in this file.

    // Add everything to the root and retain
    root->addChild(_worldnode,0);
    root->addChild(_debugnode,1);
    root->addChild(_winnode,3);
    root->addChild(_losenode,4);
	//root->addChild(_timernode, 5);
	root->addChild(_exposurenode, 6);
    _rootnode = root;
    _rootnode->retain();
    
    // Now populate the physics objects
    populate();
    _active = true;
    setDebug(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 *
 * This method is different from dispose() in that it ALSO deletes the
 * input controller.
 */
GameController::~GameController() {
    dispose();
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameController::dispose() {
    if (_world != nullptr) {
        _world->clear();
        _world->release();
    }
    _worldnode = nullptr;
    _debugnode = nullptr;
    _winnode = nullptr;
	_timernode = nullptr;
	_exposurenode = nullptr;
    _rootnode->removeAllChildren();
    _rootnode->release();
    _rootnode = nullptr;
}


#pragma mark -
#pragma mark Level Creation
/**
 * Lays out the game geography.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void GameController::populate() {
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    // To convert from design resolution to real, divide positions by cscale
    float cscale = Director::getInstance()->getContentScaleFactor();
    
#pragma mark : Goal door
    Texture2D* image = _assets->get<Texture2D>(GOAL_TEXTURE);
    PolygonNode* sprite;
    WireNode* draw;
    
    // Create obstacle
    Vec2 goalPos = GOAL_POS;
    Size goalSize(image->getContentSize().width*cscale/_scale.x,
                  image->getContentSize().height*cscale/_scale.y);
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

#pragma mark : Dude
    Vec2 dudePos = DUDE_POS;
    image  = _assets->get<Texture2D>(DUDE_TEXTURE);
    _avatar = Shadow::create(dudePos,_scale*DUDE_SCALE);
    _avatar->setDrawScale(_scale);
    
    // Add the scene graph nodes to this object
    sprite = PolygonNode::createWithTexture(image);
    sprite->setScale(cscale/DUDE_SCALE);
    _avatar->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    _avatar->setDebugNode(draw);
    addObstacle(_avatar, 4); // Put this at the very front
    
    // Play the background music on a loop.
    Sound* source = _assets->get<Sound>(GAME_MUSIC);
    SoundEngine::getInstance()->playMusic(source, true, MUSIC_VOLUME);

#pragma mark : Buildings

	addBuilding("b1", "s1", Vec2(10, 10), 0.7);
	addBuilding("b5", "s5", Vec2(8, 15), 0.5);
}

/**
 * Add a rectangular building and shadow to the world.
 * bname and sname are the names used when the building and shadow textures
 * were loaded by the asset loader.
 * pos is the position of the upper left corner of the building and shadow.
 * The size of the building and shadow will be the size of their source
 * images scaled by scale.
 */
void GameController::addBuilding(const char* bname,
	                             const char* sname,
	                             const Vec2& pos,
	                             float scale) {
	auto* image = _assets->get<Texture2D>(bname);
	auto* sprite = PolygonNode::createWithTexture(image);
	sprite->setScale(scale);
	Size bs(image->getContentSize().width*scale / _scale.x,
		image->getContentSize().height*scale / _scale.y);
	Vec2 bpos(pos.x + bs.width / 2, pos.y - bs.height / 2);
	auto* box = BoxObstacle::create(bpos, bs);
	box->setDrawScale(_scale.x, _scale.y);
	box->setName(std::string(BUILDING_NAME));
	box->setBodyType(b2_staticBody);
	box->setDensity(BASIC_DENSITY);
	box->setFriction(BASIC_FRICTION);
	box->setRestitution(BASIC_RESTITUTION);
	box->setSceneNode(sprite);
	auto* draw = WireNode::create();
	draw->setColor(DEBUG_COLOR);
	draw->setOpacity(DEBUG_OPACITY);
	box->setDebugNode(draw);
	addObstacle(box, 2);

	image = _assets->get<Texture2D>(sname);
	sprite = PolygonNode::createWithTexture(image);
	sprite->setScale(scale);
	Size ss(image->getContentSize().width*scale / _scale.x,
		image->getContentSize().height*scale / _scale.y);
	Vec2 spos(pos.x + ss.width / 2, pos.y - ss.height / 2);
	box = BoxObstacle::create(spos, ss);
	box->setDrawScale(_scale.x, _scale.y);
	box->setName(std::string(SHADOW_NAME));
	box->setBodyType(b2_staticBody);
	box->setDensity(0);
	box->setFriction(0);
	box->setRestitution(0);
	box->setSensor(true);
	box->setSceneNode(sprite);
	draw = WireNode::create();
	draw->setColor(DEBUG_COLOR);
	draw->setOpacity(DEBUG_OPACITY);
	box->setDebugNode(draw);
	addObstacle(box, 1);
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
void GameController::addObstacle(Obstacle* obj, int zOrder) {
    _world->addObstacle(obj);  // Implicit retain
    if (obj->getSceneNode() != nullptr) {
        _worldnode->addChild(obj->getSceneNode(),zOrder);
    }
    if (obj->getDebugNode() != nullptr) {
        _debugnode->addChild(obj->getDebugNode(),zOrder);
    }
}


#pragma mark -
#pragma mark Gameplay Handling

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameController::reset() {
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    
	_exposure = 0;
    setFailure(false);
    setComplete(false);
    populate();
    
    
    
}

/**
 * Sets whether the level is completed.
 *
 * If true, the level will advance after a countdown
 *
 * @param value whether the level is completed.
 */
void GameController::setComplete(bool value) {
    _complete = value;
    if (value) {
        Sound* source = _assets->get<Sound>(WIN_MUSIC);
        SoundEngine::getInstance()->playMusic(source,false,MUSIC_VOLUME);
        _winnode->setVisible(true);
        _countdown = EXIT_COUNT;
    } else {
        _winnode->setVisible(false);
        _countdown = -1;
    }
}

/**
 * Sets whether the level is failed.
 *
 * If true, the level will reset after a countdown
 *
 * @param value whether the level is failed.
 */
void GameController::setFailure(bool value) {
    _failed = value;
    if (value) {
        Sound* source = _assets->get<Sound>(LOSE_MUSIC);
        SoundEngine::getInstance()->playMusic(source,false,MUSIC_VOLUME);
        _losenode->setVisible(true);
        _countdown = EXIT_COUNT;
    } else {
        _losenode->setVisible(false);
        _countdown = -1;
    }
    
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
void GameController::update(float dt) {
	_input.update(dt);

	// Process the toggled key commands
	if (_input.didDebug()) { setDebug(!isDebug()); }
	if (_input.didReset()) { reset(); }
	if (_input.didExit()) {
		_rootnode->shutdown();
	}

	if (!_failed && !_complete) {
	
		// Process the movement
		_avatar->setHorizontalMovement(_input.getHorizontal()*_avatar->getForce());
		_avatar->setVerticalMovement(_input.getVertical()*_avatar->getForce());
		//_avatar->setJumping( _input.didJump());
		_avatar->applyForce();

	}
	else {
		_avatar->setVX(0.0f);
		_avatar->setVY(0.0f);
		_avatar->setAngularVelocity(0.0f);
	}

    /* if (_avatar->isJumping()) {
        Sound* source = _assets->get<Sound>(JUMP_EFFECT);
        SoundEngine::getInstance()->playEffect(JUMP_EFFECT,source,false,EFFECT_VOLUME);
     */

    // Turn the physics engine crank.
    _world->update(dt);
    
    // Since items may be deleted, garbage collect
    _world->garbageCollect();
    
    // Check for exposure or cover
	if (_inShadow)
		_exposure = max(_exposure - dt, 0.0f);
	else
		_exposure += dt;

	if (!_failed && !_complete) {
		_exposurenode->setString(cocos2d::to_string((int)(
			(_exposure / EXPOSURE_LIMIT) * 100)) + "%");
		if (_exposure > EXPOSURE_LIMIT)
			setFailure(true);
	}

    
    // Reset the game if we win or lose.
    if (_countdown > 0) {
        _countdown--;
    } else if (_countdown == 0) {
        reset();
    }
}

/**
 * Add a new bullet to the world and send it in the right direction.
 */
void GameController::createBullet() {
    float offset = BULLET_OFFSET;
    Vec2 pos = _avatar->getPosition();
    pos.x += (_avatar->isFacingRight() ? offset : -offset);
    
    Texture2D* image = _assets->get<Texture2D>(BULLET_TEXTURE);
    float radius = 0.5f*image->getContentSize().width/_scale.x;
    
    WheelObstacle* bullet = WheelObstacle::create(pos, radius);
    bullet->setName(BULLET_NAME);
    bullet->setDensity(HEAVY_DENSITY);
    bullet->setDrawScale(_scale);
    bullet->setBullet(true);
    bullet->setGravityScale(0);
    
    float cscale = Director::getInstance()->getContentScaleFactor();
    PolygonNode* sprite = PolygonNode::createWithTexture(image);
    sprite->setScale(cscale);
    bullet->setSceneNode(sprite);
    
    WireNode* draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    bullet->setDebugNode(draw);

    // Compute position and velocity
    float speed  = (_avatar->isFacingRight() ? BULLET_SPEED : -BULLET_SPEED);
    bullet->setVX(speed);
    addObstacle(bullet,5);
    
    Sound* source = _assets->get<Sound>(PEW_EFFECT);
    SoundEngine::getInstance()->playEffect(PEW_EFFECT,source, false, EFFECT_VOLUME, true);
}

/**
 * Remove a new bullet from the world.
 *
 * @param  bullet   the bullet to remove
 */
void GameController::removeBullet(Obstacle* bullet) {
    _worldnode->removeChild(bullet->getSceneNode());
    _debugnode->removeChild(bullet->getDebugNode());
    bullet->markRemoved(true);
    
    Sound* source = _assets->get<Sound>(POP_EFFECT);
    SoundEngine::getInstance()->playEffect(POP_EFFECT,source,false,EFFECT_VOLUME, true);
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
void GameController::beginContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();
    
    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();
    
    void* fd1 = fix1->GetUserData();
    void* fd2 = fix2->GetUserData();
    
    Obstacle* bd1 = (Obstacle*)body1->GetUserData();
    Obstacle* bd2 = (Obstacle*)body2->GetUserData();
    
    // Test bullet collision with world
    if (bd1->getName() == BULLET_NAME && bd2 != _avatar) {
        removeBullet(bd1);
    } else if (bd2->getName() == BULLET_NAME && bd1 != _avatar) {
        removeBullet(bd2);
    }

    /* // See if we have landed on the ground.
    if ((_avatar->getSensorName() == fd2 && _avatar != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar != bd2)) {
        _avatar->setGrounded(true);
		
        // Could have more than one ground
        _sensorFixtures.emplace(_avatar == bd1 ? fix2 : fix1);
    } */

	// See if we are in a shadow. TODO THIS IS HORRIBLY INEFFICIENT, WE'LL CHANGE THIS
	if ((_avatar->getSensorName() == fd2 && bd1->getName().find(SHADOW_NAME) == 0) ||
		(_avatar->getSensorName() == fd1 && bd2->getName().find(SHADOW_NAME) == 0)) {
		_inShadow = true;

		// Could have more than one ground
		_sensorFixtures.emplace(_avatar == bd1 ? fix2 : fix1);
	}
    
    // If we hit the "win" door, we are done
    if((bd1 == _avatar   && bd2 == _goalDoor) ||
       (bd1 == _goalDoor && bd2 == _avatar)) {
        setComplete(true);
    }
}

/**
 * Callback method for the start of a collision
 *
 * This method is called when two objects cease to touch.  The main use of this method
 * is to determine when the characer is NOT on the ground.  This is how we prevent
 * double jumping.
 */
void GameController::endContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();
    
    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();
    
    void* fd1 = fix1->GetUserData();
    void* fd2 = fix2->GetUserData();
    
    void* bd1 = body1->GetUserData();
    void* bd2 = body2->GetUserData();

    
    if ((_avatar->getSensorName() == fd2 && _avatar != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar != bd2)) {
        _sensorFixtures.erase(_avatar == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty()) {
           // _avatar->setGrounded(false);
			_inShadow = false;
        }
    }
}


#pragma mark -
#pragma mark Post-Collision Processing
/**
* Determines the type of the power-up collided with and applies the
* associated effects to the shadow or the game.
*
* @param	powerUp		The power-up object encountered
*/
void GameController::applyPowerup(const Powerup& powerup) {
	switch (powerup.getType()) {
	// TODO implement the effects for each type	
	default:
		return;
	}
}


#pragma mark -
#pragma mark Asset Management
/**
 * Preloads the assets needed for the game.
 */
void GameController::preload() {
    // Load the textures (Autorelease objects)
    Texture2D::TexParams params;
    params.wrapS = GL_REPEAT;
    params.wrapT = GL_REPEAT;
    params.magFilter = GL_LINEAR;
    params.minFilter = GL_NEAREST;

    _assets = AssetManager::getInstance()->getCurrent();
    TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();
	
	/* ***********************************************************
	******************** CODE ADDED FOR SHADE ********************
	************************************************************ */

	for (int building_index = 0; building_index < BUILDING_TYPES; building_index++) {
		tloader->loadAsync(buildingTextures[building_index * 4],
			"textures/buildings/" + buildingTextures[building_index * 4 + 1]);
		tloader->loadAsync(buildingTextures[building_index * 4 + 2],
			"textures/buildings/" + buildingTextures[building_index * 4 + 3]);
	}
	
	/* ***********************************************************
	***************** END OF CODE ADDED FOR SHADE ****************
	************************************************************ */

    tloader->loadAsync(EARTH_TEXTURE,   "textures/earthtile.png", params);
    tloader->loadAsync(DUDE_TEXTURE,    "textures/ShadeDude.png");
    tloader->loadAsync(BULLET_TEXTURE,  "textures/bullet.png");
    tloader->loadAsync(GOAL_TEXTURE,    "textures/goaldoor.png");
    _assets->loadAsync<Sound>(GAME_MUSIC,   "sounds/DD_Main.mp3");
    _assets->loadAsync<Sound>(WIN_MUSIC,    "sounds/DD_Victory.mp3");
    _assets->loadAsync<Sound>(LOSE_MUSIC,   "sounds/DD_Failure.mp3");
    _assets->loadAsync<Sound>(JUMP_EFFECT,  "sounds/jump.mp3");
    _assets->loadAsync<Sound>(PEW_EFFECT,   "sounds/pew.mp3");
    _assets->loadAsync<Sound>(POP_EFFECT,   "sounds/plop.mp3");
    _assets->loadAsync<TTFont>(MESSAGE_FONT,"fonts/RetroGame.ttf");
}

/**
* Clear all memory when exiting.
*/
void GameController::stop() {
}
