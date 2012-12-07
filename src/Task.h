#pragma once
#include "Platform.h"

#include <functional>
#include <future>

namespace workers {

class EXAMPLES_LIB_API Task {
public:
    Task();
    virtual ~Task();

    inline std::future<bool> getCompletionFuture();

    void perform(std::function<void(void)> priorToCompleteFunction);
    void failToPerform();

protected:
    virtual void performSpecific() = 0;

    std::promise<bool> mTaskCompletePromise;
};

//inline implementations
//------------------------------------------------------------------------------
std::future<bool> Task::getCompletionFuture()
{
    return mTaskCompletePromise.get_future();
}

}