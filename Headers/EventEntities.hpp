/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    struct EventToken {

        template<typename R, typename ...Args>
        friend class Event;

        template<typename R, typename ...Args>
        friend class EventDelegate;

        friend inline bool operator<(const EventToken& lhs, const EventToken& rhs);

        friend inline bool operator>(const EventToken& lhs, const EventToken& rhs);

        friend inline bool operator==(const EventToken& lhs, const EventToken& rhs);

        friend inline bool operator!=(const EventToken& lhs, const EventToken& rhs);

        ~EventToken() = default;

    protected:

        EventToken() = default;

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

    template<typename R, typename ...Args>
    class EventDelegate {
    public:

        template<typename R, typename... A>
        friend class Event;

        EventDelegate<R, Args...>& operator=(EventDelegate<R, Args...>&& other) {
            this->theFunction = move(other.theFunction);
            this->eventToken = move(other.eventToken);
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

        EventToken eventToken{};

    protected:
        function<R(Args...)>theFunction{};
    };

    template<typename R, typename  ...Args>
    class Event {
    public:

        Event<R, Args...>& operator=(const Event<R, Args...>& other) = delete;

        Event(const Event<R, Args...>& other) = delete;

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventToken add(EventDelegate<R, Args...> eventDelegate) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            EventToken eventToken = eventDelegate.eventToken;
            eventToken.handlerId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.storeValue(eventDelegate.eventToken.handlerId, move(eventDelegate));
            return eventToken;
        }

        void remove(EventToken  eventToken) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            if (this->theFunctions.contains(eventToken.handlerId)) {
                this->theFunctions.erase(eventToken.handlerId);
            }
        }

        map<EventToken, R> operator()(Args... args) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            map<EventToken, R> theMap{};
            for (auto& [key, value] : this->theFunctions) {
                theMap.insert_or_assign(value.eventToken, value.theFunction(args...));
            }
            return theMap;
        }

    protected:
        ObjectCache<string, EventDelegate<R, Args...>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
    };

    template<typename ...Args>
    class EventDelegate<void, Args...> {
    public:

        friend class Event<void, Args...>;

        EventDelegate<void, Args...>& operator=(EventDelegate<void, Args...>&& other) {
            this->theFunction = move(other.theFunction);
            this->eventToken = move(other.eventToken);
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

        EventToken eventToken{};

    protected:
        function<void(Args...)>theFunction{};
    };

    template<typename ...Args>
    class Event<void, Args...> {
    public:

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventToken add(EventDelegate<void, Args...> eventDelegate) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            EventToken eventToken = eventDelegate.eventToken;
            eventToken.handlerId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.storeValue(eventDelegate.eventToken.handlerId, move(eventDelegate));
            return eventToken;
        }

        void remove(EventToken  eventToken) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            if (this->theFunctions.contains(eventToken.handlerId)) {
                this->theFunctions.erase(eventToken.handlerId);
            }
        }

        void operator()(Args... args) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        ObjectCache<string, EventDelegate<void, Args...>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
    };
}