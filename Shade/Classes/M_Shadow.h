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

#ifndef __M_SHADOW_H__
#define __M_SHADOW_H__
#include <cornell/CUBoxObstacle.h>
#include <cornell/CUCapsuleObstacle.h>
#include <cornell/CUWireNode.h>
#include <unordered_set>

using namespace cocos2d;
 
#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define DUDE_TEXTURE    "dude"
/** Identifier to allow us to track the sensor in ContactListener */
#define SENSOR_NAME     "dudesensor"


#pragma mark -
#pragma mark Physics Constants
/** The factor to multiply by the input */
#define DUDE_FORCE      5.0f //20.0f
/** The factor by which to scale down the avatar image */
#define DUDE_SCALE	7.0f
/** The amount to slow the character down */
#define DUDE_DAMPING    10.0f
/** The maximum character speed */
#define DUDE_MAXSPEED   5.0f

/** The player animation filmstrip attributes */
#define PLAYER_ROWS 1
#define PLAYER_COLS 10

typedef std::unordered_set<b2Fixture*> usp;

#pragma mark -
#pragma mark Dude Model
/**
 * Player avatar for the plaform game.
 *
 * Note that this class uses a capsule shape, not a rectangular shape.  In our
 * experience, using a rectangular shape for a character will regularly snag
 * on a platform.  The round shapes on the end caps lead to smoother movement.
 */
class Shadow : public CapsuleObstacle {  // TODO change this to PolygonObstacle instead
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CC_DISALLOW_COPY_AND_ASSIGN(Shadow);

protected:
    /** The current horizontal movement of the character */
    float _horizontalMovement;
	/** The current horizontal movement of the character */
	float _verticalMovement;
	/** Whether the character image is facing right */
	bool _faceRight;
	/** The number of sensor fixtures the character has */
	int _sensorCount;
	/** Array holding pointers to the character's sensor fixtures */
	b2Fixture** _sensorFixtures;
	/** Array holding pointers to the sets containing the shadow fixtures
	 * overlapping with the sensor fixture at the respective index of _sensorFixtures */
	usp** _unorderedSets;
    /** Reference to the sensor name (since a constant cannot have a pointer) */
    std::string _sensorName;
    /** The node for debugging the sensor */
    WireNode* _sensorNode;
	/** Pointer to the collision filter for the sensor fixtures */
	const b2Filter* _sensorFilter;
    
    /**
     * Redraws the outline of the physics fixtures to the debug node
     *
     * The debug node is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     */
    virtual void resetDebugNode() override;

public:
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
    static Shadow* create();
    
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
    static Shadow* create(const Vec2& pos);
    
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
    static Shadow* create(const Vec2& pos, const Vec2& scale);

	/**
	* Creates a new dude at the given position and collision filters.
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
	static Shadow* create(const Vec2& pos, const Vec2& scale, const b2Filter* const characterFilter, const b2Filter* const sensorFilter);

    
#pragma mark Attribute Properties
    /**
     * Returns left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @return left/right movement of this character.
     */
    float getHorizontalMovement() const { return _horizontalMovement; }

	/**
	* Returns left/right movement of this character.
	*
	* This is the result of input times dude force.
	*
	* @return left/right movement of this character.
	*/
	float getVerticalMovement() const { return _verticalMovement; }
    
    /**
     * Sets left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @param value left/right movement of this character.
     */
    void setHorizontalMovement(float value);

	/**
	* Sets up/down movement of this character.
	*
	* This is the result of input times dude force.
	*
	* @param value up/down movement of this character.
	*/
	void setVerticalMovement(float value);
    
    /**
     * Returns the speed the character moves at
     *
     * Multiply this by the input to get the movement value.
     *
     * @return the speed the character moves at
     */
    float getSpeed() const { return DUDE_FORCE; }
    
    /**
     * Returns ow hard the brakes are applied to get a dude to stop moving
     *
     * @return ow hard the brakes are applied to get a dude to stop moving
     */
    float getDamping() const { return DUDE_DAMPING; }
    
    /**
     * Returns the upper limit on dude left-right movement.
     *
     * This does NOT apply to vertical movement.
     *
     * @return the upper limit on dude left-right movement.
     */
    float getMaxSpeed() const { return DUDE_MAXSPEED; }
    
    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string* getSensorName() { return &_sensorName; }

	/**
	 * Returns true the character is facing right, false otherwise.
	 *
	 * @return whether the character is facing right
	 */
	bool isFacingRight() const { return _faceRight; }

	/** Returns the portion of the character covered by shadows. */
	float getCoverRatio() const;
    
    
#pragma mark Physics Methods
	/**
	* Create new fixtures for this body, defining the shape
	*
	* This is the primary method to override for custom physics objects
	*/
    void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;
    
    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt) override;

	/** Stops the character by setting the body velocity to zero. */
	void stopMovement();

	/** Changes the velocity of the character by normalizing the input x and y
	 * values and multiplying it with the desired speed. */
	void changeVelocity(float x, float y);

	/** Delete everything allocated with new. */
	void deleteEverything();

	/** Updates the character's animation frame according to its movement. */
	void updateAnimation();

    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a DudeModel, you must call init().
     */
    Shadow() : CapsuleObstacle(), _sensorName(SENSOR_NAME),
		_sensorCount(0), _sensorFixtures(nullptr), _unorderedSets(nullptr) { }

    /**
     * Initializes a new dude with no attributes.
     *
     * The dude is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
	// COMMENTED OUT FOR DYNAMIC LEVEL LOADING
    //virtual bool init() override { return init(Vec2::ZERO, Vec2::ONE); }
	virtual bool init() override { return Obstacle::init(); }
    
    /**
     * Initializes a new dude at the given position.
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
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos) override { return init(pos, Vec2::ONE); }
    
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
	virtual bool init(const Vec2& pos, const Vec2& scale) { return init(pos, scale, nullptr, nullptr); }

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
	virtual bool init(const Vec2& pos, const Vec2& scale, const b2Filter* const characterFilter, const b2Filter* const sensorFilter);
};

#endif /* __M_SHADOW_H__ */
