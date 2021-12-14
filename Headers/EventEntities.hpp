/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    struct EventToken {

        template<typename Args>
        friend class Event;

        template<typename Args>
        friend class EventDelegate;

        EventToken() = default;

    protected:
        string handlerId{ "" };
        string eventId{ "" };

    };

    template<typename Arg>
    class EventDelegate {
    public:

        template<typename A>
        friend class Event;

        EventDelegate<Arg>& operator=(EventDelegate<Arg>&& other) {
            this->theFunction = move(other.theFunction);
            this->eventToken = move(other.eventToken);
            return *this;
        }

        EventDelegate(EventDelegate<Arg>&& other) {
            *this = move(other);
        }

        EventDelegate& operator=(EventDelegate<Arg>&other) {
            this->theFunction = other.theFunction;
            this->eventToken = other.eventToken;
            return *this;
        };

        EventDelegate(EventDelegate<Arg>&other){
            *this = other;
        }

        EventDelegate(function<void(Arg)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(void(*theFunctionNew)(Arg)) {
            this->theFunction = theFunctionNew;
        }

        EventToken eventToken{};

    protected:
        function<void(Arg)>theFunction{};
    };

    template<typename Arg>
    class Event {
    public:

        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }

        EventToken add(EventDelegate<Arg> eventDelegate) {
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

        void operator()(Argt arg) {
            lock_guard<mutex> accessLock{ this->accessMutex };
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(arg);
            }
        }

    protected:
        ObjectCache<string, EventDelegate<Arg>> theFunctions{};
        string eventId{ "" };
        mutex accessMutex{};
    };
}