// SongAPI.cpp - Source file for the song api related stuff.
// Sep 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include <SongAPI.hpp>
#include <DiscordCoreClient.hpp>
#include <VoiceConnection.hpp>
#include <SoundCloudAPI.hpp>
#include <YouTubeAPI.hpp>

namespace DiscordCoreAPI {

	SongAPI::SongAPI(std::string guildId) {
		this->guildId = guildId;
	}

	void SongAPI::onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, std::string guildId) {
		auto returnValue = getSongAPIMap()->at(guildId).get();
		returnValue->onSongCompletionEvent = handler;
	}

	bool SongAPI::sendNextSong(std::string guildId) {
		if (getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled) {
			if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() > 1 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
				for (int32_t x = 0; x < getSongAPIMap()->at(guildId)->playlist.songQueue.size(); x += 1) {
					if (x == getSongAPIMap()->at(guildId)->playlist.songQueue.size() - 1) {
						break;
					}
					getSongAPIMap()->at(guildId)->playlist.songQueue[x] = getSongAPIMap()->at(guildId)->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				getSongAPIMap()->at(guildId)->playlist.songQueue.erase(getSongAPIMap()->at(guildId)->playlist.songQueue.end() - 1, getSongAPIMap()->at(guildId)->playlist.songQueue.end());
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() > 0 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.currentSong;
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.currentSong.songId != "" && getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 0) {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.currentSong;
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 1 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
				getSongAPIMap()->at(guildId)->playlist.songQueue.erase(getSongAPIMap()->at(guildId)->playlist.songQueue.begin(), getSongAPIMap()->at(guildId)->playlist.songQueue.begin() + 1);
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				return false;
			}
		}
		else if (getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled) {
			if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() > 1 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
				for (int32_t x = 0; x < getSongAPIMap()->at(guildId)->playlist.songQueue.size(); x += 1) {
					if (x == getSongAPIMap()->at(guildId)->playlist.songQueue.size() - 1) {
						break;
					}
					getSongAPIMap()->at(guildId)->playlist.songQueue[x] = getSongAPIMap()->at(guildId)->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				getSongAPIMap()->at(guildId)->playlist.songQueue.erase(getSongAPIMap()->at(guildId)->playlist.songQueue.end() - 1, getSongAPIMap()->at(guildId)->playlist.songQueue.end());
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() > 0 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId != "") {
				Song tempSong02 = getSongAPIMap()->at(guildId)->playlist.currentSong;
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
				for (int32_t x = 0; x < getSongAPIMap()->at(guildId)->playlist.songQueue.size(); x += 1) {
					if (x == getSongAPIMap()->at(guildId)->playlist.songQueue.size() - 1) {
						break;
					}
					getSongAPIMap()->at(guildId)->playlist.songQueue[x] = getSongAPIMap()->at(guildId)->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				getSongAPIMap()->at(guildId)->playlist.songQueue.at(getSongAPIMap()->at(guildId)->playlist.songQueue.size() - 1) = tempSong02;
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.currentSong.songId != "" && getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 0) {
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 1 && getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
				getSongAPIMap()->at(guildId)->playlist.songQueue.erase(getSongAPIMap()->at(guildId)->playlist.songQueue.begin(), getSongAPIMap()->at(guildId)->playlist.songQueue.begin() + 1);
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
				return false;
			}
		}
		else {
			if (getSongAPIMap()->at(guildId)->playlist.songQueue.size() > 0 && (getSongAPIMap()->at(guildId)->playlist.currentSong.songId != "" || getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "")) {
				getSongAPIMap()->at(guildId)->playlist.currentSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(0);
				for (int32_t x = 0; x < getSongAPIMap()->at(guildId)->playlist.songQueue.size() - 1; x += 1) {
					getSongAPIMap()->at(guildId)->playlist.songQueue[x] = getSongAPIMap()->at(guildId)->playlist.songQueue[static_cast<int64_t>(x + static_cast<int64_t>(1))];
				}
				getSongAPIMap()->at(guildId)->playlist.songQueue.erase(getSongAPIMap()->at(guildId)->playlist.songQueue.end() - 1, getSongAPIMap()->at(guildId)->playlist.songQueue.end());
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.currentSong.description != "" && getSongAPIMap()->at(guildId)->playlist.songQueue.size() == 0) {
				getSongAPIMap()->at(guildId)->playlist.currentSong = Song();
				return true;
			}
			else if (getSongAPIMap()->at(guildId)->playlist.currentSong.songId == "") {
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
		frameData.type = AudioFrameType::Skip;
		frameData.guildMemberId = guildMember.user.id;
		getAudioBufferMap()->at(guildMember.guildId)->send(frameData);
	}

	void SongAPI::stop(std::string guildId) {
		getVoiceConnectionMap()->at(guildId)->stop();
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
		auto resultValue = getSongAPIMap()->at(guildId).get();
		if (resultValue != nullptr) {
			getSongAPIMap()->at(guildId)->onSongCompletionEvent = std::function<CoRoutine<void>(SongCompletionEventData)>{};
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
		if (!getSongAPIMap()->contains(guildId)) {
			getSongAPIMap()->insert_or_assign(guildId, std::make_unique<SongAPI>(guildId));
		}
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

	void SongAPI::sendNextSongFinal(GuildMember guildMember) {
		try {
			if (getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong.type == SongType::SoundCloud) {
				getSoundCloudAPIMap()->at(guildMember.guildId)->stop();
				auto newerSong = getSoundCloudAPIMap()->at(guildMember.guildId)->collectFinalSong(guildMember, getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong);
				newerSong.addedByUserId = guildMember.user.id;
				getSongAPIMap()->at(this->guildId)->theTask = std::make_unique<CoRoutine<void>>(getSoundCloudAPIMap()->at(this->guildId)->downloadAndStreamAudio(newerSong, getSoundCloudAPIMap()->at(this->guildId).get()));
			}
			else if (getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong.type == SongType::YouTube) {
				getSoundCloudAPIMap()->at(guildMember.guildId)->stop();
				auto newerSong = getSoundCloudAPIMap()->at(guildMember.guildId)->collectFinalSong(guildMember, getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong);
				newerSong.addedByUserId = guildMember.user.id;
				getSongAPIMap()->at(this->guildId)->theTask = std::make_unique<CoRoutine<void>>(getYouTubeAPIMap()->at(this->guildId)->downloadAndStreamAudio(newerSong, getYouTubeAPIMap()->at(this->guildId).get()));
			}
			
		}
		catch (...) {
			reportException("SoundCloudAPI::sendNextSong()");
		}
	}

	bool SongAPI::sendNextSong(GuildMember guildMember) {
		std::lock_guard<std::mutex> accessLock{ SongAPI::accessMutex };
		getSongAPIMap()->at(guildMember.guildId)->sendNextSong(guildMember.guildId);
		if (getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong.songId == "") {
			if (!getSongAPIMap()->at(guildMember.guildId)->sendNextSong(guildMember.guildId)) {
				return false;
			};
		}
		try {
			getSongAPIMap()->at(guildMember.guildId)->sendNextSongFinal(guildMember);
			return true;
		}
		catch (...) {
			reportException("SongAPI::sendNextSong()");
			SongCompletionEventData eventData{ .voiceConnection = getVoiceConnectionMap()->at(guildMember.guildId).get(), .wasItAFail = true, .previousSong = getSongAPIMap()->at(guildMember.guildId)->playlist.currentSong };
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
			if (!getSongAPIMap()->at(guildMember.guildId)->sendNextSong(guildMember.guildId)) {
				return false;
			};
			getSongAPIMap()->at(guildMember.guildId)->onSongCompletionEvent(eventData);
		}
		return false;		
	}

	std::mutex SongAPI::accessMutex{};
};