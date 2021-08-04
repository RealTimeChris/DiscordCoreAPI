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

    task<shared_ptr<ThreadContext>> createThreadContext() {
        DispatcherQueueOptions options{
             sizeof(DispatcherQueueOptions),
             DQTYPE_THREAD_DEDICATED,
             DQTAT_COM_ASTA
        };

        ABI::Windows::System::IDispatcherQueueController* ptrNew2{ nullptr };
        check_hresult(CreateDispatcherQueueController(options, &ptrNew2));
        DispatcherQueueController queueController2 = { ptrNew2, take_ownership_from_abi };
        DispatcherQueue threadQueue = queueController2.DispatcherQueue();
        co_await resume_foreground(threadQueue);
        SchedulerPolicy policy;
        policy.SetConcurrencyLimits(1, 1);
        policy.SetPolicyValue(PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
        policy.SetPolicyValue(PolicyElementKey::ContextStackSize, 0);
        policy.SetPolicyValue(PolicyElementKey::DynamicProgressFeedback, ProgressFeedbackEnabled);
        policy.SetPolicyValue(PolicyElementKey::LocalContextCacheSize, 0);
        policy.SetPolicyValue(PolicyElementKey::SchedulerKind, ThreadScheduler);
        policy.SetPolicyValue(PolicyElementKey::SchedulingProtocol, EnhanceScheduleGroupLocality);
        policy.SetPolicyValue(PolicyElementKey::TargetOversubscriptionFactor, 1);
        policy.SetPolicyValue(PolicyElementKey::WinRTInitialization, InitializeWinRTAsMTA);
        Scheduler* newScheduler = Scheduler::Create(policy);
        newScheduler->Attach();
        shared_ptr<ThreadContext> threadContext = make_shared<ThreadContext>();
        threadContext->scheduler = newScheduler;
        threadContext->dispatcherQueue = make_shared<DispatcherQueue>(threadQueue.GetForCurrentThread());
        threadContext->schedulerGroup = threadContext->scheduler->CreateScheduleGroup();
        co_return threadContext;
    }

    class ThreadManagerAgent :public agent {
    protected:
        friend class ThreadManager;
        static shared_ptr<ThreadContext> threadContext;
        static unbounded_buffer<bool> readyBuffer;
        static unbounded_buffer<shared_ptr<ThreadContext>> outputBuffer;
        ThreadManagerAgent():agent(*ThreadManagerAgent::threadContext->scheduler){
        }

        static void initialize(shared_ptr<ThreadContext> threadContextNew) {
            ThreadManagerAgent::threadContext = threadContextNew;
        }

        static void cleanup() {
            ThreadManagerAgent::threadContext->releaseGroup();
        }

        void run() {
            if (receive(ThreadManagerAgent::readyBuffer)) {
                auto threadContextNew = createThreadContext().get();
                send(ThreadManagerAgent::outputBuffer, threadContextNew);
                done();
            }
        }

    };

    class ThreadManager {
    public:

        static concurrent_vector<shared_ptr<ThreadContext>>* threads;

        static task<void> intialize() {
            if (ThreadManager::threads == nullptr) {
                ThreadManager::threads = new concurrent_vector<shared_ptr<ThreadContext>>();
            }
            shared_ptr<ThreadContext> threadContext = createThreadContext().get();
            ThreadManager::threads->push_back(threadContext);
            ThreadManagerAgent::initialize(threadContext);
            co_return;
        }

        static task<shared_ptr<ThreadContext>> getThreadContext() {
            ThreadManagerAgent requestAgent;
            send(ThreadManagerAgent::readyBuffer, true);
            requestAgent.start();
            agent::wait(&requestAgent);
            auto threadContext = receive(ThreadManagerAgent::outputBuffer);
            ThreadManager::threads->push_back(threadContext);
            co_return threadContext;
        }

        ~ThreadManager() {
            for (auto value : *ThreadManager::threads) {
                value->scheduler->Release();
                value->schedulerGroup->Release();
            }
            delete ThreadManager::threads;
            ThreadManager::threads = nullptr;
            ThreadManagerAgent::cleanup();
        };

    };
    concurrent_vector<shared_ptr<ThreadContext>>* ThreadManager::threads{ nullptr };
    unbounded_buffer<bool> ThreadManagerAgent::readyBuffer;
    unbounded_buffer<shared_ptr<ThreadContext>> ThreadManagerAgent::outputBuffer;
    shared_ptr<ThreadContext> ThreadManagerAgent::threadContext{ nullptr };
}
#endif
