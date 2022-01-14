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

        template<typename ReturnType, typename ...ArgTypes>
        friend class Event;

        template<typename ReturnType, typename ...ArgTypes>
        friend class EventDelegate;

        friend inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        EventDelegateToken() = default;

    protected:

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

    template<typename ReturnType, typename ...ArgTypes>
    class EventDelegate {
    public:

        template<typename ReturnType, typename... A>
        friend class Event;

        EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunction = move(other.theFunction);
                other.theFunction = function<ReturnType(ArgTypes...)>{};
            }
            return *this;
        }

        EventDelegate(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
            *this = move(other);
        }

        EventDelegate<ReturnType, ArgTypes...>& operator=(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate(EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate(function<ReturnType(ArgTypes...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(ReturnType(*theFunctionNew)(ArgTypes...)) {
            this->theFunction = theFunctionNew;
        }

    protected:
        function<ReturnType(ArgTypes...)>theFunction{};
    };

    template<typename ReturnType, typename  ...ArgTypes>
    class Event {
    public:

        Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunctions = move(other.theFunctions);
                other.theFunctions = map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>>{};
                this->eventId = move(other.eventId);
                other.eventId = string{};
            }
            return *this;
        }

        Event(Event<ReturnType, ArgTypes...>&& other) noexcept {
            *this = move(other);
        }

        Event<ReturnType, ArgTypes...>& operator=(const Event<ReturnType, ArgTypes...>&) = delete;

        Event(const Event<ReturnType, ArgTypes...>&) = delete;

        Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&) = delete;

        Event(Event<ReturnType, ArgTypes...>&) = delete;

        Event() {
            this->eventId = to_string(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<ReturnType, ArgTypes...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = to_string(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.insert_or_assign(eventToken, move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        void operator()(ArgTypes... args) {
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>> theFunctions{};
        string eventId{ "" };
    };

    template<typename ...ArgTypes>
    class EventDelegate<void, ArgTypes...> {
    public:

        friend class Event<void, ArgTypes...>;

        EventDelegate<void, ArgTypes...>& operator=(EventDelegate<void, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunction = move(other.theFunction);
                other.theFunction = function<void(ArgTypes...)>{};
            }
            return *this;
        }

        EventDelegate(EventDelegate<void, ArgTypes...>&& other) noexcept {
            *this = move(other);
        }

        EventDelegate<void, ArgTypes...>& operator=(const EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate(const EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate<void, ArgTypes...>& operator=(EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate(EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate(function<void(ArgTypes...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(void(*theFunctionNew)(ArgTypes...)) {
            this->theFunction = theFunctionNew;
        }

    protected:
        function<void(ArgTypes...)>theFunction{};
    };

    template<typename ...ArgTypes>
    class Event<void, ArgTypes...> {
    public:

        Event<void, ArgTypes...>& operator=(Event<void, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunctions = move(other.theFunctions);
                other.theFunctions = map<EventDelegateToken, EventDelegate<void, ArgTypes...>>{};
                this->eventId = move(other.eventId);
                other.eventId = string{};
            }
            return *this;
        }

        Event(Event<void, ArgTypes...>&& other) noexcept {
            *this = move(other);
        }

        Event<void, ArgTypes...>& operator=(const Event<void, ArgTypes...>&) = delete;

        Event(const Event<void, ArgTypes...>&) = delete;

        Event<void, ArgTypes...>& operator=(Event<void, ArgTypes...>&) = delete;

        Event(Event<void, ArgTypes...>&) = delete;

        Event() {
            this->eventId = to_string(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<void, ArgTypes...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = to_string(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.insert_or_assign(eventToken, move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        void operator()(ArgTypes... args) {
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:

        map<EventDelegateToken, EventDelegate<void, ArgTypes...>> theFunctions{};
        string eventId{ "" };

    };

    struct DiscordCoreAPI_Dll EventCore {

        friend class EventWaiter;

        EventCore& operator=(const EventCore&) = delete;

        EventCore(const EventCore&) = delete;

        EventCore& operator=(EventCore&) = delete;

        EventCore(EventCore&) = delete;

        EventCore() = default;

    protected:

        unique_ptr<bool> theEventState{ make_unique<bool>() };

    };

    class DiscordCoreAPI_Dll EventWaiter {
    public:

        shared_ptr<EventCore> theEventCore{ nullptr };
        atomic<bool*> theEventState{ nullptr };

        EventWaiter& operator=(EventWaiter& other) {
            this->theEventState.store(other.theEventState.load());
            this->theEventCore = other.theEventCore;
            return *this;
        }

        EventWaiter(EventWaiter& other) {
            *this = other;
        }

        EventWaiter() {
            this->theEventCore = make_shared<EventCore>();
            this->theEventState.store(this->theEventCore->theEventState.get());
        }

        uint32_t wait(uint64_t millisecondsMaxToWait = UINT64_MAX) {
            uint64_t millisecondsWaited{ 0 };
            while (true) {
                if (*this->theEventState.load()) {
                    return 0;
                }
                else {
                    int64_t startTime = duration_cast<milliseconds, int64_t>(steady_clock::now().time_since_epoch()).count();
                    this_thread::sleep_for(milliseconds(1));
                    int64_t endTime = duration_cast<milliseconds, int64_t>(steady_clock::now().time_since_epoch()).count();
                    millisecondsWaited += endTime - startTime;
                }
                if (millisecondsWaited >= millisecondsMaxToWait) {
                    return 1;
                }
            }
        }

        void set() {
            auto thePtr = this->theEventState.load();
            *thePtr = true;
            this->theEventState.store(thePtr);
        }

        void reset() {
            auto thePtr = this->theEventState.load();
            *thePtr = false;
            this->theEventState.store(thePtr);
        }
    };


}