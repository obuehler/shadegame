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
#define OBJECT_BIT 0x0002
/** Category bit for shadows in a level */
#define SHADOW_BIT 0x0004
/** Category bit for the caster */
#define CASTER_BIT 0x0008
/** Category bit for the character sensor fixtures */
#define CHARACTER_SENSOR_BIT 0x0010
/** Category bit for the character itself */
#define CHARACTER_BIT 0x0020
/** Category bit for the pedestrian */
#define PEDESTRIAN_BIT 0x0040
/** Category bit for the filter which doesnt collide */
#define EMPTY_BIT 0x0080
/** Category bit for the latch indicator */
#define LATCH_BIT 0x0100
/** Category bit for the car */
#define CAR_BIT 0x0200

/** Default scale from Box2D to intended pixel coordinates */
#define BOX2D_SCALE 50.0f

/**
* If, in the JSON file, the moving object does not have a "cyclic" field,
* the actual moving object's action queue will cycle if this is true and will
* not if it is false.
*/
#define DEFAULT_CYCLING_VALUE false

/** The name of the level index field */
#define LEVEL_INDEX_FIELD "index"
/** The name of the level background path field */
#define BACKGROUND_PATH_FIELD "background"
/** The name of the level size field */
#define SIZE_FIELD "pixelSize"
/** The name of the target pixel's x field */
#define TARGET_X_FIELD "targetPixelX"
/** The name of the target pixel's y field */
#define TARGET_Y_FIELD "targetPixelY"
/**
* The name of the x-coordinate field in all coordinate-based JSON objects in
* the level files
*/
#define X_FIELD "x"
/**
* The name of the y-coordinate field in all coordinate-based JSON objects in
* the level files
*/
#define Y_FIELD "y"

