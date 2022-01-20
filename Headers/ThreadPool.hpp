// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
// Dec 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

    class DiscordCoreAPI_Dll ThreadPool {
    public:

        ThreadPool& operator=(const ThreadPool&) = delete;

        ThreadPool(const ThreadPool&) = delete;

        ThreadPool& operator=(ThreadPool&) = delete;

        ThreadPool(ThreadPool&) = delete;

        ThreadPool();

        void storeThread(string theKey, unique_ptr<CoRoutine<void>> thread);

        void stopThread(string theKey);

        CoRoutineStatus getThreadStatus(string theKey);

        ~ThreadPool();

    protected:

        map<string, unique_ptr<CoRoutine<void>>> threads{};
        CoRoutine<void> cleanupTask{};
        bool doWeQuit{ false };

        CoRoutine<void> theTask();

    };

    using TimeElapsedHandler = function<void(void)>;

    using TimeElapsedHandlerTwo = void(*)(void);

    class DiscordCoreAPI_Dll ThreadPoolTimer {
    public:
        
        ThreadPoolTimer& operator=(ThreadPoolTimer&& other) noexcept;

        ThreadPoolTimer(ThreadPoolTimer&& other) noexcept;

        ThreadPoolTimer& operator=(const ThreadPoolTimer& other) = delete;

        ThreadPoolTimer(const ThreadPoolTimer& other) = delete;

        ThreadPoolTimer& operator=(ThreadPoolTimer& other) = delete;

        ThreadPoolTimer(ThreadPoolTimer& other) = delete;

        ThreadPoolTimer(nullptr_t);

        static ThreadPoolTimer createTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeDelay);

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeInterval);

        static ThreadPoolTimer createTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeDelay);

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeInterval);

        void cancel();

        bool running();

    protected:

        static ThreadPool threads;

        string threadId{ "" };

        ThreadPoolTimer();

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating);

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandlerTwo theFunction, bool repeating);

    };

    template <typename ...T>
    void executeFunctionAfterTimePeriod(function<void(T...)>theFunction, int32_t timeDelayInMs, bool isRepeating, T... args) {
        if (timeDelayInMs > 0 && !isRepeating) {
            TimeElapsedHandler timeElapsedHandler = [=]()->void {
                theFunction(args...);
            };
            ThreadPoolTimer threadPoolTimer = ThreadPoolTimer::createTimer(timeElapsedHandler, timeDelayInMs);
        }
        else if (timeDelayInMs > 0 && isRepeating) {
            TimeElapsedHandler timeElapsedHandler = [=]()->void {
                theFunction(args...);
            };
            ThreadPoolTimer threadPoolTimer = ThreadPoolTimer::createPeriodicTimer(timeElapsedHandler, timeDelayInMs);
        }
        else {
            throw exception("Please enter a valid delay time!");
        }
        
    };
}
