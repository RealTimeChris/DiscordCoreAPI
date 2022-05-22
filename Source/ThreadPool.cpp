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

	WorkerThread::WorkerThread() {
		this->threadId = std::make_unique<std::thread::id>();
	}

	WorkerThread& WorkerThread::operator=(WorkerThread& other) {
		this->theCurrentStatus.store(other.theCurrentStatus.load());
		this->theThread.swap(other.theThread);
		this->threadId.swap(other.threadId);
		return *this;
	}

	WorkerThread::WorkerThread(WorkerThread& other) {
		*this = other;
	}

	WorkerThread::~WorkerThread() {
		if (this->threadId != nullptr) {
			if (Globals ::httpConnections.contains(*this->threadId)) {
				Globals::httpConnections.erase(*this->threadId);
			}
		}
	}

	CoRoutineThreadPool::CoRoutineThreadPool() {
		for (uint32_t x = 0; x < std::thread::hardware_concurrency(); ++x) {
			WorkerThread workerThread{};
			this->currentIndex += 1;
			int64_t currentIndex = this->currentIndex;
			workerThread.theThread = std::jthread([=, this]() {
				auto thePtr = std::make_unique<HttpConnection>();
				Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
				this->threadFunction(currentIndex);
			});
			this->workerThreads.insert_or_assign(currentIndex, workerThread);
		}
	}

	void CoRoutineThreadPool::submitTask(std::coroutine_handle<> coro) noexcept {
		std::unique_lock<std::mutex> theLock(this->theMutex01);
		bool areWeAllBusy{ true };
		for (auto& [key, value]: this->workerThreads) {
			if (!value.theCurrentStatus.load()) {
				areWeAllBusy = false;
			}
		}
		if (areWeAllBusy) {
			WorkerThread workerThread{};
			this->currentIndex += 1;
			int64_t currentIndex = this->currentIndex;
			workerThread.theThread = std::jthread([=, this]() {
				auto thePtr = std::make_unique<HttpConnection>();
				Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::move(thePtr)));
				this->threadFunction(currentIndex);
			});
			this->workerThreads.insert_or_assign(currentIndex, workerThread);
		}
		this->theCoroutineHandles.emplace(coro);
		this->theCondVar.notify_one();
	}

	void CoRoutineThreadPool::threadFunction(int64_t theIndex) {
		std::unique_lock<std::mutex> theLock00{ this->theMutex01 };
		auto theAtomicBoolPtr = &this->workerThreads[theIndex].theCurrentStatus;
		*this->workerThreads[theIndex].threadId = std::this_thread::get_id();
		theLock00.unlock();
		while (!this->areWeQuitting.load()) {
			std::unique_lock<std::mutex> theLock01{ this->theMutex01 };
			while (!this->areWeQuitting.load() && this->theCoroutineHandles.size() == 0) {
				if (this->workerThreads.size() > std::thread::hardware_concurrency()) {
					for (auto& [key, value]: this->workerThreads) {
						if (value.theCurrentStatus.load() && theIndex != key) {
							if (value.theThread.joinable() && this->workerThreads.size() > std::thread::hardware_concurrency()) {
								value.theThread.detach();
								this->workerThreads.erase(key);
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
			if (!this->workerThreads.contains(theIndex)) {
				return;
			}
		}
	}

	CoRoutineThreadPool::~CoRoutineThreadPool() {
		this->areWeQuitting.store(true);
		for (auto&[key, value]:this->workerThreads) {
			if (value.theThread.joinable()) {
				value.theThread.join();
			}
			this->workerThreads.erase(key);
		}
	}

}