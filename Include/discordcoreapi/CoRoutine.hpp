/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// CoRoutine.hpp - Header for the coroutine class.
/// Oct 23, 2021
/// https://discordcoreapi.com
/// \file CoRoutine.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/ThreadPool.hpp>

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
	/// \tparam RTy The type of parameter that is returned by the CoRoutine.
	template<typename RTy> class CoRoutine {
	  public:
		class promise_type {
		  public:
			template<typename RTy02> friend class CoRoutine;

			void requestStop() {
				areWeStoppedBool.store(true);
			}

			bool areWeStopped() {
				return areWeStoppedBool.load();
			}

			void return_value(const RTy& at) {
				result = at;
			}

			void return_value(RTy&& at) {
				result = at;
			}

			CoRoutine<RTy> get_return_object() {
				return CoRoutine<RTy>{ std::coroutine_handle<CoRoutine<RTy>::promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				if (areWeDone) {
					areWeDone->store(true);
				}
				return {};
			}

			void unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->send(std::current_exception());
				}
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{};
			std::atomic_bool areWeStoppedBool{};
			std::atomic_bool* areWeDone{};
			RTy result{};
		};

		CoRoutine<RTy>& operator=(CoRoutine<RTy>&& other) noexcept {
			if (this != &other) {
				coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().areWeDone = &areWeDone;
				currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
			}
			return *this;
		};

		CoRoutine(CoRoutine<RTy>&& other) noexcept {
			*this = std::move(other);
		}

		CoRoutine<RTy>& operator=(const CoRoutine<RTy>& other) = delete;

		CoRoutine(const CoRoutine<RTy>& other) = delete;

		CoRoutine<RTy>& operator=(std::coroutine_handle<CoRoutine<RTy>::promise_type> coroutineHandleNew) {
			coroutineHandle = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().areWeDone = &areWeDone;
			return *this;
		}

		explicit CoRoutine(std::coroutine_handle<CoRoutine<RTy>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		~CoRoutine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().areWeDone = nullptr;
				if (coroutineHandle.done()) {
					coroutineHandle.destroy();
				}
			}
		}

		/// \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		CoRoutineStatus getStatus() {
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
		/// \returns RTy The return value of the CoRoutine.
		RTy get() {
			if (coroutineHandle) {
				while (!areWeDone.load()) {
					std::this_thread::sleep_for(1ms);
				}
				currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
				result = std::move(coroutineHandle.promise().result);
				return result;
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
			return RTy{};
		}

		/// \brief Cancels the currently executing CoRoutine and returns the current result.
		/// \returns RTy The return value of the CoRoutine.
		RTy cancel() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					while (!areWeDone.load()) {
						std::this_thread::sleep_for(1ms);
					}
				}
				std::exception_ptr exception{};
				currentStatus.store(CoRoutineStatus::Cancelled);
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
				result = std::move(coroutineHandle.promise().result);
				return result;
			}
			return RTy{};
		}

	  protected:
		std::coroutine_handle<CoRoutine<RTy>::promise_type> coroutineHandle{};
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
		std::atomic_bool areWeDone{};
		RTy result{};
	};

	/// \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam void The type of parameter that is returned by the CoRoutine.
	template<> class CoRoutine<void> {
	  public:
		class promise_type {
		  public:
			template<typename RTy> friend class CoRoutine;

			void requestStop() {
				areWeStoppedBool.store(true);
			}

			bool areWeStopped() {
				return areWeStoppedBool.load();
			}

			void return_void() {
			}

			CoRoutine<void> get_return_object() {
				return CoRoutine<void>{ std::coroutine_handle<CoRoutine<void>::promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				if (areWeDone) {
					areWeDone->store(true);
				}
				return {};
			}

			void unhandled_exception() {
				if (exceptionBuffer) {
					exceptionBuffer->send(std::current_exception());
				}
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{};
			std::atomic_bool areWeStoppedBool{};
			std::atomic_bool* areWeDone{};
		};

		CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
			if (this != &other) {
				coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
				coroutineHandle.promise().areWeDone = &areWeDone;
				currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
			}
			return *this;
		};

		CoRoutine(CoRoutine<void>&& other) noexcept {
			*this = std::move(other);
		}

		CoRoutine<void>& operator=(const CoRoutine<void>& other) = delete;

		CoRoutine(const CoRoutine<void>& other) = delete;

		CoRoutine<void>& operator=(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) {
			coroutineHandle = coroutineHandleNew;
			coroutineHandle.promise().exceptionBuffer = &exceptionBuffer;
			coroutineHandle.promise().areWeDone = &areWeDone;
			return *this;
		}

		explicit CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		~CoRoutine() {
			if (coroutineHandle) {
				coroutineHandle.promise().exceptionBuffer = nullptr;
				coroutineHandle.promise().areWeDone = nullptr;
				if (coroutineHandle.done()) {
					coroutineHandle.destroy();
				}
			}
		}

		/// \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		CoRoutineStatus getStatus() {
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
		void get() {
			if (coroutineHandle) {
				while (!areWeDone.load()) {
					std::this_thread::sleep_for(1ms);
				}
				currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

		/// \brief Cancels the currently executing CoRoutine and returns the current result.
		void cancel() {
			if (coroutineHandle) {
				if (!coroutineHandle.done()) {
					coroutineHandle.promise().requestStop();
					while (!areWeDone.load()) {
						std::this_thread::sleep_for(1ms);
					}
				}
				currentStatus.store(CoRoutineStatus::Cancelled);
				std::exception_ptr exception{};
				while (exceptionBuffer.tryReceive(exception)) {
					std::rethrow_exception(exception);
					std::this_thread::sleep_for(1ms);
				}
			}
		}

	  protected:
		std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandle{};
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
		std::atomic_bool areWeDone{};
	};

	class DiscordCoreAPI_Dll NewThreadAwaiterBase {
	  public:
		static DiscordCoreInternal::CoRoutineThreadPool threadPool;
	};

	/// \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam RTy The type of value returned by the containing CoRoutine.
	template<typename RTy> class NewThreadAwaiter : public NewThreadAwaiterBase {
	  public:
		bool await_ready() const noexcept {
			return false;
		}

		void await_suspend(std::coroutine_handle<typename CoRoutine<RTy>::promise_type> coroHandleNew) noexcept {
			NewThreadAwaiterBase::threadPool.submitTask(coroHandleNew);
			coroHandle = coroHandleNew;
		}

		auto await_resume() noexcept {
			return coroHandle;
		}

	  protected:
		std::coroutine_handle<typename CoRoutine<RTy>::promise_type> coroHandle{};
	};

	/**@}*/
};