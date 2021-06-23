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

    class ThreadManager {
    public:

        static concurrent_vector<ThreadContext>* threads;

        static task<ThreadContext> getThreadContext() {
            DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_DEDICATED,
                DQTAT_COM_ASTA
            };
            DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
            ABI::Windows::System::IDispatcherQueueController* ptrNew2{ nullptr };
            check_hresult(CreateDispatcherQueueController(options, &ptrNew2));
            DispatcherQueueController queueController2 = { ptrNew2, take_ownership_from_abi };
            DispatcherQueue threadQueue = queueController2.DispatcherQueue();
            co_await resume_foreground(threadQueue);
            SchedulerPolicy policy;
            policy.SetConcurrencyLimits(12, 12);
            policy.SetPolicyValue(PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
            policy.SetPolicyValue(PolicyElementKey::DynamicProgressFeedback, ProgressFeedbackEnabled);
            policy.SetPolicyValue(PolicyElementKey::LocalContextCacheSize, 0);
            policy.SetPolicyValue(PolicyElementKey::SchedulerKind, ThreadScheduler);
            policy.SetPolicyValue(PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            policy.SetPolicyValue(PolicyElementKey::TargetOversubscriptionFactor, 1);
            policy.SetPolicyValue(PolicyElementKey::WinRTInitialization, InitializeWinRTAsMTA);
            Scheduler* newScheduler = Scheduler::Create(policy);
            newScheduler->Attach();
            ThreadContext threadContext;
            threadContext.scheduler = newScheduler;
            threadContext.dispatcherQueue = make_shared<DispatcherQueue>(threadQueue.GetForCurrentThread());
            ThreadManager::threads->push_back(threadContext);
            co_return threadContext;
        }

        task<ThreadContext> getThreadContext(bool enterYes) {
            DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_DEDICATED,
                DQTAT_COM_ASTA
            };
            DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
            ABI::Windows::System::IDispatcherQueueController* ptrNew2{ nullptr };
            check_hresult(CreateDispatcherQueueController(options, &ptrNew2));
            DispatcherQueueController queueController2 = { ptrNew2, take_ownership_from_abi };
            DispatcherQueue threadQueue = queueController2.DispatcherQueue();
            co_await resume_foreground(threadQueue);
            SchedulerPolicy policy;
            policy.SetConcurrencyLimits(12, 12);
            policy.SetPolicyValue(PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
            policy.SetPolicyValue(PolicyElementKey::DynamicProgressFeedback, ProgressFeedbackEnabled);
            policy.SetPolicyValue(PolicyElementKey::LocalContextCacheSize, 0);
            policy.SetPolicyValue(PolicyElementKey::SchedulerKind, ThreadScheduler);
            policy.SetPolicyValue(PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            policy.SetPolicyValue(PolicyElementKey::TargetOversubscriptionFactor, 1);
            policy.SetPolicyValue(PolicyElementKey::WinRTInitialization, InitializeWinRTAsMTA);
            Scheduler* newScheduler = Scheduler::Create(policy);
            newScheduler->Attach();
            ThreadContext threadContext;
            threadContext.scheduler = newScheduler;
            threadContext.dispatcherQueue = make_shared<DispatcherQueue>(threadQueue.GetForCurrentThread());
            ThreadManager::threads->push_back(threadContext);
            co_return threadContext;
        }

        static void initialize() {
            ThreadManager::threads = new concurrent_vector<ThreadContext>();
        };

        ~ThreadManager() {
            for (auto value : *ThreadManager::threads) {
                value.scheduler->Release();
            }
            delete ThreadManager::threads;
            ThreadManager::threads = nullptr;
        };

    };
    concurrent_vector<ThreadContext>* ThreadManager::threads;
}
#endif
