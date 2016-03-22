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
#define DUDE_FORCE      7.0f //20.0f
/** The factor by which to scale down the avatar image */
#define DUDE_SCALE	7.0f
/** The amount to slow the character down */
#define DUDE_DAMPING    10.0f
/** The maximum character speed */
#define DUDE_MAXSPEED   5.0f


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
    /** Ground sensor to represent our feet */
    b2Fixture*  _sensorFixture; // TODO change this with many small fixtures for exposure calculation
    /** Reference to the sensor name (since a constant cannot have a pointer) */
    std::string _sensorName;
    /** The node for debugging the sensor */
    WireNode* _sensorNode;
    
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
     * Returns how much force to apply to get the dude moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the dude moving
     */
    float getForce() const { return DUDE_FORCE; }
    
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
    
    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();

    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a DudeModel, you must call init().
     */
    Shadow() : CapsuleObstacle(), _sensorName(SENSOR_NAME) { }

    /**
     * Initializes a new dude at the origin.
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
    virtual bool init() override { return init(Vec2::ZERO, Vec2::ONE); }
    
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
    virtual bool init(const Vec2& pos, const Vec2& scale);
};

#endif /* __M_SHADOW_H__ */
