//
//  CUJSONReader.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a simple reader to process JSON files. It is an adaptation of the JSON
//  parser provided with the Spine editor (built-into Cocos2d).  It provides a DOM-type interface
//  for parsing the data in a structured way.
//
//  Previously, we had used the Spine JSON loader directly.  However, there appear to be some
//  linker errors when we try to pull Spine into Cocos (as opposed to into the application) within
//  Android.  So we were forced to reimplement the functionality.  When the code is the same, we
//  have cited it accordingly.
//
//  Most of the API is straight-forward except for objects and arrays.  Those must be "unwrapped"
//  in order to access them.  You unwrap with a startObject or a startArray method.  Those methods
//  have returns values that indicate whether or not they were successful on the given node.
//  However, regardless of whether or not the method succeed, you must wrap everything back up
//  with an endObject or endArray method to proceed.
//
//  Author: Walker White
//  Version: 3/12/16
//
#ifndef __CU_JSON_READER_H__
#define __CU_JSON_READER_H__
#include <base/CCRef.h>
#include <platform/CCFileUtils.h>

NS_CC_BEGIN

#pragma mark -
#pragma mark JSON Type Identifiers

/* JSON Value is empty */
#define JSON_TYPE_NULL      0
/* JSON Value is a boolean */
#define JSON_TYPE_BOOL      1
/* JSON Value is a number */
#define JSON_TYPE_FLOAT     2
/* JSON Value is a string */
#define JSON_TYPE_STRING    3
/* JSON Value is an array */
#define JSON_TYPE_ARRAY     4
/* JSON Value is an object */
#define JSON_TYPE_OBJECT    5


#pragma mark -
#pragma mark JSON Tree
/**
 * Class to represent a JSON as a tree
 *
 * Unlike the cursor access of JSONReader, this class allows you access the JSON tree directly.
 * In some applications, this may be more natural.  However, the downside is that this does
 * not support the reference counting interface of Cocos2d.  If you want a reader with release
 * and retain semantics, you should use JSONReader instead.
 *
 * NOTE: This class is not a subclass of Ref.  Despite our usage of the create() method, everything
 * is allocated directly.  Hence, this class has lower overhead.  We only use the static constructor
 * so that we can allow the constructor to fail.
 */
class CC_DLL JSONValue {
private:
    /** The item's name string, if this item is in an object. */
    std::string _name;
    /** The item's position, in an object or array */
    int _pos;
    /** The type of the item */
    int _type;
    
    // TREE STRUCTURE
    /** The next sibling (NULL if none) */
    JSONValue* _next;
    /** The previous sibling (NULL if none) */
    JSONValue* _prev;
    /** The child array or object (NULL if none) */
    JSONValue* _child;
    /** The number of children. */
    int _size;
    
    // NODE VALUES
    /* The item's string, if type is a string */
    std::string _valueString;
    /* The item's value, if type is a boolean */
    bool  _valueBool;
    /* The item's number, if type is a number */
    float _valueFloat;

    /** Pointer to parsing error */
    const char* _ep;

public:
#pragma mark Static Constructors
    /**
     * Creates a new JSONValue.
     *
     * This constructor does not do any parsing, as there is no file or source string.
     *
     * @returns an allocated JSONValue THAT IS NOT MANAGED
     */
    static JSONValue* create() {
        return new JSONValue();
    }

    /**
     * Creates a new JSONValue.
     *
     * This constructor parses the given JSON string (not a file) and creates the 
     * associated JSON tree.  The string is parsed immediately.  If parsing fails,
     * this method returns nullptr, but stores the error in the associated char **.
     *
     * @param  src      the JSON string to parse
     * @param  error    pointer to store an error result
     *
     * @returns an allocated JSONValue THAT IS NOT MANAGED
    */
    static JSONValue* createWithString(std::string src, const char** error = nullptr);

    /**
     * Creates a new JSONValue.
     *
     * This constructor does load the JSON file and creates the associated JSON tree.  
     * The string is parsed immediately.  If parsing fails, this method returns nullptr, 
     * but stores the error in the associated char **.     
     *
     * @param  file     the file to read from
     * @param  error    pointer to store an error result
     *
     * @returns an allocated JSONValue THAT IS NOT MANAGED
     */
    static JSONValue* createWithFile(std::string file, const char** error = nullptr);

    
#pragma mark Node Attributes
    /**
     * Returns the type of this JSON value, encoded as an integer
     *
     * @return the type of this JSON value, encoded as an integer
     */
    int getType() const {
        return _type;
    }

