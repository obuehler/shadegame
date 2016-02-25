//
//  CUJSONReader.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a simple reader to process JSON files.  It is built upon the JSON parser
//  provided with the Spine editor (built-into Cocos2d).  It provides a DOM-type interface for
//  parsing the data in a structured way.
//
//  Most of the API is straight-forward except for objects and arrays.  Those must be "unwrapped"
//  in order to access them.  You unwrap with a startObject or a startArray method.  Those methods
//  have returns values that indicate whether or not they were successful on the given node.
//  However, regardless of whether or not the method succeed, you must wrap everything back up
//  with an endObject or endArray method to proceed.
//
//  Author: Walker White
//  Version: 1/19/16
//
#include "CUJSONReader.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new JSONReader.
 *
 * This constructor does not load any JSON, as there is no associate file. You can associate
 * a file later with the setFile() method.
 *
 * @returns an autoreleased JSONReader object
 */
JSONReader* JSONReader::create() {
    JSONReader *reader = new (std::nothrow) JSONReader();
    if (reader && reader->init()) {
        reader->autorelease();
        return reader;
    }
    CC_SAFE_DELETE(reader);
    return nullptr;
}

/**
 * Creates a new JSONReader.
 *
 * This constructor does load the JSON file; it only stores the file name. You start parsing
 * with the startJSON() method.
 *
 * @param  file the file to read from
 *
 * @returns an autoreleased JSONReader object
 */
JSONReader* JSONReader::create(std::string file) {
    JSONReader *reader = new (std::nothrow) JSONReader();
    if (reader && reader->init(file)) {
        reader->autorelease();
        return reader;
    }
    CC_SAFE_DELETE(reader);
    return nullptr;
}

/**
 * Initializes a new JSONReader.
 *
 * This initializer does not load any JSON, as there is no associate file. You can associate
 * a file later with the setFile() method.
 *
 * @return  true if the reader is initialized properly, false otherwise.
 */
bool JSONReader::init() {
    _file = "";
    return true;
}

/**
 * Initializes a new JSONReader.
 *
 * This initializer does load the JSON file; it only stores the file name. You start parsing
 * with the startJSON() method.
 *
 * @param  file the file to read from
 *
 * @return  true if the reader is initialized properly, false otherwise.
 */
bool JSONReader::init(std::string file) {
    _file = FileUtils::getInstance()->fullPathForFilename(file);
    return _file != "";
}

/**
 * Starts a JSON parser for the current file.
 *
 * The parser will fail if the JSON is not well-formed.  In that case, the method will return false.
 * There are (unfortunately) no error messages for why parsing failed.
 *
 * @return true if the file is a well-formed JSON file.
 */
bool JSONReader::startJSON() {
    CCASSERT(!_root, "JSON is already in progress");

    std::string contents = FileUtils::getInstance()->getStringFromFile(_file);
    _root = Json_create(contents.c_str());
    _json = _root;
    
    return _json != nullptr;
}

/**
 * Starts a JSON parser for the given JSON string.
 *
 * This method will ignore the associated file, and parse the provided string instead.  The string
 * should be an JSON string, not a file containing a JSON string.
 *
 * The parser will fail if the JSON is not well-formed.  In that case, the method will return false.
 * There are (unfortunately) no error messages for why parsing failed.
 *
 * @param  source   the JSON source string
 *
 * @return true if the file is a well-formed JSON file.
 */
bool JSONReader::startJSON(std::string source) {
    CCASSERT(!_root, "JSON is already in progress");
    
    _root = Json_create(source.c_str());
    _json = _root;
    
    return _json != nullptr;
}

/**
 * Ends the current JSON parsing session, erasing the DOM tree.
 *
 * Once this method is called of the JSON methods will fail until startJSON() -- either version --
 * is called again.
 */
void JSONReader::endJSON() {
    if (_root != nullptr) {
        Json_dispose(_root);
        _root = nullptr;
    }
    reset();
}


