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
#include "C_Gameplay.h"
#include "C_Input.h"
#include "M_Shadow.h"
#include "M_MovingObject.h"
#include "ActionQueue.h"


using namespace cocos2d;
using namespace std;

#pragma mark -
#pragma mark Level Geography

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
float DUDE_POS[] = { 2.5f, 7.5f};
/** The position of the rope bridge */
float BRIDGE_POS[] = {9.0f, 3.8f};

#pragma mark -
#pragma mark Physics Constants
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
/** The integer used as the action tag for the layer movement */
#define FOLLOW_ACTION_TAG 5

/** The ratio of the y-radius of the area in the center that stops the
 * character to the half of the height of the screen (or x-radius and width) */
#define DEADSPACE_SIZE 0.2f

/** Horizontal positioning of the exposure bar */
#define EXPOSURE_X_OFFSET 220
/** Vertical positioning of the exposure bar */
#define EXPOSURE_Y_OFFSET 50

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

/** Seconds before death due to exposure */
#define EXPOSURE_LIMIT 5.0f
/** Ratio of exposure cooldown speed to exposure increase speed */
#define EXPOSURE_COOLDOWN_RATIO 0.5f

/** The key for the (temporary) background image */
#define BACKGROUND_IMAGE "bimage"
/** The key for the pedestrian object texture in the asset manager */
#define PEDESTRIAN_TEXTURE "pimage"
/** The key for the pedestrian shadow texture in the asset manager */
#define PEDESTRIAN_SHADOW_TEXTURE "psimage"
/** The key for the car object texture in the asset manager */
#define CAR_TEXTURE "cimage"
/** The key for the car shadow texture in the asset manager */
#define CAR_SHADOW_TEXTURE "csimage"

#define PLANT1_TEXTURE "plt1image"
#define PLANT1S_TEXTURE "plt1simage"
#define PLANT2_TEXTURE "plt2image"
#define PLANT2S_TEXTURE "plt2simage"
/** The key for the exposure bar texture in the asset manager */
#define EXPOSURE_BAR	"ebar"
/** The key for the exposure bar frame texture in the asset manager */
#define EXPOSURE_FRAME	"eframe"
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
/* The tag for the object in an object-shadow pair, used for loading */
#define OBJECT_TAG "_o"
/* The tag for the shadow in an object-shadow pair, used for loading */
#define SHADOW_TAG "_s"
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

/** Static object types file path */
#define STATIC_OBJECTS "constants/static_objects.shadc"

b2Filter GameController::characterFilter = b2Filter(CHARACTER_BIT, OBJECT_BIT, 0);
b2Filter GameController::objectFilter = b2Filter(OBJECT_BIT, CHARACTER_BIT | CASTER_BIT, 1);
b2Filter GameController::casterFilter = b2Filter(CASTER_BIT, CHARACTER_SENSOR_BIT | OBJECT_BIT, 1);
b2Filter GameController::shadowFilter = b2Filter(SHADOW_BIT, CHARACTER_SENSOR_BIT, -1);
b2Filter GameController::characterSensorFilter = b2Filter(CHARACTER_SENSOR_BIT, SHADOW_BIT | CASTER_BIT, -2);


#pragma mark -
#pragma mark Initialization

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameController::GameController(string& levelkey, string& levelpath) :
_rootnode(nullptr),
_worldnode(nullptr),
_backgroundnode(nullptr),
_debugnode(nullptr),
_active(false),
_debug(false),
_levelKey(levelkey),
_levelPath(levelpath)
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
 * with the Box2d coordinates.  This initializer uses the default scale *
 * @retain a reference to the root layer
 * @return  true if the controller is initialized properly, false otherwise.
 */
