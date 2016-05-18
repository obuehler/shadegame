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
#include "C_Physics.h"


using namespace cocos2d;
using namespace std;

#pragma mark -
#pragma mark Level Geography

#define WALL_THICKNESS 0.08f

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

#pragma mark -
#pragma mark Asset Constants

/** Seconds before death due to exposure */
#define EXPOSURE_LIMIT 5.0f
/** Ratio of exposure cooldown speed to exposure increase speed */
#define EXPOSURE_COOLDOWN_RATIO 0.5f
/** Scale of exposure HUD */
#define EXPOSURE_SCALE 1.0f
/** Exposure bar positioning constants */
#define EXPOSURE_X_POS 0.7f
#define EXPOSURE_Y_POS 0.9f

/** The relative background images folder path */
#define BACKGROUNDS_FOLDER "textures/backgrounds/"
/** The key for the (temporary) background image */
#define BACKGROUND_IMAGE "bimage"

#define PLANT1_TEXTURE "plt1image"
#define PLANT1S_TEXTURE "plt1simage"
#define PLANT2_TEXTURE "plt2image"
#define PLANT2S_TEXTURE "plt2simage"

/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
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
/** The message for winning the game */
#define WIN_MESSAGE     "VICTORY!"
/** The color of the win message */
#define WIN_COLOR       Color3B::YELLOW
/** The message for losing the game */
#define LOSE_MESSAGE    "FAILURE!"
/** The color of the lose message */
#define LOSE_COLOR      Color3B::RED
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

/** Z-levels for nodes */
#define DEBUG_Z 12
#define INDICATOR_Z 13
#define EXPOSURE_BAR_Z 14
#define EXPOSURE_FRAME_Z 15
#define BACK_BUTTON_Z 16
#define RESUME_BUTTON_Z 17
#define CASTER_Z 11
#define PLAYER_Z 7
#define BUILDING_OBJECT_Z 9
#define BUILDING_SHADOW_Z 5
#define PEDESTRIAN_OBJECT_Z 8
#define PEDESTRIAN_SHADOW_Z 4 
#define CAR_OBJECT_Z 10
#define CAR_SHADOW_Z 6

b2Filter GameController::characterFilter = b2Filter(CHARACTER_BIT, OBJECT_BIT, 0);
b2Filter GameController::objectFilter = b2Filter(OBJECT_BIT, CHARACTER_BIT | CASTER_BIT, 1);
b2Filter GameController::casterFilter = b2Filter(CASTER_BIT, CHARACTER_SENSOR_BIT | OBJECT_BIT, 1);
b2Filter GameController::shadowFilter = b2Filter(SHADOW_BIT, CHARACTER_SENSOR_BIT, -1);
b2Filter GameController::characterSensorFilter = b2Filter(CHARACTER_SENSOR_BIT, SHADOW_BIT | CASTER_BIT, -2);

#pragma mark -
#pragma mark Initialization

GameController::GameController() :
	_rootnode(nullptr),
	_worldnode(nullptr),
	_level(nullptr),
	_gameroot(nullptr),
	_backgroundnode(nullptr),
	_debugnode(nullptr),
	_winnode(nullptr),
	_timernode(nullptr),
	_exposurebar(nullptr),
	_indicator(nullptr),
	_exposureframe(nullptr),
	_active(false),
	_debug(false),
	_paused(false),
	_complete(false),
	_failed(false),
	_countdown(-1),
	_levelKey(nullptr),
	_levelPath(nullptr)
{}

GameController* GameController::create(const char * levelkey, const char * levelpath)
{
	GameController* q = new (std::nothrow) GameController();
	if (q && q->init(levelkey, levelpath)) {
		q->autorelease();
		return q;
	}
	CC_SAFE_DELETE(q);
	return nullptr;
}

