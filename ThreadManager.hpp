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

    class ThreadManagerAgent :public agent {
    protected:

        friend class ThreadManager;

        static shared_ptr<ThreadContext> threadContext;

        unbounded_buffer<shared_ptr<ThreadContext>> outputBuffer{ nullptr };
        unbounded_buffer<ThreadType> readyBuffer{ nullptr };
        unbounded_buffer<exception> errorBuffer{ nullptr };

        ThreadManagerAgent() :agent(*ThreadManagerAgent::threadContext->scheduler->scheduler) {}

        void getError() {
            exception error;
            while (try_receive(errorBuffer, error)) {
                cout << "ThreadManagerAgent::run() Error: " << error.what() << endl << endl;
            }
        }

        static void initialize(shared_ptr<ThreadContext> threadContextNew) {
            ThreadManagerAgent::threadContext = threadContextNew;
        }

        static void cleanup() {
            ThreadManagerAgent::threadContext->releaseGroup();
        }

        void run() {
            try {
                ThreadType threadType = receive(ThreadManagerAgent::readyBuffer);
                auto threadContextNew = createThreadContext(threadType).get();
                send(ThreadManagerAgent::outputBuffer, threadContextNew);
                this->done();
            }
            catch (...) {
                DiscordCoreAPI::rethrowException("ThreadManagerAgent::run() Error: ", &this->errorBuffer);
                this->done();
            }
            this->done();
        }

        ~ThreadManagerAgent() {
            this->getError();
        }

    };

    class ThreadManager {
    public:

        static concurrent_vector<shared_ptr<ThreadContext>> threads;

        static void intialize() {
            shared_ptr<ThreadContext> threadContext = createThreadContext(ThreadType::Regular).get();
            ThreadManager::threads.push_back(threadContext);
            ThreadManagerAgent::initialize(threadContext);
        }

        static task<shared_ptr<ThreadContext>> getThreadContext(ThreadType threadType = ThreadType::Regular) {
            ThreadManagerAgent requestAgent{};
            send(requestAgent.readyBuffer, threadType);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError();
            auto threadContext = receive(requestAgent.outputBuffer);
            ThreadManager::threads.push_back(threadContext);
            co_return threadContext;
        }

        ~ThreadManager() {
            for (auto value : ThreadManager::threads) {
                value->scheduler->scheduler->Release();
                value->scheduleGroup->scheduleGroup->Release();
            }
            ThreadManagerAgent::cleanup();
        };

    };

    task<shared_ptr<ThreadContext>> createThreadContext(ThreadType threadType) {
        for (auto value : ThreadManager::threads) {
            if (value->scheduleGroup == nullptr) {
                if (threadType == ThreadType::Music) {
                    if (value->scheduler->scheduler->GetPolicy().GetPolicyValue(PolicyElementKey::ContextPriority) == THREAD_PRIORITY_HIGHEST) {
                        value->scheduleGroup = make_shared<ScheduleGroupWrapper>(value->scheduler->scheduler->CreateScheduleGroup());
                        co_return value;
                    }
                }
                else {
                    value->scheduleGroup = make_shared<ScheduleGroupWrapper>(value->scheduler->scheduler->CreateScheduleGroup());
                    co_return value;
                }
            }
        }

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
        threadContext->scheduler = make_shared<ScheduleWrapper>(newScheduler);
        threadContext->dispatcherQueue = make_shared<DispatcherQueue>(threadQueue.GetForCurrentThread());
        threadContext->scheduleGroup = make_shared<ScheduleGroupWrapper>(threadContext->scheduler->scheduler->CreateScheduleGroup());
        co_return threadContext;
    }
    concurrent_vector<shared_ptr<ThreadContext>> ThreadManager::threads{};
    shared_ptr<ThreadContext> ThreadManagerAgent::threadContext{ nullptr };
}
#endif