/*bool GameController::init(RootLayer* root) {
    return init(root,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT));
}*/

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
 * @param root	The RootLayer creating this GameController
 *
 * @retain a reference to the root layer
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameController::init(RootLayer* root) {

	_level = _assets->get<LevelInstance>(_levelKey);

	if (_level == nullptr || root == nullptr) {
		return false;
	}

	_level->retain();

    // Determine the center of the screen
    Size dimen  = root->getContentSize();
    Vec2 center(dimen.width/2.0f,dimen.height/2.0f);
   
	/* Rect screen = Rect(Vec2(0,0), _level->_size);
   
    screen.origin.x *= root->getContentSize().width / screen.size.width;
	screen.origin.y *= root->getContentSize().height / screen.size.height;
    screen.size.width *= root->getContentSize().width / screen.size.width;
	screen.size.height *= root->getContentSize().height / screen.size.height; */

    _input.init(Rect(0.0f, 0.0f, root->getContentSize().width, root->getContentSize().height));
    _input.start();

	_physics.init(_level->_size);

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

	_backgroundnode = PolygonNode::createWithTexture(_assets->get<Texture2D>(BACKGROUND_IMAGE));
	_backgroundnode->setAnchorPoint(Vec2(0, 0));
	_backgroundnode->setPosition(0, 0);
	{
		float cascale = Director::getInstance()->getContentScaleFactor();
		_backgroundnode->setScale((_level->_size.width * BOX2D_SCALE) / _backgroundnode->getContentSize().width,
			(_level->_size.height * BOX2D_SCALE) / _backgroundnode->getContentSize().height);
	}
	_backgroundnode->setVisible(true);

	// Starting exposure is 0
	_exposure = 0.0f;

	_timernode = Label::create();
	_timernode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
	_timernode->setString("");
	_timernode->setPosition(60, root->getContentSize().height - 30);
	_timernode->setColor(WIN_COLOR);
	_timernode->setVisible(true);

	_exposurebar = PolygonNode::createWithTexture(_assets->get<Texture2D>(EXPOSURE_BAR));
	_exposurebar->setAnchorPoint(Vec2(0, 0));
	_exposurebar->setPosition(root->getContentSize().width - EXPOSURE_X_OFFSET, root->getContentSize().height - EXPOSURE_Y_OFFSET);
	_exposurebar->setScale(Director::getInstance()->getContentScaleFactor());
	_exposurebar->setVisible(true);
	
	_exposurepoly = Poly2(Rect(Vec2(0.0f, 0.0f), _exposurebar->getTexture()->getContentSize()));

	_exposureframe = Sprite::createWithTexture(_assets->get<Texture2D>(EXPOSURE_FRAME));
	_exposureframe->setAnchorPoint(Vec2(0, 0));
	_exposureframe->setPosition(root->getContentSize().width - EXPOSURE_X_OFFSET - 3, root->getContentSize().height - EXPOSURE_Y_OFFSET - 3);
	_exposureframe->setScale(Director::getInstance()->getContentScaleFactor());
	_exposureframe->setVisible(true);

	_exposurenode = Label::create();
	_exposurenode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
	_exposurenode->setString("");
	_exposurenode->setPosition(root->getContentSize().width - EXPOSURE_X_OFFSET, root->getContentSize().height - EXPOSURE_Y_OFFSET);
	_exposurenode->setColor(WIN_COLOR);
	//_exposurenode->setVisible(true);

    // Add everything to the root and retain
	root->addChild(_backgroundnode, 0);
	root->addChild(_worldnode,1);
    root->addChild(_debugnode,2);
    root->addChild(_winnode,3);
    root->addChild(_losenode,4);
	//root->addChild(_timernode, 5);
	root->addChild(_exposurenode, 6);
	root->addChild(_exposurebar, 6);
	root->addChild(_exposureframe, 7);
    _rootnode = root;
    _rootnode->retain();

    // Now populate the physics objects
    populate();
	_worldnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode())); // TODO change when lazy camera implemented
	_debugnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode())); // TODO change when lazy camera implemented
	_backgroundnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode()));
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
	_physics.dispose();
    _worldnode = nullptr;
    _debugnode = nullptr;
    _winnode = nullptr;
	_timernode = nullptr;
	_exposurenode = nullptr;
	_exposurebar = nullptr;
	_exposureframe = nullptr;
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

	Vec2 scale(BOX2D_SCALE, BOX2D_SCALE);

	// We need to know the content scale for resolution independence
	// If the device is higher resolution than 1024x576, Cocos2d will scale it
	// This was set as the design resolution in AppDelegate
	// To convert from design resolution to real, divide positions by cscale
	float cscale = Director::getInstance()->getContentScaleFactor();
    
