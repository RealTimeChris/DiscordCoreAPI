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
/// SongAPI.cpp - Source file for the song api related stuff.
/// Sep 17, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file SongAPI.cpp

#include <discordcoreapi/SongAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/YouTubeAPI.hpp>

namespace DiscordCoreAPI {

	SongAPI::SongAPI(const std::string& guildId) {
		this->guildId = guildId;
	}

	void SongAPI::onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, const std::string& guildId) {
		auto returnValue = getSongAPIMap()[guildId].get();
		returnValue->onSongCompletionEvent.remove(returnValue->theToken);
		returnValue->theToken = returnValue->onSongCompletionEvent.add(handler);
	}

	bool SongAPI::sendNextSong() {
		if (this->playlist.isLoopSongEnabled) {
			if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (int32_t x = 0; x < this->playlist.songQueue.size(); x += 1) {
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
				for (int32_t x = 0; x < this->playlist.songQueue.size(); x += 1) {
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
				for (int32_t x = 0; x < this->playlist.songQueue.size(); x += 1) {
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
				for (int32_t x = 0; x < this->playlist.songQueue.size() - 1; x += 1) {
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

	bool SongAPI::play(const std::string& guildId) {
		return getVoiceConnectionMap()[guildId]->play();
	}

	void SongAPI::pauseToggle(const std::string& guildId) {
		getVoiceConnectionMap()[guildId]->pauseToggle();
	}

	bool SongAPI::areWeCurrentlyPlaying(const std::string& guildId) {
		return getVoiceConnectionMap()[guildId]->areWeCurrentlyPlaying();
	}

	void SongAPI::skip(const GuildMember& guildMember) {
		if (SongAPI::getCurrentSong(guildMember.guildId).type == SongType::SoundCloud) {
			getSoundCloudAPIMap()[guildMember.guildId]->cancelCurrentSong();
		} else {
			getYouTubeAPIMap()[guildMember.guildId]->cancelCurrentSong();
		}
		if (SongAPI::isLoopAllEnabled(guildMember.guildId) || SongAPI::isLoopSongEnabled(guildMember.guildId)) {
			getSongAPIMap()[guildMember.guildId]->playlist.songQueue.push_back(getSongAPIMap()[guildMember.guildId]->playlist.currentSong);
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		} else {
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		}
		AudioFrameData frameData{};
		while (getVoiceConnectionMap()[guildMember.guildId]->audioBuffer.tryReceive(frameData)) {
		};
		frameData.type = AudioFrameType::Skip;
		frameData.guildMemberId = guildMember.user.id;
		getVoiceConnectionMap()[guildMember.guildId]->audioBuffer.send(frameData);
	}

	void SongAPI::stop(const std::string& guildId) {
		getVoiceConnectionMap()[guildId]->stop();
		if (SongAPI::getCurrentSong(guildId).type == SongType::SoundCloud) {
			getSoundCloudAPIMap()[guildId]->cancelCurrentSong();
		} else {
			getYouTubeAPIMap()[guildId]->cancelCurrentSong();
		}
		std::vector<Song> newVector02;
		if (getSongAPIMap()[guildId]->playlist.currentSong.description != "") {
			newVector02.push_back(getSongAPIMap()[guildId]->playlist.currentSong);
			getSongAPIMap()[guildId]->playlist.currentSong = Song();
		}
		for (auto& value: getSongAPIMap()[guildId]->playlist.songQueue) {
			newVector02.push_back(value);
		}
		getSongAPIMap()[guildId]->playlist.songQueue = newVector02;
		auto resultValue = getSongAPIMap()[guildId].get();
		if (resultValue) {
			getSongAPIMap()[guildId]->onSongCompletionEvent.remove(getSongAPIMap()[guildId]->theToken);
			getSongAPIMap()[guildId]->theToken = DiscordCoreInternal::EventDelegateToken{};
		}
	}

	std::vector<Song> SongAPI::searchForSong(const std::string& searchQuery, const std::string& guildId) {
		auto vector01 = getSoundCloudAPIMap()[guildId]->searchForSong(searchQuery);
		auto vector02 = getYouTubeAPIMap()[guildId]->searchForSong(searchQuery);
		int32_t totalLength = static_cast<int32_t>(vector01.size() + vector02.size());
		std::vector<Song> newVector{};
		int32_t vector01Used{ 0 };
		int32_t vector02Used{ 0 };
		for (int32_t x = 0; x < totalLength; x += 1) {
			if ((vector01Used < vector01.size() - 1) && (x % 2 == 0) && vector01.size() > 0) {
				vector01[vector01Used].type = SongType::SoundCloud;
				newVector.push_back(vector01[vector01Used]);
				vector01Used += 1;
			} else if (vector02Used < vector02.size() - 1 && vector02.size() > 0) {
				vector02[vector02Used].type = SongType::YouTube;
				newVector.push_back(vector02[vector02Used]);
				vector02Used += 1;
			}
		}
		return newVector;
	}

	void SongAPI::setLoopAllStatus(bool enabled, const std::string& guildId) {
		getSongAPIMap()[guildId]->playlist.isLoopAllEnabled = enabled;
	}

	bool SongAPI::isLoopAllEnabled(const std::string& guildId) {
		return getSongAPIMap()[guildId]->playlist.isLoopAllEnabled;
	}

	void SongAPI::setLoopSongStatus(bool enabled, const std::string& guildId) {
		getSongAPIMap()[guildId]->playlist.isLoopSongEnabled = enabled;
	}

	bool SongAPI::isLoopSongEnabled(const std::string& guildId) {
		return getSongAPIMap()[guildId]->playlist.isLoopSongEnabled;
	}

	bool SongAPI::isThereAnySongs(const std::string& guildId) {
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
		song.addedByUserId = guildMember.user.id;
		song.addedByUserName = guildMember.user.username;
		getSongAPIMap()[guildMember.guildId]->playlist.songQueue.push_back(song);
		return song;
	}

	void SongAPI::setPlaylist(const Playlist& playlistNew, const std::string& guildId) {
		getSongAPIMap()[guildId]->playlist.currentSong = playlistNew.currentSong;
		getSongAPIMap()[guildId]->playlist.isLoopAllEnabled = playlistNew.isLoopAllEnabled;
		getSongAPIMap()[guildId]->playlist.isLoopSongEnabled = playlistNew.isLoopSongEnabled;
		getSongAPIMap()[guildId]->playlist.songQueue = playlistNew.songQueue;
		getSongAPIMap()[guildId]->playlist.currentSong = playlistNew.currentSong;
	}

	Playlist SongAPI::getPlaylist(const std::string& guildId) {
		return getSongAPIMap()[guildId]->playlist;
	}

	void SongAPI::modifyQueue(int32_t firstSongPosition, int32_t secondSongPosition, const std::string& guildId) {
		Song tempSong = getSongAPIMap()[guildId]->playlist.songQueue[firstSongPosition];
		getSongAPIMap()[guildId]->playlist.songQueue[firstSongPosition] = getSongAPIMap()[guildId]->playlist.songQueue[secondSongPosition];
		getSongAPIMap()[guildId]->playlist.songQueue[secondSongPosition] = tempSong;
	}

	Song SongAPI::getCurrentSong(const std::string& guildId) {
		if (getSongAPIMap()[guildId]->playlist.currentSong.songId != "") {
			return getSongAPIMap()[guildId]->playlist.currentSong;
		} else if (getSongAPIMap()[guildId]->playlist.songQueue.size() > 0) {
			return getSongAPIMap()[guildId]->playlist.songQueue[0];
		} else {
			return Song();
		};
	}

	void SongAPI::setCurrentSong(const Song& song, const std::string& guildId) {
		getSongAPIMap()[guildId]->playlist.currentSong = song;
	}

	void SongAPI::sendNextSongFinal(const GuildMember& guildMember) {
		if (getSongAPIMap()[guildMember.guildId]->playlist.currentSong.type == SongType::SoundCloud) {
			getSoundCloudAPIMap()[guildMember.guildId]->cancelCurrentSong();
			auto newerSong = getSoundCloudAPIMap()[guildMember.guildId]->collectFinalSong(guildMember, getSongAPIMap()[guildMember.guildId]->playlist.currentSong);
			newerSong.addedByUserId = guildMember.user.id;
			getSongAPIMap()[this->guildId]->theTask = std::make_unique<std::jthread>([=, this](std::stop_token theToken) {
				getSoundCloudAPIMap()[this->guildId]->downloadAndStreamAudio(newerSong, getSoundCloudAPIMap()[this->guildId].get(), theToken, 0);
			});

		} else if (getSongAPIMap()[guildMember.guildId]->playlist.currentSong.type == SongType::YouTube) {
			getYouTubeAPIMap()[guildMember.guildId]->cancelCurrentSong();
			Song newerSong = getYouTubeAPIMap()[guildMember.guildId]->collectFinalSong(guildMember, getSongAPIMap()[guildMember.guildId]->playlist.currentSong);
			newerSong.addedByUserId = guildMember.user.id;
			getSongAPIMap()[this->guildId]->theTask = std::make_unique<std::jthread>([=, this](std::stop_token theToken) {
				getYouTubeAPIMap()[this->guildId]->downloadAndStreamAudio(newerSong, getYouTubeAPIMap()[this->guildId].get(), theToken, 0);
			});
		};
	}

	bool SongAPI::sendNextSong(const GuildMember& guildMember) {
		std::lock_guard<std::mutex> accessLock{ SongAPI::accessMutex };
		if (!getSongAPIMap().contains(guildMember.guildId)) {
			getSongAPIMap().insert_or_assign(guildMember.guildId, std::make_unique<SongAPI>(guildMember.guildId));
		}
		getSongAPIMap()[guildMember.guildId]->sendNextSong();
		if (getSongAPIMap()[guildMember.guildId]->playlist.currentSong.songId == "") {
			getSongAPIMap()[guildMember.guildId]->sendNextSong();
		}
		try {
			getSongAPIMap()[guildMember.guildId]->sendNextSongFinal(guildMember);
			return true;
		} catch (...) {
			reportException("SongAPI::sendNextSong()");
			SongCompletionEventData eventData{ .wasItAFail = true, .previousSong = getSongAPIMap()[guildMember.guildId]->playlist.currentSong };
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
			if (!getSongAPIMap()[guildMember.guildId]->sendNextSong()) {
				return false;
			};
			getSongAPIMap()[guildMember.guildId]->onSongCompletionEvent(eventData);
		}
		return false;
	}

	SongAPI::~SongAPI() {
		if (this->theTask) {
			this->theTask->request_stop();
			if (this->theTask->joinable()) {
				this->theTask->join();
			}
			this->theTask.reset(nullptr);
		}
	}

	std::mutex SongAPI::accessMutex{};
};