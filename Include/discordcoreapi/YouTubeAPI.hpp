/*
*
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
/// YouTubeAPI.hpp - Header for the YouTube API related stuff.
/// Jun 30, 2021
/// https://discordcoreapi.com
/// \file YouTubeAPI.hpp

#pragma once

#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <regex>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll YouTubeRequestBuilder {
	  public:
		YouTubeRequestBuilder() = default;

		YouTubeRequestBuilder(HttpsClient*);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::GuildMemberData& addedByGuildMember, DiscordCoreAPI::Song& newSong);

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const std::string& theString);

	  protected:
		HttpsClient* httpsClient{ nullptr };
		std::string baseUrl{ "https://www.youtube.com" };

		DiscordCoreAPI::Song constructDownloadInfo(const DiscordCoreAPI::GuildMemberData& guildMember, DiscordCoreAPI::Song& newSong);
	};

	class DiscordCoreAPI_Dll YouTubeAPI {
	  public:
		YouTubeAPI(const uint64_t& guildId, HttpsClient* httpsClient);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token theToken, int32_t currentRecursionDepth);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token theToken, int32_t currentRecursionDepth);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::GuildMemberData& addedByGuildMember, DiscordCoreAPI::Song& newSong);

		std::vector<DiscordCoreAPI::Song> searchForSong(const std::string& searchQuery);

		void cancelCurrentSong();

	  protected:
		
		bool doWePrintWebSocketErrorMessages{ false };
		bool doWePrintFFMPEGSuccessMessages{ false };
		bool doWePrintFFMPEGErrorMessages{ false };
		YouTubeRequestBuilder requestBuilder{};
		const int32_t maxBufferSize{ 8192 };
		DiscordCoreAPI::Song theSong{};
		uint64_t guildId{};

		void breakOut(std::stop_token theToken, std::unique_ptr<AudioDecoder> audioDecoder);
	};

};// namespace DiscordCoreAPI
