#pragma once
#include "Platform.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <thread>

namespace workers {

class Task;

class EXAMPLES_LIB_API Worker {
public:
    //Constructor, takes a function to call every time worker has completed a task
    Worker(std::function<void (Worker*)> taskCompleteFunction);
    virtual ~Worker();

    //Set the task for this worker to run
    void runTask(std::shared_ptr<Task> task);
    //Stop worker thread. Worker can no longer run tasks
    void shutdown();

    //Wait until the worker has started up and is ready to accept tasks
    inline void waitUntilReady();
    inline const bool isShutdown();
private:
    //Entry point for our thread
    void run();

    //thread for work
    std::unique_ptr<std::thread> mThread;
    //promise and future used to know when our worker has entered its work thread and is ready for tasks
    std::promise<bool> mReadyForWorkPromise;
    std::future<bool> mReadyForWorkFuture;
    //mutex to allow changing the task to run, done when setting a new task to run
    std::mutex mMutex;
    std::shared_ptr<Task> mRunningTask;
    //signal to know when a task is available
    std::condition_variable mWaitingForTask;
    //function to call after we finish with a task
    std::function<void (Worker*)> mTaskCompleteFunction;
    std::atomic<bool> mShutdown;
    bool mHasTask;
};

//inline implementations
//------------------------------------------------------------------------------
void Worker::waitUntilReady()
{
     mReadyForWorkFuture.wait();
}

//------------------------------------------------------------------------------
const bool Worker::isShutdown()
{
    return mShutdown;
}

}