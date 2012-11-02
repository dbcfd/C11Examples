#include "MultiThreading.h"

#include <chrono>

Runnable::Runnable() : mHasRun(false), mValue(0), mShouldIncrement(false)
{

}

void Runnable::run()
{
    mHasRun = true;
}

void Runnable::runWithValue(int value)
{
    mValue = value;
    run();
}

void Runnable::runWithPromise(int value, std::promise<int>* promise)
{
    mValue = value;
    run();
    promise->set_value(value);
}

void Runnable::conditionRun(std::mutex* m)
{
    std::unique_lock<std::mutex> lock(*m);
    //boolean prevens spurious wake ups
    while(!mShouldIncrement)
    {
        mWaiter.wait(lock);
    }
    mShouldIncrement = false;
    ++mValue;

    while(!mShouldIncrement)
    {
        mWaiter.wait(lock);
    }
    ++mValue;
}

void Runnable::incrementValue()
{
    ++mValue;
    run();
}
 
int Runnable::sleepThenIncrement()
{
    std::chrono::seconds dur(1);
    std::this_thread::sleep_for(dur);
    ++mValue;
    run();
    return mValue;
}