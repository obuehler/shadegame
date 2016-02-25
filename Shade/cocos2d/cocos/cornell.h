//
//  cornell.h
//  Cornell Extensions to Cocos2D
//
//  This header includes all of the Cornell Extensions
//
//  Author: Walker White
//  Version: 12/10/15
//
#ifndef __CORNELL_H__
#define __CORNELL_H__

// Math primitives
#include "cornell/CUPoly2.h"
#include "cornell/CUPolynomial.h"
#include "cornell/CUCubicSpline.h"

// Utilities
#include "cornell/CUTimestamp.h"
#include "cornell/CUThreadPool.h"
#include "cornell/CUStrings.h"

// Scene graph
#include "cornell/CUTexturedNode.h"
#include "cornell/CUWireNode.h"
#include "cornell/CUPolygonNode.h"
#include "cornell/CUPathNode.h"
#include "cornell/CUAnimationNode.h"
#include "cornell/CURootLayer.h"

// Physics management
#include "cornell/CUObstacle.h"
#include "cornell/CUSimpleObstacle.h"
#include "cornell/CUComplexObstacle.h"
#include "cornell/CUBoxObstacle.h"
#include "cornell/CUWheelObstacle.h"
#include "cornell/CUPolygonObstacle.h"
#include "cornell/CUWorldController.h"
#include "cornell/CUObstacleSelector.h"

// Asset management
#include "cornell/CUJSONReader.h"
#include "cornell/CUAssetManager.h"
#include "cornell/CUSceneManager.h"
#include "cornell/CUSoundLoader.h"
#include "cornell/CUTextureLoader.h"
#include "cornell/CUFontLoader.h"
#include "cornell/CUSoundEngine.h"
#include "cornell/CUSound.h"
#include "cornell/CUTTFont.h"
#include "cornell/CUAsset.h"

// Input handlers
#include "cornell/CUKeyboardPoller.h"
#include "cornell/CUAccelerationPoller.h"
#include "cornell/CUMouseListener.h"
#include "cornell/CUTouchListener.h"
#include "cornell/CUMultiTouchListener.h"

// These are templates and should be included explicitly
//#include "cornell/CUFreeList.h"
//#include "cornell/CUGreedyFreeList.h"
//#include "cornell/CULoader.h"
//#include "cornell/CUAssetLoader.h"

#endif /* defined(__CORNELL_H__) */

