#include "FunctionalProgramming.h"
#include "MultiThreading.h"

#include <algorithm>
#include <vector>
#include <mutex>

void example_std_function() {
    //create a function object utilizing the function bool nonMemberFunction(int)
    //this function was defined in FunctionalProgramming.h
    std::function<bool(int)> nmfObj = nonMemberFunction;

    //invoke the function object at a later time with some argument
    //invoking the object returns a value to us
    bool returnValue = nmfObj(1);

    //now we will create an object to a member function
    //auto is used so we don't have to explicitly define the types of the function
    auto mfObj = std::mem_fn(&SomeClass::memberFunction);

    //even though we have an object representing the member function, we
    //still need an object to call the member function on
    SomeClass obj;
    //now we can invoke the member function on that object with arguments
    //the first argument to a member function object must be the object
    std::string anotherReturnValue = mfObj(obj, 4.5);

    //we can also create a member function object without using std::mem_fn
    auto mfObj2 = &SomeClass::memberFunction;
}

void example_bind() {
    //standard bind allows us to attach some or all arguments to a function object so
    //that we may call it later without needing all arguments
    //again, start by creating a function object to a member function
    //defined in FunctionalProgramming.h, bool loadResources(const ResourceManager&, const std::string&)
    auto mfObj = std::mem_fn(&MapLoader::loadResources);

    //now, we have one of the arguments that we want to use (the path to the resources), but not the object to call
    //the member function on, or the resource manager. Placeholder objects (std::placeholders) are used so that we
    //can wait to define the MapLoader and ResourceManager
    auto pathBoundFunction = 
        std::bind(mfObj, std::placeholders::_1, std::placeholders::_2, "MyLoadPath");

    //now we know our ResourceManager and our MapLoader, we can invoke our loadResources function, that already
    //knows the path we want to load from
    ResourceManager mgr;
    MapLoader loader;
    bool returnValue = pathBoundFunction(&loader, mgr);
}

void example_lambda() 
{
    //lambdas are anonymouse functions (they are defined inline without a name/identifier)
    //they can take parameters from outside their scope, arguments, and return values

    //here, we're defining a lambda which doesn't use parameters from outside the scope (empty capture []),
    //but takes an int as an argument and returns a boolean. Again, we're using a function object to allow
    //the lambda to be invoked
    std::function<bool(int)> lfObj = [](int x) -> bool { return true; };

    //at a later time, we can use our lambda function with arguments, and the lambda will be evaluated
    //returning a boolean result
    bool returnValue = lfObj(1);

    //lambdas can also "capture" parameters from the scope in which they are defined for later us
    //the lambda capture is indicated by [], and can capture by reference (&) or value (=). If captured
    //by value, the captured values cannot be altered without defining the capture as mutable
    //here, we have a boolean outside of the scope that we are capturing in the lambda. Since it is
    //by reference, we can check and modify its value
    bool result = false;
    std::function<bool(int)> lfObj2 = [&result](int x) -> bool { result = true; return result; };

    bool anotherReturnValue = lfObj2(4);
    //at this point, result is true

    //when we capture by reference, we cannot modify the values (unless we use mutable) but we
    //can read their values and use them within the lambda logic
    result = false;
    std::function<bool(int)> lfObj3 = [=](int x) -> bool { return (result == false);};
    result = true;
    bool yetAnotherReturnValue = lfObj3(0);

    std::vector<int> values;
    values.push_back(1);
    values.push_back(2);
    values.push_back(3);

    //a number of algorithms can also take a lambda as an argument, providing greater control
    //over what the algorithm does
    //here, we're using std::for_each to loop over a vector, with a lamda that captures a boolean
    //that functions as an accumulator
    result = true;
    std::for_each(values.begin(), values.end(), [&result](int x) { result = result && x > 0; });
    //result should continue to be true now, if one of the values was less than or equal to 0, result would be false
}