    /**
     * Returns the position of this JSON node, as compared to its siblings
     *
     * If a value has no siblings, this method returns 0.  Otherwise, it
     * returns the number of steps before _prev is 0.
     *
     * @return the position of this JSON node, as compared to its siblings
     */
    int getPosition() const {
        return _pos;
    }
    
    /**
     * Returns the name of this JSON node in the tree
     *
     * If this node is the root, or an element in an array, this method 
     * returns ""
     *
     * @return the name of this JSON node in the tree
     */
    std::string getName() const {
        return _name;
    }
 
    
#pragma mark Tree Traversal
    /**
     * Returns the child of this node with the given name
     *
     * If this node has no children, or no child with that name, it returns null.
     *
     * @param name the identifier for the child
     *
     * @return the child of this node with the given name
     */
    JSONValue* getItem(std::string name) const;

    /**
     * Returns the child of this node at the given position
     *
     * If this node has no children, or no child at that position, it returns null.
     *
     * @param pos the position for the child
     *
     * @return the child of this node at the given position
     */
    JSONValue* getItem(int pos) const;
    
    /**
     * Returns the next sibling in the current sibling list
     *
     * If there are no more siblings, then this method returns null
     *
     * @return the next sibling in the current sibling list
     */
    JSONValue* getNext() const { return _next; }

    /**
     * Returns the previous sibling in the current sibling list
     *
     * If there are no previous siblings, then this method returns null
     *
     * @return the previous sibling in the current sibling list
     */
    JSONValue* getPrev() const { return _prev; }
    
    /**
     * Returns the immediate child of this node
     *
     * If this node has no children, then this method returns null
     *
     * @return the immediate child of this node
     */
    JSONValue* getChild() const { return _child ; }
    
    /**
     * Returns the number of children of this node.
     *
     * @return the number of children of this node.
     */
    int getSize() const { return _size; }


#pragma mark JSON Values
    /**
     * Returns the boolean value for the JSON node
     *
     * If the current position is not a boolean, it returns the given default value
     * instead.  If no default it specified, it will return false in this case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the boolean value for the JSON node
     */
    bool  asBool(bool defaultValue=false) const;

    /**
     * Returns the float for the JSON node
     *
     * All numbers in JSON files are treated as floats. If the current position is not
     * a number, it returns the given default value instead.  If no default it specified,
     * it will return 0.0f in this case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the float for the JSON node
     */
    float asFloat(float defaultValue=0.0f) const;
    
    /**
     * Returns the float for the JSON node
     *
     * All numbers in JSON files are treated as floats. This method is simply the result
     * of casting asFloat() to an int.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the float for the JSON node
     */
    int asInt(int defaultValue=0) const;
    
    /**
     * Returns the string for the JSON node
     *
     * If the current position is not a string, it returns the given default value instead.
     * If no default it specified, it will return the empty string "" in this case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the string for the JSON node
     */
    std::string asString(std::string defaultValue="") const;

    /**
     * Returns the boolean value for the child with the given name.
     *
     * If the name does not exist, or does not refer to a boolean, this method returns
     * the given default value instead.  If no default it specified, it will return
     * false in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the boolean value for the given name.
     */
    bool getBool(std::string name, bool defaultValue=false) const;
    
    /**
     * Returns the float for the given name.
     *
     * All numbers in JSON files are treated as floats.  If the name does not exist,
     * or does not refer to a number, this method returns the given default value
     * instead.  If no default it specified, it will return 0.0f in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the float for the given name.
     */
    float getFloat(std::string name, float defaultValue=0.0f) const;

    /**
     * Returns the float for the given name.
     *
     * All numbers in JSON files are treated as floats.  This method is simply the 
     * result of casting getFloat() to an int.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the float for the given name.
     */
    int getInt(std::string name, int defaultValue=0) const;
    
    /**
     * Returns the string for the given name.
     *
     * If the name does not exist, or does not refer to a string, this method returns
     * the given default value instead. If no default it specified, it will return
     * the empty string "" in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the string for the given name.
     */
    std::string getString(std::string name, std::string defaultValue="") const;


#pragma mark Parsing
private:
    /**
     * Parse the given character position into this JSON node
     *
     * This method only parses locally.  If there is material in the string past the
     * parsable value, it is ignored.  This allows us to parse recursively.
     *
     * If parsing fails, this method returns null.
     *
     * @param value string whose suffix is a JSON value
     *
     * @return the character position after the parsed value
     */
    const char* parseValue(const char* value);
    
