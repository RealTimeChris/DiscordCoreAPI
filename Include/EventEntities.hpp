/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include <FoundationEntities.hpp>
#include <CoRoutine.hpp>
#include <iostream>

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
                this->theFunction.swap(other.theFunction);
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
                this->theFunctions.swap(other.theFunctions);
                other.theFunctions = std::make_unique<std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>>>();
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
            this->theFunctions->insert_or_assign(eventToken, std::move(eventDelegate));
            return eventToken;
        }

        void remove(EventDelegateToken  eventToken) {
            if (this->theFunctions->contains(eventToken)) {
                this->theFunctions->erase(eventToken);
            }
        }

        void operator()(ArgTypes... args) {
            for (auto& [key, value] : *this->theFunctions) {
                value.theFunction(args...);
            }
        }

    protected:
        std::unique_ptr<std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>>> theFunctions{ std::make_unique<std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>>>() };
        std::string eventId{ "" };
    };

    template<typename ReturnType, typename  ...ArgTypes>
    class UniEvent {
    public:

        UniEvent<ReturnType, ArgTypes...>& operator=(UniEvent<ReturnType, ArgTypes...>&& other) noexcept {
            if (this != &other) {
                this->theFunction.swap(other.theFunction);
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
            if (this->theTask.getStatus() == CoRoutineStatus::Running) {
                this->theTask.cancel();
            }
            this->theTask = this->theFunction(args...);
        }

        ~UniEvent() {
            this->theTask.cancel();
        }

    protected:
        std::function<ReturnType(ArgTypes...)> theFunction{ nullptr };
        CoRoutine<void> theTask{ nullptr };
    };

    class DiscordCoreAPI_Dll EventWaiter {
    public:

        ReferenceCountingPtr<std::atomic<bool>> theEventState{ nullptr };

        EventWaiter& operator=(const EventWaiter& other) {
            this->theEventState = other.theEventState;
            return *this;
        }

        EventWaiter(const EventWaiter& other) {
            *this = other;
        }

        EventWaiter& operator=(EventWaiter& other) {
            this->theEventState = other.theEventState;
            return *this;
        }

        EventWaiter(EventWaiter& other) {
            *this = other;
        }

        EventWaiter() {
            this->theEventState = new std::atomic<bool>{ true };
        }

        bool wait(int64_t millisecondsMaxToWait = INT64_MAX) {
            int64_t millisecondsWaited{ 0 };
            int64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
            while (true) {
                if (this->theEventState->load(std::memory_order_seq_cst)) {
                    return true;
                }
                else if (millisecondsMaxToWait - millisecondsWaited <= 20) {

                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
                millisecondsWaited = currentTime - startTime;
                if (millisecondsWaited >= millisecondsMaxToWait) {
                    return false;
                }
            }
        }

        void set() {
            this->theEventState->store(true, std::memory_order_seq_cst);
        }

        void reset() {
            this->theEventState->store(false, std::memory_order_seq_cst);
        }
    };


}
