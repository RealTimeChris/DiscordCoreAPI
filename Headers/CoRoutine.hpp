// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COROUTINES_
#define _COROUTINES_

namespace DiscordCoreAPI {

    auto NewThreadAwaitable(jthread& out) {
        struct awaitable {
            jthread* p_out;
            bool await_ready() { return false; };
            void await_suspend(coroutine_handle<>handle) {
                std::jthread& out = *p_out;
                if (out.joinable()) {
                    throw std::runtime_error("Output jthread parameter not empty");
                }                    
                out = std::jthread([handle] { handle.resume(); });
                std::cout << "New thread ID: " << out.get_id() << '\n';
            }
            void await_resume() {};
        };
        return awaitable{ &out };
    }

    template<typename returnType>
    struct CoRoutine {
        struct promise_type;

        coroutine_handle<promise_type> coroutineHandle;

        CoRoutine(coroutine_handle<promise_type> h) : coroutineHandle(h) {};
        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            };
        }

        returnType get() {
            return coroutineHandle.promise().result;
        }
    protected:
        struct promise_type {
            returnType result = returnType();

            promise_type() {}
            ~promise_type() {}

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
                cout << "        promise_type::final_suspend:  "
                    << "this_thread::get_id(): "
                    << this_thread::get_id() << '\n';
                return{};
            }
            void unhandled_exception() {
                exit(1);
            }
        };
    };

    template<>
    struct CoRoutine<void> {
    public:
        struct promise_type;

        coroutine_handle<promise_type> coroutineHandle;
        bool haveWeReturned{ false };

        CoRoutine(coroutine_handle<promise_type> h) : coroutineHandle(h) {};
        ~CoRoutine() {
            if (coroutineHandle) coroutineHandle.destroy();
        }

        void get() {
            if (this->haveWeReturned) {
                return;
            }
            this->haveWeReturned = true;
            cout << "Thread in get() " << this_thread::get_id() << "\n";
            thread t([this] { coroutineHandle.resume(); });
            t.join();
            return;
        }

        struct promise_type {
            void* result{ nullptr };

            promise_type() {};
            ~promise_type() {};

            auto get_return_object() {
                return CoRoutine{ coroutine_handle<promise_type>::from_promise(*this) };
            }
            void return_void() {}
            suspend_always initial_suspend() {
                return{};
            }
            suspend_always final_suspend() noexcept {
                cout << "        promise_type::final_suspend:  "
                    << "this_thread::get_id(): "
                    << this_thread::get_id() << '\n';
                return{};
            }
            void unhandled_exception() {
                exit(1);
            }
        };
        promise_type promise{};
    };
};
#endif 