bool GameController::init(const char * levelkey, const char * levelpath) {
	_levelKey = levelkey;
	_levelPath = levelpath;
    a = new char(LATCH_NAME);
	return true;
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
 * @param root	The RootLayer creating this GameController
 *
 * @retain a reference to the root layer
 * @return  true if the controller is initialized properly, false otherwise.
 */
void GameController::initialize(RootLayer* root) {

	_level = _assets->get<LevelInstance>(_levelKey);

	if (_level == nullptr || root == nullptr) {
		return;
	}

	_level->retain();

    // Determine the center of the screen
    Size dimen  = root->getContentSize();
    Vec2 center(dimen.width/2.0f,dimen.height/2.0f);

    _input.init(Rect(0.0f, 0.0f, dimen.width, dimen.height));
    _input.start();

	_physics.init(_level->_size);

    // Create the scene graph
    _worldnode = Node::create();
    _debugnode = Node::create();
	_gameroot = Node::create();
    
    _winnode = PolygonNode::createWithTexture(_assets->get<Texture2D>(WIN_IMAGE));
    _winnode->setPosition(Point(center.x, dimen.height * 0.6f));
	_winnode->setVisible(false);
    
    
    _losenode = PolygonNode::createWithTexture(_assets->get<Texture2D>(LOSE_IMAGE));
    _losenode->setPosition(Point(center.x, dimen.height * 0.7f));
    _losenode->setScale(0.7f, 0.7f);
	_losenode->setVisible(false);

	_backgroundnode = PolygonNode::createWithTexture(
		_assets->get<Texture2D>(BACKGROUND_IMAGE + _level->_name));
	_backgroundnode->setAnchorPoint(Vec2(0, 0));
	_backgroundnode->setPosition(0, 0);
	_backgroundnode->setScale((_level->_size.width * BOX2D_SCALE) / _backgroundnode->getContentSize().width,
			(_level->_size.height * BOX2D_SCALE) / _backgroundnode->getContentSize().height);
	_backgroundnode->setVisible(true);

	// Starting exposure is 0
	_exposure = 0.0f;

	/* _timernode = Label::create();
	_timernode->setTTFConfig(_assets->get<TTFont>(MESSAGE_FONT)->getTTF());
	_timernode->setString("");
	_timernode->setPosition(60, root->getContentSize().height - 30);
	_timernode->setColor(WIN_COLOR);
	_timernode->setVisible(true); */

	_indicator = PolygonNode::createWithTexture(_assets->get<Texture2D>(INDICATOR));
	_indicator->setPosition(center.x, dimen.height * 0.9f);
	_indicator->setScale(0.08f, 0.15f);
	_indicator->setVisible(true);

	_exposurebar = PolygonNode::createWithTexture(_assets->get<Texture2D>(EXPOSURE_BAR));
	_exposurebar->setAnchorPoint(Vec2(0, 0));
	_exposurebar->setPosition(dimen.width * EXPOSURE_X_POS, dimen.height * EXPOSURE_Y_POS);
	_exposurebar->setScale(Director::getInstance()->getContentScaleFactor()*EXPOSURE_SCALE);
	_exposurebar->setVisible(true);
	
	_exposurepoly = Poly2(Rect(Vec2(0.0f, 0.0f), _exposurebar->getTexture()->getContentSize()));

	_exposureframe = Sprite::createWithTexture(_assets->get<Texture2D>(EXPOSURE_FRAME));
	_exposureframe->setPosition(
		(dimen.width * EXPOSURE_X_POS) + ((_exposurebar->getContentSize().width
			* _exposurebar->getScaleX()) / 2.0f),
		(dimen.height * EXPOSURE_Y_POS) + ((_exposurebar->getContentSize().height
			* _exposurebar->getScaleY()) / 2.0f));
	_exposureframe->setScale(Director::getInstance()->getContentScaleFactor()*EXPOSURE_SCALE);
	_exposureframe->setVisible(true);

	_resumeButton = ui::Button::create();
	_resumeButton->setTouchEnabled(true);
	_resumeButton->loadTextures("textures/menu/resume_button.png", "textures/menu/resume_button.png", "");
	_resumeButton->setPosition(Point(center.x, dimen.height * 0.35f));
	_resumeButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			togglePause();
			break;
		default:
			break;
		}
	});
	_resumeButton->setVisible(false);

	_backButton = ui::Button::create();
	_backButton->setTouchEnabled(true);
	_backButton->loadTextures("textures/menu/back_to_menu_button.png", "textures/menu/back_to_menu_button.png", "");
	_backButton->setPosition(Point(center.x, dimen.height * 0.18f));
	_backButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			deinitialize(); // sets _active to false
			break;
		default:
			break;
		}
	});
	_backButton->setVisible(false);

	_tryAgainButton = ui::Button::create();
	_tryAgainButton->setTouchEnabled(true);
	_tryAgainButton->loadTextures("textures/menu/try_again_button.png", "textures/menu/try_again_button.png", "");
	_tryAgainButton->setPosition(Point(center.x, dimen.height * 0.35f));
	_tryAgainButton->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			reset();
			break;
		default:
			break;
		}
	});
	_tryAgainButton->setVisible(false);

    // Add everything to the root and retain
	_gameroot->addChild(_backgroundnode, 1);
	_gameroot->addChild(_worldnode,2);
    _gameroot->addChild(_debugnode,DEBUG_Z);
    _gameroot->addChild(_winnode,3);
    _gameroot->addChild(_losenode,3);
	//root->addChild(_timernode, 5);
	_gameroot->addChild(_exposurebar, EXPOSURE_BAR_Z);
	_gameroot->addChild(_exposureframe, EXPOSURE_FRAME_Z);
	_gameroot->addChild(_backButton, BACK_BUTTON_Z);
	_gameroot->addChild(_tryAgainButton, RESUME_BUTTON_Z);
	_gameroot->addChild(_resumeButton, RESUME_BUTTON_Z);
	_gameroot->addChild(_indicator, INDICATOR_Z);
    _rootnode = root;
	_rootnode->addChild(_gameroot, 0);
    _rootnode->retain();

    // Now populate the physics objects
    populate();
	_worldnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode())); // TODO change when lazy camera implemented
	_debugnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode())); // TODO change when lazy camera implemented
	_backgroundnode->runAction(Follow::create(_level->_playerPos.object->getSceneNode()));
    
	setDebug(false);
	setComplete(false);
	setFailure(false);
	setPaused(false);
	_input.setZero();

	_active = true;
    
   

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
	if (_active) {
		deinitialize();
	}
	_levelKey = nullptr;
	_levelPath = nullptr;
    delete a;
    a = nullptr;
}

