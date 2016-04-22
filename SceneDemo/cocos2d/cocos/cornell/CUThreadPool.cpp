//
//  CUThreadPool.cpp
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
#include <cocos2d.h>
#include "CUThreadPool.h"

NS_CC_BEGIN

#pragma mark -
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
ThreadPool* ThreadPool::create(int threads) {
    ThreadPool* pool = new (std::nothrow) ThreadPool();
    if (pool && pool->init(threads)) {
        pool->autorelease();
        return pool;
    }
    CC_SAFE_DELETE(pool);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers
/**
 * Initializes a thread pool with no active threads.
 */
ThreadPool::ThreadPool() :_stop(false), _complete(0) { }

/**
 * Deletes this thread pool, destroying all resources.
 *
 * It is a bad idea to destroy the thread pool if the pool is not yet shut down.
 * The task queue is shared by the child threads, so we cannot delete it until
 * all the threads complete.  This destructor will block unti showndown.
 */
ThreadPool::~ThreadPool() {
    stop();
    while (!isShutdown());
}

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
bool ThreadPool::init(int threads) {
    for (int index = 0; index < threads; ++index) {
        _workers.emplace_back(std::thread(std::bind(&ThreadPool::threadFunc, this)));
    }
    return true;
}


#pragma mark -
#pragma mark Thread Execution

/** The body function of a single thread; it pulls tasks from the task queue */
void ThreadPool::threadFunc() {
    while (!_stop) {
        std::function<void()> task = nullptr;
        {   // Lock for save queue access
            std::unique_lock<std::mutex> lk(_queueMutex);
            if (_stop) {
                break;
            }
            // Pull the next task off the queue
            if (!_taskQueue.empty()) {
                task = std::move(_taskQueue.front());
                _taskQueue.pop();
            } else {
                _taskCondition.wait(lk);
                continue;
            }
        }
        // Perform the current task
        task();
    }
    _complete++;
}


#pragma mark -
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
void ThreadPool::addTask(const std::function<void()> &task){
    std::unique_lock<std::mutex> lk(_queueMutex);
    _taskQueue.emplace(task);
    _taskCondition.notify_one();
}


/**
 * Stop the thread pool, marking it for shut down.
 *
 * A stopped thread pool is marked for shutdown, but it shutdown has not necessarily
 * completed.  Shutdown will be complete when the current child threads have
 * finished with their tasks.
 */
void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lk(_queueMutex);
        _stop = true;
        _taskCondition.notify_all();
    }
    
    for (auto&& worker : _workers) {
        worker.join();
    }
}


NS_CC_END
