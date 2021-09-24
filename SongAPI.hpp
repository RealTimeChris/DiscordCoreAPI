// SongAPI.hpp - Header for the "song api" stuff.
// Sep 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SONG_API_
#define _SONG_API_

#include "../DiscordCoreAPI-main/DiscordCoreClient02.hpp"
#include "YouTubeStuff.hpp"
#include "SoundCloudStuff.hpp"
#include "GuildStuff.hpp"

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
		Playlist playlist{};
		string guildId{ "" };

		SongAPI(DiscordGuild* discordGuildNew) {
			this->discordGuild = discordGuildNew;
			this->guildId = this->discordGuild->data.guildId;
			this->voiceConnection = SongAPI::voiceConnectionMap->at(this->guildId);
			this->sendAudioDataBuffer = SongAPI::sendAudioDataBufferMap->at(this->guildId);
			this->loadPlaylist();
		}

		static void initialize(map<string, shared_ptr<SongAPI>>* songAPICoreMapNew, map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMapNew, map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* audioBuffersMapNew, map<string, DiscordGuild*>* discordGuildMapNew, map<string, shared_ptr<VoiceConnection>>* voiceConnectionMapNew) {
			SoundCloudAPI::initialize(soundCloudAPIMapNew, audioBuffersMapNew, discordGuildMapNew, voiceConnectionMapNew);
			YouTubeAPI::initialize(youtubeAPIMapNew, audioBuffersMapNew, discordGuildMapNew, voiceConnectionMapNew);
			SongAPI::sendAudioDataBufferMap = audioBuffersMapNew;
			SongAPI::voiceConnectionMap = voiceConnectionMapNew;
			SongAPI::discordGuildMap = discordGuildMapNew;
			SongAPI::songAPIMap = songAPICoreMapNew;
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

		void sendNextSong() {
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
					return;
				}
				else if (this->playlist.songQueue.size() > 0 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.currentSong;
					this->savePlaylist();
					return;
				}
				else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
					this->playlist.currentSong = this->playlist.currentSong;
					this->savePlaylist();
					return;
				}
				else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
					this->savePlaylist();
					return;
				}
				else if (this->playlist.currentSong.songId == "") {
					this->savePlaylist();
					return;
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
					return;
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
					return;
				}
				else if (this->playlist.currentSong.songId != "" && this->playlist.songQueue.size() == 0) {
					this->savePlaylist();
					return;
				}
				else if (this->playlist.songQueue.size() == 1 && this->playlist.currentSong.songId == "") {
					this->playlist.currentSong = this->playlist.songQueue.at(0);
					this->playlist.songQueue.erase(this->playlist.songQueue.begin(), this->playlist.songQueue.begin() + 1);
					this->savePlaylist();
					return;
				}
				else if (this->playlist.currentSong.songId == "") {
					this->savePlaylist();
					return;
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
					return;
				}
				else if (this->playlist.currentSong.description != "" && this->playlist.songQueue.size() == 0) {
					this->playlist.currentSong = Song();
					this->savePlaylist();
					return;
				}
				else if (this->playlist.currentSong.songId == "") {
					this->savePlaylist();
					return;
				}
			}
			return;
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
			DiscordCoreClientBase::songAPIMap->erase(guildId);
			SongAPI::songAPIMap->erase(guildId);
			DiscordGuild* discordGuild = SongAPI::discordGuildMap->at(guildId);
			shared_ptr<SongAPI> songAPICore = make_shared<SongAPI>(discordGuild);
			SongAPI::songAPIMap->insert_or_assign(guildId, songAPICore);
			SongAPI::sendNextSong(guildId, guildMember);
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

		static void sendNextSong(string guildId, GuildMember guildMember) {
			SongAPI::songAPIMap->at(guildId)->sendNextSong();
			if (SongAPI::songAPIMap->at(guildId)->playlist.currentSong.songId == "") {
				return;
			}
			else {
				try {
					if (SongAPI::songAPIMap->at(guildId)->playlist.currentSong.type == SongType::SoundCloud) {
						auto newerSong = SoundCloudAPI::soundCloudAPIMap->at(guildId)->theSong.collectFinalSong(guildMember, SongAPI::songAPIMap->at(guildId)->playlist.currentSong);
						SoundCloudAPI::sendNextSong(newerSong, guildId);
						return;
					}
					else {
						auto newerSong = YouTubeAPI::youtubeAPIMap->at(guildId)->theSong.collectFinalSong(guildMember, SongAPI::songAPIMap->at(guildId)->playlist.currentSong);
						YouTubeAPI::sendNextSong(newerSong, guildId);
						return;
					}
				}
				catch (...) {
					AudioFrameData frameData{ .type = AudioFrameType::Cancel, .encodedFrameData = {.sampleCount = 0},.rawFrameData = {.sampleCount = 0} };
					send(SongAPI::songAPIMap->at(guildId)->sendAudioDataBuffer.get(), frameData);
					SongCompletionEventData eventData{ .voiceConnection = SongAPI::songAPIMap->at(guildId)->voiceConnection.get(), .isThisAReplay = true, .previousSong = SongAPI::songAPIMap->at(guildId)->discordGuild->data.playlist.currentSong };
					SongAPI::setCurrentSong(Song(), guildId);
					(*SongAPI::songAPIMap->at(guildId)->voiceConnection->onSongCompletionEvent)(eventData);
					return;
				}
			}
		}

	};

	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* SongAPI::sendAudioDataBufferMap{ new map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>() };
	map<string, shared_ptr<VoiceConnection>>* SongAPI::voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
	map<string, shared_ptr<SongAPI>>* SongAPI::songAPIMap{ new map <string, shared_ptr<SongAPI >>() };
	map<string, DiscordGuild*>* SongAPI::discordGuildMap{ new map<string, DiscordGuild*>() };
}

