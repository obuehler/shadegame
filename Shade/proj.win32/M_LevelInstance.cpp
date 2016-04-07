#include "M_LevelInstance.h"

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
#define SIZE_FIELD "size"
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
#define CYCLIC_FIELD "cyclic"

const map<string, LevelInstance::StaticObjectType> LevelInstance::staticObjectMap = {
	{ "building1", StaticObjectType::BUILDING_1 }
	// TODO add more static object types
};

/**
* Creates a new game level with the given source file.
*
* This method does NOT load the level. You must call the load() method to do that.
* This method returns false if file does not exist.
*
* @return  an autoreleased level file
*/
LevelInstance* LevelInstance::create(string filePath) {
	LevelInstance* level = new (nothrow) LevelInstance();
	if (level && level->init(filePath)) {
		level->autorelease();
		return level;
	}
	CC_SAFE_DELETE(level);
	return nullptr;
}


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
bool LevelInstance::initializeMetadata() {

	/* The JSON reader to be used for reading the level file */
	JSONReader reader;

	reader.initWithFile(_file);

	/* Try beginning parsing */
	if (!reader.startJSON()) {
		CCASSERT(false, "Failed to load level file");
		return false;
	}

	// Set level index.
	_metadata._levelIndex = (int)reader.getNumber(LEVEL_INDEX_FIELD, -1.0f);
	if (_metadata._levelIndex < 0) {
		CCASSERT(false, "Failed to assign level index");
		return false;
	}

	/* Set the background image file path. If the associated field does not
	 * exist in the JSON file, the JSON reader sets _backgroundPath to an empty
	 * string. Levels without background images are still fine, we will just set
	 * the background to a plain color in loadTextures(). */
	_metadata._backgroundPath = reader.getString(BACKGROUND_PATH_FIELD);
	if (_metadata._backgroundPath == "") CCLOG("%s", "No background path");

	// Set the level width and height values
	if (reader.startObject(SIZE_FIELD)) {
		_metadata._size.width = reader.getNumber(X_FIELD);
		if (_metadata._size.width <= 0.0f) {
			CCASSERT(false, "Failed to assign level width");
			return false;
		}
		_metadata._size.height = reader.getNumber(Y_FIELD);
		if (_metadata._size.height <= 0.0f) {
			CCASSERT(false, "Failed to assign level height");
			return false;
		}
		reader.endObject();
	}
	else {
		reader.endObject();
		CCASSERT(false, "Failed to get level size");
		return false;
	}

	// Set the shadow position value
	if (reader.startObject(SHADOW_POSITION_FIELD)) {
		_metadata._playerPos.position.x = reader.getNumber(X_FIELD, -1.0f);
		if (_metadata._playerPos.position.x < 0.0f ||
			_metadata._playerPos.position.x > _metadata._size.width) {
			CCASSERT(false, "Failed to assign player position.x");
			return false;
		}
		_metadata._playerPos.position.y = reader.getNumber(Y_FIELD, -1.0f);
		if (_metadata._playerPos.position.y < 0.0f ||
			_metadata._playerPos.position.y > _metadata._size.height) {
			CCASSERT(false, "Failed to assign player position.y");
			return false;
		}
		reader.endObject();
	}
	else {
		reader.endObject();
		CCASSERT(false, "Failed to get player position");
		return false;
	}

	// Set the caster position and heading values
	if (reader.startObject(CASTER_POSITION_FIELD)) {
		_metadata._casterPos.position.x = reader.getNumber(X_FIELD, -1.0f);
		if (_metadata._casterPos.position.x < 0.0f ||
			_metadata._casterPos.position.x > _metadata._size.width) {
			CCASSERT(false, "Failed to assign caster position.x");
			return false;
		}
		_metadata._casterPos.position.y = reader.getNumber(Y_FIELD, -1.0f);
		if (_metadata._casterPos.position.y < 0.0f ||
			_metadata._casterPos.position.y > _metadata._size.height) {
			CCASSERT(false, "Failed to assign caster position.y");
			return false;
		}
		_metadata._casterPos.heading = reader.getNumber(HEADING_FIELD, -1.0f);
		if (_metadata._casterPos.heading < 0.0f || _metadata._casterPos.heading >= 360.0f) {
			CCASSERT(false, "Failed to assign caster heading");
			return false;
		}
		reader.endObject();
	}
	else {
		reader.endObject();
		CCASSERT(false, "Failed to get caster position");
		return false;
	}

	// Set the metadata for static objects
	if (reader.startObject(STATIC_OBJECTS_FIELD)) {
		int staticObjectCount = reader.startArray();
		for (int staticObjectIndex = 0; staticObjectIndex < staticObjectCount; staticObjectIndex++) {
			if (reader.startObject()) {
				StaticObjectMetadata data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _metadata._size.width) {
					CCASSERT(false, ("Failed to assign static object " + std::to_string(staticObjectIndex + 1) + " position.x").c_str());
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.y > _metadata._size.height) {
					CCASSERT(false, ("Failed to assign static object " + std::to_string(staticObjectIndex + 1) + " position.y").c_str());
					return false;
				}
				try {
					data.type = staticObjectMap.at(reader.getString(TYPE_FIELD));
				}
				catch (out_of_range) {
					CCASSERT(false, ("Failed to assign static object " + std::to_string(staticObjectIndex + 1) + " type").c_str());
					return false;
				}
				_metadata._staticObjects.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				CCASSERT(false, ("Failed to get static object " + std::to_string(staticObjectIndex + 1)).c_str());
			}
		}
		reader.endArray();
		reader.endObject();
	}
	else {
		reader.endObject();
		CCASSERT(false, "Failed to get static objects");
		return false;
	}

	// Set the metadata for pedestrians, including initial actions
	if (reader.startObject(PEDESTRIANS_FIELD)) {
		int pedestrianCount = reader.startArray();
		for (int pedestrianIndex = 0; pedestrianIndex < pedestrianCount; pedestrianIndex++) {
			if (reader.startObject()) {
				PedestrianMetadata data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _metadata._size.width) {
					CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " position.x").c_str());
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.y > _metadata._size.height) {
					CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " position.y").c_str());
					return false;
				}
				data.heading = reader.getNumber(HEADING_FIELD, -1.0f);
				if (data.heading < 0.0f || data.heading >= 360.0f) {
					CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " heading").c_str());
					return false;
				}
				if (reader.startObject(ACTIONS_FIELD)) {
					int actionCount = reader.startArray();
					for (int actionIndex = 0; actionIndex < actionCount; actionIndex++) {
						if (reader.startObject()) {
							Pedestrian::ActionType type;
							try {
								type = Pedestrian::actionMap.at(reader.getString(TYPE_FIELD));
							}
							catch (out_of_range) {
								CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " type").c_str());
								return false;
							}
							int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
							if (length <= 0) {
								CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " length").c_str());
								return false;
							}
							// If there is no counter, it is equal to length
							int counter = (int)reader.getNumber(COUNTER_FIELD, (float) length);
							if (counter <= 0 || counter > length) {
								CCASSERT(false, ("Invalid counter value for pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " counter").c_str());
								return false;
							}
							data.actions.push(type, length, counter);
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1)).c_str());
						}
					}
					reader.endArray();
					reader.endObject();
				}
				else {
					CCASSERT(false, ("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " actions").c_str());
					return false;
				}
				int cyclic = (int)reader.getNumber(CYCLIC_FIELD, -1.0f);
				if (cyclic == 0 || cyclic == 1) {
					data.actions.setCycling(cyclic != 0);
				}
				else {
					if (DEFAULT_CYCLING_VALUE) {
						CCLOG("No cyclic field for pedestrian %i, setting to cyclic", pedestrianIndex + 1);
					}
					else {
						CCLOG("No cyclic field for pedestrian %i, setting to non-cyclic", pedestrianIndex + 1);
					}
					data.actions.setCycling(DEFAULT_CYCLING_VALUE);
				}
				_metadata._pedestrians.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				CCASSERT(false, ("Failed to get pedestrian " + std::to_string(pedestrianIndex + 1)).c_str());
			}
		}
		reader.endArray();
		reader.endObject();
	}
	else {
		reader.endObject();
		CCASSERT(false, "Failed to get pedestrians");
		return false;
	}

	// Set the metadata for cars, including initial actions
	if (reader.startObject(CARS_FIELD)) {
		int carCount = reader.startArray();
		for (int carIndex = 0; carIndex < carCount; carIndex++) {
			if (reader.startObject()) {
				CarMetadata data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _metadata._size.width) {
					CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " position.x").c_str());
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.x > _metadata._size.height) {
					CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " position.y").c_str());
					return false;
				}
				data.heading = reader.getNumber(HEADING_FIELD, -1.0f);
				if (data.heading < 0.0f || data.heading >= 360.0f) {
					CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " heading").c_str());
					return false;
				}
				if (reader.startObject(ACTIONS_FIELD)) {
					int actionCount = reader.startArray();
					for (int actionIndex = 0; actionIndex < actionCount; actionIndex++) {
						if (reader.startObject()) {
							Car::ActionType type;
							try {
								type = Car::actionMap.at(reader.getString(TYPE_FIELD));
							}
							catch (out_of_range) {
								CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " type").c_str());
								return false;
							}
							int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
							if (length <= 0) {
								CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " length").c_str());
								return false;
							}
							// If there is no counter, it is equal to length
							int counter = (int)reader.getNumber(COUNTER_FIELD, (float)length);
							if (counter <= 0 || counter > length) {
								CCASSERT(false, ("Invalid counter value for car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " counter").c_str());
								return false;
							}
							data.actions.push(type, length, counter);
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1)).c_str());
						}
					}
					reader.endArray();
					reader.endObject();
				}
				else {
					CCASSERT(false, ("Failed to assign car " + std::to_string(carIndex + 1) + " actions").c_str());
					return false;
				}
				int cyclic = (int)reader.getNumber(CYCLIC_FIELD, -1.0f);
				if (cyclic == 0 || cyclic == 1) {
					data.actions.setCycling(cyclic != 0);
				}
				else {
					if (DEFAULT_CYCLING_VALUE) {
						CCLOG("No cyclic field for car %i, setting to cyclic", carIndex + 1);
					}
					else {
						CCLOG("No cyclic field for car %i, setting to non-cyclic", carIndex + 1);
					}
					data.actions.setCycling(DEFAULT_CYCLING_VALUE);
				}
				_metadata._cars.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				CCASSERT(false, ("Failed to get car " + std::to_string(carIndex + 1)).c_str());
			}
		}
		reader.endArray();
		reader.endObject();
	}
	else {
		reader.endObject();
		CCASSERT(false, "Failed to get cars");
		return false;
	}
	
	// Load succeeded
	return true;
}