#pragma mark : Goal door
    Texture2D* image = _assets->get<Texture2D>(GOAL_TEXTURE);
    
    // Create obstacle
   /* Vec2 goalPos = GOAL_POS;
    Size goalSize(image->getContentSize().width*cscale/_scale.x,
                  image->getContentSize().height*cscale/_scale.y);
    _goalDoor = BoxObstacle::create(goalPos, goalSize, &_casterFilter);
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
    _goalDoor->setDebugNode(draw); */


	((PolygonNode*)(_level->_casterPos.object->getObject()->getSceneNode()))->initWithTexture(image);
	((PolygonNode*)(_level->_casterPos.object->getObject()->getSceneNode()))->setScale(cscale / CASTER_SCALE_DOWN);
	_level->_casterPos.object->getObject()->init(_level->_casterPos.position,
		Size((image->getContentSize().width * cscale) / (CASTER_SCALE_DOWN * scale.x),
			(image->getContentSize().height * cscale) / (CASTER_SCALE_DOWN * scale.y)),
			&casterFilter);
	_level->_casterPos.object->getObject()->setDrawScale(scale);
	_level->_casterPos.object->getObject()->positionSceneNode();
	_level->_casterPos.object->getObject()->resetSceneNode();
	_level->_casterPos.object->getObject()->setDebugNode(newDebugNode());
    addObstacle(_level->_casterPos.object->getObject(), 5);


#pragma mark : Dude
    /*Vec2 dudePos = DUDE_POS;
    image  = _assets->get<Texture2D>(DUDE_TEXTURE);
    _avatar = Shadow::create(dudePos,_scale*DUDE_SCALE, &_characterFilter, &_characterSensorFilter);
    _avatar->setDrawScale(_scale);
    
    // Add the scene graph nodes to this object
    sprite = PolygonNode::createWithTexture(image);
    sprite->setScale(cscale/DUDE_SCALE);
    _avatar->setSceneNode(sprite);
    
    draw = WireNode::create();
    draw->setColor(DEBUG_COLOR);
    draw->setOpacity(DEBUG_OPACITY);
    _avatar->setDebugNode(draw); */
	image = _assets->get<Texture2D>(DUDE_TEXTURE);
	((PolygonNode*)(_level->_playerPos.object->getSceneNode()))->initWithTexture(image);
	((PolygonNode*)(_level->_playerPos.object->getSceneNode()))->setScale(cscale / DUDE_SCALE);
	_level->_playerPos.object->init(_level->_playerPos.position, scale * DUDE_SCALE, &characterFilter, &characterSensorFilter);
	_level->_playerPos.object->setDrawScale(scale);
	_level->_playerPos.object->positionSceneNode();
	_level->_playerPos.object->resetSceneNode();
	_level->_playerPos.object->setDebugNode(newDebugNode());
    addObstacle(_level->_playerPos.object, 4); // Put this at the very front


#pragma mark : Buildings

	for (LevelInstance::StaticObjectMetadata d : _level->_staticObjects) {

		image = _assets->get<Texture2D>(d.type + OBJECT_TAG);
		((PolygonNode*)(d.object->getSceneNode()))->initWithTexture(image);
		((PolygonNode*)(d.object->getSceneNode()))->setScale(cscale);
		d.object->init(d.position, Size(image->getContentSize().width * cscale / scale.x, image->getContentSize().height * cscale / scale.y), &objectFilter);

		image = _assets->get<Texture2D>(d.type + SHADOW_TAG);
		((PolygonNode*)(d.shadow->getSceneNode()))->initWithTexture(image);		
		((PolygonNode*)(d.shadow->getSceneNode()))->setScale(cscale);
		d.shadow->init(d.position, Size(image->getContentSize().width * cscale / scale.x, image->getContentSize().height * cscale / scale.y), &shadowFilter);

		d.object->setDrawScale(scale);
		d.object->positionSceneNode();
		d.object->resetSceneNode();
		d.shadow->setDrawScale(scale);
		d.shadow->positionSceneNode();
		d.shadow->resetSceneNode();
		d.object->setDebugNode(newDebugNode());
		d.shadow->setDebugNode(newDebugNode());
		d.object->setBodyType(b2_staticBody);
		addObstacle(d.object, 2);
		addObstacle(d.shadow, 1);
		
	}

	//addBuilding("b1", "s1", Vec2(10, 10), 0.7f);
	//addBuilding("b5", "s5", Vec2(8, 15), 0.5f);

