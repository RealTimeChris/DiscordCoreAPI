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
#include <discordcoreapi/Utilities/ThreadWrapper.hpp>
#include <coroutine>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/

		/// @brief A struct representing a worker thread for coroutine-based tasks.
		struct WorkerThread {
			inline WorkerThread() = default;

			inline ~WorkerThread() = default;

			UnboundedMessageBlock<std::coroutine_handle<>> tasks{};///< Queue of coroutine tasks.
			std::atomic_bool areWeCurrentlyWorking{};///< Atomic flag indicating if the thread is working.
			UniquePtr<ThreadWrapper> thread{};///< Joinable thread.
		};

		/// @brief A class representing a coroutine-based thread pool.
		class CoRoutineThreadPool {
		  public:
			friend class DiscordCoreAPI::DiscordCoreClient;

			/// @brief Constructor to create a coroutine thread pool. Initializes the worker threads.
			inline CoRoutineThreadPool() : threadCount(std::thread::hardware_concurrency()) {
				for (uint32_t x = 0; x < threadCount; ++x) {
					UniquePtr<WorkerThread> workerThread{ makeUnique<WorkerThread>() };
					currentIndex.fetch_add(1, std::memory_order_release);
					currentCount.fetch_add(1, std::memory_order_release);
					uint64_t indexNew = currentIndex.load(std::memory_order_acquire);
					workerThreads.emplace(indexNew, std::move(workerThread));
					workerThreads[indexNew]->thread = makeUnique<ThreadWrapper>([=, this](StopToken stopToken) {
						threadFunction(stopToken, indexNew);
					});
				}
			}

			/// @brief Submit a coroutine task to the thread pool.
			/// @param coro The coroutine handle to submit.
			inline void submitTask(std::coroutine_handle<> coro) {
				bool areWeAllBusy{ true };
				uint64_t currentLowestValue{ std::numeric_limits<uint64_t>::max() };
				uint64_t currentLowestIndex{ std::numeric_limits<uint64_t>::max() };
				std::shared_lock lock01{ workerAccessMutex };
				for (auto& [key, value]: workerThreads) {
					if (!value->areWeCurrentlyWorking.load(std::memory_order_acquire)) {
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
					currentIndex.fetch_add(1, std::memory_order_release);
					currentCount.fetch_add(1, std::memory_order_release);
					uint64_t indexNew = currentIndex.load(std::memory_order_acquire);
					lock01.unlock();
					std::unique_lock lock02{ workerAccessMutex };
					workerThreads.emplace(indexNew, std::move(workerThread));
					workerThreads[indexNew]->thread = makeUnique<ThreadWrapper>([=, this](StopToken stopToken) {
						threadFunction(stopToken, indexNew);
					});
					lock02.unlock();
				} else {
					workerThreads[currentLowestIndex]->tasks.send(std::move(coro));
				}
			}

		  protected:
			DiscordCoreAPI::UnorderedMap<uint64_t, UniquePtr<WorkerThread>> workerThreads{};///< Map of worker threads.
			std::shared_mutex workerAccessMutex{};///< Shared mutex for worker thread access.
			std::atomic_uint64_t currentCount{};///< Current count of worker threads.
			std::atomic_uint64_t currentIndex{};///< Current index of worker threads.
			const uint64_t threadCount{};///< Total thread count.

			/// @brief Thread function for each worker thread.
			/// @param stopToken The stop token for the thread.
			/// @param index The index of the worker thread.
			inline void threadFunction(StopToken stopToken, uint64_t index) {
				while (!stopToken.stopRequested()) {
					if (!workerThreads.contains(index)) {
						return;
					}
					std::coroutine_handle<> coroHandle{};
					if (workerThreads[index]->tasks.tryReceive(coroHandle)) {
						workerThreads[index]->areWeCurrentlyWorking.store(true, std::memory_order_release);
						try {
							coroHandle();
						} catch (...) {
						}
						if (!workerThreads.contains(index)) {
							return;
						}
						workerThreads[index]->areWeCurrentlyWorking.store(false, std::memory_order_release);
					}
					if (currentCount.load(std::memory_order_acquire) > threadCount) {
						uint64_t extraWorkers{ currentCount.load(std::memory_order_acquire) - threadCount };
						while (extraWorkers > 0) {
							--extraWorkers;
							std::unique_lock lock{ workerAccessMutex };
							auto oldThread = workerThreads.begin();
							if (oldThread.operator*().second->thread->joinable()) {
								oldThread.operator*().second->thread->requestStop();
								oldThread.operator*().second->thread->detach();
								currentCount.store(currentCount.load(std::memory_order_acquire) - 1, std::memory_order_release);
								workerThreads.erase(oldThread.operator*().first);
							}
						}
					}
					std::this_thread::sleep_for(Nanoseconds{ 100000 });
				}
			}
		};

		/**@}*/
	}
}
