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

        bool running();

        void cancel();

    protected:

        static ThreadPool threads;

        std::string threadId{ "" };

        ThreadPoolTimer();

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating);

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandlerTwo theFunction, bool repeating);

    };

    template <typename ...ArgTypes>
    void executeFunctionAfterTimePeriod(std::function<void(ArgTypes...)>theFunction, int32_t timeDelayInMs, bool isRepeating, ArgTypes... args) {
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
            throw std::exception("Please enter a valid delay time!");
        }
        
    };
}
