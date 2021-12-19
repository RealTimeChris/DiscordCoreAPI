/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

    struct DiscordCoreAPI_Dll EventDelegateToken {

        template<typename R, typename ...Args>
        friend class Event;

        template<typename R, typename ...Args>
        friend class EventDelegate;

        friend inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator>(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

    protected:

        EventDelegateToken() = default;

        string handlerId{ "" };
        string eventId{ "" };

    };

    bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs) {
        if (lhs.eventId == rhs.eventId && lhs.handlerId == rhs.handlerId) {
            return true;
        }
        else {
            return false;
        }
    }

    bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs) {
        if (lhs.handlerId < rhs.handlerId) {
            return true;
        }
        else {
            return false;
        }
    }

    bool operator>(const EventDelegateToken& lhs, const EventDelegateToken& rhs) {
        if (lhs < rhs) {
            return false;
        }
        else {
            return true;
        }
    }

    template<typename R, typename ...Args>
    class EventDelegate {
    public:

        template<typename R, typename... A>
        friend class Event;

        EventDelegate<R, Args...>& operator=(EventDelegate<R, Args...>&& other) {
            this->theFunction = move(other.theFunction);
            return *this;
        }

        EventDelegate(EventDelegate<R, Args...>&& other) {
            *this = move(other);
        }

        EventDelegate<R, Args...>& operator=(const EventDelegate<R, Args...>& other) = delete;

        EventDelegate(const EventDelegate<R, Args...>& other) = delete;

        EventDelegate(function<R(Args...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(R(*theFunctionNew)(Args...)) {
            this->theFunction = theFunctionNew;
        }

    protected:
        function<R(Args...)>theFunction{};
    };

    template<typename R, typename  ...Args>
    class DiscordCoreAPI_Dll Event {
    public:

        Event<R, Args...>& operator=(Event<R, Args...>&& other) {
            this->theFunctions = move(other.theFunctions);
            this->eventId = other.eventId;
            return *this;
        }

        Event(Event<R, Args...>&& other) {
            *this = move(other);
        }

        Event<R, Args...>& operator=(const Event<R, Args...>&) = delete;

        Event(const Event<R, Args...>&) = delete;

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<R, Args...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.storeValue(eventToken, move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        void operator()(Args... args) {
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        ObjectCache<EventDelegateToken, EventDelegate<R, Args...>> theFunctions{};
        string eventId{ "" };
    };

    template<typename ...Args>
    class EventDelegate<void, Args...> {
    public:

        friend class Event<void, Args...>;

        EventDelegate<void, Args...>& operator=(EventDelegate<void, Args...>&& other) {
            this->theFunction = move(other.theFunction);
            return *this;
        }

        EventDelegate(EventDelegate<void, Args...>&& other) {
            *this = move(other);
        }

        EventDelegate<void, Args...>& operator=(const EventDelegate<void, Args...>& other) = delete;

        EventDelegate(const EventDelegate<void, Args...>& other) = delete;

        EventDelegate(function<void(Args...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(void(*theFunctionNew)(Args...)) {
            this->theFunction = theFunctionNew;
        }

    protected:
        function<void(Args...)>theFunction{};
    };

    template<typename ...Args>
    class DiscordCoreAPI_Dll Event<void, Args...> {
    public:

        Event<void, Args...>& operator=(Event<void, Args...>&& other) {
            this->theFunctions = move(other.theFunctions);
            this->eventId = other.eventId;
            return *this;
        }

        Event(Event<void, Args...>&& other) {
            *this = move(other);
        }

        Event<void, Args...>& operator=(const Event<void, Args...>&) = delete;

        Event(const Event<void, Args...>&) = delete;

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<void, Args...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.storeValue(eventToken, move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        void operator()(Args... args) {
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:

        ObjectCache<EventDelegateToken, EventDelegate<void, Args...>> theFunctions{};
        string eventId{ "" };

    };

    struct DiscordCoreAPI_Dll EventCore {

        friend class Event<void, void>;

        EventCore& operator=(EventCore&) = delete;

        EventCore(EventCore&) = delete;

        EventCore() = default;

    protected:

        static ObjectCache<string, unique_ptr<EventCore>> theEvents;
        static ObjectCache<string, uint32_t> refCounts;

        shared_ptr<bool> theEventState{ make_shared<bool>(false) };

    };

    template<>
    class DiscordCoreAPI_Dll Event<void, void> {
    public:

        shared_ptr<bool> theEventState{ make_shared<bool>(false) };
        string eventId{ "" };

        Event<void, void>& operator=(Event<void, void>&&) = delete;

        Event(Event<void, void>&&) = delete;

        Event<void, void>& operator=(const Event<void, void>&) = delete;

        Event(const Event<void, void>&) = delete;

        Event<void, void>& operator=(Event<void, void>& other) {
            this->theEventState = other.theEventState;
            this->eventId = other.eventId;
            return *this;
        }

        Event(Event<void, void>& other) {
            *this = other;
            EventCore::refCounts.returnValue(this->eventId) += 1;
        }

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
            EventCore::theEvents.storeValue(this->eventId, make_unique<EventCore>());
            EventCore::refCounts.storeValue(this->eventId, 1);
            this->theEventState = EventCore::theEvents.returnValue(this->eventId)->theEventState;
        }

        uint32_t wait(uint64_t millisecondsMaxToWait = UINT64_MAX, string testString = "") {
            uint32_t millisecondsWaited{ 0 };
            while (true) {
                if (*this->theEventState) {
                    return 0;
                }
                else {
                    this_thread::sleep_for(chrono::microseconds(1000));
                    millisecondsWaited += 1;
                }
                if (millisecondsWaited >= millisecondsMaxToWait) {
                    return 1;
                }
            }
        }

        void set(string testValue = "") {
            *this->theEventState = true;
        }

        void reset() {
            *this->theEventState = false;
        }

        ~Event() {
            if (EventCore::refCounts.contains(this->eventId)) {
                if (EventCore::refCounts.returnValue(this->eventId) > 0) {
                    EventCore::refCounts.returnValue(this->eventId) -= 1;

                }
                if (EventCore::refCounts.returnValue(this->eventId) == 0) {
                    EventCore::refCounts.erase(this->eventId);
                    EventCore::theEvents.erase(this->eventId);
                }
            }
        }
    };

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
                cout << "IT CONTAINS THE KEY" << endl;
                this->threads.at(theKey)->cancel();
                this->threads.erase(theKey);
            }
            else {
                cout << "IT DOES NOT CONTAIN THE KEY" << endl;
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
            cout << "THE COUNT: " << ThreadPoolTimer::threadRefCounts.at(this->threadId) << endl;
            return *this;
        }

        ThreadPoolTimer(const ThreadPoolTimer&& other) {
            *this = other;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            cout << "THE COUNT: " << ThreadPoolTimer::threadRefCounts.at(this->threadId) << endl;
        }

        ThreadPoolTimer& operator=(ThreadPoolTimer&& other) {
            this->threadId = other.threadId;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            cout << "THE COUNT: " << ThreadPoolTimer::threadRefCounts.at(this->threadId) << endl;
            return *this;
        }

        ThreadPoolTimer(ThreadPoolTimer&& other) {
            *this = move(other);
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            cout << "THE COUNT: " << ThreadPoolTimer::threadRefCounts.at(this->threadId) << endl;
        }

        ThreadPoolTimer& operator=(const ThreadPoolTimer& other) {
            this->threadId = other.threadId;
            return *this;
        }

        ThreadPoolTimer(const ThreadPoolTimer& other) {
            *this = other;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            cout << "THE COUNT: " << ThreadPoolTimer::threadRefCounts.at(this->threadId) << endl;
        }

        ThreadPoolTimer& operator=(ThreadPoolTimer& other) {
            this->threadId = other.threadId;
            return *this;
        }

        ThreadPoolTimer(ThreadPoolTimer& other) {
            *this = other;
            ThreadPoolTimer::threadRefCounts.at(this->threadId) += 1;
            cout << "THE COUNT: " << ThreadPoolTimer::threadRefCounts.at(this->threadId) << endl;
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
                cout << "THREAD POOL TIMER DESTRUCTION CURRENT COUNT: " << to_string(ThreadPoolTimer::threadRefCounts.at(this->threadId)) << endl;
                if (ThreadPoolTimer::threadRefCounts.at(this->threadId) == 0) {
                    cout << "WERE BEING DESTROYED!! " << endl;
                    ThreadPoolTimer::threads.stopThread(this->threadId);
                    cout << "WEVE BEEN DESTROYED!! " << endl;
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
                cout << "TIMED TIMED TIMED" << endl;
                stopWatch.resetTimer();
                cancelHandle.promise().waitForTime(static_cast<int64_t>(theInterval * 98 / 100));
                while (!stopWatch.hasTimePassed()) {
                    cout << "WE HAVE YET TO HIT THE TIMER!" << endl;
                    if (cancelHandle.promise().isItStopped()) {
                        cout << "WE HAVE YET TO HIT THE TIMER!" << endl;
                        co_return;
                    }
                    this_thread::sleep_for(chrono::milliseconds(2));
                }
                cout << "WE HIT THE TIMER!" << endl;
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
    }


}