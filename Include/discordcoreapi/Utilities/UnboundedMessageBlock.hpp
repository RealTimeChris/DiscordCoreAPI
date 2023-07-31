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
/// ObjectCache.hpp - Header file for the "ObjectCache" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file ObjectCache.hpp

#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

namespace DiscordCoreAPI {

	/// @brief A thread-safe messaging block for data-structures.
	/// @tparam ValueType The type of object that will be sent over the message block.
	template<CopyableOrMovable ValueType> class UnboundedMessageBlock {
	  public:
		inline UnboundedMessageBlock(){};

		inline UnboundedMessageBlock<std::decay_t<ValueType>>& operator=(UnboundedMessageBlock<std::decay_t<ValueType>>&& other) {
			if (this != &other) {
				std::lock(accessMutex, other.accessMutex);
				std::unique_lock lock{ accessMutex, std::adopt_lock };
				std::unique_lock otherLock{ other.accessMutex, std::adopt_lock };
				queue = std::move(other.queue);
			}
			return *this;
		}

		inline UnboundedMessageBlock<std::decay_t<ValueType>>& operator=(const UnboundedMessageBlock<std::decay_t<ValueType>>&) = delete;
		inline UnboundedMessageBlock(const UnboundedMessageBlock&) = delete;

		inline ~UnboundedMessageBlock() = default;

		template<typename ValueTypeNew> inline void send(ValueTypeNew&& object) {
			std::unique_lock lock{ accessMutex };
			queue.emplace_back(std::forward<ValueTypeNew>(object));
		}

		inline void clearContents() {
			std::unique_lock lock{ accessMutex };
			queue.clear();
		}

		inline bool tryReceive(ValueType& object) {
			std::unique_lock lock{ accessMutex };
			if (queue.size() > 0) {
				object = std::move(queue.front());
				queue.pop_front();
				return true;
			} else {
				return false;
			}
		}

		inline uint64_t size() {
			std::unique_lock lock{ accessMutex };
			return queue.size();
		}

	  protected:
		std::deque<std::decay_t<ValueType>> queue{};
		std::mutex accessMutex{};
	};

	template<typename ValueType>
	inline bool waitForTimeToPass(UnboundedMessageBlock<std::decay_t<ValueType>>& outBuffer, ValueType& argOne, uint64_t timeInMsNew) {
		StopWatch<std::chrono::milliseconds> stopWatchNew{ Milliseconds{ timeInMsNew } };
		while (!outBuffer.tryReceive(argOne)) {
			std::this_thread::sleep_for(1ms);
			if (stopWatchNew.hasTimePassed()) {
				return true;
			}
		};
		return false;
	}

}
