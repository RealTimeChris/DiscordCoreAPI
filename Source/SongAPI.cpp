/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
		SongAPI* returnValue = getSongAPIMap()[guildId].get();
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
				this->playlist.currentSong = this->playlist.currentSong;
				return true;
			} else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
				this->playlist.currentSong = this->playlist.currentSong;
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
		return getVoiceConnectionMap()[guildId]->play();
	}

	void SongAPI::pauseToggle(const Snowflake guildId) {
		getVoiceConnectionMap()[guildId]->pauseToggle();
	}

	bool SongAPI::areWeCurrentlyPlaying(const Snowflake guildId) {
		return getVoiceConnectionMap()[guildId]->areWeCurrentlyPlaying();
	}

	void SongAPI::skip(const GuildMember& guildMember) {
		getSongAPIMap()[guildMember.guildId]->cancelCurrentSong();
		if (SongAPI::isLoopAllEnabled(guildMember.guildId) || SongAPI::isLoopSongEnabled(guildMember.guildId)) {
			getSongAPIMap()[guildMember.guildId]->playlist.songQueue.emplace_back(getSongAPIMap()[guildMember.guildId]->playlist.currentSong);
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		} else {
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		}
		AudioFrameData frameData{};
		while (getVoiceConnectionMap()[guildMember.guildId]->audioDataBuffer.tryReceive(frameData)) {
		};
		frameData.type = AudioFrameType::Skip;
		frameData.guildMemberId = guildMember.id;
		getVoiceConnectionMap()[guildMember.guildId]->audioDataBuffer.send(frameData);
	}

	void SongAPI::stop(const Snowflake guildId) {
		getVoiceConnectionMap()[guildId]->stop();
		getSongAPIMap()[guildId]->cancelCurrentSong();
		std::vector<Song> newVector02;
		newVector02.emplace_back(getSongAPIMap()[guildId]->playlist.currentSong);
		for (auto& value: getSongAPIMap()[guildId]->playlist.songQueue) {
			newVector02.emplace_back(value);
		}
		getSongAPIMap()[guildId]->playlist.songQueue = newVector02;
		auto returnValue = getSongAPIMap()[guildId].get();
		if (returnValue) {
			getSongAPIMap()[guildId]->onSongCompletionEvent.remove(getSongAPIMap()[guildId]->eventToken);
			getSongAPIMap()[guildId]->eventToken = DiscordCoreInternal::EventDelegateToken{};
		}
	}

	std::vector<Song> SongAPI::searchForSong(const std::string& searchQuery, const Snowflake guildId) {
		auto vector01 = getSoundCloudAPIMap()[guildId]->searchForSong(searchQuery);
		auto vector02 = getYouTubeAPIMap()[guildId]->searchForSong(searchQuery);
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
		getSongAPIMap()[guildId]->playlist.isLoopAllEnabled = enabled;
	}

	bool SongAPI::isLoopAllEnabled(const Snowflake guildId) {
		return getSongAPIMap()[guildId]->playlist.isLoopAllEnabled;
	}

	void SongAPI::setLoopSongStatus(bool enabled, const Snowflake guildId) {
		getSongAPIMap()[guildId]->playlist.isLoopSongEnabled = enabled;
	}

	bool SongAPI::isLoopSongEnabled(const Snowflake guildId) {
		return getSongAPIMap()[guildId]->playlist.isLoopSongEnabled;
	}

	bool SongAPI::isThereAnySongs(const Snowflake guildId) {
		if (!getSongAPIMap().contains(guildId)) {
			getSongAPIMap()[guildId] = std::make_unique<SongAPI>(guildId);
		}
		if (getSongAPIMap()[guildId]->playlist.isLoopAllEnabled || getSongAPIMap()[guildId]->playlist.isLoopSongEnabled) {
			if (getSongAPIMap()[guildId]->playlist.songQueue.size() == 0 && getSongAPIMap()[guildId]->playlist.currentSong.songId == "") {
				return false;
			} else {
				return true;
			}
		} else {
			if (getSongAPIMap()[guildId]->playlist.songQueue.size() == 0) {
				return false;
			} else {
				return true;
			}
		}
	}

	Song SongAPI::addSongToQueue(const GuildMember& guildMember, Song& song) {
		song.addedByUserId = guildMember.id;
		StringWrapper theString = guildMember.userName;
		song.addedByUserName = theString;
		getSongAPIMap()[guildMember.guildId]->playlist.songQueue.emplace_back(song);
		return song;
	}

	void SongAPI::setPlaylist(const Playlist& playlistNew, const Snowflake guildId) {
		getSongAPIMap()[guildId]->playlist = playlistNew;
	}

	Playlist SongAPI::getPlaylist(const Snowflake guildId) {
		return getSongAPIMap()[guildId]->playlist;
	}

	void SongAPI::modifyQueue(int32_t firstSongPosition, int32_t secondSongPosition, const Snowflake guildId) {
		Song tempSong = getSongAPIMap()[guildId]->playlist.songQueue[firstSongPosition];
		getSongAPIMap()[guildId]->playlist.songQueue[firstSongPosition] = getSongAPIMap()[guildId]->playlist.songQueue[secondSongPosition];
		getSongAPIMap()[guildId]->playlist.songQueue[secondSongPosition] = tempSong;
	}

	Song SongAPI::getCurrentSong(const Snowflake guildId) {
		if (getSongAPIMap()[guildId]->playlist.currentSong.songId != "") {
			return getSongAPIMap()[guildId]->playlist.currentSong;
		} else if (getSongAPIMap()[guildId]->playlist.songQueue.size() > 0) {
			return getSongAPIMap()[guildId]->playlist.songQueue[0];
		} else {
			return Song();
		};
	}

	void SongAPI::setCurrentSong(const Song& song, const Snowflake guildId) {
		getSongAPIMap()[guildId]->playlist.currentSong = song;
	}

	void SongAPI::sendNextSongFinal(const GuildMember& guildMember) {
		getSongAPIMap()[guildMember.guildId]->cancelCurrentSong();
		if (getSongAPIMap()[guildMember.guildId]->playlist.currentSong.type == SongType::SoundCloud) {
			Song newerSong = getSoundCloudAPIMap()[guildMember.guildId]->collectFinalSong(getSongAPIMap()[guildMember.guildId]->playlist.currentSong);
			getSongAPIMap()[this->guildId]->taskThread = std::make_unique<std::jthread>([=, this](std::stop_token eventToken) {
				getSoundCloudAPIMap()[this->guildId]->downloadAndStreamAudio(newerSong, eventToken, 0);
			});

		} else if (getSongAPIMap()[guildMember.guildId]->playlist.currentSong.type == SongType::YouTube) {
			Song newerSong = getYouTubeAPIMap()[guildMember.guildId]->collectFinalSong(getSongAPIMap()[guildMember.guildId]->playlist.currentSong);
			getSongAPIMap()[this->guildId]->taskThread = std::make_unique<std::jthread>([=, this](std::stop_token eventToken) {
				getYouTubeAPIMap()[this->guildId]->downloadAndStreamAudio(newerSong, eventToken, 0);
			});
		};
	}

	bool SongAPI::sendNextSong(const GuildMember& guildMember) {
		std::unique_lock accessLock{ SongAPI::accessMutex };
		if (!getSongAPIMap().contains(guildMember.guildId)) {
			getSongAPIMap()[guildMember.guildId] = std::make_unique<SongAPI>(guildMember.guildId);
		}
		getSongAPIMap()[guildMember.guildId]->sendNextSong();
		if (getSongAPIMap()[guildMember.guildId]->playlist.currentSong.songId == "") {
			getSongAPIMap()[guildMember.guildId]->sendNextSong();
		}
		getSongAPIMap()[guildMember.guildId]->sendNextSongFinal(guildMember);
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
		if (DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]) {
			while (DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.tryReceive(dataFrame)) {
			};
		}
	}

	std::mutex SongAPI::accessMutex{};
};