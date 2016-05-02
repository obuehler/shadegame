//
//  CUWorldController.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a Cocos2d wrapper to Box2d that is superior to the one built into
//  Cocos2d.  The existing implementation is coupled with the scene graph, which is bad, bad
//  bad. This is a way to handle Box2d with proper reference counting and garbage collection
//  that does not require the scene graph.
//
//  Author: Walker White
//  Version: 12/5/15
//
#ifndef __CU_WORLD_CONTROLLER_H__
#define __CU_WORLD_CONTROLLER_H__

#include <cocos2d.h>
#include <vector>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
class b2World;

NS_CC_BEGIN

// We need a lot of forward references to the classes used by this controller
class Obstacle;

/** Default amount of time for a physics engine step. */
#define DEFAULT_WORLD_STEP  1/60.0f
/** Default number of velocity iterations for the constrain solvers */
#define DEFAULT_WORLD_VELOC 6
/** Default number of position iterations for the constrain solvers */
#define DEFAULT_WORLD_POSIT 2


#pragma mark -
#pragma mark World Controller
/**
 * A Cocos2d wrapper for a Box2d world.
 *
 * This class is superior to the Box2d support in Cocos2d. The existing implementation 
 * is coupled with the scene graph, which is bad, bad bad. This is a way to handle
 * Box2d with proper reference counting and garbage collection that does not require 
 * the scene graph.
 * 
 * This class provides a modern callback approach supporting closures assigned to 
 * attributes.  This allows you to modify the callback functions while the program
 * is running.
 */
class CC_DLL WorldController : public b2ContactListener, b2DestructionListener, b2ContactFilter, public Ref {
protected:
    /** Reference to the Box2D world */
    b2World* _world;
    /** Whether to lock the physic timestep to a constant amount */
    bool _lockstep;
    /** The amount of time for a single engine step */
    float _stepssize;
    /** The number of velocity iterations for the constrain solvers */
    int _itvelocity;
    /** The number of position iterations for the constrain solvers */
    int _itposition;
    /** The current gravitational value of the world */
    Vec2 _gravity;
    
    /** The list of objects in this world */
    std::vector<Obstacle*> _objects;
    
    /** The boundary of the world */
    Rect _bounds;
    
    /** Whether or not to activate the collision listener */
    bool _collide;
    /** Whether or not to activate the filter listener */
    bool _filters;
    /** Whether or not to activate the destruction listener */
    bool _destroy;
    
    
public:
#pragma mark Static Constructors
    /**
     * Creates a new physics world
     *
     * The specified bounds are in terms of the Box2d world, not the screen.
     * A few attached to this Box2d world should have ways to convert between
     * the coordinate systems.
     *
     * This constructor will use the default gravitational value.
     *
     * @param  bounds   The game bounds in Box2d coordinates
     * @param  gravity  The gravitational force on this Box2d world
     *
     * @return  An autoreleased physics controller
     */
    static WorldController* create(const Rect& bounds);
    
    /**
     * Creates a new physics world
     *
     * The specified bounds are in terms of the Box2d world, not the screen.
     * A few attached to this Box2d world should have ways to convert between
     * the coordinate systems.
     *
     * @param  bounds   The game bounds in Box2d coordinates
     * @param  gravity  The gravitational force on this Box2d world
     *
     * @return  An autoreleased physics controller
     */
    static WorldController* create(const Rect& bounds, const Vec2& gravity);

    
#pragma mark -
#pragma mark Physics Handling
    
    /**
     * Returns a reference to the Box2d world.
     *
     * This accessor is for any world methods that are not encapsulated by this
     * constroller.  We have largely limited the controller to functionality that
     * requires b2WorldCallbacks, as those classes are antiquated in the face of
     * modern closures.
     *
     * @return a reference to the Box2d world.
     */
    b2World* getWorld() { return _world; }
    
    /**
     * Returns true if the physics is locked to a constant timestep.
     *
     * If this is false, the physics timestep will vary with the graphics framerate.
     *
     * @return true if the physics is locked to a constant timestep.
     */
    bool isLockStep() const { return _lockstep; }
    
    /**
     * Sets whether the physics is locked to a constant timestep.
     *
     * If this is false, the physics timestep will vary with the graphics framerate.
     * Any change will take effect at the time of the next call to update.
     *
     * @param  flag whether the physics is locked to a constant timestep.
     */
    void setLockStep(bool flag) { _lockstep = flag; }
    
    /** 
     * Returns the amount of time for a single engine step.
     *
     * This attribute is only relevant if isLockStep() is true.
     *
     * @return the amount of time for a single engine step.
     */
    float getStepsize() const { return _stepssize; }
    
