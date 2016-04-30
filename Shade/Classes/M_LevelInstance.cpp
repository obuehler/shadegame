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
#define SIZE_FIELD "pixelSize"
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

LevelInstance::LevelInstance(void) : Asset() {}

LevelInstance::~LevelInstance(void) { unload(); }

/*const map<string, LevelInstance::StaticObjectType> LevelInstance::staticObjectMap = {
{ "building1", StaticObjectType::BUILDING_1 }
// TODO add more static object types
}; */

/**
* Creates a new game level with no source file.
*
* The source file can be set at any time via the setFile() method. This method
* does NOT load the asset.  You must call the load() method to do that.
*
* @return  an autoreleased level file
*/
LevelInstance* LevelInstance::create() {
	LevelInstance* level = new (std::nothrow) LevelInstance();
	if (level && level->init()) {
		level->autorelease();
		return level;
	}
	CC_SAFE_DELETE(level);
	return nullptr;

}

/**
* Creates a new game level with the given source file.
*
* This method does NOT load the level. You must call the load() method to do that.
* This method returns false if file does not exist.
*
* @return  an autoreleased level file
*/
LevelInstance* LevelInstance::create(std::string file) {
	LevelInstance* level = new (nothrow) LevelInstance();
	if (level && level->init(file)) {
		level->autorelease();
		return level;
	}
	CC_SAFE_DELETE(level);
	return nullptr;
}


