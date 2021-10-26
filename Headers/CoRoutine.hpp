// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COROUTINES_
#define _COROUTINES_

namespace DiscordCoreAPI {

    template<typename returnType>
    struct CoRoutine {
    public:
        struct promise_type;

        CoRoutine(coroutine_handle<promise_type> h) : coroutineHandle(h) {};

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

        struct promise_type {
            returnType result = returnType();

            jthread* newThread{ nullptr };

            promise_type() {};

            ~promise_type() {};

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            void return_value(returnType v) {
                this->result = v;
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
    struct CoRoutine<void> {
    public:
        struct promise_type;
        CoRoutine(coroutine_handle<promise_type> h) : coroutineHandle(h) {};

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

        struct promise_type {

            jthread* newThread{ nullptr };

            promise_type() {};

            ~promise_type() {};

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            void return_void() {};

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
        struct NewThreadAwaitable {
            bool await_ready() { return false; };
            bool await_suspend(coroutine_handle<CoRoutine<returnType>::promise_type>handle) {
                handle.promise().newThread = new std::jthread([handle] { handle.resume(); });
                return true;
            }
            void await_resume() {};
        };
        return NewThreadAwaitable();
    }

};
#endif 
