// SongAPI.hpp - Header for the song api related stuff.
// Sep 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SONG_API_
#define _SONG_API_

#include "../DiscordCoreAPI-main/DiscordCoreClient02.hpp"
#include "YouTubeAPI.hpp"
#include "SoundCloudAPI.hpp"
#include "GuildEntities.hpp"

namespace DiscordCoreAPI {

	vector<Song> cleanQueue(vector<Song> originalQueue) {
		auto newQueue = originalQueue;
		for (int x = 0; x < newQueue.size(); x += 1) {
			if (newQueue[x].songId == "") {
				newQueue.erase(newQueue.begin() + x);
			}
		}
		return newQueue;
	}

	class SongAPI {
	public:

		friend class Guild;

		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		DiscordGuild* discordGuild{ new DiscordGuild };
		string guildId{ "" };
		Playlist playlist{};

		SongAPI(DiscordGuild* discordGuildNew) {
			this->sendAudioDataBuffer = getAudioBuffersMap()->at(discordGuildNew->data.guildId);
			this->voiceConnection = getVoiceConnectionMap()->at(discordGuildNew->data.guildId);
			this->guildId = discordGuildNew->data.guildId;
			this->discordGuild = discordGuildNew;
			this->loadPlaylist();
		}

		static void initialize() {
			SoundCloudAPI::initialize(getSoundCloudAPIMap(), getAudioBuffersMap(), getDiscordGuildMap(), getVoiceConnectionMap());
			YouTubeAPI::initialize(getYouTubeAPIMap(), getAudioBuffersMap(), getDiscordGuildMap(), getVoiceConnectionMap());
		}

		void refreshInterfaces() {
			DiscordGuild* discordGuildNew = this->discordGuild;
			getSongAPIMap()->insert_or_assign(discordGuildNew->data.guildId, make_shared<SongAPI>(discordGuildNew));
			YouTubeAPI::youtubeAPIMap->insert_or_assign(discordGuildNew->data.guildId, make_shared<YouTubeAPI>(discordGuildNew->data.guildId));
			SoundCloudAPI::soundCloudAPIMap->insert_or_assign(discordGuildNew->data.guildId, make_shared<SoundCloudAPI>(discordGuildNew->data.guildId));
		}

		void savePlaylist() {
			this->playlist.songQueue = cleanQueue(this->playlist.songQueue);
			this->discordGuild->data.playlist = this->playlist;
			this->discordGuild->writeDataToDB();
		}

		void loadPlaylist() {
			discordGuild->getDataFromDB();
			discordGuild->data.playlist.songList = cleanQueue(discordGuild->data.playlist.songList);
			this->playlist = discordGuild->data.playlist;
		}

		bool sendNextSong() {
			this->loadPlaylist();
			if (this->playlist.isLoopSongEnabled) {
				if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					for (int x = 0; x < this->playlist.songQueue.size(); x += 1) {
						if (x == this->playlist.songQueue.size() - 1) {
							break;
						}
						this->playlist.songQueue[x] = this->playlist.songQueue[x + 1];
					}
					this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.currentSong;
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
					this->playlist.currentSong = this->playlist.currentSong;
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.currentSong.songId == "") {
					this->savePlaylist();
					return false;
				}
			}
			else if (this->playlist.isLoopAllEnabled) {
				if (this->playlist.songQueue.size() > 1 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					for (int x = 0; x < this->playlist.songQueue.size(); x += 1) {
						if (x == this->playlist.songQueue.size() - 1) {
							break;
						}
						this->playlist.songQueue[x] = this->playlist.songQueue[x + 1];
					}
					this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId != "") {
					auto tempSong02 = this->playlist.currentSong;
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					for (int x = 0; x < this->playlist.songQueue.size(); x += 1) {
						if (x == this->playlist.songQueue.size() - 1) {
							break;
						}
						this->playlist.songQueue[x] = this->playlist.songQueue[x + 1];
					}
					this->playlist.songQueue.at(this->playlist.songQueue.size() - 1) = tempSong02;
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.currentSong.songId == "") {
					this->savePlaylist();
					return false;
				}
			}
			else {
				if (this->playlist.songQueue.size() > 0 && (this->playlist.currentSong.songId != "" || this->playlist.currentSong.songId == "")) {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					for (int x = 0; x < this->playlist.songQueue.size() - 1; x += 1) {
						this->playlist.songQueue[x] = this->playlist.songQueue[x + 1];
					}
					this->playlist.songQueue.erase(this->playlist.songQueue.end() - 1, this->playlist.songQueue.end());
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.currentSong.description != "" && this->playlist.songQueue.size() == 0) {
					this->playlist.currentSong = Song();
					this->savePlaylist();
					return true;
				}
				else if (this->playlist.currentSong.songId == "") {
					this->savePlaylist();
					return false;
				}
			}
			return false;
		}

