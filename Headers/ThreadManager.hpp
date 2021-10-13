// ThreadManager.hpp - Header for the "system threads" class.
// May 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _THREAD_MANAGER_
#define _THREAD_MANAGER_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

    enum class ThreadType {
        Regular = 0,
        Music = 1
    };

    task<shared_ptr<ThreadContext>> createThreadContext(ThreadType threadType);

    class DiscordCoreAPI_Dll ThreadManagerAgent : agent {
    protected:

        friend class DiscordCoreAPI::DiscordCoreClient;
        friend class ThreadManager;

        static shared_ptr<ThreadContext> threadContext;

        unbounded_buffer<shared_ptr<ThreadContext>> outputBuffer{ nullptr };
        unbounded_buffer<ThreadType> readyBuffer{ nullptr };

        ThreadManagerAgent();

        static void initialize(shared_ptr<ThreadContext> threadContextNew);
        
        static void cleanup();

        void run();

    };

    class DiscordCoreAPI_Dll ThreadManager {
    public:

        static void intialize();

        static task<shared_ptr<ThreadContext>> getThreadContext(ThreadType threadType = ThreadType::Regular);

        ~ThreadManager();

    };
}
#endif
