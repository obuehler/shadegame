#ifndef __MOVING_OBJECT_H__
#define __MOVING_OBJECT_H__

//#include "cocos2d.h"
#include <tuple>

#include <cornell/CUSimpleObstacle.h>
#include <cornell/CUPolygonNode.h>
#include <cornell/CUBoxObstacle.h>
#include <cornell/CUAssetManager.h>
#include <cornell/CUSceneManager.h>
#include <cornell.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <ActionQueue.h>


//#include <ActionQueue.h>
//#include <MovingObjectMetadata.h>


#pragma mark : Constants
#define MOVER_NAME "mover"
/** The density for most physics objects */
#define BASIC_DENSITY   0.0f
/** The density for a bullet */
#define HEAVY_DENSITY   10.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.0f
/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color3B::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192


using namespace std;
using namespace cocos2d;

/*template <class T>
class ActionQueue; */

// T must be an enum, whose values all have a method named
// act(SimpleObstacle object, SimpleObstacle shadow).
template <class T>
class OurMovingObject : public Ref {

protected:

	BoxObstacle* object;
	BoxObstacle* shadow;

	/** The current horizontal movement of the movingobject */
	float _horizontalMovement;
	/** The current horizontal movement of the movingobject */
	float _verticalMovement;
	/** Whether the movingobject image is facing right */
	bool _faceRight;
	/** Ground sensor to represent our feet */
	b2Fixture*  _sensorFixture; // TODO change this with many small fixtures for exposure calculation

	OurMovingObject() : Ref(), object(nullptr), shadow(nullptr) {}
	~OurMovingObject() {		
		releaseShadow();
		releaseObject();

		// DO NOT RELEASE THE ACTION QUEUE, THE LEVEL METADATA OWNS IT
		_actionQueue = nullptr;
	}

public:
	ActionQueue<T>* _actionQueue;

	/** Creates the moving object with object = m and shadow = s */
	static OurMovingObject* create(ActionQueue<T>* queue, BoxObstacle * m, BoxObstacle * s) {
		OurMovingObject* mover = new (std::nothrow) OurMovingObject();
		if (mover && mover->init(queue, m, s)) {
			mover->autorelease();
			//mover->retain();
			return mover;
		}
		CC_SAFE_DELETE(mover);
		return nullptr;
	}

	/** Initializes the moving object with object = m and shadow = s */
	bool init(ActionQueue<T>* queue, BoxObstacle * m, BoxObstacle * s) {
		_actionQueue = queue;
		_actionQueue->retain(); // queue is retained since it is a separate copy than in the metadata
		setShadow(s);
		setObject(m);
		return true;
	}
	
	/**
	* Executes the next move in the _actionQueue.
	*/
	void act() {
		if (!_actionQueue->isEmpty()) {
			while (!_actionQueue->isEmpty() && _actionQueue->_head->_counter <= 0) {
				assert(_actionQueue->_head->_length > 0);
				if (_actionQueue->tailHasNext()) {
					_actionQueue->_head->_counter = _actionQueue->_head->_length;
				}
				_actionQueue->next(); // The head of the queue gets popped if not cyclic
			}
			// Check to see if we are left with an empty queue
			if (!_actionQueue->isEmpty()) {
				/* The head of the queue */
				shared_ptr<ActionQueue<typename T>::ActionNode> action(_actionQueue->_head);
				// TODO the act() method of action types take the current and remaining
				// number of frames as arguments
				if (action->_counter == action->_length) {
					object->setAngle(action->_bearing);
					shadow->setAngle(action->_bearing);
				}
				T::act(action->_type, action->_length, action->_counter, object, shadow);
				action->_counter--;
			}
		}
	}
	void setShadow(BoxObstacle * s) {
		releaseShadow();
		shadow = s;
		if (shadow != nullptr) shadow->retain();
	}
	void setObject(BoxObstacle * o) {
		releaseObject();
		object = o;
		object->retain();
	}

	BoxObstacle* getShadow() {
		return shadow;
	}

	BoxObstacle* getObject() {
		return object;
	}

	Vec2 getPosition(){
		return object->getPosition();
	}

	void releaseShadow() {
		if (shadow != nullptr) shadow->release();
	}

	void releaseObject() {
		if (object != nullptr) object->release();
	}

	void setHorizontalMovement(float value) {
		_horizontalMovement = value;
		bool face = _horizontalMovement > 0;
		/*
		if (_horizontalMovement == 0 || _faceRight == face) {
			return;
		}
		*/
    
		// Change facing
		/*
		TexturedNode* image = dynamic_cast<TexturedNode*>(_node);
		if (image != nullptr) {
			image->flipHorizontal(!image->isFlipHorizontal());
		}
		_faceRight = (_horizontalMovement > 0);
		*/
	}

	void setVerticalMovement(float value) {
		_verticalMovement = value;
	}

	float getHorizontalMovement() {
		return _horizontalMovement;
	}
	float getVerticalMovement() {
		return _verticalMovement;
	}

	void applyForce() {
		if (!object->isActive()) {
			return;
		}
		b2Vec2 moveVector = b2Vec2(getHorizontalMovement(), getVerticalMovement());
		b2Body* obody = object->getBody();
		obody->SetLinearVelocity(moveVector);
		b2Body* sbody = shadow->getBody();
		sbody->SetLinearVelocity(moveVector);
		
		//shadow->setBodyState(*sbody);
	}
};

#endif /*__MOVING_OBJECT_H__*/