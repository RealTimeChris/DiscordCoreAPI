/*
*
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
/// Chris M.
/// https://discordcoreapi.com
/// \file ThreadPool.cpp

#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreAPI {

	std::string ThreadPool::storeThread(TimeElapsedHandler timeElapsedHandler, int32_t timeInterval) {
		std::string threadId =
			std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

		this->threads.insert(std::make_pair(threadId, std::jthread([=](std::stop_token stopToken) {
			StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{ timeInterval } };
			while (true) {
				stopWatch.resetTimer();
				std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int32_t>(std::ceil(static_cast<float>(timeInterval) * 90.0f / 100.0f)) });
				while (!stopWatch.hasTimePassed()) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
				}
				if (stopToken.stop_requested()) {
					return;
				}
				timeElapsedHandler();
				if (stopToken.stop_requested()) {
					return;
				}
			}
		})));
		this->threads[threadId].detach();
		return threadId;
	}

	void ThreadPool::stopThread(std::string theKey) {
		if (this->threads.contains(theKey)) {
			this->threads[theKey].request_stop();
			this->threads.erase(theKey);
		}
	}

}