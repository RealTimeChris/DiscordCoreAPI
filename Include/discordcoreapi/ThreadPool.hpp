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

namespace DiscordCoreAPI {
	/**
	 * \addtogroup utilities
	 * @{
	 */

	using TimeElapsedHandler = std::function<void(void)>;

	class DiscordCoreAPI_Dll ThreadPool {
	  public:
		ThreadPool& operator=(const ThreadPool&) = delete;

		ThreadPool(const ThreadPool&) = delete;

		ThreadPool& operator=(ThreadPool&) = delete;

		ThreadPool(ThreadPool&) = delete;

		ThreadPool() = default;

		std::string storeThread(TimeElapsedHandler timeElapsedHandler, int32_t timeInterval);

		void stopThread(std::string theKey);

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

	struct DiscordCoreAPI_Dll WorkloadStatus {
		WorkloadStatus& operator=(WorkloadStatus& other) {
			this->theCurrentStatus.store(other.doWeQuit.load(std::memory_order::seq_cst), std::memory_order::seq_cst);
			this->doWeQuit.store(other.doWeQuit.load(std::memory_order::seq_cst), std::memory_order::seq_cst);
			return *this;
		}

		WorkloadStatus(WorkloadStatus& other) {
			*this = other;
		}

		WorkloadStatus() = default;

		std::atomic_bool theCurrentStatus{ false };
		std::atomic_bool doWeQuit{ false };
	};

	class DiscordCoreAPI_Dll CoRoutineThreadPool {
	  public:
		CoRoutineThreadPool() {
			for (uint32_t x = 0; x < std::jthread::hardware_concurrency(); ++x) {
				std::jthread workerThread([this]() {
					this->threadFunction();
				});
				this->theThreads.push_back(std::move(workerThread));
			}
		}

		void submitTask(std::coroutine_handle<> coro) noexcept {
			std::unique_lock<std::mutex> theLock(this->theMutex);
			bool areWeAllBusy{ true };
			for (auto& [key, value]: this->theWorkingStatuses) {
				if (!value.theCurrentStatus.load(std::memory_order::seq_cst)) {
					areWeAllBusy = false;
				}
			}
			if (areWeAllBusy) {
				std::jthread workerThread([this]() {
					this->threadFunction();
				});
				theThreads.push_back(std::move(workerThread));
			}
			this->coroutineHandles.emplace(coro);
			this->condVar.notify_one();
		}

		~CoRoutineThreadPool() {
			this->areWeQuitting.store(true, std::memory_order::seq_cst);
			for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
				std::jthread& thread = this->theThreads[x];
				if (thread.joinable()) {
					thread.join();
				}
				this->theThreads.erase(this->theThreads.begin() + x);
			}
		}

	  private:
		std::unordered_map<std::thread::id, WorkloadStatus> theWorkingStatuses{};
		std::queue<std::coroutine_handle<>> coroutineHandles{};
		std::atomic_bool areWeQuitting{ false };
		std::vector<std::jthread> theThreads{};
		std::condition_variable condVar{};
		std::mutex theMutex{};

		void threadFunction() {
			std::unique_lock<std::mutex> theLock{ this->theMutex };
			WorkloadStatus theStatus{};
			this->theWorkingStatuses.insert_or_assign(std::this_thread::get_id(), theStatus);
			auto theAtomicBoolPtr = &this->theWorkingStatuses[std::this_thread::get_id()].theCurrentStatus;
			theLock.unlock();
			while (!this->areWeQuitting.load(std::memory_order::seq_cst)) {
				std::unique_lock<std::mutex> theLock02{ this->theMutex };

				while (!this->areWeQuitting.load(std::memory_order::seq_cst) && this->coroutineHandles.size() == 0) {
					if (this->theThreads.size() > std::thread::hardware_concurrency()) {
						for (auto& [key, value]: this->theWorkingStatuses) {
							bool doWeBreak{ false };
							if (value.theCurrentStatus.load(std::memory_order::seq_cst) && key != std::this_thread::get_id()) {
								for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
									if (this->theThreads[x].get_id() == key) {
										this->theThreads[x].detach();
										this->theThreads.erase(this->theThreads.begin() + x);
										doWeBreak = true;
										break;
									}
								}
								value.doWeQuit.store(true, std::memory_order::seq_cst);
								if (doWeBreak) {
									break;
								}
							}
						}
					}
					this->condVar.wait_for(theLock02, std::chrono::microseconds(1000));
				}
				if (this->areWeQuitting.load(std::memory_order::seq_cst)) {
					break;
				}
				auto& coroHandle = this->coroutineHandles.front();
				this->coroutineHandles.pop();
				theLock02.unlock();
				if (theAtomicBoolPtr) {
					theAtomicBoolPtr->store(true, std::memory_order::seq_cst);
				}
				coroHandle.resume();
				if (theAtomicBoolPtr) {
					theAtomicBoolPtr->store(false, std::memory_order::seq_cst);
				}
				if (this->theWorkingStatuses[std::this_thread::get_id()].doWeQuit.load(std::memory_order::seq_cst)) {
					this->theWorkingStatuses.erase(std::this_thread::get_id());
					break;
				}
			}
		}
	};

	/**@}*/
}// namespace DiscordCoreAPI