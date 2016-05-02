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
#ifndef __CU_STRINGS_H__
#define __CU_STRINGS_H__

#include <base/ccMacros.h>

NS_CC_BEGIN

#pragma mark NUMBER TO STRING FUNCTIONS

/** 
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( int value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( long value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( long long value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( unsigned value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( unsigned long value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( unsigned long long value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( float value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( double value );

/**
 * Returns a string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a string equivalent to the given numeric value
 */
std::string CC_DLL to_string( long double value );


#pragma mark -
#pragma mark NUMBER TO WIDE STRING FUNCTIONS

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( int value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( long value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( long long value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( unsigned value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( unsigned long value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( unsigned long long value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( float value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( double value );

/**
 * Returns a wide string equivalent to the given numeric value
 *
 * @param  value    the numeric value to convert
 *
 * @return a wide string equivalent to the given numeric value
 */
std::wstring CC_DLL to_wstring( long double value );


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
int CC_DLL stoi(const std::string& str, std::size_t* pos = 0, int base = 10 );

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
int CC_DLL stoi( const std::wstring& str, std::size_t* pos = 0, int base = 10 );

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
long CC_DLL stol( const std::string& str, std::size_t* pos = 0, int base = 10 );

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
long CC_DLL stol( const std::wstring& str, std::size_t* pos = 0, int base = 10 );

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
long long CC_DLL stoll( const std::string& str, std::size_t* pos = 0, int base = 10 );

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
long long CC_DLL stoll( const std::wstring& str, std::size_t* pos = 0, int base = 10 );

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
unsigned long CC_DLL stoul( const std::string& str, std::size_t* pos = 0, int base = 10 );

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
unsigned long CC_DLL stoul( const std::wstring& str, std::size_t* pos = 0, int base = 10 );

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
unsigned long long CC_DLL stoull( const std::string& str, std::size_t* pos = 0, int base = 10 );

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
unsigned long long CC_DLL stoull( const std::wstring& str, std::size_t* pos = 0, int base = 10 );

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
float CC_DLL stof( const std::string& str, std::size_t* pos = 0 );

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
float CC_DLL stof( const std::wstring& str, std::size_t* pos = 0 );

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
double CC_DLL stod( const std::string& str, std::size_t* pos = 0 );

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
double CC_DLL stod( const std::wstring& str, std::size_t* pos = 0 );

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
long double CC_DLL stold( const std::string& str, std::size_t* pos = 0 );

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
long double CC_DLL stold( const std::wstring& str, std::size_t* pos = 0 );

NS_CC_END

#endif /* __CU_STRINGS_H__ */
