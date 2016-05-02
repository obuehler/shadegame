//
//  RDRocketObject.cpp
//  RocketDemo
//
//  This encapsulates all of the information for the rocket demo.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  Pay close attention to how this class designed.  Subclasses of Cocos2d classes
//  (which are all subclasses of the class Ref) should never have normal public
//  constructors.  Instead, you should organize their constructors into three parts,
//  just like we have done in this class.
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 12/20/15
//
#include "RDRocketModel.h"
#include <cornell/CUAssetManager.h>
#include <cornell/CUSceneManager.h>

using namespace cocos2d;

#pragma mark -
#pragma mark Animation and Physics Constants

/** The number of frames for the afterburner */
#define FIRE_FRAMES     4

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.1f
/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f

#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new rocket at the origin.
 *
 * The rocket is 1 unit by 1 unit in size. The rocket is scaled so that
 * 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @return  An autoreleased physics object
 */
RocketModel* RocketModel::create() {
    RocketModel* rocket = new (std::nothrow) RocketModel();
    if (rocket && rocket->init()) {
        rocket->autorelease();
        return rocket;
    }
    CC_SAFE_DELETE(rocket);
    return nullptr;
}

/**
 * Creates a new rocket with the given position
 *
 * The rocket is 1 unit by 1 unit in size. The rocket is scaled so that
 * 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos  Initial position in world coordinates
 *
 * @return  An autoreleased physics object
 */
RocketModel* RocketModel::create(const Vec2& pos) {
    RocketModel* rocket = new (std::nothrow) RocketModel();
    if (rocket && rocket->init(pos)) {
        rocket->autorelease();
        return rocket;
    }
    CC_SAFE_DELETE(rocket);
    return nullptr;
}

/**
 * Creates a new rocket with the given position and size.
 *
 * The rocket size is specified in world coordinates.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size   	The dimensions of the box.
 *
 * @return  An autoreleased physics object
 */
