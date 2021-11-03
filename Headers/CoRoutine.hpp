// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COROUTINES_
#define _COROUTINES_

#include "IndexInitial.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup utilities
    * @{
    */
    /// An exception for when the CoRoutine is not in the correct state. \brief An exception for when the CoRoutine is not in the correct state.
    class DiscordCoreAPI_Dll InvalidState : public exception {
    public:
        explicit  InvalidState(const string& _Message) : exception(_Message.c_str()) {}
    };

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
    class DiscordCoreAPI_Dll CoRoutine {
    public:

        class promise_type;

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
        returnType get(void) {
            if (!coroutineHandle) {
                throw InvalidState("CoRoutine is not initialized with a proper task.");
            }
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
            if (!coroutineHandle) {
                throw InvalidState("CoRoutine is not initialized with a proper task.");
            }
            if (coroutineHandle.promise().newThread != nullptr) {
                coroutineHandle.promise().newThread->get_stop_source().request_stop();
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
            return coroutineHandle.promise().result;
        }

        class DiscordCoreAPI_Dll promise_type {
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

    protected:

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

        coroutine_handle<promise_type> coroutineHandle{};
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A CoRoutine - representing a potentially asynchronous operation/function (The void specialization).
    /// \param void The type of parameter that is returned by the CoRoutine.
    template<>
    class CoRoutine<void> {
    public:

        class promise_type;

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
        void get(void) {
            if (!coroutineHandle) {
                throw InvalidState("CoRoutine is not initialized with a proper task.");
            }
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
            if (!coroutineHandle) {
                throw InvalidState("CoRoutine is not initialized with a proper task.");
            }
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

    protected:

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

        coroutine_handle<promise_type> coroutineHandle{};
    };

    /// Used to set the CoRoutine into executing on a new thread, relative to the thread of the caller, as well as acquire the CoRoutine handle. \brief Used to set the CoRoutine into executing on a new thread, relative to the thread of the caller, as well as acquire the CoRoutine handle.
    /// \param returnType The type returned by the containing CoRoutined.
    template<class returnType>
    inline DiscordCoreAPI_Dll auto NewThreadAwaitable() {
        class NewThreadAwaitable {
        public:

            coroutine_handle<CoRoutine<returnType>::promise_type> handleWaiter;

            NewThreadAwaitable() : handleWaiter(nullptr) {}

            bool await_ready() const noexcept {
                return false;
            }

            bool await_suspend(coroutine_handle<CoRoutine<returnType>::promise_type>handle) {
                handle.promise().newThread = new std::jthread([handle] { handle.resume(); });
                this->handleWaiter = handle;
                return true;
            }

            auto await_resume() {
                return this->handleWaiter;
            }
        };
        return NewThreadAwaitable();
    }
    /**@}*/
};
#endif 