void DiscordCoreInternal::DataParser::parseObject(json jsonObjectData, DiscordCoreAPI::YouTubeSong* pDataStructure) {
	DiscordCoreAPI::YouTubeSong newData = *pDataStructure;

	if (jsonObjectData.contains("lengthText") && !jsonObjectData.at("lengthText").is_null()) {
		newData.duration = jsonObjectData.at("lengthText").at("accessibility").at("accessibilityData").at("label").get<string>();
	}

	if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData.at("detailedMetadataSnippets").is_null()) {
		for (auto value : jsonObjectData.at("detailedMetadataSnippets").at(0).at("snippetText").at("runs")) {
			newData.description += value.at("text").get<string>();
		}
	}

	if (jsonObjectData.contains("thumbnail") && !jsonObjectData.at("thumbnail").is_null()) {
		newData.thumbnailURL = jsonObjectData.at("thumbnail").at("thumbnails").at(0).at("url").get<string>();
	}

	if (jsonObjectData.contains("videoId") && !jsonObjectData.at("videoId").is_null()) {
		newData.songId = "https://www.youtube.com/watch?v=" + jsonObjectData.at("videoId").get<string>();
	}

	if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null()) {
		if (jsonObjectData.at("title").contains("runs")) {
			newData.songTitle = jsonObjectData.at("title").at("runs").at(0).at("text").get<string>();
		}
		else if (jsonObjectData.at("title").contains("simpleText")) {
			newData.songTitle = jsonObjectData.at("title").at("simpleText").get<string>();
		}
	}

	if (jsonObjectData.contains("videoId") && !jsonObjectData.at("videoId").is_null()) {
		newData.songId = jsonObjectData.at("videoId").get<string>();
	}

	*pDataStructure = newData;
}

void DiscordCoreInternal::DataParser::parseObject(json jsonObjectData, DiscordCoreAPI::SoundCloudSong* pDataStructure) {
	DiscordCoreAPI::SoundCloudSong newData = *pDataStructure;

	if (jsonObjectData.contains("track_authorization") && !jsonObjectData.at("track_authorization").is_null()) {
		newData.trackAuthorization = jsonObjectData.at("track_authorization").get<string>();
	}

	if (jsonObjectData.contains("media") && !jsonObjectData.at("media").is_null()) {
		bool isItFound{ false };
		for (auto value : jsonObjectData.at("media").at("transcodings")) {
			if (value.at("preset") == "opus_0_0") {
				isItFound = true;
				newData.firstDownloadURL = to_string(to_hstring(value.at("url").get<string>()));
				newData.songId = to_string(to_hstring(value.at("url").get<string>()));
			}
		}
		bool isItFound2{ false };
		if (!isItFound) {
			for (auto value : jsonObjectData.at("media").at("transcodings")) {
				if (value.at("preset") == "mp3_0_0") {
					newData.firstDownloadURL = to_string(to_hstring(value.at("url").get<string>()));
					newData.songId = to_string(to_hstring(value.at("url").get<string>()));
					isItFound2 = true;
				}
			}
		}
		if (!isItFound2) {
			for (auto value : jsonObjectData.at("media").at("transcodings")) {
				newData.firstDownloadURL = to_string(to_hstring(value.at("url").get<string>()));
				newData.songId = to_string(to_hstring(value.at("url").get<string>()));
			}
		}
	}

	if (jsonObjectData.contains("title") && !jsonObjectData.at("title").is_null() && !jsonObjectData.at("title").is_object()) {
		newData.songTitle = to_string(to_hstring(jsonObjectData.at("title").get<string>()));
	}
	if (jsonObjectData.contains("description") && !jsonObjectData.at("description").is_null()) {
		string newString = to_string(to_hstring(jsonObjectData.at("description").get<string>()));
		if (newString.size() > 100) {
			newString = newString.substr(0, 100);
		}
		char* newString01 = g_utf8_make_valid(newString.c_str(), newString.size());
		char* newString02 = g_utf8_normalize(newString01, newString.size(), GNormalizeMode::G_NORMALIZE_ALL);
		for (int x = 0; x < newString.size(); x += 1) {
			newData.description.push_back(newString02[x]);
		}
		newData.description += "...";
	}

	if (jsonObjectData.contains("artwork_url") && !jsonObjectData.at("artwork_url").is_null()) {
		newData.thumbnailURL = to_string(to_hstring(jsonObjectData.at("artwork_url").get<string>()));
	}

	if (jsonObjectData.contains("duration") && !jsonObjectData.at("duration").is_null()) {
		int durationNew = jsonObjectData.at("duration").get<int>();
		newData.duration = DiscordCoreAPI::convertMsToDurationString(durationNew);
	}

	if (jsonObjectData.contains("permalink_url") && !jsonObjectData.at("permalink_url").is_null()) {
		newData.viewURL = to_string(to_hstring(jsonObjectData.at("permalink_url").get<string>()));
	}

	*pDataStructure = newData;
}

#endif
