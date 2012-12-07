#pragma once
#include "Platform.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>

namespace workers {

class Task;
class Worker;

class EXAMPLES_LIB_API Manager {
public:
    Manager(const size_t nbWorkers);
    ~Manager();

    void run(std::shared_ptr<Task> task);
    void shutdown();
    void waitForTasksToComplete();

    inline const bool isShutdown();
protected:
    void workerDone(Worker* worker);
    void run();

    std::vector< Worker* > mWorkers;

    std::mutex mMutex;
    std::condition_variable mTasksRemovedSignal;

    std::queue< std::shared_ptr<Task> > mTasks;
    std::queue< Worker* > mAvailableWorkers;

    std::atomic<bool> mShutdown;
};

//inline implementations
//------------------------------------------------------------------------------
const bool Manager::isShutdown()
{
    return mShutdown;
}

}