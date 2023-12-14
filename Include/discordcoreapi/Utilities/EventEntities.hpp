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
/// EventEntities.hpp - Header for the "event-related stuff".
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file EventEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		/**
		 * \addtogroup utilities
		 * @{
		 */

		/// @brief Struct representing an event delegate token, associated with an event.
		struct event_delegate_token {
			/// @brief Equality operator for comparing event_delegate_token instances.
			/// @param rhs the right-hand side event_delegate_token to compare against.
			/// @return bool true if the event_delegate_token instances are equal, otherwise false.
			DCA_INLINE bool operator==(const event_delegate_token& rhs) const {
				return handlerId == rhs.handlerId && eventId == rhs.eventId;
			}

			/// @brief Less-than operator for comparing event_delegate_token instances.
			/// @param rhs the right-hand side event_delegate_token to compare against.
			/// @return bool true if this event_delegate_token is less than rhs, otherwise false.
			DCA_INLINE bool operator<(const event_delegate_token& rhs) const {
				return handlerId < rhs.handlerId;
			}

			/// @brief Default constructor for event_delegate_token.
			DCA_INLINE event_delegate_token() = default;

			uint64_t handlerId{};///< Identifier for the handler.
			uint64_t eventId{};///< Identifier for the event.
		};

	};

	template<event_delegate_token_t value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& data) {
			uint64_t values[2]{};
			values[0] = data.eventId;
			values[0] = data.handlerId;
			return internalHashFunction(values, std::size(values) * sizeof(uint64_t));
		};
	};

	namespace discord_core_internal {

		/// @brief Event-delegate, representing an event function to be executed.
		template<typename return_type, typename... arg_types> class event_delegate {
		  public:
			template<typename rty02, typename... arg_types02> friend class event;

			/// @brief Default constructor for event_delegate class.
			event_delegate() = default;

			DCA_INLINE event_delegate& operator=(const event_delegate& other) = delete;
			DCA_INLINE event_delegate(const event_delegate& other)			  = delete;

			/// @brief Move assignment operator for the event_delegate class.
			/// this operator moves the contents of another event_delegate instance, 'other', into the current instance.
			/// it swaps the 'function' member between 'other' and the current instance, and then clears 'other' function.
			/// @param other the event_delegate instance to be moved from.
			/// @return reference to the current event_delegate instance after the move assignment.
			DCA_INLINE event_delegate& operator=(event_delegate&& other) noexcept {
				if (this != &other) {
					function.swap(other.function);
				}
				return *this;
			}

			/// @brief Move constructor for the event_delegate class.
			/// this constructor moves the contents of another event_delegate instance, 'other', into the current instance.
			/// it performs a move assignment using the 'operator=' function.
			/// @param other the event_delegate instance to be moved from.
			DCA_INLINE event_delegate(event_delegate&& other) noexcept {
				// perform move assignment using the 'operator=' function.
				*this = std::move(other);
			}

			/// @brief Assignment operator to set the delegate function.
			/// this operator assigns a new std::function, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew the std::function to assign as the current event.
			/// @return reference to the current event_delegate instance after the assignment.
			DCA_INLINE event_delegate& operator=(std::function<return_type(arg_types...)> functionNew) {
				function = functionNew;
				return *this;
			}

			/// @brief Constructor, taking a std::function<return_type(arg_types...)> as an argument.
			/// @param functionNew the function to construct as the current event.
			DCA_INLINE event_delegate(std::function<return_type(arg_types...)> functionNew) {
				*this = functionNew;
			}

			/// @brief Assignment operator to set the delegate function using a function pointer.
			/// this operator assigns a new function pointer, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew the function pointer to assign as the current event.
			/// @return reference to the current event_delegate instance after the assignment.
			DCA_INLINE event_delegate& operator=(return_type (*functionNew)(arg_types...)) {
				function = functionNew;
				return *this;
			}

			/// @brief Constructor for the event_delegate class, taking a pointer to a function as an argument.
			/// this constructor initializes the event_delegate instance with the provided function pointer.
			/// it calls the assignment operator to set the delegate function using the provided function pointer.
			/// @param functionNew the function to construct as the current event.
			DCA_INLINE event_delegate(return_type (*functionNew)(arg_types...)) {
				// call the assignment operator to set the delegate function using the provided function pointer.
				*this = functionNew;
			}

		  protected:
			std::function<return_type(const arg_types&...)> function{};
		};

		/// @brief Template class representing an event that executes event functions.
		template<typename return_type, typename... arg_types> class event {
		  public:
			unordered_map<event_delegate_token, event_delegate<return_type, arg_types...>> functions{};

			DCA_INLINE event& operator=(const event& other) = delete;
			DCA_INLINE event(const event& other)			= delete;

			/// @brief Move assignment operator for the event class.
			/// this operator moves the contents of another event instance, 'other', into the current instance.
			/// it transfers the functions and eventId from 'other' to the current instance, clearing 'other' in the process.
			/// @param other the event instance to be moved from.
			/// @return reference to the current event instance after the move assignment.
			DCA_INLINE event& operator=(event&& other) noexcept {
				if (this != &other) {
					std::swap(functions, other.functions);
					std::swap(eventId, other.eventId);
				}
				return *this;
			}

			/// @brief Move constructor for the event class.
			/// this constructor moves the contents of another event instance, 'other', into the current instance.
			/// it performs a move assignment using the 'operator=' function.
			/// @param other the event instance to be moved from.
			DCA_INLINE event(event&& other) noexcept {
				// perform move assignment using the 'operator=' function.
				*this = std::move(other);
			}

			/// @brief Default constructor for event class.
			DCA_INLINE event() {
				std::unique_lock lock{ accessMutex };
				eventId = std::chrono::duration_cast<std::chrono::duration<uint64_t, std::micro>>(sys_clock::now().time_since_epoch()).count();
			}

			/// @brief Add an event delegate to the event.
			/// @param eventDelegate the event delegate to add.
			/// @return event_delegate_token the token representing the added event delegate.
			DCA_INLINE event_delegate_token add(event_delegate<return_type, arg_types...>&& eventDelegate) {
				std::unique_lock lock{ accessMutex };
				event_delegate_token eventToken{};
				eventToken.handlerId  = std::chrono::duration_cast<std::chrono::duration<uint64_t, std::micro>>(sys_clock::now().time_since_epoch()).count();
				eventToken.eventId	  = eventId;
				functions[eventToken] = std::move(eventDelegate);
				return eventToken;
			}

			/// @brief Remove an event delegate from the event.
			/// @param eventToken the token representing the event delegate to remove.
			DCA_INLINE void erase(const event_delegate_token& eventToken) {
				std::unique_lock lock{ accessMutex };
				if (eventToken.eventId == eventId) {
					if (functions.contains(eventToken)) {
						functions.erase(eventToken);
					}
				}
			}

			/// @brief Invoke the event with provided arguments.
			/// @param args the arguments to pass to the event delegates.
			DCA_INLINE void operator()(const arg_types&... args) {
				std::unique_lock lock{ accessMutex };
				for (auto& [key, value]: functions) {
					try {
						value.function(args...).get();
					} catch (const dca_exception& error) {
						message_printer::printError<print_message_type::general>(error.what());
					}
				}
				return;
			}

			~event() {
				{
					std::unique_lock lock{ accessMutex };
					lock.unlock();
				}
			}

		  protected:
			std::mutex accessMutex{};
			uint64_t eventId{};
		};

		/// @brief Event-delegate, for representing an event-function to be executed conditionally.
		template<typename return_type, typename... arg_types> class trigger_event_delegate {
		  public:
			template<typename rty02, typename... arg_types02> friend class trigger_event;

			DCA_INLINE trigger_event_delegate& operator=(const trigger_event_delegate& other) = delete;
			DCA_INLINE trigger_event_delegate(const trigger_event_delegate& other)			  = delete;

			/// @brief Move assignment operator for trigger_event_delegate class.
			/// this operator moves the contents of another trigger_event_delegate instance ('other') into the current instance.
			/// @param other the trigger_event_delegate instance to be moved.
			/// @return reference to the current trigger_event_delegate instance after the move assignment.
			DCA_INLINE trigger_event_delegate& operator=(trigger_event_delegate&& other) noexcept {
				if (this != &other) {
					testFunction.swap(other.testFunction);
					function.swap(other.function);
				}
				return *this;
			}

			/// @brief Move constructor for trigger_event_delegate class.
			/// this constructor moves the contents of another trigger_event_delegate instance ('other') into the current instance.
			/// @param other the trigger_event_delegate instance to be moved.
			DCA_INLINE trigger_event_delegate(trigger_event_delegate&& other) noexcept {
				*this = std::move(other);
			}

			/// @brief Assignment operator to set the delegate function using a std::function.
			/// this operator assigns a new std::function, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew the std::function to assign as the current event.
			/// @return reference to the current trigger_event_delegate instance after the assignment.
			DCA_INLINE trigger_event_delegate& operator=(std::function<return_type(arg_types...)> functionNew) {
				function = functionNew;
				return *this;
			}

			/// @brief Constructor for the trigger_event_delegate class, taking a std::function as an argument.
			/// this constructor initializes the trigger_event_delegate instance with the provided std::function.
			/// it calls the assignment operator to set the delegate function using the provided std::function.
			/// @param functionNew the function to construct as the current event.
			DCA_INLINE trigger_event_delegate(std::function<return_type(arg_types...)> functionNew) {
				// call the assignment operator to set the delegate function using the provided std::function.
				*this = functionNew;
			}

			/// @brief Assignment operator to set the delegate function using a function pointer.
			/// this operator assigns a new function pointer, 'functionNew', as the delegate function of the current instance.
			/// @param functionNew the function pointer to assign as the current event.
			/// @return reference to the current trigger_event_delegate instance after the assignment.
			DCA_INLINE trigger_event_delegate& operator=(return_type (*functionNew)(arg_types...)) {
				function = functionNew;
				return *this;
			}

			/// @brief Constructor for trigger_event_delegate class, taking a function pointer as argument.
			/// @param functionNew the function to construct as the current event.
			DCA_INLINE trigger_event_delegate(return_type (*functionNew)(arg_types...)) {
				*this = functionNew;
			}

			/// @brief Default constructor for trigger_event_delegate class.
			DCA_INLINE trigger_event_delegate() = default;

			/// @brief Sets the test function to determine conditional execution.
			/// @param testFunctionNew the test function to set.
			DCA_INLINE void setTestFunction(std::function<bool(arg_types...)> testFunctionNew) {
				testFunction = testFunctionNew;
			}

			/// @brief Sets the test function using a function pointer.
			/// @param testFunctionNew the test function to set.
			DCA_INLINE void setTestFunction(bool (*testFunctionNew)(arg_types...)) {
				testFunction = testFunctionNew;
			}

		  protected:
			std::function<return_type(const arg_types&...)> function{};
			std::function<bool(const arg_types&...)> testFunction{};
		};

		/// @brief A trigger event that fires based on the result of trigger-function return value.
		template<typename return_type, typename... arg_types> class trigger_event {
		  public:
			unordered_map<event_delegate_token, trigger_event_delegate<return_type, arg_types...>> functions{};

			DCA_INLINE trigger_event& operator=(const trigger_event& other) = delete;
			DCA_INLINE trigger_event(const trigger_event& other)			= delete;

			/// @brief Move assignment operator for the trigger_event class.
			/// this operator moves the contents of another trigger_event instance, 'other', into the current instance.
			/// it transfers the functions and eventId from 'other' to the current instance, clearing 'other' in the process.
			/// @param other the trigger_event instance to be moved from.
			/// @return reference to the current trigger_event instance after the move assignment.
			DCA_INLINE trigger_event& operator=(trigger_event&& other) noexcept {
				if (this != &other) {
					std::swap(functions, other.functions);
					std::swap(eventId, other.eventId);
				}
				return *this;
			}

			/// @brief Move constructor for the trigger_event class.
			/// this constructor moves the contents of another trigger_event instance, 'other', into the current instance.
			/// it performs a move assignment using the 'operator=' function.
			/// @param other the trigger_event instance to be moved from.
			DCA_INLINE trigger_event(trigger_event&& other) noexcept {
				// perform move assignment using the 'operator=' function.
				*this = std::move(other);
			}

			/// @brief Default constructor for trigger_event class.
			DCA_INLINE trigger_event() {
				std::unique_lock lock{ accessMutex };
				eventId = std::chrono::duration_cast<std::chrono::duration<uint64_t, std::micro>>(sys_clock::now().time_since_epoch()).count();
			}

			/// @brief Add an event delegate to the event.
			/// @param eventDelegate the event delegate to add.
			/// @return event_delegate_token the token representing the added event delegate.
			DCA_INLINE event_delegate_token add(trigger_event_delegate<return_type, arg_types...>&& eventDelegate) {
				std::unique_lock lock{ accessMutex };
				event_delegate_token eventToken{};
				eventToken.handlerId  = std::chrono::duration_cast<std::chrono::duration<uint64_t, std::micro>>(sys_clock::now().time_since_epoch()).count();
				eventToken.eventId	  = eventId;
				functions[eventToken] = std::move(eventDelegate);
				return eventToken;
			}

			/// @brief Remove an event delegate from the event.
			/// @param eventToken the token representing the event delegate to remove.
			DCA_INLINE void erase(const event_delegate_token& eventToken) {
				std::unique_lock lock{ accessMutex };
				if (eventToken.eventId == eventId) {
					if (functions.contains(eventToken)) {
						functions.erase(eventToken);
					}
				}
			}

			/// @brief Invoke the trigger event with provided arguments.
			/// @param args the arguments to pass to the trigger event delegates.
			DCA_INLINE void operator()(const arg_types&... args) {
				std::unique_lock lock{ accessMutex };
				for (auto iterator = functions.begin(); iterator != functions.end(); ++iterator) {
					try {
						if (iterator.operator*().second.testFunction(args...)) {
							iterator.operator*().second.function(args...);
							iterator = functions.erase(iterator->first);
						}
					} catch (const dca_exception& error) {
						message_printer::printError<print_message_type::general>(error.what());
					}
				}
				return;
			}

			~trigger_event() {
				{
					std::unique_lock lock{ accessMutex };
					lock.unlock();
				}
			}

		  protected:
			std::mutex accessMutex{};
			uint64_t eventId{};
		};

		/**@}*/

	}
}
