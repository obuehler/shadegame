//
//  CUJSONReader.cpp
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
#include "CUJSONReader.h"
#include "CUStrings.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark Utility Functions

/** Special characters for processing string data */
static const unsigned char byteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

/**
 * Returns the first position of in that is not whitespace 
 *
 * This utility function is taken from the Spine JSON loader
 *
 * @param in the input string to parse
 *
 * @return the first position of in that is not whitespace 
 */
static const char* skipSpace(const char* in) {
    /* must propagate NULL since it's often called in skip(f(...)) form */
    if (!in) return 0;
    
    while (*in && (unsigned char)*in <= 32) {
        in++;
    }
    return in;
}

/**
 * Returns the case-insensitive comparison of two strings
 *
 * As with most comparison functions, it returns < 0 if s1 is less than
 * s2, 0 if they are equal, and > 0 if s1 is greater than s2.
 *
 * This utility function is taken from the Spine JSON loader.  However,
 * it uses std::string instead of char*
 *
 * @param s1 the first string to compare
 * @param s2 the first string to compare
 *
 * @return the case-insensitive comparison of two strings
 */
static int stdstrcasecmp(const char* s1, const char* s2) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    return _stricmp(s1, s2);
#else
    return strcasecmp(s1, s2);
#endif
}


#pragma mark -
#pragma mark JSONValue

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
JSONValue* JSONValue::createWithString(std::string src, const char** error) {
    JSONValue *reader = new (std::nothrow) JSONValue();
    if (reader && reader->initWithString(src, error)) {
        return reader;
    }
    return nullptr;
}

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
JSONValue* JSONValue::createWithFile(std::string file, const char** error) {
    JSONValue *reader = new (std::nothrow) JSONValue();
    if (reader && reader->initWithFile(file, error)) {
        return reader;
    }
    return nullptr;
    
}

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
bool JSONValue::initWithString(std::string src, const char** error) {
    const char* parse = parseValue(skipSpace(src.c_str()));
    if (!parse) {
        if (error != nullptr) *error = getError();
        dispose();
        return false;
    }
    return true;
}

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
bool JSONValue::initWithFile(std::string file, const char** error) {
    std::string contents = FileUtils::getInstance()->getStringFromFile(file);
    return initWithString(contents);
}

/**
 * Deletes all children and RIGHT siblings of this node, but does not deallocate 
 * this node.  Parents and left siblings are unaffected
 *
 * All values are restored to their defaults as well.
 */
void JSONValue::dispose() {
    _prev = nullptr;
    if (_next != nullptr) {
        _next->dispose();
        delete _next;
        _next = nullptr;
    }
    if (_child != nullptr) {
        _child->dispose();
        delete _child;
        _child = nullptr;
    }
    _valueString = "";
    _valueFloat = 0.0f;
    _valueBool = false;
}


#pragma mark Parsing

