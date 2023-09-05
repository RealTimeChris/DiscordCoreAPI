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
/// RateLimitQueue.hpp - Header file for the "RateLimitQueue Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file RateLimitQueue.hpp

#pragma once

#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/UniquePtr.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <mutex>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		struct RateLimitData {
			friend class HttpsConnectionManager;
			friend class RateLimitStackHolder;
			friend class HttpsRnRBuilder;
			friend class RateLimitQueue;
			friend class HttpsClient;

			inline RateLimitData() noexcept = default;

		  protected:
			std::unique_lock<std::mutex> lock{ accessMutex, std::defer_lock };
			std::atomic<Milliseconds> sampledTimeInMs{ Milliseconds{} };
			std::atomic<Seconds> sRemain{ Seconds{} };
			std::atomic_bool areWeASpecialBucket{};
			std::atomic_bool didWeHitRateLimit{};
			std::atomic_int64_t getsRemaining{ 1 };
			std::atomic_bool haveWeGoneYet{};
			std::atomic_bool doWeWait{};
			std::mutex accessMutex{};
			std::string bucket{};
		};

		class RateLimitQueue {
		  public:
			friend class HttpsClient;

			inline RateLimitQueue() noexcept = default;

			inline void initialize() {
				for (int64_t enumOne = static_cast<int64_t>(HttpsWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpsWorkloadType::LAST); enumOne++) {
					auto tempBucket = std::to_string(std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count());
					buckets.emplace(static_cast<HttpsWorkloadType>(enumOne), tempBucket);
					rateLimits.emplace(tempBucket, makeUnique<RateLimitData>());
					std::this_thread::sleep_for(1ms);
				}
			}

			inline RateLimitData* getEndpointAccess(HttpsWorkloadType workloadType) {
				if (rateLimits[buckets[workloadType]]->getsRemaining.load(std::memory_order_acquire) <= 0) {
					while (HRClock::now().time_since_epoch() - rateLimits[buckets[workloadType]]->sampledTimeInMs.load(std::memory_order_acquire) <=
						std::chrono::duration_cast<Milliseconds>(rateLimits[buckets[workloadType]]->sRemain.load(std::memory_order_acquire))) {
						std::this_thread::sleep_for(1ms);
					}
				}
				while (!rateLimits[buckets[workloadType]]->lock.try_lock()) {
					std::this_thread::sleep_for(1ms);
				}
				return rateLimits[buckets[workloadType]].get();
			}

			inline void relaseEndpointAccess(HttpsWorkloadType workloadType) {
				rateLimits[buckets[workloadType]]->lock.unlock();
			}

		  protected:
			UnorderedMap<HttpsWorkloadType, std::string> buckets{};
			UnorderedMap<std::string, UniquePtr<RateLimitData>> rateLimits{};
		};

	}// namespace DiscordCoreInternal
}
