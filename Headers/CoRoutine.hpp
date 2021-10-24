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
        struct promise_type;
        coroutine_handle<promise_type> coroutineHandle;

        CoRoutine(coroutine_handle<promise_type> h) : coroutineHandle(h) {};
        ~CoRoutine() {
            if (coroutineHandle) {
                coroutineHandle.destroy();
            };
        }

        returnType get() {
            jthread newThread = jthread([&]() {coroutineHandle.resume(); });
            newThread.join();
            return coroutineHandle.promise().result;
        }

        struct promise_type {
            returnType result = returnType();

            promise_type() {};
            ~promise_type() {};

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }
            void return_value(returnType v) {
                cout << "        promise_type::return_value:  "
                    << "this_thread::get_id(): "
                    << this_thread::get_id() << '\n';
                this->result = v;
            }
            suspend_always initial_suspend() {
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

    auto NewThreadAwaitableFunction() {
        struct NewThreadAwaitable {
            bool await_ready() { return false; };
            bool await_suspend(coroutine_handle<>handle) {
                jthread newThread = std::jthread([handle] { handle.resume(); });
                std::cout << "New thread ID: " << newThread.get_id() << '\n';
                return false;
            }
            void await_resume() {};
        };
        return NewThreadAwaitable();
    };

    template<>
    struct CoRoutine<void> {
        struct promise_type;

        coroutine_handle<promise_type> coroutineHandle;

        CoRoutine(coroutine_handle<promise_type> h) : coroutineHandle(h) {};
        ~CoRoutine() {
            if (coroutineHandle) {
                coroutineHandle.promise().~promise_type();
            };
        }

        void get() {
            jthread newThread = jthread([&]() {coroutineHandle.resume(); });
            newThread.join();
            return;
        }

        struct promise_type {
            void* result{ nullptr };

            promise_type() {};
            ~promise_type() {};

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }
            void return_void() {  };
            suspend_always initial_suspend() {
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

};
#endif 