/**
* Initializes the metadata using a JSON reader. It does not create the
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
		failToLoad("Failed to load level file");
		return false;
	}

	// Set level index.
	_levelIndex = (int)reader.getNumber(LEVEL_INDEX_FIELD, -1.0f);
	if (_levelIndex < 0) {
		failToLoad("Failed to assign level index");
		return false;
	}

	// Set the level width and height values
	if (reader.startObject(SIZE_FIELD)) {
		_size.width = reader.getNumber(WIDTH_FIELD) / BOX2D_SCALE;
		if (_size.width <= 0.0f) {
			failToLoad("Failed to assign level width");
			return false;
		}
		_size.height = reader.getNumber(HEIGHT_FIELD) / BOX2D_SCALE;
		if (_size.height <= 0.0f) {
			failToLoad("Failed to assign level height");
			return false;
		}
		reader.endObject();

	}
	else {
		reader.endObject();
		failToLoad("Failed to get level size");
		return false;
	}

	// Set the shadow position value
	if (reader.startObject(SHADOW_POSITION_FIELD)) {
		_playerPos.position.x = reader.getNumber(X_FIELD, -1.0f);
		if (_playerPos.position.x < 0.0f ||
			_playerPos.position.x > _size.width) {
			failToLoad("Failed to assign player position.x");
			return false;
		}
		_playerPos.position.y = reader.getNumber(Y_FIELD, -1.0f);
		if (_playerPos.position.y < 0.0f ||
			_playerPos.position.y > _size.height) {
			failToLoad("Failed to assign player position.y");
			return false;
		}
		reader.endObject();
	}
	else {
		reader.endObject();
		failToLoad("Failed to get player position");
		return false;
	}

	// Set the caster position and heading values
	if (reader.startObject(CASTER_POSITION_FIELD)) {
		_casterPos.position.x = reader.getNumber(X_FIELD, -1.0f);
		if (_casterPos.position.x < 0.0f ||
			_casterPos.position.x > _size.width) {
			failToLoad("Failed to assign caster position.x");
			return false;
		}
		_casterPos.position.y = reader.getNumber(Y_FIELD, -1.0f);
		if (_casterPos.position.y < 0.0f ||
			_casterPos.position.y > _size.height) {
			failToLoad("Failed to assign caster position.y");
			return false;
		}
		_casterPos.heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
		if (_casterPos.heading < -1.0f) {
			failToLoad("Failed to assign caster heading");
			return false;
		}
		reader.endObject();
	}
	else {
		reader.endObject();
		failToLoad("Failed to get caster position");
		return false;
	}

	// Set the metadata for static objects
	if (reader.isArray(STATIC_OBJECTS_FIELD)) {
		int staticObjectCount = reader.startArray(STATIC_OBJECTS_FIELD);
		for (int staticObjectIndex = 0; staticObjectIndex < staticObjectCount; staticObjectIndex++) {
			if (reader.startObject()) {
				StaticObjectMetadata data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _size.width) {
					failToLoad("Failed to assign static object " + std::to_string(staticObjectIndex + 1) + " position.x");
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.y > _size.height) {
					failToLoad("Failed to assign static object " + std::to_string(staticObjectIndex + 1) + " position.y");
					return false;
				}
				if (reader.startObject(TYPE_FIELD)) {
					data.type = reader.getString("name");
					reader.endObject();
				}
				else {
					reader.endObject();
					failToLoad("Failed to assign static object " + std::to_string(staticObjectIndex + 1) + " type");
					return false;
				}
				_staticObjects.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				failToLoad("Failed to get static object " + std::to_string(staticObjectIndex + 1));
			}
		}
		reader.endArray();
	}
	else {
		reader.endObject();
		failToLoad("Failed to get static objects");
		return false;
	}

	// Set the metadata for pedestrians, including initial actions
	if (reader.isArray(PEDESTRIANS_FIELD)) {
		int pedestrianCount = reader.startArray(PEDESTRIANS_FIELD);
		for (int pedestrianIndex = 0; pedestrianIndex < pedestrianCount; pedestrianIndex++) {
			if (reader.startObject()) {
				PedestrianMetadata data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _size.width) {
					failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " position.x");
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.x > _size.height) {
					failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " position.y");
					return false;
				}
				data.heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
				if (data.heading < -1.0f) {
					failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " heading");
					return false;
				}
				int actionStartIndex = 0;
				bool queueIsCyclic = false;
				// Initialize actions with empty action queue
				data.actions = ActionQueue<Pedestrian>::create();
				data.actions->retain();
				if (reader.isArray(ACTIONS_FIELD)) {
					int actionCoun = reader.startArray(ACTIONS_FIELD);
					for (int actionInde = 0; actionInde < actionCoun; actionInde++) {
						if (reader.startObject()) {
							bool cyclic = reader.getBool(CYCLIC_FIELD, true);
							bool cyclic2 = reader.getBool(CYCLIC_FIELD, false);
							if (cyclic != cyclic2) {
								failToLoad("Failed to fetch cyclic value for pedestrian " + std::to_string(pedestrianIndex + 1));
								return false;
							}
							if (cyclic2) {
								actionStartIndex = actionInde;
								queueIsCyclic = true;
							}
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionInde + 1));
						}
					}
					reader.endArray();
				}
				else {
					failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " actions");
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
							Pedestrian::ActionType type;
							try {
								type = Pedestrian::actionMap.at(reader.getString(TYPE_FIELD));
							}
							catch (out_of_range) {
								failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " type");
								return false;
							}
							int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
							if (length <= 0) {
								failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " length");
								return false;
							}
							float heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
							if (heading < -1.0f) {
								failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " heading");
								return false;
							}
							// If there is no counter, it is equal to length
							int counter = (int)reader.getNumber(COUNTER_FIELD, (float)length);
							if (counter <= 0 || counter > length) {
								failToLoad("Invalid counter value for pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1) + " counter");
								return false;
							}
							data.actions->push(type, length, counter, heading);
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(actionIndex + 1));
						}
					}
					reader.endArray();
					data.actions->setCycling(queueIsCyclic);
					if (queueIsCyclic) {
						ActionQueue<Pedestrian> additionalQueue;
						reader.startArray(ACTIONS_FIELD);
						for (int count = 0; count < actionStartIndex; count++) {
							reader.startObject();
							Pedestrian::ActionType type;
							try {
								type = Pedestrian::actionMap.at(reader.getString(TYPE_FIELD));
							}
							catch (out_of_range) {
								failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(count + 1) + " type");
								return false;
							}
							int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
							if (length <= 0) {
								failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(count + 1) + " length");
								return false;
							}
							float heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
							if (heading < -1.0f) {
								failToLoad("Failed to assign pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(count + 1) + " heading");
								return false;
							}
							// If there is no counter, it is equal to length
							int counter = (int)reader.getNumber(COUNTER_FIELD, (float)length);
							if (counter <= 0 || counter > length) {
								failToLoad("Invalid counter value for pedestrian " + std::to_string(pedestrianIndex + 1) + " action " + std::to_string(count + 1) + " counter");
								return false;
							}
							additionalQueue.push(type, length, counter, heading);
							reader.endObject();
							reader.advance();
						}
						reader.endArray();
						data.actions->force(additionalQueue, true);
					}
				}
				_pedestrians.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				failToLoad("Failed to get pedestrian " + std::to_string(pedestrianIndex + 1));
			}
		}
		reader.endArray();
	}
	else {
		failToLoad("Failed to get pedestrians");
		return false;
	}

	// Set the metadata for cars, including initial actions
	if (reader.isArray(CARS_FIELD)) {
		int carCount = reader.startArray(CARS_FIELD);
		for (int carIndex = 0; carIndex < carCount; carIndex++) {
			if (reader.startObject()) {
				CarMetadata data;
				data.position.x = reader.getNumber(X_FIELD, -1.0f);
				if (data.position.x < 0.0f
					|| data.position.x > _size.width) {
					failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " position.x");
					return false;
				}
				data.position.y = reader.getNumber(Y_FIELD, -1.0f);
				if (data.position.y < 0.0f
					|| data.position.x > _size.height) {
					failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " position.y");
					return false;
				}
				data.heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
				if (data.heading < -1.0f) {
					failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " heading");
					return false;
				}
				int actionStartIndex = 0;
				bool queueIsCyclic = false;
				// Initialize actions with empty action queue
				data.actions = ActionQueue<Car>::create();
				data.actions->retain();
				if (reader.isArray(ACTIONS_FIELD)) {
					int actionCoun = reader.startArray(ACTIONS_FIELD);
					for (int actionInde = 0; actionInde < actionCoun; actionInde++) {
						if (reader.startObject()) {
							bool cyclic = reader.getBool(CYCLIC_FIELD, true);
							bool cyclic2 = reader.getBool(CYCLIC_FIELD, false);
							if (cyclic != cyclic2) {
								failToLoad("Failed to fetch cyclic value for car " + std::to_string(carIndex + 1));
								return false;
							}
							if (cyclic2) {
								actionStartIndex = actionInde;
								queueIsCyclic = true;
							}
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionInde + 1));
						}
					}
					reader.endArray();
				}
				else {
					failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " actions");
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
							Car::ActionType type;
							try {
								type = Car::actionMap.at(reader.getString(TYPE_FIELD));
							}
							catch (out_of_range) {
								failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " type");
								return false;
							}
							int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
							if (length <= 0) {
								failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " length");
								return false;
							}
							float heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
							if (heading < -1.0f) {
								failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " heading");
								return false;
							}
							// If there is no counter, it is equal to length
							int counter = (int)reader.getNumber(COUNTER_FIELD, (float)length);
							if (counter <= 0 || counter > length) {
								failToLoad("Invalid counter value for car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1) + " counter");
								return false;
							}
							data.actions->push(type, length, counter, heading);
							reader.endObject();
							reader.advance();
						}
						else {
							reader.endObject();
							reader.endArray();
							failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(actionIndex + 1));
						}
					}
					reader.endArray();
					data.actions->setCycling(queueIsCyclic);
					if (queueIsCyclic) {
						ActionQueue<Car> additionalQueue;
						reader.startArray(ACTIONS_FIELD);
						for (int count = 0; count < actionStartIndex; count++) {
							reader.startObject();
							Car::ActionType type;
							try {
								type = Car::actionMap.at(reader.getString(TYPE_FIELD));
							}
							catch (out_of_range) {
								failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(count + 1) + " type");
								return false;
							}
							int length = (int)reader.getNumber(LENGTH_FIELD, -1.0f);
							if (length <= 0) {
								failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(count + 1) + " length");
								return false;
							}
							float heading = CC_DEGREES_TO_RADIANS(reader.getNumber(HEADING_FIELD, -90.0f));
							if (heading < -1.0f) {
								failToLoad("Failed to assign car " + std::to_string(carIndex + 1) + " action " + std::to_string(count + 1) + " heading");
								return false;
							}
							// If there is no counter, it is equal to length
							int counter = (int)reader.getNumber(COUNTER_FIELD, (float)length);
							if (counter <= 0 || counter > length) {
								failToLoad("Invalid counter value for car " + std::to_string(carIndex + 1) + " action " + std::to_string(count + 1) + " counter");
								return false;
							}
							additionalQueue.push(type, length, counter, heading);
							reader.endObject();
							reader.advance();
						}
						reader.endArray();
						data.actions->force(additionalQueue, true);
					}
				}
				data.actions->retain();
				_cars.push_back(data);
				reader.endObject();
				reader.advance();
			}
			else {
				reader.endObject();
				reader.endArray();
				failToLoad("Failed to get car " + std::to_string(carIndex + 1));
			}
		}
		reader.endArray();
	}
	else {
		failToLoad("Failed to get cars");
		return false;
	}

	// Load succeeded
	return true;
}

void LevelInstance::failToLoad(const char* errorMessage) {
	for (PedestrianMetadata pData : _pedestrians) {
		if (pData.actions != nullptr) {
			pData.actions->release();
			pData.actions = nullptr;
		}
	}
	for (CarMetadata cData : _cars) {
		if (cData.actions != nullptr) {
			cData.actions->release();
			cData.actions = nullptr;
		}
	}
	CCASSERT(false, errorMessage);
}

/**
* Populates the level using the metadata obtained from the JSON file. Retains
* a reference to every object created until they are added to the physics
* world.
*
* @param	reset	Whether the level is being reset
*
* @retain	every object created
*/
void LevelInstance::populateLevel(bool reset) {

	// We need to know the content scale for resolution independence
	// If the device is higher resolution than 1024x576, Cocos2d will scale it
	// This was set as the design resolution in AppDelegate
	// To convert from design resolution to real, divide positions by cscale
	float cscale = Director::getInstance()->getContentScaleFactor();

	/* Local pointers to hold newly created sprites before they get assigned
	* as the scene nodes to obstacles */
	PolygonNode* sprite;

	// Initialize the main character
	_playerPos.object = Shadow::create(); // Initialize in GameController
	//_playerPos.object = Shadow::create(_playerPos.position, _scale * DUDE_SCALE, &characterFilter, &characterSensorFilter);
	_playerPos.object->retain();
	sprite = PolygonNode::create();
	sprite->setScale(cscale / DUDE_SCALE);
	_playerPos.object->setSceneNode(sprite);

	// Initialize the caster

	sprite = PolygonNode::create();
	sprite->setScale(cscale / DUDE_SCALE);
	auto* casterObject = BoxObstacle::create();
	//auto* casterObject = BoxObstacle::create(_casterPos.position, Size::ZERO, &casterFilter);
	casterObject->setBodyType(b2_dynamicBody);
	casterObject->setDensity(BASIC_DENSITY);
	casterObject->setFriction(BASIC_FRICTION);
	casterObject->setRestitution(BASIC_RESTITUTION);
	casterObject->setSensor(true);
	casterObject->setSceneNode(sprite);

	// The caster is initialized with an empty action queue, actions will be added by AIController
	// The following line implicitly retains casterObject
	_casterPos.object = OurMovingObject<Caster>::create(ActionQueue<Caster>::create(), casterObject, nullptr);
	_casterPos.object->retain();

	if (!reset) {
		// Initialize the static objects
		for (StaticObjectMetadata &data : _staticObjects) {
			//data.object = BoxObstacle::create(data.position, Size::ZERO, &objectFilter);
			data.object = BoxObstacle::create();
			data.object->setBodyType(b2_staticBody);
			data.object->setDensity(BASIC_DENSITY);
			data.object->setFriction(BASIC_FRICTION);
			data.object->setRestitution(BASIC_RESTITUTION);
			sprite = PolygonNode::create();
			sprite->setScale(cscale);
			data.object->setSceneNode(sprite);
			data.object->retain();

			//data.shadow = BoxObstacle::create(data.position, Size::ZERO, &objectFilter);
			data.shadow = BoxObstacle::create();	
			data.shadow->setBodyType(b2_dynamicBody);
			data.shadow->setDensity(0);
			data.shadow->setFriction(0);
			data.shadow->setRestitution(0);
			sprite = PolygonNode::create();
			sprite->setScale(cscale);
			data.shadow->setSceneNode(sprite);
			data.shadow->retain();

		}
	}



	// Initialize the pedestrians
	for (PedestrianMetadata &data : _pedestrians) {
		sprite = PolygonNode::create();
		sprite->setScale(cscale / DUDE_SCALE);
		//auto* pedestrianShadow = BoxObstacle::create(data.position, Size::ZERO, &shadowFilter);
		auto* pedestrianShadow = BoxObstacle::create();
		pedestrianShadow->setBodyType(b2_dynamicBody);
		pedestrianShadow->setDensity(0);
		pedestrianShadow->setFriction(0);
		pedestrianShadow->setRestitution(0);
		pedestrianShadow->setSensor(true);
		pedestrianShadow->setSceneNode(sprite);

		sprite = PolygonNode::create();
		sprite->setScale(cscale / DUDE_SCALE);
		//auto* pedestrianObject = BoxObstacle::create(data.position, Size::ZERO, &objectFilter);
		auto* pedestrianObject = BoxObstacle::create();
		pedestrianObject->setBodyType(b2_dynamicBody);
		pedestrianObject->setDensity(BASIC_DENSITY);
		pedestrianObject->setFriction(BASIC_FRICTION);
		pedestrianObject->setRestitution(BASIC_RESTITUTION);
		pedestrianObject->setSensor(true);
		pedestrianObject->setSceneNode(sprite);

		// The following line implicitly retains pedestrianObject and pedestrianShadow
		// It also creates a copy of the initial action queue so that the initial
		// action queue is preserved.
		data.object = OurMovingObject<Pedestrian>::create(
			ActionQueue<Pedestrian>::create(*(data.actions)),
			pedestrianObject, pedestrianShadow);
		data.object->retain();

	}

	// Initialize the cars
	for (CarMetadata &data : _cars) {
		sprite = PolygonNode::create();
		sprite->setScale(cscale / DUDE_SCALE);
		//auto* carShadow = BoxObstacle::create(data.position, Size::ZERO, &shadowFilter);
		auto* carShadow = BoxObstacle::create();
		carShadow->setBodyType(b2_dynamicBody);
		carShadow->setDensity(0);
		carShadow->setFriction(0);
		carShadow->setRestitution(0);
		carShadow->setSensor(true);
		carShadow->setSceneNode(sprite);

		sprite = PolygonNode::create();
		sprite->setScale(cscale / DUDE_SCALE);
		//auto* carObject = BoxObstacle::create(data.position, Size::ZERO, &objectFilter);
		auto* carObject = BoxObstacle::create();
		carObject->setBodyType(b2_dynamicBody);
		carObject->setDensity(BASIC_DENSITY);
		carObject->setFriction(BASIC_FRICTION);
		carObject->setRestitution(BASIC_RESTITUTION);
		carObject->setSensor(true);
		carObject->setSceneNode(sprite);

		// The following line implicitly retains carObject and carShadow
		data.object = OurMovingObject<Car>::create(
			ActionQueue<Car>::create(*(data.actions)),
			carObject, carShadow);
		data.object->retain();

	}
	sprite = nullptr;
}

bool LevelInstance::load() {
	if (initializeMetadata()) {
		populateLevel(false);
		return true;
	}
	return false;
}


void LevelInstance::unload() {
	for (PedestrianMetadata p : _pedestrians) {
		p.actions->release();
	}
	for (CarMetadata c : _cars) {
		c.actions->release();
	}
}