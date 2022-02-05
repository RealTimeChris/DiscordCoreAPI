// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef COROUTINE
#define COROUTINE

#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup utilities
    * @{
    */

    /// The current status of the associated CoRoutine. \brief The current status of the associated CoRoutine.
    enum class CoRoutineStatus {
        Idle = 0,///< Idle.
        Running = 1,///< Running.
        Complete = 2,///< Complete.
        Cancelled = 3///< Cancelled.
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
    /// \param ReturnType The type of parameter that is returned by the CoRoutine.
    template<typename ReturnType>
    class CoRoutine {
    public:

        class promise_type {
        public:

            template<typename ReturnType02>
            friend auto NewThreadAwaitable();
            template<typename ReturnType02>
            friend class CoRoutine;

            promise_type() {};

            void requestStop() {
                this->newThread->get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread->get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds(timeToWaitForInMs));
            }

            void return_value(ReturnType at) {
                this->result = at;
            }

            auto get_return_object() {
                return CoRoutine<ReturnType>{ std::coroutine_handle<CoRoutine<ReturnType>::promise_type>::from_promise(*this) };
            }

            std::suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            std::suspend_always final_suspend() noexcept {
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(std::current_exception());
            }

            ~promise_type() {};

        protected:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            std::unique_ptr<std::jthread> newThread{ nullptr };
            std::condition_variable condVar{};
            std::mutex lockMutex{};
            ReturnType result;
        };

        CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<CoRoutine<ReturnType>::promise_type>();
                this->currentStatus = other.currentStatus;
                other.currentStatus = CoRoutineStatus::Cancelled;
            }
            return *this;
        };

        CoRoutine(CoRoutine<ReturnType>&& other) noexcept {
            *this = std::move(other);
        };

        CoRoutine<ReturnType>& operator=(const CoRoutine<ReturnType>& other) = delete;

        CoRoutine(const CoRoutine<ReturnType>& other) = delete;

        CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>& other) = delete;

        CoRoutine(CoRoutine<ReturnType>& other) = delete;

        CoRoutine() {
            this->currentStatus = CoRoutineStatus::Idle;
            this->coroutineHandle = nullptr;
        }

        CoRoutine(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        void detachThread() {
            if (this != nullptr && this->coroutineHandle && this->coroutineHandle.promise().newThread != nullptr) {
                this->coroutineHandle.promise().newThread->detach();
            }
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr && this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns ReturnType The return value of the CoRoutine.
        ReturnType get() {
            if (this != nullptr && this->coroutineHandle.promise().newThread != nullptr) {
                if (this->coroutineHandle.promise().newThread->joinable()) {
                    this->coroutineHandle.promise().newThread->join();
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return this->coroutineHandle.promise().result;
            }
            return ReturnType{};
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns ReturnType The object to be returned.
        ReturnType cancel() {
            if (this != nullptr && this->coroutineHandle && this->coroutineHandle.promise().newThread != nullptr) {
                if (this->coroutineHandle.promise().newThread->joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return this->coroutineHandle.promise().result;
            }
            return ReturnType{};
        }

    protected:
        std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandle{ nullptr };
        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A CoRoutine - representing a potentially asynchronous operation/function (The void specialization).
    /// \param void The type of parameter that is returned by the CoRoutine.
    /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
    /// \param void The type of parameter that is returned by the CoRoutine.
    template<>
    class CoRoutine<void> {
    public:

        class promise_type {
        public:

            template<typename void02>
            friend auto NewThreadAwaitable();
            template<typename void02>
            friend class CoRoutine;

            promise_type() {};

            void requestStop() {
                this->newThread->get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread->get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds(timeToWaitForInMs));
            }

            void return_void() {
                return;
            }

            auto get_return_object() {
                return CoRoutine<void>{ std::coroutine_handle<CoRoutine<void>::promise_type>::from_promise(*this) };
            }

            std::suspend_never initial_suspend() {
                this->currentStatus = CoRoutineStatus::Running;
                return{};
            }

            std::suspend_always final_suspend() noexcept {
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                this->exceptionBuffer.send(std::current_exception());
            }

            ~promise_type() {};

        protected:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            std::unique_ptr<std::jthread> newThread{ nullptr };
            std::condition_variable condVar{};
            std::mutex lockMutex{};
        };

        CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<CoRoutine<void>::promise_type>();
                this->currentStatus = other.currentStatus;
                other.currentStatus = CoRoutineStatus::Cancelled;
            }
            return *this;
        };

        CoRoutine(CoRoutine<void>&& other) noexcept {
            *this = std::move(other);
        };

        CoRoutine<void>& operator=(const CoRoutine<void>& other) = delete;

        CoRoutine(const CoRoutine<void>& other) = delete;

        CoRoutine<void>& operator=(CoRoutine<void>& other) = delete;

        CoRoutine(CoRoutine<void>& other) = delete;

        CoRoutine() {
            this->currentStatus = CoRoutineStatus::Idle;
            this->coroutineHandle = nullptr;
        }

        CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        void detachThread() {
            if (this != nullptr && this->coroutineHandle && this->coroutineHandle.promise().newThread != nullptr) {
                this->coroutineHandle.promise().newThread->detach();
            }
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr && this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns void The return value of the CoRoutine.
        void get() {
            if (this != nullptr && this->coroutineHandle.promise().newThread != nullptr) {
                if (this->coroutineHandle.promise().newThread->joinable()) {
                    this->coroutineHandle.promise().newThread->join();
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return;
            }
            return;
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns void The object to be returned.
        void cancel() {
            if (this != nullptr && this->coroutineHandle && this->coroutineHandle.promise().newThread != nullptr) {
                if (this->coroutineHandle.promise().newThread->joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                return;
            }
            return;
        }

    protected:
        std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandle{ nullptr };
        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \param ReturnType The type of the value returned by the containing CoRoutine.
    /// \returns A std::coroutine_handle<CoRoutine<ReturnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename ReturnType>
    auto NewThreadAwaitable() {

        struct NewThreadAwaitableClass {

            std::coroutine_handle<class CoRoutine<ReturnType>::promise_type> coroHandle{ nullptr };

            bool await_ready() {
                return false;
            };

            void await_suspend(std::coroutine_handle<class CoRoutine<ReturnType>::promise_type> handle) {
                this->coroHandle = handle;
                this->coroHandle.promise().newThread = std::make_unique<std::jthread>([=, this] { this->coroHandle.resume(); });
            }

            auto await_resume() {
                return this->coroHandle;
            }
        };
        return NewThreadAwaitableClass{};
    }

    /**@}*/
};
#endif