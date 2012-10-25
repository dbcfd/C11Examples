#include "MultiThreading.h"

#include <chrono>

Runnable::Runnable() : mHasRun(false), mValue(0)
{

}

void Runnable::run()
{
    mHasRun = true;
}

void Runnable::runWithValue(int value)
{
    mValue = value;
}

void Runnable::runWithPromise(int value, std::promise<int> promise)
{
    mValue = value;
    promise.set_value(value);
}

void Runnable::incrementValue()
{
    ++mValue;
}
 
int Runnable::sleepThenIncrement()
{
    std::chrono::seconds dur(5);
    std::this_thread::sleep_for(dur);
    ++mValue;
    return mValue;
}