void GameController::deinitialize() {
	_input.setZero();
	_input.stop();
	_level->release();
	_physics.dispose();
	_level = nullptr;
	_worldnode = nullptr;
	_debugnode = nullptr;
	_winnode = nullptr;
	_timernode = nullptr;
	_indicator = nullptr;
	_exposurebar = nullptr;
	_exposureframe = nullptr;
	_rootnode->removeChild(_gameroot);
	_gameroot = nullptr;
	_rootnode->release();
	_rootnode = nullptr;
	_debug = false;
	_paused = false;
	_complete = false;
	_failed = false;
	_active = false;
}

void GameController::addWalls() {
	BoxObstacle* wallobj = BoxObstacle::create(Vec2(WALL_THICKNESS * 0.5f, _level->_size.height * 0.5f), Size(WALL_THICKNESS, _level->_size.height), &objectFilter);
	wallobj->setBodyType(b2_staticBody);
	wallobj->setDensity(BASIC_DENSITY);
	wallobj->setFriction(BASIC_FRICTION);
	wallobj->setRestitution(BASIC_RESTITUTION);
	wallobj->setDrawScale(BOX2D_SCALE, BOX2D_SCALE);
	wallobj->setSceneNode(Node::create());
	wallobj->setDebugNode(newDebugNode());
	addObstacle(wallobj, 1);
	wallobj = BoxObstacle::create(Vec2(_level->_size.width - WALL_THICKNESS * 0.5f, _level->_size.height * 0.5f), Size(WALL_THICKNESS, _level->_size.height), &objectFilter);
	wallobj->setBodyType(b2_staticBody);
	wallobj->setDensity(BASIC_DENSITY);
	wallobj->setFriction(BASIC_FRICTION);
	wallobj->setRestitution(BASIC_RESTITUTION);
	wallobj->setDrawScale(BOX2D_SCALE, BOX2D_SCALE);
	wallobj->setSceneNode(Node::create());
	wallobj->setDebugNode(newDebugNode());
	addObstacle(wallobj, 1);
	wallobj = BoxObstacle::create(Vec2(_level->_size.width * 0.5f, WALL_THICKNESS * 0.5f), Size(_level->_size.width - WALL_THICKNESS * 2, WALL_THICKNESS), &objectFilter);
	wallobj->setBodyType(b2_staticBody);
	wallobj->setDensity(BASIC_DENSITY);
	wallobj->setFriction(BASIC_FRICTION);
	wallobj->setRestitution(BASIC_RESTITUTION);
	wallobj->setDrawScale(BOX2D_SCALE, BOX2D_SCALE);
	wallobj->setSceneNode(Node::create());
	wallobj->setDebugNode(newDebugNode());
	addObstacle(wallobj, 1);
	wallobj = BoxObstacle::create(Vec2(_level->_size.width * 0.5f, _level->_size.height - WALL_THICKNESS * 0.5f), Size(_level->_size.width - WALL_THICKNESS * 2, WALL_THICKNESS), &objectFilter);
	wallobj->setBodyType(b2_staticBody);
	wallobj->setDensity(BASIC_DENSITY);
	wallobj->setFriction(BASIC_FRICTION);
	wallobj->setRestitution(BASIC_RESTITUTION);
	wallobj->setDrawScale(BOX2D_SCALE, BOX2D_SCALE);
	wallobj->setSceneNode(Node::create());
	wallobj->setDebugNode(newDebugNode());
	addObstacle(wallobj, 1);
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
	addWalls();
	Vec2 scale(BOX2D_SCALE, BOX2D_SCALE);

	// We need to know the content scale for resolution independence
	// If the device is higher resolution than 1024x576, Cocos2d will scale it
	// This was set as the design resolution in AppDelegate
	// To convert from design resolution to real, divide positions by cscale
	float cscale = Director::getInstance()->getContentScaleFactor();
	AnimationNode* animNodePtr;
	PolygonNode* polyNodePtr;
	PolygonNode* polyNodePtr1;

#pragma mark : Goal door
	animNodePtr = (AnimationNode*)(_level->_casterPos.object->getObject()->getSceneNode());
	animNodePtr->initWithFilmstrip(_assets->get<Texture2D>(GOAL_TEXTURE), CASTER_ROWS, CASTER_COLS);
	animNodePtr->setScale(cscale / CASTER_SCALE_DOWN);
	_level->_casterPos.object->getObject()->init(_level->_casterPos.position,
		Size((animNodePtr->getContentSize().width * cscale) / (CASTER_SCALE_DOWN * scale.x),
			(animNodePtr->getContentSize().height * cscale) / (CASTER_SCALE_DOWN * scale.y)),
			&casterFilter);
	_level->_casterPos.object->getObject()->setDrawScale(scale);
	_level->_casterPos.object->getObject()->positionSceneNode();
	_level->_casterPos.object->getObject()->resetSceneNode();
	_level->_casterPos.object->getObject()->setDebugNode(newDebugNode());
    addObstacle(_level->_casterPos.object->getObject(), CASTER_Z);


#pragma mark : Dude
	animNodePtr = ((AnimationNode*)(_level->_playerPos.object->getSceneNode()));
	animNodePtr->initWithFilmstrip(_assets->get<Texture2D>(DUDE_TEXTURE), PLAYER_ROWS, PLAYER_COLS);
	animNodePtr->setScale(cscale / DUDE_SCALE);
	_level->_playerPos.object->init(_level->_playerPos.position, scale * DUDE_SCALE, &characterFilter, &characterSensorFilter);
	_level->_playerPos.object->setDrawScale(scale);
	_level->_playerPos.object->positionSceneNode();
	_level->_playerPos.object->resetSceneNode();
	_level->_playerPos.object->setDebugNode(newDebugNode());
    addObstacle(_level->_playerPos.object, PLAYER_Z); // Put this at the very front


#pragma mark : Buildings
	for (LevelInstance::StaticObjectMetadata d : _level->_staticObjects) {
		polyNodePtr1 = (PolygonNode*)(d.object->getSceneNode());
		polyNodePtr1->initWithTexture(_assets->get<Texture2D>(d.type + OBJECT_TAG));
		polyNodePtr1->setScale(cscale);

		polyNodePtr = (PolygonNode*)(d.shadow->getSceneNode());
		polyNodePtr->initWithTexture(_assets->get<Texture2D>(d.type + SHADOW_TAG));
		polyNodePtr->setScale(cscale);
		
		Vec2 offset = { polyNodePtr1->getContentSize().width * cscale / (scale.x * -5.0f), polyNodePtr1->getContentSize().height * cscale / (scale.y * 4.0f) };
		d.object->init(d.position + offset, Size(polyNodePtr1->getContentSize().width * cscale / scale.x, polyNodePtr1->getContentSize().height * cscale / scale.y), &objectFilter); // Body
		d.shadow->init(d.position, Size(polyNodePtr->getContentSize().width * cscale / scale.x, polyNodePtr->getContentSize().height * cscale / scale.y), &shadowFilter); // Shadoe

		d.object->setDrawScale(scale);
		d.object->positionSceneNode();
		d.object->resetSceneNode();
		d.shadow->setDrawScale(scale);
		d.shadow->positionSceneNode();
		d.shadow->resetSceneNode();
		d.object->setDebugNode(newDebugNode());
		d.shadow->setDebugNode(newDebugNode());
		d.object->setBodyType(b2_staticBody);
		addObstacle(d.object, BUILDING_OBJECT_Z);
		addObstacle(d.shadow, BUILDING_SHADOW_Z);
		
	}

#pragma mark : Movers

	// Play the background music on a loop.
	/*Sound* source = _assets->get<Sound>(GAME_MUSIC);
	SoundEngine::getInstance()->playMusic(source, true, MUSIC_VOLUME); */
	for (LevelInstance::PedestrianMetadata pd : _level->_pedestrians) {
		animNodePtr = (AnimationNode*)(pd.object->getObject()->getSceneNode());
		animNodePtr->initWithTexture(_assets->get<Texture2D>(PEDESTRIAN_TEXTURE));
		//animNodePtr->initWithFilmstrip(_assets->get<Texture2D>(PEDESTRIAN_TEXTURE), PEDESTRIAN_ROWS, PEDESTRIAN_COLS);   TODO uncomment when we have pedestrian filmstrip
		animNodePtr->setScale(cscale / PEDESTRIAN_SCALE_DOWN);
		pd.object->getObject()->init(pd.position, Size((animNodePtr->getContentSize().width * cscale)
			/ (scale.x * PEDESTRIAN_SCALE_DOWN), (animNodePtr->getContentSize().height * cscale)
			/ (scale.y * PEDESTRIAN_SCALE_DOWN)), &objectFilter);

		animNodePtr = (AnimationNode*)(pd.object->getShadow()->getSceneNode());
		animNodePtr->initWithTexture(_assets->get<Texture2D>(PEDESTRIAN_SHADOW_TEXTURE));
		//animNodePtr->initWithFilmstrip(_assets->get<Texture2D>(PEDESTRIAN_SHADOW_TEXTURE), PEDESTRIAN_ROWS, PEDESTRIAN_COLS);   TODO uncomment when we have pedestrian filmstrip
		animNodePtr->setScale(cscale / PEDESTRIAN_SCALE_DOWN);
		pd.object->getShadow()->init(pd.position, Size((animNodePtr->getContentSize().width * cscale)
			/ (scale.x * PEDESTRIAN_SCALE_DOWN), (animNodePtr->getContentSize().height * cscale)
			/ (PEDESTRIAN_SCALE_DOWN * scale.y)), &shadowFilter);

		pd.object->getObject()->setDrawScale(scale);
		pd.object->getObject()->positionSceneNode();
		pd.object->getObject()->resetSceneNode();
		pd.object->getShadow()->setDrawScale(scale);
		pd.object->getShadow()->positionSceneNode();
		pd.object->getShadow()->resetSceneNode();
		pd.object->getObject()->setDebugNode(newDebugNode());
		pd.object->getShadow()->setDebugNode(newDebugNode());
		addObstacle(pd.object->getObject(), PEDESTRIAN_OBJECT_Z);
		addObstacle(pd.object->getShadow(), PEDESTRIAN_SHADOW_Z);
	}

	for (LevelInstance::CarMetadata pd : _level->_cars) {
		animNodePtr = (AnimationNode*)(pd.object->getObject()->getSceneNode());
		animNodePtr->initWithTexture(_assets->get<Texture2D>(CAR_TEXTURE));
		// animNodePtr->initWithFilmstrip(_assets->get<Texture2D>(CAR_TEXTURE), CAR_ROWS, CAR_COLS);
		animNodePtr->setScale(cscale / CAR_SCALE_DOWN);
		pd.object->getObject()->init(pd.position, Size((animNodePtr->getContentSize().width * cscale)
			/ (scale.x * CAR_SCALE_DOWN), (animNodePtr->getContentSize().height * cscale)
			/ (scale.y * CAR_SCALE_DOWN)), &shadowFilter);
		pd.object->getObject()->setDrawScale(scale);
		pd.object->getObject()->positionSceneNode();
		pd.object->getObject()->resetSceneNode();
		pd.object->getObject()->setDebugNode(newDebugNode());
		addObstacle(pd.object->getObject(), CAR_OBJECT_Z);

		polyNodePtr = (PolygonNode*)(pd.object->getShadow()->getSceneNode());
		polyNodePtr->initWithTexture(_assets->get<Texture2D>(CAR_SHADOW_TEXTURE));
		polyNodePtr->setScale(cscale / CAR_SCALE_DOWN);
		pd.object->getShadow()->init(pd.position, Size((polyNodePtr->getContentSize().width * cscale)
			/ (scale.x * CAR_SCALE_DOWN), (polyNodePtr->getContentSize().height * cscale)
			/ (scale.y * CAR_SCALE_DOWN)), &shadowFilter);
		pd.object->getShadow()->setDrawScale(scale);
		pd.object->getShadow()->positionSceneNode();
		pd.object->getShadow()->resetSceneNode();
		pd.object->getShadow()->setDebugNode(newDebugNode());
		addObstacle(pd.object->getShadow(), CAR_SHADOW_Z);
        
	}
    
    latchposition = WheelObstacle::create(Vec2(0.01f,0.01f), 0.0001f, &shadowFilter);
    latchposition->getBody()->SetUserData(a);
    
    
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
	/*_level->_playerPos.object->getBody()->SetTransform(b2Vec2(_level->_size.width * 2.0f, _level->_size.height * 2.0f), 0.0f);
	_physics.update(dt);
	_physics.update(dt);
	_physics.update(dt);*/
	_physics.reset();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    
	_input.setZero();
	_exposure = 0;
	setPaused(false);
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

	_tryAgainButton->setVisible(false);
	_backButton->setVisible(false);
	_resumeButton->setVisible(false);
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
	if (_input.didReset()) { 
		reset();
	}
	if (_input.didExit()) {
		_rootnode->shutdown();
	}

	if (_input.didPause() && !_failed && !_complete) {
		togglePause();
	}
    
	if (!_paused) {
		if (!_failed && !_complete) {
			if (_input.didDebug()) { setDebug(!isDebug()); }
			// Process the movement
			if (_input.getHorizontal() * _input.getHorizontal() + _input.getVertical()
				* _input.getVertical() < DEADSPACE_SIZE * DEADSPACE_SIZE) {
				_level->_playerPos.object->stopMovement();
			}
			else {
				_level->_playerPos.object->changeVelocity(_input.getHorizontal(), _input.getVertical());
			}
			for (LevelInstance::CarMetadata car : _level->_cars)
				car.object->act();
			for (LevelInstance::PedestrianMetadata ped : _level->_pedestrians)
				ped.object->act();
			
			_physics.update(dt);
			
			// Update the indicator direction
			// Subtract the found angle from 90 since getAngle returns angle with x-axis instead of y
			_indicator->setRotation(90.0f - CC_RADIANS_TO_DEGREES(
				(_level->_casterPos.object->getObject()->getPosition() - 
					_level->_playerPos.object->getPosition()).getAngle()));
            
            
            
          
		}


		/* if (_avatar->isJumping()) {
			Sound* source = _assets->get<Sound>(JUMP_EFFECT);
			SoundEngine::getInstance()->playEffect(JUMP_EFFECT,source,false,EFFECT_VOLUME);
		 */

		/* if (_avatar->getVX() != 0.0f || _avatar->getVY() != 0.0f) {
			_worldnode->stopAllActionsByTag(FOLLOW_ACTION_TAG);
			_worldnode->runAction(EaseOut::create(MoveTo::create(2, _avatar->getPosition()), 4.0f))->setTag(FOLLOW_ACTION_TAG);
		} */ // TODO UNCOMMENT AND FIX FOR LAZY CAMERA

		if (!_failed) {
			if (!_complete && _physics._reachedCaster) setComplete(true);
			if (!_complete) {
				// Check for exposure or cover
				_exposure += dt * (1.0f - ((1.0f + EXPOSURE_COOLDOWN_RATIO) * _level->_playerPos.object->getCoverRatio()));
				if (_exposure < 0.0f) _exposure = 0.0f;
				if (_exposure >= EXPOSURE_LIMIT) {
					_exposure = EXPOSURE_LIMIT;
					setFailure(true);
				}
				_exposurebar->setPolygon(_exposurepoly * Vec2(1.0f - (_exposure / EXPOSURE_LIMIT), 1.0f));
				_exposurebar->setVisible(true);
			}
		}

		_level->_playerPos.object->updateAnimation();

		// Reset the game if we win or lose.
		if (_countdown > 0) {
			_countdown--;
		}
		else if (_countdown == 0) {
			if (_failed || _complete) {
				_backButton->setVisible(true);
				if (_failed) {
					_tryAgainButton->setVisible(true);
				}
			}
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
	string levelName;
	string backgroundPath = BACKGROUNDS_FOLDER;
	_assets = AssetManager::getInstance()->getCurrent();
	{
		JSONReader reader;
		reader.initWithFile(_levelPath);
		if (!reader.startJSON()) {
			CCASSERT(false, "Failed to load background image");
			return;
		}
		levelName = reader.getString("name");
		backgroundPath += levelName;
		backgroundPath += '.';
		backgroundPath += reader.getString("imageFormat");
		reader.endJSON();
	}
	((TextureLoader*)_assets->access<Texture2D>())->loadAsync(
		BACKGROUND_IMAGE + levelName, backgroundPath);
	_assets->loadAsync<LevelInstance>(_levelKey, _levelPath);
}

void GameController::setPaused(bool value) {
	_paused = value;
	_resumeButton->setVisible(_paused);
	_backButton->setVisible(_paused);
}
