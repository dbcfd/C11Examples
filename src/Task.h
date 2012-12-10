#pragma once
#include "Platform.h"

#include <functional>
#include <future>

namespace workers {

class EXAMPLES_LIB_API Task {
public:
    Task();
    virtual ~Task();

    //Get future associated with this task to determine when it completes
    inline std::future<bool> getCompletionFuture();
    //used by workers to perform the functionality of this task (performSpecific)
    void perform(std::function<void(void)> priorToCompleteFunction);
    //used by workers to indicate completion status if they do not call perform
    void setCompletionStatus(const bool status);

protected:
    //abstract method for task functionality, returning true if successful
    virtual bool performSpecific() = 0;

private:
    //promise used to determine when task is finished
    std::promise<bool> mTaskCompletePromise;
};

//inline implementations
//------------------------------------------------------------------------------
std::future<bool> Task::getCompletionFuture()
{
    return mTaskCompletePromise.get_future();
}

}