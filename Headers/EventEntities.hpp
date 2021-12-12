/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    struct EventToken {

        template<typename EventHander>
        friend struct Event;

        EventToken() = default;

        EventToken(string handlerIdNew, string eventIdNew) :handlerId(handlerId), eventId(eventIdNew) {};

    protected:
        const string handlerId{};
        const string eventId{};
    };

    template <typename R, typename... Args>
    struct __declspec(empty_bases)EventHandlerBase : Windows::Foundation::IUnknown
    {
        EventHandlerBase(std::nullptr_t = nullptr) noexcept {}
        EventHandlerBase(void* ptr, take_ownership_from_abi_t) noexcept : IUnknown(ptr, take_ownership_from_abi) {};

        template <typename F> EventHandlerBase(F* handler) :
            EventHandlerBase([=](auto&& ... args) { return handler(args...); })
        {}

        template <typename O, typename M> EventHandlerBase(O* object, M method) :
            EventHandlerBase([=](auto&& ... args) { return ((*object).*(method))(args...); })
        {}

        template <typename O, typename M> EventHandlerBase(com_ptr<O>&& object, M method) :
            EventHandlerBase([o = std::move(object), method](auto&& ... args) { return ((*o).*(method))(args...); })
        {
        }

        template <typename O, typename M> EventHandlerBase(winrt::weak_ref<O>&& object, M method) :
            EventHandlerBase([o = std::move(object), method](auto&& ... args) { if (auto s = o.get()) { ((*s).*(method))(args...); } })
        {
        }

        auto operator()(Args const& ... args) const
        {
            return (*(impl::variadic_delegate_abi<R, Args...> **)this)->invoke(args...);
        }

    };

    template <typename... Args>
    struct __declspec(empty_bases)EventHandler : EventHandlerBase<void, Args...>
    {
        typedef  void(*EventHandlerNew)(Args...);
        using EventHandlerBase<void, Args...>::EventHandlerBase;
    };

    template <typename R, typename... Args>
    struct __declspec(empty_bases)EventHandler<R(Args...)> : EventHandlerBase<R, Args...>
    {
        typedef  void(*EventHandlerNew)(Args...);
        using EventHandlerBase<R, Args...>::EventHandlerBase;
    };

    template <typename ArgType>
    struct Event
    {
        using delegate_type = ArgType;
        Event() = default;
        Event(Event<ArgType> const&other) {
            *this = other;
        }
        Event<ArgType>& operator =(Event<ArgType> const&other) {
            this->currentHandlers = other.currentHandlers;
            this->currentStates = other.currentStates;
            this->timerWatch = other.timerWatch;
            this->targets = move(other.targets);
            this->eventId = other.eventId;
        };

        explicit operator bool() const noexcept
        {
            return this->targets != nullptr;
        }

        EventToken add(delegate_type const& delegate)
        {
            EventToken token{};

            // Extends life of old this->targets array to release delegates outside of lock.
            HandlerArray temp_targets;

            {
                lock_guard<mutex> change_guard{ changeMutex };
                HandlerArray new_targets = impl::make_event_array<delegate_type>((!this->targets) ? 1 : this->targets->size() + 1);

                if (this->targets)
                {
                    std::copy_n(this->targets->begin(), this->targets->size(), new_targets->begin());
                }

                new_targets->back() = impl::make_agile_delegate(delegate);
                get_token(new_targets->back());

                slim_lock_guard const swap_guard(swapMutex);
                temp_targets = std::exchange(this->targets, std::move(new_targets));
            }

            return EventToken();
        }

        void remove(EventToken const token)
        {
            // Extends life of old this->targets array to release delegates outside of lock.
            HandlerArray temp_targets;

            {
                lock_guard<mutex> change_guard{ changeMutex };

                if (!this->targets)
                {
                    return;
                }

                uint32_t available_slots = this->targets->size() - 1;
                HandlerArray new_targets;
                bool removed = false;

                if (available_slots == 0)
                {
                    removed = true;
                }
                else
                {
                    new_targets = make_unique<vector<EventHandler<ArgType>>>();
                    auto new_iterator = new_targets->begin();
                }

                if (removed)
                {
                    lock_guard<mutex> swap_guard{ swapMutex };
                    temp_targets = std::exchange(this->targets, std::move(new_targets));
                }
            }
        }

        template<typename...Arg>
        void operator()(Arg const&... args)
        {
            HandlerArray temp_targets;

            {
                lock_guard<mutex> swap_guard{ swapMutex };
                temp_targets = move(this->targets);
            }

            if (temp_targets)
            {
                for (EventHandler<ArgType> element : *temp_targets)
                {
                    if (!impl::invoke(element, args...))
                    {
                        remove(createToken());
                    }
                }
            }
        }

    private:

        EventToken createToken() const noexcept
        {
            return EventToken{ to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()), to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()) };
        }

        using HandlerArray = unique_ptr<vector<EventHandler<ArgType>>>;

        HandlerArray targets{ make_unique<vector<EventHandler<ArgType>>>() };
        mutex changeMutex{};
        mutex swapMutex{};
        static ObjectCache<string, bool> currentStates;
        ObjectCache<string, EventHandler<ArgType>> currentHandlers{};
        unique_ptr<mutex> accessMutex{ make_unique<mutex>() };
        StopWatch<chrono::milliseconds> timerWatch{ 0 };
        string eventId{ "" };
    };
    /*
    * 
    * 

    template <typename EventHandler>
    struct Event {
    public:
        Event() = default;
        Event(Event<EventHandler> &other) {
            *this = other;
        }
        Event<EventHandler>& operator=(Event<EventHandler> & other) {
            this->currentHandlers = other.currentHandlers;
            this->this->targets = move(other.this->targets);
            this->timerWatch = other.timerWatch;
            this->eventId = other.eventId;
        } 

        explicit operator bool() const noexcept
        {
            return this->this->targets != nullptr;
        }

        EventToken add(EventHandler const& delegate)
        {
            lock_guard<mutex> addLock{ *this->accessMutex };
            EventToken returnToken{ to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()), this->eventId };
            this->currentHandlers.storeValue(returnToken.handlerId, delegate);
            return returnToken;
        }

        void remove(EventToken const token)
        {
            HandlerArray tempTargets{};

            {
                lock_guard<mutex> changeGuard{ this->changeMutex };

                if (!this->this->targets)
                {
                    return;
                }

                uint64_t availableSlots = this->this->targets->size() - 1;
                HandlerArray newTargets{};
                bool removed{ false };

                if (availableSlots == 0)
                {
                    removed = true;
                }
                else
                {
                    newTargets = make_unique<vector<EventHandler>>();
                    auto newIterator = newTargets->begin();

                    for (EventHandler const& element : *this->this->targets)
                    {
                        if (!removed) 
                        {
                            removed = true;
                            continue;
                        }

                        if (availableSlots == 0)
                        {
                            WINRT_ASSERT(!removed);
                            break;
                        }

                        *newIterator = element;
                        ++newIterator;
                        --availableSlots;
                    }
                }

                if (removed)
                {
                    lock_guard<mutex> swapGuard{ *this->accessMutex };
                    tempTargets = exchange(this->this->targets, move(newTargets));
                }
            }
        }
        
        template<typename...Arg>
        void operator()(Arg const&... args)
        {
            HandlerArray tempTargets{};

            {
                lock_guard<mutex> swapLock{ *this->accessMutex };
                tempTargets = move(this->this->targets);
            }

            if (tempTargets)
            {
                for (EventHandler const& element : *tempTargets)
                {
                    invoke(element, args...);
                }
            }
            this->this->targets = move(tempTargets);
        }
        
    protected:

        EventToken createToken() const noexcept
        {
            return EventToken{ to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()), to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()) };
        }

        using HandlerArray = unique_ptr<vector<EventHandler>>;

        HandlerArray this->targets{ make_unique<vector<EventHandler>>() };
        mutex changeMutex{};
        static ObjectCache<string, bool> currentStates;
        ObjectCache<string, EventHandler> currentHandlers{};
        unique_ptr<mutex> accessMutex{ make_unique<mutex>() };
        StopWatch<chrono::milliseconds> timerWatch{ 0 };
        string eventId{ "" };
    };
    */
	template<>
	class Event<void> {
	public:
		Event() {};

		uint32_t wait(uint64_t milliseconds = 0) {

		}

		void set() {

		}

		void reset() {

		}

	protected:

		static ObjectCache<string, bool> currentStates;
		string eventId{ "" };
	};


	//template<typename EventHandler>
	//ObjectCache<string, bool> Event<EventHandler>::currentStates{};
}