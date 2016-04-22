//
//  CUWorldController.cpp
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
// This is the root, so there are a lot of includes
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "CUWorldController.h"
#include "CUObstacle.h"

NS_CC_BEGIN

#pragma mark Constants

/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -9.8f

#pragma mark -
#pragma mark Proxy Classes

/**
 * A lightweight b2QueryCallback proxy.
 *
 * This class allows us to replace the listener class with a modern closure.
 */
class QueryProxy : public b2QueryCallback {
public:
    /**
     * Creates a new query proxy
     */
    QueryProxy() { onQuery = nullptr; }

    /**
     * Returns false to terminate an AABB query
     *
     * This function is called for each fixture found in the query AABB.
     *
     * @param  fixture  the fixture selected
     *
     * @return false to terminate the query.
     */
    std::function<bool(b2Fixture* fixture)> onQuery;
    
    /**
     * Returns false to terminate an AABB query
     *
     * This function is called for each fixture found in the query AABB.
     *
     * @param  fixture  the fixture selected
     *
     * @return false to terminate the query.
     */
    bool ReportFixture(b2Fixture* fixture) override {
        if (onQuery != nullptr) {
            return onQuery(fixture);
        }
        return false;
    }
};

/**
 * A lightweight b2RayCastCallback proxy.
 *
 * This class allows us to replace the listener class with a modern closure.
 */
class RaycastProxy : public b2RayCastCallback {
public:
    /**
     * Creates a new raycast proxy
     */
    RaycastProxy() { onQuery = nullptr; }

    /**
     * Called for each fixture found in the query. 
     *
     * This callback controls how the ray cast proceeds by returning a float.  If
     * -1, it ignores this fixture and continues.  If 0, it terminates the ray
     * cast.  If 1, it does not clip the ray and continues.  Finally, for any
     * fraction, it clips the ray at that point.
     *
     * @param  fixture  the fixture hit by the ray
     * @param  point    the point of initial intersection
     * @param  normal   the normal vector at the point of intersection
     * @param  faction  the fraction to return
     *
     * @return -1 to filter, 0 to terminate, fraction to clip the ray, 1 to continue
     */
    std::function<float(b2Fixture* fixture, const Vec2& point, const Vec2& normal, float fraction)> onQuery;
    
    /**
     * Called for each fixture found in the query.
     *
     * This callback controls how the ray cast proceeds by returning a float.  If
     * -1, it ignores this fixture and continues.  If 0, it terminates the ray
     * cast.  If 1, it does not clip the ray and continues.  Finally, for any
     * fraction, it clips the ray at that point.
     *
     * @param  fixture  the fixture hit by the ray
     * @param  point    the point of initial intersection
     * @param  normal   the normal vector at the point of intersection
     * @param  faction  the fraction to return
     *
     * @return -1 to filter, 0 to terminate, fraction to clip the ray, 1 to continue
     */
    float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override {
        if (onQuery != nullptr) {
            return onQuery(fixture,Vec2(point.x,point.y),Vec2(normal.x,normal.y),fraction);
        }
        return -1;
    }
};


