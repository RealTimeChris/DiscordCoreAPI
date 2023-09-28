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

namespace DiscordCoreAPI {

	/**
	* \addtogroup utilities
	* @{
	*/

	/// @brief The current status of the associated CoRoutine.
	enum class CoRoutineStatus {
		Idle	  = 0,///< Idle.
		Running	  = 1,///< Running.
		Complete  = 2,///< Complete.
		Cancelled = 3///< Cancelled.
	};

	/// @brief An error type for CoRoutines.
	struct CoRoutineError : public DCAException {
		inline CoRoutineError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	template<typename ValueType> class ResultHolder {
	  public:
		template<typename ValueTypeNew> inline void setResult(ValueTypeNew&& newResult) {
			result = makeUnique<ValueType>(std::forward<ValueTypeNew>(newResult));
		}

		inline void setStatus() {
			sentYet.store(true, std::memory_order_release);
		}

		inline ValueType getResult() {
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
		UniquePtr<ValueType> result{};
		std::atomic_bool sentYet{};
	};

	/// @brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam ReturnType The type of parameter that is returned by the CoRoutine.
	template<typename ReturnType, bool timeOut> class CoRoutine {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType02, bool timeOut02> friend class CoRoutine;

			inline void requestStop() {
				areWeStoppedBool.store(true, std::memory_order_release);
			}

			inline bool stopRequested() {
				return areWeStoppedBool.load(std::memory_order_acquire);
			}

			template<typename ReturnTypeNew> inline void return_value(ReturnTypeNew&& returnValue) {
				if (resultBuffer) {
					resultBuffer->setResult(std::forward<ReturnTypeNew>(returnValue));
				}
			}

			inline auto get_return_object() {
				return CoRoutine<ReturnType, timeOut>{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			inline std::suspend_never initial_suspend() {
				while (!resultBuffer) {
					std::this_thread::sleep_for(1ms);
				}
				return {};
			}

			inline std::suspend_always final_suspend() noexcept {
				if (resultBuffer) {
					resultBuffer->setStatus();
				}
				return {};
			}

			inline void unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->setResult(std::current_exception());
					exceptionBuffer->setStatus();
				}
			}

			inline ~promise_type() {
				exceptionBuffer = nullptr;
				resultBuffer	= nullptr;
			}

		  protected:
			ResultHolder<std::exception_ptr>* exceptionBuffer{};
			ResultHolder<ReturnType>* resultBuffer{};
			std::atomic_bool areWeStoppedBool{};
		};

		inline CoRoutine() = default;

		inline CoRoutine& operator=(CoRoutine<ReturnType, timeOut>&& other) noexcept {
			if (this != &other) {
				coroutineHandle							  = other.coroutineHandle;
				other.coroutineHandle					  = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().resultBuffer	  = &resultBuffer;
				currentStatus.store(other.currentStatus.load(std::memory_order_acquire), std::memory_order_release);
				other.currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
			}
			return *this;
		}

		inline CoRoutine(CoRoutine<ReturnType, timeOut>&& other) noexcept {
			*this = std::move(other);
		}

		inline CoRoutine& operator=(const CoRoutine<ReturnType, timeOut>& other) = delete;
		inline CoRoutine(const CoRoutine<ReturnType, timeOut>& other)			 = delete;

		inline CoRoutine& operator=(std::coroutine_handle<promise_type> coroutineHandleNew) {
			coroutineHandle							  = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().resultBuffer	  = &resultBuffer;
			return *this;
		}

		inline explicit CoRoutine(std::coroutine_handle<promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		inline ~CoRoutine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().resultBuffer	  = nullptr;
			}
		}

