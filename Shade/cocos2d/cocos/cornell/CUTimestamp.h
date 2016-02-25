//
//  CUTimestamp.h
//  Cornell Extensions to Cocos2D
//
//  This module simplifies timestamp support a bit.  Accurate timestamp support
//  is necessary for touch and mouse support, but timestamps requires some really
//  arcane C++11.
//
//  Author: Walker White
//  Version: 12/5/15
//
#ifndef __CU_TIMESTAMP_H__
#define __CU_TIMESTAMP_H__

#include <base/ccMacros.h>

NS_CC_BEGIN

/** Data type for timestamp support */
typedef std::chrono::high_resolution_clock::time_point timestamp_t;

/** 
 * Returns the current timestamp
 *
 * @return the current timestamp
 */
inline timestamp_t current_time() { return std::chrono::high_resolution_clock::now(); }

/**
 * Returns the elapsed time in milliseconds.
 *
 * @param  start    the start time
 * @param  end      the end time
 *
 * @return the elapsed time in milliseconds.
 */
inline long elapsed_millis(timestamp_t start, timestamp_t end) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    return (long)elapsed.count();
}

/**
 * Returns the elapsed time in microseconds.
 *
 * @param  start    the start time
 * @param  end      the end time
 *
 * @return the elapsed time in microseconds.
 */
inline long elapsed_micros(timestamp_t start, timestamp_t end) {
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    return (long)elapsed.count();
}

/**
 * Returns the elapsed time in nanoseconds.
 *
 * @param  start    the start time
 * @param  end      the end time
 *
 * @return the elapsed time in nanoseconds.
 */
inline long elapsed_nanos(timestamp_t start, timestamp_t end) {
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
    return (long)elapsed.count();
}

NS_CC_END
#endif /* defined(__CU_TIMESTAMP_H__) */