#define WIDTH_FIELD "width"
#define HEIGHT_FIELD "height"
/**
* The name of the heading coordinate field in all JSON objects in the level
* files that have a heading
*/
#define HEADING_FIELD "bearing"
/** The name of the player site field */
#define SHADOW_POSITION_FIELD "playerSite"
/** The name of the caster site field */
#define CASTER_POSITION_FIELD "casterSite"
/** The name of the static object list field */
#define STATIC_OBJECTS_FIELD "staticObjects"
/** The name of the pedestrian list field */
#define PEDESTRIANS_FIELD "pedestrians"
/** The name of the car list field */
#define CARS_FIELD "cars"
/**
* The name of the type field in all JSON objects in the level files that have
* a type, such as ActionType or a static object type
*/
#define TYPE_FIELD "type"
/**
* The name of the action list field for the JSON representations of the moving
* objects in the level files
*/
#define ACTIONS_FIELD "actionQueue"
/** The name of the length field under each element of the JSON action lists */
#define LENGTH_FIELD "length"
/** The name of the counter field under each element of the JSON action lists*/
#define COUNTER_FIELD "counter"
/**
* The name of the field in the JSON representations of the moving objects in
* the level files that indicates whether that moving object's action queue
* will be cyclic or not
*/
#define CYCLIC_FIELD "cycleStart"

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
	string _name;
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

	inline void printWarning(const string& warningMessage) {
		CCLOG(warningMessage.c_str());
	}

	void failToLoad(const char* errorMessage);

	template <class T>
	void pushAction(bool noTarget, bool noLength, bool noCounter, bool noHeading, ActionQueue<T>* actions, float heading, typename T::ActionType type, int length, int counter, Vec2 target) {
		if (noTarget) {
			if (noHeading) {
				if (noCounter) {
					actions->push(type, length);
				}
				else {
					actions->push(type, length, counter);
				}
			}
			else {
				if (noCounter) {
					actions->push(heading, type, length);
				}
				else {
					actions->push(heading, type, length, counter);
				}
			}
		}
		else {
			if (noLength) {
				if (noHeading) {
					actions->push(type, target);
				}
				else {
					actions->push(heading, type, target);
				}
			}
			else {
				if (noHeading) {
					if (noCounter) {
						actions->push(type, length, target);
					}
					else {
						actions->push(type, length, counter, target);
					}
				}
				else {
					if (noCounter) {
						actions->push(heading, type, length, target);
					}
					else {
						actions->push(heading, type, length, counter, target);
					}
				}
			}
		}
	}

	template <class T>
	bool deserializeAction(JSONReader& reader, int pedestrianIndex, int actionIndex, ActionQueue<T>* actions) {
		typename T::ActionType type;
		try {
			type = T::actionMap.at(reader.getString(TYPE_FIELD));
		}
		catch (out_of_range) {
			failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " type");
			return false;
		}
		Vec2 target(reader.getNumber(TARGET_X_FIELD, -1.0f) / BOX2D_SCALE,
			reader.getNumber(TARGET_Y_FIELD, -1.0f) / BOX2D_SCALE);
		bool noTarget = target.x == -1.0f / BOX2D_SCALE || target.y == -1.0f / BOX2D_SCALE;
		if (noTarget) printWarning("Attention: invalid target for " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1));
		int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
		bool noLength = length <= 0;
		if (noLength) {
			if (noTarget) {
				failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " length");
				return false;
			}
			else {
				printWarning("Attention: invalid length for " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1));
			}
		}
		// If there is no heading, the act method will recognize the negative heading value and not change heading
		float heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
		bool noHeading = heading < -1.0f;
		if (noHeading) printWarning("Attention: invalid heading for " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1));
		// If there is no counter, it is equal to length
		int counter = (int)reader.getNumber(COUNTER_FIELD, -1.0f);
		bool noCounter = counter <= 0 || counter > length;
		if (noCounter) printWarning("Attention: invalid counter for " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1));
		pushAction<T>(noTarget, noLength, noCounter, noHeading, actions, heading, type, length, counter, target);
		return true;
	}

	template <class T>
	bool loadMovingObject(JSONReader& reader, int pedestrianCount, vector<MovingObjectMetadata<T>>& vec) {
		for (int pedestrianIndex = 0; pedestrianIndex < pedestrianCount; pedestrianIndex++) {
			if (reader.startObject()) {
				MovingObjectMetadata<T> data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _size.width) {
					failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " position.x");
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.x > _size.height) {
					failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " position.y");
					return false;
				}
				data.heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
				if (data.heading < -1.0f) {
					failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " heading");
					return false;
				}
				int actionStartIndex = 0;
				bool queueIsCyclic = false;
				// Initialize actions with empty action queue
				data.actions = ActionQueue<T>::create();
				data.actions->retain();
				if (reader.isArray(ACTIONS_FIELD)) {
					int actionCoun = reader.startArray(ACTIONS_FIELD);
					for (int actionInde = 0; actionInde < actionCoun; actionInde++) {
						if (reader.startObject()) {
							if (reader.getBool(CYCLIC_FIELD, false)) {
								actionStartIndex = actionInde;
								queueIsCyclic = true;
							}
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionInde + 1));
							return false;
						}
					}
					reader.endArray();
				}
				else {
					failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " actions");
					return false;
				}
				if (reader.isArray(ACTIONS_FIELD)) {
					int actionCount = reader.startArray(ACTIONS_FIELD);
					if (queueIsCyclic) {
						for (int count = 0; count < actionStartIndex; count++) {
							reader.advance();
						}
					}

					for (int actionIndex = actionStartIndex; actionIndex < actionCount; actionIndex++) {
						if (reader.startObject()) {
							if (!deserializeAction(reader, pedestrianIndex, actionIndex, data.actions)) return false;
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							failToLoad("Failed to assign " + T::name + " " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1));
							return false;
						}
					}
					reader.endArray();
					data.actions->setCycling(queueIsCyclic);
					if (queueIsCyclic) {
						ActionQueue<T> additionalQueue;
						reader.startArray(ACTIONS_FIELD);
						for (int count = 0; count < actionStartIndex; count++) {
							reader.startObject();
							if (!deserializeAction(reader, pedestrianIndex, count, data.actions)) return false;
							reader.endObject();
							reader.advance();
						}
						reader.endArray();
						data.actions->force(additionalQueue, true);
					}
				}
				vec.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				failToLoad("Failed to get " + T::name + " " + std::to_string(pedestrianIndex + 1));
				return false;
			}
		}
		return true;
	}

public:

	void populateLevel(bool reset);

	virtual bool load() override;

	virtual void unload() override;

	LevelInstance(void);

	virtual ~LevelInstance(void);
};



#endif /* __M_LEVEL_INSTANCE_H__ */