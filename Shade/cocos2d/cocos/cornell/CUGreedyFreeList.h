//
//  CUGreedyFreeList.h
//  Cornell Extensions to Cocos2D
//
//  This header provides a template for a free list subclass. This version of free
//  is very aggressive at recycling memory.  It is not expandable, and never allocates
//  memory beyond the preallocated capacity.  Instead, if you attempt to allocate
//  beyond the capacity, it will immediately recycle the oldest allocated object,
//  even if it is not freed.
//
//  This sounds a bit unsafe. In order to use it safely, object pointers have to
//  be prepared to be working with a reset object at any given time.
//
//  This is not a class.  It is a class template.  Templates do not have cpp files.
//  They only have a header file.  When you include the header, it compiles the specific
//  template used by your program. Hence all of the code for this templated class is
//  in this header.
//
//  Author: Walker White
//  Version: 11/15/15
//
#ifndef __CU_GREEDY_LIST_H__
#define __CU_GREEDY_LIST_H__

#include "CUFreeList.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark GreedyFreeList Template

/**
 * Template for a free list class with aggressive recycling.
 *
 * This free list is not expandable, and never allocates memory beyond the preallocated 
 * capacity.  Instead, if you attempt to allocate beyond the capacity, it will immediately 
 * recycle the oldest allocated object, even if it is not freed.
 * 
 * This sounds a bit unsafe. In order to use it safely, object pointers have to be 
 * prepared to be working with a reset object at any given time.  In particular, it is
 * designed for particle systems, where the particles are managed by a set that does
 * not permit duplicates.  That way, an allocation of a forceably recycled object
 * will only appear once in the list.
 *
 * In order to work properly, the objects allocated must be a class with the method
 *
 *    void reset();
 *
 * The class does not have to formally subclass anything or implement an interface 
 * (C++ does not work that way). It just has to have this method. In addition, the 
 * class must have a default constructor with no arguments.  You should have an init() 
 * method if you need to initialize the object after allocation.
 *
 * This class owns all memory that it allocates.  When the free list is deleted, all
 * of the objects that it allocated will be deleted also.
 */
template <class T>
class GreedyFreeList : public FreeList<T> {
protected:
    /** Tracks all of the memory that has been allocated, allowing forceable recycling */
    std::queue<T*> _allocation;
    
public:
    /**
     * Creates a new free list with the given capacity.
     *
     * As greedy free lists are not expandable, the capacity must be non-zero.
     *
     * @param  capacity the number of objects to preallocate
     */
    GreedyFreeList(size_t capacity) : FreeList<T>(capacity, false) {
        CCASSERT(capacity > 0, "Greedy free list needs non-zero capacity");
    }
    
    /**
     * Deletes this free list, releasing all memory.
     *
     * A free list is the owner of all memory it allocates.  Any object allocated by
     * this free list will be deleted and unsafe to access.
     */
    ~GreedyFreeList() { }
    
    
#pragma mark Memory Managment
    /**
     * Returns a pointer to a newly allocated T object.
     *
     * If there are any objects on the free list, it will recycle them.  Next, if
     * there are any preallocated objects, it will use one of those.  Finally, it
     * will forceably recycle the oldest allocated object.
     *
     * @return a pointer to a newly allocated T object.
     */
    virtual T* alloc() override;
};


#pragma mark -
#pragma mark Method Implementations

/**
 * Returns a pointer to a newly allocated T object.
 *
 * If there are any objects on the free list, it will recycle them.  Next, if
 * there are any preallocated objects, it will use one of those.  Finally, it
 * will forceably recycle the oldest allocated object.
 *
 * @return a pointer to a newly allocated T object.
 */
template <class T>
T* GreedyFreeList<T>::alloc() {
    T* result = FreeList<T>::alloc();
    if (result == nullptr && !(_allocation.empty())) {
        result = _allocation.front();
        _allocation.pop();
        result->reset();
        _allocation.push(result);
    } else if (result != nullptr) {
        _allocation.push(result);
    }
    return result;
}
NS_CC_END

#endif /* defined(__CU_GREEDY_LIST_H__) */
