#ifndef __M_LEVEL_INSTANCE_H__
#define __M_LEVEL_INSTANCE_H__

#include <tuple>
#include <vector>
#include <string>
#include <cocos2d.h>
#include <cornell.h>
#include <M_MovingObject.h>
#include <M_Pedestrian.h>
#include <M_Car.h>
#include <ActionQueue.h>
#include <M_Shadow.h>
#include <M_Caster.h>

using namespace std;
using namespace cocos2d;

// Forward declaration
class LevelInstance;

template <class T>
class LevelObjectMetadata {
	friend class LevelInstance;

private:
	T* object;
	Vec2 position;
};


class LevelInstance : public Asset {
	
protected:

	enum struct StaticObjectType {
		BUILDING_1, // DO NOT INITIALIZE ANY OF THE ENUM CONSTANTS

					// TODO add more static object types



					/* DO NOT ADD ANY OTHER ENUM CONSTANT BELOW THIS LINE
					* OTHER THAN NUM_ELEMENTS */
		NUM_ELEMENTS
	};

	static const map<string, StaticObjectType> staticObjectMap;


	enum struct MovingObjectType {
		PEDESTRIAN, CAR
	};

	struct ShadowMetadata : public LevelObjectMetadata<Shadow> {};


	struct CasterMetadata : public LevelObjectMetadata<OurMovingObject<Caster>> {
		float heading;
	};


	struct StaticObjectMetadata : public LevelObjectMetadata<Obstacle> {
		StaticObjectType type;
	};


	template <class T>
	struct MovingObjectMetadata : public LevelObjectMetadata<OurMovingObject<T>> {
		float heading;
		ActionQueue<T> actions;
	};


	typedef MovingObjectMetadata<Pedestrian> PedestrianMetadata;

	typedef MovingObjectMetadata<Car> CarMetadata;


	struct LevelMetadata {
		int _levelIndex;
		string _backgroundPath;
		Size _size;
		ShadowMetadata _playerPos;
		CasterMetadata _casterPos;
		vector<StaticObjectMetadata> _staticObjects;
		vector<PedestrianMetadata> _pedestrians;
		vector<CarMetadata> _cars;
		// TODO add powerups
	};


	LevelMetadata _metadata;
	// TODO add stuff from JSLevelModel

	/** The root node of this level */
	Node* _root;
	/** The level drawing scale (difference between physics and drawing coordinates) */
	Vec2 _scale;
	/** Reference to the physics root of the scene graph */
	Node* _worldnode;
	/** Reference to the debug root of the scene graph */
	Node* _debugnode;
	/** The physics word; part of the model (though listeners elsewhere) */
	WorldController* _world;

	/**
	* Creates a new game level with the given source file.
	*
	* This method does NOT load the level. You must call the load() method to do that.
	* This method returns false if file does not exist.
	*
	* @return  an autoreleased level file
	*/
	static LevelInstance* create(string filePath);

	/**
	* Initializes _levelMetadata using a JSON reader. It does not create the
	* actual objects. If, at any point during parsing, the JSON reader
	* encounters an invalid value or if the necessary field does not exist (the
	* only exceptions being the background path and the moving object action
	* counter fields), we fail to load the level from the JSON file. Otherwise,
	* we succeed.
	*
	* @return true if loading succeeds, false otherwise
	*/
	bool initializeMetadata();

	bool populateLevel();

	bool loadTextures();

	void reset();

	virtual bool load() override {
		if (initializeMetadata()) return populateLevel();
		return false;
	}

	virtual void unload() override;

	LevelInstance();

	virtual ~LevelInstance();
};



#endif /* __M_LEVEL_INSTANCE_H__ */