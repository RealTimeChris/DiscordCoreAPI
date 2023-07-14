/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file EventEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/**
		 * \addtogroup discord_events
		 * @{
		 */

		/// \brief Event-delegate token, representing an event.
		struct DiscordCoreAPI_Dll EventDelegateToken {
			template<typename ReturnType, typename... ArgTypes> friend class EventDelegate;

			template<typename ReturnType, typename... ArgTypes> friend class Event;

			template<typename ReturnType, typename... ArgTypes> friend class TriggerEventDelegate;

			template<typename ReturnType, typename... ArgTypes> friend class TriggerEvent;

			inline bool operator==(const EventDelegateToken& rhs) const {
				return stoll(handlerId) == stoll(rhs.handlerId) && stoll(eventId) == stoll(rhs.eventId);
			}

			inline bool operator<(const EventDelegateToken& rhs) const {
				return stoll(handlerId) < stoll(rhs.handlerId);
			}

			EventDelegateToken() = default;

			std::string handlerId{};
			std::string eventId{};
		};

		/// \brief Event-delegate, for representing an event-function to be executed.
		template<typename ReturnType, typename... ArgTypes> class EventDelegate {
		  public:
			template<typename RTy02, typename... ArgTypes02> friend class Event;

			EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
				if (this != &other) {
					function.swap(other.function);
					other.function = std::function<ReturnType(ArgTypes...)>{};
				}
				return *this;
			}

			EventDelegate(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
				*this = std::move(other);
			}

			EventDelegate<ReturnType, ArgTypes...>& operator=(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

			EventDelegate(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

			EventDelegate<ReturnType, ArgTypes...>& operator=(std::function<ReturnType(ArgTypes...)> functionNew) {
				function = functionNew;
				return *this;
			}

			/// \brief Constructor, taking a std::function<ReturnType(ArgTypes.)> as an argument.
			EventDelegate(std::function<ReturnType(ArgTypes...)> functionNew) {
				*this = functionNew;
			}

			EventDelegate<ReturnType, ArgTypes...>& operator=(ReturnType (*functionNew)(ArgTypes...)) {
				function = functionNew;
				return *this;
			}

			/// \brief Constructor, taking a pointer to a function of type ReturnType(*)(ArgTypes...) as an argument.
			EventDelegate(ReturnType (*functionNew)(ArgTypes...)) {
				*this = functionNew;
			}

			EventDelegate() = default;

		  protected:
			std::function<ReturnType(ArgTypes...)> function{};
		};

		template<typename ReturnType, typename... ArgTypes> class Event {
		  public:
			std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>> functions{};

			Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&& other) noexcept {
				if (this != &other) {
					functions = std::move(other.functions);
					other.functions.clear();
					eventId = std::move(other.eventId);
					other.eventId.clear();
				}
				return *this;
			}

			Event(Event<ReturnType, ArgTypes...>&& other) noexcept {
				*this = std::move(other);
			}

			Event<ReturnType, ArgTypes...>& operator=(const Event<ReturnType, ArgTypes...>&) = delete;

			Event(const Event<ReturnType, ArgTypes...>&) = delete;

			Event() {
				std::unique_lock lock{ accessMutex };
				eventId = std::to_string(std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count());
			}

			EventDelegateToken add(EventDelegate<ReturnType, ArgTypes...> eventDelegate) {
				std::unique_lock lock{ accessMutex };
				EventDelegateToken eventToken{};
				eventToken.handlerId = std::to_string(std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count());
				eventToken.eventId = eventId;
				functions[eventToken] = std::move(eventDelegate);
				return eventToken;
			}

			void erase(EventDelegateToken eventToken) {
				std::unique_lock lock{ accessMutex };
				if (eventToken.eventId == eventId) {
					if (functions.contains(eventToken)) {
						functions.erase(eventToken);
					}
				}
			}

			void operator()(const ArgTypes... args) {
				std::unique_lock lock{ accessMutex };
				for (auto& [key, value]: functions) {
					try {
						value.function(args...).get();
					} catch (const DCAException& error) {
						MessagePrinter::printError<PrintMessageType::General>(error.what());
					}
				}
				return;
			}

		  protected:
			std::mutex accessMutex{};
			std::string eventId{};
		};

		/// \brief An event that gets fired depending on the result of a "trigger-function" return value.
		template<typename ReturnType, typename... ArgTypes> class TriggerEventDelegate {
		  public:
			template<typename RTy02, typename... ArgTypes02> friend class TriggerEvent;

			TriggerEventDelegate<ReturnType, ArgTypes...>& operator=(TriggerEventDelegate<ReturnType, ArgTypes...>&& other) {
				if (this != &other) {
					function.swap(other.function);
					other.function = std::function<ReturnType(ArgTypes...)>{};
					testFunction.swap(other.testFunction);
					other.testFunction = std::function<bool(ArgTypes...)>{};
				}
				return *this;
			}

			TriggerEventDelegate(TriggerEventDelegate<ReturnType, ArgTypes...>&& other) {
				*this = std::move(other);
			}

			TriggerEventDelegate<ReturnType, ArgTypes...>& operator=(const TriggerEventDelegate<ReturnType, ArgTypes...>& other) = delete;

			TriggerEventDelegate(const TriggerEventDelegate<ReturnType, ArgTypes...>& other) = delete;

			TriggerEventDelegate<ReturnType, ArgTypes...>& operator=(std::function<ReturnType(ArgTypes...)> functionNew) {
				function = functionNew;
				return *this;
			}

			/// \brief Constructor, taking a std::function<ReturnType(ArgTypes..)> as an argument.
			TriggerEventDelegate(std::function<ReturnType(ArgTypes...)> functionNew) {
				*this = functionNew;
			}

			TriggerEventDelegate<ReturnType, ArgTypes...>& operator=(ReturnType (*functionNew)(ArgTypes...)) {
				function = functionNew;
				return *this;
			}

			void setTestFunction(std::function<bool(ArgTypes...)> testFunctionNew) {
				testFunction = testFunctionNew;
			}

			void setTestFunction(bool (*testFunctionNew)(ArgTypes...)) {
				testFunction = testFunctionNew;
			}

			/// \brief Constructor, taking a pointer to a function of type ReturnType(*)(ArgTypes...) as an argument.
			TriggerEventDelegate(ReturnType (*functionNew)(ArgTypes...)) {
				*this = functionNew;
			}

			TriggerEventDelegate() = default;

		  protected:
			std::function<bool(ArgTypes...)> testFunction{};
			std::function<ReturnType(ArgTypes...)> function{};
		};

		/**@}*/

		template<typename ReturnType, typename... ArgTypes> class TriggerEvent {
		  public:
			std::map<EventDelegateToken, TriggerEventDelegate<ReturnType, ArgTypes...>> functions{};

			TriggerEvent<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&& other) {
				if (this != &other) {
					functions = std::move(other.functions);
					other.functions = std::map<EventDelegateToken, TriggerEventDelegate<ReturnType, ArgTypes...>>{};
					eventId = std::move(other.eventId);
					other.eventId = std::string{};
				}
				return *this;
			}

			TriggerEvent(Event<ReturnType, ArgTypes...>&& other) {
				*this = std::move(other);
			}

			TriggerEvent<ReturnType, ArgTypes...>& operator=(const Event<ReturnType, ArgTypes...>&) = delete;

			TriggerEvent(const TriggerEvent<ReturnType, ArgTypes...>&) = delete;

			TriggerEvent() {
				std::unique_lock lock{ accessMutex };
				eventId = std::to_string(std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count());
			}

			EventDelegateToken add(TriggerEventDelegate<ReturnType, ArgTypes...> eventDelegate) {
				std::unique_lock lock{ accessMutex };
				EventDelegateToken eventToken{};
				eventToken.handlerId = std::to_string(std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count());
				eventToken.eventId = eventId;
				functions[eventToken] = std::move(eventDelegate);
				return eventToken;
			}

			void erase(EventDelegateToken eventToken) {
				std::unique_lock lock{ accessMutex };
				if (eventToken.eventId == eventId) {
					if (functions.contains(eventToken)) {
						functions.erase(eventToken);
					}
				}
			}

			void operator()(ArgTypes&... args) {
				std::unique_lock lock{ accessMutex };
				for (auto iterator = functions.begin(); iterator != functions.end(); ++iterator) {
					if (iterator.operator*().second.testFunction(args...)) {
						iterator.operator*().second.function(args...);
						iterator = functions.erase(iterator);
					}
				}
				return;
			}

		  protected:
			std::mutex accessMutex{};
			std::string eventId{};
		};

	}
}
