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

	void ThreadPool::stopThread(const std::string& theKey) {
		if (ThreadPool::threads.contains(theKey)) {
			ThreadPool::threads[theKey].request_stop();
			if (ThreadPool::threads[theKey].joinable()) {
				ThreadPool::threads[theKey].join();
			}
			ThreadPool::threads.erase(theKey);
		}
	}

	ThreadPool::~ThreadPool() {
		for (auto& [key, value]: this->threads) {
			value.request_stop();
			if (value.joinable()) {
				value.join();
			}
		}
	}
}

namespace DiscordCoreInternal {

	WorkerThread& WorkerThread::operator=(WorkerThread&& other) noexcept {
		if (this != &other) {
			this->theCurrentStatus.store(other.theCurrentStatus.load());
			this->theThread.swap(other.theThread);
		}
		return *this;
	}

	WorkerThread::WorkerThread(WorkerThread&& other) noexcept {
		*this = std::move(other);
	}

	CommandThreadPool::CommandThreadPool() {
		uint32_t threadCount = std::thread::hardware_concurrency() / 2;
		if (threadCount < 1) {
			threadCount = 1;
		}
		for (uint32_t x = 0; x < threadCount; x++) {
			WorkerThread workerThread{};
			this->currentIndex++;
			this->currentCount++;
			int64_t theIndexNew = this->currentIndex;
			workerThread.theThread = std::jthread([=, this](std::stop_token stopToken) {
				this->threadFunction(stopToken, theIndexNew);
			});
			this->workerThreads[this->currentIndex] = std::move(workerThread);
		}
	}

	void CommandThreadPool::submitTask(std::function<void()> coro) noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		bool areWeAllBusy{ true };
		for (auto& [key, value]: this->workerThreads) {
			if (!value.theCurrentStatus.load()) {
				areWeAllBusy = false;
				break;
			}
		}
		if (areWeAllBusy) {
			WorkerThread workerThread{};
			this->currentIndex++;
			this->currentCount++;
			int64_t theIndexNew = this->currentIndex;
			workerThread.theThread = std::jthread([=, this](std::stop_token stopToken) {
				this->threadFunction(stopToken, theIndexNew);
			});
			this->workerThreads[this->currentIndex] = std::move(workerThread);
		}
		this->theFunctions.push(coro);
		theLock.unlock();
		this->theCondVar.notify_one();
	}

	void CommandThreadPool::threadFunction(std::stop_token stopToken, int64_t theIndex) {
		auto theAtomicBoolPtr = &this->workerThreads[theIndex].theCurrentStatus;
		while (!this->areWeQuitting.load() && !stopToken.stop_requested()) {
			std::unique_lock theLock01{ this->theMutex01 };
			while (!this->areWeQuitting.load() && this->theFunctions.size() == 0) {
				if (this->currentCount > std::thread::hardware_concurrency()) {
					for (auto& [key, value]: this->workerThreads) {
						if (value.theCurrentStatus.load() && value.theThread.joinable()) {
							value.theThread.get_stop_source().request_stop();
							value.theThread.detach();
							this->currentCount--;
							break;
						}
					}
				}
				this->theCondVar.wait_for(theLock01, std::chrono::milliseconds(1));
			}

			if (this->areWeQuitting.load() || stopToken.stop_requested()) {
				return;
			}
			auto coroHandle = this->theFunctions.front();
			this->theFunctions.pop();
			theLock01.unlock();
			if (theAtomicBoolPtr) {
				theAtomicBoolPtr->store(true);
			}
			coroHandle();
			if (theAtomicBoolPtr) {
				theAtomicBoolPtr->store(false);
			}
		}
		if (!this->areWeQuitting.load()) {
			this->workerThreads.erase(theIndex);
		}
	}

	void CommandThreadPool::clearContents() {
		if (this->workerThreads.size() == 0) {
			return;
		}
		for (auto& [key, value]: this->workerThreads) {
			value.theThread.request_stop();
			if (value.theThread.joinable()) {
				value.theThread.join();
				this->workerThreads.erase(key);
				break;
			}
		}
		return this->clearContents();
	}

	CommandThreadPool::~CommandThreadPool() {
		this->areWeQuitting.store(true);
		this->clearContents();
	}

	CoRoutineThreadPool::CoRoutineThreadPool() {
		uint32_t threadCount = std::thread::hardware_concurrency();
		if (threadCount < 1) {
			threadCount = 1;
		}
		for (uint32_t x = 0; x < threadCount; x++) {
			WorkerThread workerThread{};
			this->currentIndex++;
			this->currentCount++;
			int64_t theIndexNew = this->currentIndex;
			workerThread.theThread = std::jthread([=, this](std::stop_token stopToken) {
				this->threadFunction(stopToken, theIndexNew);
			});
			this->workerThreads[this->currentIndex] = std::move(workerThread);
		}
	}

	void CoRoutineThreadPool::submitTask(std::coroutine_handle<> coro) noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		bool areWeAllBusy{ true };
		for (auto& [key, value]: this->workerThreads) {
			if (!value.theCurrentStatus.load()) {
				areWeAllBusy = false;
				break;
			}
		}
		if (areWeAllBusy) {
			WorkerThread workerThread{};
			this->currentIndex++;
			this->currentCount++;
			int64_t theIndexNew = this->currentIndex;
			workerThread.theThread = std::jthread([=, this](std::stop_token stopToken) {
				this->threadFunction(stopToken, theIndexNew);
			});
			this->workerThreads[this->currentIndex] = std::move(workerThread);
		}
		this->theCoroutineHandles.push(coro);
		theLock.unlock();
		this->theCondVar.notify_one();
	}

	void CoRoutineThreadPool::threadFunction(std::stop_token stopToken, int64_t theIndex) {
		auto theAtomicBoolPtr = &this->workerThreads[theIndex].theCurrentStatus;
		while (!this->areWeQuitting.load() && !stopToken.stop_requested()) {
			std::unique_lock theLock01{ this->theMutex01 };
			while (!this->areWeQuitting.load() && this->theCoroutineHandles.size() == 0) {
				if (this->currentCount > std::thread::hardware_concurrency()) {
					for (auto& [key, value]: this->workerThreads) {
						if (value.theCurrentStatus.load() && value.theThread.joinable()) {
							value.theThread.get_stop_source().request_stop();
							value.theThread.detach();
							this->currentCount--;
							break;
						}
					}
				}
				this->theCondVar.wait_for(theLock01, std::chrono::milliseconds(1));
			}


			if (this->areWeQuitting.load() || stopToken.stop_requested()) {
				return;
			}
			auto coroHandle = this->theCoroutineHandles.front();
			this->theCoroutineHandles.pop();
			theLock01.unlock();
			if (theAtomicBoolPtr) {
				theAtomicBoolPtr->store(true);
			}
			coroHandle.resume();
			if (theAtomicBoolPtr) {
				theAtomicBoolPtr->store(false);
			}
		}
		if (!this->areWeQuitting.load()) {
			this->workerThreads.erase(theIndex);
		}
	}

	void CoRoutineThreadPool::clearContents() {
		if (this->workerThreads.size() == 0) {
			return;
		}
		for (auto& [key, value]: this->workerThreads) {
			value.theThread.request_stop();
			if (value.theThread.joinable()) {
				value.theThread.join();
				this->workerThreads.erase(key);
				break;
			}
		}
		return this->clearContents();
	}

	CoRoutineThreadPool::~CoRoutineThreadPool() {
		this->areWeQuitting.store(true);
		this->clearContents();
	}

}