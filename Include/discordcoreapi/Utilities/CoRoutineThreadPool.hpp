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
			DCA_INLINE worker_thread(){};

			DCA_INLINE worker_thread& operator=(worker_thread&& other) noexcept {
				areWeCurrentlyWorking.store(other.areWeCurrentlyWorking.load(std::memory_order_acquire), std::memory_order_release);
				std::swap(thread, other.thread);
				tasks = std::move(other.tasks);
				return *this;
			}

			DCA_INLINE worker_thread(worker_thread&& other) noexcept {
				*this = std::move(other);
			}

			DCA_INLINE ~worker_thread() = default;

			unbounded_message_block<std::coroutine_handle<>> tasks{};///< Queue of coroutine tasks.
			std::atomic_bool areWeCurrentlyWorking{};///< Atomic flag indicating if the thread is working.
			std::jthread thread{};///< Joinable thread.
		};

		/// @brief A class representing a coroutine-based thread pool.
		class co_routine_thread_pool : protected std::unordered_map<uint64_t, unique_ptr<worker_thread>> {
		  public:
			using map_type = std::unordered_map<uint64_t, unique_ptr<worker_thread>>;

			/// @brief Constructor to create a coroutine thread pool. initializes the worker threads.
			DCA_INLINE co_routine_thread_pool() : threadCount(std::thread::hardware_concurrency()) {
				std::unique_lock lock01{ workerAccessMutex };
				for (uint32_t x = 0; x < threadCount; ++x) {
					uint64_t indexNew = x;
					getMap().emplace(indexNew, makeUnique<worker_thread>());
					std::this_thread::sleep_for(150ms);
					getMap().at(indexNew)->thread = std::jthread{ [=, this](std::stop_token tokenNew) mutable {
						auto indexNewer			  = indexNew;
						threadFunction(getMap().at(indexNewer).get(), tokenNew);
					} };
				}
			}

			/// @brief Submit a coroutine task to the thread pool.
			/// @param coro the coroutine handle to submit.
			DCA_INLINE void submitTask(std::coroutine_handle<> coro) {
				uint64_t currentLowestIndex{ std::numeric_limits<uint64_t>::max() };
				while (currentLowestIndex == std::numeric_limits<uint64_t>::max()) {
					currentLowestIndex = areWeAllBusy();
					std::this_thread::sleep_for(1ms);
				}
				std::shared_lock lock01{ workerAccessMutex };
				if (std ::this_thread::get_id() == getMap()[currentLowestIndex]->thread.get_id()) {
					++currentLowestIndex;
				}
				getMap()[currentLowestIndex]->tasks.send(std::move(coro));
			}

			~co_routine_thread_pool() {
				doWeQuit.store(true, std::memory_order_release);
			}

		  protected:
			std::shared_mutex workerAccessMutex{};///< Shared mutex for worker thread access.
			std::atomic_bool doWeQuit{ false };///< Whether or not we're quitting.
			const uint64_t threadCount{};///< Total thread count.

			/// @brief Thread function for each worker thread.
			/// @param thread A pointer to the current thread of execution.
			/// @param tokenNew The stop token for the thread.
			DCA_INLINE void threadFunction(worker_thread* thread, std::stop_token tokenNew) {
				while (!doWeQuit.load(std::memory_order_acquire) && !tokenNew.stop_requested()) {
					std::coroutine_handle<> coroHandle{};
					if (thread->tasks.tryReceive(coroHandle)) {
						thread->areWeCurrentlyWorking.store(true, std::memory_order_release);
						try {
							coroHandle();
							while (!coroHandle.done()) {
								std::this_thread::sleep_for(1ms);
							}
						} catch (const std::runtime_error& error) {
							message_printer::printError<print_message_type::general>(error.what());
						}
						thread->areWeCurrentlyWorking.store(false, std::memory_order_release);
					}
					std::this_thread::sleep_for(std::chrono::nanoseconds{ 100000 });
				}
			}

			DCA_INLINE uint64_t areWeAllBusy() {
				for (uint64_t x = 0; x < this->size(); ++x) {
					if (this->contains(x)) {
						if (!this->operator[](x)->areWeCurrentlyWorking.load(std::memory_order_acquire)) {
							return x;
						}
					}
				}
				return std::numeric_limits<uint64_t>::max();
			}

			DCA_INLINE map_type& getMap() {
				return *this;
			}
		};

		/**@}*/
	}
}
