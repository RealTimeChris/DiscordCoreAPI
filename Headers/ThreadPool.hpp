// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
// Dec 18, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

    class jthreadWrapper {
    public:

        jthreadWrapper() {}

        jthreadWrapper(unique_ptr<bool> theBool, jthread thread) {
            this->theThread.swap(thread);
            this->theBool = move(theBool);
        }

        void stopThread() {
            this->theThread.get_stop_source().request_stop();
        }

        bool areWeRunning() {
            return *this->theBool;
        }

    protected:

        unique_ptr<bool> theBool{};
        jthread theThread{};            
    };

    class DiscordCoreAPI_Dll ThreadPool {
    public:

        ThreadPool& operator=(const ThreadPool&) = delete;

        ThreadPool(const ThreadPool&) = delete;

        ThreadPool& operator=(ThreadPool&) = delete;

        ThreadPool(ThreadPool&) = delete;

        ThreadPool() {
            this->cleanupTask = this->theTask();
        }

        void storeThread(string theKey, unique_ptr<CoRoutine<void>> thread) {
            this->threads02.insert(make_pair(theKey, move(thread)));
        }

        void storeThread(string theKey, unique_ptr<jthreadWrapper> thread) {
            this->threads.insert(make_pair(theKey, move(thread)));
        }

        bool getThreadStatus(string theKey) {
            bool result{ false };
            if (this->threads.contains(theKey)) {
                result = this->threads.at(theKey)->areWeRunning();
            }
            else if (
                this->threads02.contains(theKey)) {
                if (this->threads02.at(theKey)->getStatus() == CoRoutineStatus::Running) {
                    result = true;
                }
            }
            return result;
        }

        void stopThread(string theKey) {
            if (this->threads.contains(theKey)) {
                this->threads.at(theKey)->stopThread();
                this->threads.erase(theKey);
            }
            else if (this->threads02.contains(theKey)) {
                this->threads02.at(theKey)->cancel();
            }
        }

        ~ThreadPool() {
            this->doWeQuit = true;
        }

    protected:

        map<string, unique_ptr<CoRoutine<void>>> threads02{};
        map<string, unique_ptr<jthreadWrapper>> threads{};
        CoRoutine<void> cleanupTask{};
        bool doWeQuit{ false };

        CoRoutine<void> theTask() {
            co_await NewThreadAwaitable<void>();
            while (!this->doWeQuit) {
                this_thread::sleep_for(chrono::milliseconds(30000));
                for (auto& [key, value] : this->threads) {
                    if (!value->areWeRunning()) {
                        this->threads.erase(key);
                    }
                }
                for (auto& [key, value] : this->threads02) {
                    if (value->getStatus() != CoRoutineStatus::Running) {
                        this->threads02.erase(key);
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
            unique_ptr<bool> theBool{ make_unique<bool>() };
            jthread newThread([](stop_token stopToken, TimeElapsedHandler theFunction, uint64_t timeDelay, bool* theState) {
                StopWatch<chrono::milliseconds> stopWatch{ chrono::milliseconds(timeDelay) };
                while (true) {
                    stopWatch.resetTimer();
                    uint64_t msCount{ 0 };
                    while (!stopToken.stop_requested() && msCount < timeDelay) {
                        msCount += 1;
                        this_thread::sleep_for(chrono::milliseconds(1));
                    }
                    while (!stopWatch.hasTimePassed()) {
                        if (stopToken.stop_requested()) {
                            *theState = false;
                            return;
                        }
                        this_thread::sleep_for(chrono::milliseconds(10));
                    }
                    theFunction();
                    *theState = false;
                    return;
                }
                }, timeElapsedHandler, timeDelay, theBool.get());
            ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, make_unique<jthreadWrapper>(move(theBool), move(newThread)));
            return threadPoolTimer;
        }

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, uint64_t timeInterval) {
            ThreadPoolTimer threadPoolTimer{};
            unique_ptr<bool> theBool{ make_unique<bool>() };
            jthread newThread([](stop_token stopToken, TimeElapsedHandler theFunction, uint64_t timeDelay, bool* theState) {
                StopWatch<chrono::milliseconds> stopWatch{ chrono::milliseconds(timeDelay) };
                while (true) {
                    stopWatch.resetTimer();
                    uint64_t msCount{ 0 };
                    while (!stopToken.stop_requested() && msCount < timeDelay) {
                        msCount += 1;
                        this_thread::sleep_for(chrono::milliseconds(1));
                    }
                    while (!stopWatch.hasTimePassed()) {
                        if (stopToken.stop_requested()) {
                            *theState = false;
                            return;
                        }
                        this_thread::sleep_for(chrono::milliseconds(10));
                    }
                    theFunction();
                    if (stopToken.stop_requested()) {
                        *theState = false;
                        return;
                    }
                }
                }, timeElapsedHandler, timeInterval, theBool.get());
            ThreadPoolTimer::threads.storeThread(threadPoolTimer.threadId, make_unique<jthreadWrapper>(move(theBool), move(newThread)));
            return threadPoolTimer;
        }

        void cancel() {
            ThreadPoolTimer::threads.stopThread(this->threadId);
        }

        bool running() {
            if (ThreadPoolTimer::threads.getThreadStatus(this->threadId)) {
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