    /**
     * Sets the amount of time for a single engine step.
     *
     * This attribute is only relevant if isLockStep() is true. Any change will take 
     * effect at the time of the next call to update.
     *
     * @param  step the amount of time for a single engine step.
     */
    void setStepsize(float step) { _stepssize = step; }

    /** 
     * Returns number of velocity iterations for the constrain solvers 
     *
     * @return number of velocity iterations for the constrain solvers
     */
    int getVelocityIterations() const { return _itvelocity; }

    /**
     * Sets number of velocity iterations for the constrain solvers
     *
     * Any change will take effect at the time of the next call to update.
     *
     * @param  velocity number of velocity iterations for the constrain solvers
     */
    void setVelocityIterations(int velocity) { _itvelocity = velocity; }

    /**
     * Returns number of position iterations for the constrain solvers
     *
     * @return number of position iterations for the constrain solvers
     */
    int getPositionIterations() const { return _itposition; }
    
    /**
     * Sets number of position iterations for the constrain solvers
     *
     * Any change will take effect at the time of the next call to update.
     *
     * @param  position number of position iterations for the constrain solvers
     */
    void setPositionIterations(int position) { _itposition = position; }
    
    /**
     * Returns the global gravity vector.
     *
     * @return the global gravity vector.
     */
    const Vec2& getGravity() const { return _gravity; }

    /**
     * Sets the global gravity vector.
     *
     * Any change will take effect at the time of the next call to update.
     *
     * @param  gravity  the global gravity vector.
     */
    void setGravity(const Vec2& gravity);
    
    /**
     * Executes a single step of the physics engine.
     *
     * This method contains the specific update code for this mini-game. It does
     * not handle collisions, as those are managed by the parent class WorldController.
     * This method is called after input is read, but before collisions are resolved.
     * The very last thing that it should do is apply forces to the appropriate objects.
     *
     * Once the update phase is over, but before we draw, we are ready to handle
     * physics.  The primary method is the step() method in world.  This implementation
     * works for all applications and should not need to be overwritten.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt);
    
    /**
     * Returns the bounds for the world controller.
     *
     * @return the bounds for the world controller.
     */
    const Rect& getBounds() const { return _bounds; }
    
    /**
     * Returns true if the object is in bounds.
     *
     * This assertion is useful for debugging the physics.
     *
     * @param obj The object to check.
     *
     * @return true if the object is in bounds.
     */
    bool inBounds(Obstacle* obj);
    
    
#pragma mark -
#pragma mark Object Management
    /**
     * Returns a read-only reference to the list of active obstacles.
     *
     * @return a read-only reference to the list of active obstacles.
     */
    const std::vector<Obstacle*>& getObstacles() { return _objects; }

    /**
     * Immediately adds the object to the physics world
     *
     * The object will be available at the next call to update. The object will be
     * retained, preventing it from being garbage collected.
     *
     * param obj The object to add
     *
     * @retain a reference to the obstacle
     */
    void addObstacle(Obstacle* obj);
    
    /**
     * Immediately removes object to the physics world
     *
     * The object will be released immediately.  If no more objects assert ownership,
     * then the object will be garbage collected.
     *
     * This method of removing objects is very heavy weight, and should only be used
     * for single object removal.  If you want to remove multiple objects, then you
     * should mark them for removal and call garbageCollect.
     *
     * param obj The object to remove
     *
     * @release a reference to the obstacle
     */
    void removeObstacle(Obstacle* obj);
    
    /**
     * Remove all objects marked for removal.
     *
     * The objects will be released immediately. If no more objects assert ownership,
     * then the objects will be garbage collected.
     *
     * This method is the efficient, preferred way to remove objects.
     */
    void garbageCollect();

    /**
     * Remove all objects, emptying this controller.
     *
     * This method is different from a constructor in that the controller can still
     * receive new objects.
     */
    void clear();

    
#pragma mark -
#pragma mark Collision Callback Functions
    
    /**
     * Activates the collision callbacks.
     *
     * If flag is false, then the collision callbacks (even if defined) will be ignored.
     * Otherwise, the callbacks will be executed (on collision) if they are defined.
     *
     * @param  flag whether to activate the collision callbacks.
     */
    void activateCollisionCallbacks(bool flag);
    
    /**
     * Returns true if the collision callbacks are active
     *
     * If this value is false, then the collision callbacks (even if defined) will be ignored.
     * Otherwise, the callbacks will be executed (on collision) if they are defined.
     *
     * @return true if the collision callbacks are active
     */
    bool enabledCollisionCallbacks() const { return _collide; }
    
    /**
     * Called when two fixtures begin to touch
     *
     * This attribute is a dynamically assignable callback and may be changed at
     * any given time.
     * 
     * @param  contact  the contact information
     */
    std::function<void(b2Contact* contact)> onBeginContact;
    
