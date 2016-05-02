//
//  RDRocketObject.h
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
//  Note that this object manages its own texture, but DOES NOT manage its own scene
//  graph node.  This is a very common way that we will approach complex objects.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 12/20/15
//
#ifndef __RD_ROCKET_OBJECT_H__
#define __RD_ROCKET_OBJECT_H__

#include <cornell/CUBoxObstacle.h>
#include <cornell/CUAnimationNode.h>


using namespace cocos2d;

/** The thrust factor to convert player input into thrust */
#define DEFAULT_THRUST 30.0f

#pragma mark -
#pragma mark Rocket Model

/**
 * Player avatar for the rocket lander game.
 *
 * Note that this class (unlike the other physics objects) constructs its own
 * scene graph node.  Either way is acceptable.  However, in both cases, the
 * physics object must retain ownership of the scene graph node.
 */
class RocketModel : public BoxObstacle {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(RocketModel);

protected:
    /** The force to apply to this rocket */
    Vec2 _force;

    /** The texture filmstrip for the rocket ship */
    std::string _shipTexture;
    
    /** The texture filmstrip for the left animation node */
    std::string _mainTexture;
    /** The animation node for the main afterburner */
    AnimationNode* _mainBurner;
    /** The associated sound for the main afterburner */
    std::string _mainSound;
    /** The animation phase for the main afterburner */
    bool _mainCycle;

    /** The texture filmstrip for the left animation node */
    std::string _leftTexture;
    /** The animation node for the left side burner */
    AnimationNode* _leftBurner;
    /** The associated sound for the left side burner */
    std::string _leftSound;
    /** The animation phase for the left side burner */
    bool _leftCycle;

    /** The texture filmstrip for the left animation node */
    std::string _rghtTexture;
    /** The animation node for the right side burner */
    AnimationNode* _rghtBurner;
    /** The associated sound for the right side burner */
    bool _rghtCycle;
    /** The animation phase for the right side burner */
    std::string _rghtSound;
    
    /** Cache object for transforming the force according the object angle */
    Mat4 _affine;
    
    /**
     * Performs any necessary additions to the scene graph node.
     *
     * This method is necessary for custom physics objects that are composed
     * of multiple scene graph nodes.  In this case, it is because we
     * manage our own afterburner animations.
     */
    virtual void resetSceneNode() override;
    
public:
    
    /**
     * Enumeration to identify the rocket afterburner
     */
    enum Burner {
        /** The main afterburner */
        MAIN,
        /** The left side thruster */
        LEFT,
        /** The right side thruster */
        RIGHT
    };
    
    
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
    static RocketModel* create();
    
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
    static RocketModel* create(const Vec2& pos);
    
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
    static RocketModel* create(const Vec2& pos, const Size& size);

    
#pragma mark -
#pragma mark Accessors
    /**
     * Returns the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @return the force applied to this rocket.
     */
    const Vec2& getForce() const { return _force; }

    /**
     * Sets the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @param value  the force applied to this rocket.
     */
    void setForce(const Vec2& value) { _force.set(value); }

    /**
     * Returns the x-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @return the x-component of the force applied to this rocket.
     */
    float getFX() const { return _force.x; }
    
    /**
     * Sets the x-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @param value the x-component of the force applied to this rocket.
     */
    void setFX(float value) { _force.x = value; }
    
    /**
     * Returns the y-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @return the y-component of the force applied to this rocket.
     */
    float getFY() const { return _force.y; }
    
    /**
     * Sets the x-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @param value the x-component of the force applied to this rocket.
     */
    void setFY(float value) { _force.y = value; }
    
    /**
     * Returns the amount of thrust that this rocket has.
     *
     * Multiply this value times the horizontal and vertical values in the
     * input controller to get the force.
     *
     * @return the amount of thrust that this rocket has.
     */
    float getThrust() const { return DEFAULT_THRUST; }

    
#pragma mark -
#pragma mark Animation
    /**
     * Returns the texture (key) for this rocket
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @return the texture (key) for this rocket
     */
    const std::string& getShipTexture() const { return _shipTexture; }

    /**
     * Returns the texture (key) for this rocket
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @param  strip    the texture (key) for this rocket
     */
    void setShipTexture(std::string strip) { _shipTexture = strip; }

    /**
     * Returns the filmstrip for the given afterburner
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @param  burner   enumeration to identify the afterburner
     *
     * @return the filmstrip for for the given afterburner
     */
    const std::string& getBurnerStrip(Burner burner) const;
    
    /**
     * Sets the animation node for the given afterburner
     *
     * @param  burner   enumeration to identify the afterburner
     *
     * @param  node the animation node for the given afterburner
     */
    void setBurnerStrip(Burner burner, std::string strip);
    
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
    const std::string& getBurnerSound(Burner burner) const;
    
    /**
     * Sets the key for the sound to accompany the given afterburner
     *
     * The key should either refer to a valid sound loaded in the AssetManager or
     * be empty ("").  If the key is "", then no sound will play.
     *
     * @param  burner   enumeration to identify the afterburner
     * @param  key      the key for the sound to accompany the main afterburner
     */
    void setBurnerSound(Burner burner, const std::string& key);

    /**
     * Animates the given burner.
     *
     * If the animation is not active, it will reset to the initial animation frame.
     *
     * @param  burner   The reference to the rocket burner
     * @param  on       Whether the animation is active
     */
    void animateBurner(Burner burner, bool on);
    
    
#pragma mark -
#pragma mark Physics
    /**
     * Applies the force to the body of this ship
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();
    
    
#pragma mark -
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /*
     * Creates a new rocket at the origin.
     */
    RocketModel(void) : BoxObstacle() { }
    
    /**
     * Destroys this rocket, releasing all resources.
     */
    virtual ~RocketModel(void);
    
    /**
     * Initializes a new rocket at the origin.
     *
     * The rocket is 1 unit by 1 unit in size. The rocket is scaled so that 
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO,Size::ZERO); }
    
    /**
     * Initializes a new rocket with the given position
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
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos) override { return init(pos,Size(1,1)); }
    
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
    virtual bool init(const Vec2& pos, const Size& size) override;

};

#endif /* defined(__RD_ROCKET_OBJECT_H__) */
