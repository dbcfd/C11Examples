#include "Worker.h"
#include "Task.h"

namespace workers {

//------------------------------------------------------------------------------
Worker::Worker(std::function<void (Worker*)> taskCompleteFunction) : mShutdown(false), mTaskCompleteFunction(taskCompleteFunction), mHasTask(false)
{
    mReadyForWorkFuture = mReadyForWorkPromise.get_future();
    mThread = std::unique_ptr<std::thread>(new std::thread(std::bind(&Worker::run, this)));
}

//------------------------------------------------------------------------------
Worker::~Worker()
{
    shutdown();
}

//------------------------------------------------------------------------------
void Worker::shutdown()
{
    bool wasShutdown = !(mShutdown.exchange(true));
    
    if(wasShutdown)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mHasTask = true;
        }
        mWaitingForTask.notify_all();
        mThread->join();
    }

    if(0 != mRunningTask)
    {
        mRunningTask->setCompletionStatus(false);
    }
}

//------------------------------------------------------------------------------
void Worker::runTask(std::shared_ptr<Task> task)
{
    if(!isShutdown())
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mRunningTask = task;
            mHasTask = true;
        }

        mWaitingForTask.notify_all();
    }
    else if(task != 0)
    {
        task->setCompletionStatus(false);
    }
}

//------------------------------------------------------------------------------
void Worker::run()
{
    mReadyForWorkPromise.set_value(true);

    while(!isShutdown())
    {
        std::shared_ptr<Task> taskToRun;
        {
            std::unique_lock<std::mutex> lock(mMutex);

            while(!mHasTask)
            {
                mWaitingForTask.wait(lock);
            }

            taskToRun.swap(mRunningTask);
            mHasTask = false;
        }

        if(taskToRun != 0)
        {
            taskToRun->perform([this]()->void { this->mTaskCompleteFunction(this); });
        }
    }
}

}
