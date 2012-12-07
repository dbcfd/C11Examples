#include "Task.h"

namespace workers {

//------------------------------------------------------------------------------
Task::Task()
{

}

//------------------------------------------------------------------------------
Task::~Task()
{
    try {
        //attempt to fail the task, which sets the future to false
        failToPerform();
    }
    catch(std::future_error&)
    {
        //task was successfully performed already, so we can't set value to false
    }
}

//------------------------------------------------------------------------------
void Task::failToPerform()
{
    mTaskCompletePromise.set_value(false);
}

//------------------------------------------------------------------------------
void Task::perform(std::function<void(void)> completeFunction)
{
    performSpecific();
    completeFunction();
    mTaskCompletePromise.set_value(true);
}

}