#pragma mark : Movers
	//int carnum = 0;
	/*Vec2 movPos = { 5.5f, 4.0f };
	float scale = 0.3f;
	const char * mname = "car1";
	const char * sname = "car1s";
	addMover(mname, sname, movPos, scale); */

	// Play the background music on a loop.
	/*Sound* source = _assets->get<Sound>(GAME_MUSIC);
	SoundEngine::getInstance()->playMusic(source, true, MUSIC_VOLUME); */
	image = _assets->get<Texture2D>(PEDESTRIAN_TEXTURE);
	Texture2D* shadowImage = _assets->get<Texture2D>(PEDESTRIAN_SHADOW_TEXTURE);
	for (LevelInstance::PedestrianMetadata pd : _level->_pedestrians) {
		((PolygonNode*)(pd.object->getObject()->getSceneNode()))->initWithTexture(image);
		((PolygonNode*)(pd.object->getObject()->getSceneNode()))->setScale(cscale / PEDESTRIAN_SCALE_DOWN);
		pd.object->getObject()->init(pd.position, Size((image->getContentSize().width * cscale)
			/ (scale.x * PEDESTRIAN_SCALE_DOWN), (image->getContentSize().height * cscale)
			/ (scale.y * PEDESTRIAN_SCALE_DOWN)), &objectFilter);

		((PolygonNode*)(pd.object->getShadow()->getSceneNode()))->initWithTexture(shadowImage);
		((PolygonNode*)(pd.object->getShadow()->getSceneNode()))->setScale(cscale / PEDESTRIAN_SCALE_DOWN);
		pd.object->getShadow()->init(pd.position, Size((shadowImage->getContentSize().width * cscale)
			/ (scale.x * PEDESTRIAN_SCALE_DOWN), (shadowImage->getContentSize().height * cscale)
			/ (PEDESTRIAN_SCALE_DOWN * scale.y)), &shadowFilter);

		pd.object->getObject()->setDrawScale(scale);
		pd.object->getObject()->positionSceneNode();
		pd.object->getObject()->resetSceneNode();
		pd.object->getShadow()->setDrawScale(scale);
		pd.object->getShadow()->positionSceneNode();
		pd.object->getShadow()->resetSceneNode();
		pd.object->getObject()->setDebugNode(newDebugNode());
		pd.object->getShadow()->setDebugNode(newDebugNode());
		addObstacle(pd.object->getObject(), 3);
		addObstacle(pd.object->getShadow(), 2);
	}

	image = _assets->get<Texture2D>(CAR_TEXTURE);
	shadowImage = _assets->get<Texture2D>(CAR_SHADOW_TEXTURE);
	for (LevelInstance::CarMetadata pd : _level->_cars) {
		((PolygonNode*)(pd.object->getObject()->getSceneNode()))->initWithTexture(image);
		((PolygonNode*)(pd.object->getShadow()->getSceneNode()))->initWithTexture(shadowImage);		
		((PolygonNode*)(pd.object->getObject()->getSceneNode()))->setScale(cscale / CAR_SCALE_DOWN);
		((PolygonNode*)(pd.object->getShadow()->getSceneNode()))->setScale(cscale / CAR_SCALE_DOWN);
		pd.object->getObject()->init(pd.position, Size((image->getContentSize().width * cscale)
			/ (scale.x * CAR_SCALE_DOWN), (image->getContentSize().height * cscale)
			/ (scale.y * CAR_SCALE_DOWN)), &objectFilter);
		pd.object->getShadow()->init(pd.position, Size((shadowImage->getContentSize().width * cscale)
			/ (scale.x * CAR_SCALE_DOWN), (shadowImage->getContentSize().height * cscale)
			/ (scale.y * CAR_SCALE_DOWN)), &shadowFilter);
		pd.object->getObject()->setDrawScale(scale);
		pd.object->getObject()->positionSceneNode();
		pd.object->getObject()->resetSceneNode();
		pd.object->getShadow()->setDrawScale(scale);
		pd.object->getShadow()->positionSceneNode();
		pd.object->getShadow()->resetSceneNode();
		pd.object->getObject()->setDebugNode(newDebugNode());
		pd.object->getShadow()->setDebugNode(newDebugNode());
		addObstacle(pd.object->getObject(), 3);

		addObstacle(pd.object->getShadow(), 2);
	}
}

