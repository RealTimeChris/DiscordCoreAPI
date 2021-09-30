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

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<SongAPI>>* songAPIMap;
		static map<string, DiscordGuild*>* discordGuildMap;

		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		DiscordGuild* discordGuild{ new DiscordGuild };
		string guildId{ "" };
		Playlist playlist{};

		SongAPI(DiscordGuild* discordGuildNew) {
			this->sendAudioDataBuffer = SongAPI::sendAudioDataBufferMap->at(discordGuildNew->data.guildId);
			this->voiceConnection = SongAPI::voiceConnectionMap->at(discordGuildNew->data.guildId);
			this->guildId = discordGuildNew->data.guildId;
			this->discordGuild = discordGuildNew;
			this->loadPlaylist();
		}

		static void initialize(map<string, DiscordGuild*>* discordGuildMapNew) {
			SoundCloudAPI::initialize(getSoundCloudAPIMap(), getAudioBuffersMap(), discordGuildMapNew, getVoiceConnectionMap());
			YouTubeAPI::initialize(getYouTubeAPIMap(), getAudioBuffersMap(), discordGuildMapNew, getVoiceConnectionMap());
			SongAPI::sendAudioDataBufferMap = getAudioBuffersMap();
			SongAPI::voiceConnectionMap = getVoiceConnectionMap();
			SongAPI::songAPIMap = getSongAPIMap();
			SongAPI::discordGuildMap = discordGuildMapNew;
		}

		static void cleanup() {
			delete SongAPI::sendAudioDataBufferMap;
			SongAPI::sendAudioDataBufferMap = nullptr;
			delete SongAPI::voiceConnectionMap;
			SongAPI::voiceConnectionMap = nullptr;
			delete SongAPI::songAPIMap;
			SongAPI::songAPIMap = nullptr;
			delete SongAPI::discordGuildMap;
			SongAPI::discordGuildMap = nullptr;
		}

		void refreshInterfaces() {
			DiscordGuild* discordGuildNew = this->discordGuild;
			SongAPI::songAPIMap->insert_or_assign(discordGuildNew->data.guildId, make_shared<SongAPI>(discordGuildNew));
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
			SongAPI::songAPIMap->at(guildId)->refreshInterfaces();
			if (SongAPI::isLoopAllEnabled(guildId) || SongAPI::isLoopSongEnabled(guildId)) {
				SongAPI::songAPIMap->at(guildId)->playlist.songQueue.push_back(SongAPI::songAPIMap->at(guildId)->playlist.currentSong);
				SongAPI::setCurrentSong(Song(), guildId);
			}
			else {
				SongAPI::setCurrentSong(Song(), guildId);
			}
			AudioFrameData frameData{};
			while (try_receive(SongAPI::sendAudioDataBufferMap->at(guildId).get(), frameData)) {};
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
			SongAPI::songAPIMap->erase(guildId);
			DiscordGuild* discordGuild = SongAPI::discordGuildMap->at(guildId);
			shared_ptr<SongAPI> songAPICore = make_shared<SongAPI>(discordGuild);
			SongAPI::songAPIMap->insert_or_assign(guildId, songAPICore);
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
			SongAPI::songAPIMap->at(guildId)->refreshInterfaces();
			vector<Song> newVector02;
			if (SongAPI::songAPIMap->at(guildId)->playlist.currentSong.description != "") {
				newVector02.push_back(SongAPI::songAPIMap->at(guildId)->playlist.currentSong);
				SongAPI::songAPIMap->at(guildId)->playlist.currentSong = Song();
			}
			for (auto value : SongAPI::songAPIMap->at(guildId)->playlist.songQueue) {
				newVector02.push_back(value);
			}
			SongAPI::songAPIMap->at(guildId)->playlist.songQueue = newVector02;
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
			SongAPI::songAPIMap->erase(guildId);
			auto discordGuild = SongAPI::discordGuildMap->at(guildId);
			shared_ptr<SongAPI> songAPI = make_shared<SongAPI>(discordGuild);
			SongAPI::songAPIMap->insert_or_assign(guildId, songAPI);
			SongAPI::songAPIMap->at(guildId)->loadPlaylist();
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
			SongAPI::songAPIMap->at(guildId)->playlist.isLoopAllEnabled = enabled;
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
		}

		static bool isLoopAllEnabled(string guildId) {
			SongAPI::songAPIMap->at(guildId)->loadPlaylist();
			return SongAPI::songAPIMap->at(guildId)->playlist.isLoopAllEnabled;
		}

		static void setLoopSongStatus(bool enabled, string guildId) {
			SongAPI::songAPIMap->at(guildId)->playlist.isLoopSongEnabled = enabled;
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
		}

		static bool isLoopSongEnabled(string guildId) {
			SongAPI::songAPIMap->at(guildId)->loadPlaylist();
			return SongAPI::songAPIMap->at(guildId)->playlist.isLoopSongEnabled;
		}

		static bool isThereAnySongs(string guildId) {
			SongAPI::songAPIMap->at(guildId)->loadPlaylist();
			if (SongAPI::songAPIMap->at(guildId)->playlist.isLoopAllEnabled || SongAPI::songAPIMap->at(guildId)->playlist.isLoopSongEnabled) {
				if (SongAPI::songAPIMap->at(guildId)->playlist.songQueue.size() == 0 && SongAPI::songAPIMap->at(guildId)->playlist.currentSong.songId == "") {
					return false;
				}
				else {
					return true;
				}
			}
			else {
				if (SongAPI::songAPIMap->at(guildId)->playlist.songQueue.size() == 0) {
					return false;
				}
				else {
					return true;
				}
			}
		}

		static Song addSongToQueue(GuildMember guildMember, string guildId, Song song) {
			DiscordGuild* discordGuild = SongAPI::discordGuildMap->at(guildId);
			shared_ptr<SongAPI> songAPI = make_shared<SongAPI>(discordGuild);
			songAPI->playlist = discordGuild->data.playlist;
			SongAPI::songAPIMap->insert_or_assign(guildId, songAPI);
			song.addedByUserId = guildMember.user.id;
			song.addedByUserName = guildMember.user.userName;
			SongAPI::songAPIMap->at(guildId)->playlist.songQueue.push_back(song);
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
			return song;
		}

		static void setPlaylist(Playlist playlistNew, string guildId) {
			SongAPI::songAPIMap->at(guildId)->playlist.currentSong = playlistNew.currentSong;
			SongAPI::songAPIMap->at(guildId)->playlist.isLoopAllEnabled = playlistNew.isLoopAllEnabled;
			SongAPI::songAPIMap->at(guildId)->playlist.isLoopSongEnabled = playlistNew.isLoopSongEnabled;
			SongAPI::songAPIMap->at(guildId)->playlist.songQueue = playlistNew.songQueue;
			SongAPI::songAPIMap->at(guildId)->playlist.currentSong = Song();
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
		}

		static Playlist getPlaylist(string guildId) {
			SongAPI::songAPIMap->at(guildId)->loadPlaylist();
			return SongAPI::songAPIMap->at(guildId)->playlist;
		}

		static void modifyQueue(int firstSongPosition, int secondSongPosition, string guildId) {
			DiscordGuild* discordGuild = SongAPI::discordGuildMap->at(guildId);
			shared_ptr<SongAPI> songAPI = make_shared<SongAPI>(discordGuild);
			discordGuild = SongAPI::discordGuildMap->at(guildId);
			Song tempSong = SongAPI::songAPIMap->at(guildId)->playlist.songQueue.at(firstSongPosition);
			SongAPI::songAPIMap->at(guildId)->playlist.songQueue.at(firstSongPosition) = SongAPI::songAPIMap->at(guildId)->playlist.songQueue.at(secondSongPosition);
			SongAPI::songAPIMap->at(guildId)->playlist.songQueue.at(secondSongPosition) = tempSong;
			SongAPI::songAPIMap->at(guildId)->savePlaylist();

		}

		static Song getCurrentSong(string guildId) {
			SongAPI::songAPIMap->at(guildId)->loadPlaylist();
			if (SongAPI::songAPIMap->at(guildId)->playlist.currentSong.songId != "") {
				return SongAPI::songAPIMap->at(guildId)->playlist.currentSong;
			}
			else if (SongAPI::songAPIMap->at(guildId)->playlist.songQueue.size() > 0) {
				return SongAPI::songAPIMap->at(guildId)->playlist.songQueue.at(0);
			}
			else {
				return Song();
			};
		}

		static void setCurrentSong(Song song, string guildId) {
			SongAPI::songAPIMap->at(guildId)->playlist.currentSong = song;
			SongAPI::songAPIMap->at(guildId)->savePlaylist();
		}

		static bool sendNextSong(string guildId, GuildMember guildMember) {
			SongAPI::songAPIMap->at(guildId)->refreshInterfaces();
			SongAPI::songAPIMap->at(guildId)->sendNextSong();
			SongAPI::songAPIMap->at(guildId)->discordGuild->getDataFromDB();
			if (SongAPI::songAPIMap->at(guildId)->discordGuild->data.playlist.currentSong.songId == "") {
				if (!SongAPI::songAPIMap->at(guildId)->sendNextSong()) {
					return false;
				};
			}
			try {
				if (SongAPI::songAPIMap->at(guildId)->playlist.currentSong.type == SongType::SoundCloud) {
					auto newerSong = SoundCloudAPI::soundCloudAPIMap->at(guildId)->theSong.collectFinalSong(guildMember, SongAPI::songAPIMap->at(guildId)->playlist.currentSong);
					SoundCloudAPI::sendNextSong(newerSong, guildId);
					return true;
				}
				else {
					auto newerSong = YouTubeAPI::youtubeAPIMap->at(guildId)->theSong.collectFinalSong(guildMember, SongAPI::songAPIMap->at(guildId)->playlist.currentSong);
					YouTubeAPI::sendNextSong(newerSong, guildId);
					return true;
				}
			}
			catch (...) {
				SongCompletionEventData eventData{ .voiceConnection = SongAPI::songAPIMap->at(guildId)->voiceConnection.get(), .isThisAReplay = true, .previousSong = SongAPI::songAPIMap->at(guildId)->discordGuild->data.playlist.currentSong };
				SongAPI::setCurrentSong(Song(), guildId);
				if (!SongAPI::songAPIMap->at(guildId)->sendNextSong()) {
					return false;
				};
				AudioFrameData frameData{ .type = AudioFrameType::Cancel, .encodedFrameData = {.sampleCount = 0},.rawFrameData = {.sampleCount = 0} };
				send(SongAPI::songAPIMap->at(guildId)->sendAudioDataBuffer.get(), frameData);
				(*SongAPI::songAPIMap->at(guildId)->voiceConnection->onSongCompletionEvent)(eventData);
				return false;
			}
		}

	};

	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* SongAPI::sendAudioDataBufferMap{ nullptr };
	map<string, shared_ptr<VoiceConnection>>* SongAPI::voiceConnectionMap{ nullptr };
	map<string, shared_ptr<SongAPI>>* SongAPI::songAPIMap{ nullptr };
	map<string, DiscordGuild*>* SongAPI::discordGuildMap{ nullptr };
};
#endif
