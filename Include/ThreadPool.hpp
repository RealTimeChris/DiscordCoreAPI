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

        void storeThread(std::string theKey, CoRoutine<void> thread);

        CoRoutineStatus getThreadStatus(std::string theKey);

        void stopThread(std::string theKey);

        ~ThreadPool();

    protected:
        std::unique_ptr< std::unordered_map<std::string, CoRoutine<void>>>threadsPtr{ std::make_unique<std::unordered_map<std::string, CoRoutine<void>>>() };
        std::atomic<std::unordered_map<std::string, CoRoutine<void>>*> threads{};
        bool doWeQuit{ false };

    };

    using TimeElapsedHandler = std::function<void(void)>;

    using TimeElapsedHandlerTwo = void(*)(void);

    class DiscordCoreAPI_Dll ThreadPoolTimer {
    public:

        ThreadPoolTimer& operator=(ThreadPoolTimer&&) noexcept;

        ThreadPoolTimer(ThreadPoolTimer&&) noexcept;

        ThreadPoolTimer& operator=(const ThreadPoolTimer&) = delete;

        ThreadPoolTimer(const ThreadPoolTimer&) = delete;

        ThreadPoolTimer& operator=(ThreadPoolTimer&) = delete;

        ThreadPoolTimer(ThreadPoolTimer&) = delete;

        ThreadPoolTimer(nullptr_t);

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeInterval);

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeInterval);

        bool running();

        void cancel();

        template <typename ...ArgTypes>
        static CoRoutine<void> executeFunctionAfterTimePeriod(std::function<void(ArgTypes...)>theFunction, int32_t timeDelayInMs, ArgTypes... args) {
            co_await NewThreadAwaitable<void>();
            if (timeDelayInMs >= 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds{ timeDelayInMs });
                theFunction(args...);
            }
            else {
                throw std::runtime_error("Please enter a valid delay time!");
            }
        };

        ~ThreadPoolTimer();

    protected:

        static std::unique_ptr<ThreadPool> threads;

        std::string threadId{ "" };

        ThreadPoolTimer();

        static CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating);

        static CoRoutine<void> run(int64_t theInterval, TimeElapsedHandlerTwo theFunction, bool repeating);

    };
}
