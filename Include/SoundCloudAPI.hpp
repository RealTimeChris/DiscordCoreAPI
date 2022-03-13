/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without 
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
/// Aug 25, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file SoundCloudAPI.hpp

#pragma once

#include <CoRoutine.hpp>
#include <AudioEncoder.hpp>
#include <Http.hpp>
#include <AudioDecoder.hpp>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder {
	public:

		SoundCloudRequestBuilder() = default;

		SoundCloudRequestBuilder(DiscordCoreInternal::HttpClient*);
		
		Song collectFinalSong(GuildMemberData addedByGuildMember, Song newSong);

		std::vector<DiscordCoreInternal::HttpData> submitWorkloadAndGetResult(std::vector<DiscordCoreInternal::HttpWorkloadData> httpData);

		std::vector<Song> collectSearchResults(std::string theString);

	protected:

		inline static std::string clientId{ "" };

		const std::string baseUrl02{ "https://api-v2.soundcloud.com" };
		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		const std::string baseUrl{ "https://soundcloud.com" };
		const std::string appVersion{ "1631696495" };

		Song constructSecondDownloadUrl(Song newSong);

		Song constructFinalDownloadUrl(Song newSong);

		std::string collectClientId();

	};

	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		SoundCloudAPI(std::string guildId, DiscordCoreInternal::HttpClient* httpClient);

		Song collectFinalSong(GuildMemberData addedByGuildMember, Song newSong);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		std::vector<Song> searchForSong(std::string searchQuery);

		void stop();

	protected:

		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		SoundCloudRequestBuilder requestBuilder{};
		const int32_t maxBufferSize{ 8192 };
		Song theSong{ };
		std::string guildId{ "" };

		void cancelCurrentSong();
		
	};

};
