/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// ThreadPool.hpp - Header for the "Thread-Pool" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file ThreadPool.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {
	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename... ArgTypes> using TimeElapsedHandler = std::function<void(ArgTypes...)>;

	using TimeElapsedHandlerNoArgs = std::function<void(void)>;

	constexpr float thePercentage{ 10.0f / 100.0f };

	class DiscordCoreAPI_Dll ThreadPool {
	  public:
		ThreadPool& operator=(const ThreadPool&) = delete;

		ThreadPool(const ThreadPool&) = delete;

		ThreadPool& operator=(ThreadPool&) = delete;

		ThreadPool(ThreadPool&) = delete;

		ThreadPool() = default;

		static std::string storeThread(TimeElapsedHandlerNoArgs timeElapsedHandler, int64_t timeInterval, bool repeated);
		
		template<typename... ArgTypes>
		static void executeFunctionAfterTimePeriod(TimeElapsedHandler<ArgTypes...> timeElapsedHandler, int64_t timeInterval, bool blockForCompletion, ArgTypes... args);

		void stopThread(const std::string& theKey);

		~ThreadPool();

	  protected:
		static std::unordered_map<std::string, std::jthread> threads;
	};
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll WorkerThread {
		WorkerThread& operator=(WorkerThread&& other) noexcept;

		WorkerThread(WorkerThread&&) noexcept;

		WorkerThread() = default;

		~WorkerThread() = default;

		std::atomic_bool theCurrentStatus{ false };
		std::jthread theThread{};
	};

	class DiscordCoreAPI_Dll CommandThreadPool {
	  public:
		CommandThreadPool();

		void submitTask(std::function<void(void)> theFunction) noexcept;

		~CommandThreadPool();

	  private:
		std::unordered_map<int64_t, WorkerThread> workerThreads{};
		std::queue<std::function<void(void)>> theFunctions{};
		std::atomic_bool areWeQuitting{ false };
		std::condition_variable theCondVar{};
		int64_t currentCount{ 0 };
		int64_t currentIndex{ 0 };
		std::mutex theMutex01{};

		void threadFunction(std::stop_token stopToken, int64_t theIndex);

		void clearContents();
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		CoRoutineThreadPool();

		void submitTask(std::coroutine_handle<> coro) noexcept;

		~CoRoutineThreadPool();

	  private:
		std::unordered_map<int64_t, WorkerThread> workerThreads{};
		std::queue<std::coroutine_handle<>> theCoroutineHandles{};
		std::atomic_bool areWeQuitting{ false };
		std::condition_variable theCondVar{};
		int64_t currentCount{ 0 };
		int64_t currentIndex{ 0 };
		std::mutex theMutex01{};

		void threadFunction(std::stop_token stopToken, int64_t theIndex);

		void clearContents();
	};
	/**@}*/
}// namespace DiscordCoreAPI