    /**
     * Parse the given character position into a number
     *
     * This method only parses locally.  If there is material in the string past the
     * parsable value, it is ignored.  This allows us to parse recursively.
     *
     * The number is stored in this node. If parsing fails, this method returns null.
     *
     * @param value string whose suffix is a JSON number
     *
     * @return the character position after the parsed number
     */
    const char* parseNumber(const char* num);

    /**
     * Parse the given character position into a string
     *
     * This method only parses locally.  If there is material in the string past the
     * parsable value, it is ignored.  This allows us to parse recursively.
     *
     * The string is stored in this node. If parsing fails, this method returns null.
     *
     * @param value string whose suffix is a JSON string
     *
     * @return the character position after the parsed string
     */
    const char* parseString(const char* str);
    
    /**
     * Parse the given character position into an array
     *
     * This method only parses locally.  If there is material in the string past the
     * parsable value, it is ignored.  This allows us to parse recursively.
     *
     * The array is stored as children of this node. If parsing fails, this method 
     * returns null.
     *
     * @param value string whose suffix is a JSON array
     *
     * @return the character position after the parsed array
     */
    const char* parseArray(const char* value);
    
    /**
     * Parse the given character position into an object
     *
     * This method only parses locally.  If there is material in the string past the
     * parsable value, it is ignored.  This allows us to parse recursively.
     *
     * The object is stored as children of this node. If parsing fails, this method
     * returns null.
     *
     * @param value string whose suffix is a JSON object
     *
     * @return the character position after the parsed object
     */
    const char* parseObject(const char* value);
    
    /**
     * Returns a pointer to the parse error.
     *
     * The returned value is just a pointer to the position in the parse string
     * that caused a problem.  You may need to move up or down the string to
     * properly understand the error.
     */
    const char* getError(void) const {
        return _ep;
    }
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a basic JSON value node with the default values
     */
    JSONValue() : _name(""), _type(0), _pos(0), _next(nullptr), _prev(nullptr), _child(nullptr), _size(0), _ep(nullptr), _valueBool(false), _valueFloat(0.0f), _valueString("") {}
    
    /**
     * Deletes the JSON value, releasing all resources
     */
    ~JSONValue() { dispose(); }
    
    /**
     * Deletes all children and RIGHT siblings of this node, but does not deallocate
     * this node.  Parents and left siblings are unaffected
     *
     * All values are restored to their defaults as well.
     */
    void dispose();
    
    /**
     * Initializes a new JSONValue.
     *
     * This initializer parses the given JSON string (not a file) and creates the
     * associated JSON tree.  The string is parsed immediately.  If parsing fails,
     * this method returns false, but stores the error in the associated char **.
     *
     * @param  src      the JSON string to parse
     * @param  error    pointer to store an error result
     *
     * @returns true if the JSON is initialized properly, false otherwise.
     */
    bool initWithString(std::string src, const char** error = nullptr);
    
    /**
     * Initializes a new JSONReader.
     *
     * This initializer does load the JSON file and creates the associated JSON tree.
     * The string is parsed immediately.  If parsing fails, this method returns false,
     * but stores the error in the associated char **.
     *
     * @param  file     the file to read from
     * @param  error    pointer to store an error result
     *
     * @returns true if the JSON is initialized properly, false otherwise.
     */
    bool initWithFile(std::string file, const char** error = nullptr);
};


#pragma mark -
#pragma mark JSON Reader
/**
 * Class to provide a simple DOM reader for parsing JSON
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
    JSONValue* _root;
    /** The current position in the DOM tree */
    JSONValue* _json;
    /** The file with the JSON source (may be empty) */
    std::string _file;
    /** A stack to allow us to traverse the tree*/
    std::vector<JSONValue*> _stack;
    /** Whether we are in object mode or array mode */
    bool _arraymode;
    /** A stack to allow us to tracks states as we traverse the tree */
    std::vector<bool> _states;
    