RocketModel* RocketModel::create(const Vec2& pos, const Size& size) {
    RocketModel* rocket = new (std::nothrow) RocketModel();
    if (rocket && rocket->init(pos,size)) {
        rocket->autorelease();
        return rocket;
    }
    CC_SAFE_DELETE(rocket);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers

/**
 * Initializes a new rocket with the given position and size.
 *
 * The rocket size is specified in world coordinates.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size   	The dimensions of the box.
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool RocketModel::init(const Vec2& pos, const Size& size) {
    BoxObstacle::init(pos,size);
    string name("rocket");
    setName(name);
    
    _shipTexture = "";
    
    _mainBurner = nullptr;
    _mainSound    = "";
    _mainTexture  = "";
    _mainCycle  = true;
    
    _leftBurner = nullptr;
    _leftSound   = "";
    _leftTexture = "";
    _leftCycle  = true;

    _rghtBurner = nullptr;
    _rghtSound   = "";
    _rghtTexture = "";
    _rghtCycle  = true;

    setDensity(DEFAULT_DENSITY);
    setFriction(DEFAULT_FRICTION);
    setRestitution(DEFAULT_RESTITUTION);
    setFixedRotation(true);
    
    return true;
}


/**
 * Destroys this rocket, releasing all resources.
 */
RocketModel::~RocketModel(void) {
    // We do not own any of these, so we can just set to null
    _mainBurner = nullptr;
    _leftBurner = nullptr;
    _rghtBurner = nullptr;
}


#pragma mark -
#pragma mark Physics
/**
 * Applies the force to the body of this ship
 *
 * This method should be called after the force attribute is set.
 */
void RocketModel::applyForce() {
    if (!isActive()) {
        return;
    }
    
    // Orient the force with rotation.
    Vec4 netforce(_force.x,_force.y,0.0f,1.0f);
    Mat4::createRotationZ(getAngle(),&_affine);
    _affine.transformVector(&netforce);
    
    // Apply force to the rocket BODY, not the rocket
    _body->ApplyForceToCenter(b2Vec2(netforce.x,netforce.y), true);
}


#pragma mark -
#pragma mark Animation
/**
 * Performs any necessary additions to the scene graph node.
 *
 * This method is necessary for custom physics objects that are composed
 * of multiple scene graph nodes.  In this case, it is because we
 * manage our own afterburner animations.
 */
void RocketModel::resetSceneNode() {
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    // To convert from design resolution to real, divide positions by cscale
    float cscale = Director::getInstance()->getContentScaleFactor();

    PolygonNode* pnode = dynamic_cast<PolygonNode*>(_node);
    if (pnode != nullptr) {
        SceneManager* assets =  AssetManager::getInstance()->getCurrent();

        Rect bounds;
        bounds.size = getDimension();
        bounds.size.width  *= _drawScale.x/cscale;
        bounds.size.height *= _drawScale.y/cscale;

        Texture2D* image = assets->get<Texture2D>(_shipTexture);

        pnode->setTexture(image);
        pnode->setPolygon(bounds);

        // Main burner.
        image  = assets->get<Texture2D>(_mainTexture);
        _mainBurner = AnimationNode::create(image, 1, FIRE_FRAMES, FIRE_FRAMES);
        //_mainBurner->setScale(cscale);

        pnode->addChild(_mainBurner);   // TRANSFER OWNERSHIP
        _mainBurner->setPosition(pnode->getContentSize().width/2.0f,
                                 pnode->getContentSize().height-
                                 _mainBurner->getContentSize().height/2.0f);
        // Right burner
        image = assets->get<Texture2D>(_rghtTexture);
        _rghtBurner = AnimationNode::create(image, 1, FIRE_FRAMES, FIRE_FRAMES);
        pnode->addChild(_rghtBurner);   // TRANSFER OWNERSHIP
        _rghtBurner->setPosition(pnode->getContentSize().width/2.0f,
                                 pnode->getContentSize().height/2.0f);
        
        // Left burner
        image = assets->get<Texture2D>(_leftTexture);
        _leftBurner = AnimationNode::create(image, 1, FIRE_FRAMES, FIRE_FRAMES);
        pnode->addChild(_leftBurner);   // TRANSFER OWNERSHIP
        _leftBurner->setPosition(pnode->getContentSize().width/2.0f,
                                 pnode->getContentSize().height/2.0f);
    }
}


/**
 * Returns the animation node for the given afterburner
 *
 * @param  burner   enumeration to identify the afterburner
 *
 * @return the animation node for the given afterburner
 */
const std::string& RocketModel::getBurnerStrip(Burner burner) const {
    switch (burner) {
        case Burner::MAIN:
            return _mainTexture;
        case Burner::LEFT:
            return _leftTexture;
        case Burner::RIGHT:
            return _rghtTexture;
    }
    CCASSERT(false, "Invalid burner enumeration");
}

/**
 * Sets the animation node for the given afterburner
 *
 * @param  burner   enumeration to identify the afterburner
 *
 * @param  node the animation node for the given afterburner
 */
void RocketModel::setBurnerStrip(Burner burner, std::string strip) {
    switch (burner) {
        case Burner::MAIN:
            _mainTexture = strip;
            break;
        case Burner::LEFT:
            _leftTexture = strip;
            break;
        case Burner::RIGHT:
            _rghtTexture = strip;
            break;
        default:
            CCASSERT(false, "Invalid burner enumeration");
    }
}

/**
 * Returns the key for the sound to accompany the given afterburner
 *
 * The key should either refer to a valid sound loaded in the AssetManager or
 * be empty ("").  If the key is "", then no sound will play.
 *
 * @param  burner   enumeration to identify the afterburner
 *
 * @return the key for the sound to accompany the given afterburner
 */
const std::string& RocketModel::getBurnerSound(Burner burner) const {
    switch (burner) {
        case Burner::MAIN:
            return _mainSound;
        case Burner::LEFT:
            return _leftSound;
        case Burner::RIGHT:
            return _rghtSound;
    }
    CCASSERT(false, "Invalid burner enumeration");
}

/**
 * Sets the key for the sound to accompany the given afterburner
 *
 * The key should either refer to a valid sound loaded in the AssetManager or
 * be empty ("").  If the key is "", then no sound will play.
 *
 * @param  burner   enumeration to identify the afterburner
 * @param  key      the key for the sound to accompany the main afterburner
 */
void RocketModel::setBurnerSound(Burner burner, const std::string& key) {
    switch (burner) {
        case Burner::MAIN:
            _mainSound = key;
            break;
        case Burner::LEFT:
            _leftSound = key;
            break;
        case Burner::RIGHT:
            _rghtSound = key;
            break;
        default:
            CCASSERT(false, "Invalid burner enumeration");
    }
}

/**
 * Animates the given burner.
 *
 * If the animation is not active, it will reset to the initial animation frame.
 *
 * @param  burner   The reference to the rocket burner
 * @param  on       Whether the animation is active
 */
void RocketModel::animateBurner(Burner burner, bool on) {
    AnimationNode* node;
    bool*  cycle;
    
    switch (burner) {
        case Burner::MAIN:
            node  = _mainBurner,
            cycle = &_mainCycle;
            break;
        case Burner::LEFT:
            node  = _leftBurner,
            cycle = &_leftCycle;
            break;
        case Burner::RIGHT:
            node  = _rghtBurner,
            cycle = &_rghtCycle;
            break;
    }
    
    if (on) {
        // Turn on the flames and go back and forth
        if (node->getFrame() == 0 || node->getFrame() == 1) {
            *cycle = true;
        } else if (node->getFrame() == node->getSize()-1) {
            *cycle = false;
        }
        
        // Increment
        if (*cycle) {
            node->setFrame(node->getFrame()+1);
        } else {
            node->setFrame(node->getFrame()-1);
        }
    } else {
        node->setFrame(0);
    }
}
