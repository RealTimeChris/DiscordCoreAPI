/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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

#pragma once

#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder {
	  public:
		SoundCloudRequestBuilder() noexcept = default;

	  protected:
		static std::string clientId;

		const std::string baseUrl02{ "https://api-v2.soundcloud.com" };
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		const std::string baseUrl{ "https://soundcloud.com" };
		const std::string appVersion{ "1681464840" };
		HttpsClient* httpsClient{ nullptr };

		DiscordCoreAPI::Song constructDownloadInfo(const DiscordCoreAPI::Song& newSong);

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const std::string& string);

		virtual DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong);

		std::string collectClientId();
	};

	class DiscordCoreAPI_Dll SoundCloudAPI : public SoundCloudRequestBuilder {
	  public:
		SoundCloudAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClient, const DiscordCoreAPI::Snowflake guildId);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries);

		std::vector<DiscordCoreAPI::Song> searchForSong(const std::string& searchQuery);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong) override;

	  protected:
		DiscordCoreAPI::Snowflake guildId{};
		const int32_t maxBufferSize{ 8192 };
		DiscordCoreAPI::Song theSong{};
	};

	struct Transcoding {
		std::string preset{};
		std::string url{};
	};

	struct Media {
		std::vector<Transcoding> transcodings{};
	};

	struct SecondDownloadUrl {
		std::string url{};
	};

	struct RawSoundCloudSong {
		std::string trackAuthorization{};
		std::string description{};
		std::string artworkUrl{};
		std::string title{};
		int32_t duration{};
		Media media{};
	};

	struct SoundCloudSearchResults {
		std::vector<RawSoundCloudSong> collection{};
	};

};
