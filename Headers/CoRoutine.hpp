// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup utilities
    * @{
    */
    /// An exception for when the CoRoutine is not in the correct state. \brief An exception for when the CoRoutine is not in the correct state.
    struct DiscordCoreAPI_Dll InvalidState : public exception {
    public:
        explicit InvalidState(const string & message) : exception(message.c_str()) {}
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
    class DiscordCoreAPI_Dll CoRoutine {
    public:

        class DiscordCoreAPI_Dll promise_type;

        CoRoutine<returnType>& operator=(CoRoutine<returnType>&& other) noexcept {
            this->coroutineHandle = move(other.coroutineHandle);
            this->currentStatus = other.currentStatus;
            return *this;
        }

        CoRoutine(CoRoutine<returnType>&& other) noexcept {
            *this = move(other);
        }

        CoRoutine<returnType>& operator=(const CoRoutine<returnType>&) = delete;

        CoRoutine(const CoRoutine<returnType>&) = delete;

        CoRoutine<returnType>(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        CoRoutine<returnType>() {}

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.get_stop_source().request_stop();
                    this->coroutineHandle.promise().newThread.join();
                }
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        /// \returns void.
        void detachThread() {
            if (this->coroutineHandle) {
                this->coroutineHandle.promise().newThread.detach();
            }
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this->coroutineHandle) {
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
            else {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.join();
                }
                exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                    rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                return this->coroutineHandle.promise().result;
            }
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns returnType The return value of the CoRoutine.
        returnType cancel() {
            if (this != nullptr) {
                if (this->coroutineHandle) {
                    if (this->coroutineHandle.promise().newThread.joinable()) {
                        this->coroutineHandle.promise().newThread.get_stop_source().request_stop();
                        this->coroutineHandle.promise().newThread.join();
                    }
                    exception_ptr exceptionPtr{};
                    while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                        rethrow_exception(exceptionPtr);
                    }
                    this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                    return this->coroutineHandle.promise().result;
                }
            }
        }

        class DiscordCoreAPI_Dll promise_type {
        public:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

            UnboundedMessageBlock<exception_ptr> exceptionBuffer{};

            jthread newThread{};

            returnType result;

            promise_type() {}

            ~promise_type() {}

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
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(current_exception());
            }
        };

    protected:

        coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A CoRoutine - representing a potentially asynchronous operation/function (The void specialization).
    /// \param void The type of parameter that is returned by the CoRoutine.
    template<>
    class DiscordCoreAPI_Dll CoRoutine<void> {
    public:

        class DiscordCoreAPI_Dll promise_type;

        CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
            this->coroutineHandle = move(other.coroutineHandle);
            this->currentStatus = other.currentStatus;
            return *this;
        }

        CoRoutine(CoRoutine<void>&& other) noexcept {
            *this = move(other);
        }

        CoRoutine<void>& operator=(const CoRoutine<void>&) = delete;

        CoRoutine(const CoRoutine<void>&) = delete;

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        CoRoutine() {}

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.get_stop_source().request_stop();
                    this->coroutineHandle.promise().newThread.join();
                }
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        /// \returns void.
        void detachThread() {
            if (this->coroutineHandle) {
                this->coroutineHandle.promise().newThread.detach();
            }
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
            }
            return this->currentStatus;
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns void.
        void get(void) {
            if (!this->coroutineHandle) {
                throw InvalidState("CoRoutine is not initialized with a proper task.");
            }
            else {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.join();
                }
                exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                    rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
            }
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns void.
        void cancel() {
            if (this != nullptr) {
                if (this->coroutineHandle) {
                    if (this->coroutineHandle.promise().newThread.joinable()) {
                        this->coroutineHandle.promise().newThread.get_stop_source().request_stop();
                        this->coroutineHandle.promise().newThread.join();
                    }
                    exception_ptr exceptionPtr{};
                    while (this->coroutineHandle.promise().exceptionBuffer.try_receive(exceptionPtr)) {
                        rethrow_exception(exceptionPtr);
                    }
                    this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                }
            }
        }

        class DiscordCoreAPI_Dll promise_type {
        public:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

            UnboundedMessageBlock<exception_ptr> exceptionBuffer{};

            jthread newThread{};

            promise_type() {};

            ~promise_type() {};

            void return_void() {};

            auto get_return_object() {
                return CoRoutine<void>{ coroutine_handle<promise_type>::from_promise(*this) };
            }

            suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            suspend_always final_suspend() noexcept {
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(current_exception());
            }
        };

    protected:

        coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \param returnType The type of the value returned by the containing CoRoutine.
    /// \returns A coroutine_handle<CoRoutine<returnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename returnType>
    DiscordCoreAPI_Dll inline auto NewThreadAwaitable() {
        class DiscordCoreAPI_Dll NewThreadAwaitable {
        public:

            coroutine_handle<CoRoutine<returnType>::promise_type> waiterHandle{ nullptr };

            NewThreadAwaitable() {}

            bool await_ready() const noexcept {
                return false;
            }

            bool await_suspend(coroutine_handle<CoRoutine<returnType>::promise_type>handle) {
                this->waiterHandle = handle;
                this->waiterHandle.promise().newThread = jthread([=] { this->waiterHandle.resume(); });
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