void example_thread()
{
    //C++11 also provides a basic low level threading api
    {
        //Runnable is defined in MultiThreading.h. It provides a number of methods that can be called which
        //perform some behavior. It does not inherit from any special classes.
        Runnable runner;

        //std::thread is used to create an object which represents a thread of execution. It will execute the method
        //that it is given
        //here, we're creating a thread that executes the run method of a Runnable object
        auto thread = std::thread(&Runnable::run, &runner);

        //wait for the thread to complete execution
        thread.join();

        //check a value of the object
        //this is a much less functional approach, and very low level, since we're specifying the thread
        //and manipulating the state of an object
        bool hasRun = runner.hasRun();
    }

    {
        Runnable runner;

        //invoke a thread on a different Runnable method, this time with an argument to the method
        auto thread = std::thread(&Runnable::runWithValue, &runner, 1);

        //again, we wait for completion
        thread.join();

        //we've now modified the value from 0 to 1
        int returnValue = runner.value();
    }

    {
        Runnable runner;

        //std::call_once is used similar to mutexes, in an attempt to guarantee that a function runs only
        //once. Subsequent attempts to run a function will not occur
        //std::call_once requires access to a std::once_flag
        std::once_flag flag;

        //define our function that we want to call only once, even if multiple threads attempt to call it
        //we are wrapping the function in a lambda since std::call_once required a void() function
        std::function<void()> func = [&] () { runner.incrementValue(); };
        //use call_once to prevent multiple calls. We are using capture all by reference [&] to grab the 
        //oncee_flag and pass it to call_one
        std::function<void()> callOnce = [&] () { std::call_once(flag, func); };
        //invoke multiple threads attempting to call our function
        auto thread1 = std::thread(callOnce);
        auto thread2 = std::thread(callOnce);
        auto thread3 = std::thread(callOnce);

        //wait for all of them to complete
        thread1.join();
        thread2.join();
        thread3.join();

        //only one increment should have occurred, taking us from 0 to 1
        bool returnShouldBe1 = runner.value();
    }
}

void example_future()
{
    {
        //Runnable is defined in MultiThreading.h. It provides a number of methods that can be called which
        //perform some behavior. It does not inherit from any special classes.
        Runnable runner;

        //promises and futures are used to define some action that will return a result at some point in 
        //the future. A promise is often defined, and the result passed to the promise is determined
        //in another thread. A thread can then wait (block) on the result of the promise, usually
        //after it has performed some work of its own
        //here we define a promise, and get the future that results from that promise
        std::promise<int> promise;
        std::future<int> result = promise.get_future();

        //now I'm creating a thread which will run a function, which takes that promise as its argument
        //eventually, the promise will have a value and the future result will be ready
        auto thread = std::thread(&Runnable::runWithPromise, &runner, 5, &promise);

        //here we wait on the result to be ready
        result.wait();

        //the result is now ready to get
        int futureResult = result.get();

        //the thread is complete, so there should be no waiting associated with this join
        thread.join();
    }

    {
        Runnable runner;

        //std::async provides an interface to obtain a future and simultaneously launch a thread that
        //calls a function to fulfill that future by the result of its return
        //here, we're calling the sleepThenIncrement function which returns in an int on completion
        //std::async automatically runs sleepThenIncrement in another function, and we receive a future
        //that we can wait for the result of
        std::future<int> result = std::async(std::launch::async, &Runnable::sleepThenIncrement, &runner);

        int runnerValueIs0 = runner.value();

        //wait for the future
        result.wait();

        //the other thread has now slept and incremented
        int runnerValueIs1 = runner.value();
    }

    {
        Runnable runner;

        //packaged task is another way to retrieve a future, using a function object or lambda
        //packaged task does not automatically run the task in another thread, but is easily compatible
        //with a move to a thread
        //here, we're packaging a task that captures a defined Runnable, runs its sleepThenIncrement,
        //and returns the result
        std::packaged_task<int()> task( [&runner] () -> int { return runner.sleepThenIncrement(); } );
        //once we have a task, we can get a future for the result of its completed calculation
        auto result = task.get_future();
        //now we can run that task in another thread using move semantics
        auto thread = std::thread(std::move(task));

        int runnerHasValue0 = runner.value();

        //wait for the computation to complete
        result.wait();

        //task has completed, value has been incremented
        int runnerHasValue1 = runner.value();
    }
}

int main(int argv, char **argc) {

	return 0;
}