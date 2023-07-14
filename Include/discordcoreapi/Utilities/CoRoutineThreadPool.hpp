/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// CoRoutineThreadPool.hpp - Header for the "CoRoutineThreadPool" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file CoRoutineThreadPool.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <coroutine>

namespace DiscordCoreAPI {

	/**
	* \addtogroup utilities
	* @{
	*/
	namespace DiscordCoreInternal {

		struct DiscordCoreAPI_Dll WorkerThread {
			inline WorkerThread() = default;

			inline ~WorkerThread() = default;

			UnboundedMessageBlock<std::coroutine_handle<>> tasks{};
			std::atomic_bool areWeCurrentlyWorking{};
			std::jthread thread{};
		};

		class DiscordCoreAPI_Dll CoRoutineThreadPool {
		  public:
			friend class DiscordCoreAPI::DiscordCoreClient;

			inline CoRoutineThreadPool() : threadCount(std::thread::hardware_concurrency()) {
				for (uint32_t x = 0; x < threadCount; ++x) {
					UniquePtr<WorkerThread> workerThread{ makeUnique<WorkerThread>() };
					currentIndex.store(currentIndex.load() + 1);
					currentCount.store(currentCount.load() + 1);
					int64_t indexNew = currentIndex.load();
					workerThread->thread = std::jthread([=, this](std::stop_token stopToken) {
						threadFunction(stopToken, indexNew);
					});
					workerThreads.emplace(std::make_pair(currentIndex.load(), std::move(workerThread)));
				}
			}

			inline void submitTask(std::coroutine_handle<> coro) {
				bool areWeAllBusy{ true };
				uint64_t currentLowestValue{ std::numeric_limits<uint64_t>::max() };
				uint64_t currentLowestIndex{ std::numeric_limits<uint64_t>::max() };
				std::shared_lock lock01{ workerAccessMutex };
				for (auto& [key, value]: workerThreads) {
					if (!value->areWeCurrentlyWorking.load()) {
						areWeAllBusy = false;
						if (value->tasks.size() < currentLowestValue) {
							currentLowestValue = value->tasks.size();
							currentLowestIndex = key;
						}
						break;
					}
				}
				if (areWeAllBusy) {
					UniquePtr<WorkerThread> workerThread{ makeUnique<WorkerThread>() };
					currentIndex.store(currentIndex.load() + 1);
					currentCount.store(currentCount.load() + 1);
					int64_t indexNew = currentIndex.load();
					workerThread->thread = std::jthread([=, this](std::stop_token stopToken) {
						threadFunction(stopToken, indexNew);
					});
					lock01.unlock();
					std::unique_lock lock02{ workerAccessMutex };
					workerThreads.emplace(std::make_pair(currentIndex.load(), std::move(workerThread)));
					lock02.unlock();
				} else {
					workerThreads.at(currentLowestIndex)->tasks.send(std::move(coro));
				}
			}

		  protected:
			std::unordered_map<int64_t, UniquePtr<WorkerThread>> workerThreads{};
			std::shared_mutex workerAccessMutex{};
			std::atomic_int64_t currentCount{};
			std::atomic_int64_t currentIndex{};
			const int64_t threadCount{};

			inline void threadFunction(std::stop_token stopToken, int64_t index) {
				while (!stopToken.stop_requested()) {
					std::coroutine_handle<> coroHandle{};
					if (workerThreads.at(index)->tasks.tryReceive(coroHandle)) {
						workerThreads.at(index)->areWeCurrentlyWorking.store(true);
						try {
							coroHandle();
						} catch (const DCAException& error) {
							MessagePrinter::printError<PrintMessageType::General>(error.what());
						}
						workerThreads.at(index)->areWeCurrentlyWorking.store(false);
					}
					if (currentCount.load() > threadCount) {
						int64_t extraWorkers{ currentCount.load() - threadCount };
						while (extraWorkers > 0) {
							--extraWorkers;
							std::unique_lock lock{ workerAccessMutex };
							const auto oldThread = workerThreads.begin();
							if (oldThread->second->thread.joinable()) {
								oldThread->second->thread.request_stop();
								oldThread->second->thread.detach();
								currentCount.store(currentCount.load() - 1);
								workerThreads.erase(oldThread->first);
							}
						}
					}
					std::this_thread::sleep_for(100000ns);
				}
			}
		};
		/**@}*/
	}
}
