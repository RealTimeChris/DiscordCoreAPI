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
/// ThreadPool.cpp - Source file for the "Thread-Pool" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file ThreadPool.cpp

#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreAPI {

	std::string ThreadPool::storeThread(TimeElapsedHandlerNoArgs timeElapsedHandler, int64_t timeInterval) {
		std::string threadId = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

		auto theThread = std::jthread([=](std::stop_token stopToken) {
			StopWatch stopWatch{ std::chrono::milliseconds{ timeInterval } };
			while (true) {
				stopWatch.resetTimer();
				std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int64_t>(std::ceil(static_cast<float>(timeInterval) * thePercentage)) });
				while (!stopWatch.hasTimePassed() && !stopToken.stop_requested()) {
					std::this_thread::sleep_for(1ms);
				}
				if (stopToken.stop_requested()) {
					return;
				}
				timeElapsedHandler();
				if (stopToken.stop_requested()) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
		});
		theThread.detach();
		ThreadPool::threads[threadId] = std::move(theThread);
		return threadId;
	}

	void ThreadPool::stopThread(const std::string& theKey) {
		if (ThreadPool::threads.contains(theKey)) {
			ThreadPool::threads[theKey].request_stop();
			ThreadPool::threads.erase(theKey);
		}
	}

	std::unordered_map<std::string, std::jthread> ThreadPool::threads{};
}

namespace DiscordCoreInternal {

	void closeFunction(std::unordered_map<int64_t, WorkerThread>& other) {
		if (other.size() == 0) {
			return;
		} else {
			for (auto& [key, value]: other) {
				other.erase(key);
				break;
			}
		}
		return closeFunction(other);
	}

	WorkerThread& WorkerThread::operator=(WorkerThread&& other) noexcept {
		if (this != &other) {
			this->areWeCurrentlyWorking.store(other.areWeCurrentlyWorking.load());
			this->theThread.swap(other.theThread);
		}
		return *this;
	}

	WorkerThread::WorkerThread(WorkerThread&& other) noexcept {
		*this = std::move(other);
	}

	CoRoutineThreadPool::CoRoutineThreadPool() {
		this->threadCount.store(std::thread::hardware_concurrency() / 2);
		if (this->threadCount.load() < 1) {
			this->threadCount.store(1);
		}
		for (uint32_t x = 0; x < this->threadCount.load(); ++x) {
			WorkerThread workerThread{};
			this->currentIndex.store(this->currentIndex.load() + 1);
			this->currentCount.store(this->currentCount.load() + 1);
			int64_t theIndexNew = this->currentIndex.load();
			workerThread.theThread = std::jthread([=, this](std::stop_token stopToken) {
				this->threadFunction(stopToken, theIndexNew);
			});
			this->workerThreads[this->currentIndex.load()] = std::move(workerThread);
		}
	}

	void CoRoutineThreadPool::submitTask(std::coroutine_handle<> coro) noexcept {
		std::unique_lock theLock{ this->theMutex };
		bool areWeAllBusy{ true };
		for (auto& [key, value]: this->workerThreads) {
			if (!value.areWeCurrentlyWorking.load()) {
				areWeAllBusy = false;
				break;
			}
		}
		if (areWeAllBusy) {
			WorkerThread workerThread{};
			this->currentIndex.store(this->currentIndex.load() + 1);
			this->currentCount.store(this->currentCount.load() + 1);
			int64_t theIndexNew = this->currentIndex.load();
			workerThread.theThread = std::jthread([=, this](std::stop_token stopToken) {
				this->threadFunction(stopToken, theIndexNew);
			});
			this->workerThreads[this->currentIndex.load()] = std::move(workerThread);
		}
		this->theCoroutineHandles.push(coro);
		this->coroHandleCount.store(this->coroHandleCount.load() + 1);
	}

	void CoRoutineThreadPool::threadFunction(std::stop_token& stopToken, int64_t theIndex) {
		while (!stopToken.stop_requested()) {
			if (this->coroHandleCount.load() > 0) {
				std::unique_lock theLock01{ this->theMutex, std::defer_lock_t{} };
				if (theLock01.try_lock()) {
					if (this->theCoroutineHandles.size() > 0) {
						std::coroutine_handle<> coroHandle = this->theCoroutineHandles.front();
						this->coroHandleCount.store(this->coroHandleCount.load() - 1);
						this->theCoroutineHandles.pop();
						theLock01.unlock();
						this->workerThreads[theIndex].areWeCurrentlyWorking.store(true);
						coroHandle();
						this->workerThreads[theIndex].areWeCurrentlyWorking.store(false);
					}
				}
			} else if (this->currentCount.load() > this->threadCount.load()) {
				std::unique_lock theLock01{ this->theMutex, std::defer_lock_t{} };
				if (theLock01.try_lock()) {
					for (auto& [key, value]: this->workerThreads) {
						if (value.areWeCurrentlyWorking.load() && value.theThread.joinable()) {
							value.theThread.request_stop();
							value.theThread.detach();
							this->currentCount.store(this->currentCount.load() - 1);
							break;
						}
					}
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		std::unique_lock theLock{ this->theMutex };
		this->workerThreads.erase(theIndex);
	}

	CoRoutineThreadPool::~CoRoutineThreadPool() {
		closeFunction(this->workerThreads);
	}

}