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

	Void SongAPI::onSongCompletion(std::function<CoRoutine<Void>(SongCompletionEventData)> handler, const Snowflake guildId) {
		SongAPI* returnValue = DiscordCoreClient::getSongAPI(guildId);
		returnValue->onSongCompletionEvent.remove(returnValue->eventToken);
		returnValue->eventToken = returnValue->onSongCompletionEvent.add(DiscordCoreInternal::EventDelegate<CoRoutine<Void>, SongCompletionEventData>{ handler });
	}

	Bool SongAPI::sendNextSong() {
		if (this->playlist.isLoopSongEnabled) {
			if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (Int32 x = 0; x < this->playlist.songQueue.size(); ++x) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<Int64>(x + static_cast<Int64>(1))];
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
				for (Int32 x = 0; x < this->playlist.songQueue.size(); ++x) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<Int64>(x + static_cast<Int64>(1))];
				}
				this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
				return true;
			} else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId != "") {
				Song tempSong02 = this->playlist.currentSong;
				this->playlist.currentSong = this->playlist.songQueue[0];
				for (Int32 x = 0; x < this->playlist.songQueue.size(); ++x) {
					if (x == this->playlist.songQueue.size() - 1) {
						break;
					}
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<Int64>(x + static_cast<Int64>(1))];
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
				for (Int32 x = 0; x < this->playlist.songQueue.size() - 1; ++x) {
					this->playlist.songQueue[x] = this->playlist.songQueue[static_cast<Int64>(x + static_cast<Int64>(1))];
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

	Bool SongAPI::play(const Snowflake guildId) {
		return DiscordCoreClient::getVoiceConnection(guildId)->play();
	}

	Void SongAPI::pauseToggle(const Snowflake guildId) {
		DiscordCoreClient::getVoiceConnection(guildId)->pauseToggle();
	}

	Bool SongAPI::areWeCurrentlyPlaying(const Snowflake guildId) {
		return DiscordCoreClient::getVoiceConnection(guildId)->areWeCurrentlyPlaying();
	}

	Void SongAPI::skip(const GuildMember& guildMember) {
		DiscordCoreClient::getSongAPI(guildMember.guildId)->cancelCurrentSong();
		if (SongAPI::isLoopAllEnabled(guildMember.guildId) || SongAPI::isLoopSongEnabled(guildMember.guildId)) {
			DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.songQueue.emplace_back(DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.currentSong);
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		} else {
			SongAPI::setCurrentSong(Song(), guildMember.guildId);
		}
		AudioFrameData frameData{};
		while (DiscordCoreClient::getSongAPI(guildMember.guildId)->audioDataBuffer.tryReceive(frameData)) {
		};
		frameData.type = AudioFrameType::Skip;
		frameData.guildMemberId = static_cast<GuildMember>(guildMember).id;
		DiscordCoreClient::getSongAPI(guildMember.guildId)->audioDataBuffer.send(frameData);
	}

	Void SongAPI::stop(const Snowflake guildId) {
		DiscordCoreClient::getVoiceConnection(guildId)->stop();
		DiscordCoreClient::getSongAPI(guildId)->cancelCurrentSong();
		Vector<Song> newVector02;
		newVector02.emplace_back(DiscordCoreClient::getSongAPI(guildId)->playlist.currentSong);
		for (auto& value: DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue) {
			newVector02.emplace_back(value);
		}
		DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue = newVector02;
		auto returnValue = DiscordCoreClient::getSongAPI(guildId);
		if (returnValue) {
			DiscordCoreClient::getSongAPI(guildId)->onSongCompletionEvent.remove(DiscordCoreClient::getSongAPI(guildId)->eventToken);
			DiscordCoreClient::getSongAPI(guildId)->eventToken = DiscordCoreInternal::EventDelegateToken{};
		}
	}

	Vector<Song> SongAPI::searchForSong(const String& searchQuery, const Snowflake guildId) {
		auto vector01 = DiscordCoreClient::getSoundCloudAPI(guildId)->searchForSong(searchQuery);
		auto vector02 = DiscordCoreClient::getYouTubeAPI(guildId)->searchForSong(searchQuery);
		Int32 totalLength = static_cast<Int32>(vector01.size() + vector02.size());
		Vector<Song> newVector{};
		Int32 vector01Used{ 0 };
		Int32 vector02Used{ 0 };
		for (Int32 x = 0; x < totalLength; ++x) {
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

	Void SongAPI::setLoopAllStatus(Bool enabled, const Snowflake guildId) {
		DiscordCoreClient::getSongAPI(guildId)->playlist.isLoopAllEnabled = enabled;
	}

	Bool SongAPI::isLoopAllEnabled(const Snowflake guildId) {
		return DiscordCoreClient::getSongAPI(guildId)->playlist.isLoopAllEnabled;
	}

	Void SongAPI::setLoopSongStatus(Bool enabled, const Snowflake guildId) {
		DiscordCoreClient::getSongAPI(guildId)->playlist.isLoopSongEnabled = enabled;
	}

	Bool SongAPI::isLoopSongEnabled(const Snowflake guildId) {
		return DiscordCoreClient::getSongAPI(guildId)->playlist.isLoopSongEnabled;
	}

	Bool SongAPI::isThereAnySongs(const Snowflake guildId) {
		if (DiscordCoreClient::getSongAPI(guildId)->playlist.isLoopAllEnabled || DiscordCoreClient::getSongAPI(guildId)->playlist.isLoopSongEnabled) {
			if (DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue.size() == 0 && DiscordCoreClient::getSongAPI(guildId)->playlist.currentSong.songId == "") {
				return false;
			} else {
				return true;
			}
		} else {
			if (DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue.size() == 0) {
				return false;
			} else {
				return true;
			}
		}
	}

	Song SongAPI::addSongToQueue(const GuildMember& guildMember, Song& song) {
		song.addedByUserId = guildMember.id;
		String theString{ (( GuildMember& )guildMember).getUserData().userName };
		song.addedByUserName = theString;
		DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.songQueue.emplace_back(song);
		return song;
	}

	Void SongAPI::setPlaylist(const Playlist& playlistNew, const Snowflake guildId) {
		DiscordCoreClient::getSongAPI(guildId)->playlist = playlistNew;
	}

	Playlist SongAPI::getPlaylist(const Snowflake guildId) {
		return DiscordCoreClient::getSongAPI(guildId)->playlist;
	}

	Void SongAPI::modifyQueue(Int32 firstSongPosition, Int32 secondSongPosition, const Snowflake guildId) {
		Song tempSong = DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue[firstSongPosition];
		DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue[firstSongPosition] = DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue[secondSongPosition];
		DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue[secondSongPosition] = tempSong;
	}

	Song SongAPI::getCurrentSong(const Snowflake guildId) {
		if (DiscordCoreClient::getSongAPI(guildId)->playlist.currentSong.songId != "") {
			return DiscordCoreClient::getSongAPI(guildId)->playlist.currentSong;
		} else if (DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue.size() > 0) {
			return DiscordCoreClient::getSongAPI(guildId)->playlist.songQueue[0];
		} else {
			return Song();
		};
	}

	Void SongAPI::setCurrentSong(const Song& song, const Snowflake guildId) {
		DiscordCoreClient::getSongAPI(guildId)->playlist.currentSong = song;
	}

	Void SongAPI::sendNextSongFinal(const GuildMember& guildMember) {
		DiscordCoreClient::getSongAPI(guildMember.guildId)->cancelCurrentSong();
		if (DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.currentSong.type == SongType::SoundCloud) {
			Song newerSong = DiscordCoreClient::getSoundCloudAPI(guildMember.guildId)->collectFinalSong(DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.currentSong);
			if (DiscordCoreClient::getSongAPI(this->guildId)->taskThread) {
				DiscordCoreClient::getSongAPI(this->guildId)->taskThread->request_stop();
				if (DiscordCoreClient::getSongAPI(this->guildId)->taskThread->joinable()) {
					DiscordCoreClient::getSongAPI(this->guildId)->taskThread->join();
				}
			}
			DiscordCoreClient::getSongAPI(this->guildId)->taskThread = std::make_unique<std::jthread>([=, this](std::stop_token eventToken) {
				DiscordCoreClient::getSoundCloudAPI(this->guildId)->downloadAndStreamAudio(newerSong, eventToken, 0);
			});

		} else if (DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.currentSong.type == SongType::YouTube) {
			Song newerSong = DiscordCoreClient::getYouTubeAPI(guildMember.guildId)->collectFinalSong(DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.currentSong);
			if (DiscordCoreClient::getSongAPI(this->guildId)->taskThread) {
				DiscordCoreClient::getSongAPI(this->guildId)->taskThread->request_stop();
				if (DiscordCoreClient::getSongAPI(this->guildId)->taskThread->joinable()) {
					DiscordCoreClient::getSongAPI(this->guildId)->taskThread->join();
				}
			}
			DiscordCoreClient::getSongAPI(this->guildId)->taskThread = std::make_unique<std::jthread>([=, this](std::stop_token eventToken) {
				DiscordCoreClient::getYouTubeAPI(this->guildId)->downloadAndStreamAudio(newerSong, eventToken, 0);
			});
		};
	}

	Bool SongAPI::sendNextSong(const GuildMember& guildMember) {
		std::unique_lock accessLock{ SongAPI::accessMutex };
		DiscordCoreClient::getSongAPI(guildMember.guildId)->sendNextSong();
		if (DiscordCoreClient::getSongAPI(guildMember.guildId)->playlist.currentSong.songId == "") {
			DiscordCoreClient::getSongAPI(guildMember.guildId)->sendNextSong();
		}
		DiscordCoreClient::getSongAPI(guildMember.guildId)->sendNextSongFinal(guildMember);
		return true;
	}

	Void SongAPI::cancelCurrentSong() {
		if (this->taskThread) {
			this->taskThread->request_stop();
			if (this->taskThread->joinable()) {
				this->taskThread->join();
			}
			this->taskThread.reset(nullptr);
		}
		DiscordCoreAPI::AudioFrameData dataFrame{};
		while (DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.tryReceive(dataFrame)) {
		};
	}

	std::mutex SongAPI::accessMutex{};
};