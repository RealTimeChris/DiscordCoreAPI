// CoRoutine.hpp - Header for the coroutine class.
// Oct 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "ApplicationCommandEntities.hpp"
#include "ChannelEntities.hpp"
#include "GuildEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "GuildScheduledEventEntities.hpp"
#include "InteractionEntities.hpp"
#include "MessageEntities.hpp"
#include "ReactionEntities.hpp"
#include "RoleEntities.hpp"
#include "StageInstanceEntities.hpp"
#include "StickerEntities.hpp"
#include "UserEntities.hpp"
#include "WebHookEntities.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup utilities
    * @{
    */
    /// An exception for when the CoRoutine is not in the correct state. \brief An exception for when the CoRoutine is not in the correct state.
    class DiscordCoreAPI_Dll InvalidState : public exception{
    public:
        explicit InvalidState(const string & message) : exception(message.c_str()) {}
    };

    /// A CoRoutine - representing a potentially asynchronous operation/function. \brief A CoRoutine - representing a potentially asynchronous operation/function.
    /// \param returnType The type of parameter that is returned by the CoRoutine.
    template<typename returnType>
    class DiscordCoreAPI_Dll CoRoutine {
    public:

        /** CoRoutineStatus
        *  The current status of the associated CoRoutine.
        */
        enum class CoRoutineStatus {
            Idle = 0,///< Idle.
            Running = 1,///< Running.
            Complete = 2,///< Complete.
            Cancelled = 3///< Cancelled.
        };

        class DiscordCoreAPI_Dll promise_type;

        CoRoutine<returnType>(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        CoRoutine<returnType>() {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        /// \returns void.
        void detachThread() {
            coroutineHandle.promise().newThread.detach();
            return;
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (coroutineHandle) {
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
            if (coroutineHandle.promise().newThread.joinable()) {
                coroutineHandle.promise().newThread.join();
            }
            exception_ptr exceptionPtr{};
            while (try_receive(coroutineHandle.promise().exceptionBuffer, exceptionPtr)) {
                rethrow_exception(exceptionPtr);
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
            return coroutineHandle.promise().result;
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns returnType The return value of the CoRoutine.
        returnType cancel() {
            if (this != nullptr) {
                if (coroutineHandle) {
                    if (coroutineHandle.promise().newThread.joinable()) {
                        coroutineHandle.promise().newThread.get_stop_source().request_stop();
                        coroutineHandle.promise().newThread.join();
                    }
                    exception_ptr exceptionPtr{};
                    while (try_receive(coroutineHandle.promise().exceptionBuffer, exceptionPtr)) {
                        rethrow_exception(exceptionPtr);
                    }
                    coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                }
                return coroutineHandle.promise().result;
            }
            return returnType{};
        }

        class DiscordCoreAPI_Dll promise_type {
        public:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

            unbounded_buffer<exception_ptr> exceptionBuffer{};

            jthread newThread{};

            returnType result{};

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
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                send(this->exceptionBuffer, current_exception());
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

        /** CoRoutineStatus
        *  The current status of the associated CoRoutine.
        */
        enum class CoRoutineStatus {
            Idle = 0,///< Idle.
            Running = 1,///< Running.
            Complete = 2,///< Complete.
            Cancelled = 3///< Cancelled.
        };

        CoRoutine(coroutine_handle<promise_type> coroutineHandleNew) : coroutineHandle(coroutineHandleNew) {};

        CoRoutine() {}

        ~CoRoutine() {
            if (coroutineHandle && coroutineHandle.done()) {
                coroutineHandle.destroy();
            }
        }

        /// Detaches the thread from the calling thread's context. \brief Detaches the thread from the calling thread's context.
        /// \returns void.
        void detachThread() {
            coroutineHandle.promise().newThread.detach();
            return;
        }

        /// Collects the status of the CoRoutine. \brief Collects the status of the CoRoutine.
        /// \returns CoRoutineStatus The status of the CoRoutine.
        CoRoutineStatus getStatus() {
            if (coroutineHandle) {
                this->currentStatus = coroutineHandle.promise().currentStatus;
            }
            return this->currentStatus;
        }

        /// Gets the resulting value of the CoRoutine. \brief Gets the resulting value of the CoRoutine.
        /// \returns void.
        void get(void) {
            if (!coroutineHandle) {
                throw InvalidState("CoRoutine is not initialized with a proper task.");
            }
            if (coroutineHandle.promise().newThread.joinable()) {
                coroutineHandle.promise().newThread.join();
            }
            exception_ptr exceptionPtr{};
            while (try_receive(coroutineHandle.promise().exceptionBuffer, exceptionPtr)) {
                rethrow_exception(exceptionPtr);
            }
            coroutineHandle.promise().currentStatus = CoRoutineStatus::Complete;
        }

        /// Cancels the CoRoutine, and returns the currently held value of the result. \brief Cancels the CoRoutine, and returns the currently held value of the result.
        /// \returns void.
        void cancel() {
            if (this != nullptr) {
                if (coroutineHandle) {
                    if (coroutineHandle.promise().newThread.joinable()) {
                        coroutineHandle.promise().newThread.get_stop_source().request_stop();
                        coroutineHandle.promise().newThread.join();
                    }
                    exception_ptr exceptionPtr{};
                    while (try_receive(coroutineHandle.promise().exceptionBuffer, exceptionPtr)) {
                        rethrow_exception(exceptionPtr);
                    }
                    coroutineHandle.promise().currentStatus = CoRoutineStatus::Cancelled;
                }
            }
            return;
        }

        class DiscordCoreAPI_Dll promise_type {
        public:

            CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };

            unbounded_buffer<exception_ptr> exceptionBuffer{};

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
                this->currentStatus = CoRoutineStatus::Complete;
                return{};
            }

            void unhandled_exception() {
                send(this->exceptionBuffer, current_exception());
            }
        };

    protected:

        coroutine_handle<promise_type> coroutineHandle{ nullptr };

        CoRoutineStatus currentStatus{ CoRoutineStatus::Idle };
    };

    /// For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation. \brief For launching the CoRoutine onto a new CPU thread, as well as returning the CoRoutine's handle to the inside of the function itself, for handling cancellation.
    /// \returns A orocutine_handle<CoRoutine<returnType>::promise_type> object, which contains the NewThread, which contains a Stop_Token and Stop_Source.
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