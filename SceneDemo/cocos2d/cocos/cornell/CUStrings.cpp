//
//  CUStrings.h
//  Cornell Extensions to Cocos2D
//
//  Android does not support a lot of the built-in string methods.  Therefore, we
//  need alternate definitions that are platform agnostic.  Note that these functions
//  have exactly the same name as functions in the std namespace, but all live in
//  the cocos2d namespace.
//
//  Author: Walker White
//  Version: 2/10/16
//
#include "CUStrings.h"
#include <string>

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <wchar.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#endif

NS_CC_BEGIN

#pragma mark NUMBER TO STRING FUNCTIONS

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( int value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( long long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( unsigned value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( unsigned long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( unsigned long long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( float value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( double value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string to_string( long double value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_string(value);
#endif
}


#pragma mark -
#pragma mark NUMBER TO WIDE STRING FUNCTIONS

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( int value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( long long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( unsigned value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( unsigned long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( unsigned long long value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( float value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( double value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring to_wstring( long double value ) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstringstream ss;
    ss << value;
    return ss.str();
#else
    return std::to_wstring(value);
#endif
}


#pragma mark -
#pragma mark STRING TO NUMBER FUNCTIONS

/**
 * Returns the integer equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to an integer value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the integer equivalent to the given string
 */
int  stoi(const std::string& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const char* start = str.c_str();
    char* end;
    int result = (int)std::strtol (start, &end, base);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stoi(str,pos,base);
#endif
}

/**
 * Returns the integer equivalent to the given string
 *
 * This function any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to an integer value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the integer equivalent to the given string
 */
int  stoi( const std::wstring& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const wchar_t* start = str.c_str();
    wchar_t* end;
    int result = (int)std::wcstol(start, &end, base);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stoi(str,pos,base);
#endif
}

/**
 * Returns the long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a long value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the long equivalent to the given string
 */
long stol( const std::string& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const char* start = str.c_str();
    char* end;
    long result = std::strtol (start, &end, base);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stol(str,pos,base);
#endif
}

/**
 * Returns the long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a long value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the long equivalent to the given string
 */
long stol( const std::wstring& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const wchar_t* start = str.c_str();
    wchar_t* end;
    long result = std::wcstol(start, &end, base);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stol(str,pos,base);
#endif
}

/**
 * Returns the long long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a long long value.
 *
 * On Android, the only supported values for base are 8, 10, and 16.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the long long equivalent to the given string
 */
long long stoll( const std::string& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::string format = "%lli";
    switch (base) {
        case 8:
            format = "%llo";
            break;
        case 10:
            format = "%lld";
            break;
        case 16:
            format = "%llx";
            break;
    }
    long long result = 0;
    *pos = sscanf(str.c_str(), format.c_str(), &result);
    return result;
#else
    return std::stoll(str,pos,base);
#endif
}

/**
 * Returns the long long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a long long value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the long long equivalent to the given string
 */
long long stoll( const std::wstring& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstring format = L"%lli";
    switch (base) {
        case 8:
            format = L"%llo";
            break;
        case 10:
            format = L"%lld";
            break;
        case 16:
            format = L"%llx";
            break;
    }
    long long result = 0;
    *pos = swscanf(str.c_str(), format.c_str(), &result);
    return result;
#else
    return std::stoll(str,pos,base);
#endif
}

/**
 * Returns the unsigned long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a unsigned long value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the unsigned long equivalent to the given string
 */
unsigned long stoul( const std::string& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const char* start = str.c_str();
    char* end;
    unsigned long result = std::strtoul (start, &end, base);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stoul(str,pos,base);
#endif
}

/**
 * Returns the unsigned long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a unsigned long value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the unsigned long equivalent to the given string
 */
unsigned long stoul( const std::wstring& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const wchar_t* start = str.c_str();
    wchar_t* end;
    unsigned long result = std::wcstoul(start, &end, base);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stoul(str,pos,base);
#endif
}

/**
 * Returns the unsigned long long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a unsigned long long value.
 *
 * On Android, the only supported values for base are 8, 10, and 16.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the unsigned long long equivalent to the given string
 */
unsigned long long stoull( const std::string& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::string format = "%llu";
    switch (base) {
        case 8:
            format = "%llo";
            break;
        case 10:
            format = "%llu";
            break;
        case 16:
            format = "%llx";
            break;
    }
    unsigned long long result = 0;
    *pos = sscanf(str.c_str(), format.c_str(), &result);
    return result;
#else
    return std::stoull(str,pos,base);
#endif
}

/**
 * Returns the unsigned long long equivalent to the given string
 *
 * This function discards any whitespace characters (as identified by calling isspace())
 * until the first non-whitespace character is found, then takes as many characters
 * as possible to form a valid base-n (where n=base) integer number representation
 * and converts them to a unsigned long long value.
 *
 * On Android, the only supported values for base are 8, 10, and 16.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 * @param  base the number base
 *
 * @return the unsigned long long equivalent to the given string
 */
unsigned long long stoull( const std::wstring& str, std::size_t* pos, int base) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::wstring format = L"%llu";
    switch (base) {
        case 8:
            format = L"%llo";
            break;
        case 10:
            format = L"%llu";
            break;
        case 16:
            format = L"%llx";
            break;
    }
    unsigned long long result = 0;
    *pos = swscanf(str.c_str(), format.c_str(), &result);
    return result;
#else
    return std::stoull(str,pos,base);
#endif
}

/**
 * Returns the float equivalent to the given string
 *
 * This function discards any whitespace characters (as determined by std::isspace())
 * until first non-whitespace character is found. Then it takes as many characters as
 * possible to form a valid floating point representation and converts them to a floating
 * point value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 *
 * @return the float equivalent to the given string
 */
float  stof( const std::string& str, std::size_t* pos) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const char* start = str.c_str();
    char* end;
    float result = (float)std::strtod(start, &end);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stof(str,pos);
#endif
}

/**
 * Returns the float equivalent to the given string
 *
 * This function discards any whitespace characters (as determined by std::isspace())
 * until first non-whitespace character is found. Then it takes as many characters as
 * possible to form a valid floating point representation and converts them to a floating
 * point value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 *
 * @return the float equivalent to the given string
 */
float  stof( const std::wstring& str, std::size_t* pos) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const wchar_t* start = str.c_str();
    wchar_t* end;
    float result = (float)std::wcstod(start, &end);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stof(str,pos);
#endif
}

/**
 * Returns the double equivalent to the given string
 *
 * This function discards any whitespace characters (as determined by std::isspace())
 * until first non-whitespace character is found. Then it takes as many characters as
 * possible to form a valid floating point representation and converts them to a floating
 * point value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 *
 * @return the double equivalent to the given string
 */
double stod( const std::string& str, std::size_t* pos) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const char* start = str.c_str();
    char* end;
    double result = std::strtod(start, &end);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stod(str,pos);
#endif
}

/**
 * Returns the double equivalent to the given string
 *
 * This function discards any whitespace characters (as determined by std::isspace())
 * until first non-whitespace character is found. Then it takes as many characters as
 * possible to form a valid floating point representation and converts them to a floating
 * point value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 *
 * @return the double equivalent to the given string
 */
double stod( const std::wstring& str, std::size_t* pos) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const wchar_t* start = str.c_str();
    wchar_t* end;
    double result = std::wcstod(start, &end);
    *pos = (std::size_t)(end-start); // Bad but no alternative on android
    return result;
#else
    return std::stod(str,pos);
#endif
}

/**
 * Returns the long double equivalent to the given string
 *
 * This function discards any whitespace characters (as determined by std::isspace())
 * until first non-whitespace character is found. Then it takes as many characters as
 * possible to form a valid floating point representation and converts them to a floating
 * point value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 *
 * @return the long double equivalent to the given string
 */
long double stold( const std::string& str, std::size_t* pos) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    long double result = 0.0;
    *pos = sscanf(str.c_str(), "%Lg", &result);
    return result;
#else
    return std::stold(str,pos);
#endif
}

/**
 * Returns the long double equivalent to the given string
 *
 * This function discards any whitespace characters (as determined by std::isspace())
 * until first non-whitespace character is found. Then it takes as many characters as
 * possible to form a valid floating point representation and converts them to a floating
 * point value.
 *
 * @param  str  the string to convert
 * @param  pos  address of an integer to store the number of characters processed
 *
 * @return the long double equivalent to the given string
 */
long double stold( const std::wstring& str, std::size_t* pos) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    long double result = 0.0;
    *pos = swscanf(str.c_str(), L"%Lg", &result);
    return result;
#else
    return std::stold(str,pos);
#endif
}

NS_CC_END