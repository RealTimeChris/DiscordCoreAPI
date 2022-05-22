/*
*
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
/// Chris M.
/// https://discordcoreapi.com
/// \file ThreadPool.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Http.hpp>

namespace DiscordCoreAPI {
	/**
	 * \addtogroup utilities
	 * @{
	 */
	extern std::unordered_map<std::thread::id, std::unique_ptr<DiscordCoreInternal::HttpConnection>> httpConnections;

	using TimeElapsedHandler = std::function<void(void)>;

	class DiscordCoreAPI_Dll ThreadPool {
	  public:
		ThreadPool& operator=(const ThreadPool&) = delete;

		ThreadPool(const ThreadPool&) = delete;

		ThreadPool& operator=(ThreadPool&) = delete;

		ThreadPool(ThreadPool&) = delete;

		ThreadPool() = default;

		~ThreadPool();

		std::string storeThread(TimeElapsedHandler timeElapsedHandler, int32_t timeInterval);

		void stopThread(const std::string& theKey);

		/// For executing a chosen function of type std::function<void(ArgTypes...)> after a chosen time delay. \brief For executing a chosen function of type std::function<void(ArgTypes...)> after a chosen time delay.
		/// \tparam ArgTypes The types of arguments that will be passed into the function.
		/// \param theFunction A pointer to the chosen function to be executed.
		/// \param timeDelayInMs The number of milliseconds to wait before executing the function.
		/// \param waitForResult Do we wait for the thread to complete/block the current thread of execution?
		/// \param args The set of arguments to be passed into the executing function.
		template<typename... ArgTypes>
		static void executeFunctionAfterTimePeriod(std::function<void(ArgTypes...)> theFunction, uint32_t timeDelayInMs, bool waitForResult, ArgTypes... args) {
			std::jthread newThread{ [=](std::stop_token theToken) {
				if (timeDelayInMs > 0) {
					int64_t timePassed{ 0 };
					int64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				
					while (timePassed < timeDelayInMs) {
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
						int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						timePassed = currentTime - startTime;
						if (theToken.stop_requested()) {
							return;
						}
					}
				}
				theFunction(args...);
			} };
			if (waitForResult) {
				newThread.join();
			} else {
				newThread.detach();
			}
		};

	  protected:
		std::unordered_map<std::string, std::jthread> threads{};
		std::mutex theMutex{};
	};
}

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll CoRoutineThreadPool;

	struct DiscordCoreAPI_Dll WorkerThread{
		WorkerThread& operator=(WorkerThread&& other) noexcept;
		
		WorkerThread(WorkerThread&& other) noexcept;

		WorkerThread(CoRoutineThreadPool* thePtrNew);

		WorkerThread() = default;

		~WorkerThread();

		std::unique_ptr<std::thread::id> threadId{ std::make_unique<std::thread::id>() };
		std::atomic_bool theCurrentStatus{ false };
		CoRoutineThreadPool* thePtr{ nullptr };
		std::jthread theThread{};
		int64_t theIndex{ 0 };
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		friend WorkerThread;

		CoRoutineThreadPool();

		void submitTask(std::coroutine_handle<> coro) noexcept;

		~CoRoutineThreadPool();

	  private:
		std::unordered_map<int64_t, WorkerThread> workerThreads{};
		std::queue<std::coroutine_handle<>> theCoroutineHandles{};
		std::atomic_bool areWeQuitting{ false };
		std::condition_variable theCondVar{};
		int64_t currentIndex{ 0 };
		std::mutex theMutex01{};
		std::mutex theMutex02{};

		void threadFunction(std::stop_token theToken, int64_t theIndex);
	};
	/**@}*/
}// namespace DiscordCoreAPI