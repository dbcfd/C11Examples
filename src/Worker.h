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
    Worker(std::function<void (Worker*)> taskCompleteFunction);
    virtual ~Worker();

    void runTask(std::shared_ptr<Task> task);
    void shutdown();

    inline void waitUntilReady();
    inline const bool isShutdown();
private:
    void run();
    void completeTask();

    std::unique_ptr<std::thread> mThread;
    std::promise<bool> mReadyForWorkPromise;
    std::future<bool> mReadyForWorkFuture;
    std::mutex mMutex;
    std::shared_ptr<Task> mRunningTask;
    std::condition_variable mWaitingForTask;
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