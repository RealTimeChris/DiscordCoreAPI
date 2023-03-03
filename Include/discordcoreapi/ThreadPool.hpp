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

	const double percentage{ 10.0f / 100.0f };

	class DiscordCoreAPI_Dll ThreadPool {
	  public:
		ThreadPool& operator=(const ThreadPool&) = delete;

		ThreadPool(const ThreadPool&) = delete;

		ThreadPool() noexcept = default;

		static std::string storeThread(TimeElapsedHandlerNoArgs timeElapsedHandler, int64_t timeInterval);

		template<typename... ArgTypes> static void executeFunctionAfterTimePeriod(TimeElapsedHandler<ArgTypes...> timeElapsedHandler,
			int64_t timeDelay, bool blockForCompletion, ArgTypes... args) {
			std::jthread thread = std::jthread([=](std::stop_token token) {
				StopWatch stopWatch{ Milliseconds{ timeDelay } };
				stopWatch.resetTimer();
				if (static_cast<int64_t>(std::ceil(static_cast<double>(timeDelay) * percentage)) <= timeDelay &&
					static_cast<int64_t>(std::ceil(static_cast<double>(timeDelay) * percentage)) > 0) {
					std::this_thread::sleep_for(Milliseconds{ static_cast<int64_t>(std::ceil(static_cast<double>(timeDelay) * percentage)) });
				}
				while (!stopWatch.hasTimePassed() && !token.stop_requested()) {
					std::this_thread::sleep_for(1ms);
				}
				if (token.stop_requested()) {
					return;
				}
				timeElapsedHandler(args...);
				if (token.stop_requested()) {
					return;
				}
			});
			if (blockForCompletion) {
				if (thread.joinable()) {
					thread.join();
				}
			} else {
				if (thread.joinable()) {
					thread.detach();
				}
			}
		}

		void stopThread(const std::string& key);

		~ThreadPool() noexcept = default;

	  protected:
		static std::unordered_map<std::string, std::jthread> threads;
	};
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll WorkerThread {
		WorkerThread& operator=(WorkerThread&& other) noexcept;

		WorkerThread() noexcept = default;

		~WorkerThread() noexcept = default;

		std::atomic_bool areWeCurrentlyWorking{};
		std::jthread thread{};
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		friend class DiscordCoreAPI::DiscordCoreClient;

		CoRoutineThreadPool();

		void submitTask(std::coroutine_handle<> coro) noexcept;

	  protected:
		std::unordered_map<int64_t, WorkerThread> workerThreads{};
		std::deque<std::coroutine_handle<>> coroutineHandles{};
		const std::atomic_int64_t threadCount{};
		std::atomic_int64_t coroHandleCount{};
		std::shared_mutex workerAccessMutex{};
		std::atomic_int64_t currentCount{};
		std::atomic_int64_t currentIndex{};
		std::mutex coroHandleAccessMutex{};

		void threadFunction(std::stop_token token, int64_t index);
	};
	/**@}*/
}// namespace DiscordCoreAPI
