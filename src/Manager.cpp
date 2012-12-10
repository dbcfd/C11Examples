#include "Manager.h"
#include "Worker.h"
#include "Task.h"

#include <functional>

namespace workers {

//------------------------------------------------------------------------------
Manager::Manager(const size_t nbWorkers) : mShutdown(false)
{
    mWorkers.reserve(nbWorkers);

    for(size_t workerIdx = 0; workerIdx < nbWorkers; ++workerIdx)
    {
        Worker* worker = new Worker([this](Worker* worker) -> void {
            //grab the next task if available, otherwise add our worker to a wait list
            if(!this->isShutdown())
            {
                std::shared_ptr<Task> task;
                {
                    std::unique_lock<std::mutex> lock(mMutex);

                    if(this->mTasks.empty())
                    {
                        this->mAvailableWorkers.push(worker);
                    }
                    else
                    {
                        //task available, run it
                        task.swap(mTasks.front());
                        this->mTasks.pop();
                        this->mTasksRemovedSignal.notify_all();
                    }
                }
                if(0 != task)
                {
                    worker->runTask(task);
                }
            }
        } );
        mWorkers.push_back(worker);
        mAvailableWorkers.push(worker);
    }

    for(std::vector< Worker* >::iterator worker = mWorkers.begin(); worker != mWorkers.end(); ++worker)
    {
        (*worker)->waitUntilReady();
    }
}

//------------------------------------------------------------------------------
Manager::~Manager()
{
    shutdown();
}

//------------------------------------------------------------------------------
void Manager::shutdown()
{
    bool wasShutdown = !(mShutdown.exchange(true));

    if(wasShutdown)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            {
                std::queue<Worker*> empty;
                std::swap(empty, mAvailableWorkers);
            }

            {
                std::queue< std::shared_ptr<Task> > empty;
                std::swap(empty, mTasks);
                mTasksRemovedSignal.notify_all();
            }
        }

        for(std::vector< Worker* >::iterator worker = mWorkers.begin(); worker != mWorkers.end(); ++worker)
        {
            (*worker)->shutdown();
            delete (*worker);
        }

        mWorkers.clear();
    }
}

//------------------------------------------------------------------------------
void Manager::waitForTasksToComplete()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while(!mTasks.empty())
    {
        mTasksRemovedSignal.wait(lock);
    }
}

//------------------------------------------------------------------------------
void Manager::run(std::shared_ptr<Task> task)
{
    //we want to run this task in a worker if one is available, else, add it to a queue
    if(!isShutdown())
    {
        Worker* worker = 0;
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if(mAvailableWorkers.empty())
            {
                //no workers available, queue the task
                mTasks.push(task);
            }
            else
            {
                //worker available, grab it, and run task
                worker = mAvailableWorkers.front();
                mAvailableWorkers.pop();
            }
        }
        if(0 != worker)
        {
            worker->runTask(task);
        }
    }
    else if(task != 0)
    {
        task->setCompletionStatus(false);
    }
}

}
