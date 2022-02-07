// ThreadPool.cpp - Source file for the "Thread-Pool" related stuff.
// Dec 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "ThreadPool.hpp"

namespace DiscordCoreAPI {

    ThreadPool::ThreadPool() {
        this->threads.store(this->threadsPtr.get(), std::memory_order_release);
        this->cleanupTask = this->theTask();
    }

    void ThreadPool::storeThread(std::string theKey, CoRoutine<void> thread) {
        auto thePtr = this->threads.load(std::memory_order_consume);
        thePtr->insert(std::make_pair(theKey, std::move(thread)));
        this->threads.store(thePtr, std::memory_order_release);
    }

    void ThreadPool::stopThread(std::string theKey) {
        auto thePtr = this->threads.load(std::memory_order_consume);
        if (thePtr->contains(theKey)) {
            thePtr->at(theKey).cancel();
            thePtr->at(theKey).get();
            thePtr->erase(theKey);
        }
        this->threads.store(thePtr, std::memory_order_release);
    }

    void ThreadPool::awaitThreadResult(std::string theKey) {
        auto thePtr = this->threads.load(std::memory_order_consume);
        if (thePtr->contains(theKey)) {
            thePtr->at(theKey).get();
        }
        this->threads.store(thePtr, std::memory_order_release);
    }

    CoRoutineStatus ThreadPool::getThreadStatus(std::string theKey) {
        if (this->threads.load(std::memory_order_consume)->contains(theKey)) {
            return this->threads.load(std::memory_order_consume)->at(theKey).getStatus();
        }
        else {
            return CoRoutineStatus{};
        }
    }

    ThreadPool::~ThreadPool() {
        this->doWeQuit = true;
    }

    CoRoutine<void> ThreadPool::theTask() {
        co_await NewThreadAwaitable<void>();
        while (!this->doWeQuit) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30000));
            auto thePtr = this->threads.load(std::memory_order_consume);
            for (auto& [key, value] : *thePtr) {
                if (value.getStatus() != CoRoutineStatus::Running) {
                    thePtr->at(key).get();
                    thePtr->erase(key);
                }
            }
            this->threads.store(thePtr, std::memory_order_release);
        }
        co_return;
    }

    void ThreadPoolTimer::initialize() {
        ThreadPoolTimer::threadsAtomic.store(ThreadPoolTimer::threads.get(), std::memory_order_release);
    }

    ThreadPoolTimer& ThreadPoolTimer::operator=(ThreadPoolTimer&& other) noexcept {
        this->threadId = std::move(other.threadId);
        other.threadId = "";
        return *this;
    }

    ThreadPoolTimer::ThreadPoolTimer(ThreadPoolTimer&& other) noexcept {
        *this = std::move(other);
    }

    ThreadPoolTimer::ThreadPoolTimer(nullptr_t) {}

    ThreadPoolTimer ThreadPoolTimer::createPeriodicTimer(TimeElapsedHandlerTwo timeElapsedHandler, int64_t timeInterval) {
        ThreadPoolTimer threadPoolTimer{};
        auto threadPoolPtr = ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume);
        threadPoolPtr->storeThread(threadPoolTimer.threadId, threadPoolTimer.run(timeInterval, timeElapsedHandler, true));
        ThreadPoolTimer::threadsAtomic.store(threadPoolPtr, std::memory_order_release);
        return threadPoolTimer;
    }

    ThreadPoolTimer ThreadPoolTimer::createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeInterval) {
        ThreadPoolTimer threadPoolTimer{};
        auto threadPoolPtr = ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume);
        threadPoolPtr->storeThread(threadPoolTimer.threadId, threadPoolTimer.run(timeInterval, timeElapsedHandler, true));
        ThreadPoolTimer::threadsAtomic.store(threadPoolPtr, std::memory_order_release);
        return threadPoolTimer;
    }

    void ThreadPoolTimer::awaitResult() {
        ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume)->awaitThreadResult(this->threadId);
    }

    bool ThreadPoolTimer::running() {
        if (ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume)->getThreadStatus(this->threadId) == CoRoutineStatus::Running) {
            return true;
        }
        else {
            return false;
        }
    }

    void ThreadPoolTimer::cancel() {
        ThreadPoolTimer::threadsAtomic.load(std::memory_order_consume)->stopThread(this->threadId);
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
    
    std::unique_ptr<ThreadPool> ThreadPoolTimer::threads{ std::make_unique<ThreadPool>() };
    std::atomic<ThreadPool*> ThreadPoolTimer::threadsAtomic{ nullptr };

    
}
