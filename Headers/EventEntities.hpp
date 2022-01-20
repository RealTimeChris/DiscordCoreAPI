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

        template<typename ReturnType, typename... A>
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
            this->eventId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<ReturnType, ArgTypes...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
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
        std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>> theFunctions{};
        std::string eventId{ "" };
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
            this->eventId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        }

        EventDelegateToken add(EventDelegate<void, ArgTypes...> eventDelegate) {
            EventDelegateToken eventToken{};
            eventToken.handlerId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
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

        friend EventWaiter;

        EventCore& operator=(const EventCore&) = delete;

        EventCore(const EventCore&) = delete;

        EventCore& operator=(EventCore&) = delete;

        EventCore(EventCore&) = delete;

        EventCore() = default;

    protected:

        std::unique_ptr<bool> theEventState{ std::make_unique<bool>() };

    };

    class DiscordCoreAPI_Dll EventWaiter {
    public:

        std::unique_ptr<EventCore> theEventCoreMain{ nullptr };
        std::atomic<bool*> theEventState{ nullptr };
        EventCore* theEventCore{ nullptr };

        EventWaiter& operator=(EventWaiter& other) {
            this->theEventState.store(other.theEventState.load());
            this->theEventCore = other.theEventCore;
            return *this;
        }

        EventWaiter(EventWaiter& other) {
            *this = other;
        }

        EventWaiter() {
            this->theEventCoreMain = std::make_unique<EventCore>();
            this->theEventCore = this->theEventCoreMain.get();
            this->theEventState.store(this->theEventCore->theEventState.get());
        }

        uint32_t wait(uint64_t millisecondsMaxToWait = UINT64_MAX) {
            uint64_t millisecondsWaited{ 0 };
            while (true) {
                int64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::steady_clock::now().time_since_epoch()).count();
                if (*this->theEventState.load()) {
                    return 0;
                }
                else if (millisecondsMaxToWait - millisecondsWaited <= 20) {

                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                int64_t endTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::steady_clock::now().time_since_epoch()).count();
                millisecondsWaited += endTime - startTime;
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