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
/// YouTubeAPI.hpp - Header for the YouTube API related stuff.
/// Jun 30, 2021
/// https://discordcoreapi.com
/// \file YouTubeAPI.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll YouTubeRequestBuilder {
	  public:

		YouTubeRequestBuilder() = default;

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		std::string baseUrl{ "https://www.youtube.com" };
		HttpsClient* httpsClient{ nullptr };

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::GuildMemberData& addedByGuildMember, DiscordCoreAPI::Song& newSong);

		DiscordCoreAPI::Song constructDownloadInfo(const DiscordCoreAPI::GuildMemberData& guildMember, DiscordCoreAPI::Song& newSong);

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const std::string& theString);
	};

	class DiscordCoreAPI_Dll YouTubeAPI : public YouTubeRequestBuilder {
	  public:

		YouTubeAPI(DiscordCoreAPI::ConfigManager*, HttpsClient* httpsClient, const Snowflake& guildId);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::GuildMemberData& addedByGuildMember, DiscordCoreAPI::Song& newSong);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, int32_t currentReconnectTries);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, int32_t currentReconnectTries);

		std::vector<DiscordCoreAPI::Song> searchForSong(const std::string& searchQuery);

		void cancelCurrentSong();

	  protected:
		DiscordCoreAPI::Snowflake guildId{};
		const int32_t maxBufferSize{ 8192 };
		DiscordCoreAPI::Song theSong{};
	};

};// namespace DiscordCoreAPI
