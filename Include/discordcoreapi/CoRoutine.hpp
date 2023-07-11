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
#include <discordcoreapi/Utilities/ThreadPool.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	* \addtogroup utilities
	* @{
	*/

	/// \brief The current status of the associated CoRoutine.
	enum class CoRoutineStatus {
		Idle = 0,///< Idle.
		Running = 1,///< Running.
		Complete = 2,///< Complete.
		Cancelled = 3///< Cancelled.
	};

	/// \brief An error type for CoRoutines.
	struct DiscordCoreAPI_Dll CoRoutineError : public DCAException {
		explicit CoRoutineError(const std::string& message);
	};

	/// \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam ReturnType The type of parameter that is returned by the CoRoutine.
	template<typename ReturnType, bool timeOut> class CoRoutine {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType02, bool timeOut02> friend class CoRoutine;

			inline void requestStop() {
				areWeStoppedBool.store(true);
			}

			inline bool areWeStopped() {
				return areWeStoppedBool.load();
			}

			inline void return_value(ReturnType&& at) {
				result = std::move(at);
			}

			inline CoRoutine<ReturnType, timeOut> get_return_object() {
				return CoRoutine<ReturnType, timeOut>{ std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type>::from_promise(*this) };
			}

			inline std::suspend_never initial_suspend() {
				while (!resultBuffer) {
					std::this_thread::sleep_for(1ms);
				}
				return {};
			}

			inline std::suspend_always final_suspend() noexcept {
				if (resultBuffer) {
					resultBuffer->send(std::move(result));
				}
				return {};
			}

			inline void unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->send(std::current_exception());
				}
			}

			inline ~promise_type() noexcept {
				exceptionBuffer = nullptr;
				resultBuffer = nullptr;
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{};
			UnboundedMessageBlock<ReturnType>* resultBuffer{};
			std::atomic_bool areWeStoppedBool{};
			ReturnType result{};
		};

		inline CoRoutine& operator=(CoRoutine<ReturnType, timeOut>&& other) noexcept {
			if (this != &other) {
				coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().resultBuffer = &resultBuffer;
				currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
			}
			return *this;
		}

		inline CoRoutine& operator=(const CoRoutine<ReturnType, timeOut>& other) = delete;

		inline CoRoutine(const CoRoutine<ReturnType, timeOut>& other) = delete;

		inline CoRoutine& operator=(std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type> coroutineHandleNew) {
			coroutineHandle = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().resultBuffer = &resultBuffer;
			return *this;
		}

		inline explicit CoRoutine(std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		inline ~CoRoutine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().resultBuffer = nullptr;
			}
		}

		/// \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		inline CoRoutineStatus getStatus() {
			if (!coroutineHandle) {
				currentStatus.store(CoRoutineStatus::Cancelled);
			} else if (coroutineHandle && !coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Running);
			} else if (coroutineHandle && coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Complete);
			}
			return currentStatus.load();
		}

		/// \brief Gets the resulting value of the CoRoutine.
		inline ReturnType get() {
			if (coroutineHandle) {
				DiscordCoreAPI::Milliseconds startTime{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
					DiscordCoreAPI::HRClock::now().time_since_epoch()) };
				if (!coroutineHandle.done()) {
					while (!resultBuffer.tryReceive(result)) {
						DiscordCoreAPI::Milliseconds now{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
							DiscordCoreAPI::HRClock::now().time_since_epoch()) };
						if (timeOut && (now - startTime).count() >= 15000) {
							return std::move(result);
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
				return std::move(result);
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

		/// \brief Cancels the currently executing CoRoutine and returns the current result.
		inline ReturnType cancel() {
			if (coroutineHandle) {
				DiscordCoreAPI::Milliseconds startTime{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
					DiscordCoreAPI::HRClock::now().time_since_epoch()) };
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					while (!resultBuffer.tryReceive(result)) {
						DiscordCoreAPI::Milliseconds now{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
							DiscordCoreAPI::HRClock::now().time_since_epoch()) };
						if (timeOut && (now - startTime).count() >= 15000) {
							return std::move(result);
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				currentStatus.store(CoRoutineStatus::Cancelled);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
				return std::move(result);
			} else {
				throw CoRoutineError("CoRoutine::cancel(), You called cancel() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

	  protected:
		std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type> coroutineHandle{};
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
		UnboundedMessageBlock<ReturnType> resultBuffer{};
		ReturnType result{};
	};

	/// \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam void The type of parameter that is returned by the CoRoutine.
	template<DiscordCoreInternal::VoidT ReturnType, bool timeOut> class CoRoutine<ReturnType, timeOut> {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType02, bool timeOut02> friend class CoRoutine;

			inline ReturnType requestStop() {
				areWeStoppedBool.store(true);
			}

			inline bool areWeStopped() {
				return areWeStoppedBool.load();
			}

			inline void return_void(){};

			inline CoRoutine<ReturnType, timeOut> get_return_object() {
				return CoRoutine<ReturnType, timeOut>{ std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type>::from_promise(*this) };
			}

			inline std::suspend_never initial_suspend() {
				while (!resultBuffer) {
					std::this_thread::sleep_for(1ms);
				}
				return {};
			}

			inline std::suspend_always final_suspend() noexcept {
				if (resultBuffer) {
					resultBuffer->send(true);
				}
				return {};
			}

			inline ReturnType unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->send(std::current_exception());
				}
			}

			inline ~promise_type() noexcept {
				exceptionBuffer = nullptr;
				resultBuffer = nullptr;
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{};
			UnboundedMessageBlock<bool>* resultBuffer{};
			std::atomic_bool areWeStoppedBool{};
		};

		inline CoRoutine& operator=(CoRoutine<ReturnType, timeOut>&& other) noexcept {
			if (this != &other) {
				coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().resultBuffer = &resultBuffer;
				currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
			}
			return *this;
		}

		inline CoRoutine& operator=(const CoRoutine<ReturnType, timeOut>& other) = delete;

		inline CoRoutine(const CoRoutine<ReturnType, timeOut>& other) = delete;

		inline CoRoutine& operator=(std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type> coroutineHandleNew) {
			coroutineHandle = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().resultBuffer = &resultBuffer;
			return *this;
		}

		inline explicit CoRoutine(std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		inline ~CoRoutine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().resultBuffer = nullptr;
			}
		}

		/// \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		inline CoRoutineStatus getStatus() {
			if (!coroutineHandle) {
				currentStatus.store(CoRoutineStatus::Cancelled);
			} else if (coroutineHandle && !coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Running);
			} else if (coroutineHandle && coroutineHandle.done()) {
				currentStatus.store(CoRoutineStatus::Complete);
			}
			return currentStatus.load();
		}

		/// \brief Gets the resulting value of the CoRoutine.
		inline void get() {
			if (coroutineHandle) {
				DiscordCoreAPI::Milliseconds startTime{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
					DiscordCoreAPI::HRClock::now().time_since_epoch()) };
				if (!coroutineHandle.done()) {
					bool result{};
					while (!resultBuffer.tryReceive(result)) {
						DiscordCoreAPI::Milliseconds now{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
							DiscordCoreAPI::HRClock::now().time_since_epoch()) };
						if (timeOut && (now - startTime).count() >= 15000) {
							return;
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
				return;
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

		/// \brief Cancels the currently executing CoRoutine and returns the current result.
		inline void cancel() {
			if (coroutineHandle) {
				DiscordCoreAPI::Milliseconds startTime{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
					DiscordCoreAPI::HRClock::now().time_since_epoch()) };
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					bool result{};
					while (!resultBuffer.tryReceive(result)) {
						DiscordCoreAPI::Milliseconds now{ std::chrono::duration_cast<DiscordCoreAPI::Milliseconds>(
							DiscordCoreAPI::HRClock::now().time_since_epoch()) };
						if (timeOut && (now - startTime).count() >= 15000) {
							return;
						}
						std::this_thread::sleep_for(1ms);
					}
				}
				currentStatus.store(CoRoutineStatus::Cancelled);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
				return;
			} else {
				throw CoRoutineError("CoRoutine::cancel(), You called cancel() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

	  protected:
		std::coroutine_handle<CoRoutine<ReturnType, timeOut>::promise_type> coroutineHandle{};
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
		UnboundedMessageBlock<bool> resultBuffer{};
	};

	class DiscordCoreAPI_Dll NewThreadAwaiterBase {
	  public:
		static DiscordCoreInternal::CoRoutineThreadPool threadPool;
	};

	/// \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType, bool timeOut> class NewThreadAwaiter : public NewThreadAwaiterBase {
	  public:
		bool await_ready() const noexcept {
			return false;
		}

		void await_suspend(std::coroutine_handle<typename CoRoutine<ReturnType, timeOut>::promise_type> coroHandleNew) noexcept {
			NewThreadAwaiterBase::threadPool.submitTask(coroHandleNew);
			coroHandle = coroHandleNew;
		}

		auto await_resume() noexcept {
			return coroHandle;
		}

	  protected:
		std::coroutine_handle<typename CoRoutine<ReturnType, timeOut>::promise_type> coroHandle{};
	};

	/**@}*/
};