#pragma mark -
#pragma mark Type Checking
/**
 * Returns true if there is an entry for the given key.
 *
 * @param  key  the field name to query
 *
 * @return true if there is an entry for the given key.
 */
bool JSONReader::exists(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr;
}

/**
 * Returns true if the entry for key exists and has a nullptr value.
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and has a nullptr value.
 */
bool JSONReader::isNull(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr && j->type == Json_NULL;
}

/**
 * Returns true if the entry for key exists and represents an object.
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents an object.
 */
bool JSONReader::isObject(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr && (j->type == Json_Object || j->type == Json_NULL);
}

/**
 * Returns true if the entry for key exists and represents an array.
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents an array.
 */
bool JSONReader::isArray(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr && (j->type == Json_Array || j->type == Json_NULL);
}

/**
 * Returns true if the entry for key exists and represents a boolean value
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents a boolean value
 */
bool JSONReader::isBool(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr && (j->type == Json_True || j->type == Json_False);
}

/**
 * Returns true if the entry for key exists and represents a number
 *
 * All numbers in JSON files are treated as floats.
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents a number
 */
bool JSONReader::isNumber(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr && j->type == Json_Number;
}

/**
 * Returns true if the entry for key exists and represents a string
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents a string
 */
bool JSONReader::isString(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    return j != nullptr && j->type == Json_String;
}

/**
 * Returns true if the entry for key exists and represents a Vec2 value
 *
 * In a JSON file, a Vec2 is just a two-element array, whose elements are both numbers.
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents a Vec2 value
 */
bool JSONReader::isVec2(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr || j->type != Json_Array || j->size != 2) {
        return false;
    }
    Json* child = j->child;
    return (child->type == Json_Number && child->next->type == Json_Number);
}

/**
 * Returns true if the entry for key exists and represents an array of floats
 *
 * This method is just an additional type-check on top of isArray.
 *
 * @param  key  the field name to query
 *
 * @return true if the entry for key exists and represents an array of floats
 */
bool JSONReader::isFloatArray(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr || j->type != Json_Array) {
        return false;
    }
    Json* child = j->child;
    for(int ii = 0; ii < j->size; ii++) {
        if (child->type != Json_Number) {
            return false;
        }
        child = child->next;
    }
    return true;
}

/**
 * Returns true if the current cursor position represents a Vec2 value
 *
 * In a JSON file, a Vec2 is just a two-element array, whose elements are both numbers.
 *
 * @return true if the current cursor position represents a Vec2 value
 */
bool JSONReader::isVec2() const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    if (_json == nullptr || _json->type != Json_Array || _json->size != 2) {
        return false;
    }
    Json* child = _json->child;
    return (child->type == Json_Number && child->next->type == Json_Number);

}

/**
 * Returns true if the current cursor position represents an array of floats
 *
 * This method is just an additional type-check on top of isArray.
 *
 * @return true if the current cursor position represents an array of floats
 */
bool JSONReader::isFloatArray() const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    if (_json == nullptr || _json->type != Json_Array) {
        return false;
    }
    Json* child = _json->child;
    for(int ii = 0; ii < _json->size; ii++) {
        if (child->type != Json_Number) {
            return false;
        }
        child = child->next;
    }
    return true;
}


#pragma mark -
#pragma mark Primitives
/**
 * Returns the boolean value for the given key.
 *
 * If the key does not exist, or does not refer to a boolean, this method returns
 * False as a default.
 *
 * @param  key  the field name to query
 *
 * @return the boolean value for the given key.
 */
bool JSONReader::getBool(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr) {
        return false;
    }
    // Return the actual value
    CCASSERT(j->type == Json_True || j->type == Json_False, "Value is not a boolean");
    return j->type == Json_True;
}

/**
 * Returns the number for the given key.
 *
 * If the key does not exist, or does not refer to a number, this method returns
 * 0.0f as a default. All numbers in JSON files are treated as floats.
 *
 * @param  key  the field name to query
 *
 * @return the number for the given key.
 */
