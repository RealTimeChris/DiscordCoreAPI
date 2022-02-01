// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
// Dec 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef THREAD_POOL
#define THREAD_POOL

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
        std::unique_ptr< std::unordered_map<std::string, UniquePtrWrapper<CoRoutine<void>>>>threadsPtr{ std::make_unique<std::unordered_map<std::string, UniquePtrWrapper<CoRoutine<void>>>>() };
        std::atomic<std::unordered_map<std::string, UniquePtrWrapper<CoRoutine<void>>>*> threads{};
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

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeInterval);

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeInterval);

        static ThreadPoolTimer createTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeDelay);

        static ThreadPoolTimer createTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeDelay);

        static void initialize();

        void awaitResult();

        bool running();

        void cancel();

        template <typename ...ArgTypes>
        static CoRoutine<void> executeFunctionAfterTimePeriod(std::function<void(ArgTypes...)>theFunction, int32_t timeDelayInMs, bool isItRepeating, ArgTypes... args) {
            co_await NewThreadAwaitable<void>();
            if (timeDelayInMs >= 0 && !isItRepeating) {
                TimeElapsedHandler timeElapsedHandler = [=]()->void {
                    theFunction(args...);
                };
                ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume)->storeThread(std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()), std::make_unique<CoRoutine<void>>(ThreadPoolTimer::run(timeDelayInMs, timeElapsedHandler, false)));
            }
            else if (timeDelayInMs >= 0 && isItRepeating) {
                TimeElapsedHandler timeElapsedHandler = [=]()->void {
                    theFunction(args...);
                };
                ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume)->storeThread(std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()), std::make_unique<CoRoutine<void>>(ThreadPoolTimer::run(timeDelayInMs, timeElapsedHandler, true)));
            }
            else {
                throw std::exception("Please enter a valid delay time!");
            }

        };

        ~ThreadPoolTimer();

    protected:

        static std::atomic<ThreadPool*> threadsAtomic;
        static std::unique_ptr<ThreadPool> threads;

        std::string threadId{ "" };

        ThreadPoolTimer();

        static CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating);

        static CoRoutine<void> run(int64_t theInterval, TimeElapsedHandlerTwo theFunction, bool repeating);

    };
}
#endif