    /**
     * Called when two fixtures cease to touch
     *
     * This attribute is a dynamically assignable callback and may be changed at
     * any given time.
     *
     * @param  contact  the contact information
     */
    std::function<void(b2Contact* contact)> onEndContact;
    
    /**
     * Called after a contact is updated. 
     *
     * This callback allows you to inspect a contact before it goes to the solver. If 
     * you are careful, you can modify the contact manifold (e.g. disable contact).
     *
     * A copy of the old manifold is provided so that you can detect changes.
     *
     * Note: this is called only for awake bodies.
     * Note: this is called even when the number of contact points is zero.
     * Note: this is not called for sensors.
     * Note: if you set the number of contact points to zero, you will not get an
     * EndContact callback. However, you may get a BeginContact callback the next step.
     *
     * This attribute is a dynamically assignable callback and may be changed at
     * any given time.
     *
     * @param  contact      the contact information
     * @param  oldManifold  the contact manifold last iteration
     */
    std::function<void(b2Contact* contact, const b2Manifold* oldManifold)> beforeSolve;
    
    /**
     * Called after the solver is finished.
     *
     * This callback lets you inspect a contact after the solver is finished. This is useful
     * for inspecting impulses.
     *
     * Note: the contact manifold does not include time of impact impulses, which can be
     * arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
     * in a separate data structure.
     * Note: this is only called for contacts that are touching, solid, and awake.
     *
     * This attribute is a dynamically assignable callback and may be changed at
     * any given time.
     *
     * @param  contact  the contact information
     * @param  impulse  the impulse produced by the solver
     */
    std::function<void(b2Contact* contact, const b2ContactImpulse* impulse)> afterSolve;
    
    /**
     * Called when two fixtures begin to touch
     *
     * This method is the static callback required by the Box2d API.  It should
     * not be altered.
     *
     * @param  contact  the contact information
     */
    void BeginContact(b2Contact* contact) override {
        if (onBeginContact != nullptr) {
            onBeginContact(contact);
        }
    }
    
    /**
     * Called when two fixtures cease to touch
     *
     * This method is the static callback required by the Box2d API.  It should
     * not be altered.
     *
     * @param  contact  the contact information
     */
    void EndContact(b2Contact* contact) override {
        if (onEndContact != nullptr) {
            onEndContact(contact);
        }
    }
    
    /**
     * Called after a contact is updated.
     *
     * This callback allows you to inspect a contact before it goes to the solver. If
     * you are careful, you can modify the contact manifold (e.g. disable contact).
     *
     * A copy of the old manifold is provided so that you can detect changes.
     *
     * Note: this is called only for awake bodies.
     * Note: this is called even when the number of contact points is zero.
     * Note: this is not called for sensors.
     * Note: if you set the number of contact points to zero, you will not get an
     * EndContact callback. However, you may get a BeginContact callback the next step.
     *
     * This method is the static callback required by the Box2d API.  It should
     * not be altered.
     *
     * @param  contact      the contact information
     * @param  oldManifold  the contact manifold last iteration
     */
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {
        if (beforeSolve != nullptr) {
            beforeSolve(contact,oldManifold);
        }
    }
    
    /** 
     * Called after the solver is finished.
     * 
     * This callback lets you inspect a contact after the solver is finished. This is useful
     * for inspecting impulses.
     *
     * Note: the contact manifold does not include time of impact impulses, which can be
     * arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
     * in a separate data structure.
     * Note: this is only called for contacts that are touching, solid, and awake.
     *
     * This method is the static callback required by the Box2d API.  It should
     * not be altered.
     *
     * @param  contact  the contact information
     * @param  impulse  the impulse produced by the solver
     */
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {
        if (afterSolve != nullptr) {
            afterSolve(contact,impulse);
        }
    }

    
#pragma mark -
#pragma mark Filter Callback Functions
    
    /**
     * Activates the collision filter callbacks.
     *
     * If flag is false, then the collision filter callbacks (even if defined) will be 
     * ignored. Otherwise, the callbacks will be executed (to test a collision) if they
     * are defined.
     *
     * @param  flag whether to activate the collision callbacks.
     */
    void activateFilterCallbacks(bool flag);
    
    /**
     * Returns true if the collision filter callbacks are active
     *
     * If this value is false, then the collision filter callbacks (even if defined) will
     * be ignored. Otherwise, the callbacks will be executed (to test a collision) if they
     * are defined.
     *
     * @return true if the collision filter callbacks are active
     */
    bool enabledFilterCallbacks() const { return _filters; }
    
    /**
     * Return true if contact calculations should be performed between these two shapes.
     *
     * For performance reasons this is only called when the AABBs begin to overlap.
     *
     * @param  fixtureA the first colliding shape
     * @param  fixtureB the second colliding shape
     *
     * @return true if contact calculations should be performed between these two shapes.
     */
    std::function<bool(b2Fixture* fixtureA, b2Fixture* fixtureB)> shouldCollide;
    
