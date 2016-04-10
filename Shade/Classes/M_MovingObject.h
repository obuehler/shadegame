#ifndef __MOVING_OBJECT_H__
#define __MOVING_OBJECT_H__

//#include "cocos2d.h"
#include <tuple>

#include <cornell/CUSimpleObstacle.h>
#include <cornell/CUPolygonNode.h>
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
#include <MovingObjectMetadata.h>


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

/*template <class T>
class ActionQueue; */

// T must be an enum, whose values all have a method named
// act(SimpleObstacle object, SimpleObstacle shadow).
template <class T>
class OurMovingObject : public Ref {

protected:

	BoxObstacle * object;
	BoxObstacle * shadow;

	/** The current horizontal movement of the movingobject */
	float _horizontalMovement;
	/** The current horizontal movement of the movingobject */
	float _verticalMovement;
	/** Whether the movingobject image is facing right */
	bool _faceRight;
	/** Ground sensor to represent our feet */
	b2Fixture*  _sensorFixture; // TODO change this with many small fixtures for exposure calculation

public:
	ActionQueue<T>* _actionQueue;

	static OurMovingObject* create(const Vec2& pos, BoxObstacle * m, BoxObstacle * s) {
		OurMovingObject* mover = new (std::nothrow) OurMovingObject();
		if (mover && mover->init(pos, m, s)) {
			CCLOG("%s", "asd");
			mover->autorelease();
			//mover->retain();
			return mover;
		}
		CC_SAFE_DELETE(mover);
		return nullptr;
	}

	bool init(const Vec2& pos, BoxObstacle * m, BoxObstacle * s) {
		SceneManager* scene = AssetManager::getInstance()->getCurrent();
		Texture2D* image = scene->get<Texture2D>("b1");

		// Multiply by the scaling factor so we can be resolution independent
		float cscale = Director::getInstance()->getContentScaleFactor();
		Size nsize = image->getContentSize()*cscale;

		_actionQueue = ActionQueue<T>::create();
		_actionQueue->retain();

		setShadow(s);
		setObject(m);

		/*nsize.width *= scale.x;
		nsize.height *= scale.y;
		*/
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
				if (_actionQueue->_head->_counter == _actionQueue->_head->_length) {
					T::act(action->_type, this->object, this->shadow);
				}
				action->_counter--;
			}
		}
	}
	void setShadow(BoxObstacle * s) {
		shadow = s;
	}
	void setObject(BoxObstacle * o) {
		object = o;
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