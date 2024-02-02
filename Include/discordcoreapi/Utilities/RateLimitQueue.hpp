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
/// RateLimitQueue.hpp - Header file for the "rate_limit_queue stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file RateLimitQueue.hpp
#pragma once

#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/UniquePtr.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <mutex>

namespace discord_core_api {

	namespace discord_core_internal {

		struct rate_limit_data {
			friend class https_connection_stack_holder;
			friend class https_connection_manager;
			friend class rate_limit_stack_holder;
			friend class https_rnr_builder;
			friend class rate_limit_queue;
			friend class https_client;

		  protected:
			std::unique_lock<std::mutex> lock{ accessMutex, std::defer_lock };
			std::atomic<milliseconds> sampledTimeInMs{ milliseconds{} };
			std::atomic<seconds> sRemain{ seconds{} };
			std::atomic_int64_t getsRemaining{ 1 };
			std::atomic_bool areWeASpecialBucket{};
			std::atomic_bool didWeHitRateLimit{};
			std::atomic_bool doWeWait{};
			jsonifier::string bucket{};
			std::mutex accessMutex{};
		};

		class rate_limit_queue {
		  public:
			friend class https_client;

			DCA_INLINE rate_limit_queue() = default;

			DCA_INLINE void initialize();

			DCA_INLINE rate_limit_data* getEndpointAccess(https_workload_type workloadType);

			DCA_INLINE void releaseEndPointAccess(https_workload_type type);

		  protected:
			unordered_map<jsonifier::string, unique_ptr<rate_limit_data>> rateLimits{};
			unordered_map<https_workload_type, jsonifier::string> buckets{};
		};

	}// namespace discord_core_internal
}