    /**
     * Return true if contact calculations should be performed between these two shapes.
     *
     * For performance reasons this is only called when the AABBs begin to overlap.
     *
     * @param  fixtureA the first colliding shape
     * @param  fixtureB the second colliding shape
     *
     * @return true if contact calculations should be performed between these two shapes.
     */
    bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override {
        if (shouldCollide != nullptr) {
            return shouldCollide(fixtureA,fixtureB);
        }
        return false;
    }

    
#pragma mark -
#pragma mark Destruction Callback Functions
    
    /**
     * Activates the destruction callbacks.
     *
     * If flag is false, then the destruction callbacks (even if defined) will be ignored.
     * Otherwise, the callbacks will be executed (on body destruction) if they are defined.
     *
     * @param  flag whether to activate the collision callbacks.
     */
    void activateDestructionCallbacks(bool flag);
    
    /**
     * Returns true if the destruction callbacks are active
     *
     * If this value is false, then the destruction callbacks (even if defined) will be 
     * ignored. Otherwise, the callbacks will be executed (on body destruction) if they 
     * are defined.
     *
     * @return true if the destruction callbacks are active
     */
    bool enabledDestructionCallbacks() const { return _destroy; }
    
    /**
     * Called when a fixture is about to be destroyed.
     *
     * This function is only called when the destruction is the result of the
     * destruction of its parent body.
     *
     * @param  fixture  the fixture to be destroyed
     */
    std::function<void(b2Fixture* fixture)> destroyFixture;

    /**
     * Called when a joint is about to be destroyed.
     *
     * This function is only called when the destruction is the result of the
     * destruction of one of its attached bodies.
     *
     * @param  joint    the joint to be destroyed
     */
    std::function<void(b2Joint* joint)>     destroyJoint;

    /**
     * Called when a joint is about to be destroyed.
     *
     * This function is only called when the destruction is the result of the
     * destruction of one of its attached bodies.
     *
     * @param  joint    the joint to be destroyed
     */
    void SayGoodbye(b2Joint* joint) override {
        if (destroyJoint != nullptr) {
            destroyJoint(joint);
        }
    }
    
    /**
     * Called when a fixture is about to be destroyed.
     *
     * This function is only called when the destruction is the result of the
     * destruction of its parent body.
     *
     * @param  fixture  the fixture to be destroyed
     */
    void SayGoodbye(b2Fixture* fixture) override {
        if (destroyFixture != nullptr) {
            destroyFixture(fixture);
        }
    }


#pragma mark -
#pragma mark Query Functions

    /**
     * Query the world for all fixtures that potentially overlap the provided AABB.
     *
     * The AABB is specified by a Cocos2d rectangle.
     *
     * @param  callback a user implemented callback function.
     * @param  rect     the axis-aligned bounding box
     */
    void queryAABB(std::function<bool(b2Fixture* fixture)> callback, const Rect& aabb) const;

    /**
     * Ray-cast the world for all fixtures in the path of the ray.
     *
     * The callback controls whether you get the closest point, any point, or n-points.
     * The ray-cast ignores shapes that contain the starting point.
     * @param  callback a user implemented callback function.
     * @param  point1   the ray starting point
     * @param  point2   the ray ending point
     */
    void rayCast(std::function<float(b2Fixture* fixture, const Vec2& point, const Vec2& normal, float fraction)> callback,
                 const Vec2& point1, const Vec2& point2) const;


    
CC_CONSTRUCTOR_ACCESS:
#pragma mark -
#pragma mark Initializers
    /**
     * Creates an inactive world controller
     *
     * The Box2d world will not be created until the appropriate init is called.
     */
    WorldController();
    
    /**
     * Dispose of all resources allocated to this controller.
     */
    ~WorldController();
    
    /**
     * Initializes a new physics world
     *
     * The specified bounds are in terms of the Box2d world, not the screen.
     * A few attached to this Box2d world should have ways to convert between
     * the coordinate systems.
     *
     * This constructor will use the default gravitational value.
     *
     * @param  bounds   The game bounds in Box2d coordinates
     * @param  gravity  The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    virtual bool init(const Rect& bounds);
    
    /**
     * Initializes a new physics world
     *
     * The specified bounds are in terms of the Box2d world, not the screen.
     * A few attached to this Box2d world should have ways to convert between
     * the coordinate systems.
     *
     * @param  bounds   The game bounds in Box2d coordinates
     * @param  gravity  The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    virtual bool init(const Rect& bounds, const Vec2& gravity);
    
};

NS_CC_END
#endif /* defined(__CU_WORLD_CONTROLLER_H__) */
