/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file ThreadPool.hpp

#pragma once

#ifndef THREAD_POOL
#define THREAD_POOL

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>
#include <coroutine>

namespace DiscordCoreAPI {
	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename... ArgTypes> using TimeElapsedHandler = std::function<void(ArgTypes...)>;

	using TimeElapsedHandlerNoArgs = std::function<void(void)>;

	constexpr Float thePercentage{ 10.0f / 100.0f };

	class DiscordCoreAPI_Dll ThreadPool {
	  public:
		ThreadPool& operator=(const ThreadPool&) = delete;

		ThreadPool(const ThreadPool&) = delete;

		ThreadPool& operator=(ThreadPool&) = delete;

		ThreadPool(ThreadPool&) = delete;

		ThreadPool() noexcept = default;

		static String storeThread(TimeElapsedHandlerNoArgs timeElapsedHandler, Int64 timeInterval);

		template<typename... ArgTypes>
		static void executeFunctionAfterTimePeriod(TimeElapsedHandler<ArgTypes...> timeElapsedHandler, Int64 timeDelay, Bool blockForCompletion, ArgTypes... args) {
			std::jthread theThread = std::jthread([=](std::stop_token stopToken) {
				StopWatch stopWatch{ std::chrono::milliseconds{ timeDelay } };
				stopWatch.resetTimer();
				std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<Int64>(std::ceil(static_cast<Float>(timeDelay) * thePercentage)) });
				while (!stopWatch.hasTimePassed() && !stopToken.stop_requested()) {
					std::this_thread::sleep_for(1ms);
				}
				if (stopToken.stop_requested()) {
					return;
				}
				timeElapsedHandler(args...);
				if (stopToken.stop_requested()) {
					return;
				}
			});
			if (blockForCompletion) {
				if (theThread.joinable()) {
					theThread.join();
				}
			} else {
				if (theThread.joinable()) {
					theThread.detach();
				}
			}
		}

		void stopThread(const String& theKey);

		~ThreadPool() noexcept = default;

	  protected:
		static std::unordered_map<String, std::jthread> threads;
	};
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll WorkerThread {
		WorkerThread& operator=(WorkerThread&& other) noexcept;

		WorkerThread(WorkerThread&&) noexcept;

		WorkerThread() noexcept = default;

		~WorkerThread() noexcept = default;

		AtomicBool areWeCurrentlyWorking{ false };
		std::jthread theThread{};
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;

		CoRoutineThreadPool();

		void submitTask(std::coroutine_handle<> coro) noexcept;

	  protected:
		std::deque<std::coroutine_handle<>> theCoroutineHandles{};
		std::unordered_map<Int64, WorkerThread> workerThreads{};
		AtomicInt64 coroHandleCount{ 0 };
		AtomicInt64 currentCount{ 0 };
		AtomicInt64 currentIndex{ 0 };
		AtomicUint32 threadCount{};
		std::mutex theMutex{};

		void threadFunction(std::stop_token stopToken, Int64 theIndex);
	};
	/**@}*/
}// namespace DiscordCoreAPI
#endif