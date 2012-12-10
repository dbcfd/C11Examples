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
    //Constructor, saying how many workers are available
    Manager(const size_t nbWorkers);
    ~Manager();

    //Run a task. Run on the next available worker, queued until worker available
    void run(std::shared_ptr<Task> task);
    //Stop all workers, prevent tasks from being run
    void shutdown();
    //Wait for all tasks that are queued/running to complete
    void waitForTasksToComplete();

    inline const bool isShutdown();
protected:
    //Our set of workers
    std::vector< Worker* > mWorkers;

    //Mutex for tasks and signalling
    std::mutex mMutex;
    std::condition_variable mTasksRemovedSignal;

    //Queue for tasks, added to when workers not available
    std::queue< std::shared_ptr<Task> > mTasks;
    //Queue for workers that are waiting to receive a task
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