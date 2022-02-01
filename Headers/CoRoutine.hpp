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

        class DiscordCoreAPI_Dll promise_type;

        CoRoutine<ReturnType>& operator=(CoRoutine<ReturnType>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<promise_type>();
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

        CoRoutine(std::coroutine_handle<promise_type> coroutineHandleNew) {
            this->coroutineHandle = coroutineHandleNew;
        };

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
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
        /// \returns ReturnType The return value of the CoRoutine.
        ReturnType get() {
            if (this != nullptr) {
                if (this->coroutineHandle.promise().newThread != nullptr) {
                    if (this->coroutineHandle.promise().newThread->joinable()) {
                        this->coroutineHandle.promise().newThread->join();
                    }
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

        class DiscordCoreAPI_Dll promise_type {
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
                return CoRoutine<ReturnType>{ std::coroutine_handle<promise_type>::from_promise(*this) };
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

            ~promise_type() {}

        protected:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            std::unique_ptr<std::jthread> newThread{ nullptr };
            std::condition_variable condVar{};
            ReturnType result;
            std::mutex lockMutex{};

        };

    protected:

        std::coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A CoRoutine - representing a potentially asynchronous operation/function (The void specialization).
    /// \param void The type of parameter that is returned by the CoRoutine.
    template<>
    class CoRoutine<void> {
    public:

        class DiscordCoreAPI_Dll promise_type;

        CoRoutine<void>& operator=(CoRoutine<void>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<promise_type>();
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

        CoRoutine(std::coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle && this->coroutineHandle.done()) {
                this->coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
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
        void get() {
            if (this != nullptr) {
                if (this->coroutineHandle.promise().newThread != nullptr) {
                    if (this->coroutineHandle.promise().newThread->joinable()) {
                        this->coroutineHandle.promise().newThread->join();
                    }
                }
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
            }
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
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
            }
        }

        class DiscordCoreAPI_Dll promise_type {
        public:

            template<typename ReturnType>
            friend auto NewThreadAwaitable();
            friend CoRoutine<void>;

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

            std::unique_ptr<std::stop_token> getStopToken() {
                auto thePtr = this->newThread->get_stop_token();
                std::unique_ptr<std::stop_token> thePtr02(&thePtr);
                return thePtr02;
            }

            void return_void() {}

            auto get_return_object() {
                return CoRoutine<void>{ std::coroutine_handle<promise_type>::from_promise(*this) };
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

            ~promise_type() {}

        protected:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            std::unique_ptr<std::jthread> newThread{ nullptr };
            std::condition_variable condVar{};
            std::mutex lockMutex{};

        };

    protected:

        std::coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

    };

    /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \param ReturnType The type of the value returned by the containing CoRoutine.
    /// \returns A std::coroutine_handle<CoRoutine<ReturnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename ReturnType>
    auto NewThreadAwaitable() {
        class DiscordCoreAPI_Dll NewThreadAwaitableClass {
        public:

            std::coroutine_handle<CoRoutine<ReturnType>::promise_type> waiterHandle{ nullptr };

            NewThreadAwaitableClass() {};

            bool await_ready() const noexcept {
                return false;
            }

            bool await_suspend(std::coroutine_handle<CoRoutine<ReturnType>::promise_type>handle) {
                this->waiterHandle = handle;
                this->waiterHandle.promise().newThread = std::make_unique<std::jthread>([=] { this->waiterHandle.resume(); });
                return true;
            }

            auto await_resume() {
                return this->waiterHandle;
            }
        };
        return NewThreadAwaitableClass{};
    }

  struct DiscordCoreAPI_Dll CoRoutineWrapper {

      struct DiscordCoreAPI_Dll CoRoutineDeleter {
            void operator()(CoRoutine<void>* other) {
                if (other != nullptr) {
                    other->cancel();
                    other->get();
                    other = nullptr;
                }
            }
        };
        
        CoRoutineWrapper& operator=(CoRoutine<void> other) {
            this->theCoroutine.reset(&other);
            return *this;
        }

        CoRoutineWrapper& operator=(CoRoutine<void>* other) {
            this->theCoroutine.reset(other);
            return *this;
        }

        operator CoRoutine<void>*() {
            return this->theCoroutine.get();
        }

        CoRoutine<void>*  operator->() {
            return this->theCoroutine.get();
        }

        std::unique_ptr<CoRoutine<void>, CoRoutineDeleter> operator*() {
            return std::move(this->theCoroutine);
        }

        CoRoutineWrapper(nullptr_t) {};

        CoRoutineWrapper(CoRoutine<void>* other) {
            this->theCoroutine.reset(other);
        }

    protected:

        std::unique_ptr<CoRoutine<void>, CoRoutineDeleter> theCoroutine{ nullptr, CoRoutineDeleter{} };
    };

    /**@}*/
};
#endif