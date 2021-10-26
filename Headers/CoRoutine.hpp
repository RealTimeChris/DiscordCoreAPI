// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COROUTINES_
#define _COROUTINES_

namespace DiscordCoreAPI {

    enum class CoRoutineStatus {
        Idle = 0,
        Running = 1,
        Complete = 2,
        Cancelled = 3
    };

    template<typename returnType>
    class CoRoutine {
    public:
        class promise_type;

        coroutine_handle<promise_type> coroutineHandle{};

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {}

        CoRoutine() {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        CoRoutineStatus getStatus() {
            return coroutineHandle.promise().currentStatus;
        }

        returnType get() {
            if (coroutineHandle.promise().newThread != nullptr) {
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
            return coroutineHandle.promise().result;
        }

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

    template<>
    class CoRoutine<void> {
    public:
        class promise_type;

        coroutine_handle<promise_type> coroutineHandle{};

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {}

        CoRoutine() {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        CoRoutineStatus getStatus() {
            return coroutineHandle.promise().currentStatus;
        }

        void get() {
            if (coroutineHandle.promise().newThread != nullptr) {
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
            return;
        }

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

};
#endif 
