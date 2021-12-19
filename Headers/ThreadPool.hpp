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

        ThreadPool() = default;

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

    protected:

        map<string, unique_ptr<CoRoutine<void>>> threads{};
        mutex accessMutex{};

    };

    typedef function<void(void)> TimeElapsedHandler;

    class DiscordCoreAPI_Dll ThreadPoolTimer {
    public:

        ThreadPoolTimer& operator=(const ThreadPoolTimer&& other) {
            this->threadId = other.threadId;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            return *this;
        }

        ThreadPoolTimer(const ThreadPoolTimer&& other) {
            *this = other;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
        }

        ThreadPoolTimer& operator=(ThreadPoolTimer&& other) {
            this->threadId = other.threadId;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            return *this;
        }

        ThreadPoolTimer(ThreadPoolTimer&& other) {
            *this = move(other);
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
        }

        ThreadPoolTimer& operator=(const ThreadPoolTimer& other) {
            this->threadId = other.threadId;
            return *this;
        }

        ThreadPoolTimer(const ThreadPoolTimer& other) {
            *this = other;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
        }

        ThreadPoolTimer& operator=(ThreadPoolTimer& other) {
            this->threadId = other.threadId;
            return *this;
        }

        ThreadPoolTimer(ThreadPoolTimer& other) {
            *this = other;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
        }

        ThreadPoolTimer() {
            this->threadId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count());
            ThreadPoolTimer::threadRefCounts.insert(make_pair(this->threadId, 1));
        }

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
            ThreadPoolTimer::threadRefCounts.erase(this->threadId);

        }

        bool Running() {
            if (ThreadPoolTimer::threads.getThreadStatus(this->threadId) == CoRoutineStatus::Running) {
                return true;
            }
            else {
                return false;
            }
        }

        ~ThreadPoolTimer() {
            lock_guard<mutex> accessLock{ ThreadPoolTimer::accessMutex };
            if (ThreadPoolTimer::threadRefCounts.contains(this->threadId)) {
                if (ThreadPoolTimer::threadRefCounts.at(this->threadId) > 0) {
                    ThreadPoolTimer::threadRefCounts.at(this->threadId) -= 1;
                }
                if (ThreadPoolTimer::threadRefCounts.at(this->threadId) == 0) {
                    ThreadPoolTimer::threads.stopThread(this->threadId);
                    ThreadPoolTimer::threadRefCounts.erase(this->threadId);
                }
            }
        }

    protected:

        string threadId{ "" };

        static map<string, uint64_t> threadRefCounts;
        static ThreadPool threads;
        static mutex accessMutex;

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating) {
            auto cancelHandle = co_await NewThreadAwaitable<void>();
            StopWatch<chrono::milliseconds> stopWatch{ chrono::milliseconds(theInterval) };
            while (true) {
                stopWatch.resetTimer();
                cancelHandle.promise().waitForTime(static_cast<int64_t>(theInterval * 98 / 100));
                while (!stopWatch.hasTimePassed()) {
                    if (cancelHandle.promise().newThread->get_stop_token().stop_requested()) {
                        co_return;
                    }
                    this_thread::sleep_for(chrono::milliseconds(2));
                }
                theFunction();
                if (cancelHandle.promise().newThread->get_stop_token().stop_requested() || !repeating) {
                    co_return;
                }
            }
            co_return;
        }

    };



}
