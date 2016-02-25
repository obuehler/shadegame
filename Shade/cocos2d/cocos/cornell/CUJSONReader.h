//
//  CUJSONReader.h
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
#ifndef __CU_JSON_READER_H__
#define __CU_JSON_READER_H__
#include <base/CCRef.h>
#include <platform/CCFileUtils.h>
#include <editor-support/spine/Json.h>

NS_CC_BEGIN


#pragma mark -
#pragma mark JSON Reader
/**
 * Class represents a simple DOM reader for parsing JSON
 *
 * As most DOM readers, this uses a begin-end syntax to descend the document to tree. At any time, 
 * this reader has a cursor positioned at a particular node. You use methods to move this cursor.  
 * For example, to read the contents of an object, you must call startObject() to move the cursor
 * to access its contents, and then must call endObject() when you are done.  Similarly for arrays.  
 * Primitive types, such as bools, floats, and strings can all be access directly.
 *
 * Objects and arrays are interchangeable.  Any object can be treated as an array and traversed 
 * accordingly.  Traversing an object as an array allows you to query both the keys and values
 * of the object, instead of just the keys.
 *
 * Elements of the JSON may be accessed without or without a name.  If no name is given, it assumes
 * that you are refering to the current node in the DOM tree.  Otherwise, you are refering to a field
 * of a given name.  This is crucial for going back and forth between objects and arrays.  For example,
 * if you call startObject("foo"), it will descend the field "foo" from the current position.  If you
 * then call startArray() it will stay at the same position, but treat it as an array instead.
 *
 * This reader tries to fail as little as possible.  If a field or current position is treated as the
 * wrong type, it will return a default value instead.  This makes it easier to process missing data.
 * If you really care about whether data is there or not, use the type-checking methods.
 */
class CC_DLL JSONReader : public Ref {
protected:
    /** The root of the JSON DOM tree */
    Json* _root;
    /** The current position in the DOM tree */
    Json* _json;
    /** The file with the JSON source */
    std::string _file;
    /** A stack to allow us to traverse the tree*/
    std::vector<Json*> _stack;
    /** Whether we are in object mode or array mode */
    bool _arraymode;
    /** A stack to allow us to tracks states as we traverse the tree */
    std::vector<bool> _states;
    
public:
#pragma mark Static Constructors
    /**
     * Creates a new JSONReader.
     *
     * This constructor does not load any JSON, as there is no associate file. You can associate
     * a file later with the setFile() method.
     *
     * @returns an autoreleased JSONReader object
     */
    static JSONReader* create();

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
    static JSONReader* create(std::string file);
    
    
#pragma mark Attributes
    /**
     * Sets the file for this JSON reader.
     *
     * This constructor does load the JSON file; it only stores the file name. You start parsing
     * with the startJSON() method.
     *
     * @param  file for this JSON reader
     */
    void setFile(std::string file) {
        endJSON();
        _file = FileUtils::getInstance()->fullPathForFilename(file);
    }
    
    /**
     * Returns the file for this JSON reader.
     *
     * If this value is "", then there is no file.  Instead, the JSONReader is intended to be used
     * directly on a string via the startJSON(string) method.
     *
     * @param  file for this JSON reader
     */
    std::string getFile() const { return _file; }
    
    
#pragma mark Initial Parsing
    /**
     * Starts a JSON parser for the current file.
     *
     * The parser will fail if the JSON is not well-formed.  In that case, the method will return false.
     * There are (unfortunately) no error messages for why parsing failed.
     *
     * @return true if the file is a well-formed JSON file.
     */
    bool startJSON();
    
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
    bool startJSON(std::string source);
    
    /**
     * Ends the current JSON parsing session, erasing the DOM tree.
     *
     * Once this method is called of the JSON methods will fail until startJSON() -- either version --
     * is called again.
     */
    void endJSON();
    
    /**
     * Resets the JSON parser to the top of the DOM tree.
     *
     * No information about the JSON is lost.  This method simply resets the cursor position.
     */
    void reset() { _stack.clear(); _states.clear(); _json = _root; _arraymode = false; }
    
    /**
     * Returns the key for the current cursor position in the DOM.
     *
     * @return the key for the current cursor position in the DOM.
     */
    std::string getKey() const { return _json->name; }

    
#pragma mark Type Checking
    /**
     * Returns true if the current cursor position has a nullptr value.
     * 
     * @return true if the current cursor position has a nullptr value.
     */
    bool isNull() const     {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_NULL;
    }

    /**
     * Returns true if the current cursor position represents an object.
     *
     * @return true if the current cursor position represents an object.
     */
    bool isObject() const   {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_Object || _json->type == Json_NULL;
    }

    /**
     * Returns true if the current cursor position represents an array.
     *
     * @return true if the current cursor position represents an array.
     */
    bool isArray() const    {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_Array || _json->type == Json_NULL;
    }
    
    /**
     * Returns true if the current cursor position represents a boolean value
     *
     * @return true if the current cursor position represents a boolean value
     */
    bool isBool() const     {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_True || _json->type == Json_False;
    }
    
