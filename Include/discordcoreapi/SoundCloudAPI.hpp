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
/// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file SoundCloudAPI.hpp

#ifndef SOUNDCLOUD_API
#define SOUNDCLOUD_API

#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder {
	  public:
		SoundCloudRequestBuilder() noexcept = default;

	  protected:
		static String clientId;

		const String baseUrl02{ "https://api-v2.soundcloud.com" };
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		const String baseUrl{ "https://soundcloud.com" };
		const String appVersion{ "1654762087" };
		HttpsClient* httpsClient{ nullptr };

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const String& theString);

		DiscordCoreAPI::Song constructDownloadInfo(DiscordCoreAPI::Song& newSong);

		DiscordCoreAPI::Song collectFinalSong(DiscordCoreAPI::Song& newSong);

		String collectClientId();
	};

	class DiscordCoreAPI_Dll SoundCloudAPI : public SoundCloudRequestBuilder {
	  public:
		SoundCloudAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClient, const DiscordCoreAPI::Snowflake guildId);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, Int32 currentReconnectTries);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, Int32 currentReconnectTries);

		std::vector<DiscordCoreAPI::Song> searchForSong(const String& searchQuery);

		DiscordCoreAPI::Song collectFinalSong(DiscordCoreAPI::Song& newSong);

	  protected:
		DiscordCoreAPI::Snowflake guildId{};
		const Int32 maxBufferSize{ 8192 };
		DiscordCoreAPI::Song theSong{};
	};

};// namespace DiscordCoreAPI
#endif