float JSONReader::getNumber(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr) {
        return 0.0f;
    }
    
    // Return the actual value
    CCASSERT(j->type == Json_Number, "Value is not a number");
    return j->valueFloat;
}

/**
 * Returns the string for the given key.
 *
 * If the key does not exist, or does not refer to a string, this method returns
 * "" as a default.
 *
 * @param  key  the field name to query
 *
 * @return the string for the given key.
 */
std::string JSONReader::getString(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr) {
        return "";
    }
    
    // Return the actual value
    CCASSERT(j->type == Json_String, "Value is not a string");
    return std::string(j->valueString);
}

/**
 * Returns the Vec2 value for the current cursor position.
 *
 * If the current position is not a Vec2, it returns Vec2::ZERO as a default.
 *
 * @return the Vec2 value for the current cursor position.
 */
Vec2 JSONReader::getVec2() const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    Json* child = _json->child;
    if (child == nullptr || child->next == nullptr) {
        return Vec2::ZERO;
    }
    float x = child->valueFloat;
    float y = child->next->valueFloat;
    return Vec2(x,y);
}

/**
 * Fills the buffer with the contents of the cursor position
 *
 * This method assumes that the current cursor position is an array of floats
 * (e.g. the children of the cursor are all float nodes), and writes these
 * floats to the given buffer.  The method returns the number of elements
 * written. If the cursor is not a valid float array, nothing will be written.
 *
 * The buffer must be sized large enough to recieve the float array.  Use the
 * method getSize() to determine the size of the array.
 *
 * @param  buffer   the buffer to store the float values
 *
 * @return number of elements written to the buffer
 */
int JSONReader::getFloatArray(float* buffer) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    int size = _json->size;
    Json* child = _json->child;
    if (child == nullptr || _json->type != Json_Array) {
        return 0;
    }

    // First pass to check array of floats assumption
    for(int ii = 0; ii < size; ii++) {
        if (child->type != Json_Number) {
            return 0;
        }
        child = child->next;
    }
    // Reset child pointer and write to buffer
    child = _json->child;
    for(int ii = 0; ii < size; ii++) {
        buffer[ii] = child->valueFloat;
        child = child->next;
    }
    return size;
}

/**
 * Returns the Vec2 value for the given key.
 *
 * If the key does not exist, or does not refer to a Vec2 value, this method
 * returns Vec2::ZERO as a default.
 *
 * @param  key  the field name to query
 *
 * @return the Vec2 value for the given key.
 */
Vec2 JSONReader::getVec2(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr || j->type != Json_Array || j->size != 2) {
        return Vec2::ZERO;
    }
    Json* child = j->child;
    float x = child->valueFloat;
    float y = child->next->valueFloat;
    return Vec2(x,y);
}

/**
 * Fills the buffer with the contents of the given key.
 *
 * This method assumes that the key refers to an array of floats (e.g. the children
 * of the key are all float nodes), and writes these floats to the given buffer.
 * The method returns the number of elements written. If the key does not exist, or
 * does not refer to a valid float array, nothing will be written.
 *
 * The buffer must be sized large enough to recieve the float array.  Use the
 * method getSize() to determine the size of the array.
 *
 * @param  buffer   the buffer to store the float values
 *
 * @param  key  the field name to query
 *
 * @return number of elements written to the buffer
 */
int  JSONReader::getFloatArray(std::string key, float* buffer) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* j = Json_getItem(_json, key.c_str());
    if (j == nullptr || j->type != Json_Array) {
        return 0;
    }
    int size = j->size;
    Json* child = j->child;
    
    // First pass to check array of floats assumption
    for (int ii = 0; ii < size; ii++) {
        if (child->type != Json_Number) {
            return 0;
        }
        child = child->next;
    }
    // Reset child pointer and write to buffer
    child = j->child;
    for (int ii = 0; ii < size; ii++) {
        buffer[ii] = child->valueFloat;
        child = child->next;
    }
    return size;
}