    /**
     * Returns true if the current cursor position represents a number
     *
     * All numbers in JSON files are treated as floats.
     *
     * @return true if the current cursor position represents a number
     */
    bool isNumber() const   {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_Number;
    }
    
    /**
     * Returns true if the current cursor position represents a string
     *
     * @return true if the current cursor position represents a string
     */
    bool isString() const   {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_String;
    }
    
    /**
     * Returns true if the current cursor position represents a Vec2 value
     *
     * In a JSON file, a Vec2 is just a two-element array, whose elements are both numbers.
     *
     * @return true if the current cursor position represents a Vec2 value
     */
    bool isVec2() const;
    
    /**
     * Returns true if the current cursor position represents an array of floats
     *
     * This method is just an additional type-check on top of isArray.
     *
     * @return true if the current cursor position represents an array of floats
     */
    bool isFloatArray() const;

    /**
     * Returns true if there is an entry for the given key.
     *
     * @param  key  the field name to query
     *
     * @return true if there is an entry for the given key.
     */
    bool exists(std::string key) const;

    /**
     * Returns true if the entry for key exists and has a nullptr value.
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and has a nullptr value.
     */
    bool isNull(std::string key) const;

    /**
     * Returns true if the entry for key exists and represents an object.
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents an object.
     */
    bool isObject(std::string key) const;

    /**
     * Returns true if the entry for key exists and represents an array.
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents an array.
     */
    bool isArray(std::string key) const;

    /**
     * Returns true if the entry for key exists and represents a boolean value
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents a boolean value
     */
    bool isBool(std::string key) const;

    /**
     * Returns true if the entry for key exists and represents a number
     *
     * All numbers in JSON files are treated as floats.
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents a number
     */
    bool isNumber(std::string key) const;

    /**
     * Returns true if the entry for key exists and represents a string
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents a string
     */
    bool isString(std::string key) const;

    /**
     * Returns true if the entry for key exists and represents a Vec2 value
     *
     * In a JSON file, a Vec2 is just a two-element array, whose elements are both numbers.
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents a Vec2 value
     */
    bool isVec2(std::string key) const;
    
    /**
     * Returns true if the entry for key exists and represents an array of floats
     *
     * This method is just an additional type-check on top of isArray.
     *
     * @param  key  the field name to query
     *
     * @return true if the entry for key exists and represents an array of floats
     */
    bool isFloatArray(std::string key) const;

    
#pragma mark Primitives
    /**
     * Returns the boolean value for the current cursor position.
     *
     * If the current position is not a boolean, it returns False as a default.
     *
     * @return the boolean value for the current cursor position.
     */
    bool getBool() const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->type == Json_True;
    }

    /**
     * Returns the number for the current cursor position.
     *
     * All numbers in JSON files are treated as floats. If the current position is not
     * a number, it returns 0.0f as a default.
     *
     * @return the number for the current cursor position.
     */
    float getNumber() const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->valueFloat;
    }

    /**
     * Returns the string for the current cursor position.
     *
     * If the current position is not a string, it returns "" as a default.
     *
     * @return the string for the current cursor position.
     */
    std::string getString() const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return std::string(_json->valueString);
    }
    
    /**
     * Returns the Vec2 value for the current cursor position.
     *
     * If the current position is not a Vec2, it returns Vec2::ZERO as a default.
     *
     * @return the Vec2 value for the current cursor position.
     */
    Vec2 getVec2() const;
    
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
    int  getFloatArray(float* buffer) const;
    
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
    bool getBool(std::string key) const;

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
    float getNumber(std::string key) const;

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
    std::string getString(std::string key) const;
    
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
    Vec2 getVec2(std::string key) const;
    
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
    int  getFloatArray(std::string key, float* buffer) const;
    
    
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
    bool startObject(std::string key);

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
    bool startObject();
    
    /**
     * Ends processing of the current object, returning the cursor to its parent
     */
    void endObject();
    
    
#pragma mark Array Parsing
    /**
     * Returns the number of children for the cursor node
     *
     * If the current cursor is an array, it returns the number of elements of the array.
     * If it is an object, it returns the number of fields in the object.
     *
     * @return the number of children for the cursor node
     */
    int  getSize() const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->size;
    }
    
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
    int  getSize(std::string key) const;

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
    int  startArray(std::string key);

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
    int  startArray();
    
    /**
     * Ends processing of the current array, returning the cursor to its parent
     */
    void endArray();
    
    /**
     * Advances the cursor to the next position in the array.
     *
     * This method returns false if there are no more elements in the array. You must have 
     * called startArray() for this method to work.
     *
     * @return true if there was an element to advance to
     */
    bool advance();
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a basic JSON reader with the default values 
     */
    JSONReader() : _root(nullptr), _json(nullptr), _arraymode(false) {}
    
    /**
     * Deletes the JSON reader, releasing all resources
     */
    ~JSONReader() { endJSON(); }
    
    /**
     * Initializes a new JSONReader.
     *
     * This initializer does not load any JSON, as there is no associate file. You can associate
     * a file later with the setFile() method.
     *
     * @return  true if the reader is initialized properly, false otherwise.
     */
    bool init();
    
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
    bool init(std::string file);
};

NS_CC_END
#endif /* __CU_JSON_READER_H__ */
