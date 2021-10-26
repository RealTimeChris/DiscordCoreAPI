// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COROUTINES_
#define _COROUTINES_

namespace DiscordCoreAPI {
    /**
    * \addtogroup utilities
    * @{ 
    */
    
    /// The current status of the CoRoutine. \brief The current status of the CoRoutine.
    enum class CoRoutineStatus {
        Idle = 0,
        Running = 1,
        Complete = 2,
        Cancelled = 3
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
    /// \param returnType The type of parameter that is returned by the CoRoutine.
    template<typename returnType>
    class CoRoutine {
    public:
        class promise_type;

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

        coroutine_handle<promise_type> coroutineHandle{};

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {}

        CoRoutine() {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
            }
            return this->currentStatus;
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns returnType The return value of the CoRoutine.
        returnType get() {
            if (coroutineHandle.promise().newThread != nullptr) {
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
            return coroutineHandle.promise().result;
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns returnType The return value of the CoRoutine.
        returnType cancel() {
            if (coroutineHandle.promise().newThread != nullptr) {
                coroutineHandle.promise().newThread->get_stop_source().request_stop();
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
            return coroutineHandle.promise().result;
        }

        class promise_type {
        public:
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

            jthread* newThread{ nullptr };

            returnType result{};
            

            promise_type() {}

            ~promise_type() {}

            void return_value(returnType returnValue) {
                this->result = returnValue;
            }

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            suspend_always final_suspend() noexcept {
                return{};
            }

            void unhandled_exception() {
                exit(1);
            }
        };
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
    template<>
    class CoRoutine<void> {
    public:
        class promise_type;

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

        coroutine_handle<promise_type> coroutineHandle{};

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {}

        CoRoutine() {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
            }
            return this->currentStatus;
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns void.
        void get() {
            if (coroutineHandle.promise().newThread != nullptr) {
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
            return;
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns void.
        void cancel() {
            if (coroutineHandle.promise().newThread != nullptr) {
                coroutineHandle.promise().newThread->get_stop_source().request_stop();
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
            return;
        }

        class promise_type {
        public:
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

            jthread* newThread{ nullptr };

            promise_type() {}

            ~promise_type() {}

            void return_void() {}

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            suspend_always final_suspend() noexcept {
                return{};
            }

            void unhandled_exception() {
                exit(1);
            }
        };
    };

    /// Used to acquire the CoRoutine handle from within the CoRoutine, as it is executing. \brief Used to acquire the CoRoutine handle from within the CoRoutine, as it is executing.
    /// \param returnType The type returned by the containing CoRoutined.
    /// \returns GetCoRoutineHandleAwaitable The awaitable type.
    template<typename returnType>
    auto GetCoRoutineHandleAwaitable() {
        class GetCoRoutineHandleAwaitable {
        public:
            coroutine_handle<CoRoutine<returnType>::promise_type> handleWaiter;

            GetCoRoutineHandleAwaitable() : handleWaiter(nullptr) {}

            bool await_ready() const noexcept {
                return false;
            }

            bool await_suspend(coroutine_handle<CoRoutine<returnType>::promise_type>handle) {
                this->handleWaiter = handle;
                return false;
            }

            auto await_resume() {
                return this->handleWaiter;
            }
        };
        return GetCoRoutineHandleAwaitable();
    }

    /// Used to set the CoRoutine into executing on a new thread, relative to the thread of the caller. \brief Used to set the CoRoutine into executing on a new thread, relative to the thread of the caller.
    /// \param returnType The type returned by the containing CoRoutined.
    /// \returns NewThreadAwaitable The awaitable type.
    template<typename returnType>
    auto NewThreadAwaitable() {

        class NewThreadAwaitable {
        public:

            bool await_ready() {
                return false;
            }

            bool await_suspend(coroutine_handle<CoRoutine<returnType>::promise_type>handle) {
                handle.promise().newThread = new std::jthread([handle] { handle.resume(); });
                return true;
            }

            void await_resume() {}
        };
        return NewThreadAwaitable();
    }
    /**@}*/

};
#endif 