/**
 * Add a rectangular building and shadow to the world.
 * bname and sname are the names used when the building and shadow textures
 * were loaded by the asset loader.
 * pos is the position of the upper left corner of the building and shadow.
 * The size of the building and shadow will be the size of their source
 * images scaled by scale.
 */
/*void GameController::addBuilding(const char* bname,
	                             const char* sname,
	                             const Vec2& pos,
	                             float scale) {
	auto* image = _assets->get<Texture2D>(bname);
	auto* sprite = PolygonNode::createWithTexture(image);
	sprite->setScale(scale);
	Size bs(image->getContentSize().width*scale / _scale.x,
		image->getContentSize().height*scale / _scale.y);
	Vec2 bpos(pos.x + bs.width / 2, pos.y - bs.height / 2);
	auto* box = BoxObstacle::create(bpos, bs, &LevelInstance::objectFilter);
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
	box = BoxObstacle::create(spos, ss, &LevelInstance::shadowFilter);
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

void GameController::addMover(
	const char * mname,
	const char * sname,
	const Vec2& movPos,
	float scale
	) {
	float cscale = Director::getInstance()->getContentScaleFactor();
	// Create mover shadow boxobstacle
	auto * image = _assets->get<Texture2D>(sname);
	auto * sprite = PolygonNode::createWithTexture(image);
	sprite->setScale(scale);
	Size ss(image->getContentSize().width*scale / _scale.x,
		image->getContentSize().height*scale / _scale.y);
	Vec2 spos(movPos.x, movPos.y);
	auto* box = BoxObstacle::create(spos, ss, &LevelInstance::shadowFilter);
	box->setDrawScale(_scale.x, _scale.y);
	box->setName(std::string(SHADOW_NAME));
	box->setBodyType(b2_dynamicBody);
	box->setDensity(0);
	box->setFriction(0);
	box->setRestitution(0);
	box->setSensor(true);
	box->setSceneNode(sprite);
	auto * draw = WireNode::create();
	draw->setColor(DEBUG_COLOR);
	draw->setOpacity(DEBUG_OPACITY);
	box->setDebugNode(draw);
	addObstacle(box, 1);

	// Create mover boxobstacle
	image = _assets->get<Texture2D>(mname);
	sprite = PolygonNode::createWithTexture(image);
	sprite->setScale(scale);
	auto* mbox = BoxObstacle::create(spos, ss, &LevelInstance::objectFilter);
	mbox->setDrawScale(_scale.x, _scale.y);
	mbox->setName(std::string(SHADOW_NAME));
	mbox->setBodyType(b2_dynamicBody);
	mbox->setDensity(0);
	mbox->setFriction(0);
	mbox->setRestitution(0);
	mbox->setSensor(true);
	mbox->setSceneNode(sprite);
	draw = WireNode::create();
	draw->setColor(DEBUG_COLOR);
	draw->setOpacity(DEBUG_OPACITY);
	mbox->setDebugNode(draw);
	addObstacle(mbox, 4);

	// Create mover
	OurMovingObject<Car>* _mover = OurMovingObject<Car>::create(movPos, mbox, box);

	//_mover->setHorizontalMovement(1.0f);
	//_mover->setVerticalMovement(0.0f);
	//_mover->applyForce();
	_mover->_actionQueue->push(Car::ActionType::GO,100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::TURN_LEFT, 10);
	_mover->_actionQueue->push(Car::ActionType::GO, 100);
	_mover->_actionQueue->push(Car::ActionType::STOP, 100);


	carMovers.push_back(_mover);
	_mover->retain();
} */


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
    _physics._world->addObstacle(obj);  // Implicit retain
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
	_physics.reset();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    
	_input.setZero();
	_exposure = 0;
    setFailure(false);
    setComplete(false);
	//_level.populateLevel(true) TODO uncomment this after dynamic level loading is implemented
    populate();

	// Reset the moving objects' action queues
	_level->_casterPos.object->_actionQueue = ActionQueue<Caster>::create();
	for (LevelInstance::CarMetadata &car : _level->_cars) {
		car.object->_actionQueue->release();
		car.object->_actionQueue = ActionQueue<Car>::create(*(car.actions));
		car.object->_actionQueue->retain();
	}
	for (LevelInstance::PedestrianMetadata &ped : _level->_pedestrians) {
		ped.object->_actionQueue->release();
		ped.object->_actionQueue = ActionQueue<Pedestrian>::create(*(ped.actions));
		ped.object->_actionQueue->retain();
	}

	_worldnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode())); // TODO uncomment when lazy camera implemented
	_debugnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode())); // TODO uncomment when lazy camera implemented
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
	if (_input.didReset()) { 

		reset(); 
	}
	if (_input.didExit()) {
		_rootnode->shutdown();
	}

	if (!_failed && !_complete) {
		// Process the movement
		if (_input.getHorizontal() * _input.getHorizontal() + _input.getVertical()
			* _input.getVertical() < DEADSPACE_SIZE * DEADSPACE_SIZE) {
			_level->_playerPos.object->stopMovement();
		}
		else {
			_level->_playerPos.object->changeVelocity(_input.getHorizontal(), _input.getVertical());
		}

		//_avatar->setJumping( _input.didJump());
		//_level->_playerPos.object->applyForce();

		/* for (int i = 0; i < carMovers.size(); i++) {
			OurMovingObject<Car>* car = carMovers[i];
			car->act();
			//car->setHorizontalMovement(movvec.x);
			//car->setVerticalMovement(movvec.y);
			//car->applyForce();
		} */
		for (LevelInstance::CarMetadata car : _level->_cars) 
			car.object->act();
		for (LevelInstance::PedestrianMetadata ped : _level->_pedestrians) ped.object->act();

	}
	else {
		_level->_playerPos.object->stopMovement();
		_level->_playerPos.object->setAngularVelocity(0.0f);
	}



    /* if (_avatar->isJumping()) {
        Sound* source = _assets->get<Sound>(JUMP_EFFECT);
        SoundEngine::getInstance()->playEffect(JUMP_EFFECT,source,false,EFFECT_VOLUME);
     */
	_physics.update(dt);
    
	/* if (_avatar->getVX() != 0.0f || _avatar->getVY() != 0.0f) {
		_worldnode->stopAllActionsByTag(FOLLOW_ACTION_TAG);
		_worldnode->runAction(EaseOut::create(MoveTo::create(2, _avatar->getPosition()), 4.0f))->setTag(FOLLOW_ACTION_TAG);
	} */ // TODO UNCOMMENT AND FIX FOR LAZY CAMERA

	if (!_failed) {
		if (!_complete && _physics._reachedCaster) setComplete(true);
		if (!_complete) {
			// Check for exposure or cover
			//CCLOG("%1.4f", _avatar->getCoverRatio());
			_exposure += dt * (1.0f - ((1.0f + EXPOSURE_COOLDOWN_RATIO) * _level->_playerPos.object->getCoverRatio()));
			if (_exposure < 0.0f) _exposure = 0.0f;

			if (_exposure >= EXPOSURE_LIMIT) {
				_exposure = EXPOSURE_LIMIT;
				setFailure(true);
			}
			_exposurenode->setString(cocos2d::to_string((int)(
				(_exposure / EXPOSURE_LIMIT) * 100)) + "%");
			_exposurebar->setPolygon(_exposurepoly * Vec2(_exposure / EXPOSURE_LIMIT, 1.0f));
			_exposurebar->setVisible(true);
		}
	}

    
    // Reset the game if we win or lose.
    if (_countdown > 0) {
        _countdown--;
    } else if (_countdown == 0) {
        reset();
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
	//tloader->loadAsync(BACKGROUND_IMAGE, "textures/Background.png");

	for (int building_index = 0; building_index < BUILDING_TYPES; building_index++) {
		tloader->loadAsync(buildingTextures[building_index * 4],
			"textures/buildings/" + buildingTextures[building_index * 4 + 1]);
		tloader->loadAsync(buildingTextures[building_index * 4 + 2],
			"textures/buildings/" + buildingTextures[building_index * 4 + 3]);
	}
	
	//tloader->loadAsync("car1", "textures/Car1.png");
	//tloader->loadAsync("car1s", "textures/Car1_S.png");
	//tloader->loadAsync("car2", "textures/Car2.png");
	//tloader->loadAsync("car2s", "textures/Car2_S.png");

	tloader->loadAsync(EXPOSURE_BAR, "textures/exposure_bar.png");
	tloader->loadAsync(EXPOSURE_FRAME, "textures/exposure_bar_frame.png");
	tloader->loadAsync(EARTH_TEXTURE,   "textures/earthtile.png", params);
    tloader->loadAsync(DUDE_TEXTURE,    "textures/ShadeDude.png");
	tloader->loadAsync(PEDESTRIAN_TEXTURE, "textures/pedestrian_td.png");
	tloader->loadAsync(PEDESTRIAN_SHADOW_TEXTURE, "textures/pedestrian_s_td.png");
	tloader->loadAsync(CAR_TEXTURE, "textures/Car1.png");
	tloader->loadAsync(CAR_SHADOW_TEXTURE, "textures/Car1_S.png");
    tloader->loadAsync(BULLET_TEXTURE,  "textures/bullet.png");
    tloader->loadAsync(GOAL_TEXTURE,    "textures/owner.png");
	tloader->loadAsync(PLANT1_TEXTURE, "textures/Plant1.png");
	tloader->loadAsync(PLANT1S_TEXTURE, "textures/Plant1_S.png");
	tloader->loadAsync(PLANT2_TEXTURE, "textures/Plant2.png");
	tloader->loadAsync(PLANT2S_TEXTURE, "textures/Plant2_S.png");
    _assets->loadAsync<Sound>(GAME_MUSIC,   "sounds/DD_Main.mp3");
    _assets->loadAsync<Sound>(WIN_MUSIC,    "sounds/DD_Victory.mp3");
    _assets->loadAsync<Sound>(LOSE_MUSIC,   "sounds/DD_Failure.mp3");
    _assets->loadAsync<Sound>(JUMP_EFFECT,  "sounds/jump.mp3");
    _assets->loadAsync<Sound>(PEW_EFFECT,   "sounds/pew.mp3");
    _assets->loadAsync<Sound>(POP_EFFECT,   "sounds/plop.mp3");
    _assets->loadAsync<TTFont>(MESSAGE_FONT,"fonts/RetroGame.ttf");
	_assets->loadAsync<LevelInstance>(_levelKey, _levelPath);

	

	JSONReader reader;
	reader.initWithFile(STATIC_OBJECTS);
	if (!reader.startJSON()) {
		CCASSERT(false, "Failed to load static objects");
		return;
	}
	int count = reader.startArray("types");
	for (int index = 0; index < count; index++) {
		reader.startObject();
		string name = reader.getString("name");
		string imageFormat = reader.getString("imageFormat");
		string shadowImageFormat = reader.getString("shadowImageFormat");
		if (shadowImageFormat == "") {
			shadowImageFormat = imageFormat;
		}
		tloader->loadAsync(name + OBJECT_TAG, "textures/static_objects/" + name + "." + imageFormat);
		tloader->loadAsync(name + SHADOW_TAG, "textures/static_objects/" + name + "_S." + shadowImageFormat);
		reader.endObject();
		reader.advance();
	}
	reader.endArray();
	reader.endJSON();

	reader.initWithFile(_levelPath);
	if (!reader.startJSON()) {
		CCASSERT(false, "Failed to load background image");
		return;
	}
	tloader->loadAsync(BACKGROUND_IMAGE, "textures/backgrounds/" + reader.getString("name") + "." + reader.getString("imageFormat"));
}

/**
* Clear all memory when exiting.
*/
void GameController::stop() {
	_physics.stop();
	_level->_playerPos.object->deleteEverything();
	_level->release();
}
