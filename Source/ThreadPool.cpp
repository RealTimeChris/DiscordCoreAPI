/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// ThreadPool.cpp - Source file for the "Thread-Pool" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file ThreadPool.cpp

#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreAPI {

	std::string ThreadPool::storeThread(TimeElapsedHandlerNoArgs timeElapsedHandler, int64_t timeInterval) {
		std::string threadId = std::to_string(std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count());

		auto thread = std::jthread([=](std::stop_token token) {
			StopWatch stopWatch{ Milliseconds{ timeInterval } };
			while (true) {
				stopWatch.resetTimer();
				std::this_thread::sleep_for(Milliseconds{ static_cast<int64_t>(std::ceil(static_cast<double>(timeInterval) * percentage)) });
				while (!stopWatch.hasTimePassed() && !token.stop_requested()) {
					std::this_thread::sleep_for(1ms);
				}
				if (token.stop_requested()) {
					return;
				}
				timeElapsedHandler();
				if (token.stop_requested()) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
		});
		thread.detach();
		ThreadPool::threads[threadId] = std::move(thread);
		return threadId;
	}

	void ThreadPool::stopThread(const std::string& key) {
		if (ThreadPool::threads.contains(key)) {
			ThreadPool::threads[key].request_stop();
			ThreadPool::threads.erase(key);
		}
	}

	std::unordered_map<std::string, std::jthread> ThreadPool::threads{};
}

namespace DiscordCoreInternal {

	WorkerThread& WorkerThread::operator=(WorkerThread&& other) noexcept {
		if (this != &other) {
			areWeCurrentlyWorking.store(other.areWeCurrentlyWorking.load());
			thread.swap(other.thread);
		}
		return *this;
	}

	CoRoutineThreadPool::CoRoutineThreadPool()
		: threadCount(std::thread::hardware_concurrency() / 2 > 0 ? std::thread::hardware_concurrency() / 2 : 1) {
		for (uint32_t x = 0; x < threadCount.load(); ++x) {
			WorkerThread workerThread{};
			currentIndex.store(currentIndex.load() + 1);
			currentCount.store(currentCount.load() + 1);
			int64_t indexNew = currentIndex.load();
			workerThread.thread = std::jthread([=, this](std::stop_token stopToken) {
				threadFunction(stopToken, indexNew);
			});
			workerThreads[currentIndex.load()] = std::move(workerThread);
		}
	}

	void CoRoutineThreadPool::submitTask(std::coroutine_handle<> coro) noexcept {
		std::shared_lock lock01{ workerAccessMutex };
		bool areWeAllBusy{ true };
		for (const auto& [key, value]: workerThreads) {
			if (!value.areWeCurrentlyWorking.load()) {
				areWeAllBusy = false;
				break;
			}
		}
		if (areWeAllBusy) {
			WorkerThread workerThread{};
			currentIndex.store(currentIndex.load() + 1);
			currentCount.store(currentCount.load() + 1);
			int64_t indexNew = currentIndex.load();
			workerThread.thread = std::jthread([=, this](std::stop_token stopToken) {
				threadFunction(stopToken, indexNew);
			});
			lock01.unlock();
			std::unique_lock lock{ workerAccessMutex };
			workerThreads[currentIndex.load()] = std::move(workerThread);
		}
		std::unique_lock lock{ coroHandleAccessMutex };
		coroutineHandles.emplace_back(coro);
		lock.unlock();
		coroHandleCount.store(coroHandleCount.load() + 1);
	}

	void CoRoutineThreadPool::threadFunction(std::stop_token stopToken, int64_t index) {
		while (!stopToken.stop_requested()) {
			if (coroHandleCount.load() > 0) {
				std::unique_lock lock{ coroHandleAccessMutex, std::defer_lock_t{} };
				if (lock.try_lock() && coroutineHandles.size() > 0) {
					std::coroutine_handle<> coroHandle = coroutineHandles.front();
					coroHandleCount.store(coroHandleCount.load() - 1);
					coroutineHandles.pop_front();
					lock.unlock();
					workerThreads[index].areWeCurrentlyWorking.store(true);
					coroHandle();
					workerThreads[index].areWeCurrentlyWorking.store(false);
				}
			}
			if (currentCount.load() > threadCount.load()) {
				int64_t extraWorkers{ currentCount.load() - threadCount.load() };
				while (extraWorkers > 0) {
					--extraWorkers;
					std::unique_lock lock{ workerAccessMutex };
					const auto oldThread = workerThreads.begin();
					if (oldThread->second.thread.joinable() && oldThread->second.areWeCurrentlyWorking.load()) {
						oldThread->second.thread.detach();
						currentCount.store(currentCount.load() - 1);
						workerThreads.erase(oldThread->first);
					}
				}
			}
			std::this_thread::sleep_for(100000ns);
		}
	}
}