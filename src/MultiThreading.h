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
    void runWithPromise(int value, std::promise<int>* promise);
    void conditionRun(std::mutex* m);
    void incrementValue();
    int sleepThenIncrement();

    inline bool hasRun() const;
    inline int value() const;
    inline std::condition_variable& getWaiter();
    inline void shouldIncrement();

private:
    Runnable(const Runnable&);

    std::atomic<bool> mHasRun;
    std::atomic<int> mValue;
    std::condition_variable mWaiter;
    bool mShouldIncrement;
};

bool Runnable::hasRun() const
{
    return mHasRun;
}

int Runnable::value() const
{
    return mValue;
}

std::condition_variable& Runnable::getWaiter()
{
    return mWaiter;
}

void Runnable::shouldIncrement()
{
    mShouldIncrement = true;
}
