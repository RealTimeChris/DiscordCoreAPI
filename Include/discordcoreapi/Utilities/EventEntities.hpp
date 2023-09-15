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
		 * \addtogroup utilities
		 * @{
		 */

		/// @brief Struct representing an event delegate token, associated with an event.
		struct EventDelegateToken {
			/// @brief Equality operator for comparing EventDelegateToken instances.
			/// @param rhs The right-hand side EventDelegateToken to compare against.
			/// @return bool True if the EventDelegateToken instances are equal, otherwise false.
			inline bool operator==(const EventDelegateToken& rhs) const {
				return handlerId == rhs.handlerId && eventId == rhs.eventId;
			}

			/// @brief Less-than operator for comparing EventDelegateToken instances.
			/// @param rhs The right-hand side EventDelegateToken to compare against.
			/// @return bool True if this EventDelegateToken is less than rhs, otherwise false.
			inline bool operator<(const EventDelegateToken& rhs) const {
				return handlerId < rhs.handlerId;
			}

			/// @brief Default constructor for EventDelegateToken.
			inline EventDelegateToken() = default;

			uint64_t handlerId{};///< Identifier for the handler.
			uint64_t eventId{};///< Identifier for the event.
		};

	};

	template<EventDelegateTokenT ValueType> struct KeyHasher<ValueType> {
		inline uint64_t operator()(const ValueType& data) const {
			uint64_t values[2]{};
			values[0] = data.eventId;
			values[0] = data.handlerId;
			return internalHashFunction(values, std::size(values) * sizeof(uint64_t));
		};
	};

	namespace DiscordCoreInternal {

		/// @brief Event-delegate, representing an event function to be executed.
		template<typename ReturnType, typename... ArgTypes> class EventDelegate {
		  public:
			template<typename RTy02, typename... ArgTypes02> friend class Event;

			/// @brief Default constructor for EventDelegate class.
			EventDelegate() = default;

			inline EventDelegate& operator=(const EventDelegate& other) noexcept = delete;
			inline EventDelegate(const EventDelegate& other) noexcept			 = delete;

			/// @brief Move assignment operator for the EventDelegate class.
			/// This operator moves the contents of another EventDelegate instance, 'other', into the current instance.
			/// It swaps the 'function' member between 'other' and the current instance, and then clears 'other' function.
			/// @param other The EventDelegate instance to be moved from.
			/// @returns Reference to the current EventDelegate instance after the move assignment.
			inline EventDelegate& operator=(EventDelegate&& other) noexcept {
				if (this != &other) {
					function.swap(other.function);
				}
				return *this;
			}

			/// @brief Move constructor for the EventDelegate class.
			/// This constructor moves the contents of another EventDelegate instance, 'other', into the current instance.
			/// It performs a move assignment using the 'operator=' function.
			/// @param other The EventDelegate instance to be moved from.
			inline EventDelegate(EventDelegate&& other) noexcept {
				// Perform move assignment using the 'operator=' function.
				*this = std::move(other);
			}

			/// @brief Assignment operator to set the delegate function.
			/// This operator assigns a new std::function, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew The std::function to assign as the current event.
			/// @returns Reference to the current EventDelegate instance after the assignment.
			inline EventDelegate& operator=(std::function<ReturnType(ArgTypes...)> functionNew) {
				function = functionNew;
				return *this;
			}

			/// @brief Constructor, taking a std::function<ReturnType(ArgTypes...)> as an argument.
			/// @param functionNew The function to construct as the current event.
			inline EventDelegate(std::function<ReturnType(ArgTypes...)> functionNew) {
				*this = functionNew;
			}

			/// @brief Assignment operator to set the delegate function using a function pointer.
			/// This operator assigns a new function pointer, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew The function pointer to assign as the current event.
			/// @returns Reference to the current EventDelegate instance after the assignment.
			inline EventDelegate& operator=(ReturnType (*functionNew)(ArgTypes...)) {
				function = functionNew;
				return *this;
			}

			/// @brief Constructor for the EventDelegate class, taking a pointer to a function as an argument.
			/// This constructor initializes the EventDelegate instance with the provided function pointer.
			/// It calls the assignment operator to set the delegate function using the provided function pointer.
			/// @param functionNew The function to construct as the current event.
			inline EventDelegate(ReturnType (*functionNew)(ArgTypes...)) {
				// Call the assignment operator to set the delegate function using the provided function pointer.
				*this = functionNew;
			}

		  protected:
			std::function<ReturnType(ArgTypes...)> function{};
		};

		/// @brief Template class representing an event that executes event functions.
		template<typename ReturnType, typename... ArgTypes> class Event {
		  public:
			UnorderedMap<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>> functions{};

			inline Event& operator=(const Event& other) noexcept = delete;
			inline Event(const Event& other) noexcept			 = delete;

			/// @brief Move assignment operator for the Event class.
			/// This operator moves the contents of another Event instance, 'other', into the current instance.
			/// It transfers the functions and eventId from 'other' to the current instance, clearing 'other' in the process.
			/// @param other The Event instance to be moved from.
			/// @returns Reference to the current Event instance after the move assignment.
			inline Event& operator=(Event&& other) noexcept {
				if (this != &other) {
					std::swap(functions, other.functions);
					std::swap(eventId, other.eventId);
				}
				return *this;
			}

			/// @brief Move constructor for the Event class.
			/// This constructor moves the contents of another Event instance, 'other', into the current instance.
			/// It performs a move assignment using the 'operator=' function.
			/// @param other The Event instance to be moved from.
			inline Event(Event&& other) noexcept {
				// Perform move assignment using the 'operator=' function.
				*this = std::move(other);
			}

			/// @brief Default constructor for Event class.
			inline Event() {
				std::unique_lock lock{ accessMutex };
				eventId = std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count();
			}

			/// @brief Add an event delegate to the event.
			/// @param eventDelegate The event delegate to add.
			/// @return EventDelegateToken The token representing the added event delegate.
			inline EventDelegateToken add(EventDelegate<ReturnType, ArgTypes...>&& eventDelegate) {
				std::unique_lock lock{ accessMutex };
				EventDelegateToken eventToken{};
				eventToken.handlerId  = std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count();
				eventToken.eventId	  = eventId;
				functions[eventToken] = std::move(eventDelegate);
				return eventToken;
			}

			/// @brief Remove an event delegate from the event.
			/// @param eventToken The token representing the event delegate to remove.
			inline void erase(const EventDelegateToken& eventToken) {
				std::unique_lock lock{ accessMutex };
				if (eventToken.eventId == eventId) {
					if (functions.contains(eventToken)) {
						functions.erase(eventToken);
					}
				}
			}

			/// @brief Invoke the event with provided arguments.
			/// @param args The arguments to pass to the event delegates.
			template<typename... ArgTypesNew> inline void operator()(ArgTypesNew&&... args) {
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
			uint64_t eventId{};
		};

		/// @brief Event-delegate, for representing an event-function to be executed conditionally.
		template<typename ReturnType, typename... ArgTypes> class TriggerEventDelegate {
		  public:
			template<typename RTy02, typename... ArgTypes02> friend class TriggerEvent;

			inline TriggerEventDelegate& operator=(const TriggerEventDelegate& other) noexcept = delete;
			inline TriggerEventDelegate(const TriggerEventDelegate& other) noexcept			   = delete;

			/// @brief Move assignment operator for TriggerEventDelegate class.
			/// This operator moves the contents of another TriggerEventDelegate instance ('other') into the current instance.
			/// @param other The TriggerEventDelegate instance to be moved.
			/// @returns Reference to the current TriggerEventDelegate instance after the move assignment.
			inline TriggerEventDelegate& operator=(TriggerEventDelegate&& other) noexcept {
				if (this != &other) {
					function.swap(other.function);
					testFunction.swap(other.testFunction);
				}
				return *this;
			}

			/// @brief Move constructor for TriggerEventDelegate class.
			/// This constructor moves the contents of another TriggerEventDelegate instance ('other') into the current instance.
			/// @param other The TriggerEventDelegate instance to be moved.
			inline TriggerEventDelegate(TriggerEventDelegate&& other) noexcept {
				*this = std::move(other);
			}

			/// @brief Assignment operator to set the delegate function using a std::function.
			/// This operator assigns a new std::function, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew The std::function to assign as the current event.
			/// @returns Reference to the current TriggerEventDelegate instance after the assignment.
			inline TriggerEventDelegate& operator=(std::function<ReturnType(ArgTypes...)> functionNew) {
				function = functionNew;
				return *this;
			}


			/// @brief Constructor for the TriggerEventDelegate class, taking a std::function as an argument.
			/// This constructor initializes the TriggerEventDelegate instance with the provided std::function.
			/// It calls the assignment operator to set the delegate function using the provided std::function.
			/// @param functionNew The function to construct as the current event.
			inline TriggerEventDelegate(std::function<ReturnType(ArgTypes...)> functionNew) {
				// Call the assignment operator to set the delegate function using the provided std::function.
				*this = functionNew;
			}

			/// @brief Assignment operator to set the delegate function using a function pointer.
			/// This operator assigns a new function pointer, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew The function pointer to assign as the current event.
			/// @returns Reference to the current TriggerEventDelegate instance after the assignment.
			inline TriggerEventDelegate& operator=(ReturnType (*functionNew)(ArgTypes...)) {
				function = functionNew;
				return *this;
			}


			/// @brief Constructor for TriggerEventDelegate class, taking a function pointer as argument.
			/// @param functionNew The function to construct as the current event.
			inline TriggerEventDelegate(ReturnType (*functionNew)(ArgTypes...)) {
				*this = functionNew;
			}

			/// @brief Default constructor for TriggerEventDelegate class.
			inline TriggerEventDelegate() = default;

			/// @brief Sets the test function to determine conditional execution.
			/// @param testFunctionNew The test function to set.
			inline void setTestFunction(std::function<bool(ArgTypes...)> testFunctionNew) {
				testFunction = testFunctionNew;
			}

			/// @brief Sets the test function using a function pointer.
			/// @param testFunctionNew The test function to set.
			inline void setTestFunction(bool (*testFunctionNew)(ArgTypes...)) {
				testFunction = testFunctionNew;
			}

		  protected:
			std::function<ReturnType(ArgTypes...)> function{};
			std::function<bool(ArgTypes...)> testFunction{};
		};

		/// @brief A trigger event that fires based on the result of trigger-function return value.
		template<typename ReturnType, typename... ArgTypes> class TriggerEvent {
		  public:
			UnorderedMap<EventDelegateToken, TriggerEventDelegate<ReturnType, ArgTypes...>> functions{};

			inline TriggerEvent& operator=(const TriggerEvent& other) noexcept = delete;
			inline TriggerEvent(const TriggerEvent& other) noexcept			   = delete;

			/// @brief Move assignment operator for the TriggerEvent class.
			/// This operator moves the contents of another TriggerEvent instance, 'other', into the current instance.
			/// It transfers the functions and eventId from 'other' to the current instance, clearing 'other' in the process.
			/// @param other The TriggerEvent instance to be moved from.
			/// @returns Reference to the current TriggerEvent instance after the move assignment.
			inline TriggerEvent& operator=(TriggerEvent&& other) noexcept {
				if (this != &other) {
					std::swap(functions, other.functions);
					std::swap(eventId, other.eventId);
				}
				return *this;
			}

			/// @brief Move constructor for the TriggerEvent class.
			/// This constructor moves the contents of another TriggerEvent instance, 'other', into the current instance.
			/// It performs a move assignment using the 'operator=' function.
			/// @param other The TriggerEvent instance to be moved from.
			inline TriggerEvent(TriggerEvent&& other) noexcept {
				// Perform move assignment using the 'operator=' function.
				*this = std::move(other);
			}

			/// @brief Default constructor for TriggerEvent class.
			inline TriggerEvent() {
				std::unique_lock lock{ accessMutex };
				eventId = std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count();
			}

			/// @brief Add an event delegate to the event.
			/// @param eventDelegate The event delegate to add.
			/// @return EventDelegateToken The token representing the added event delegate.
			inline EventDelegateToken add(TriggerEventDelegate<ReturnType, ArgTypes...>&& eventDelegate) {
				std::unique_lock lock{ accessMutex };
				EventDelegateToken eventToken{};
				eventToken.handlerId  = std::chrono::duration_cast<Microseconds>(HRClock::now().time_since_epoch()).count();
				eventToken.eventId	  = eventId;
				functions[eventToken] = std::move(eventDelegate);
				return eventToken;
			}

			/// @brief Remove an event delegate from the event.
			/// @param eventToken The token representing the event delegate to remove.
			inline void erase(const EventDelegateToken& eventToken) {
				std::unique_lock lock{ accessMutex };
				if (eventToken.eventId == eventId) {
					if (functions.contains(eventToken)) {
						functions.erase(eventToken);
					}
				}
			}

			/// @brief Invoke the trigger event with provided arguments.
			/// @param args The arguments to pass to the trigger event delegates.
			template<typename... ArgTypesNew> inline void operator()(ArgTypesNew&&... args) {
				std::unique_lock lock{ accessMutex };
				for (auto iterator = functions.begin(); iterator != functions.end(); ++iterator) {
					if (iterator.operator*().second.testFunction(args...)) {
						iterator.operator*().second.function(args...);
						iterator = functions.erase(iterator->first);
					}
				}
				return;
			}

		  protected:
			std::mutex accessMutex{};
			uint64_t eventId{};
		};

		/**@}*/

	}
}
