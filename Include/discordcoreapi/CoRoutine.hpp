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

#ifndef COROUTINE
	#define COROUTINE

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
		CoRoutineError(const String& theMessage);
	};

	class DiscordCoreAPI_Dll CoRoutineBase {
	  public:
		static DiscordCoreInternal::CoRoutineThreadPool threadPool;

		virtual ~CoRoutineBase() noexcept = default;
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam ReturnType The type of parameter that is returned by the CoRoutine.
	template<typename ReturnType> class CoRoutine : public CoRoutineBase {
	  public:
		using VoidType = void;

		class DiscordCoreAPI_Dll promise_type {
		  public:
			template<typename ReturnType02> friend class CoRoutine;

			VoidType requestStop() {
				this->areWeStoppedBool.store(true);
			}

			Bool areWeStopped() {
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

			VoidType unhandled_exception() {
				if (this->exceptionBuffer) {
					this->exceptionBuffer->send(std::current_exception());
				}
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{ nullptr };
			AtomicBool areWeStoppedBool{ false };
			AtomicBool* areWeDone{ nullptr };
			ReturnType result{};
		};

		CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				this->coroutineHandle.promise().exceptionBuffer = &this->exceptionBuffer;
				this->coroutineHandle.promise().areWeDone = &this->areWeDone;
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
			this->coroutineHandle.promise().exceptionBuffer = &this->exceptionBuffer;
			this->coroutineHandle.promise().areWeDone = &this->areWeDone;
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
				while (!this->areWeDone.load()) {
					std::this_thread::sleep_for(1ms);
				}
				this->currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer.tryReceive(exceptionPtr)) {
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
					while (!this->areWeDone.load()) {
						std::this_thread::sleep_for(1ms);
					}
				}
				std::exception_ptr exceptionPtr{};
				this->currentStatus.store(CoRoutineStatus::Cancelled);
				while (this->exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
				this->result = std::move(this->coroutineHandle.promise().result);
				return this->result;
			}
			return ReturnType{};
		}

	  protected:
		std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandle{ nullptr };
		Atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
		AtomicBool areWeDone{};
		ReturnType result{};
	};

	/// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
	/// \tparam VoidType The type of parameter that is returned by the CoRoutine.
	template<> class CoRoutine<void> : public CoRoutineBase {
	  public:
		using VoidType = void;

		class DiscordCoreAPI_Dll promise_type {
		  public:
			template<typename ReturnType> friend class CoRoutine;

			VoidType requestStop() {
				this->areWeStoppedBool.store(true);
			}

			Bool areWeStopped() {
				return this->areWeStoppedBool.load();
			}

			void return_void() {
			}

			CoRoutine<VoidType> get_return_object() {
				return CoRoutine<VoidType>{ std::coroutine_handle<CoRoutine<VoidType>::promise_type>::from_promise(*this) };
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

			VoidType unhandled_exception() {
				if (this->exceptionBuffer) {
					this->exceptionBuffer->send(std::current_exception());
				}
			}

		  protected:
			UnboundedMessageBlock<std::exception_ptr>* exceptionBuffer{ nullptr };
			AtomicBool areWeStoppedBool{ false };
			AtomicBool* areWeDone{ nullptr };
		};

		CoRoutine<VoidType>& operator=(CoRoutine<VoidType>&& other) noexcept {
			if (this != &other) {
				this->coroutineHandle = other.coroutineHandle;
				other.coroutineHandle = nullptr;
				this->coroutineHandle.promise().exceptionBuffer = &this->exceptionBuffer;
				this->coroutineHandle.promise().areWeDone = &this->areWeDone;
				this->currentStatus.store(other.currentStatus.load());
				other.currentStatus.store(CoRoutineStatus::Cancelled);
			}
			return *this;
		};

		CoRoutine(CoRoutine<VoidType>&& other) noexcept {
			*this = std::move(other);
		};

		CoRoutine<VoidType>& operator=(const CoRoutine<VoidType>& other) = delete;

		CoRoutine(const CoRoutine<VoidType>& other) = delete;

		CoRoutine<VoidType>& operator=(CoRoutine<VoidType>& other) = delete;

		CoRoutine(CoRoutine<VoidType>& other) = delete;

		CoRoutine<VoidType>& operator=(std::coroutine_handle<CoRoutine<VoidType>::promise_type> coroutineHandleNew) {
			this->coroutineHandle = coroutineHandleNew;
			this->coroutineHandle.promise().exceptionBuffer = &this->exceptionBuffer;
			this->coroutineHandle.promise().areWeDone = &this->areWeDone;
			return *this;
		}

		explicit CoRoutine(std::coroutine_handle<CoRoutine<VoidType>::promise_type> coroutineHandleNew) {
			*this = coroutineHandleNew;
		};

		~CoRoutine() {
			if (this && this->coroutineHandle) {
				this->coroutineHandle.promise().exceptionBuffer = nullptr;
				this->coroutineHandle.promise().areWeDone = nullptr;
				if (this->coroutineHandle.done()) {
					this->coroutineHandle.destroy();
				}
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
		VoidType get() {
			if (this && this->coroutineHandle) {
				while (!this->areWeDone.load()) {
					std::this_thread::sleep_for(1ms);
				}
				this->currentStatus.store(CoRoutineStatus::Complete);
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
			} else {
				throw CoRoutineError("CoRoutine::get(), You called get() on a CoRoutine that is "
									 "not in a valid state.");
			}
		}

		/// Cancels the currently executing CoRoutine and returns the current result. \brief Cancels the currently executing CoRoutine and returns the current result.
		VoidType cancel() {
			if (this && this->coroutineHandle) {
				if (!this->coroutineHandle.done()) {
					this->coroutineHandle.promise().requestStop();
					while (!this->areWeDone.load()) {
						std::this_thread::sleep_for(1ms);
					}
				}
				this->currentStatus.store(CoRoutineStatus::Cancelled);
				std::exception_ptr exceptionPtr{};
				while (this->exceptionBuffer.tryReceive(exceptionPtr)) {
					std::rethrow_exception(exceptionPtr);
					std::this_thread::sleep_for(1ms);
				}
			}
		}

	  protected:
		std::coroutine_handle<CoRoutine<VoidType>::promise_type> coroutineHandle{ nullptr };
		Atomic<CoRoutineStatus> currentStatus{ CoRoutineStatus::Idle };
		UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
		AtomicBool areWeDone{};
	};

	/// An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution. \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType> class NewThreadAwaiter {
	  public:
		using VoidType = void;

		Bool await_ready() const noexcept {
			return false;
		}

		VoidType await_suspend(std::coroutine_handle<typename CoRoutine<ReturnType>::promise_type> coroHandleNew) noexcept {
			CoRoutine<ReturnType>::threadPool.submitTask(coroHandleNew);
			this->coroHandle = coroHandleNew;
		}

		auto await_resume() noexcept {
			return this->coroHandle;
		}

	  protected:
		std::coroutine_handle<typename CoRoutine<ReturnType>::promise_type> coroHandle{};
	};

	/**@}*/
};// namespace DiscordCoreAPI
#endif