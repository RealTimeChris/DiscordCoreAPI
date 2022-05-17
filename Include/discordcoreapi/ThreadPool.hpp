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

		void stopThread( const std::string&theKey);

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

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		CoRoutineThreadPool() {
			for (uint32_t x = 0; x < std::thread::hardware_concurrency(); ++x) {
				std::unique_ptr<std::jthread> workerThread = std::make_unique<std::jthread>([this]() {
					auto thePtr = std::make_unique<HttpConnection>();
					Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
					this->threadFunction();
				});
				theThreads.push_back(std::move(workerThread));
			}
		}

		void submitTask(std::coroutine_handle<> coro) noexcept {
			std::unique_lock<std::mutex> theLock(this->theMutex01);
			bool areWeAllBusy{ true };
			for (auto& [key, value]: this->theWorkingStatuses) {
				if (!value.theCurrentStatus.load()) {
					areWeAllBusy = false;
				}
			}
			if (areWeAllBusy) {
				std::unique_ptr<std::jthread> workerThread = std::make_unique<std::jthread>([this]() {
					auto thePtr = std::make_unique<HttpConnection>();
					Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
					this->threadFunction();
				});
				theThreads.push_back(std::move(workerThread));
			}
			this->theCoroutineHandles.emplace(coro);
			this->theCondVar.notify_one();
		}

		~CoRoutineThreadPool() {
			this->areWeQuitting.store(true);
			for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
				std::jthread& thread = *this->theThreads[x];
				if (thread.joinable()) {
					thread.join();
				}
				this->theThreads.erase(this->theThreads.begin() + x);
			}
		}

	  private:
		std::unordered_map<std::thread::id, WorkloadStatus> theWorkingStatuses{};
		std::queue<std::coroutine_handle<>> theCoroutineHandles{};
		std::atomic_bool areWeQuitting{ false };
		std::vector<std::unique_ptr<std::jthread>> theThreads{};
		std::condition_variable theCondVar{};
		std::mutex theMutex01{};
		std::mutex theMutex02{};

		void threadFunction() {
			std::unique_lock<std::mutex> theLock00{ this->theMutex02 };
			WorkloadStatus theStatus{ std::this_thread::get_id() };
			this->theWorkingStatuses.insert_or_assign(std::this_thread::get_id(), theStatus);
			auto theAtomicBoolPtr = &this->theWorkingStatuses[std::this_thread::get_id()].theCurrentStatus;
			theLock00.unlock();
			while (!this->areWeQuitting.load()) {
				std::unique_lock<std::mutex> theLock01{ this->theMutex01 };
				while (!this->areWeQuitting.load() && this->theCoroutineHandles.size() == 0) {
					if (this->theThreads.size() > std::thread::hardware_concurrency()) {
						for (auto& [key, value]: this->theWorkingStatuses) {
							bool doWeBreak{ false };
							if (value.theCurrentStatus.load()) {
								for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
									if (this->theThreads[x]->get_id() == key) {
										this->theThreads[x]->detach();
										this->theThreads.erase(this->theThreads.begin() + x);
										doWeBreak = true;
										value.doWeQuit.store(true);
										break;
									}
								}
								if (doWeBreak) {
									break;
								}
							}
						}
					}
					this->theCondVar.wait_for(theLock01, std::chrono::microseconds(1000));
				}
				if (this->areWeQuitting.load()) {
					break;
				}
				auto& coroHandle = this->theCoroutineHandles.front();
				this->theCoroutineHandles.pop();
				theLock01.unlock();
				if (theAtomicBoolPtr) {
					theAtomicBoolPtr->store(true);
				}
				coroHandle.resume();
				if (theAtomicBoolPtr) {
					theAtomicBoolPtr->store(false);
				}
				if (this->theWorkingStatuses[std::this_thread::get_id()].doWeQuit.load()) {
					break;
				}
			}
			std::unique_lock<std::mutex> theLock02{ this->theMutex02 };
			this->theWorkingStatuses.erase(std::this_thread::get_id());
			theLock02.unlock();
		}
	};
	/**@}*/
}// namespace DiscordCoreAPI