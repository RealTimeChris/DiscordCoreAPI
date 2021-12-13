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
        string handlerId{};
        string eventId{};
    };

   

    template <typename... Args>
    struct EventHandlerSmall {

        function<void(Args...)> theFunction{};
        EventToken idToken{};

        EventHandlerSmall(void(*function)(Args...)) {
            this->theFunction = function;
        };
        
        typedef  void(*EventHandlerNew)(Args...);        
    };

    template <typename... Args>
    struct EventHandlerSmall<void(Args...)> {

        function<void(Args...)> theFunction{};
        EventToken idToken{};

        EventHandlerSmall(void(*function)(Args...)) {
            this->theFunction = function;
        };
        
        typedef  void(*EventHandlerNew)(Args...);
    };

    template <typename ArgType>
    struct Event
    {
        Event() {
            this->eventId = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        }
        ;
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
        template<typename ArgType>
        EventToken add(EventHandlerSmall<ArgType> delegateNew) {
            lock_guard<mutex> change_guard{ changeMutex };
            EventToken idToken = createToken();
            delegateNew.idToken = idToken;
            this->currentHandlers.storeValue(idToken.handlerId, delegateNew);
            return idToken;
        }

        void remove(EventToken const token)
        {

            lock_guard<mutex> change_guard{ changeMutex };
            this->currentHandlers.erase(token.handlerId);
        }

        template<typename...Arg>
        void operator()(Arg... args)
        {
            lock_guard<mutex> swap_guard{ swapMutex };
            for (auto[key, value]: this->currentHandlers)
            {
                value.theFunction(args...);
            }
        }

    private:

        static ObjectCache<string, bool> currentStates;

        ObjectCache<string, ArgType> currentHandlers{};
        StopWatch<chrono::milliseconds> timerWatch{ 0 };
        string eventId{ "" };
        mutex changeMutex{};
        mutex swapMutex{};

        EventToken createToken() const noexcept
        {
            return EventToken{ to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()), eventId };
        }        
    };
    
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
    

	//template<typename EventHandlerSmall>
	//ObjectCache<string, bool> Event<EventHandlerSmall>::currentStates{};
}