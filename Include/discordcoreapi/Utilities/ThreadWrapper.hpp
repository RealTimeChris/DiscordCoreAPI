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
/// ThreadWrapper.hpp - Header file for the thread_wrapper class.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file ThreadWrapper.hpp
#pragma once

#include <discordcoreapi/Utilities/CountedPtr.hpp>
#include <thread>

namespace discord_core_api {

	class counted_atomic_bool : public reference_counter, public std::atomic_bool {};

	namespace discord_core_internal {

		/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

		/// @brief A token used to control thread stopping.
		class stop_token {
		  public:
			/// @brief Constructor that takes a counted_ptr to a std::atomic_bool.
			/// @param theBool the counted_ptr to a std::atomic_bool.
			inline stop_token(const counted_ptr<counted_atomic_bool>& theBool) {
				atomicBoolPtr = theBool;
			}

			/// @brief Check if stop has been requested.
			/// @return `true` if stop has been requested, `false` otherwise.
			inline bool stopRequested() {
				if (atomicBoolPtr) {
					return atomicBoolPtr->load(std::memory_order_acquire);
				} else {
					return false;
				}
			}

			/// @brief Request stop.
			inline void requestStop() {
				if (atomicBoolPtr) {
					atomicBoolPtr->store(true, std::memory_order_release);
				}
			}

		  protected:
			counted_ptr<counted_atomic_bool> atomicBoolPtr{};///< Pointer to the managed std::atomic_bool.
		};

		/// @brief A wrapper class for managing threads with stopping capability.
		class thread_wrapper {
		  public:
			/// @brief Default constructor.
			inline thread_wrapper() = default;

			/// @brief Move assignment operator.
			/// @param other the other thread_wrapper to copy from.
			/// @return the new thread_wrapper instance.
			inline thread_wrapper& operator=(thread_wrapper&& other) noexcept {
				currentThread.swap(other.currentThread);
				atomicBool.swap(other.atomicBool);
				return *this;
			}

			/// @brief Move constructor.
			/// @param other the other thread_wrapper to copy from.
			inline thread_wrapper(thread_wrapper&& other) noexcept {
				*this = std::move(other);
			}

			inline thread_wrapper& operator=(const thread_wrapper& other) = delete;
			inline thread_wrapper(const thread_wrapper& other)			  = delete;

			/// @brief Constructor that takes a callable object.
			/// @tparam function the type of the callable object.
			/// @param functionNew the callable object to be executed by the thread.
			template<typename function_type> inline thread_wrapper(function_type&& functionNew) {
				currentThread = std::thread(
					[&, function = std::forward<function_type>(functionNew)](stop_token stopToken) {
						function(stopToken);
					},
					atomicBool);
			}

			/// @brief Detach the managed thread if joinable.
			inline void detach() {
				if (currentThread.joinable()) {
					currentThread.detach();
				}
			}

			/// @brief Request the managed thread to stop.
			inline void requestStop() {
				atomicBool->store(true, std::memory_order_release);
			}

			/// @brief Get the hardware concurrency available.
			/// @return the number of available hardware threads.
			inline static uint64_t hardware_concurrency() {
				return std::thread::hardware_concurrency();
			}

			/// @brief Get the stop_token associated with this thread wrapper.
			/// @return the stop_token for stopping control.
			inline stop_token stopToken() {
				return atomicBool;
			}

			/// @brief Check if the managed thread is joinable.
			/// @return `true` if the thread is joinable, `false` otherwise.
			inline bool joinable() {
				return currentThread.joinable();
			}

			/// @brief Join the managed thread.
			inline void join() {
				if (currentThread.joinable()) {
					return currentThread.join();
				}
			}

			/// @brief Destructor that requests stop and joins the managed thread.
			inline ~thread_wrapper() {
				requestStop();
				if (currentThread.joinable()) {
					currentThread.join();
				}
			}

		  protected:
			counted_ptr<counted_atomic_bool> atomicBool{ makeCounted<counted_atomic_bool>() };///< Managed atomic bool for stopping control.
			std::thread currentThread{};///< Managed thread.
		};
		/**@}*/
	}
}