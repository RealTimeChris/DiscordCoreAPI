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
/// UnboundedMessageBlock.hpp - Header file for the "UnboundedMessageBlock" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file UnboundedMessageBlock.hpp
#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

namespace discord_core_api {

	template<typename value_type>
	concept copyable_or_movable = std::copyable<std::remove_cvref_t<value_type>> || std::movable<std::remove_cvref_t<value_type>>;

	/// @brief A thread-safe messaging block for data-structures.
	/// @tparam value_type the type of object that will be sent over the message block.
	template<copyable_or_movable value_type_new> class unbounded_message_block {
	  public:
		using value_type = value_type_new;

		DCA_INLINE unbounded_message_block(){};

		DCA_INLINE unbounded_message_block& operator=(unbounded_message_block&& other) noexcept {
			if (this != &other) {
				std::swap(queue, other.queue);
			}
			return *this;
		}

		DCA_INLINE unbounded_message_block& operator=(const unbounded_message_block&) = delete;
		DCA_INLINE unbounded_message_block(const unbounded_message_block&)			  = delete;

		template<copyable_or_movable value_type_newer> DCA_INLINE void send(value_type_newer&& object) {
			std::unique_lock lock{ accessMutex };
			queue.emplace_back(std::forward<value_type_newer>(object));
		}

		DCA_INLINE void clearContents() {
			std::unique_lock lock{ accessMutex };
			queue.clear();
		}

		DCA_INLINE bool tryReceive(value_type& object) {
			std::unique_lock lock{ accessMutex };
			if (queue.size() > 0) {
				object = std::move(queue.front());
				queue.pop_front();
				return true;
			} else {
				return false;
			}
		}

		DCA_INLINE uint64_t size() {
			std::unique_lock lock{ accessMutex };
			return queue.size();
		}

		DCA_INLINE ~unbounded_message_block() = default;

	  protected:
		std::deque<value_type> queue{};
		std::mutex accessMutex{};
	};

	template<typename value_type> DCA_INLINE bool waitForTimeToPass(unbounded_message_block<std::remove_cvref_t<value_type>>& outBuffer, value_type& argOne, uint64_t timeInMsNew) {
		stop_watch<milliseconds> stopWatch{ milliseconds{ timeInMsNew } };
		stopWatch.reset();
		while (!outBuffer.tryReceive(argOne)) {
			std::this_thread::sleep_for(1ms);
			if (stopWatch.hasTimeElapsed()) {
				return true;
			}
		};
		return false;
	}

}
