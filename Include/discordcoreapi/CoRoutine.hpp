/*
*
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
/// Chris M.
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
		explicit CoRoutineError(std::string theMessage) : std::runtime_error(theMessage){};
	};

	class CoRoutineBase {
	  public:
		inline static CoRoutineThreadPool threadPool{};
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam ReturnType The type of parameter that is returned by the CoRoutine.
	template<typename ReturnType> class CoRoutine : public CoRoutineBase {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType02> friend class CoRoutine;

			void requestStop() {
				this->areWeStopped.store(true, std::memory_order::seq_cst);
			}

			bool isItStopped() {
				return this->areWeStopped.load(std::memory_order::seq_cst);
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

			std::suspend_never initial_suspend() {
				this->currentStatus = CoRoutineStatus::Running;
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				this->currentStatus = CoRoutineStatus::Complete;
				this->theFlag.test_and_set();
				this->theFlag.notify_one();
				return {};
			}

			void unhandled_exception() {
				this->exceptionBuffer.send(std::current_exception());
			}

			~promise_type() {
				this->theFlag.wait(false, std::memory_order::seq_cst);
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
			CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
			std::atomic_bool areWeStopped{ false };
			std::atomic_flag theFlag{};
			ReturnType result;
		};

		CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
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

		CoRoutine() = default;

		CoRoutine(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew){};

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
			if (this && this->coroutineHandle) {
				this->currentStatus = this->coroutineHandle.promise().currentStatus;
			}
			return this->currentStatus;
		}

		/// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
		/// \returns ReturnType The return value of the CoRoutine.
		ReturnType get() {
			if (this && this->coroutineHandle) {
				this->coroutineHandle.promise().theFlag.wait(false, std::memory_order::seq_cst);
				std::exception_ptr exceptionPtr{};
				while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
				this->currentStatus = this->coroutineHandle.promise().currentStatus;
				return this->coroutineHandle.promise().result;
			} else {
				throw CoRoutineError("You called get() on a CoRoutine that is not in a valid state.");
			}
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		/// \returns ReturnType The return value of the CoRoutine.
		ReturnType cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					this->coroutineHandle.promise().theFlag.wait(false, std::memory_order::seq_cst);
				}
				std::exception_ptr exceptionPtr{};
				while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
				this->currentStatus = this->coroutineHandle.promise().currentStatus;
				return this->coroutineHandle.promise().result;
			}
			return ReturnType{};
		}

	  protected:
		std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandle{ nullptr };
		CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A CoRoutine - representing a potentially asynchronous operation/function (The void specialization).
	template<> class CoRoutine<void> : public CoRoutineBase {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType> friend class CoRoutine;

			void return_void() {
			}

			void requestStop() {
				this->areWeStopped.store(true, std::memory_order::seq_cst);
			}

			bool isItStopped() {
				return this->areWeStopped.load(std::memory_order::seq_cst);
			}

			auto get_return_object() {
				return CoRoutine<void>{ std::coroutine_handle<CoRoutine<void>::promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				this->currentStatus = CoRoutineStatus::Running;
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				this->currentStatus = CoRoutineStatus::Complete;
				this->theFlag.test_and_set();
				this->theFlag.notify_one();
				return {};
			}

			void unhandled_exception() {
				this->exceptionBuffer.send(std::current_exception());
			}

			~promise_type() {
				this->theFlag.wait(false, std::memory_order::seq_cst);
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
			CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
			std::atomic_bool areWeStopped{ false };
			std::atomic_flag theFlag{};
		};

		CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
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

		CoRoutine() = default;

		CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew){};

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
			if (this && this->coroutineHandle) {
				this->currentStatus = this->coroutineHandle.promise().currentStatus;
			}
			return this->currentStatus;
		}

		/// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
		void get() {
			if (this && this->coroutineHandle) {
				this->coroutineHandle.promise().theFlag.wait(false, std::memory_order::seq_cst);
				std::exception_ptr exceptionPtr{};
				while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
				this->currentStatus = this->coroutineHandle.promise().currentStatus;
				return;
			} else {
				throw CoRoutineError("You called get() on a CoRoutine that is not in a valid state.");
			}
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		void cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					this->coroutineHandle.promise().theFlag.wait(false, std::memory_order::seq_cst);
				}
				std::exception_ptr exceptionPtr{};
				while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
				}
				this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
				this->currentStatus = this->coroutineHandle.promise().currentStatus;
			}
		}

	  protected:
		std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandle{ nullptr };
		CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
	};

	/// An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution. \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType> struct NewThreadAwaiter {
		std::coroutine_handle<typename CoRoutine<ReturnType>::promise_type> coroHandle{};

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
	};

	/// An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution. \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType> auto NewThreadAwaitable() {
		return NewThreadAwaiter<ReturnType>{};
	}

	/**@}*/
};// namespace DiscordCoreAPI