#pragma mark -
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
WorldController* WorldController::create(const Rect& bounds) {
    WorldController* controller = new (std::nothrow) WorldController();
    if (controller && controller->init(bounds)) {
        controller->autorelease();
        return controller;
    }
    CC_SAFE_DELETE(controller);
    return nullptr;
}

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
WorldController* WorldController::create(const Rect& bounds, const Vec2& gravity) {
    WorldController* controller = new (std::nothrow) WorldController();
    if (controller && controller->init(bounds,gravity)) {
        controller->autorelease();
        return controller;
    }
    CC_SAFE_DELETE(controller);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers

/**
 * Creates an inactive world controller
 *
 * The Box2d world will not be created until the appropriate init is called.
 */
WorldController::WorldController() :
_world(nullptr),
_collide(false),
_filters(false),
_destroy(false) {
    _lockstep   = false;
    _stepssize  = DEFAULT_WORLD_STEP;
    _itvelocity = DEFAULT_WORLD_VELOC;
    _itposition = DEFAULT_WORLD_POSIT;
    _gravity = Vec2(0,DEFAULT_GRAVITY);
    
    onBeginContact = nullptr;
    onEndContact   = nullptr;
    beforeSolve    = nullptr;
    afterSolve     = nullptr;
    shouldCollide  = nullptr;
    destroyFixture = nullptr;
    destroyJoint   = nullptr;
}

/**
 * Dispose of all resources allocated to this controller.
 */
WorldController::~WorldController() {
    clear();
    delete _world;
    _world  = nullptr;
    onBeginContact = nullptr;
    onEndContact   = nullptr;
    beforeSolve    = nullptr;
    afterSolve     = nullptr;
    shouldCollide  = nullptr;
    destroyFixture = nullptr;
    destroyJoint   = nullptr;
}

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
bool WorldController::init(const Rect& bounds) {
    return init(bounds,_gravity);
}

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
bool WorldController::init(const Rect& bounds, const Vec2& gravity) {
    _bounds = bounds;
    _world = new b2World(b2Vec2(gravity.x,gravity.y));
    if (_world) {
        return true;
    }
    return false;
}


#pragma mark -
#pragma mark Object Management
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
void WorldController::addObstacle(Obstacle* obj) {
    CCASSERT(inBounds(obj), "Object is not in bounds");
    _objects.push_back(obj);
    obj->retain();
    obj->activatePhysics(*_world);
}

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
void WorldController::removeObstacle(Obstacle* obj) {
    for(auto it = _objects.begin(); it != _objects.end(); ++it) {
        if (*it == obj) {
            obj->deactivatePhysics(*_world);
            obj->release();
            _objects.erase(it);
            return;
        }
    }
    CCASSERT(false, "Physics object not present in world");
}

/**
 * Remove all objects marked for removal.
 *
 * The objects will be released immediately. If no more objects assert ownership,
 * then the objects will be garbage collected.
 *
 * This method is the efficient, preferred way to remove objects.
 */
void WorldController::garbageCollect() {
    size_t count = 0;
    size_t pos = 0;
    for(size_t ii = 0; ii < _objects.size(); ii++) {
        if (_objects[ii]->isRemoved()) {
            _objects[ii]->deactivatePhysics(*_world);
            _objects[ii]->release();
        } else {
            if (pos != ii) {
                _objects[pos] = _objects[ii];
            }
            pos++;
            count++;
        }
    }
    _objects.resize(count);
}

/**
 * Remove all objects, emptying this controller.
 *
 * This method is different from a constructor in that the controller can still
 * receive new objects.
 */
void WorldController::clear() {
    for(auto it = _objects.begin() ; it != _objects.end(); ++it) {
        Obstacle* obj = *it;
        obj->deactivatePhysics(*_world);
        obj->release();
    }
    _objects.clear();
}


#pragma mark -
#pragma mark Physics Handling

/**
 * Sets the global gravity vector.
 *
 * Any change will take effect at the time of the next call to update.
 *
 * @param  gravity  the global gravity vector.
 */
void WorldController::setGravity(const Vec2& gravity) {
    _gravity = gravity;
    if (_world != nullptr) {
        _world->SetGravity(b2Vec2(gravity.x,gravity.y));
    }
}

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
void WorldController::update(float dt) {
    // Turn the physics engine crank.
    _world->Step((_lockstep ? _stepssize : dt),_itvelocity,_itposition);
    
    // Post process all objects after physics (this updates graphics)
    for(auto it = _objects.begin() ; it != _objects.end(); ++it) {
        Obstacle* obj = *it;
        obj->update(dt);
    }
}

/**
 * Returns true if the object is in bounds.
 *
 * This assertion is useful for debugging the physics.
 *
 * @param obj The object to check.
 *
 * @return true if the object is in bounds.
 */
bool WorldController::inBounds(Obstacle* obj) {
    bool horiz = (_bounds.origin.x <= obj->getX() &&
                  obj->getX() <= _bounds.origin.x+_bounds.size.width);
    bool vert  = (_bounds.origin.y <= obj->getY() &&
                  obj->getY() <= _bounds.origin.y+_bounds.size.height);
    return horiz && vert;
}

#pragma mark -
#pragma mark Callback Activation

/**
 * Activates the collision callbacks.
 *
 * If flag is false, then the collision callbacks (even if defined) will be ignored.
 * Otherwise, the callbacks will be executed (on collision) if they are defined.
 *
 * @param  flag whether to activate the collision callbacks.
 */
void WorldController::activateCollisionCallbacks(bool flag) {
    if (_collide == flag) {
        return;
    }
    
    _world->SetContactListener(flag ? this : nullptr);
    _collide = flag;
}

/**
 * Activates the collision filter callbacks.
 *
 * If flag is false, then the collision filter callbacks (even if defined) will be
 * ignored. Otherwise, the callbacks will be executed (to test a collision) if they
 * are defined.
 *
 * @param  flag whether to activate the collision callbacks.
 */
void WorldController::activateFilterCallbacks(bool flag) {
    if (_filters == flag) {
        return;
    }
    
    _world->SetContactFilter(flag ? this : nullptr);
    _filters = flag;
}

/**
 * Activates the destruction callbacks.
 *
 * If flag is false, then the destruction callbacks (even if defined) will be ignored.
 * Otherwise, the callbacks will be executed (on body destruction) if they are defined.
 *
 * @param  flag whether to activate the collision callbacks.
 */
void WorldController::activateDestructionCallbacks(bool flag) {
    if (_destroy == flag) {
        return;
    }
    
    _world->SetDestructionListener(flag ? this : nullptr);
    _destroy = flag;
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
void WorldController::queryAABB(std::function<bool(b2Fixture* fixture)> callback, const Rect& aabb) const {
    b2AABB b2box;
    b2box.lowerBound.Set(aabb.origin.x, aabb.origin.y);
    b2box.upperBound.Set(aabb.origin.x+aabb.size.width, aabb.origin.y+aabb.size.height);
    QueryProxy proxy;
    proxy.onQuery = callback;
    _world->QueryAABB(&proxy, b2box);
}

/** 
 * Ray-cast the world for all fixtures in the path of the ray. 
 *
 * The callback controls whether you get the closest point, any point, or n-points.
 * The ray-cast ignores shapes that contain the starting point.
 * @param  callback a user implemented callback function.
 * @param  point1   the ray starting point
 * @param  point2   the ray ending point
 */
void WorldController::rayCast(std::function<float(b2Fixture* fixture, const Vec2& point, const Vec2& normal, float fraction)> callback,
                              const Vec2& point1, const Vec2& point2) const {
    RaycastProxy proxy;
    proxy.onQuery = callback;
    _world->RayCast(&proxy, b2Vec2(point1.x,point1.y), b2Vec2(point2.x,point2.y));
}



NS_CC_END
