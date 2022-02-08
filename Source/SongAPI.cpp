// SongAPI.cpp - Source file for the song api related stuff.
// Sep 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "SongAPI.hpp"
#include "DiscordCoreClient.hpp"
#include "VoiceConnection.hpp"
#include "SoundCloudAPI.hpp"
#include "YouTubeAPI.hpp"

namespace DiscordCoreAPI {

	SongAPI::SongAPI(std::string guildId) {
		this->guildId = guildId;
	}

	EventDelegateToken SongAPI::onSongCompletion(EventDelegate<CoRoutine<void>, SongCompletionEventData> handler, std::string guildId) {
		auto returnValue = getSongAPIMap()->at(guildId).get();
		if (returnValue != nullptr) {
			if (!returnValue->areWeCurrentlyPlaying(guildId)) {
				auto theToken = getSongAPIMap()->at(guildId)->onSongCompletionEvent.add(std::move(handler));
				getSongAPIMap()->at(guildId)->eventDelegateToken = theToken;
				return theToken;
			}
		}
		return EventDelegateToken{};
	}

	void SongAPI::onSongCompletion(EventDelegateToken token, std::string guildId) {
		getSongAPIMap()->at(guildId)->onSongCompletionEvent.remove(token);
	}

	bool SongAPI::sendNextSong() {
		this->playlist = getSongAPIMap()->at(this->guildId)->playlist;
		if (this->playlist.isLoopSongEnabled) {
			if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue.at(0);
				for (int32_t x = 0; x < this->playlist.songQueue.size(); x += 1) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			}
			else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.currentSong;
				return true;
			}
			else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
				this->playlist.currentSong = this->playlist.currentSong;
				return true;
			}
			else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue.at(0);
				this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
				return true;
			}
			else if (this->playlist.currentSong.songId == "") {
				return false;
			}
		}
		else if (this->playlist.isLoopAllEnabled) {
			if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue.at(0);
				for (int32_t x = 0; x < this->playlist.songQueue.size(); x += 1) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			}
			else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId != "") {
				Song tempSong02 = this->playlist.currentSong;
				this->playlist.currentSong = this->playlist.songQueue.at(0);
				for (int32_t x = 0; x < this->playlist.songQueue.size(); x += 1) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.at(this->playlist.songQueue.size() - 1) = tempSong02;
				return true;
			}
			else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
				return true;
			}
			else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue.at(0);
				this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
				return true;
			}
			else if (this->playlist.currentSong.songId == "") {
				return false;
			}
		}
		else {
			if (this->playlist.songQueue.size() > 0 && (this->playlist.currentSong.songId != "" || this->playlist.currentSong.songId == "")) {
				this->playlist.currentSong = this->playlist.songQueue.at(0);
				for (int32_t x = 0; x < this->playlist.songQueue.size() - 1; x += 1) {
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			}
			else if (this->playlist.currentSong.description != "" && this->playlist.songQueue.size() == 0) {
				this->playlist.currentSong = Song();
				return true;
			}
			else if (this->playlist.currentSong.songId == "") {
				return false;
			}
		}
		return false;
	}

	bool SongAPI::play(std::string guildId) {
		return getVoiceConnectionMap()->at(guildId)->play();
	}

	void SongAPI::pauseToggle(std::string guildId) {
		getVoiceConnectionMap()->at(guildId)->pauseToggle();
	}

	bool SongAPI::areWeCurrentlyPlaying(std::string guildId) {
		return getVoiceConnectionMap()->at(guildId)->areWeCurrentlyPlaying();
	}

	void SongAPI::skip(GuildMember guildMember) {
		if (SongAPI::getCurrentSong(guildMember.guildId).type == SongType::SoundCloud) {
			getSoundCloudAPIMap()->at(guildMember.guildId)->stop();
		}
		else {
			getYouTubeAPIMap()->at(guildMember.guildId)->stop();
		}
		if (SongAPI::isLoopAllEnabled(guildMember.guildId) || SongAPI::isLoopSongEnabled(guildMember.guildId)) {
			getSongAPIMap()->at(guildMember.guildId)->playlist.songQueue.push_back(getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong);
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		}
		else {
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		}
		AudioFrameData frameData{};
		while (getAudioBufferMap()->at(guildMember.guildId)->tryReceive(frameData)) {};
		frameData.type = AudioFrameType::Cancel;
		getAudioBufferMap()->at(guildMember.guildId)->send(frameData);
	}

	void SongAPI::stop(std::string guildId) {
		if (SongAPI::getCurrentSong(guildId).type == SongType::SoundCloud) {
			getSoundCloudAPIMap()->at(guildId)->stop();
		}
		else {
			getYouTubeAPIMap()->at(guildId)->stop();
		}
		std::vector<Song> newVector02;
		if (getSongAPIMap()->at(guildId)->playlist.currentSong.description != "") {
			newVector02.push_back(getSongAPIMap()->at(guildId)->playlist.currentSong);
			getSongAPIMap()->at(guildId)->playlist.currentSong = Song();
		}
		for (auto& value : getSongAPIMap()->at(guildId)->playlist.songQueue) {
			newVector02.push_back(value);
		}
		getSongAPIMap()->at(guildId)->playlist.songQueue = newVector02;
		getVoiceConnectionMap()->at(guildId)->stop();
		auto resultValue = getSongAPIMap()->at(guildId).get();
		if (resultValue != nullptr) {
			resultValue->onSongCompletion(getSongAPIMap()->at(guildId)->eventDelegateToken, guildId);
		}
	}

	std::vector<Song> SongAPI::searchForSong(std::string searchQuery, std::string guildId) {
		auto vector01 = getSoundCloudAPIMap()->at(guildId)->searchForSong(searchQuery, guildId);
		auto vector02 = getYouTubeAPIMap()->at(guildId)->searchForSong(searchQuery, guildId);
		int32_t totalLength = static_cast<int32_t>(vector01.size() + vector02.size());
		std::vector<Song> newVector{};
		int32_t vector01Used{ 0 };
		int32_t vector02Used{ 0 };
		for (int32_t x = 0; x < totalLength; x += 1) {
			if ((vector01Used < vector01.size() - 1) && (x % 2 == 0) && vector01.size() > 0) {
				vector01[vector01Used].type = SongType::SoundCloud;
				newVector.push_back(vector01[vector01Used]);
				vector01Used += 1;
			}
			else if (vector02Used < vector02.size() - 1 && vector02.size()>0) {
				vector02[vector02Used].type = SongType::YouTube;
				newVector.push_back(vector02[vector02Used]);
				vector02Used += 1;
			}
		}
		return newVector;
	}

	void SongAPI::setLoopAllStatus(bool enabled, std::string guildId) {
		getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled = enabled;
	}

	bool SongAPI::isLoopAllEnabled(std::string guildId) {
		return getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled;
	}

	void SongAPI::setLoopSongStatus(bool enabled, std::string guildId) {
		getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled = enabled;
	}

	bool SongAPI::isLoopSongEnabled(std::string guildId) {
		return getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled;
	}

	bool SongAPI::isThereAnySongs(std::string guildId) {
		if (getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled || getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled) {
			if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 0 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				return false;
			}
			else {
				return true;
			}
		}
		else {
			if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 0) {
				return false;
			}
			else {
				return true;
			}
		}
	}

	Song SongAPI::addSongToQueue(GuildMember guildMember, Song song) {
		song.addedByUserId = guildMember.user.id;
		song.addedByUserName = guildMember.user.userName;
		getSongAPIMap()->at(guildMember.guildId)->playlist.songQueue.push_back(song);
		return song;
	}

	void SongAPI::setPlaylist(Playlist playlistNew, std::string guildId) {
		getSongAPIMap()->at(guildId)->playlist.currentSong = playlistNew.currentSong;
		getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled = playlistNew.isLoopAllEnabled;
		getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled = playlistNew.isLoopSongEnabled;
		getSongAPIMap()->at(guildId)->playlist.songQueue = playlistNew.songQueue;
		getSongAPIMap()->at(guildId)->playlist.currentSong = playlistNew.currentSong;
	}

	Playlist SongAPI::getPlaylist(std::string guildId) {
		return getSongAPIMap()->at(guildId)->playlist;
	}

	void SongAPI::modifyQueue(int32_t firstSongPosition, int32_t secondSongPosition, std::string guildId) {
		Song tempSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(firstSongPosition);
		getSongAPIMap()->at(guildId)->playlist.songQueue.at(firstSongPosition) = getSongAPIMap()->at(guildId)->playlist.songQueue.at(secondSongPosition);
		getSongAPIMap()->at(guildId)->playlist.songQueue.at(secondSongPosition) = tempSong;
	}

	Song SongAPI::getCurrentSong(std::string guildId) {
		if (getSongAPIMap()->at(guildId)->playlist.currentSong.songId != "") {
			return getSongAPIMap()->at(guildId)->playlist.currentSong;
		}
		else if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() > 0) {
			return getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
		}
		else {
			return Song();
		};
	}

	void SongAPI::setCurrentSong(Song song, std::string guildId) {
		getSongAPIMap()->at(guildId)->playlist.currentSong = song;
	}

	bool SongAPI::sendNextSong(GuildMember guildMember) {
		getSongAPIMap()->at(guildMember.guildId)->sendNextSong();
		if (getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong.songId == "") {
			if (!getSongAPIMap()->at(guildMember.guildId)->sendNextSong()) {
				return false;
			};
		}
		try {
			switch (getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong.type) {
			case SongType::SoundCloud: {
				auto newerSong = getSoundCloudAPIMap()->at(guildMember.guildId)->collectFinalSong(guildMember, getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong);
				getSoundCloudAPIMap()->at(guildMember.guildId)->sendNextSong(newerSong);
				return true;
				break;
			}
			case SongType::YouTube: {
				auto newerSong = getYouTubeAPIMap()->at(guildMember.guildId)->collectFinalSong(guildMember, getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong);
				getYouTubeAPIMap()->at(guildMember.guildId)->sendNextSong(newerSong);
				return true;
				break;
			}
			} 
		}
		catch (...) {
			reportException("SongAPI::sendNextSong()");
			SongCompletionEventData eventData{ .voiceConnection = getVoiceConnectionMap()->at(guildMember.guildId).get(), .wasItAFail = true, .previousSong = getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong };
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
			if (!getSongAPIMap()->at(guildMember.guildId)->sendNextSong()) {
				return false;
			};
			getSongAPIMap()->at(guildMember.guildId)->onSongCompletionEvent(eventData);
		}
		return false;
	}
};