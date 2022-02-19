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
/// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
/// Dec 18, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities.hpp>
#include <CoRoutine.hpp>

namespace DiscordCoreAPI {

    class DiscordCoreAPI_Dll ThreadPool {
    public:

        ThreadPool& operator=(const ThreadPool&) = delete;

        ThreadPool(const ThreadPool&) = delete;

        ThreadPool& operator=(ThreadPool&) = delete;

        ThreadPool(ThreadPool&) = delete;

        ThreadPool() = default;

        void storeThread(std::string theKey, CoRoutine<void> thread);

        CoRoutineStatus getThreadStatus(std::string theKey);

        void stopThread(std::string theKey);

        ~ThreadPool();

    protected:

        std::unordered_map<std::string, CoRoutine<void>> threads{};
        bool doWeQuit{ false };

    };

    using TimeElapsedHandler = std::function<void(void)>;

    class DiscordCoreAPI_Dll ThreadPoolTimer {
    public:

        ThreadPoolTimer& operator=(ThreadPoolTimer&&) noexcept;

        ThreadPoolTimer(ThreadPoolTimer&&) noexcept;

        ThreadPoolTimer& operator=(const ThreadPoolTimer&) = delete;

        ThreadPoolTimer(const ThreadPoolTimer&) = delete;

        ThreadPoolTimer& operator=(ThreadPoolTimer&) = delete;

        ThreadPoolTimer(ThreadPoolTimer&) = delete;

        ThreadPoolTimer(nullptr_t);

        static ThreadPoolTimer createPeriodicTimer(TimeElapsedHandler timeElapsedHandler, int64_t timeInterval);

        bool running();

        void cancel();

        template <typename ...ArgTypes>
        static CoRoutine<void> executeFunctionAfterTimePeriod(std::function<void(ArgTypes...)>theFunction, int32_t timeDelayInMs, ArgTypes... args) {
            co_await NewThreadAwaitable<void>();
            if (timeDelayInMs >= 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds{ timeDelayInMs });
                theFunction(args...);
            }
            else {
                throw std::runtime_error("Please enter a valid delay time!");
            }
        };

        ~ThreadPoolTimer();

    protected:

        static ThreadPool threads;

        std::string threadId{ "" };

        ThreadPoolTimer();

        CoRoutine<void> run(int64_t theInterval, TimeElapsedHandler theFunction, bool repeating);

    };
}
