/*
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
	struct DiscordCoreAPI_Dll CoRoutineError : public std::runtime_error {
		CoRoutineError(const std::string& theMessage);
	};

	class DiscordCoreAPI_Dll CoRoutineBase {
	  public:
		static DiscordCoreInternal::CoRoutineThreadPool threadPool;

		virtual ~CoRoutineBase() = default;
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam ReturnType The type of parameter that is returned by the CoRoutine.
	template<typename ReturnType> class CoRoutine : public CoRoutineBase {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType02> friend class CoRoutine;

			void requestStop() {
				this->areWeStoppedBool.store(true);
			}

			bool areWeStopped() {
				return this->areWeStoppedBool.load();
			}

			void return_value(ReturnType&& at) {
				this->result = std::move(at);
			}

			void return_value(ReturnType& at) {
				this->result = at;
			}

			CoRoutine<ReturnType> get_return_object() {
				return CoRoutine<ReturnType>{ std::coroutine_handle<CoRoutine<ReturnType>::promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				if (this->areWeDone) {
					this->areWeDone->store(true);
				}
				return {};
			}

			void unhandled_exception() {
				if (this->exceptionBuffer) {
					this->exceptionBuffer->send(std::current_exception());
				}
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{ nullptr };
			std::atomic_bool areWeStoppedBool{ false };
			std::atomic_bool* areWeDone{ nullptr };
			ReturnType result{};
		};

		CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				this->coroutineHandle.promise().exceptionBuffer = this->exceptionBuffer.get();
				this->coroutineHandle.promise().areWeDone = this->areWeDone.get();
				this->currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
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

		CoRoutine<ReturnType>& operator=(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) {
			this->coroutineHandle = coroutineHandleNew;
			this->coroutineHandle.promise().exceptionBuffer = this->exceptionBuffer.get();
			this->coroutineHandle.promise().areWeDone = this->areWeDone.get();
			return *this;
		}

		explicit CoRoutine(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		~CoRoutine() {
			if (this && this->coroutineHandle) {
				this->coroutineHandle.promise().exceptionBuffer = nullptr;
				this->coroutineHandle.promise().areWeDone = nullptr;
				if (this->coroutineHandle.done()) {
					this->coroutineHandle.destroy();
				}
				this->exceptionBuffer.reset(nullptr);
				this->areWeDone.reset(nullptr);
			}
		}

		/// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
		/// \returns DiscordCoreAPI::CoRoutineStatus The status of the CoRoutine.
		CoRoutineStatus getStatus() {
			if (!this->coroutineHandle) {
				this->currentStatus.store(CoRoutineStatus::Cancelled);
			} else if (this->coroutineHandle && !this->coroutineHandle.done()) {
				this->currentStatus.store(CoRoutineStatus::Running);
			} else if (this->coroutineHandle && this->coroutineHandle.done()) {
				this->currentStatus.store(CoRoutineStatus::Complete);
			}
			return this->currentStatus.load();
		}

		/// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
		/// \returns ReturnType The return value of the CoRoutine.
		ReturnType get() {
			if (this && this->coroutineHandle) {
				while (!this->areWeDone->load()) {
					std::this_thread::sleep_for(1ms);
				}
				this->currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
				this->result = std::move(this->coroutineHandle.promise().result);
				return this->result;
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
			return ReturnType{};
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		/// \returns ReturnType The return value of the CoRoutine.
		ReturnType cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					while (!this->areWeDone->load()) {
						std::this_thread::sleep_for(1ms);
					}
				}
				std::exception_ptr exceptionPtr{};
				this->currentStatus.store(CoRoutineStatus::Cancelled);
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
				this->result = std::move(this->coroutineHandle.promise().result);
				return this->result;
			}
			return ReturnType{};
		}

	  protected:
		std::unique_ptr<UnboundedMessageBlock<std::exception_ptr>> exceptionBuffer{ std::make_unique<UnboundedMessageBlock<std::exception_ptr>>() };
		std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandle{ nullptr };
		std::unique_ptr<std::atomic_bool> areWeDone{ std::make_unique<std::atomic_bool>() };
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		ReturnType result{};
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam void The type of parameter that is returned by the CoRoutine.
	template<> class CoRoutine<void> : public CoRoutineBase {
	  public:
		class promise_type {
		  public:
			template<typename ReturnType> friend class CoRoutine;

			void requestStop() {
				this->areWeStoppedBool.store(true);
			}

			bool areWeStopped() {
				return this->areWeStoppedBool.load();
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
				if (this->areWeDone) {
					this->areWeDone->store(true);
				}
				return {};
			}

			void unhandled_exception() {
				if (this->exceptionBuffer) {
					this->exceptionBuffer->send(std::current_exception());
				}
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{ nullptr };
			std::atomic_bool areWeStoppedBool{ false };
			std::atomic_bool* areWeDone{ nullptr };
		};

		CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				this->coroutineHandle.promise().exceptionBuffer = this->exceptionBuffer.get();
				this->coroutineHandle.promise().areWeDone = this->areWeDone.get();
				this->currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
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

		CoRoutine<void>& operator=(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) {
			this->coroutineHandle = coroutineHandleNew;
			this->coroutineHandle.promise().exceptionBuffer = this->exceptionBuffer.get();
			this->coroutineHandle.promise().areWeDone = this->areWeDone.get();
			return *this;
		}

		explicit CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		~CoRoutine() {
			if (this && this->coroutineHandle) {
				this->coroutineHandle.promise().exceptionBuffer = nullptr;
				this->coroutineHandle.promise().areWeDone = nullptr;
				if (this->coroutineHandle.done()) {
					this->coroutineHandle.destroy();
				}
				this->exceptionBuffer.reset(nullptr);
				this->areWeDone.reset(nullptr);
			}
		}

		/// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
		/// \returns CoRoutineStatus The status of the CoRoutine.
		CoRoutineStatus getStatus() {
			if (!this->coroutineHandle) {
				this->currentStatus.store(CoRoutineStatus::Cancelled);
			} else if (this->coroutineHandle && !this->coroutineHandle.done()) {
				this->currentStatus.store(CoRoutineStatus::Running);
			} else if (this->coroutineHandle && this->coroutineHandle.done()) {
				this->currentStatus.store(CoRoutineStatus::Complete);
			}
			return this->currentStatus.load();
		}

		/// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
		void get() {
			if (this && this->coroutineHandle) {
				while (!this->areWeDone->load()) {
					std::this_thread::sleep_for(1ms);
				}
				this->currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		void cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					while (!this->areWeDone->load()) {
						std::this_thread::sleep_for(1ms);
					}
				}
				this->currentStatus.store(CoRoutineStatus::Cancelled);
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer->tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
			}
		}

	  protected:
		std::unique_ptr<UnboundedMessageBlock<std::exception_ptr>> exceptionBuffer{ std::make_unique<UnboundedMessageBlock<std::exception_ptr>>() };
		std::unique_ptr<std::atomic_bool> areWeDone{ std::make_unique<std::atomic_bool>() };
		std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandle{ nullptr };
		std::atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
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