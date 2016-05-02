//
//  SCProgressBar.h
//  SceneDemo
//
//  This module provides a simple progress bar that is defined by a single
//  texture atlas.
//
//  Author: Walker White
//  Version: 4/2/16
//
#ifndef __SC_PROGRESS_BAR_H__
#define __SC_PROGRESS_BAR_H__

#include <cocos2d.h>
#include <cornell/CUPolygonNode.h>

/**
 * Provides a simple progress bar that grows to fill a rounded rectangle.
 *
 * The progress bar is defined by a texture atlas.  The atlas is two equal
 * width shapes, one atop the other, and separated by a one-pixel space (to
 * prevent bleeding).  The top shape is the background.  The bottom shape
 * is a three-patch that grows as progress completes
 */
class ProgressBar : public cocos2d::Node {
private:
    /** The dimensions of the background image */
    cocos2d::Size _fullsize;
    /** The dimenstions of the end caps of the three patch */
    cocos2d::Size _capsize;
    /** The bounding rectangle of the interior shape */
    cocos2d::Rect _expanse;
    
    /** The node for the foreground image */
    cocos2d::PolygonNode* _background;
    /** The node for the starting cap of the progress bar */
    cocos2d::PolygonNode* _start;
    /** The node for the interior portion of the progress bar */
    cocos2d::PolygonNode* _interior;
    /** The node for the end cap of the progress bar */
    cocos2d::PolygonNode* _finish;
    
    /** The amount of progress (a number in the range 0..1) made so far */
    float _progress;
    
public:
    /**
     * Creates a new progress bar for the given atlas.
     *
     * The atlas must consist of two equal size rounded rectangles of
     * equal with.  They should be stacked on top of each other and 
     * separated by a pixel to prevent bleeding.
     *
     * @param atlas the texture atlas
     *
     * @return  An autoreleased progress bar
     */
    static ProgressBar* create(cocos2d::Texture2D* atlas);
    
    /**
     * Returns the current progress of this bar
     *
     * The progress is always a value between 0 (no progress) and 1
     * (completed progress)
     *
     * @return the current progress of this bar
     */
    float getProgress() const { return _progress; }
    
    /**
     * Sets the current progress of this bar
     *
     * The progress is always a value between 0 (no progress) and 1
     * (completed progress).  If the value is outside that range, this
     * method will clamp it.
     *
     * Calling this method will update the child nodes to correctly
     * display the progress.
     *
     * @value the current progress of this bar
     */
    void setProgress(float value);

    
CC_CONSTRUCTOR_ACCESS:
    /** Creates a new uninitialized Progress Bar */
    ProgressBar();
    
    /**
     * Initializes a new progress bar for the given atlas.
     *
     * The atlas must consist of two equal size rounded rectangles of
     * equal with.  They should be stacked on top of each other and
     * separated by a pixel to prevent bleeding.
     *
     * @param atlas the texture atlas
     *
     * @return  true if the progress bar initialized properly, false otherwise.
     */
    bool init(cocos2d::Texture2D* atlas);

};

#endif /* SCProgressBar_hpp */
