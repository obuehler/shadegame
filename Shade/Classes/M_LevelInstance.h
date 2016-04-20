#ifndef __M_LEVEL_INSTANCE_H__
#define __M_LEVEL_INSTANCE_H__

#include <tuple>
#include <vector>
#include <string>
#include <cocos2d.h>
#include <cornell.h>
#include <cornell/CUAsset.h>
#include <M_MovingObject.h>
#include <M_Pedestrian.h>
#include <M_Car.h>
#include <ActionQueue.h>
#include <M_Shadow.h>
#include <M_Caster.h>

// No category bit should have value 0x01 since that's Box2D default
/** Category bit for solid level objects */
#define OBJECT_BIT 0x02
/** Category bit for shadows in a level */
#define SHADOW_BIT 0x04
/** Category bit for the caster */
#define CASTER_BIT 0x08
/** Category bit for the character sensor fixtures */
#define CHARACTER_SENSOR_BIT 0x10
/** Category bit for the character itself */
#define CHARACTER_BIT 0x20

using namespace std;
using namespace cocos2d;

// Forward declaration
class LevelInstance;

template <class T>
class LevelObjectMetadata {
	friend class LevelInstance;

public:
	T* object;
	Vec2 position;

	LevelObjectMetadata<T>() : object(nullptr) {}
};


class LevelInstance : public Asset {
	
public:

	/* enum struct StaticObjectType {
		BUILDING_1, // DO NOT INITIALIZE ANY OF THE ENUM CONSTANTS

					// TODO add more static object types



					// DO NOT ADD ANY OTHER ENUM CONSTANT BELOW THIS LINE
					//OTHER THAN NUM_ELEMENTS
		NUM_ELEMENTS
	};

	static const map<string, StaticObjectType> staticObjectMap; */

	static Size rootSize;


	enum struct MovingObjectType {
		PEDESTRIAN, CAR
	};

	struct ShadowMetadata : public LevelObjectMetadata<Shadow> {};


	struct CasterMetadata : public LevelObjectMetadata<OurMovingObject<Caster>> {
		float heading;
	};


	struct StaticObjectMetadata : public LevelObjectMetadata<BoxObstacle> {
		string type;
		BoxObstacle* shadow;

		StaticObjectMetadata() : LevelObjectMetadata<BoxObstacle>(), shadow(nullptr) {}
	};


	template <class T>
	struct MovingObjectMetadata : public LevelObjectMetadata<OurMovingObject<T>> {
		float heading;
		ActionQueue<T>* actions;

		MovingObjectMetadata() : 
			LevelObjectMetadata<OurMovingObject<T>>(), actions(nullptr) {}
	};


	typedef MovingObjectMetadata<Pedestrian> PedestrianMetadata;

	typedef MovingObjectMetadata<Car> CarMetadata;

	int _levelIndex;
	string _backgroundPath;
	Size _size;
	ShadowMetadata _playerPos;
	CasterMetadata _casterPos;
	vector<StaticObjectMetadata> _staticObjects;
	vector<PedestrianMetadata> _pedestrians;
	vector<CarMetadata> _cars;

	/**
	* Creates a new game level with no source file.
	*
	* The source file can be set at any time via the setFile() method. This method
	* does NOT load the asset.  You must call the load() method to do that.
	*
	* @return  an autoreleased level file
	*/
	static LevelInstance* create();

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

	inline void failToLoad(const string& errorMessage) {
		failToLoad(errorMessage.c_str());
	}

	void failToLoad(const char* errorMessage);

public:

	void populateLevel(bool reset);

	virtual bool load() override;

	virtual void unload() override;

	LevelInstance(void);

	virtual ~LevelInstance(void);
};



#endif /* __M_LEVEL_INSTANCE_H__ */