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
		WorkloadStatus& operator=(WorkloadStatus& other) {
			this->theCurrentStatus.store(other.doWeQuit.load());
			this->doWeQuit.store(other.doWeQuit.load());
			return *this;
		}

		WorkloadStatus(WorkloadStatus& other) {
			*this = other;
		}

		WorkloadStatus() = default;

		WorkloadStatus(std::thread::id threadIdNew) {
			this->threadId = threadIdNew;
		}

		~WorkloadStatus() {
			if (Globals::httpConnections[this->threadId] != nullptr) {
				Globals::httpConnections.erase(this->threadId);
			}
		}

		std::atomic_bool theCurrentStatus{ false };
		std::atomic_bool doWeQuit{ false };
		std::thread::id threadId{};
	};

	struct ThreadWorker {
		ThreadWorker& operator=(ThreadWorker&& other) {
			this->theCoros = std::move(other.theCoros);
			this->theThread.swap(other.theThread);
			this->theMutex.swap(other.theMutex);
			return *this;
		}
		ThreadWorker(ThreadWorker&& other) {
			*this = std::move(other);
		}
		ThreadWorker() = default;
		std::unique_ptr<std::recursive_mutex> theMutex{ std::make_unique<std::recursive_mutex>() };
		std::queue<std::coroutine_handle<>> theCoros{};
		std::unique_ptr<std::jthread> theThread{};
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		CoRoutineThreadPool() {
			this->currentThreadCount = std::thread::hardware_concurrency();
			for (uint32_t x = 0; x < this->currentThreadCount; ++x) {
				ThreadWorker theThread{};
				theThread.theThread= std::make_unique<std::jthread>([=,this]() {
					auto thePtr = std::make_unique<HttpConnection>();
					Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
					this->threadFunction(static_cast<int8_t>(x));
				});
				theThreads.push_back(std::move(theThread));
			}
		}

		void submitTask(std::coroutine_handle<> coro) noexcept {
			std::unique_lock<std::recursive_mutex> theLock{ this->theMutex };
			std::unique_lock<std::recursive_mutex> theLock1{};
			int32_t currentIndexReal = this->currentIndex % this->currentThreadCount;
			ThreadWorker* theWorker = &this->theThreads[currentIndexReal];
			std::unique_lock<std::recursive_mutex> theLock2{ *theWorker->theMutex, std::defer_lock };
			if (theLock2.try_lock()) {
				theLock2.swap(theLock1);
				theWorker->theCoros.emplace(coro);
				this->currentIndex += 1;
				return;
			}
			this->currentIndex += 1;
			
			if (this->currentIndex % this->currentThreadCount == 0) {
				int32_t currentThreadCountNew = this->currentThreadCount;
				ThreadWorker theThread{};
				theThread.theThread = std::make_unique<std::jthread>([&]() mutable -> void {
					auto thePtr = std::make_unique<HttpConnection>();
					Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
					this->threadFunction(static_cast<int8_t>(currentThreadCountNew));
				});
				this->currentThreadCount += 1;
				theThreads.push_back(std::move(theThread));
			}
			submitTask(coro);
		}

		~CoRoutineThreadPool() {
			this->areWeQuitting.store(true);
			for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
				ThreadWorker* thread = &this->theThreads.front();
				if (thread->theThread->joinable()) {
					thread->theThread->join();
				}
				this->theThreads.erase(this->theThreads.begin() + x);
			}
		}

	  private:
		std::unordered_map<std::thread::id, WorkloadStatus> theWorkingStatuses{};
		std::atomic_bool areWeQuitting{ false };
		std::vector<ThreadWorker> theThreads{};
		std::recursive_mutex theMutex{};
		int64_t currentThreadCount{};
		int64_t currentIndex{};
		

		void threadFunction(int8_t theIndex) {
			ThreadWorker* theWorker = &this->theThreads[theIndex];
			while (!this->areWeQuitting.load()) {
				
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
				if (this->areWeQuitting.load()) { 
					break;
				}
				
				std::unique_lock<std::recursive_mutex> theLock{ *theWorker->theMutex };
				if (theWorker->theCoros.size() > 0) {
					std::cout << "WERE HERE THIS IS IT!, THE INDEX: " << std::to_string(theIndex) << std::endl;
					auto& coroHandle = theWorker->theCoros.front();
					theWorker->theCoros.pop();
					theLock.unlock();
					coroHandle.resume();
				}
			}
		}
	};
	/**@}*/
}// namespace DiscordCoreAPI