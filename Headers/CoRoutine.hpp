// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COROUTINES_
#define _COROUTINES_

namespace DiscordCoreAPI {

    template<typename returnType>
    class CoRoutine {
    public:
        class promise_type;

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        returnType Get() {
            if (coroutineHandle.promise().newThread != nullptr) {
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            return coroutineHandle.promise().result;
        }

        class promise_type {
        public:
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
        coroutine_handle<promise_type> coroutineHandle{};
    };

    template<>
    class CoRoutine<void> {
    public:
        class promise_type;

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {}

        ~CoRoutine() {

            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        void Get() {
            if (coroutineHandle.promise().newThread != nullptr) {
                if (coroutineHandle.promise().newThread->joinable()) {
                    coroutineHandle.promise().newThread->join();
                }
            }
            return;
        }

        class promise_type {
        public:
            jthread* newThread{ nullptr };

            promise_type() {}

            ~promise_type() {}

            void return_void() {}

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            suspend_never initial_suspend() {
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
        coroutine_handle<promise_type> coroutineHandle{};
    };

    template<typename returnType>
    auto NewThreadAwaitableFunction() {

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
