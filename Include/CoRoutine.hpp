/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// CoRoutine.hpp - Header for the coroutine class DiscordCoreAPI_Dll.
/// Oct 23, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities.hpp>
#include <coroutine>

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

     /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
     /// \param ReturnType The type of parameter that is returned by the CoRoutine.
    template<typename ReturnType>
    class CoRoutine {
    public:

        class DiscordCoreAPI_Dll promise_type {
        public:

            template<typename ReturnType02>
            friend auto NewThreadAwaitable();
            template<typename ReturnType02>
            friend class CoRoutine;

            void requestStop() {
                this->newThread.get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread.get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds{ timeToWaitForInMs });
            }

            void return_value(ReturnType at) {
                this->result = at;
            }

            auto get_return_object() {
                return CoRoutine<ReturnType>{ std::coroutine_handle<CoRoutine<ReturnType>::promise_type>::from_promise(*this) };
            }

            std::suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            std::suspend_always final_suspend() noexcept {
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(std::current_exception());
            }

        protected:

            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            std::condition_variable condVar{};
            std::jthread newThread{};
            std::mutex lockMutex{};
            ReturnType result;
        };

        CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<CoRoutine<ReturnType>::promise_type>();
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

        CoRoutine(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle) {
                this->coroutineHandle.promise().requestStop();
                if (this->coroutineHandle.done()) {
                    this->coroutineHandle.destroy();
                }
            }
        }

         /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        void detachThread() {
            if (this != nullptr && this->coroutineHandle) {
                this->coroutineHandle.promise().newThread.detach();
            }
        }

         /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
         /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr && this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

         /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
         /// \returns ReturnType The return value of the CoRoutine.
        ReturnType get() {
            if (this != nullptr) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.join();
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return this->coroutineHandle.promise().result;
            }
            return ReturnType{};
        }

         /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
         /// \returns ReturnType The object to be returned.
        ReturnType cancel() {
            if (this != nullptr && this->coroutineHandle) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                    this->coroutineHandle.promise().newThread.join();
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
     /// \param void The type of parameter that is returned by the CoRoutine.
    template<>
    class CoRoutine<void> {
    public:

        class DiscordCoreAPI_Dll promise_type {
        public:

            template<typename void02>
            friend auto NewThreadAwaitable();
            template<typename void02>
            friend class CoRoutine;

            void requestStop() {
                this->newThread.get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread.get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds{ timeToWaitForInMs });
            }

            void return_void() {
                return;
            }

            auto get_return_object() {
                return CoRoutine<void>{ std::coroutine_handle<CoRoutine<void>::promise_type>::from_promise(*this) };
            }

            std::suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            std::suspend_always final_suspend() noexcept {
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(std::current_exception());
            }

        protected:
            
            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            std::condition_variable condVar{};
            std::jthread newThread{};
            std::mutex lockMutex{};
        };

        CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<CoRoutine<void>::promise_type>();
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

        CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle) {
                this->coroutineHandle.promise().requestStop();
                if (this->coroutineHandle.done()) {
                    this->coroutineHandle.destroy();
                }
            }
        }

         /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        void detachThread() {
            if (this != nullptr && this->coroutineHandle) {
                this->coroutineHandle.promise().newThread.detach();
            }
        }

         /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
         /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr && this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

         /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        void get() {
            if (this != nullptr) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.join();
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return;
            }
            return;
        }

         /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        void cancel() {
            if (this != nullptr && this->coroutineHandle) {
                std::cout << "WERE HERE THIS IS IT!0101" << std::endl;
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                    //this->coroutineHandle.promise().newThread.join();
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return;
            }
            return;
        }

    protected:
        std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandle{ nullptr };
        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

     /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
     /// \param ReturnType The type of the value returned by the containing CoRoutine.
     /// \returns A std::coroutine_handle<CoRoutine<ReturnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename ReturnType>
    auto NewThreadAwaitable() {

        struct DiscordCoreAPI_Dll NewThreadAwaitableClass {

            std::coroutine_handle<class CoRoutine<ReturnType>::promise_type> coroHandle{ nullptr };

            bool await_ready() {
                return false;
            };

            void await_suspend(std::coroutine_handle<class CoRoutine<ReturnType>::promise_type> handle) {
                this->coroHandle = handle;
                this->coroHandle.promise().newThread = std::jthread([=, this] { this->coroHandle.resume(); });
            }

            auto await_resume() {
                return this->coroHandle;
            }
        };
        return NewThreadAwaitableClass{};
    }

    /**@}*/
    };
