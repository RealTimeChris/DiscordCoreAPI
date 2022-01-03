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
    struct DiscordCoreAPI_Dll InvalidState : public exception {
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

        CoRoutine<returnType>& operator=(CoRoutine<returnType>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = coroutine_handle<promise_type>();
                this->currentStatus = other.currentStatus;
                other.currentStatus = CoRoutineStatus::Cancelled;
            }            
            return *this;
        };

        CoRoutine(CoRoutine<returnType>&& other) noexcept {
            *this = move(other);
        };

        CoRoutine<returnType>& operator=(const CoRoutine<returnType>& other) = delete;

        CoRoutine(const CoRoutine<returnType>& other) = delete;

        CoRoutine<returnType>& operator=(CoRoutine<returnType>& other) = delete;

        CoRoutine(CoRoutine<returnType>& other) = delete;

        CoRoutine<returnType>() {
            this->currentStatus = CoRoutineStatus::Idle;
            this->coroutineHandle = nullptr;
        }

        CoRoutine<returnType>(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

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
                    while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
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
                    this->coroutineHandle.promise().requestStop();
                    {
                        lock_guard<mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                }
                exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
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

            friend auto NewThreadAwaitable<returnType>();
            friend class CoRoutine<returnType>;

            promise_type() {};

            void requestStop() {
                this->newThread->get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread->get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                unique_lock<mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, chrono::milliseconds(timeToWaitForInMs));
            }

            void return_value(returnType at) {
                this->result = at;
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

        protected:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<exception_ptr> exceptionBuffer{};
            unique_ptr<jthread> newThread{ nullptr };
            condition_variable condVar{};
            returnType result;
            mutex lockMutex{};

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

        class promise_type;

        CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = coroutine_handle<promise_type>();
                this->currentStatus = other.currentStatus;
                other.currentStatus = CoRoutineStatus::Cancelled;
            }
            return *this;
        };

        CoRoutine(CoRoutine<void>&& other) noexcept {
            *this = move(other);
        };

        CoRoutine<void>& operator=(const CoRoutine<void>& other) = delete;

        CoRoutine(const CoRoutine<void>& other) = delete;

        CoRoutine<void>& operator=(CoRoutine<void>& other) = delete;

        CoRoutine(CoRoutine<void>& other) = delete;

        CoRoutine() {
            this->currentStatus = CoRoutineStatus::Idle;
            this->coroutineHandle = nullptr;
        }

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

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
                    while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
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
                    this->coroutineHandle.promise().requestStop();
                    {
                        lock_guard<mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                }
                exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
            }
        }

        class promise_type {
        public:

            template<typename R>
            friend auto NewThreadAwaitable<void>();
            friend class CoRoutine<void>;

            promise_type() {};

            void requestStop() {
                this->newThread->get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread->get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                unique_lock<mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, chrono::milliseconds(timeToWaitForInMs));
            }

            unique_ptr<stop_token> getStopToken() {
                auto thePtr = this->newThread->get_stop_token();
                unique_ptr<stop_token> thePtr02(&thePtr);
                return thePtr02;
            }

            void return_void() {}

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

        protected:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<exception_ptr> exceptionBuffer{};
            unique_ptr<jthread> newThread{ nullptr };
            condition_variable condVar{};
            mutex lockMutex{};

        };

    protected:

        coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

    };

    /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \param returnType The type of the value returned by the containing CoRoutine.
    /// \returns A coroutine_handle<CoRoutine<returnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename returnType>
    auto NewThreadAwaitable() {
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

    struct CoRoutineWrapper {

        struct CoRoutineDeleter {
            void operator()(CoRoutine<void>* other) {
                if (other != nullptr) {
                    other->cancel();
                    other->get();
                    other = nullptr;
                }
            }
        };
        
        CoRoutineWrapper& operator=(CoRoutine<void>* other) {
            this->thePtr.reset(other);
            return *this;
        }

        operator CoRoutine<void>*() {
            return this->thePtr.get();
        }

        CoRoutine<void>*  operator->() {
            return this->thePtr.get();
        }

        unique_ptr<CoRoutine<void>, CoRoutineDeleter> operator*() {
            return move(this->thePtr);
        }

        CoRoutineWrapper(nullptr_t) {};

        CoRoutineWrapper(CoRoutine<void>* other) {
            this->thePtr.reset(other);
        }

    protected:

        unique_ptr<CoRoutine<void>, CoRoutineDeleter> thePtr{ nullptr, CoRoutineDeleter{} };
    };

    /**@}*/
};