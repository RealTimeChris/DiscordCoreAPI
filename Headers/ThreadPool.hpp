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

        ThreadPool() {
            this->cleanupTask = this->theTask();
        }

        ThreadPool& operator=(ThreadPool&) = delete;

        ThreadPool(ThreadPool&) = delete;

        void storeThread(string theKey, unique_ptr<CoRoutine<void>> thread) {
            this->threads.insert(make_pair(theKey, move(thread)));
        }

        void stopThread(string theKey) {
            if (this->threads.contains(theKey)) {
                this->threads.at(theKey)->cancel();
                this->threads.erase(theKey);
            }
        }

        CoRoutineStatus getThreadStatus(string theKey) {
            if (this->threads.contains(theKey)) {
                return this->threads.at(theKey)->getStatus();
            }
            else {
                return CoRoutineStatus{};
            }
        }

        ~ThreadPool() {
            this->doWeQuit = true;
        }

    protected:

        map<string, unique_ptr<CoRoutine<void>>> threads{};
        CoRoutine<void> cleanupTask;
        bool doWeQuit{ false };

        CoRoutine<void> theTask() {
            co_await NewThreadAwaitable<void>();
            while (!this->doWeQuit) {
                this_thread::sleep_for(chrono::milliseconds(30000));
                for (auto& [key, value] : this->threads) {
                    if (value->getStatus() != CoRoutineStatus::Running) {
                        this->threads.erase(key);
                    }
                }
            }
            co_return;
        }

    };

    typedef function<void(void)> TimeElapsedHandler;

    class DiscordCoreAPI_Dll ThreadPoolTimer {
    public:

        ThreadPoolTimer(nullptr_t) {}

        static ThreadPoolTimer createTimer(TimeElapsedHandler timeElapsedHandler, uint64_t timeDelay) {
            ThreadPoolTimer threadPoolTimer{};
            ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, make_unique<CoRoutine<void>>(threadPoolTimer.run(timeDelay, timeElapsedHandler, false)));
            return threadPoolTimer;
        }

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, uint64_t timeInterval) {
            ThreadPoolTimer threadPoolTimer{};
            ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, make_unique<CoRoutine<void>>(threadPoolTimer.run(timeInterval, timeElapsedHandler, true)));
            return threadPoolTimer;
        }

        void cancel() {
            ThreadPoolTimer::threads.stopThread(this->threadId);
        }

        bool running() {
            if (ThreadPoolTimer::threads.getThreadStatus(this->threadId) == CoRoutineStatus::Running) {
                return true;
            }
            else {
                return false;
            }
        }

    protected:

        static ThreadPool threads;

        string threadId{ "" };

        ThreadPoolTimer() {
            this->threadId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count());
        }

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating) {
            auto cancelHandle = co_await NewThreadAwaitable<void>();
            StopWatch<chrono::milliseconds> stopWatch{ chrono::milliseconds(theInterval) };
            while (true) {
                stopWatch.resetTimer();
                cancelHandle.promise().waitForTime(static_cast<int64_t>(theInterval * 99 / 100));
                while (!stopWatch.hasTimePassed()) {
                    if (cancelHandle.promise().isItStopped()) {
                        co_return;
                    }
                    this_thread::sleep_for(chrono::milliseconds(10));
                }
                theFunction();
                if (cancelHandle.promise().isItStopped() || !repeating) {
                    co_return;
                }
            }
            co_return;
        }

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
