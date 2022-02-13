/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

   struct DiscordCoreAPI_Dll EventDelegateToken {

        template<typename ReturnType, typename ...ArgTypes>
        friend class EventDelegate;

        template<typename ReturnType, typename ...ArgTypes>
        friend class Event;

        friend inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        friend inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

        EventDelegateToken() = default;

    protected:

        std::string handlerId{ "" };
        std::string eventId{ "" };

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

        template<typename ReturnType02, typename... ArgTypes02>
        friend class Event;

        EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunction = std::move(other.theFunction);
                other.theFunction = std::function<ReturnType(ArgTypes...)>{};
            }
            return *this;
        }

        EventDelegate(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
            *this = std::move(other);
        }

        EventDelegate<ReturnType, ArgTypes...>& operator=(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate(EventDelegate<ReturnType, ArgTypes...>& other) = delete;

        EventDelegate(std::function<ReturnType(ArgTypes...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(ReturnType(*theFunctionNew)(ArgTypes...)) {
            this->theFunction = theFunctionNew;
        }

    protected:
        std::function<ReturnType(ArgTypes...)>theFunction{};
    };

    template<typename ReturnType, typename  ...ArgTypes>
    class UniEvent {
    public:

        UniEvent<ReturnType, ArgTypes...>& operator=(UniEvent<ReturnType, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunction = std::move(other.theFunction);
                other.theFunction = std::function<ReturnType(ArgTypes...)>{};
            }
            return *this;
        }

        UniEvent(UniEvent<ReturnType, ArgTypes...>&& other) noexcept {
            *this = std::move(other);
        }

        UniEvent<ReturnType, ArgTypes...>& operator=(const UniEvent<ReturnType, ArgTypes...>&) = delete;

        UniEvent(const UniEvent<ReturnType, ArgTypes...>&) = delete;

        UniEvent<ReturnType, ArgTypes...>& operator=(UniEvent<ReturnType, ArgTypes...>&) = delete;

        UniEvent(UniEvent<ReturnType, ArgTypes...>&) = delete;

        UniEvent() {};

        UniEvent(std::function<ReturnType(ArgTypes...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        UniEvent(ReturnType(*theFunctionNew)(ArgTypes...)) {
            this->theFunction = theFunctionNew;
        }       

        void operator()(ArgTypes... args) {
            this->theFunction(args...);
        }

    protected:
        std::function<ReturnType(ArgTypes...)>theFunction{};
    };

    template<typename ReturnType, typename  ...ArgTypes>
    class Event {
    public:

        Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunctions = std::move(other.theFunctions);
                other.theFunctions = std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>>{};
                this->eventId = std::move(other.eventId);
                other.eventId = std::string{};
            }
            return *this;
        }

        Event(Event<ReturnType, ArgTypes...>&& other) noexcept {
            *this = std::move(other);
        }

        Event<ReturnType, ArgTypes...>& operator=(const Event<ReturnType, ArgTypes...>&) = delete;

        Event(const Event<ReturnType, ArgTypes...>&) = delete;

        Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&) = delete;

        Event(Event<ReturnType, ArgTypes...>&) = delete;

        Event() {
            this->eventId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<ReturnType, ArgTypes...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.insert_or_assign(eventToken, std::move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            if (this->theFunctions.contains(eventToken)) {
                this->theFunctions.erase(eventToken);
            }
        }

        void operator()(ArgTypes... args) {
            std::unique_lock<std::mutex> accessLock{ this->accessMutex };
            for (auto& [key, value] : this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>> theFunctions{};
        std::string eventId{ "" };
        std::mutex accessMutex{};
    };

    template<typename ...ArgTypes>
    class EventDelegate<void, ArgTypes...> {
    public:

        friend Event<void, ArgTypes...>;

        EventDelegate<void, ArgTypes...>& operator=(EventDelegate<void, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunction = std::move(other.theFunction);
                other.theFunction = std::function<void(ArgTypes...)>{};
            }
            return *this;
        }

        EventDelegate(EventDelegate<void, ArgTypes...>&& other) noexcept {
            *this = std::move(other);
        }

        EventDelegate<void, ArgTypes...>& operator=(const EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate(const EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate<void, ArgTypes...>& operator=(EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate(EventDelegate<void, ArgTypes...>& other) = delete;

        EventDelegate(std::function<void(ArgTypes...)> theFunctionNew) {
            this->theFunction = theFunctionNew;
        }

        EventDelegate(void(*theFunctionNew)(ArgTypes...)) {
            this->theFunction = theFunctionNew;
        }

    protected:
        std::function<void(ArgTypes...)>theFunction{};
    };

    template<typename ...ArgTypes>
    class Event<void, ArgTypes...> {
    public:

        Event<void, ArgTypes...>& operator=(Event<void, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunctions = std::move(other.theFunctions);
                other.theFunctions = std::map<EventDelegateToken, EventDelegate<void, ArgTypes...>>{};
                this->eventId = std::move(other.eventId);
                other.eventId = std::string{};
            }
            return *this;
        }

        Event(Event<void, ArgTypes...>&& other) noexcept {
            *this = std::move(other);
        }

        Event<void, ArgTypes...>& operator=(const Event<void, ArgTypes...>&) = delete;

        Event(const Event<void, ArgTypes...>&) = delete;

        Event<void, ArgTypes...>& operator=(Event<void, ArgTypes...>&) = delete;

        Event(Event<void, ArgTypes...>&) = delete;

        Event() {
            this->eventId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<void, ArgTypes...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
            eventToken.eventId = this->eventId;
            this->theFunctions.insert_or_assign(eventToken, std::move(eventDelegate));
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

        std::map<EventDelegateToken, EventDelegate<void, ArgTypes...>> theFunctions{};
        std::string eventId{ "" };

    };

   struct DiscordCoreAPI_Dll EventCore {

        EventCore& operator=(const EventCore&) = delete;

        EventCore(const EventCore&) = delete;

        EventCore& operator=(EventCore&) = delete;

        EventCore(EventCore&) = delete;

        bool* getEventState() {
            return this->theEventState.get();
        }

        EventCore() = default;

    protected:
        
        std::unique_ptr<bool> theEventState{ std::make_unique<bool>() };
    };

    class DiscordCoreAPI_Dll EventWaiter {
    public:

        std::atomic<bool*> theEventState{ nullptr };
        ReferenceCountingPtr<EventCore> theEventCore{ nullptr };

        EventWaiter& operator=(const EventWaiter& other) {
            this->theEventState.store(other.theEventState.load(std::memory_order_consume), std::memory_order_release);
            this->theEventCore = other.theEventCore;
            return *this;
        }

        EventWaiter(const EventWaiter& other) {
            *this = other;
        }

        EventWaiter& operator=(EventWaiter& other) {
            this->theEventState.store(other.theEventState.load(std::memory_order_consume), std::memory_order_release);
            this->theEventCore = other.theEventCore;
            return *this;
        }

        EventWaiter(EventWaiter& other) {
            *this = other;
        }

        EventWaiter() {
            this->theEventCore = new EventCore{};
            this->theEventState.store(this->theEventCore->getEventState(), std::memory_order_release);
        }

        bool wait(int64_t millisecondsMaxToWait = INT64_MAX) {
            int64_t millisecondsWaited{ 0 };
            int64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
            while (true) {
                if (*this->theEventState.load(std::memory_order_consume)) {
                    return false;
                }
                else if (millisecondsMaxToWait - millisecondsWaited <= 20) {

                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
                millisecondsWaited = currentTime - startTime;
                if (millisecondsWaited >= millisecondsMaxToWait) {
                    return true;
                }
            }
        }

        void set() {
            auto thePtr = this->theEventState.load(std::memory_order_consume);
            *thePtr = true;
            this->theEventState.store(thePtr, std::memory_order_release);
        }

        void reset() {
            auto thePtr = this->theEventState.load(std::memory_order_consume);
            *thePtr = false;
            this->theEventState.store(thePtr, std::memory_order_release);
        }
    };


}
