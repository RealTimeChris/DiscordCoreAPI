/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
		
		SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

		std::vector<DiscordCoreInternal::HttpData> submitWorkloadAndGetResult(std::vector<DiscordCoreInternal::HttpWorkloadData> httpData);

		std::vector<SoundCloudSong> collectSearchResults(std::string theString);

	protected:

		inline static std::string clientId{ "" };

		const std::string baseUrl02{ "https://api-v2.soundcloud.com" };
		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		const std::string baseUrl{ "https://soundcloud.com" };
		const std::string appVersion{ "1631696495" };

		SoundCloudSong constructSecondDownloadUrl(SoundCloudSong newSong);

		SoundCloudSong constructFinalDownloadUrl(SoundCloudSong newSong);

		std::string collectClientId();

	};

	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		SoundCloudAPI(std::string guildId, DiscordCoreInternal::HttpClient* httpClient);

		SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		std::vector<SoundCloudSong> searchForSong(std::string searchQuery);

		void stop();

	protected:

		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		SoundCloudRequestBuilder requestBuilder{};
		const int32_t maxBufferSize{ 8192 };
		SoundCloudSong theSong{ };
		std::string guildId{ "" };

		void cancelCurrentSong();
		
	};

};
