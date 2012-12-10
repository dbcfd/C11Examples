#include "Task.h"

namespace workers {

//------------------------------------------------------------------------------
Task::Task()
{

}

//------------------------------------------------------------------------------
Task::~Task()
{
    try
    {
        setCompletionStatus(false);
    }
    catch(std::future_error&)
    {
        //task was already completed by perform
    }
}

//------------------------------------------------------------------------------
void Task::setCompletionStatus(const bool status)
{
    mTaskCompletePromise.set_value(status);
}

//------------------------------------------------------------------------------
void Task::perform(std::function<void(void)> completeFunction)
{
    bool result = performSpecific();
    completeFunction();
    setCompletionStatus(result);
}

}
