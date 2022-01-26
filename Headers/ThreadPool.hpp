// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
// Dec 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

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

        void awaitThreadResult(std::string theKey);

        void storeThread(std::string theKey, std::unique_ptr<CoRoutine<void>> thread);

        CoRoutineStatus getThreadStatus(std::string theKey);

        void stopThread(std::string theKey);        

        ~ThreadPool();

    protected:

        std::unordered_map<std::string, UniquePtrWrapper<CoRoutine<void>>> threads{};
        CoRoutine<void> cleanupTask{};
        bool doWeQuit{ false };

        CoRoutine<void> theTask();

    };

    using TimeElapsedHandler = std::function<void(void)>;

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

        void awaitResult();

        bool running();

        void cancel();

        ~ThreadPoolTimer();

    protected:

        static ThreadPool threads;

        std::string threadId{ "" };

        ThreadPoolTimer();

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating);

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandlerTwo theFunction, bool repeating);

    };

    template <typename ...ArgTypes>
    CoRoutine<void> executeFunctionAfterTimePeriod(std::function<void(ArgTypes...)>theFunction, int32_t timeDelayInMs, bool isItRepeating, ArgTypes&... args) {
        co_await NewThreadAwaitable<void>();
        if (timeDelayInMs >= 0 && !isItRepeating) {
            TimeElapsedHandler timeElapsedHandler = [=]()->void {
                theFunction(args...);
            };
            ThreadPoolTimer threadPoolTimer = ThreadPoolTimer::createTimer(timeElapsedHandler, timeDelayInMs);
            threadPoolTimer.awaitResult();
        }
        else if (timeDelayInMs >= 0 && isItRepeating) {
            TimeElapsedHandler timeElapsedHandler = [=]()->void {
                theFunction(args...);
            };
            ThreadPoolTimer threadPoolTimer = ThreadPoolTimer::createPeriodicTimer(timeElapsedHandler, timeDelayInMs);
            threadPoolTimer.awaitResult();
        }
        else {
            throw std::exception("Please enter a valid delay time!");
        }
        
    };
}
