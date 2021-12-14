/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {
    
    struct EventToken {

        template<typename...args>
        friend class Event;

        template<typename... args>
        friend class EventDelegate;

        EventToken() = default;

    protected:
        string handlerId{ "" };
        string eventId{ "" };
        
    };

    
    template<typename...Args>
    class EventDelegate {
    public:

        template<typename ...A>
        friend class Event;

        EventDelegate<Args...>& operator=(EventDelegate<Args...>&&other) {
            this->theFunction = move(other.theFunction);
            this->eventToken = move(other.eventToken);
            return *this;
        }

        EventDelegate(EventDelegate<Args...>&& other) {
            *this = move(other);
        }

        EventDelegate& operator=(EventDelegate<Args...>&) = delete;

        EventDelegate(EventDelegate<Args...>&) = delete;

        EventDelegate(function<void(Args...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(void(*theFunctionNew)(Args...)) {
            this->theFunction = static_cast<function<void(Args...)>>(theFunctionNew);
        }

        EventToken eventToken{};

    protected:
        function<void(Args...)> theFunction{};
    };

    template<typename... Args>
    class Event {
    public:

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventToken add(EventDelegate<Args...> eventDelegate) {
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

        void operator()(Args...args) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        ObjectCache<string, EventDelegate<Args...>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
    };
}