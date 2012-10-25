#include "Platform.h"

#include <thread>
#include <condition_variable>
#include <atomic>
#include <future>

class EXAMPLES_LIB_API Runnable 
{
public:
    Runnable();

    void run();
    void runWithValue(int value);
    void runWithPromise(int value, std::promise<int> promise);
    void incrementValue();
    int sleepThenIncrement();

    inline bool hasRun() const;
    inline int value() const;

private:
    std::atomic<bool> mHasRun;
    std::atomic<int> mValue;
};

bool Runnable::hasRun() const
{
    return mHasRun;
}

int Runnable::value() const
{
    return mValue;
}
