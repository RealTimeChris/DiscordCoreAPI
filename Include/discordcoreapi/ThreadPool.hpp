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
			std::jthread newThread{ [=]() {
				if (timeDelayInMs > 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ timeDelayInMs });
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
	};
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll WorkloadStatus {
		WorkloadStatus& operator=(WorkloadStatus& other);
		
		WorkloadStatus(WorkloadStatus& other);

		WorkloadStatus() = default;

		WorkloadStatus(std::thread::id threadIdNew);

		~WorkloadStatus();

		std::atomic_bool theCurrentStatus{ false };
		std::atomic_bool doWeQuit{ false };
		std::thread::id threadId{};
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		CoRoutineThreadPool();

		void submitTask(std::coroutine_handle<> coro) noexcept;

		~CoRoutineThreadPool();

	  private:
		std::unordered_map<std::thread::id, WorkloadStatus> theWorkingStatuses{};
		std::queue<std::coroutine_handle<>> theCoroutineHandles{};
		std::atomic_bool areWeQuitting{ false };
		std::vector<std::unique_ptr<std::jthread>> theThreads{};
		std::condition_variable theCondVar{};
		std::mutex theMutex01{};

		void threadFunction();
	};
	/**@}*/
}// namespace DiscordCoreAPI