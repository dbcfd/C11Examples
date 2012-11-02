#include "FunctionalProgramming.h"
#include "MultiThreading.h"

#include <algorithm>
#include <vector>
#include <mutex>

#pragma warning(disable:4251)
#include <gtest/gtest.h>

TEST(EXAMPLES_TEST, TEST_FUNCTION)
{
   std::function<bool(int)> nmfObj = nonMemberFunction;

   ASSERT_TRUE(nmfObj(0));

   auto mfObj = std::mem_fn(&SomeClass::memberFunction);

   SomeClass obj;
   ASSERT_STREQ("TRUE", mfObj(obj, 4.5).c_str());

   auto mfObj2 = &SomeClass::memberFunction;
   ASSERT_STREQ("TRUE", mfObj(obj, 4.5).c_str());
}

TEST(EXAMPLES_TEST, TEST_BIND)
{
    ResourceManager mgr;

    auto mfObj = std::mem_fn(&MapLoader::loadResources);

    auto pathBoundFunction = 
        std::bind(mfObj, std::placeholders::_1, std::placeholders::_2, "MyLoadPath");

    MapLoader loader;
    ASSERT_TRUE(pathBoundFunction(&loader, mgr));
}

TEST(EXAMPLES_TEST, TEST_LAMBDA)
{
    std::function<bool(int)> lfObj = [](int x) -> bool { return true; };

    ASSERT_TRUE(lfObj(1));

    bool result = false;

    std::function<bool(int)> lfObj2 = [&result](int x) -> bool { result = true; return result; };

    ASSERT_TRUE(lfObj2(4));

    result = false;
    std::function<bool(int)> lfObj3 = [=](int x) -> bool { return (result == false);};
    result = true;

    ASSERT_TRUE(lfObj3(0));

    std::vector<int> values;
    values.push_back(1);
    values.push_back(2);
    values.push_back(3);

    result = true;
    std::for_each(values.begin(), values.end(), [&result](int x) { result = result && x > 0; });

    ASSERT_TRUE(result);
}

TEST(EXAMPLES_TEST, TEST_THREAD)
{
    {
        Runnable runner;

        auto thread = std::thread(&Runnable::run, &runner);

        thread.join();

        ASSERT_TRUE(runner.hasRun());
        ASSERT_EQ(0, runner.value());
    }

    {
        Runnable runner;

        auto thread = std::thread(&Runnable::runWithValue, &runner, 1);

        thread.join();

        ASSERT_TRUE(runner.hasRun());
        ASSERT_EQ(1, runner.value());
    }

    {
        std::once_flag flag;
        Runnable runner;

        std::function<void()> func = [&] () { runner.incrementValue(); };
        std::function<void()> callOnce = [&] () { std::call_once(flag, func); };
        auto thread1 = std::thread(callOnce);
        auto thread2 = std::thread(callOnce);
        auto thread3 = std::thread(callOnce);

        thread1.join();
        thread2.join();
        thread3.join();

        ASSERT_TRUE(runner.hasRun());
        ASSERT_EQ(1, runner.value());
    }
}

TEST(EXAMPLES_TEST, TEST_FUTURE)
{
    {
        Runnable runner;

        std::promise<int> promise;
        std::future<int> result = promise.get_future();

        auto thread = std::thread(&Runnable::runWithPromise, &runner, 5, &promise);

        result.wait();

        ASSERT_EQ(5, runner.value());
        ASSERT_EQ(runner.value(), result.get());
        ASSERT_TRUE(runner.hasRun());
    }

    {
        Runnable runner;

        std::future<int> result = std::async(std::launch::async, &Runnable::sleepThenIncrement, &runner);

        ASSERT_EQ(0, runner.value());

        result.wait();

        ASSERT_EQ(1, result.get());
        ASSERT_EQ(1, runner.value());
    }

    {
        Runnable runner;

        std::packaged_task<int()> task( [&runner] () -> int { return runner.sleepThenIncrement(); } );
        auto result = task.get_future();
        auto thread = std::thread(std::move(task));

        ASSERT_EQ(0, runner.value());

        result.wait();

        ASSERT_EQ(1, result.get());
        ASSERT_EQ(1, runner.value());
        ASSERT_FALSE(thread.joinable());
    }
}