#pragma mark -
#pragma mark Object Parsing
/**
 * Moves the cursor to the value for the given key, and treats it as an object.
 *
 * This method returns false if the key does not exist, or its value is not an object.
 * However, the cursor always moves, so you must call endObject() to restore the
 * cursor even if it fails.
 *
 * @param  key  the field name to query
 *
 * @return true if the associated value is an object
 */
bool JSONReader::startObject(std::string key) {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    _stack.push_back(_json);
    _states.push_back(_arraymode);
    _arraymode = false;
    _json = Json_getItem(_json, key.c_str());
    return _json != nullptr && (_json->type == Json_Object || _json->type == Json_NULL);
}

/**
 * Treats the current cursor position as an object.
 *
 * This method should be used when extracting an object while you are traversing an
 * array (e.g. the object is an element inside the array).  It changes the parsing
 * state, but does not move the cursor.
 *
 * This method returns false if the current cursor position is not an object. However, 
 * the state always changes, so you must call endObject() to restore the state even 
 * if it fails.
 *
 * @return true if the cursor position is an object
 */
bool JSONReader::startObject() {
    CCASSERT(_json, "DOM cursor is currently undefined");
    _stack.push_back(_json);
    _states.push_back(_arraymode);
    _arraymode = false;
    return _json != nullptr && (_json->type == Json_Object || _json->type == Json_NULL);
}

/**
 * Ends processing of the current object, returning the cursor to its parent
 */
void JSONReader::endObject() {
    CCASSERT(!_arraymode, "Attempting to end object while in array mode");
    _json = _stack.back();
    _stack.pop_back();
    _arraymode = _states.back();
    _states.pop_back();
}


#pragma mark -
#pragma mark Array Parsing
/**
 * Returns the number of children for the given key
 *
 * If the key refers to an array, it returns the number of elements of the array.
 * If it refers to an object, it returns the number of fields in the object. If the
 * key does not exist, it returns 0.
 *
 * @param  key  the field name to query
 *
 * @return the number of children for the given key
 */
int JSONReader::getSize(std::string key) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    Json* child = Json_getItem(_json, key.c_str());
    return (child == nullptr ? 0 : child->size);
}

/**
 * Moves the cursor to begin processing the given key as an array
 *
 * The cursor will move to the first position of the array.  This method will return
 * the number of elements in the array. This method returns 0 if the cursor does not
 * refer to an array. However, the cursor always moves, so you must call endArray()
 * to restore the cursor even if it fails.
 *
 * @param  key  the field name to query
 *
 * @return the number of elements in the array
 */
int JSONReader::startArray(std::string key) {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    _stack.push_back(_json);
    _states.push_back(_arraymode);
    _json = Json_getItem(_json, key.c_str());
    _arraymode = true;
    if (_json == nullptr) {
        return 0;
    }
    
    int size = _json->size;
    _json = _json->child;
    return size;
}

/**
 * Moves the cursor to begin processing the current cursor position as an array
 *
 * The cursor will move to the first position of the array.  This method will return
 * the number of elements in the array. This method returns 0 if key does not exist,
 * or its value is not an array. However, the cursor always moves, so you must call
 * endArray() to restore the cursor even if it fails.
 *
 * @return the number of elements in the array
 */
int JSONReader::startArray() {
    CCASSERT(_json, "DOM cursor is currently undefined");
    _stack.push_back(_json);
    _states.push_back(_arraymode);
    _arraymode = true;
    int size = _json->size;
    _json = _json->child;
    return size;
}

/**
 * Ends processing of the current array, returning the cursor to its parent
 */
void JSONReader::endArray() {
    CCASSERT(_arraymode, "Attempting to end array while in object mode");
    _json = _stack.back();
    _stack.pop_back();
    _arraymode = _states.back();
    _states.pop_back();
}

/**
 * Advances the cursor to the next position in the array.
 *
 * This method returns false if there are no more elements in the array. You must have
 * called startArray() for this method to work.
 *
 * @return true if there was an element to advance to
 */
bool JSONReader::advance() {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(_arraymode, "Attempting to advance position while in object mode");
    _json = _json->next;
    return (_json == nullptr);
}





NS_CC_END
