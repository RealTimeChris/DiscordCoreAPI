/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// CoRoutine.hpp - Header for the coroutine class.
/// Oct 23, 2021
/// https://discordcoreapi.com
/// \file CoRoutine.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreAPI {
	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// The current status of the associated CoRoutine. \brief The current status of the associated CoRoutine.
	enum class CoRoutineStatus {
		Idle = 0,///< Idle.
		Running = 1,///< Running.
		Complete = 2,///< Complete.
		Cancelled = 3///< Cancelled.
	};

	/// An error type for CoRoutines. \brief An error type for CoRoutines.
	struct CoRoutineError : public std::runtime_error {
		CoRoutineError(const std::string& theMessage) : std::runtime_error(theMessage){};
	};

	class DiscordCoreAPI_Dll CoRoutineBase {
	  public:
		inline static DiscordCoreInternal::CoRoutineThreadPool threadPool{};
		virtual ~CoRoutineBase() = default;
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam ReturnType The type of parameter that is returned by the CoRoutine.
	template<typename ReturnType> class CoRoutine : public CoRoutineBase {
	  public:
		friend class promise_type;

		class promise_type {
		  public:
			template<typename ReturnType02> friend class CoRoutine;

			void requestStop() {
				this->areWeStopped.store(true);
			}

			bool isItStopped() {
				return this->areWeStopped.load();
			}

			void return_value(ReturnType&& at) {
				this->result = std::move(at);
			}

			void return_value(ReturnType& at) {
				this->result = at;
			}

			auto get_return_object() {
				return CoRoutine<ReturnType>{ std::coroutine_handle<CoRoutine<ReturnType>::promise_type>::from_promise(*this) };
			}

			auto initial_suspend() {
				return std::suspend_never{};
			}

			auto final_suspend() noexcept {
				this->theFlag->test_and_set();
				return std::suspend_always{};
			}

			void unhandled_exception() {
				this->exceptionBuffer->send(std::current_exception());
			}

			~promise_type() {
			}

		  protected:
			std::unique_ptr<UnboundedMessageBlock<std::exception_ptr>> exceptionBuffer{ std::make_unique<UnboundedMessageBlock<std::exception_ptr>>() };
			std::unique_ptr<std::atomic_flag> theFlag{ std::make_unique<std::atomic_flag>() };
			std::atomic_bool areWeStopped{ false };
			ReturnType result{};
		};

		CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				this->exceptionBuffer = this->coroutineHandle.promise().exceptionBuffer.get();
				this->theFlag = this->coroutineHandle.promise().theFlag.get();
				this->currentStatus = other.currentStatus;
				other.currentStatus = CoRoutineStatus::Cancelled;
			}
			return *this;
		};

		CoRoutine(CoRoutine<ReturnType>&& other) noexcept {
			*this = std::move(other);
		};

		CoRoutine<ReturnType>& operator=(const CoRoutine<ReturnType>& other) = delete;

		CoRoutine(const CoRoutine<ReturnType>& other) = delete;

		CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>& other) = delete;

		CoRoutine(CoRoutine<ReturnType>& other) = delete;

		CoRoutine(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {
			this->exceptionBuffer = this->coroutineHandle.promise().exceptionBuffer.get();
			this->theFlag = this->coroutineHandle.promise().theFlag.get();
		};

		~CoRoutine() {
			if (this && this->coroutineHandle) {
				if (this->coroutineHandle.done()) {
					this->coroutineHandle.destroy();
				}
			}
		}

		/// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		CoRoutineStatus getStatus() {
			if (!this->coroutineHandle) {
				this->currentStatus = CoRoutineStatus::Cancelled;
			} else if (this->coroutineHandle && !this->coroutineHandle.done()) {
				this->currentStatus = CoRoutineStatus::Running;
			} else if (this->coroutineHandle && this->coroutineHandle.done()) {
				this->currentStatus = CoRoutineStatus::Complete;
			}
			return this->currentStatus;
		}

		/// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
		/// \returns ReturnType The return value of the CoRoutine.
		ReturnType get() {
			if (this && this->coroutineHandle) {
				while (!this->theFlag->test()) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
				}
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->result = this->coroutineHandle.promise().result;
				return this->result;
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is not in a valid state.");
			}
			return ReturnType{};
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		/// \returns ReturnType The return value of the CoRoutine.
		ReturnType cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					while (!this->theFlag->test()) {
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
					}
				}
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->result = this->coroutineHandle.promise().result;
				return this->result;
			}
			return ReturnType{};
		}

	  protected:
		std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandle{ nullptr };
		UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{ nullptr };
		std::atomic_flag* theFlag{ nullptr };
		CoRoutineStatus currentStatus{};
		ReturnType result{};
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam void The type of parameter that is returned by the CoRoutine.
	template<> class CoRoutine<void> : public CoRoutineBase {
	  public:
		friend class promise_type;

		class promise_type {
		  public:
			template<typename void02> friend class CoRoutine;

			void requestStop() {
				this->areWeStopped.store(true);
			}

			bool isItStopped() {
				return this->areWeStopped.load();
			}

			void return_void() {
			}

			auto get_return_object() {
				return CoRoutine<void>{ std::coroutine_handle<CoRoutine<void>::promise_type>::from_promise(*this) };
			}

			auto initial_suspend() {
				return std::suspend_never{};
			}

			auto final_suspend() noexcept {
				this->theFlag->test_and_set();
				return std::suspend_always{};
			}

			void unhandled_exception() {
				this->exceptionBuffer->send(std::current_exception());
			}

			~promise_type() {
			}

		  protected:
			std::unique_ptr<UnboundedMessageBlock<std::exception_ptr>> exceptionBuffer{ std::make_unique<UnboundedMessageBlock<std::exception_ptr>>() };
			std::unique_ptr<std::atomic_flag> theFlag{ std::make_unique<std::atomic_flag>() };
			std::atomic_bool areWeStopped{ false };
		};

		CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				this->exceptionBuffer = this->coroutineHandle.promise().exceptionBuffer.get();
				this->theFlag = this->coroutineHandle.promise().theFlag.get();
				this->currentStatus = other.currentStatus;
				other.currentStatus = CoRoutineStatus::Cancelled;
			}
			return *this;
		};

		CoRoutine(CoRoutine<void>&& other) noexcept {
			*this = std::move(other);
		};

		CoRoutine<void>& operator=(const CoRoutine<void>& other) = delete;

		CoRoutine(const CoRoutine<void>& other) = delete;

		CoRoutine<void>& operator=(CoRoutine<void>& other) = delete;

		CoRoutine(CoRoutine<void>& other) = delete;

		CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {
			this->exceptionBuffer = this->coroutineHandle.promise().exceptionBuffer.get();
			this->theFlag = this->coroutineHandle.promise().theFlag.get();
		};

		~CoRoutine() {
			if (this && this->coroutineHandle) {
				if (this->coroutineHandle.done()) {
					this->coroutineHandle.destroy();
				}
			}
		}

		/// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		CoRoutineStatus getStatus() {
			if (!this->coroutineHandle) {
				this->currentStatus = CoRoutineStatus::Cancelled;
			} else if (this->coroutineHandle && !this->coroutineHandle.done()) {
				this->currentStatus = CoRoutineStatus::Running;
			} else if (this->coroutineHandle && this->coroutineHandle.done()) {
				this->currentStatus = CoRoutineStatus::Complete;
			}
			return this->currentStatus;
		}

		/// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
		void get() {
			if (this && this->coroutineHandle) {
				while (!this->theFlag->test()) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
				}
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is not in a valid state.");
			}
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		void cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					while (!this->theFlag->test()) {
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
					}
				}
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->currentStatus = CoRoutineStatus::Cancelled;
			}
		}

	  protected:
		std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandle{ nullptr };
		UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{ nullptr };
		std::atomic_flag* theFlag{ nullptr };
		CoRoutineStatus currentStatus{};
	};

	/// An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution. \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType> class NewThreadAwaiter {
	  public:
		bool await_ready() const noexcept {
			return false;
		}

		void await_suspend(std::coroutine_handle<typename CoRoutine<ReturnType>::promise_type> coroHandleNew) noexcept {
			CoRoutine<ReturnType>::threadPool.submitTask(coroHandleNew);
			this->coroHandle = coroHandleNew;
		}

		auto await_resume() noexcept {
			return this->coroHandle;
		}

	  protected:
		std::coroutine_handle<typename CoRoutine<ReturnType>::promise_type> coroHandle{};
	};

	/// An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution. \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType> auto NewThreadAwaitable() {
		return NewThreadAwaiter<ReturnType>{};
	}

	/**@}*/
};// namespace DiscordCoreAPI