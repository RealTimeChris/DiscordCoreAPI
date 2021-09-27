// ThreadManager.hpp - Header for the "system threads" class.
// May 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _THREAD_MANAGER_
#define _THREAD_MANAGER_

#include "../pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

    enum class ThreadType {
        Regular = 0,
        Music = 1
    };

    task<shared_ptr<ThreadContext>> createThreadContext(ThreadType threadType);

    class ThreadManagerAgent : agent {
    protected:

        friend class ThreadManager;

        static shared_ptr<ThreadContext> threadContext;

        unbounded_buffer<shared_ptr<ThreadContext>> outputBuffer{ nullptr };
        unbounded_buffer<ThreadType> readyBuffer{ nullptr };

        ThreadManagerAgent() :agent(*ThreadManagerAgent::threadContext->scheduler->scheduler) {}

        static void initialize(shared_ptr<ThreadContext> threadContextNew) {
            ThreadManagerAgent::threadContext = threadContextNew;
        }

        static void cleanup() {
            ThreadManagerAgent::threadContext->releaseContext();
        }

        void run() {
            try {
                ThreadType threadType = receive(ThreadManagerAgent::readyBuffer);
                auto threadContextNew = createThreadContext(threadType).get();
                send(ThreadManagerAgent::outputBuffer, threadContextNew);
                this->done();
            }
            catch (...) {
                DiscordCoreAPI::rethrowException("ThreadManagerAgent::run() Error: ");
                this->done();
            }
            this->done();
        }

    };

    class ThreadManager {
    public:

        static void intialize() {
            shared_ptr<ThreadContext> threadContext = createThreadContext(ThreadType::Regular).get();
            ThreadManagerAgent::initialize(threadContext);
        }

        static task<shared_ptr<ThreadContext>> getThreadContext(ThreadType threadType = ThreadType::Regular) {
            ThreadManagerAgent requestAgent{};
            send(requestAgent.readyBuffer, threadType);
            requestAgent.start();
            agent::wait(&requestAgent);
            auto threadContext = receive(requestAgent.outputBuffer);
            co_return threadContext;
        }

        ~ThreadManager() {
            ThreadManagerAgent::cleanup();
        };

    };

    task<shared_ptr<ThreadContext>> createThreadContext(ThreadType threadType) {

        DispatcherQueueOptions options{
            .dwSize = sizeof(DispatcherQueueOptions),
            .threadType = DQTYPE_THREAD_DEDICATED,
            .apartmentType = DQTAT_COM_ASTA, };

        ABI::Windows::System::IDispatcherQueueController* ptrNew2{ nullptr };
        check_hresult(CreateDispatcherQueueController(options, &ptrNew2));
        DispatcherQueueController queueController2{ ptrNew2, take_ownership_from_abi };
        DispatcherQueue threadQueue{ queueController2.DispatcherQueue() };
        co_await resume_foreground(threadQueue);
        SchedulerPolicy policy{};
        policy.SetConcurrencyLimits(1, 1);
        if (threadType == ThreadType::Music) {
            policy.SetPolicyValue(PolicyElementKey::ContextPriority, THREAD_PRIORITY_HIGHEST);
        }
        else {
            policy.SetPolicyValue(PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
        }
        policy.SetPolicyValue(PolicyElementKey::ContextStackSize, 0);
        policy.SetPolicyValue(PolicyElementKey::DynamicProgressFeedback, ProgressFeedbackEnabled);
        policy.SetPolicyValue(PolicyElementKey::LocalContextCacheSize, 0);
        policy.SetPolicyValue(PolicyElementKey::SchedulerKind, ThreadScheduler);
        policy.SetPolicyValue(PolicyElementKey::SchedulingProtocol, EnhanceScheduleGroupLocality);
        policy.SetPolicyValue(PolicyElementKey::TargetOversubscriptionFactor, 1);
        policy.SetPolicyValue(PolicyElementKey::WinRTInitialization, InitializeWinRTAsMTA);
        Scheduler* newScheduler{ Scheduler::Create(policy) };
        newScheduler->Attach();
        shared_ptr<ThreadContext> threadContext = make_shared<ThreadContext>();
        threadContext->queueController = make_shared<DispatcherQueueController>(queueController2);
        threadContext->scheduler = make_shared<SchedulerWrapper>(newScheduler);
        threadContext->dispatcherQueue = make_shared<DispatcherQueue>(threadQueue.GetForCurrentThread());
        threadContext->scheduleGroup = make_shared<ScheduleGroupWrapper>(threadContext->scheduler->scheduler->CreateScheduleGroup());
        co_return threadContext;
    }

    task<void> ThreadContext::releaseContext() {
        if (this->scheduleGroup != nullptr) {
            this->scheduleGroup->scheduleGroup->Release();
            this->scheduleGroup = nullptr;
        }
        if (this->scheduler != nullptr) {
            this->scheduler->scheduler->Release();
            this->scheduler = nullptr;
        }
        if (this->dispatcherQueue != nullptr) {
            co_await this->queueController->ShutdownQueueAsync();
            this->dispatcherQueue = nullptr;
        }        
        this->~ThreadContext();
    };

    shared_ptr<ThreadContext> ThreadManagerAgent::threadContext{ nullptr };
}
#endif
