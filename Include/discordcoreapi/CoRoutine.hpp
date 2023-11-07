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
/// CoRoutine.hpp - Header for the coroutine class.
/// Oct 23, 2021
/// https://discordcoreapi.com
/// \file CoRoutine.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/CoRoutineThreadPool.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	/**
	* \addtogroup utilities
	* @{
	*/

	/// @brief The current status of the associated co_routine.
	enum class co_routine_status {
		idle	  = 0,///< Idle.
		running	  = 1,///< Running.
		complete  = 2,///< complete.
		cancelled = 3///< cancelled.
	};

	/// @brief An error type for co_routines.
	struct co_routine_error : public dca_exception {
		inline co_routine_error(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	template<typename value_type> class result_holder {
	  public:
		template<typename value_type_new> inline void setResult(value_type_new&& newResult) {
			result = makeUnique<value_type>(std::forward<value_type_new>(newResult));
			sentYet.store(true, std::memory_order_release);
		}

		inline value_type getResult() {
			if (sentYet.load(std::memory_order_acquire) && result) {
				sentYet.store(false, std::memory_order_release);
				return std::move(*result);
			} else {
				return {};
			}
		}

		inline bool checkForResult() {
			return sentYet.load(std::memory_order_acquire);
		}

	  protected:
		unique_ptr<value_type> result{};
		std::atomic_bool sentYet{};
	};

	/// @brief A co_routine - representing a potentially asynchronous operation/function.
	/// \tparam return_type the type of parameter that is returned by the co_routine.
	template<typename return_type_new, bool timeOut> class co_routine {
	  public:
		using return_type = return_type_new;///< The return type of this co_routine.

		class promise_type {
		  public:
			template<typename return_type02, bool timeOut02> friend class co_routine;

			inline void requestStop() {
				areWeStoppedBool.store(true, std::memory_order_release);
			}

			inline bool stopRequested() {
				return areWeStoppedBool.load(std::memory_order_acquire);
			}

			template<typename return_type_newer> inline void return_value(return_type_newer&& returnValue) {
				if (resultBuffer) {
					resultBuffer->setResult(std::forward<return_type_newer>(returnValue));
				}
			}

			inline auto get_return_object() {
				return co_routine<return_type, timeOut>{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			inline std::suspend_never initial_suspend() {
				while (!resultBuffer) {
					std::this_thread::sleep_for(1ms);
				}
				return {};
			}

			inline std::suspend_always final_suspend() noexcept {
				return {};
			}

			inline void unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->setResult(std::current_exception());
				}
			}

			inline ~promise_type() {
				exceptionBuffer = nullptr;
				resultBuffer	= nullptr;
			}

		  protected:
			result_holder<std::exception_ptr>* exceptionBuffer{};
			result_holder<return_type>* resultBuffer{};
			std::atomic_bool areWeStoppedBool{};
		};

		inline co_routine() = default;

		inline co_routine& operator=(co_routine<return_type, timeOut>&& other) noexcept {
			if (this != &other) {
				coroutineHandle							  = other.coroutineHandle;
				other.coroutineHandle					  = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().resultBuffer	  = &resultBuffer;
				currentStatus.store(other.currentStatus.load(std::memory_order_acquire), std::memory_order_release);
				other.currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
			}
			return *this;
		}

		inline co_routine(co_routine<return_type, timeOut>&& other) noexcept {
			*this = std::move(other);
		}

		inline co_routine& operator=(const co_routine<return_type, timeOut>& other) = delete;
		inline co_routine(const co_routine<return_type, timeOut>& other)			= delete;

		inline co_routine& operator=(std::coroutine_handle<promise_type> coroutineHandleNew) {
			coroutineHandle							  = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().resultBuffer	  = &resultBuffer;
			return *this;
		}

		inline explicit co_routine(std::coroutine_handle<promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		inline ~co_routine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().resultBuffer	  = nullptr;
			}
		}

		/// @brief Collects the status of the co_routine.
		/// @return co_routine_status the status of the co_routine.
		inline co_routine_status getStatus() {
			if (!coroutineHandle) {
				currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
			} else if (coroutineHandle && !coroutineHandle.done()) {
				currentStatus.store(co_routine_status::running, std::memory_order_release);
			} else if (coroutineHandle && coroutineHandle.done()) {
				currentStatus.store(co_routine_status::complete, std::memory_order_release);
			}
			return currentStatus.load(std::memory_order_acquire);
		}

		/// @brief Gets the resulting value of the co_routine.
		/// @return the final value resulting from the co_routine's execution.
		inline return_type get() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					stop_watch<milliseconds> stopWatch{ 15000 };
					stopWatch.reset();
					while (!resultBuffer.checkForResult()) {
						checkForExceptions();
						if constexpr (timeOut) {
							if (stopWatch.hasTimeElapsed()) {
								return resultBuffer.getResult();
							}
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				checkForExceptions();
				currentStatus.store(co_routine_status::complete, std::memory_order_release);
				return resultBuffer.getResult();
			} else {
				throw co_routine_error{ "co_routine::get(), you called get() on a co_routine that is "
										"not in a valid state." };
			}
		}

		/// @brief Cancels the currently running co_routine, while blocking to wait for it to complete.
		/// @return the final value resulting from the co_routine's execution.
		inline return_type cancelAndWait() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					stop_watch<milliseconds> stopWatch{ 15000 };
					stopWatch.reset();
					while (!resultBuffer.checkForResult()) {
						checkForExceptions();
						if constexpr (timeOut) {
							if (stopWatch.hasTimeElapsed()) {
								return resultBuffer.getResult();
							}
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				checkForExceptions();
				currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
				return resultBuffer.getResult();
			} else {
				throw co_routine_error{ "co_routine::cancelAndWait(), you called get() on a co_routine that is "
										"not in a valid state." };
			}
		}

		/// @brief Cancels the currently executing co_routine and returns the current result.
		/// @return the final value resulting from the co_routine's execution.
		inline return_type cancel() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
				}
				checkForExceptions();
				currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
				return resultBuffer.getResult();
			} else {
				throw co_routine_error{ "co_routine::cancel(), you called cancel() on a co_routine that is "
										"not in a valid state." };
			}
		}

	  protected:
		std::atomic<co_routine_status> currentStatus{ co_routine_status::idle };
		std::coroutine_handle<promise_type> coroutineHandle{};
		result_holder<std::exception_ptr> exceptionBuffer{};
		result_holder<return_type> resultBuffer{};

		inline void checkForExceptions() {
			if (exceptionBuffer.checkForResult()) {
				std::rethrow_exception(exceptionBuffer.getResult());
			}
		}
	};

	/// @brief A co_routine - representing a potentially asynchronous operation/function.
	/// \tparam void the type of parameter that is returned by the co_routine.
	template<jsonifier::concepts::void_t return_type_new, bool timeOut> class co_routine<return_type_new, timeOut> {
	  public:
		using return_type = return_type_new;///< The return type of this co_routine.

		class promise_type {
		  public:
			template<typename return_type02, bool timeOut02> friend class co_routine;

			inline void requestStop() {
				areWeStoppedBool.store(true, std::memory_order_release);
			}

			inline bool stopRequested() {
				return areWeStoppedBool.load(std::memory_order_acquire);
			}

			inline void return_void() {
				return;
			};

			inline auto get_return_object() {
				return co_routine<return_type, timeOut>{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			inline std::suspend_never initial_suspend() {
				while (!resultBuffer) {
					std::this_thread::sleep_for(1ms);
				}
				return {};
			}

			inline std::suspend_always final_suspend() noexcept {
				if (resultBuffer) {
					resultBuffer->store(true);
				}
				return {};
			}

			inline void unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->setResult(std::current_exception());
				}
			}

			inline ~promise_type() {
				exceptionBuffer = nullptr;
				resultBuffer	= nullptr;
			}

		  protected:
			result_holder<std::exception_ptr>* exceptionBuffer{};
			std::atomic_bool areWeStoppedBool{};
			std::atomic_bool* resultBuffer{};
		};

		inline co_routine() = default;

		inline co_routine& operator=(co_routine<return_type, timeOut>&& other) noexcept {
			if (this != &other) {
				coroutineHandle							  = other.coroutineHandle;
				other.coroutineHandle					  = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().resultBuffer	  = &resultBuffer;
				currentStatus.store(other.currentStatus.load(std::memory_order_acquire), std::memory_order_release);
				other.currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
			}
			return *this;
		}

		inline co_routine(co_routine<return_type, timeOut>&& other) noexcept {
			*this = std::move(other);
		}

		inline co_routine& operator=(const co_routine<return_type, timeOut>& other) = delete;
		inline co_routine(const co_routine<return_type, timeOut>& other)			= delete;

		inline co_routine& operator=(std::coroutine_handle<promise_type> coroutineHandleNew) {
			coroutineHandle							  = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().resultBuffer	  = &resultBuffer;
			return *this;
		}

		inline explicit co_routine(std::coroutine_handle<promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		inline ~co_routine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().resultBuffer	  = nullptr;
			}
		}

		/// @brief Collects the status of the co_routine.
		/// @return co_routine_status the status of the co_routine.
		inline co_routine_status getStatus() {
			if (!coroutineHandle) {
				currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
			} else if (coroutineHandle && !coroutineHandle.done()) {
				currentStatus.store(co_routine_status::running, std::memory_order_release);
			} else if (coroutineHandle && coroutineHandle.done()) {
				currentStatus.store(co_routine_status::complete, std::memory_order_release);
			}
			return currentStatus.load(std::memory_order_acquire);
		}

		/// @brief Gets the resulting value of the co_routine.
		inline void get() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					stop_watch<milliseconds> stopWatch{ 15000 };
					stopWatch.reset();
					while (!resultBuffer.load()) {
						checkForExceptions();
						if constexpr (timeOut) {
							if (stopWatch.hasTimeElapsed()) {
								return;
							}
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				checkForExceptions();
				currentStatus.store(co_routine_status::complete, std::memory_order_release);
				return;
			} else {
				throw co_routine_error{ "co_routine::get(), you called get() on a co_routine that is "
										"not in a valid state." };
			}
		}

		/// @brief Cancels the currently running co_routine, while blocking to wait for it to complete.
		inline void cancelAndWait() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					stop_watch<milliseconds> stopWatch{ 15000 };
					stopWatch.reset();
					while (!resultBuffer.load()) {
						checkForExceptions();
						if constexpr (timeOut) {
							if (stopWatch.hasTimeElapsed()) {
								return;
							}
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				checkForExceptions();
				currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
				return;
			} else {
				throw co_routine_error{ "co_routine::cancelAndWait(), you called get() on a co_routine that is "
										"not in a valid state." };
			}
		}

		/// @brief Cancels the currently executing co_routine and returns the current result.
		inline void cancel() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
				}
				checkForExceptions();
				currentStatus.store(co_routine_status::cancelled, std::memory_order_release);
				return;
			} else {
				throw co_routine_error{ "co_routine::cancel(), you called cancel() on a co_routine that is "
										"not in a valid state." };
			}
		}

	  protected:
		std::atomic<co_routine_status> currentStatus{ co_routine_status::idle };
		std::coroutine_handle<promise_type> coroutineHandle{};
		result_holder<std::exception_ptr> exceptionBuffer{};
		std::atomic_bool resultBuffer{};

		inline void checkForExceptions() {
			if (exceptionBuffer.checkForResult()) {
				std::rethrow_exception(exceptionBuffer.getResult());
			}
		}
	};

	class new_thread_awaiter_base {
	  public:
		inline static discord_core_internal::co_routine_thread_pool threadPool{};
	};

	/// @brief An awaitable that can be used to launch the co_routine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam return_type the type of value returned by the containing co_routine.
	template<typename return_type, bool timeOut> class new_thread_awaiter : public new_thread_awaiter_base {
	  public:
		inline bool await_ready() const {
			return false;
		}

		inline void await_suspend(std::coroutine_handle<typename co_routine<return_type, timeOut>::promise_type> coroHandleNew) {
			new_thread_awaiter_base::threadPool.submitTask(coroHandleNew);
			coroHandle = coroHandleNew;
		}

		inline auto await_resume() {
			return coroHandle;
		}

	  protected:
		std::coroutine_handle<typename co_routine<return_type, timeOut>::promise_type> coroHandle{};
	};

	/**@}*/
};
