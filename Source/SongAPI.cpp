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
/// SongAPI.cpp - Source file for the song api related stuff.
/// Sep 17, 2021
/// https://discordcoreapi.com
/// \file SongAPI.cpp

#include <discordcoreapi/SongAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/YouTubeAPI.hpp>
#include <map>

namespace discord_core_api {

	song_api::song_api(const snowflake guildIdNew) {
		guildId = guildIdNew;
	}

	void song_api::onSongCompletion(std::function<co_routine<void, false>(song_completion_event_data)> handler) {
		onSongCompletionEvent.functions.clear();
		eventToken = onSongCompletionEvent.add(handler);
	}

	bool song_api::skip(bool wasItAFail) {
		audio_frame_data dataFrame{};
		audioDataBuffer.clearContents();
		auto returnValue = discord_core_client::getVoiceConnection(guildId).skip(wasItAFail);
		audioDataBuffer.send(std::move(dataFrame));
		return returnValue;
	}

	jsonifier::vector<song> song_api::searchForSong(jsonifier::string_view searchQuery, uint64_t limit) {
		std::unique_lock lock{ accessMutex };
		jsonifier::vector<song> vector01{};
		jsonifier::vector<song> vector02{};
		if (searchQuery.find("soundcloud") == jsonifier::string::npos && searchQuery.find("youtube") == jsonifier::string::npos) {
			vector01 = discord_core_client::getSoundCloudAPI(guildId).searchForSong(searchQuery, limit);
			vector02 = discord_core_client::getYouTubeAPI(guildId).searchForSong(searchQuery, limit);
		} else if (searchQuery.find("youtube") != jsonifier::string::npos) {
			vector02 = discord_core_client::getYouTubeAPI(guildId).searchForSong(searchQuery, limit);
		} else if (searchQuery.find("soundcloud") != jsonifier::string::npos) {
			vector01 = discord_core_client::getSoundCloudAPI(guildId).searchForSong(searchQuery, limit);
		}
		uint64_t totalLength = vector01.size() + vector02.size();
		jsonifier::vector<song> newVector{};
		uint64_t vector01Used{};
		uint64_t vector02Used{};
		if (vector01.size() == 0 && vector02.size() != 0 || (vector01.size() >= 1 && vector02.size() >= 1)) {
			return vector02;
		} else if (vector02.size() == 0 && vector01.size() != 0 || (vector02.size() >= 1 && vector01.size() >= 1)) {
			return vector01;
		} else {
			for (uint64_t x = 0; x < totalLength; ++x) {
				if ((vector01Used < vector01.size()) && (x % 2 == 0) && vector01.size() > 0) {
					newVector.emplace_back(vector01[vector01Used]);
					newVector[newVector.size() - 1].type = song_type::SoundCloud;
					++vector01Used;
				} else if (vector02Used < vector02.size() && vector02.size() > 0) {
					newVector.emplace_back(vector02[vector02Used]);
					newVector[newVector.size() - 1].type = song_type::YouTube;
					++vector02Used;
				}
			}
			return newVector;
		}
	}

	bool song_api::play(song songNew) {
		std::unique_lock lock{ accessMutex };
		audioDataBuffer.clearContents();
		if (taskThread.getStatus() == co_routine_status::running) {
			taskThread.cancelAndWait();
		}
		discord_core_client::getVoiceConnection(guildId).currentUserId = songNew.addedByUserId;
		if (songNew.type == song_type::SoundCloud) {
			song newerSong{ discord_core_client::getSoundCloudAPI(guildId).collectFinalSong(songNew) };
			taskThread = discord_core_client::getSoundCloudAPI(guildId).downloadAndStreamAudio(newerSong);

		} else if (songNew.type == song_type::YouTube) {
			song newerSong{ discord_core_client::getYouTubeAPI(guildId).collectFinalSong(songNew) };
			taskThread = discord_core_client::getYouTubeAPI(guildId).downloadAndStreamAudio(newerSong);
		};
		return discord_core_client::getVoiceConnection(guildId).play();
	}

	bool song_api::areWeCurrentlyPlaying() const {
		return discord_core_client::getVoiceConnection(guildId).areWeCurrentlyPlaying();
	}

	bool song_api::play() {
		return discord_core_client::getVoiceConnection(guildId).play();
	}

	bool song_api::pauseToggle() {
		return discord_core_client::getVoiceConnection(guildId).pauseToggle();
	}

	bool song_api::stop() {
		bool returnValue = discord_core_client::getVoiceConnection(guildId).stop();
		if (taskThread.getStatus() == co_routine_status::running) {
			taskThread.cancelAndWait();
		}
		audioDataBuffer.clearContents();
		return returnValue;
	}

	void song_api::disconnect() {
		if (taskThread.getStatus() == co_routine_status::running) {
			taskThread.cancelAndWait();
		}
		onSongCompletionEvent.erase(eventToken);
		audioDataBuffer.clearContents();
		stop_watch<milliseconds> stopWatch{ 10000ms };
		stopWatch.reset();
		while (discord_core_client::getSoundCloudAPI(guildId).areWeWorking() || discord_core_client::getYouTubeAPI(guildId).areWeWorking()) {
			if (stopWatch.hasTimeElapsed()) {
				break;
			}
			std::this_thread::sleep_for(1ms);
		}
	}
};
