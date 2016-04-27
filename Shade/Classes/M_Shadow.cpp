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
//  This file is based on the CS 4152 PlatformDemo code by Walker White, 2016,
//  in turn based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007

#include "M_Shadow.h"
#include <cornell/CUPolygonNode.h>
#include <cornell/CUAssetManager.h>
#include <cornell/CUSceneManager.h>
#include <math.h>

#define SIGNUM(x)  ((x > 0) - (x < 0))

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for shooting */
#define SHOOT_COOLDOWN  20
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.95f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.7f
/** Radius of each of the shadow sensor fixtures */
#define SENSOR_RADIUS 0.0001f
/** Distance between adjacent sensors' centers, in Box2D coordinates */
#define SENSOR_INTERVAL 0.3f
/** The density of the character */
#define DUDE_DENSITY    1.0f
/** The impulse for the character jump */
#define DUDE_JUMP       5.5f
/** Debug color for the sensor */
#define DEBUG_COLOR     Color3B::RED


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new dude at the origin.
 *
 * The dude is scaled so that 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @return  An autoreleased physics object
 */
Shadow* Shadow::create() {
    Shadow* dude = new (std::nothrow) Shadow();
    if (dude && dude->init()) {
        dude->autorelease();
        return dude;
    }
    CC_SAFE_DELETE(dude);
    return nullptr;
}

/**
 * Creates a new dude at the given position.
 *
 * The dude is scaled so that 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 *
 * @return  An autoreleased physics object
 */
Shadow* Shadow::create(const Vec2& pos) {
    Shadow* dude = new (std::nothrow) Shadow();
    if (dude && dude->init(pos)) {
        dude->autorelease();
        return dude;
    }
    CC_SAFE_DELETE(dude);
    return nullptr;
}

/**
 * Creates a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  scale    The drawing scale
 *
 * @return  An autoreleased physics object
 */
Shadow* Shadow::create(const Vec2& pos, const Vec2& scale) {
    Shadow* dude = new (std::nothrow) Shadow();
    if (dude && dude->init(pos, scale)) {
        dude->autorelease();
        return dude;
    }
    CC_SAFE_DELETE(dude);
    return nullptr;
}

/**
* Creates a new dude at the given position with the given collision filters.
*
* The dude is sized according to the given drawing scale.
*
* The scene graph is completely decoupled from the physics system.
* The node does not have to be the same size as the physics body. We
* only guarantee that the scene graph node is positioned correctly
* according to the drawing scale.
*
* @param  pos      Initial position in world coordinates
* @param  scale    The drawing scale
*
* @return  An autoreleased physics object
*/
Shadow* Shadow::create(const Vec2& pos, const Vec2& scale, const b2Filter* const characterFilter, const b2Filter* const sensorFilter) {
	Shadow* dude = new (std::nothrow) Shadow();
	if (dude && dude->init(pos, scale, characterFilter, sensorFilter)) {
		dude->autorelease();
		return dude;
	}
	CC_SAFE_DELETE(dude);
	return nullptr;
}


#pragma mark -
#pragma mark Initializers

/**
 * Initializes a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  scale    The drawing scale
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Shadow::init(const Vec2& pos, const Vec2& scale, const b2Filter* const characterFilter, const b2Filter* const sensorFilter) {
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(DUDE_TEXTURE);
    
    // Multiply by the scaling factor so we can be resolution independent
    float cscale = Director::getInstance()->getContentScaleFactor();
    Size nsize = image->getContentSize()*cscale;
    
    
    nsize.width  *= DUDE_HSHRINK/scale.x;
    nsize.height *= DUDE_VSHRINK/scale.y;

	_sensorFilter = sensorFilter;

    if (CapsuleObstacle::init(pos, nsize, characterFilter)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);      // HE WILL STICK TO WALLS IF YOU FORGET
        setFixedRotation(true); // OTHERWISE, HE IS A WEEBLE WOBBLE
        
        // Gameplay attributes
        _faceRight  = true;
        
        return true;
    }

	_sensorFilter = nullptr;
    return false;
}


#pragma mark -
#pragma mark Attribute Properties

/**
 * Sets left/right movement of this character.
 *
 * This is the result of input times dude force.
 *
 * @param value left/right movement of this character.
 */
void Shadow::setHorizontalMovement(float value) {
    _horizontalMovement = value;
    bool face = _horizontalMovement > 0;
    if (_horizontalMovement == 0 || _faceRight == face) {
        return;
    }
    
    // Change facing
    TexturedNode* image = dynamic_cast<TexturedNode*>(_node);
    if (image != nullptr) {
        image->flipHorizontal(!image->isFlipHorizontal());
    }
    _faceRight = (_horizontalMovement > 0);
}

void Shadow::setVerticalMovement(float value) {
	_verticalMovement = value;
}

float Shadow::getCoverRatio() const {
	if (_sensorCount > 0) {
		int coveredSensors = 0;
		for (int sensorIndex = 0; sensorIndex < _sensorCount; sensorIndex++) {
			if (!(((usp*)(_sensorFixtures[sensorIndex]->GetUserData()))->empty()))
				coveredSensors++;
		}
		return ((float)coveredSensors) / ((float)_sensorCount);
	}
	return 0.0f;
}