public:
#pragma mark Static Constructors
    /**
     * Creates a new JSONReader.
     *
     * This constructor does not load any JSON, as there is no associated file. You can associate
     * a file later with the setFile() method.  Alternatively, you can parse a string directly
     * with the startJSON(string) method.
     *
     * @returns an autoreleased JSONReader object
     */
    static JSONReader* create();

    /**
     * Creates a new JSONReader for a given file.
     *
     * This constructor does not load the JSON file; it only stores the file name. You start 
     * parsing with the startJSON() method.  You may change the file at any time before that
     * method is called.
     *
     * @param  file the file to read from
     *
     * @returns an autoreleased JSONReader object
     */
    static JSONReader* createWithFile(std::string file);

    
#pragma mark Attributes
    /**
     * Sets the file for this JSON reader.
     *
     * This method does not load the JSON file; it only stores the file name. You start
     * parsing with the startJSON() method.  You may change the file at any time before
     * that method is called. 
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
     * If this value is "", then there is no file.  Instead, the JSONReader is intended 
     * to be used directly on a string via the startJSON(string) method.
     *
     * @param  file for this JSON reader
     */
    std::string getFile() const { return _file; }
    
    
#pragma mark DOM Parsing
    /**
     * Starts a JSON parser for the current file.
     *
     * The parser will fail if the JSON is not well-formed.  In that case, the method will 
     * return false. There are (unfortunately) no error messages for why parsing failed.
     * If you need more details, you may want to use JSONValue directly.
     *
     * @return true if the file is a well-formed JSON file.
     */
    bool startJSON();
    
    /**
     * Starts a JSON parser for the given JSON string.
     *
     * This method will ignore the associated file, and parse the provided string instead.  
     * The string should be an JSON string, not a file containing a JSON string.
     *
     * The parser will fail if the JSON is not well-formed.  In that case, the method will
     * return false. There are (unfortunately) no error messages for why parsing failed.
     * If you need more details, you may want to use JSONValue directly.
     *
     * @param  source   the JSON source string
     *
     * @return true if the file is a well-formed JSON file.
     */
    bool startJSON(std::string source);
    
    /**
     * Ends the current JSON parsing session, erasing the DOM tree.
     *
     * Once this method is called of the JSON methods will fail until startJSON() -- either 
     * version -- is called again.
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
    std::string getKey() const { return _json->getName(); }

    
#pragma mark Type Checking
    /**
     * Returns true if the current cursor position has a nullptr value.
     * 
     * @return true if the current cursor position has a nullptr value.
     */
    bool isNull() const     {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->getType() == JSON_TYPE_NULL;
    }

    /**
     * Returns true if the current cursor position represents an object.
     *
     * @return true if the current cursor position represents an object.
     */
    bool isObject() const   {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->getType() == JSON_TYPE_OBJECT || _json->getType() == JSON_TYPE_NULL;
    }

    /**
     * Returns true if the current cursor position represents an array.
     *
     * @return true if the current cursor position represents an array.
     */
    bool isArray() const    {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->getType() == JSON_TYPE_ARRAY || _json->getType() == JSON_TYPE_NULL;
    }
    
    /**
     * Returns true if the current cursor position represents a boolean value
     *
     * @return true if the current cursor position represents a boolean value
     */
    bool isBool() const     {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->getType() == JSON_TYPE_BOOL;
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
        return _json->getType() == JSON_TYPE_FLOAT;
    }
    
    /**
     * Returns true if the current cursor position represents a string
     *
     * @return true if the current cursor position represents a string
     */
    bool isString() const   {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->getType() == JSON_TYPE_STRING;
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
     * If the current position is not a boolean, it returns the given default value
     * instead.  If no default it specified, it will return false in this case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the boolean value for the current cursor position.
     */
    bool asBool(bool defaultValue=false) const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->asBool(defaultValue);
    }

    /**
     * Returns the number for the current cursor position.
     *
     * All numbers in JSON files are treated as floats. If the current position is not
     * a number, it returns the given default value instead.  If no default it specified, 
     * it will return 0.0f in this case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the number for the current cursor position.
     */
    float asNumber(float defaultValue=0.0f) const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->asFloat(defaultValue);
    }

    /**
     * Returns the string for the current cursor position.
     *
     * If the current position is not a string, this method returns the given default 
     * value instead.If no default it specified, it will return the empty string "" in 
     * this case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the string for the current cursor position.
     */
    std::string asString(std::string defaultValue="") const {
        CCASSERT(_json, "DOM cursor is currently undefined");
        return _json->asString(defaultValue);
    }
    
    /**
     * Returns the Vec2 value for the current cursor position.
     *
     * If the current position is not a Vec2, this method returns the given default 
     * value instead. If no default it specified, it will return Vec2::ZERO in this 
     * case.
     *
     * @param defaultValue the value to use in case of an error
     *
     * @return the Vec2 value for the current cursor position.
     */
    Vec2 asVec2(const Vec2& defaultValue=Vec2::ZERO) const;
    
    /**
     * Fills the buffer with the contents of the cursor position
     *
     * This method assumes that the current cursor position is an array of floats
     * (e.g. the children of the cursor are all float nodes), and writes these
     * floats to the given buffer.  The method returns the number of elements 
     * written. If the cursor is not a valid float array, nothing will be written,
     * and the method will return 0.
     *
     * The buffer must be sized large enough to recieve the float array.  Use the
     * method getSize() to determine the size of the array.
     *
     * @param  buffer   the buffer to store the float values
     *
     * @return number of elements written to the buffer
     */
    int  asFloatArray(float* buffer) const;
    
    /**
     * Returns the boolean value for the child with the given name.
     *
     * If the name does not exist, or does not refer to a boolean, this method returns
     * the given default value instead.  If no default it specified, it will return 
     * false in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the boolean value for the given name.
     */
    bool getBool(std::string name, bool defaultValue=false) const;

    /**
     * Returns the number for the given name.
     *
     * All numbers in JSON files are treated as floats.  If the name does not exist,
     * or does not refer to a number, this method returns the given default value 
     * instead.  If no default it specified, it will return 0.0f in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the number for the given name.
     */
    float getNumber(std::string name, float defaultValue=0.0f) const;

    /**
     * Returns the string for the given name.
     *
     * If the name does not exist, or does not refer to a string, this method returns
     * the given default value instead. If no default it specified, it will return 
     * the empty string "" in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the string for the given name.
     */
    std::string getString(std::string name, std::string defaultValue="") const;
    
    /**
     * Returns the Vec2 value for the given name.
     *
     * If the name does not exist, or does not refer to a Vec2 value, this method 
     * returns the given default value instead. If no default it specified, it will 
     * return Vec2::ZERO in this case.
     *
     * @param  name         the name for the child to access
     * @param  defaultValue the value to use in case of an error
     *
     * @return the Vec2 value for the given name.
     */
    Vec2 getVec2(std::string name, const Vec2& defaultValue=Vec2::ZERO) const;
    
    /**
     * Fills the buffer with the contents of the given name.
     *
     * This method assumes that the key refers to an array of floats (e.g. the children
     * of the key are all float nodes), and writes these floats to the given buffer.  
     * The method returns the number of elements written. If the key does not exist, or
     * does not refer to a valid float array, nothing will be written, and the method 
     * will return 0.
     *
     * The buffer must be sized large enough to recieve the float array.  Use the
     * method getSize() to determine the size of the array.
     *
     * @param  buffer   the buffer to store the float values
     *
     * @param  name  the name for the child to access
     *
     * @return number of elements written to the buffer
     */
    int  getFloatArray(std::string name, float* buffer) const;
    
    
#pragma mark Objects
    /**
     * Moves the cursor to the value for the given key, and treats it as an object.
     *
     * This method returns false if the key does not exist, or its value is not an object.
     * However, the cursor always moves, so you must call endObject() to restore the
     * cursor even if it fails.
     *
     * @param  name  the name for the child to access
     *
     * @return true if the associated value is an object
     */
    bool startObject(std::string name);

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
    
    
#pragma mark Arrays
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
        return _json->getSize();
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
     * a file later with the setFile() method.  Alternatively, you can parse a string directly
     * with the startJSON(string) method.
     *
     * @return  true if the reader is initialized properly, false otherwise.
     */
    bool init();
    
    /**
     * Initializes a new JSONReader.
     *
     * This initialier does not load the JSON file; it only stores the file name. You start
     * parsing with the startJSON() method.  You may change the file at any time before that
     * method is called.
     *
     * @param  file the file to read from
     *
     * @return  true if the reader is initialized properly, false otherwise.
     */
    bool initWithFile(std::string file);
};

NS_CC_END
#endif /* __CU_JSON_READER_H__ */
