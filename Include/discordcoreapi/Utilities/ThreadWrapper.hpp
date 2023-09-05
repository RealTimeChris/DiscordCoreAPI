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
/// ThreadWrapper.hpp - Header file for the ThreadWrapper class.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file ThreadWrapper.hpp

#pragma once

#include <discordcoreapi/Utilities/CountedPtr.hpp>
#include <thread>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

		/// @brief A token used to control thread stopping.
		class StopToken {
		  public:
			/// @brief Constructor that takes a CountedPtr to a std::atomic_bool.
			/// @param theBool The CountedPtr to a std::atomic_bool.
			inline StopToken(const CountedPtr<std::atomic_bool>& theBool) {
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
			CountedPtr<std::atomic_bool> atomicBoolPtr{};///< Pointer to the managed std::atomic_bool.
		};

		/// @brief A wrapper class for managing threads with stopping capability.
		class ThreadWrapper {
		  public:
			/// @brief Default constructor.
			inline ThreadWrapper() noexcept = default;

			/// @brief Move assignment operator.
			/// @param other The other ThreadWrapper to copy from.
			/// @return The new ThreadWrapper instance.
			inline ThreadWrapper& operator=(ThreadWrapper&& other) noexcept {
				currentThread.swap(other.currentThread);
				atomicBool.swap(other.atomicBool);
				return *this;
			}

			/// @brief Move constructor.
			/// @param other The other ThreadWrapper to copy from.
			inline ThreadWrapper(ThreadWrapper&& other) noexcept {
				*this = std::move(other);
			}

			/// @brief Constructor that takes a callable object.
			/// @tparam Function The type of the callable object.
			/// @param function The callable object to be executed by the thread.
			template<typename Function> inline ThreadWrapper(Function&& function) {
				currentThread = std::thread(
					[&, function = std::forward<Function>(function)](StopToken stopToken) {
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
			/// @return The number of available hardware threads.
			static inline uint64_t hardware_concurrency() {
				return std::thread::hardware_concurrency();
			}

			/// @brief Get the StopToken associated with this thread wrapper.
			/// @return The StopToken for stopping control.
			inline StopToken stopToken() {
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
			inline ~ThreadWrapper() {
				requestStop();
				if (currentThread.joinable()) {
					currentThread.join();
				}
			}

		  protected:
			CountedPtr<std::atomic_bool> atomicBool{ makeCounted<std::atomic_bool>() };///< Managed atomic bool for stopping control.
			std::thread currentThread{};///< Managed thread.
		};
		/**@}*/
	}
}
