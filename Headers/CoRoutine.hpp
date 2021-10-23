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

        std::coroutine_handle<promise_type> coroutineHandle;
        bool haveWeReturned{ false };

        CoRoutine(std::coroutine_handle<promise_type> h) : coroutineHandle(h) {};
        ~CoRoutine() {
            if (coroutineHandle) coroutineHandle.destroy();
        }

        returnType get() {
            if (this->haveWeReturned) {
                return returnType();
            }
            this->haveWeReturned = true;
            std::cout << "Thread in get() " << std::this_thread::get_id() << "\n";
            std::thread t([this] { coroutineHandle.resume(); });
            t.join();
            return *coroutineHandle.promise().result.get();
        }
    protected:
        struct promise_type {
            shared_ptr<returnType> result = shared_ptr<returnType>(new returnType());

            promise_type() {}
            ~promise_type() {}

            auto get_return_object() {
                return CoRoutine{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            void return_value(returnType v) {
                std::cout << "        promise_type::return_value:  "
                    << "std::this_thread::get_id(): "
                    << std::this_thread::get_id() << '\n';
            }
            std::suspend_always initial_suspend() {
                return{};
            }
            std::suspend_always final_suspend() noexcept {
                std::cout << "        promise_type::final_suspend:  "
                    << "std::this_thread::get_id(): "
                    << std::this_thread::get_id() << '\n';
                return{};
            }
            void unhandled_exception() {
                std::exit(1);
            }
        };
    };

    template<>
    struct CoRoutine<void> {
    public:
        struct promise_type;

        std::coroutine_handle<promise_type> coroutineHandle;
        bool haveWeReturned{ false };

        CoRoutine(std::coroutine_handle<promise_type> h) : coroutineHandle(h) {};
        ~CoRoutine() {
            if (coroutineHandle) coroutineHandle.destroy();
        }

        void get() {
            if (this->haveWeReturned) {
                return;
            }
            this->haveWeReturned = true;
            std::cout << "Thread in get() " << std::this_thread::get_id() << "\n";
            std::thread t([this] { coroutineHandle.resume(); });
            t.join();
            return;
        }

        struct promise_type {
            void* result;

            promise_type() {};
            ~promise_type() {};

            auto get_return_object() {
                return CoRoutine{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            void return_void() {}
            std::suspend_always initial_suspend() {
                return{};
            }
            std::suspend_always final_suspend() noexcept {
                std::cout << "        promise_type::final_suspend:  "
                    << "std::this_thread::get_id(): "
                    << std::this_thread::get_id() << '\n';
                return{};
            }
            void unhandled_exception() {
                std::exit(1);
            }
        };
        promise_type promise{};
    };
};
#endif 
