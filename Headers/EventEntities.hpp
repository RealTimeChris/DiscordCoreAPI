/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {
    
    struct EventToken {

        template<typename EventHandler>
        friend class Event;

        template<typename EventHandler >
        friend class EventDelegate;

    protected:
        string handlerId{ "" };
        string eventId{ "" };

        EventToken() = default;
    };

    template<typename ArgOne>
    class EventDelegate {
    public:

        template<typename A>
        friend class Event;

        EventDelegate<ArgOne>& operator=(EventDelegate<ArgOne>&&other) {
            this->theFunction = move(other.theFunction);
            this->eventToken = move(other.eventToken);
            return *this;
        }

        EventDelegate(EventDelegate<ArgOne>&& other) {
            *this = move(other);
        }

        EventDelegate<ArgOne>& operator=(EventDelegate<ArgOne>&) = delete;

        EventDelegate<ArgOne>(EventDelegate<ArgOne>&) = delete;

        EventDelegate<ArgOne>(void(*theFunctionNew)(ArgOne)) {
            this->theFunction = static_cast<function<void(ArgOne)>>(theFunctionNew);
        }

        EventToken eventToken{};

    protected:
        function<void(ArgOne)> theFunction{};
    };

    template<typename ArgOne>
    class Event {
    public:

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventToken add(EventDelegate<ArgOne> eventDelegate) {
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

        void operator()(ArgOne arg) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(arg);
            }
        }

    protected:
        ObjectCache<string, EventDelegate<ArgOne>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
    };
}