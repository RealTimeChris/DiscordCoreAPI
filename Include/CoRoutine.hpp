/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// CoRoutine.hpp - Header for the coroutine class.
/// Oct 23, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file CoRoutine.hpp

#pragma once

#include <FoundationEntities.hpp>
#include <stop_token>
#include <coroutine>
#include <Http.hpp>
#include <future>

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

    class HttpThreadPool
    {
    public:

        HttpThreadPool(uint32_t numOfThreadsNew = std::thread::hardware_concurrency()) : numOfThreads(numOfThreadsNew) {
            for (int32_t x = 0; x < this->numOfThreads; x += 1) {
                this->threads.push_back(std::jthread(&HttpThreadPool::threadFunction, this, std::stop_token()));
            }
        }

        void threadFunction(std::stop_token stopToken) {

            while (running && !stopToken.stop_requested()) {
                std::function<void()> task{};
                if (!paused && popTask(task)) {
                    task();
                    taskCount--;
                }
                else
                {
                    sleepOrYield();
                }
            }

        }

        template <typename LambdaType>
        void pushTask(const LambdaType& task) {
            taskCount++;
            {
                std::scoped_lock<std::recursive_mutex> lock(accessMutex);
                tasks.push(std::move(std::function<void()>{ task }));
            }
        }

        template <typename LambdaType, typename... ArgTypes>
        void pushTask(LambdaType& task, ArgTypes &...args) {
            pushTask([task, args...]{ task(args...); });
        }

        template<typename ReturnType>
        std::future<ReturnType> submit(DiscordCoreInternal::HttpClient& httpClient, DiscordCoreInternal::HttpWorkloadData& task)
        {
            ReferenceCountingPtr<std::promise<ReturnType>> task_promise(new std::promise<ReturnType>{});
            std::future<ReturnType> future = task_promise->get_future();
            pushTask([&]
                {
                    try
                    {
                        task_promise->set_value(DiscordCoreInternal::submitWorkloadAndGetResult<ReturnType>(httpClient, task));
                    }
                    catch (...)
                    {
                        try
                        {
                            task_promise->set_exception(std::current_exception());
                        }
                        catch (...)
                        {
                        }
                    }
                });
            return future;
        }

        std::future<void> submit(DiscordCoreInternal::HttpClient& httpClient, DiscordCoreInternal::HttpWorkloadData& task)
        {
            ReferenceCountingPtr<std::promise<void>> task_promise(new std::promise<void>{});
            std::future<void> future = task_promise->get_future();
            pushTask([&, &promiseNew = *task_promise]
                {
                    try
                    {
                        DiscordCoreInternal::submitWorkloadAndGetResult<void>(httpClient, task);
                        task_promise->set_value();
                    }
                    catch (...)
                    {
                        try
                        {
                            task_promise->set_exception(std::current_exception());
                        }
                        catch (...)
                        {
                        }
                    }
                });
            return future;
        }

        ~HttpThreadPool()
        {
            running = false;
            destroyThreads();
        }

    protected:

        mutable std::recursive_mutex accessMutex{};
        std::queue<std::function<void()>> tasks{};
        std::atomic<uint32_t> taskCount{ 0 };
        std::vector<std::jthread> threads{};
        std::atomic<bool> paused = false;
        std::atomic<bool> running = true;
        int32_t sleepDuration = 1000;
        int32_t numOfThreads{ 0 };

        void destroyThreads() {
            for (int32_t i = 0; i < numOfThreads; i++) {
                if (threads[i].joinable()) {
                    threads[i].join();
                }
            }
        }

        bool popTask(std::function<void()>& task) {
            const std::scoped_lock<std::recursive_mutex> lock(accessMutex);
            if (tasks.empty()) {
                return false;
            }
            else {
                task = std::move(tasks.front());
                tasks.pop();
                return true;
            }
        }

        void sleepOrYield() {
            if (sleepDuration) {
                std::this_thread::sleep_for(std::chrono::microseconds(sleepDuration));
            }
            else {
                std::this_thread::yield();
            }
        }
    };

    namespace CoRoutineTP {
        namespace {
            HttpThreadPool threadPool{};
        }
    }

    /// A HttpCoRoutine - representing a potentially asynchronous operation/function. \brief A HttpCoRoutine - representing a potentially asynchronous operation/function.
    /// \param ReturnType The type of parameter that is returned by the HttpCoRoutine.
    template<typename ReturnType>
    class HttpCoRoutine {
    public:


        class promise_type {
        public:

            template<typename ReturnType02>
            friend auto NewThreadAwaitable();
            template<typename ReturnType02>
            friend class HttpCoRoutine;

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds(timeToWaitForInMs));
            }

            void return_value(ReturnType at) {
                this->result = at;
            }

            auto get_return_object() {
                return HttpCoRoutine<ReturnType>{ std::coroutine_handle<HttpCoRoutine<ReturnType>::promise_type>::from_promise(*this) };
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

            std::future<ReturnType> theFuture{};

        protected:

            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            std::condition_variable condVar{};
            std::mutex lockMutex{};
            ReturnType result;
        };

        HttpCoRoutine<ReturnType>& operator=(HttpCoRoutine<ReturnType>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<HttpCoRoutine<ReturnType>::promise_type>();
                this->currentStatus = other.currentStatus;
                other.currentStatus = CoRoutineStatus::Cancelled;
            }
            return *this;
        };

        HttpCoRoutine(HttpCoRoutine<ReturnType>&& other) noexcept {
            *this = std::move(other);
        };

        HttpCoRoutine<ReturnType>& operator=(const HttpCoRoutine<ReturnType>& other) = delete;

        HttpCoRoutine(const HttpCoRoutine<ReturnType>& other) = delete;

        HttpCoRoutine<ReturnType>& operator=(HttpCoRoutine<ReturnType>& other) = delete;

        HttpCoRoutine(HttpCoRoutine<ReturnType>& other) = delete;

        HttpCoRoutine() = default;

        HttpCoRoutine(std::coroutine_handle<HttpCoRoutine<ReturnType>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~HttpCoRoutine() {
            if (this->coroutineHandle) {
                if (this->coroutineHandle.done()) {
                    this->coroutineHandle.destroy();
                }
            }
        }

        /// Collects the status of the HttpCoRoutine. \brief Collects the status of the HttpCoRoutine.
        /// \returns CoRoutineStatus The status of the HttpCoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr && this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

        /// Gets the resulting value of the HttpCoRoutine. \brief Gets the resulting value of the HttpCoRoutine.
        /// \returns ReturnType The return value of the HttpCoRoutine.
        ReturnType get() {
            if (this != nullptr && this->coroutineHandle) {
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                std::cout << "WERE NOT THIS IT IS NOT !" << std::endl;
                this->coroutineHandle.promise().theFuture.wait();
                return this->coroutineHandle.promise().theFuture.get();
            }
            return ReturnType{};
        }

        /// Cancels the HttpCoRoutine, and returns the currently held value of the result. \brief Cancels the HttpCoRoutine, and returns the currently held value of the result.
        /// \returns ReturnType The object to be returned.
        ReturnType cancel() {
            if (this != nullptr && this->coroutineHandle) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                    this->coroutineHandle.promise().newThread.join();
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

        std::coroutine_handle<HttpCoRoutine<ReturnType>::promise_type> coroutineHandle{ nullptr };
        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// A HttpCoRoutine - representing a potentially asynchronous operation/function (The void specialization). \brief A HttpCoRoutine - representing a potentially asynchronous operation/function (The void specialization).
    /// \param void The type of parameter that is returned by the HttpCoRoutine.
    template<>
    class HttpCoRoutine<void> {
    public:

        class promise_type {
        public:

            template<typename void02>
            friend auto NewThreadAwaitable();
            template<typename void02>
            friend class HttpCoRoutine;


            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds(timeToWaitForInMs));
            }

            void return_void() {
                return;
            }

            auto get_return_object() {
                return HttpCoRoutine<void>{ std::coroutine_handle<HttpCoRoutine<void>::promise_type>::from_promise(*this) };
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

            std::future<void> theFuture{};

        protected:

            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            std::condition_variable condVar{};
            std::mutex lockMutex{};
        };

        HttpCoRoutine<void>& operator=(HttpCoRoutine<void>&& other) noexcept {
            if (this != &other) {
                this->coroutineHandle = other.coroutineHandle.from_address(other.coroutineHandle.address());
                other.coroutineHandle = std::coroutine_handle<HttpCoRoutine<void>::promise_type>();
                this->currentStatus = other.currentStatus;
                other.currentStatus = CoRoutineStatus::Cancelled;
            }
            return *this;
        };

        HttpCoRoutine(HttpCoRoutine<void>&& other) noexcept {
            *this = std::move(other);
        };

        HttpCoRoutine<void>& operator=(const HttpCoRoutine<void>& other) = delete;

        HttpCoRoutine(const HttpCoRoutine<void>& other) = delete;

        HttpCoRoutine<void>& operator=(HttpCoRoutine<void>& other) = delete;

        HttpCoRoutine(HttpCoRoutine<void>& other) = delete;

        HttpCoRoutine() = default;

        HttpCoRoutine(std::coroutine_handle<HttpCoRoutine<void>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~HttpCoRoutine() {
            if (this->coroutineHandle) {
                if (this->coroutineHandle.done()) {
                    this->coroutineHandle.destroy();
                }
            }
        }

        /// Collects the status of the HttpCoRoutine. \brief Collects the status of the HttpCoRoutine.
        /// \returns CoRoutineStatus The status of the HttpCoRoutine.
        CoRoutineStatus getStatus() {
            if (this != nullptr && this->coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
                return this->currentStatus;
            }
            return CoRoutineStatus();
        }

        /// Gets the resulting value of the HttpCoRoutine. \brief Gets the resulting value of the HttpCoRoutine.
        void get() {
            if (this != nullptr && this->coroutineHandle) {
                std::exception_ptr exceptionPtr{};
                while (this->coroutineHandle.promise().exceptionBuffer.tryReceive(exceptionPtr)) {
                    std::rethrow_exception(exceptionPtr);
                }
                this->coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
                this->currentStatus = this->coroutineHandle.promise().currentStatus;
                std::cout << "WERE NOT THIS IT IS NOT 020202!" << std::endl;
                this->coroutineHandle.promise().theFuture.wait();
                this->coroutineHandle.promise().theFuture.get();
                return;
            }
            return;
        }

        /// Cancels the HttpCoRoutine, and returns the currently held value of the result. \brief Cancels the HttpCoRoutine, and returns the currently held value of the result.
        void cancel() {
            if (this != nullptr && this->coroutineHandle) {
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
        std::coroutine_handle<HttpCoRoutine<void>::promise_type> coroutineHandle{ nullptr };
        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// For launching the HttpCoRoutine onto a new CPU thread, as well as returning the HttpCoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the HttpCoRoutine onto a new CPU thread, as well as returning the HttpCoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \param ReturnType The type of the value returned by the containing HttpCoRoutine.
    /// \returns A std::coroutine_handle<HttpCoRoutine<ReturnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
    template<typename ReturnType>
    auto HttpNewThreadAwaitable(DiscordCoreInternal::HttpClient& httpClientNewer, DiscordCoreInternal::HttpWorkloadData& argTwo) {

        struct NewThreadAwaitableClass {

            DiscordCoreInternal::HttpWorkloadData workload{};
            DiscordCoreInternal::HttpClient& httpClient;

            NewThreadAwaitableClass(DiscordCoreInternal::HttpClient& httpClientNew, DiscordCoreInternal::HttpWorkloadData workloadNew) :httpClient(httpClientNew) {
                this->workload = workloadNew;
            }

            std::coroutine_handle<class HttpCoRoutine<ReturnType>::promise_type> coroHandle{ nullptr };

            bool await_ready() {
                return false;
            };

            void await_suspend(std::coroutine_handle<class HttpCoRoutine<ReturnType>::promise_type> handle) {
                this->coroHandle = handle;
                this->coroHandle.promise().theFuture = CoRoutineTP::threadPool.submit<ReturnType>(this->httpClient, this->workload);
            }

            auto await_resume() {
                return this->coroHandle;
            }
        };
        return NewThreadAwaitableClass{ httpClientNewer, argTwo };
    }

    template<>
    auto HttpNewThreadAwaitable<void>(DiscordCoreInternal::HttpClient& httpClientNewer, DiscordCoreInternal::HttpWorkloadData& argTwo) {

        struct NewThreadAwaitableClass {

            DiscordCoreInternal::HttpWorkloadData workload{};
            DiscordCoreInternal::HttpClient& httpClient;

            NewThreadAwaitableClass(DiscordCoreInternal::HttpClient& httpClientNew, DiscordCoreInternal::HttpWorkloadData workloadNew) :httpClient(httpClientNew) {
                this->workload = workloadNew;
            }

            std::coroutine_handle<class HttpCoRoutine<void>::promise_type> coroHandle{ nullptr };

            bool await_ready() {
                return false;
            };

            void await_suspend(std::coroutine_handle<class HttpCoRoutine<void>::promise_type> handle) {
                this->coroHandle = handle;
                this->coroHandle.promise().theFuture = CoRoutineTP::threadPool.submit(this->httpClient, this->workload);
            }

            auto await_resume() {
                return this->coroHandle;
            }
        };
        return NewThreadAwaitableClass{ httpClientNewer, argTwo };
    }

     /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
     /// \param ReturnType The type of parameter that is returned by the CoRoutine.
    template<typename ReturnType>
    class CoRoutine {
    public:

        class DiscordCoreAPI_Dll promise_type {
        public:

            template<typename ReturnType02>
            friend auto NewThreadAwaitable();
            template<typename ReturnType02>
            friend class CoRoutine;

            void requestStop() {
                this->newThread.get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread.get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds{ timeToWaitForInMs });
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

        protected:

            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            std::condition_variable condVar{};
            std::jthread newThread{};
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

        CoRoutine() = default;

        CoRoutine(std::coroutine_handle<CoRoutine<ReturnType>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle) {
                this->coroutineHandle.promise().requestStop();
                if (this->coroutineHandle.done()) {
                    this->coroutineHandle.destroy();
                }
            }
        }

         /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        void detachThread() {
            if (this != nullptr && this->coroutineHandle) {
                this->coroutineHandle.promise().newThread.detach();
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
            if (this != nullptr) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.join();
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
            if (this != nullptr && this->coroutineHandle) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                    this->coroutineHandle.promise().newThread.join();
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
    template<>
    class CoRoutine<void> {
    public:

        class DiscordCoreAPI_Dll promise_type {
        public:

            template<typename void02>
            friend auto NewThreadAwaitable();
            template<typename void02>
            friend class CoRoutine;

            void requestStop() {
                this->newThread.get_stop_source().request_stop();
            }

            bool isItStopped() {
                return this->newThread.get_stop_token().stop_requested();
            }

            void waitForTime(uint64_t timeToWaitForInMs) {
                std::unique_lock<std::mutex> timedLock{ this->lockMutex };
                this->condVar.wait_for(timedLock, std::chrono::milliseconds{ timeToWaitForInMs });
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

        protected:
            
            UnboundedMessageBlock<std::exception_ptr> exceptionBuffer{};
            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
            std::condition_variable condVar{};
            std::jthread newThread{};
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

        CoRoutine() = default;

        CoRoutine(std::coroutine_handle<CoRoutine<void>::promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        ~CoRoutine() {
            if (this->coroutineHandle) {
                this->coroutineHandle.promise().requestStop();
                if (this->coroutineHandle.done()) {
                    this->coroutineHandle.destroy();
                }
            }
        }

         /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        void detachThread() {
            if (this != nullptr && this->coroutineHandle) {
                this->coroutineHandle.promise().newThread.detach();
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
        void get() {
            if (this != nullptr) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().newThread.join();
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
        void cancel() {
            if (this != nullptr && this->coroutineHandle) {
                if (this->coroutineHandle.promise().newThread.joinable()) {
                    this->coroutineHandle.promise().requestStop();
                    {
                        std::lock_guard<std::mutex> accessLock{ this->coroutineHandle.promise().lockMutex };
                        this->coroutineHandle.promise().condVar.notify_all();
                    }
                    this->coroutineHandle.promise().newThread.join();
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

        struct DiscordCoreAPI_Dll NewThreadAwaitableClass {

            std::coroutine_handle<class CoRoutine<ReturnType>::promise_type> coroHandle{ nullptr };

            bool await_ready() {
                return false;
            };

            void await_suspend(std::coroutine_handle<class CoRoutine<ReturnType>::promise_type> handle) {
                this->coroHandle = handle;
                this->coroHandle.promise().newThread = std::jthread([=, this] { this->coroHandle.resume(); });
            }

            auto await_resume() {
                return this->coroHandle;
            }
        };
        return NewThreadAwaitableClass{};
    }

    /**@}*/
    };
