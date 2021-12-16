/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    struct EventDelegateToken {

        template<typename R, typename ...Args>
        friend class Event;

        template<typename R, typename ...Args>
        friend class EventDelegate;

        friend inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator>(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator!=(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

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

    bool operator!=(const EventDelegateToken& lhs, const EventDelegateToken& rhs) {
        if (lhs == rhs) {
            return false;
        }
        else {
            return true;
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
    class Event {
    public:

        Event<R, Args...>& operator=(const Event<R, Args...>&) = delete;

        Event(const Event<R, Args...>&) = delete;

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<R, Args...> eventDelegate) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            EventDelegateToken eventToken{};
            eventToken.handlerId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.storeValue(eventToken, move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        map<EventDelegateToken, R> operator()(Args... args) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            map<EventDelegateToken, R> theMap{};
            for (auto& [key, value] : this->theFunctions) {
                theMap.insert_or_assign(key, value.theFunction(args...));
            }
            return theMap;
        }

    protected:
        ObjectCache<EventDelegateToken, EventDelegate<R, Args...>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
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
    class Event<void, Args...> {
    public:

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        Event& operator=(const Event&) = delete;

        Event(const Event&) = delete;

        EventDelegateToken add(EventDelegate<void, Args...> eventDelegate) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            EventDelegateToken eventToken{};
            eventToken.handlerId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.storeValue(eventToken, move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        void operator()(Args... args) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        ObjectCache<EventDelegateToken, EventDelegate<void, Args...>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
    };

    struct EventToken {

        template<typename R, typename ...Args>
        friend class Event;

        template<typename R, typename ...Args>
        friend class EventDelegate;

        friend inline bool operator<(const EventToken& lhs, const EventToken& rhs);

        friend inline bool operator>(const EventToken& lhs, const EventToken& rhs);

        friend inline bool operator==(const EventToken& lhs, const EventToken& rhs);

        friend inline bool operator!=(const EventToken& lhs, const EventToken& rhs);

    protected:

        string handlerId{ "" };
        string eventId{ "" };

    };

    bool operator==(const EventToken& lhs, const EventToken& rhs) {
        if (lhs.eventId == rhs.eventId && lhs.handlerId == rhs.handlerId) {
            return true;
        }
        else {
            return false;
        }
    }

    bool operator!=(const EventToken& lhs, const EventToken& rhs) {
        if (lhs == rhs) {
            return false;
        }
        else {
            return true;
        }
    }

    inline bool operator<(const EventToken& lhs, const EventToken& rhs) {
        if (lhs.handlerId < rhs.handlerId) {
            return true;
        }
        else {
            return false;
        }
    }

    inline bool operator>(const EventToken& lhs, const EventToken& rhs) {
        if (lhs < rhs) {
            return false;
        }
        else {
            return true;
        }
    }

    template<>
    class Event<void, void> {
    public:

        Event<void, void>& operator=(Event<void, void>& other) {
            this->eventToken.store(other.eventToken.load());
            this->amIActive.store(other.amIActive.load());
            *Event::refCounts.returnValue(*this->eventToken.load()) += 1;
            return *this;
        }

        Event(Event<void, void>& other) {
            *this = other;
        }

        Event() {
            EventToken newToken{};
            newToken.handlerId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()).c_str();
            newToken.eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()).c_str();
            this->eventToken = make_shared<EventToken>(newToken);
            Event::theEvents.storeValue(*this->eventToken.load(), this);
            Event::refCounts.storeValue(*this->eventToken.load(), make_shared<uint32_t>(1));
        }

        bool wait(uint64_t millisecondsMaxToWait = UINT64_MAX) {
            uint32_t millisecondsWaited{ 0 };
            while (true) {
                if (*Event::theEvents.returnValue(*this->eventToken.load())->amIActive.load()) {
                    return true;
                }
                else {
                    this_thread::sleep_for(chrono::microseconds(1000));
                    millisecondsWaited += 1;
                }
                if (millisecondsWaited >= millisecondsMaxToWait) {
                    return false;
                }
            }
        }

        void set() {
            cout << "SETTING!" << endl;
            *Event::theEvents.returnValue(*this->eventToken.load())->amIActive.load() = true;
        }

        void reset() {
            cout << "RESETTING!" << endl;
            *Event::theEvents.returnValue(*this->eventToken.load())->amIActive.load() = false;
        }

        ~Event() {
            *Event::refCounts.returnValue(*this->eventToken.load()) -= 1;
            if (Event::refCounts.returnValue(*this->eventToken.load()) == 0) {
                Event::refCounts.erase(*this->eventToken.load());
                Event::theEvents.erase(*this->eventToken.load());
            }
        }

    protected:
        static ObjectCache<EventToken, shared_ptr<uint32_t>> refCounts;
        static ObjectCache<EventToken, Event<void, void>*> theEvents;
        atomic<shared_ptr<EventToken>> eventToken{ atomic<shared_ptr<EventToken>>(make_shared<EventToken>()) };
        atomic<shared_ptr<bool>> amIActive{ atomic<shared_ptr<bool>>(make_shared<bool>()) };


    };
}