		static bool skip(string guildId, GuildMember guildMember) {
			if (SongAPI::getCurrentSong(guildId).type == SongType::SoundCloud) {
				if (!SoundCloudAPI::stop(guildId)) {
					return false;
				}
			}
			else {
				if (!YouTubeAPI::stop(guildId)) {
					return false;
				}
			}
			getSongAPIMap()->at(guildId)->refreshInterfaces();
			if (SongAPI::isLoopAllEnabled(guildId) || SongAPI::isLoopSongEnabled(guildId)) {
				getSongAPIMap()->at(guildId)->playlist.songQueue.push_back(getSongAPIMap()->at(guildId)->playlist.currentSong);
				SongAPI::setCurrentSong(Song(), guildId);
			}
			else {
				SongAPI::setCurrentSong(Song(), guildId);
			}
			AudioFrameData frameData{};
			while (try_receive(getAudioBuffersMap()->at(guildId).get(), frameData)) {};
			getSongAPIMap()->at(guildId)->savePlaylist();
			getSongAPIMap()->erase(guildId);
			DiscordGuild* discordGuild = getDiscordGuildMap()->at(guildId);
			shared_ptr<SongAPI> songAPICore = make_shared<SongAPI>(discordGuild);
			getSongAPIMap()->insert_or_assign(guildId, songAPICore);
			if (!SongAPI::sendNextSong(guildId, guildMember)) {
				return false;
			};
			return true;
		}

		static bool stop(string guildId) {
			if (SongAPI::getCurrentSong(guildId).type == SongType::SoundCloud) {
				if (!SoundCloudAPI::stop(guildId)) {
					return false;
				}
			}
			else {
				if (!YouTubeAPI::stop(guildId)) {
					return false;
				}
			}
			getSongAPIMap()->at(guildId)->refreshInterfaces();
			vector<Song> newVector02;
			if (getSongAPIMap()->at(guildId)->playlist.currentSong.description != "") {
				newVector02.push_back(getSongAPIMap()->at(guildId)->playlist.currentSong);
				getSongAPIMap()->at(guildId)->playlist.currentSong = Song();
			}
			for (auto value : getSongAPIMap()->at(guildId)->playlist.songQueue) {
				newVector02.push_back(value);
			}
			getSongAPIMap()->at(guildId)->playlist.songQueue = newVector02;
			getSongAPIMap()->at(guildId)->savePlaylist();
			getSongAPIMap()->erase(guildId);
			auto discordGuild = getDiscordGuildMap()->at(guildId);
			shared_ptr<SongAPI> songAPI = make_shared<SongAPI>(discordGuild);
			getSongAPIMap()->insert_or_assign(guildId, songAPI);
			getSongAPIMap()->at(guildId)->loadPlaylist();
			return true;
		}

		static vector<Song> searchForSong(string searchQuery, string guildId) {
			auto vector01 = SoundCloudAPI::searchForSong(searchQuery, guildId);
			auto vector02 = YouTubeAPI::searchForSong(searchQuery, guildId);
			int totalLength = (int)(vector01.size() + vector02.size());
			vector<Song> newVector{};
			int vector01Used{ 0 };
			int vector02Used{ 0 };
			for (int x = 0; x < totalLength; x += 1) {
				if ((vector01Used < vector01.size() - 1) && (x % 2 == 0) && vector01.size() > 0) {
					Song newSong = vector01[vector01Used];
					newSong.type = SongType::SoundCloud;
					newVector.push_back(newSong);
					vector01Used += 1;
				}
				else if (vector02Used < vector02.size() - 1 && vector02.size()>0) {
					Song newSong = vector02[vector02Used];
					newSong.type = SongType::YouTube;
					newVector.push_back(newSong);
					vector02Used += 1;
				}
			}
			return newVector;
		}

		static void setLoopAllStatus(bool enabled, string guildId) {
			getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled = enabled;
			getSongAPIMap()->at(guildId)->savePlaylist();
		}

		static bool isLoopAllEnabled(string guildId) {
			getSongAPIMap()->at(guildId)->loadPlaylist();
			return getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled;
		}

		static void setLoopSongStatus(bool enabled, string guildId) {
			getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled = enabled;
			getSongAPIMap()->at(guildId)->savePlaylist();
		}

		static bool isLoopSongEnabled(string guildId) {
			getSongAPIMap()->at(guildId)->loadPlaylist();
			return getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled;
		}

