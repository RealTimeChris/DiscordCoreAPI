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

namespace DiscordCoreAPI {

	SongAPI::SongAPI(const Snowflake guildIdNew) {
		guildId = guildIdNew;
	}

	void SongAPI::onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, const Snowflake guildId) noexcept {
		onSongCompletionEvent.functions.clear();
		eventToken = onSongCompletionEvent.add(handler);
	}

	bool SongAPI::skip(const GuildMember& guildMember, bool wasItAFail) noexcept {
		AudioFrameData dataFrame{};
		audioDataBuffer.clearContents();
		auto returnValue = DiscordCoreClient::getVoiceConnection(guildId).skip(wasItAFail);
		dataFrame.guildMemberId = guildMember.user.id.operator size_t();
		audioDataBuffer.send(std::move(dataFrame));
		return returnValue;
	}

	std::vector<Song> SongAPI::searchForSong(const std::string& searchQuery) noexcept {
		std::unique_lock lock{ accessMutex };
		auto vector01 = DiscordCoreClient::getSoundCloudAPI(guildId).searchForSong(searchQuery);
		auto vector02 = DiscordCoreClient::getYouTubeAPI(guildId).searchForSong(searchQuery);
		int32_t totalLength = static_cast<int32_t>(vector01.size() + vector02.size());
		std::vector<Song> newVector{};
		int32_t vector01Used{};
		int32_t vector02Used{};
		for (size_t x = 0; x < totalLength; ++x) {
			if ((vector01Used < vector01.size()) && (x % 2 == 0) && vector01.size() > 0) {
				newVector.emplace_back(vector01[vector01Used]);
				newVector[newVector.size() - 1].type = SongType::SoundCloud;
				++vector01Used;
			} else if (vector02Used < vector02.size() && vector02.size() > 0) {
				newVector.emplace_back(vector02[vector02Used]);
				newVector[newVector.size() - 1].type = SongType::YouTube;
				++vector02Used;
			}
		}
		return newVector;
	}

	bool SongAPI::play(Song songNew, const GuildMember& guildMember) noexcept {
		songNew.addedByUserId = guildMember.user.id;
		std::unique_lock lock{ accessMutex };
		if (taskThread) {
			taskThread->request_stop();
			if (taskThread->joinable()) {
				taskThread.reset(nullptr);
			}
		}
		if (songNew.type == SongType::SoundCloud) {
			Song newerSong{ DiscordCoreClient::getSoundCloudAPI(guildMember.guildId).collectFinalSong(songNew) };
			taskThread = makeUnique<std::jthread>([=, this](std::stop_token eventToken) {
				DiscordCoreClient::getSoundCloudAPI(guildMember.guildId).downloadAndStreamAudio(newerSong, eventToken, 0);
			});

		} else if (songNew.type == SongType::YouTube) {
			Song newerSong{ DiscordCoreClient::getYouTubeAPI(guildMember.guildId).collectFinalSong(songNew) };
			taskThread = makeUnique<std::jthread>([=, this](std::stop_token eventToken) {
				DiscordCoreClient::getYouTubeAPI(guildMember.guildId).downloadAndStreamAudio(newerSong, eventToken, 0);
			});
		};
		return DiscordCoreClient::getVoiceConnection(guildMember.guildId).play();
	}

	bool SongAPI::areWeCurrentlyPlaying() const noexcept {
		return DiscordCoreClient::getVoiceConnection(guildId).areWeCurrentlyPlaying();
	}

	bool SongAPI::play(Snowflake guildId) noexcept {
		return DiscordCoreClient::getVoiceConnection(guildId).play();
	}

	bool SongAPI::pauseToggle() noexcept {
		return DiscordCoreClient::getVoiceConnection(guildId).pauseToggle();
	}

	bool SongAPI::stop() noexcept {
		bool returnValue = DiscordCoreClient::getVoiceConnection(guildId).stop();
		audioDataBuffer.clearContents();
		return returnValue;
	}

	void SongAPI::disconnect() noexcept {
		if (taskThread) {
			taskThread->request_stop();
			if (taskThread->joinable()) {
				taskThread->detach();
				taskThread.reset(nullptr);
			}
		}
		onSongCompletionEvent.erase(eventToken);
		audioDataBuffer.clearContents();
		StopWatch stopWatch{ 10000ms };
		while (DiscordCoreClient::getSoundCloudAPI(guildId).areWeWorking() || DiscordCoreClient::getYouTubeAPI(guildId).areWeWorking()) {
			if (stopWatch.hasTimePassed()) {
				break;
			}
			std::this_thread::sleep_for(1ms);
		}
	}

	SongAPI::~SongAPI() noexcept {
		if (taskThread) {
			taskThread->request_stop();
			if (taskThread->joinable()) {
				taskThread->join();
			}
		}
	}
};