// ALL OF THIS CODE IS TAKEN FROM THE SPINE Json.c

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
const char* JSONValue::parseValue(const char* value) {
    /* Referenced by Constructor, parseArray(), and parseObject(). */
    CCASSERT(value, "JSON string is null");
    
    switch (*value) {
        case 'n': {
            if (!strncmp(value + 1, "ull", 3)) {
                _type = JSON_TYPE_NULL;
                return value + 4;
            }
            break;
        }
        case 'f': {
            if (!strncmp(value + 1, "alse", 4)) {
                _type = JSON_TYPE_BOOL;
                _valueBool = false;
                return value + 5;
            }
            break;
        }
        case 't': {
            if (!strncmp(value + 1, "rue", 3)) {
                _type = JSON_TYPE_BOOL;
                _valueBool = true;
                return value + 4;
            }
            break;
        }
        case '\"':
            return parseString(value);
        case '[':
            return parseArray(value);
        case '{':
            return parseObject(value);
        case '-': /* fallthrough */
        case '0': /* fallthrough */
        case '1': /* fallthrough */
        case '2': /* fallthrough */
        case '3': /* fallthrough */
        case '4': /* fallthrough */
        case '5': /* fallthrough */
        case '6': /* fallthrough */
        case '7': /* fallthrough */
        case '8': /* fallthrough */
        case '9':
            return parseNumber(value);
        default:
            break;
    }
    
    /* failure. */
    _ep = value;
    return nullptr;
}

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
const char* JSONValue::parseNumber(const char* num) {
    char * endptr;
    float n;
    
    /* Use the following to be Android compatible */
    n = (float)strtod( num, &endptr );
    
    /* ignore errno's ERANGE, which returns +/-HUGE_VAL */
    if (endptr != num) {
        /* Parse success, number found. */
        _valueFloat = n;
        _type = JSON_TYPE_FLOAT;
        return endptr;
    } else {
        /* Parse failure, ep is set. */
        _ep = num;
        return nullptr;
    }
}

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
const char* JSONValue::parseString(const char* str) {
    const char* ptr = str + 1;
    char* ptr2;
    char* out;
    int len = 0;
    unsigned uc, uc2;

    // Not a string!
    if (*str != '\"') {
        _ep = str;
        return nullptr;
    }
    
    // Skip escaped quotes.
    while (*ptr != '\"' && *ptr && ++len) {
        if (*ptr++ == '\\') ptr++;
    }
    
    // The length needed for the string, roughly.
    out = (char*)malloc(sizeof(char)*(len + 1));
    if (!out) return nullptr;
    
    ptr = str + 1;
    ptr2 = out;
    while (*ptr != '\"' && *ptr) {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else {
            ptr++;
            switch (*ptr) {
                case 'b':
                    *ptr2++ = '\b';
                    break;
                case 'f':
                    *ptr2++ = '\f';
                    break;
                case 'n':
                    *ptr2++ = '\n';
                    break;
                case 'r':
                    *ptr2++ = '\r';
                    break;
                case 't':
                    *ptr2++ = '\t';
                    break;
                case 'u': /* transcode utf16 to utf8. */
                    sscanf(ptr + 1, "%4x", &uc);
                    ptr += 4; /* get the unicode char. */
                    
                    // Check for invalid
                    if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) break;
                    
                    // UTF16 surrogate pairs
                    if (uc >= 0xD800 && uc <= 0xDBFF) {
                        // missing second-half of surrogate.
                        if (ptr[1] != '\\' || ptr[2] != 'u') break;
                        sscanf(ptr + 3, "%4x", &uc2);
                        ptr += 6;
                        // invalid second-half of surrogate
                        if (uc2 < 0xDC00 || uc2 > 0xDFFF) break;
                        uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
                    }
                    
                    len = 4;
                    if (uc < 0x80) {
                        len = 1;
                    } else if (uc < 0x800) {
                        len = 2;
                    } else if (uc < 0x10000) {
                        len = 3;
                    }
                    ptr2 += len;
                    
                    switch (len) {
                        case 4:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                            /* fallthrough */
                        case 3:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                            /* fallthrough */
                        case 2:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                            /* fallthrough */
                        case 1:
                            *--ptr2 = (uc | byteMark[len]);
                    }
                    ptr2 += len;
                    break;
                default:
                    *ptr2++ = *ptr;
                    break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;

    // Error handling if not \" or \0 ?
    if (*ptr == '\"') ptr++;
    
    std::string convert(out);
    _valueString = convert;
    _type = JSON_TYPE_STRING;
    free(out);
    return ptr;
}

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
const char* JSONValue::parseArray(const char* value) {
    JSONValue* curr = nullptr;
    
    // Check if an array
    if (*value != '[') {
        _ep = value;
        return 0;
    }
    
    _type = JSON_TYPE_ARRAY;
    value = skipSpace(value + 1);

    // empty array.
    if (*value == ']') return value + 1;
    
    _child = curr = new JSONValue();
    // memory fail
    if (!_child) return nullptr;
    
    // Skip any spacing, get the value.
    value = skipSpace(_child->parseValue(skipSpace(value)));
    if (!value) return nullptr;
    _size = 1;
    
    while (*value == ',') {
        JSONValue *sibl = new JSONValue();
        // memory fail
        if (!sibl) return 0;
        
        sibl->_pos = curr->_pos+1;
        curr->_next = sibl;
        sibl->_prev = curr;

        curr = sibl;
        value = skipSpace(curr->parseValue(skipSpace(value + 1)));

        // parse fail
        if (!value) return nullptr;
        _size++;
    }
    
    // end of array
    if (*value == ']') {
        return value + 1;
    }
    
    // Malformed.
    _ep = value;
    return nullptr;
}

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
const char* JSONValue::parseObject(const char* value) {
    JSONValue* curr = nullptr;

    // Check if an object
    if (*value != '{') {
        _ep = value;
        return 0;
    }

    
    _type = JSON_TYPE_OBJECT;
    value = skipSpace(value + 1);

    // Check for empty object.
    if (*value == '}') return value + 1;
    
    _child = curr = new JSONValue();
    // memory fail
    if (!_child) return nullptr;
    
    // Look for the object name first.
    value = skipSpace(_child->parseString(skipSpace(value)));
    if (!value) {
        return 0;
    }
    _child->_name = _child->_valueString;
    _child->_valueString = "";

    // Fail!
    if (*value != ':') {
        _ep = value;
        return nullptr;
    }
    
    // skip any spacing, get the value.
    value = skipSpace(_child->parseValue(skipSpace(value+1)));
    if (!value) {
        return 0;
    }
    _size = 1;
    
    while (*value == ',') {
        JSONValue *sibl = new JSONValue();
        // memory fail
        if (!sibl) return nullptr;
        
        sibl->_pos = curr->_pos+1;
        curr->_next = sibl;
        sibl->_prev = curr;

        curr = sibl;
        value = skipSpace(curr->parseValue(skipSpace(value + 1)));
        if (!value) return 0;
        curr->_name = curr->_valueString;
        curr->_valueString = "";

        // fail!
        if (*value != ':') {
            _ep = value;
            return nullptr;
        }
        value = skipSpace(curr->parseValue(skipSpace(value + 1)));
        if (!value) return 0;
        _size++;
    }
    
    // end of object
    if (*value == '}') {
        return value + 1;
    }
    
    // Malformed.
    _ep = value;
    return nullptr;
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
JSONValue* JSONValue::getItem(std::string name) const {
    JSONValue *c = _child;
    while (c != nullptr && stdstrcasecmp(c->_name.c_str(), name.c_str())) {
        c = c->_next;
    }
    return c;
}
/**
 * Returns the child of this node at the given position
 *
 * If this node has no children, or no child at that position, it returns null.
 *
 * @param pos the position for the child
 *
 * @return the child of this node at the given position
 */
JSONValue* JSONValue::getItem(int pos) const {
    JSONValue *c = _child;
    while (c != nullptr && pos < c->_pos) {
        c = c->_next;
    }
    return c;
}

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
bool JSONValue::asBool(bool defaultValue) const {
    if (_type == JSON_TYPE_BOOL) return _valueBool;
    return defaultValue;
}


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
float JSONValue::asFloat(float defaultValue) const {
    if (_type == JSON_TYPE_FLOAT) return _valueFloat;
    return defaultValue;
}

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
int JSONValue::asInt(int defaultValue) const {
    if (_type == JSON_TYPE_FLOAT) return (int)_valueFloat;
    return defaultValue;
}

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
std::string JSONValue::asString(std::string defaultValue) const {
    if (_type == JSON_TYPE_STRING) return _valueString;
    return defaultValue;
}

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
bool JSONValue::getBool(std::string name, bool defaultValue) const {
    JSONValue* object = getItem(name);
    return object->asBool(defaultValue);
}

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
float JSONValue::getFloat(std::string name, float defaultValue) const {
    JSONValue* object = getItem(name);
    return object->asFloat(defaultValue);
}

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
int JSONValue::getInt(std::string name, int defaultValue) const {
    JSONValue* object = getItem(name);
    return object->asInt(defaultValue);
}

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
std::string JSONValue::getString(std::string name, std::string defaultValue) const {
    JSONValue* object = getItem(name);
    return object->asString(defaultValue);
}


#pragma mark -
#pragma mark JSONReader
/**
 * Creates a new JSONReader.
 *
 * This constructor does not load any JSON, as there is no associated file. You can associate
 * a file later with the setFile() method.  Alternatively, you can parse a string directly
 * with the startJSON(string) method.
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
JSONReader* JSONReader::createWithFile(std::string file) {
    JSONReader *reader = new (std::nothrow) JSONReader();
    if (reader && reader->initWithFile(file)) {
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
 * a file later with the setFile() method.  Alternatively, you can parse a string directly
 * with the startJSON(string) method.
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
 * This initialier does not load the JSON file; it only stores the file name. You start
 * parsing with the startJSON() method.  You may change the file at any time before that
 * method is called.
 *
 * @param  file the file to read from
 *
 * @return  true if the reader is initialized properly, false otherwise.
 */
bool JSONReader::initWithFile(std::string file) {
    _file = FileUtils::getInstance()->fullPathForFilename(file);
    return _file != "";
}

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
bool JSONReader::startJSON() {
    CCASSERT(!_root, "JSON is already in progress");

    std::string contents = FileUtils::getInstance()->getStringFromFile(_file);
    _root = JSONValue::createWithString(contents.c_str());
    _json = _root;
    
    return _json != nullptr;
}

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
bool JSONReader::startJSON(std::string source) {
    CCASSERT(!_root, "JSON is already in progress");
    
    _root = JSONValue::createWithString(source.c_str());
    _json = _root;
    
    return _json != nullptr;
}

/**
 * Ends the current JSON parsing session, erasing the DOM tree.
 *
 * Once this method is called of the JSON methods will fail until startJSON() -- either
 * version -- is called again.
 */
void JSONReader::endJSON() {
    if (_root != nullptr) {
        delete _root;
        _root = nullptr;
    }
    reset();
}


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
    JSONValue* j = _json->getItem(key);
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
    JSONValue* j = _json->getItem(key);
    return j != nullptr && j->getType() == JSON_TYPE_NULL;
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
    JSONValue* j = _json->getItem(key);
    return j != nullptr && (j->getType() == JSON_TYPE_OBJECT || j->getType() == JSON_TYPE_NULL);
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
    JSONValue* j = _json->getItem(key);
    return j != nullptr && (j->getType() == JSON_TYPE_ARRAY || j->getType() == JSON_TYPE_NULL);
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
    JSONValue* j = _json->getItem(key);
    return j != nullptr && j->getType() == JSON_TYPE_BOOL;
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
    JSONValue* j = _json->getItem(key);
    return j != nullptr && j->getType() == JSON_TYPE_FLOAT;
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
    JSONValue* j = _json->getItem(key);
    return j != nullptr && j->getType() == JSON_TYPE_STRING;
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
    JSONValue* j = _json->getItem(key);
    if (j == nullptr || j->getType() != JSON_TYPE_ARRAY || j->getSize() != 2) {
        return false;
    }
    JSONValue* child = j->getChild();
    return (child->getType() == JSON_TYPE_FLOAT && child->getNext()->getType() == JSON_TYPE_FLOAT);
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
    JSONValue* j = _json->getItem(key);
    if (j == nullptr || j->getType() != JSON_TYPE_ARRAY) {
        return false;
    }
    JSONValue* child = j->getChild();
    for(int ii = 0; ii < j->getSize(); ii++) {
        if (child->getType() != JSON_TYPE_FLOAT) {
            return false;
        }
        child = child->getNext();
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
    if (_json == nullptr || _json->getType() != JSON_TYPE_ARRAY || _json->getSize() != 2) {
        return false;
    }
    JSONValue* child = _json->getChild();
    return (child->getType() == JSON_TYPE_FLOAT && child->getNext()->getType() == JSON_TYPE_FLOAT);

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
    if (_json == nullptr || _json->getType() != JSON_TYPE_ARRAY) {
        return false;
    }
    JSONValue* child = _json->getChild();
    for(int ii = 0; ii < _json->getSize(); ii++) {
        if (child->getType() != JSON_TYPE_FLOAT) {
            return false;
        }
        child = child->getNext();
    }
    return true;
}


#pragma mark Primitives
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
bool JSONReader::getBool(std::string name, bool defaultValue) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    JSONValue* j = _json->getItem(name);
    if (j == nullptr) {
        return false;
    }
    // Return the actual value
    return j->asBool(defaultValue);
}

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
float JSONReader::getNumber(std::string name, float defaultValue) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    JSONValue* j = _json->getItem(name);
    if (j == nullptr) {
        return 0.0f;
    }
    
    // Return the actual value
    return j->asFloat(defaultValue);
}

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
std::string JSONReader::getString(std::string name, std::string defaultValue) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    JSONValue* j = _json->getItem(name);
    if (j == nullptr) {
        return "";
    }
    
    // Return the actual value
    return j->asString(defaultValue);
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
Vec2 JSONReader::asVec2(const Vec2& defaultValue) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    JSONValue* child = _json->getChild();
    if (child == nullptr || child->getNext() == nullptr) {
        return Vec2(defaultValue);
    }
    float x = child->asFloat(defaultValue.x);
    float y = child->getNext()->asFloat(defaultValue.y);
    return Vec2(x,y);
}

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
int JSONReader::asFloatArray(float* buffer) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    int size = _json->getSize();
    JSONValue* child = _json->getChild();
    if (child == nullptr || _json->getType() != JSON_TYPE_ARRAY) {
        return 0;
    }

    // First pass to check array of floats assumption
    for(int ii = 0; ii < size; ii++) {
        if (child->getType() != JSON_TYPE_FLOAT) {
            return 0;
        }
        child = child->getNext();
    }
    // Reset child pointer and write to buffer
    child = _json->getChild();
    for(int ii = 0; ii < size; ii++) {
        buffer[ii] = child->asFloat(0.0f);
        child = child->getNext();
    }
    return size;
}

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
Vec2 JSONReader::getVec2(std::string name, const Vec2& defaultValue) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    JSONValue* j = _json->getItem(name);
    if (j == nullptr || j->getType() != JSON_TYPE_ARRAY || j->getSize() != 2) {
        return Vec2(defaultValue);
    }
    JSONValue* child = j->getChild();
    float x = child->asFloat(defaultValue.x);
    float y = child->getNext()->asFloat(defaultValue.y);
    return Vec2(x,y);
}

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
int  JSONReader::getFloatArray(std::string name, float* buffer) const {
    CCASSERT(_json, "DOM cursor is currently undefined");
    CCASSERT(!_arraymode, "Key checking is undefined in array mode");
    JSONValue* j = _json->getItem(name);
    if (j == nullptr || j->getType() != JSON_TYPE_ARRAY) {
        return 0;
    }
    int size = j->getSize();
    JSONValue* child = j->getChild();
    
    // First pass to check array of floats assumption
    for (int ii = 0; ii < size; ii++) {
        if (child->getType() != JSON_TYPE_FLOAT) {
            return 0;
        }
        child = child->getNext();
    }
    // Reset child pointer and write to buffer
    child = j->getChild();
    for (int ii = 0; ii < size; ii++) {
        buffer[ii] = child->asFloat(0.0f);
        child = child->getNext();
    }
    return size;
}


#pragma mark Objects
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
    _json = _json->getItem(key);
    return _json != nullptr && (_json->getType() == JSON_TYPE_OBJECT || _json->getType() == JSON_TYPE_NULL);
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
    return _json != nullptr && (_json->getType() == JSON_TYPE_OBJECT || _json->getType() == JSON_TYPE_NULL);
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


#pragma mark Arrays
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
    JSONValue* child = _json->getItem(key);
    return (child == nullptr ? 0 : child->getSize());
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
    _json = _json->getItem(key);
    _arraymode = true;
    if (_json == nullptr) {
        return 0;
    }
    
    int size = _json->getSize();
    _json = _json->getChild();
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
    int size = _json->getSize();
    _json = _json->getChild();
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
    _json = _json->getNext();
    return (_json == nullptr);
}


NS_CC_END
