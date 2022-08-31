/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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

	SongAPI::SongAPI(const Snowflake guildId) {
		this->guildId = guildId;
	}

	void SongAPI::onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, const Snowflake guildId) {
		SongAPI* returnValue = &getSongAPI(guildId);
		returnValue->onSongCompletionEvent.remove(returnValue->eventToken);
		returnValue->eventToken = returnValue->onSongCompletionEvent.add(DiscordCoreInternal::EventDelegate<CoRoutine<void>, SongCompletionEventData>{ handler });
	}

	bool SongAPI::sendNextSong() {
		if (this->playlist.isLoopSongEnabled) {
			if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (int32_t x = 0; x < this->playlist.songQueue.size(); ++x) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			} else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId == "") {
				return true;
			} else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
				return true;
			} else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue[0];
				this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
				return true;
			} else if (this->playlist.currentSong.songId == "") {
				return false;
			}
		} else if (this->playlist.isLoopAllEnabled) {
			if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (int32_t x = 0; x < this->playlist.songQueue.size(); ++x) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			} else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId != "") {
				Song tempSong02 = this->playlist.currentSong;
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (int32_t x = 0; x < this->playlist.songQueue.size(); ++x) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue[this->playlist.songQueue.size() - 1] = tempSong02;
				return true;
			} else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
				return true;
			} else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue[0];
				this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
				return true;
			} else if (this->playlist.currentSong.songId == "") {
				return false;
			}
		} else {
			if (this->playlist.songQueue.size() > 0 && (this->playlist.currentSong.songId != "" || this->playlist.currentSong.songId == "")) {
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (int32_t x = 0; x < this->playlist.songQueue.size() - 1; ++x) {
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			} else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
				this->playlist.currentSong = Song();
				return true;
			} else if (this->playlist.currentSong.songId == "") {
				return false;
			}
		}
		return false;
	}

	bool SongAPI::play(const Snowflake guildId) {
		return getVoiceConnection(guildId).play();
	}

	void SongAPI::pauseToggle(const Snowflake guildId) {
		return getVoiceConnection(guildId).pauseToggle();
	}

	bool SongAPI::areWeCurrentlyPlaying(const Snowflake guildId) {
		return getVoiceConnection(guildId).areWeCurrentlyPlaying();
	}

	void SongAPI::skip(const GuildMember& guildMember) {
		auto guildId = guildMember.guildId;
		getSongAPI(guildId).cancelCurrentSong();
		if (SongAPI::isLoopAllEnabled(guildMember.guildId) || SongAPI::isLoopSongEnabled(guildMember.guildId)) {
			getSongAPI(guildId).playlist.songQueue.emplace_back(getSongAPI(guildId).playlist.currentSong);
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		} else {
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		}
		AudioFrameData frameData{};
		while (getVoiceConnection(guildId).audioDataBuffer.tryReceive(frameData)) {
		};
		frameData.type = AudioFrameType::Skip;
		frameData.guildMemberId = guildMember.id;
		getVoiceConnection(guildId).audioDataBuffer.send(frameData);
	}

	void SongAPI::stop(const Snowflake guildId) {
		getVoiceConnection(guildId).stop();
		getSongAPI(guildId).cancelCurrentSong();
		std::vector<Song> newVector02;
		newVector02.emplace_back(getSongAPI(guildId).playlist.currentSong);
		for (auto& value: getSongAPI(guildId).playlist.songQueue) {
			newVector02.emplace_back(value);
		}
		getSongAPI(guildId).playlist.songQueue = newVector02;
		auto returnValue = &getSongAPI(guildId);
		if (returnValue) {
			getSongAPI(guildId).onSongCompletionEvent.remove(getSongAPI(guildId).eventToken);
			getSongAPI(guildId).eventToken = DiscordCoreInternal::EventDelegateToken{};
		}
	}

	std::vector<Song> SongAPI::searchForSong(const std::string& searchQuery, const Snowflake guildId) {
		auto vector01 = getSoundCloudAPI(guildId).searchForSong(searchQuery);
		auto vector02 = getYouTubeAPI(guildId).searchForSong(searchQuery);
		int32_t totalLength = static_cast<int32_t>(vector01.size() + vector02.size());
		std::vector<Song> newVector{};
		int32_t vector01Used{ 0 };
		int32_t vector02Used{ 0 };
		for (int32_t x = 0; x < totalLength; ++x) {
			if ((vector01Used < vector01.size() - 1) && (x % 2 == 0) && vector01.size() > 0) {
				vector01[vector01Used].type = SongType::SoundCloud;
				newVector.emplace_back(vector01[vector01Used]);
				vector01Used++;
			} else if (vector02Used < vector02.size() - 1 && vector02.size() > 0) {
				vector02[vector02Used].type = SongType::YouTube;
				newVector.emplace_back(vector02[vector02Used]);
				vector02Used++;
			}
		}
		return newVector;
	}

	void SongAPI::setLoopAllStatus(bool enabled, const Snowflake guildId) {
		getSongAPI(guildId).playlist.isLoopAllEnabled = enabled;
	}

	bool SongAPI::isLoopAllEnabled(const Snowflake guildId) {
		return getSongAPI(guildId).playlist.isLoopAllEnabled;
	}

	void SongAPI::setLoopSongStatus(bool enabled, const Snowflake guildId) {
		getSongAPI(guildId).playlist.isLoopSongEnabled = enabled;
	}

	bool SongAPI::isLoopSongEnabled(const Snowflake guildId) {
		return getSongAPI(guildId).playlist.isLoopSongEnabled;
	}

	bool SongAPI::isThereAnySongs(const Snowflake guildId) {
		if (getSongAPI(guildId).playlist.isLoopAllEnabled || getSongAPI(guildId).playlist.isLoopSongEnabled) {
			if (getSongAPI(guildId).playlist.songQueue.size() == 0 && getSongAPI(guildId).playlist.currentSong.songId == "") {
				return false;
			} else {
				return true;
			}
		} else {
			if (getSongAPI(guildId).playlist.songQueue.size() == 0) {
				return false;
			} else {
				return true;
			}
		}
	}

	Song SongAPI::addSongToQueue(const GuildMember& guildMember, Song& song) {
		auto guildId = guildMember.guildId;
		song.addedByUserId = guildMember.id;
		std::string theString{ (( GuildMember& )guildMember).getUserData().userName };
		song.addedByUserName = theString;
		getSongAPI(guildId).playlist.songQueue.emplace_back(song);
		return song;
	}

	void SongAPI::setPlaylist(const Playlist& playlistNew, const Snowflake guildId) {
		getSongAPI(guildId).playlist = playlistNew;
	}

	Playlist SongAPI::getPlaylist(const Snowflake guildId) {
		return getSongAPI(guildId).playlist;
	}

	void SongAPI::modifyQueue(int32_t firstSongPosition, int32_t secondSongPosition, const Snowflake guildId) {
		Song tempSong = getSongAPI(guildId).playlist.songQueue[firstSongPosition];
		getSongAPI(guildId).playlist.songQueue[firstSongPosition] = getSongAPI(guildId).playlist.songQueue[secondSongPosition];
		getSongAPI(guildId).playlist.songQueue[secondSongPosition] = tempSong;
	}

	Song SongAPI::getCurrentSong(const Snowflake guildId) {
		auto theGuild = Guilds::getCachedGuildAsync({ .guildId = guildId }).get();
		if (getSongAPI(guildId).playlist.currentSong.songId != "") {
			return getSongAPI(guildId).playlist.currentSong;
		} else if (getSongAPI(guildId).playlist.songQueue.size() > 0) {
			return getSongAPI(guildId).playlist.songQueue[0];
		} else {
			return Song();
		};
	}

	void SongAPI::setCurrentSong(const Song& song, const Snowflake guildId) {
		getSongAPI(guildId).playlist.currentSong = song;
	}

	void SongAPI::sendNextSongFinal(const GuildMember& guildMember) {
		getSongAPI(guildId).cancelCurrentSong();
		if (getSongAPI(guildId).playlist.currentSong.type == SongType::SoundCloud) {
			Song newerSong = getSoundCloudAPI(guildId).collectFinalSong(getSongAPI(guildId).playlist.currentSong);
			if (getSongAPI(guildId).taskThread) {
				getSongAPI(guildId).taskThread->request_stop();
				if (getSongAPI(guildId).taskThread->joinable()) {
					getSongAPI(guildId).taskThread->join();
				}
			}
			getSongAPI(guildId).taskThread = std::make_unique<std::jthread>([=, this](std::stop_token eventToken) {
				getSoundCloudAPI(guildId).downloadAndStreamAudio(newerSong, eventToken, 0);
			});

		} else if (getSongAPI(guildId).playlist.currentSong.type == SongType::YouTube) {
			Song newerSong = getYouTubeAPI(guildId).collectFinalSong(getSongAPI(guildId).playlist.currentSong);
			if (getSongAPI(guildId).taskThread) {
				getSongAPI(guildId).taskThread->request_stop();
				if (getSongAPI(guildId).taskThread->joinable()) {
					getSongAPI(guildId).taskThread->join();
				}
			}
			getSongAPI(guildId).taskThread = std::make_unique<std::jthread>([=, this](std::stop_token eventToken) {
				getYouTubeAPI(guildId).downloadAndStreamAudio(newerSong, eventToken, 0);
			});
		};
	}

	bool SongAPI::sendNextSong(const GuildMember& guildMember) {
		auto guildId = guildMember.guildId;
		std::unique_lock accessLock{ SongAPI::accessMutex };
		getSongAPI(guildId).sendNextSong();
		if (getSongAPI(guildId).playlist.currentSong.songId == "") {
			getSongAPI(guildId).sendNextSong();
		}
		getSongAPI(guildId).sendNextSongFinal(guildMember);
		return true;
	}

	void SongAPI::cancelCurrentSong() {
		if (this->taskThread) {
			this->taskThread->request_stop();
			if (this->taskThread->joinable()) {
				this->taskThread->join();
			}
			this->taskThread.reset(nullptr);
		}
		DiscordCoreAPI::AudioFrameData dataFrame{};
		while (DiscordCoreAPI::getVoiceConnection(guildId).audioDataBuffer.tryReceive(dataFrame)) {
		};
	}

	std::mutex SongAPI::accessMutex{};
};