//
//  PFSpinner.h
//  PlatformerDemo
//
//  This class provides a spinning rectangle on a fixed pin.  We did not really need
//  a separate class for this, as it has no update.  However, ComplexObstacles always
//  make joint management easier.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/16
//
#ifndef __PF_SPINNER_H__
#define __PF_SPINNER_H__
#include <cornell/CUComplexObstacle.h>

using namespace cocos2d;

#pragma mark -
#pragma mark Spinner Constants
/** The key for the spinner texture */
#define SPINNER_TEXTURE "spinner"
/** The debug name for the entire obstacle */
#define SPINNER_NAME    "spinner"
/** The debug name for the spinning barrier */
#define BARRIER_NAME    "barrier"
/** The debug name for the central pin */
#define SPIN_PIN_NAME   "pin"


#pragma mark -
#pragma mark Spinner Obstacle
/**
 * Spinning platform for the plaform game.
 *
 * We did not really need a separate class for this, as it has no update.  However, 
 * ComplexObstacles always make joint management easier.
 */
class Spinner : public ComplexObstacle {
protected:
#pragma mark Scene Graph Internals
    /**
     * Performs any necessary additions to the scene graph node.
     *
     * This method is necessary for custom physics objects that are composed
     * of multiple scene graph nodes.
     */
    virtual void resetSceneNode() override;
    
    /**
     * Redraws the outline of the physics fixtures to the debug node
     *
     * The debug node is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     *
     * Unfortunately, the current implementation is very inefficient.  Cocos2d
     * does not batch drawnode commands like it does Sprites or PolygonSprites.
     * Therefore, every distinct DrawNode is a distinct OpenGL call.  This can
     * really hurt framerate when debugging mode is on.  Ideally, we would refactor
     * this so that we only draw to a single, master draw node.  However, this
     * means that we would have to handle our own vertex transformations, instead
     * of relying on the transforms in the scene graph.
     */
    virtual void resetDebugNode() override;
    
    
public:
#pragma mark Static Constructors
    /**
     * Creates a new spinner at the origin.
     *
     * The spinner is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  An autoreleased physics object
     */
    static Spinner* create();
    
    /**
     * Creates a new spinner at the given position.
     *
     * The spinner is scaled so that 1 pixel = 1 Box2d unit
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
    static Spinner* create(const Vec2& pos);
    
    /**
     * Creates a new spinner at the given position.
     *
     * The spinner is sized according to the given drawing scale.
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
    static Spinner* create(const Vec2& pos, const Vec2& scale);
    

#pragma mark Physics Methods
    /**
     * Creates the joints for this object.
     *
     * This method is executed as part of activePhysics. This is the primary method to
     * override for custom physics objects.
     *
     * @param world Box2D world to store joints
     *
     * @return true if object allocation succeeded
     */
    bool createJoints(b2World& world) override;


CC_CONSTRUCTOR_ACCESS:
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate spinner object.
     *
     * This constructor does not initialize any of the spinner values beyond
     * the defaults.  To use a Spinner, you must call init().
     */
    Spinner() : ComplexObstacle() { }

    /**
     * Initializes a new spinner at the origin.
     *
     * The spinner is scaled so that 1 pixel = 1 Box2d unit
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
     * Initializes a new spinner at the given position.
     *
     * The spinner is scaled so that 1 pixel = 1 Box2d unit
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
     * Initializes a new spinner at the given position.
     *
     * The spinner is sized according to the given drawing scale.
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

#endif /* __PF_SPINNER_H__ */
