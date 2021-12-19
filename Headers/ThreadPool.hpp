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
            lock_guard<mutex> accessLock{ this->accessMutex };
            this->threads.insert(make_pair(theKey, move(thread)));
        }

        void stopThread(string theKey) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            if (this->threads.contains(theKey)) {
                this->threads.at(theKey)->cancel();
                this->threads.erase(theKey);
            }
        }

        CoRoutineStatus getThreadStatus(string theKey) {
            lock_guard<mutex> accessLock{ this->accessMutex };
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
        mutex accessMutex{};

        CoRoutine<void> theTask() {
            co_await NewThreadAwaitable<void>();
            while (!this->doWeQuit) {
                this_thread::sleep_for(chrono::milliseconds(10000));
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

        ThreadPoolTimer(nullptr_t Null) {}

        static ThreadPoolTimer CreateTimer(TimeElapsedHandler timeElapsedHandler, uint64_t timeDelay) {
            ThreadPoolTimer threadPoolTimer{};
            ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, make_unique<CoRoutine<void>>(threadPoolTimer.run(timeDelay, timeElapsedHandler, false)));
            return ref(threadPoolTimer);
        }

        static ThreadPoolTimer CreatePeriodicTimer(TimeElapsedHandler timeElapsedHandler, uint64_t timeInterval) {
            ThreadPoolTimer threadPoolTimer{};
            ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, make_unique<CoRoutine<void>>(threadPoolTimer.run(timeInterval, timeElapsedHandler, true)));
            return ref(threadPoolTimer);
        }

        void Cancel() {
            ThreadPoolTimer::threads.stopThread(this->threadId);
        }

        bool Running() {
            if (ThreadPoolTimer::threads.getThreadStatus(this->threadId) == CoRoutineStatus::Running) {
                return true;
            }
            else {
                return false;
            }
        }

    protected:

        static ThreadPool threads;
        static mutex accessMutex;

        string threadId{ "" };

        ThreadPoolTimer() {
            this->threadId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count());
        }

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating) {
            auto cancelHandle = co_await NewThreadAwaitable<void>();
            StopWatch<chrono::milliseconds> stopWatch{ chrono::milliseconds(theInterval) };
            while (true) {
                stopWatch.resetTimer();
                cancelHandle.promise().waitForTime(static_cast<int64_t>(theInterval * 98 / 100));
                while (!stopWatch.hasTimePassed()) {
                    if (cancelHandle.promise().isItStopped()) {
                        co_return;
                    }
                    this_thread::sleep_for(chrono::milliseconds(5));
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
    CoRoutine<void> executeFunctionAfterTimePeriod(function<void(T...)>theFunction, int32_t timeDelayInMs, bool isRepeating, T... args) {
        co_await NewThreadAwaitable<void>();
        if (timeDelayInMs > 0 && !isRepeating) {
            TimeElapsedHandler timeElapsedHandler = [&]()->void {
                theFunction(args...);
                return;
            };
            ThreadPoolTimer threadPoolTimer = ThreadPoolTimer::CreateTimer(timeElapsedHandler, timeDelayInMs);
            DiscordCoreClient::threadPoolTimers.push_back(threadPoolTimer);
            co_return;
        }
        else if (timeDelayInMs > 0 && isRepeating) {
            TimeElapsedHandler timeElapsedHandler = [&]()->void {
                theFunction(args...);
                return;
            };
            ThreadPoolTimer threadPoolTimer = ThreadPoolTimer::CreatePeriodicTimer(timeElapsedHandler, timeDelayInMs);
            DiscordCoreClient::threadPoolTimers.push_back(threadPoolTimer);
        }
        else {
            theFunction(args...);
        }
        co_return;
    };
}
