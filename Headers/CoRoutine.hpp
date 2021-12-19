// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup utilities
    * @{
    */
    /// An exception for when the CoRoutine is not in the correct state. \brief An exception for when the CoRoutine is not in the correct state.
    struct InvalidState : public exception {
    public:
        explicit InvalidState(const string& message) : exception(message.c_str()) {};
    };

    /// The current status of the associated CoRoutine. \brief The current status of the associated CoRoutine.
    enum class CoRoutineStatus {
        Idle = 0,///< Idle.
        Running = 1,///< Running.
        Complete = 2,///< Complete.
        Cancelled = 3///< Cancelled.
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
    /// \param returnType The type of parameter that is returned by the CoRoutine.
    template<typename returnType>
    class CoRoutine {
    public:

        class promise_type;

        CoRoutine<returnType>& operator=(const CoRoutine<returnType>& other) = delete;

        CoRoutine(const CoRoutine<returnType>& other) = delete;

        CoRoutine<returnType>& operator=(CoRoutine<returnType>& other) = delete;

        CoRoutine(CoRoutine<returnType>& other) = delete;

        CoRoutine<returnType>& operator=(CoRoutine<returnType>&& other) noexcept {
            this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
            other.coroutineHandle = coroutine_handle<promise_type>();
            other.currentStatus = CoRoutineStatus::Cancelled;
            this->currentStatus = other.currentStatus;
            return *this;
        };

        CoRoutine(CoRoutine<returnType>&& other) noexcept {
            *this = move(other);
        };

        CoRoutine<returnType>(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        CoRoutine<returnType>() {}

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
         /// \returns void.
        void detachThread() {
            if (this != nullptr) {
                if (this->coroutineHandle) {
                    if (this->coroutineHandle.promise().newThread != nullptr) {
                        this->coroutineHandle.promise().newThread->detach();
                    }
                }
            }
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr) {
                if (this->coroutineHandle) {
                    this->currentStatus = coroutineHandle.promise().currentStatus;
                }
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns returnType The return value of the CoRoutine.
        returnType get(void) {
            if (this != nullptr) {
                if (!this->coroutineHandle) {
                    throw InvalidState("CoRoutine is not initialized with a proper task.");
                }
                else {
                    if (this->coroutineHandle.promise().newThread != nullptr) {
                        if (this->coroutineHandle.promise().newThread->joinable()) {
                            this->coroutineHandle.promise().newThread->join();
                        }
                    }
                    exception_ptr exceptionPtr{};
                    while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                        rethrow_exception(exceptionPtr);
                    }
                    this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                    this->currentStatus = this->coroutineHandle.promise().currentStatus;
                    return this->coroutineHandle.promise().result;
                }
            }
            return returnType{};
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns void.
        returnType cancel() {
            if (this != nullptr && this->coroutineHandle && this->coroutineHandle.promise().newThread != nullptr) {
                if (this->coroutineHandle.promise().newThread->joinable()) {
                    this->coroutineHandle.promise().newThread->get_stop_source().request_stop();
                    {
                        lock_guard<mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                }
                exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                    rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return this->coroutineHandle.promise().result;

            }
            return returnType{};
        }

        class promise_type {
        public:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<exception_ptr> exceptionBuffer{};
            unique_ptr<jthread> newThread{ nullptr };
            condition_variable condVar{};
            returnType result;
            mutex lockMutex{};

            promise_type() {};

            void waitForTime(uint64_t timeToWaitForInMs) {
                unique_lock<mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, chrono::milliseconds(timeToWaitForInMs));
            }

            void return_value(returnType returnValue) {
                this->result = returnValue;
            }

            auto get_return_object() {
                return CoRoutine<returnType>{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            suspend_always final_suspend() noexcept {
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(current_exception());
            }

            ~promise_type() {}
        };

    protected:

        coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A CoRoutine - representing a potentially asynchronous operation/function (The void specialization).
    /// \param void The type of parameter that is returned by the CoRoutine.
    template<>
    class CoRoutine<void> {
    public:

        class promise_type;

        CoRoutine<void>& operator=(const CoRoutine<void>& other) = delete;

        CoRoutine(const CoRoutine<void>& other) = delete;

        CoRoutine<void>& operator=(CoRoutine<void>& other) = delete;

        CoRoutine(CoRoutine<void>& other) = delete;

        CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
            this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
            other.coroutineHandle = coroutine_handle<promise_type>();
            other.currentStatus = CoRoutineStatus::Cancelled;
            this->currentStatus = other.currentStatus;
            return *this;
        };

        CoRoutine(CoRoutine<void>&& other) noexcept {
            *this = move(other);
        };

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        CoRoutine() {}

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        /// \returns void.
        void detachThread() {
            if (this != nullptr) {
                if (this->coroutineHandle) {
                    if (this->coroutineHandle.promise().newThread != nullptr) {
                        this->coroutineHandle.promise().newThread->detach();
                    }
                }
            }
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr) {
                if (this->coroutineHandle) {
                    this->currentStatus = coroutineHandle.promise().currentStatus;
                }
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns returnType The return value of the CoRoutine.
        void get(void) {
            if (this != nullptr) {
                if (!this->coroutineHandle) {
                    throw InvalidState("CoRoutine is not initialized with a proper task.");
                }
                else {
                    if (this->coroutineHandle.promise().newThread != nullptr) {
                        if (this->coroutineHandle.promise().newThread->joinable()) {
                            this->coroutineHandle.promise().newThread->join();
                        }
                    }
                    exception_ptr exceptionPtr{};
                    while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                        rethrow_exception(exceptionPtr);
                    }
                    this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                    this->currentStatus = this->coroutineHandle.promise().currentStatus;
                }
            }
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns void.
        void cancel() {
            if (this != nullptr && this->coroutineHandle && this->coroutineHandle.promise().newThread != nullptr) {
                if (this->coroutineHandle.promise().newThread->joinable()) {
                    this->coroutineHandle.promise().newThread->get_stop_source().request_stop();
                    {
                        lock_guard<mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                }
            }
            exception_ptr exceptionPtr{};
            while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                rethrow_exception(exceptionPtr);
            }
            this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
            this->currentStatus = this->coroutineHandle.promise().currentStatus;
        }

        class promise_type {
        public:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<exception_ptr> exceptionBuffer{};
            unique_ptr<jthread> newThread{ nullptr };
            condition_variable condVar{};
            mutex lockMutex{};

            promise_type() {};

            void waitForTime(uint64_t timeToWaitForInMs) {
                unique_lock<mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, chrono::milliseconds(timeToWaitForInMs));
            }

            void return_void() {};

            auto get_return_object() {
                return CoRoutine<void>{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            suspend_always final_suspend() noexcept {
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(current_exception());
            }

            ~promise_type() {}
        };

    protected:

        coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

    };

    /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \param returnType The type of the value returned by the containing CoRoutine.
    /// \returns A coroutine_handle<CoRoutine<returnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename returnType>
    inline auto NewThreadAwaitable() {
        class NewThreadAwaitable {
        public:

            coroutine_handle<CoRoutine<returnType>::promise_type> waiterHandle{ nullptr };

            NewThreadAwaitable() {};

            bool await_ready() const noexcept {
                return false;
            }

            bool await_suspend(coroutine_handle<CoRoutine<returnType>::promise_type>handle) {
                this->waiterHandle = handle;
                this->waiterHandle.promise().newThread = make_unique<jthread>([=] { this->waiterHandle.resume(); });
                return true;
            }

            auto await_resume() {
                return this->waiterHandle;
            }
        };
        return NewThreadAwaitable();
    }

    /**@}*/
};