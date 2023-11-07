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
/// CoRoutineThreadPool.hpp - Header for the "co_routine_thread_pool" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file CoRoutineThreadPool.hpp
#pragma once

#include <discordcoreapi/Utilities/UnboundedMessageBlock.hpp>
#include <discordcoreapi/Utilities/ThreadWrapper.hpp>
#include <coroutine>
#include <deque>

using namespace std::literals;

namespace discord_core_api {

	namespace discord_core_internal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/

		/// @brief A struct representing a worker thread for coroutine-based tasks.
		struct worker_thread {
			inline worker_thread() = default;

			inline worker_thread& operator=(worker_thread&& other) noexcept {
				this->areWeCurrentlyWorking.store(other.areWeCurrentlyWorking.load(std::memory_order_acquire), std::memory_order_release);
				std::swap(this->thread, other.thread);
				tasks = std::move(other.tasks);
				return *this;
			}

			inline worker_thread(worker_thread&& other) noexcept {
				*this = std::move(other);
			}

			inline ~worker_thread() = default;

			unbounded_message_block<std::coroutine_handle<>> tasks{};///< Queue of coroutine tasks.
			std::atomic_bool areWeCurrentlyWorking{};///< Atomic flag indicating if the thread is working.
			thread_wrapper thread{};///< Joinable thread.
		};

		/// @brief A class representing a coroutine-based thread pool.
		class co_routine_thread_pool : protected std::vector<worker_thread> {
		  public:
			using map_type = std::vector<worker_thread>;

			/// @brief Constructor to create a coroutine thread pool. initializes the worker threads.
			inline co_routine_thread_pool() : threadCount(std::thread::hardware_concurrency()) {
				for (uint32_t x = 0; x < threadCount; ++x) {
					currentIndex.fetch_add(1, std::memory_order_release);
					currentCount.fetch_add(1, std::memory_order_release);
					uint64_t indexNew = currentIndex.load(std::memory_order_acquire);
					getMap().emplace_back(worker_thread());
					getMap().back().thread = thread_wrapper([=, this]() {
						threadFunction(indexNew);
					});
				}
			}

			/// @brief Submit a coroutine task to the thread pool.
			/// @param coro the coroutine handle to submit.
			inline void submitTask(std::coroutine_handle<> coro) {
				bool areWeAllBusy{ true };
				uint64_t currentLowestValue{ std::numeric_limits<uint64_t>::max() };
				uint64_t currentLowestIndex{ std::numeric_limits<uint64_t>::max() };
				std::shared_lock lock01{ workerAccessMutex };
				for (auto iter = getMap().begin(); iter != getMap().end(); ++iter) {
					if (!iter->areWeCurrentlyWorking.load(std::memory_order_acquire)) {
						areWeAllBusy = false;
						if (iter->tasks.size() < currentLowestValue) {
							currentLowestValue = static_cast<uint64_t>(iter->tasks.size());
							currentLowestIndex = static_cast<uint64_t>(iter - getMap().begin());
						}
						break;
					}
				}
				if (areWeAllBusy) {
					currentIndex.fetch_add(1, std::memory_order_release);
					currentCount.fetch_add(1, std::memory_order_release);
					uint64_t indexNew = currentIndex.load(std::memory_order_acquire);
					lock01.unlock();
					std::unique_lock lock02{ workerAccessMutex };
					getMap().emplace_back(worker_thread());
					getMap().back().thread = thread_wrapper([=, this]() {
						threadFunction(indexNew);
					});
					lock02.unlock();
				} else {
					getMap().at(currentLowestIndex).tasks.send(std::move(coro));
				}
			}

			~co_routine_thread_pool() {
				areWeStopping.store(true, std::memory_order_release);
				std::unique_lock lock{ workerAccessMutex };
			}

		  protected:
			std::shared_mutex workerAccessMutex{};///< Shared mutex for worker thread access.
			std::atomic_uint64_t currentCount{};///< current count of worker threads.
			std::atomic_uint64_t currentIndex{};///< current index of worker threads.
			std::atomic_bool areWeStopping{};
			const uint64_t threadCount{};///< Total thread count.

			/// @brief Thread function for each worker thread.
			/// @param stopToken the stop token for the thread.
			/// @param index the index of the worker thread.
			inline void threadFunction(uint64_t index) {
				while (!areWeStopping.load(std::memory_order_acquire)) {
					if (getMap().size() <= index) {
						return;
					}
					std::coroutine_handle<> coroHandle{};
					if (getMap().at(index).tasks.tryReceive(coroHandle)) {
						getMap().at(index).areWeCurrentlyWorking.store(true, std::memory_order_release);
						try {
							coroHandle();
							while (!coroHandle.done()) {
								std::this_thread::sleep_for(1ms);
							}
						} catch (const std::runtime_error& error) {
							std::cout << error.what() << std::endl;
						}
						if (getMap().size() <= index) {
							return;
						}
						getMap().at(index).areWeCurrentlyWorking.store(false, std::memory_order_release);
					}
					if (currentCount.load(std::memory_order_acquire) > threadCount) {
						uint64_t extraWorkers{ currentCount.load(std::memory_order_acquire) - threadCount };
						while (extraWorkers > 0) {
							--extraWorkers;
							size_type currentHighestIndex{};
							for (uint64_t key = 0; key < getMap().size(); ++key) {
								if (key > currentHighestIndex) {
									currentHighestIndex = key;
								}
							}
							std::unique_lock lock{ workerAccessMutex };
						}
					}
					std::this_thread::sleep_for(std::chrono::nanoseconds{ 100000 });
				}
			}

			inline map_type& getMap() {
				return *this;
			}
		};

		/**@}*/
	}
}