#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void Shadow::createFixtures() {
    if (_body == nullptr) {
        return;
    }
	// TODO edit the sensor fixtures to be correct.
	// Perhaps have so many small sensor fixtures on the character, and keep
	// track of how many collide with other shadows to then alter the rate of
	// change of exposure.

    CapsuleObstacle::createFixtures();
    
	b2FixtureDef sensorDef;
    sensorDef.density = DUDE_DENSITY;
    sensorDef.isSensor = true;

	if (_sensorFilter != nullptr) {
		sensorDef.filter = *_sensorFilter;
	}

	// The number of sensors across the character's body
	int sensorsAcross = (int)((getWidth() / SENSOR_INTERVAL) - 0.5f);
	// The number of sensors vertically across the character's body
	int sensorsDown = (int)((getHeight() / SENSOR_INTERVAL) - 0.5f);

	_sensorCount = sensorsAcross * sensorsDown;
	_sensorFixtures = new b2Fixture*[_sensorCount];
	_unorderedSets = new usp*[_sensorCount];

	for (int acrossIndex = 0; acrossIndex < sensorsAcross; acrossIndex++) {
		for (int downIndex = 0; downIndex < sensorsDown; downIndex++) {
			b2CircleShape sensorShape;
			sensorShape.m_radius = SENSOR_RADIUS;
			sensorShape.m_p.Set(SENSOR_INTERVAL * (acrossIndex + 0.5f) - getWidth() * 0.5f,
				SENSOR_INTERVAL * (downIndex + 0.5f) - getHeight() * 0.5f);
			sensorDef.shape = &sensorShape;
			_sensorFixtures[acrossIndex * sensorsDown + downIndex]
				= _body->CreateFixture(&sensorDef);

			// The user data will hold the set of overlapping shadows
			register int overallindex = acrossIndex * sensorsDown + downIndex;
			_unorderedSets[overallindex] = new usp();
			_sensorFixtures[overallindex]
				->SetUserData(_unorderedSets[overallindex]);
		}
	}
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void Shadow::releaseFixtures() {
	if (_body == nullptr) {
		return;
	}

	CapsuleObstacle::releaseFixtures();
	for (int index = 0; index < _sensorCount; index++) {
		if (_sensorFixtures[index] != nullptr) {
			_sensorFixtures[index]->SetUserData(nullptr);
			_body->DestroyFixture(_sensorFixtures[index]);
			_sensorFixtures[index] = nullptr;
			delete _unorderedSets[index];
			_unorderedSets[index] = nullptr;
		}
	}
}

/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void Shadow::applyForce() {
    if (!isActive()) {
        return;
    }
    

    // Don't want to be moving. Damp out player motion
    /*if (getHorizontalMovement() == 0.0f) {
        b2Vec2 force(-getDamping()*getVX(),0);
        _body->ApplyForce(force,_body->GetPosition(),true);
    }

	if (getVerticalMovement() == 0.0f) {
		b2Vec2 force(0, -getDamping()*getVY());
		_body->ApplyForce(force, _body->GetPosition(), true);
	}
    
	b2Vec2 force(getHorizontalMovement(),getVerticalMovement());
    _body->ApplyForce(force,_body->GetPosition(),true);

	// Velocity too high, clamp it
	if ((pow(pow(getVX(), 2) + pow(getVY(), 2), 0.5)) >= getMaxSpeed()) {
		//setVX(SIGNUM(getVX())*getMaxSpeed());
		setVX((getVX() / pow(pow(getVX(), 2) + pow(getVY(), 2), 0.5)) * getMaxSpeed());
		setVY((getVY() / pow(pow(getVX(), 2) + pow(getVY(), 2), 0.5)) * getMaxSpeed());
	} */
	_body->SetLinearVelocity(b2Vec2(getHorizontalMovement(), getVerticalMovement()));
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void Shadow::update(float dt) {
	// Add stuff here if needed
    CapsuleObstacle::update(dt);
}

/** Delete everything allocated with new. */
void Shadow::deleteEverything() {
	for (int index = 0; index < _sensorCount; index++) {
		_sensorFixtures[index] = nullptr;
		delete _unorderedSets[index];
		_unorderedSets[index] = nullptr;
	}
	delete[] _sensorFixtures;
	delete[] _unorderedSets;
}


#pragma mark -
#pragma mark Scene Graph Methods

/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void Shadow::resetDebugNode() {
    CapsuleObstacle::resetDebugNode();
    /*float w = _dimension.width*_drawScale.x;
    float h = SENSOR_HEIGHT*_drawScale.y;
    Poly2 poly(Rect(-w/2.0f,-h/2.0f,w,h));
    poly.traverse(Poly2::Traversal::INTERIOR);
    
    _sensorNode = WireNode::createWithPoly(poly);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width/2.0f, 0.0f));
    _debug->addChild(_sensorNode); */ // TODO uncomment if we want to display wireframes for sensor fixtures
}
