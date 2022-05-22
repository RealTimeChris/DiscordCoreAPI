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
/// ThreadPool.cpp - Source file for the "Thread-Pool" related stuff.
/// Dec 18, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file ThreadPool.cpp

#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreAPI {

	constexpr float thePercentage{ 90.0f / 100.0f };

	std::string ThreadPool::storeThread(TimeElapsedHandler timeElapsedHandler, int32_t timeInterval) {
		std::string threadId = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

		this->threads.insert(std::make_pair(threadId, std::jthread([=](std::stop_token stopToken) {
			DiscordCoreAPI::StopWatch stopWatch{ std::chrono::milliseconds{ timeInterval } };
			while (true) {
				stopWatch.resetTimer();
				std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int32_t>(std::ceil(static_cast<float>(timeInterval) * thePercentage)) });
				while (!stopWatch.hasTimePassed() && !stopToken.stop_requested()) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
				}
				if (stopToken.stop_requested()) {
					return;
				}
				timeElapsedHandler();
				if (stopToken.stop_requested()) {
					return;
				}
			}
		})));
		return threadId;
	}

	void ThreadPool::stopThread(const std::string& theKey) {
		if (this->threads.contains(theKey)) {
			this->threads[theKey].request_stop();
			if (this->threads[theKey].joinable()) {
				this->threads[theKey].join();
			}
			this->threads.erase(theKey);
		}
	}	

}

namespace DiscordCoreInternal {

	WorkloadStatus& WorkloadStatus::operator = (WorkloadStatus & other) {
		this->theCurrentStatus.store(other.doWeQuit.load());
		this->doWeQuit.store(other.doWeQuit.load());
		this->threadId = other.threadId;
		return *this;
	}

	WorkloadStatus::WorkloadStatus(WorkloadStatus& other) {
		*this = other;
	}

	WorkloadStatus::WorkloadStatus(std::thread::id threadIdNew) {
		this->threadId = threadIdNew;
	}

	WorkloadStatus::~WorkloadStatus() {
		if (Globals::httpConnections[this->threadId] != nullptr) {
			Globals::httpConnections.erase(this->threadId);
		}
	}

	CoRoutineThreadPool::CoRoutineThreadPool() {
		for (uint32_t x = 0; x < std::thread::hardware_concurrency(); ++x) {
			std::unique_ptr<std::jthread> workerThread = std::make_unique<std::jthread>([this]() {
				auto thePtr = std::make_unique<HttpConnection>();
				Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
				this->threadFunction();
			});
			theThreads.push_back(std::move(workerThread));
		}
	}

	void CoRoutineThreadPool::submitTask(std::coroutine_handle<> coro) noexcept {
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

	void CoRoutineThreadPool::threadFunction() {
		std::unique_lock<std::mutex> theLock00{ this->theMutex01 };
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
						if (value.theCurrentStatus.load() && value.threadId != std::this_thread::get_id()) {
							for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
								if (this->theThreads[x]->get_id() == key) {
									this->theThreads[x]->detach();
									doWeBreak = true;
									this->theThreads.erase(this->theThreads.begin() + x);
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
				return;
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
		std::unique_lock<std::mutex> theLock02{ this->theMutex01 };
		this->theWorkingStatuses.erase(std::this_thread::get_id());
		theLock02.unlock();
	}

	CoRoutineThreadPool::~CoRoutineThreadPool() {
		this->areWeQuitting.store(true);
		for (uint32_t x = 0; x < this->theThreads.size(); x += 1) {
			std::jthread& thread = *this->theThreads[x];
			if (thread.joinable()) {
				thread.join();
			}
			this->theThreads.erase(this->theThreads.begin() + x);
		}
	}

}