/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
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
		if (onSongCompletionEvent.functions.size() == 0 && !areWeCurrentlyPlaying()) {
			eventToken = onSongCompletionEvent.add(handler);
		} else {
			onSongCompletionEvent.functions.clear();
			eventToken = onSongCompletionEvent.add(handler);
		}
	}

	bool SongAPI::play(Snowflake guildId) noexcept {
		return DiscordCoreClient::getVoiceConnection(guildId).play();
	}

	bool SongAPI::play(Song songNew, const GuildMember& guildId) noexcept {
		if (DiscordCoreClient::getVoiceConnection(guildId.guildId).activeState.load() != VoiceActiveState::Playing) {
			DiscordCoreClient::getVoiceConnection(guildId.guildId).activeState.store(VoiceActiveState::Playing);

			std::unique_lock lock{ accessMutex };
			if (this->taskThread) {
				this->taskThread->request_stop();
				if (this->taskThread->joinable()) {
					this->taskThread->detach();
					this->taskThread.reset(nullptr);
				}
			}
			if (songNew.type == SongType::SoundCloud) {
				Song newerSong{ DiscordCoreClient::getSoundCloudAPI(guildId.guildId).collectFinalSong(songNew) };
				taskThread = makeUnique<std::jthread>([=, this](std::stop_token eventToken) {
					DiscordCoreClient::getSoundCloudAPI(guildId.guildId).downloadAndStreamAudio(newerSong, eventToken, 0);
				});

			} else if (songNew.type == SongType::YouTube) {
				Song newerSong{ DiscordCoreClient::getYouTubeAPI(guildId.guildId).collectFinalSong(songNew) };
				taskThread = makeUnique<std::jthread>([=, this](std::stop_token eventToken) {
					DiscordCoreClient::getYouTubeAPI(guildId.guildId).downloadAndStreamAudio(newerSong, eventToken, 0);
				});
			};
			return DiscordCoreClient::getVoiceConnection(guildId.guildId).play();
		} else {
			return DiscordCoreClient::getVoiceConnection(guildId.guildId).play();
		}
	}

	void SongAPI::pauseToggle() noexcept {
		if (DiscordCoreClient::getVoiceConnection(guildId).activeState.load() == VoiceActiveState::Stopped) {
			return;
		}
		if (DiscordCoreClient::getVoiceConnection(guildId).activeState.load() == VoiceActiveState::Paused) {
			DiscordCoreClient::getVoiceConnection(guildId).activeState.store(VoiceActiveState::Playing);
		} else {
			DiscordCoreClient::getVoiceConnection(guildId).activeState.store(VoiceActiveState::Paused);
		}
		DiscordCoreClient::getVoiceConnection(guildId).pauseToggle();
	}

	bool SongAPI::areWeCurrentlyPlaying() const noexcept {
		return DiscordCoreClient::getVoiceConnection(guildId).areWeCurrentlyPlaying();
	}

	void SongAPI::skip(Song songNew, const GuildMember& guildId) noexcept {
		std::unique_lock lock{ accessMutex };
		if (DiscordCoreClient::getVoiceConnection(guildId.guildId).activeState.load() == VoiceActiveState::Stopped) {
			return;
		}
		DiscordCoreClient::getVoiceConnection(guildId.guildId).skip();
	}

	void SongAPI::stop() noexcept {
		std::unique_lock lock{ accessMutex };
		this->audioDataBuffer.clearContents();
		DiscordCoreClient::getVoiceConnection(guildId).stop();
		DiscordCoreClient::getVoiceConnection(guildId).doWeSkip.store(false);
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

	void SongAPI::disconnect() noexcept {
		if (this->taskThread) {
			this->taskThread->request_stop();
			if (this->taskThread->joinable()) {
				this->taskThread->detach();
				this->taskThread.reset(nullptr);
			}
		}
		this->onSongCompletionEvent.erase(this->eventToken);
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