bool LevelInstance::populateLevel() {

	// Initialize the main character
	_metadata._playerPos.object = Shadow::create(_metadata._playerPos.position, _scale * DUDE_SCALE, &_characterFilter, &_characterSensorFilter);
	_metadata._playerPos.object->setDrawScale(_scale);
	sprite = PolygonNode::create();
	sprite->setScale(cscale / DUDE_SCALE);
	_metadata._playerPos.object->setSceneNode(sprite);
	draw = WireNode::create();
	draw->setColor(DEBUG_COLOR);
	draw->setOpacity(DEBUG_OPACITY);
	_metadata._playerPos.object->setDebugNode(draw);

	// Initialize the caster
	_metadata._casterPos.object = Shadow::create(_metadata._playerPos.position, _scale * DUDE_SCALE, &_characterFilter, &_characterSensorFilter);
	_metadata._casterPos.object->setDrawScale(_scale);
	sprite = PolygonNode::create();
	sprite->setScale(cscale / DUDE_SCALE);
	_metadata._casterPos.object->setSceneNode(sprite);
	draw = WireNode::create();
	draw->setColor(DEBUG_COLOR);
	draw->setOpacity(DEBUG_OPACITY);
	_metadata._casterPos.object->setDebugNode(draw);
	
}


// TODO write all the other methods