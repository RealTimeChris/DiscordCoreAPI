// ThreadPool.cpp - Source file for the "Thread-Pool" related stuff.
// Dec 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "ThreadPool.hpp"

namespace DiscordCoreAPI {

    void ThreadPool::storeThread(std::string theKey, CoRoutine<void> thread) {
        this->threads.insert(std::make_pair(theKey, std::move(thread)));
    }

    void ThreadPool::stopThread(std::string theKey) {
        if (this->threads.contains(theKey)) {
            this->threads.at(theKey).cancel();
            this->threads.at(theKey).get();
            this->threads.erase(theKey);
        }
    }

    CoRoutineStatus ThreadPool::getThreadStatus(std::string theKey) {
        if (this->threads.contains(theKey)) {
            return this->threads.at(theKey).getStatus();
        }
        else {
            return CoRoutineStatus{};
        }
    }

    ThreadPool::~ThreadPool() {
        this->doWeQuit = true;
    }

    ThreadPoolTimer& ThreadPoolTimer::operator=(ThreadPoolTimer&& other) noexcept {
        if (this != &other) {
            this->threadId = std::move(other.threadId);
            other.threadId = "";
        }
        return *this;
    }

    ThreadPoolTimer::ThreadPoolTimer(ThreadPoolTimer&& other) noexcept {
        *this = std::move(other);
    }

    ThreadPoolTimer::ThreadPoolTimer(nullptr_t) {}

    ThreadPoolTimer ThreadPoolTimer::createPeriodicTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeInterval) {
        ThreadPoolTimer threadPoolTimer{};
        ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, threadPoolTimer.run(timeInterval, timeElapsedHandler, true));
        return threadPoolTimer;
    }

    ThreadPoolTimer ThreadPoolTimer::createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeInterval) {
        ThreadPoolTimer threadPoolTimer{};
        ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, threadPoolTimer.run(timeInterval, timeElapsedHandler, true));
        return threadPoolTimer;
    }

    bool ThreadPoolTimer::running() {
        if (ThreadPoolTimer::threads.getThreadStatus(this->threadId) == CoRoutineStatus::Running) {
            return true;
        }
        else {
            return false;
        }
    }

    void ThreadPoolTimer::cancel() {
        ThreadPoolTimer::threads.stopThread(this->threadId);
    }

    ThreadPoolTimer::ThreadPoolTimer() {
        this->threadId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }

    CoRoutine<void> ThreadPoolTimer::run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating) {
        auto cancelHandle = co_await NewThreadAwaitable<void>();
        StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{theInterval} };
        while (true) {
            stopWatch.resetTimer();
            cancelHandle.promise().waitForTime(static_cast<int64_t>(ceil(static_cast<double>(theInterval) * 99.0f / 100.0f)));
            while (!stopWatch.hasTimePassed()) {
                if (cancelHandle.promise().isItStopped()) {
                    co_return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
            }
            theFunction();
            if (cancelHandle.promise().isItStopped() || !repeating) {
                co_return;
            }
        }
        co_return;
    }

    CoRoutine<void> ThreadPoolTimer::run(int64_t theInterval, TimeElapsedHandlerTwo theFunction, bool repeating) {
        auto cancelHandle = co_await NewThreadAwaitable<void>();
        StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{theInterval} };
        while (true) {
            stopWatch.resetTimer();
            cancelHandle.promise().waitForTime(static_cast<int64_t>(ceil(static_cast<double>(theInterval) * 99.0f / 100.0f)));
            while (!stopWatch.hasTimePassed()) {
                if (cancelHandle.promise().isItStopped()) {
                    co_return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
            }
            theFunction();
            if (cancelHandle.promise().isItStopped() || !repeating) {
                co_return;
            }
        }
        co_return;
    }

    ThreadPoolTimer::~ThreadPoolTimer() {
        this->cancel();
    };
    
    ThreadPool ThreadPoolTimer::threads{};
    
}