		static bool isThereAnySongs(string guildId) {
			getSongAPIMap()->at(guildId)->loadPlaylist();
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

		static Song addSongToQueue(GuildMember guildMember, string guildId, Song song) {
			DiscordGuild* discordGuild = getDiscordGuildMap()->at(guildId);
			shared_ptr<SongAPI> songAPI = make_shared<SongAPI>(discordGuild);
			songAPI->playlist = discordGuild->data.playlist;
			getSongAPIMap()->insert_or_assign(guildId, songAPI);
			song.addedByUserId = guildMember.user.id;
			song.addedByUserName = guildMember.user.userName;
			getSongAPIMap()->at(guildId)->playlist.songQueue.push_back(song);
			getSongAPIMap()->at(guildId)->savePlaylist();
			return song;
		}

		static void setPlaylist(Playlist playlistNew, string guildId) {
			getSongAPIMap()->at(guildId)->playlist.currentSong = playlistNew.currentSong;
			getSongAPIMap()->at(guildId)->playlist.isLoopAllEnabled = playlistNew.isLoopAllEnabled;
			getSongAPIMap()->at(guildId)->playlist.isLoopSongEnabled = playlistNew.isLoopSongEnabled;
			getSongAPIMap()->at(guildId)->playlist.songQueue = playlistNew.songQueue;
			getSongAPIMap()->at(guildId)->playlist.currentSong = Song();
			getSongAPIMap()->at(guildId)->savePlaylist();
		}

		static Playlist getPlaylist(string guildId) {
			getSongAPIMap()->at(guildId)->loadPlaylist();
			return getSongAPIMap()->at(guildId)->playlist;
		}

		static void modifyQueue(int firstSongPosition, int secondSongPosition, string guildId) {
			DiscordGuild* discordGuild = getDiscordGuildMap()->at(guildId);
			shared_ptr<SongAPI> songAPI = make_shared<SongAPI>(discordGuild);
			discordGuild = getDiscordGuildMap()->at(guildId);
			Song tempSong = getSongAPIMap()->at(guildId)->playlist.songQueue.at(firstSongPosition);
			getSongAPIMap()->at(guildId)->playlist.songQueue.at(firstSongPosition) = getSongAPIMap()->at(guildId)->playlist.songQueue.at(secondSongPosition);
			getSongAPIMap()->at(guildId)->playlist.songQueue.at(secondSongPosition) = tempSong;
			getSongAPIMap()->at(guildId)->savePlaylist();

		}

		static Song getCurrentSong(string guildId) {
			getSongAPIMap()->at(guildId)->loadPlaylist();
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

		static void setCurrentSong(Song song, string guildId) {
			getSongAPIMap()->at(guildId)->playlist.currentSong = song;
			getSongAPIMap()->at(guildId)->savePlaylist();
		}

		static bool sendNextSong(string guildId, GuildMember guildMember) {
			getSongAPIMap()->at(guildId)->refreshInterfaces();
			getSongAPIMap()->at(guildId)->sendNextSong();
			getSongAPIMap()->at(guildId)->discordGuild->getDataFromDB();
			if (getSongAPIMap()->at(guildId)->discordGuild->data.playlist.currentSong.songId == "") {
				if (!getSongAPIMap()->at(guildId)->sendNextSong()) {
					return false;
				};
			}
			try {
				if (getSongAPIMap()->at(guildId)->playlist.currentSong.type == SongType::SoundCloud) {
					auto newerSong = SoundCloudAPI::soundCloudAPIMap->at(guildId)->theSong.collectFinalSong(guildMember, getSongAPIMap()->at(guildId)->playlist.currentSong);
					SoundCloudAPI::sendNextSong(newerSong, guildId);
					return true;
				}
				else {
					auto newerSong = YouTubeAPI::youtubeAPIMap->at(guildId)->theSong.collectFinalSong(guildMember, getSongAPIMap()->at(guildId)->playlist.currentSong);
					YouTubeAPI::sendNextSong(newerSong, guildId);
					return true;
				}
			}
			catch (...) {
				SongCompletionEventData eventData{ .voiceConnection = getSongAPIMap()->at(guildId)->voiceConnection.get(), .isThisAReplay = true, .previousSong = getSongAPIMap()->at(guildId)->discordGuild->data.playlist.currentSong };
				SongAPI::setCurrentSong(Song(), guildId);
				if (!getSongAPIMap()->at(guildId)->sendNextSong()) {
					return false;
				};
				AudioFrameData frameData{ .type = AudioFrameType::Cancel, .encodedFrameData = {.sampleCount = 0},.rawFrameData = {.sampleCount = 0} };
				send(getSongAPIMap()->at(guildId)->sendAudioDataBuffer.get(), frameData);
				(*getSongAPIMap()->at(guildId)->voiceConnection->onSongCompletionEvent)(eventData);
				return false;
			}
		}

	};
};
#endif
