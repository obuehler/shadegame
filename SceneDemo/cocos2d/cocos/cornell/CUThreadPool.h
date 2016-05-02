//
//  CUThreadPool.h
//  Cornell Extensions to Cocos2D
//
//  Module for a pool of threads capable of executing asynchronous tasks.  Each task
//  is specified by a void function.  There are no guarantees about thread safety;
//  that is responsibility of the author of each task.
//
//  This code is largely taken from the Cocos2d file AudioEngine.cpp, from the code
//  for asynchronous asset loading.  There was no need to couple this tool with that
//  class, so we have pulled it out (with some notable safety changes) into a general
//  purpose tool.
// 
//  Author: Walker White
//  Version: 12/3/15
//
#ifndef __CU_THREAD_POOL_H__
#define __CU_THREAD_POOL_H__

#include <stdio.h>
#include <condition_variable>
#include <queue>
#include <vector>
#include <base/ccMacros.h>

NS_CC_BEGIN

#pragma mark -
#pragma mark Thread Pool

/**
 *  Class to providing a collection of worker threads.
 *
 *  This is a general purpose class for performing tasks asynchronously.  There is 
 *  no notification process for when a task is complete.  Instead, your task should
 *  either set a flag, or execute a callback when it is done.
 *
 *  This code is largely taken from the Cocos2d file AudioEngine.cpp, from the code
 *  for asynchronous asset loading.  We have made some safety changes.  In particular,
 *  stopping a thread pool does not shut it down immediately; it just marks it for
 *  shutdown.  Because of mutex locks, it is not safe to delete a thread pool until
 *  it is completely shutdown.
 *
 *  More importantly, we no longer allow for detached threads.  This makes no sense
 *  in this application, because the threads share a resource (_taskQueue) with the
 *  main thread that will be deleted.  It is therefore unsafe for the threads to
 *  ever detach. I have no idea who thought this was a good idea in AudioEngine.cpp.
 *
 *  See the class CUAssetManager for an example of how to use a thread pool.
 */
class CC_DLL ThreadPool : public Ref {
private:
    /** This macro disables the copy constructor (not allowed on thread pools) */
    CC_DISALLOW_COPY_AND_ASSIGN(ThreadPool);

protected:
    /** The individual worker threads for this thread pool */
    std::vector<std::thread>  _workers;
    /** Tasks waiting to be assigned to a thread */
    std::queue< std::function<void()> > _taskQueue;
    
    /** A mutex lock for the task queue */
    std::mutex _queueMutex;
    /** A condition variable to manage tasks waiting for a worker */
    std::condition_variable _taskCondition;
    
    /** Whether or not to detach the threads */
    bool _detach;
    /** Whether or not the thread pool has been marked for shutdown */
    bool _stop;
    /** The number of child threads that are completed */
    int _complete;
    
    /** The body function of a single thread; it pulls tasks from the task queue */
    void threadFunc();
    
    
public:
#pragma mark Static Constructors
    /**
     * Creates a thread pool with the given number of threads.
     *
     * You can specify the number of simultaneous worker threads.  4 is generally
     * a good number, even if you have a lot of tasks.  Much more than the number
     * of cores on a machine is counter-productive.
     *
     * @param  threads  the number of threads in this pool
     *
     * @return  An autoreleased thread pool
     */
    static ThreadPool* create(int threads = 4);
    
    
#pragma mark Task Management
    /**
     * Adds a task to the thread pool.
     *
     * A task is a void returning function with no parameters.  If you need state in the
     * task, you should use a method call for the state.  The task will not be executed
     * immediately, but must wait for the first available worker.
     *
     * @param  task     the task function to add to the thread pool
     */
    void addTask(const std::function<void()> &task);
    
    /**
     * Stop the thread pool, marking it for shut down.
     *
     * A stopped thread pool is marked for shutdown, but it shutdown has not necessarily
     * completed.  Shutdown will be complete when the current child threads have
     * finished with their tasks.
     */
    void stop();
    
    /**
     * Returns whether the thread pool has been stopped.
     *
     * A stopped thread pool is marked for shutdown, but it shutdown has not necessarily
     * completed.  Shutdown will be complete when the current child threads have
     * finished with their tasks.
     *
     * @return whether the thread pool has been stopped.
     */
    bool isStopped() const { return _stop; }
    
    /**
     * Returns whether the thread pool has been shut down.
     *
     * A shut down thread pool has no active threads and is safe for deletion.
     *
     * @return whether the thread pool has been shut down.
     */
    bool isShutdown() const { return _workers.size() == _complete; }

    
#pragma mark -
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Initializes a thread pool with no active threads.
     */
    ThreadPool();
    
    /**
     * Deletes this thread pool, destroying all resources.
     *
     * It is a bad idea to destroy the thread pool if the pool is not yet shut down.
     * The task queue is shared by the child threads, so we cannot delete it until
     * all the threads complete.  This destructor will block unti showndown.
     */
    ~ThreadPool();
    
    /**
     * Initializes a thread pool with the given number of threads.
     *
     * You can specify the number of simultaneous worker threads.  4 is generally
     * a good number, even if you have a lot of tasks.  Much more than the number
     * of cores on a machine is counter-productive.
     *
     * @param  threads  the number of threads in this pool
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(int threads = 4);
    
};

NS_CC_END
#endif /* defined(__CU_THREAD_POOL_H__) */