		/// @brief Collects the status of the CoRoutine.
		/// @return CoRoutineStatus The status of the CoRoutine.
		inline CoRoutineStatus getStatus() {
			if (!coroutineHandle) {
				currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
			} else if (coroutineHandle && !coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Running, std::memory_order_release);
			} else if (coroutineHandle && coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Complete, std::memory_order_release);
			}
			return currentStatus.load(std::memory_order_acquire);
		}

		/// @brief Gets the resulting value of the CoRoutine.
		/// @return The final value resulting from the CoRoutine's execution.
		inline ReturnType get() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					StopWatch<Milliseconds> stopWatch{ 15000 };
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
				currentStatus.store(CoRoutineStatus::Complete, std::memory_order_release);
				return resultBuffer.getResult();
			} else {
				throw CoRoutineError{ "CoRoutine::get(), You called get() on a CoRoutine that is "
									  "not in a valid state." };
			}
		}

		/// @brief Cancels the currently running CoRoutine, while blocking to wait for it to complete.
		/// @return The final value resulting from the CoRoutine's execution.
		inline ReturnType cancelAndWait() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					StopWatch<Milliseconds> stopWatch{ 15000 };
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
				currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
				return resultBuffer.getResult();
			} else {
				throw CoRoutineError{ "CoRoutine::cancelAndWait(), You called get() on a CoRoutine that is "
									  "not in a valid state." };
			}
		}

		/// @brief Cancels the currently executing CoRoutine and returns the current result.
		/// @return The final value resulting from the CoRoutine's execution.
		inline ReturnType cancel() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
				}
				checkForExceptions();
				currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
				return resultBuffer.getResult();
			} else {
				throw CoRoutineError{ "CoRoutine::cancel(), You called cancel() on a CoRoutine that is "
									  "not in a valid state." };
			}
		}

	  protected:
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		std::coroutine_handle<promise_type> coroutineHandle{};
		ResultHolder<std::exception_ptr> exceptionBuffer{};
		ResultHolder<ReturnType> resultBuffer{};

		inline void checkForExceptions() {
			if (exceptionBuffer.checkForResult()) {
				std::rethrow_exception(exceptionBuffer.getResult());
			}
		}
	};

	/// @brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam void The type of parameter that is returned by the CoRoutine.
	template<jsonifier_internal::void_t ReturnType, bool timeOut> class CoRoutine<ReturnType, timeOut> {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType02, bool timeOut02> friend class CoRoutine;

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
				return CoRoutine<ReturnType, timeOut>{ std::coroutine_handle<promise_type>::from_promise(*this) };
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
					exceptionBuffer->setStatus();
				}
			}

			inline ~promise_type() {
				exceptionBuffer = nullptr;
				resultBuffer	= nullptr;
			}

		  protected:
			ResultHolder<std::exception_ptr>* exceptionBuffer{};
			std::atomic_bool areWeStoppedBool{};
			std::atomic_bool* resultBuffer{};
		};

		inline CoRoutine() = default;

		inline CoRoutine& operator=(CoRoutine<ReturnType, timeOut>&& other) noexcept {
			if (this != &other) {
				coroutineHandle							  = other.coroutineHandle;
				other.coroutineHandle					  = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().resultBuffer	  = &resultBuffer;
				currentStatus.store(other.currentStatus.load(std::memory_order_acquire), std::memory_order_release);
				other.currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
			}
			return *this;
		}

		inline CoRoutine(CoRoutine<ReturnType, timeOut>&& other) noexcept {
			*this = std::move(other);
		}

		inline CoRoutine& operator=(const CoRoutine<ReturnType, timeOut>& other) = delete;
		inline CoRoutine(const CoRoutine<ReturnType, timeOut>& other)			 = delete;

		inline CoRoutine& operator=(std::coroutine_handle<promise_type> coroutineHandleNew) {
			coroutineHandle							  = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().resultBuffer	  = &resultBuffer;
			return *this;
		}

		inline explicit CoRoutine(std::coroutine_handle<promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		inline ~CoRoutine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().resultBuffer	  = nullptr;
			}
		}

		/// @brief Collects the status of the CoRoutine.
		/// @return CoRoutineStatus The status of the CoRoutine.
		inline CoRoutineStatus getStatus() {
			if (!coroutineHandle) {
				currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
			} else if (coroutineHandle && !coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Running, std::memory_order_release);
			} else if (coroutineHandle && coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Complete, std::memory_order_release);
			}
			return currentStatus.load(std::memory_order_acquire);
		}

		/// @brief Gets the resulting value of the CoRoutine.
		inline void get() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					StopWatch<Milliseconds> stopWatch{ 15000 };
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
				currentStatus.store(CoRoutineStatus::Complete, std::memory_order_release);
				return;
			} else {
				throw CoRoutineError{ "CoRoutine::get(), You called get() on a CoRoutine that is "
									  "not in a valid state." };
			}
		}

		/// @brief Cancels the currently running CoRoutine, while blocking to wait for it to complete.
		inline void cancelAndWait() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					StopWatch<Milliseconds> stopWatch{ 15000 };
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
				currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
				return;
			} else {
				throw CoRoutineError{ "CoRoutine::cancelAndWait(), You called get() on a CoRoutine that is "
									  "not in a valid state." };
			}
		}

		/// @brief Cancels the currently executing CoRoutine and returns the current result.
		inline void cancel() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
				}
				checkForExceptions();
				currentStatus.store(CoRoutineStatus::Cancelled, std::memory_order_release);
				return;
			} else {
				throw CoRoutineError{ "CoRoutine::cancel(), You called cancel() on a CoRoutine that is "
									  "not in a valid state." };
			}
		}

	  protected:
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		std::coroutine_handle<promise_type> coroutineHandle{};
		ResultHolder<std::exception_ptr> exceptionBuffer{};
		std::atomic_bool resultBuffer{};

		inline void checkForExceptions() {
			if (exceptionBuffer.checkForResult()) {
				std::rethrow_exception(exceptionBuffer.getResult());
			}
		}
	};

	class NewThreadAwaiterBase {
	  public:
		inline static DiscordCoreInternal::CoRoutineThreadPool threadPool{};
	};

	/// @brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType, bool timeOut> class NewThreadAwaiter : public NewThreadAwaiterBase {
	  public:
		inline bool await_ready() const {
			return false;
		}

		inline void await_suspend(std::coroutine_handle<typename CoRoutine<ReturnType, timeOut>::promise_type> coroHandleNew) {
			NewThreadAwaiterBase::threadPool.submitTask(coroHandleNew);
			coroHandle = coroHandleNew;
		}

		inline auto await_resume() {
			return coroHandle;
		}

	  protected:
		std::coroutine_handle<typename CoRoutine<ReturnType, timeOut>::promise_type> coroHandle{};
	};

	/**@}*/
};
