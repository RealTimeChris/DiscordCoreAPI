/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file SoundCloudAPI.hpp
#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		class DiscordCoreAPI_Dll sound_cloud_request_builder : public https_client_core {
		  public:
			sound_cloud_request_builder(config_manager* configManagerNew);

			song collectFinalSong(const song& songNew);

		  protected:
			static jsonifier::string clientId;
			static constexpr jsonifier::string_view baseUrl02{ "https://api-v2.soundcloud.com" };
			static constexpr jsonifier::string_view baseUrl{ "https://soundcloud.com" };
			static constexpr jsonifier::string_view appVersion{ "1681464840" };

			jsonifier::vector<song> collectSearchResults(jsonifier::string_view songQuery, uint64_t limit = 20);

			song constructDownloadInfo(const song& songNew, uint64_t currentRecursionDepth);

			jsonifier::vector<song> collectPlaylist(jsonifier::string_view string);

			song collectSingleResult(jsonifier::string_view string);

			jsonifier::string collectClientId();

			virtual ~sound_cloud_request_builder() = default;
		};

		class DiscordCoreAPI_Dll sound_cloud_api : public sound_cloud_request_builder {
		  public:
			sound_cloud_api(config_manager* configManagerNew, const snowflake guildId);

			co_routine<void, false> downloadAndStreamAudio(const song songNew,
				std::coroutine_handle<co_routine<void, false>::promise_type> threadHandle = std::coroutine_handle<co_routine<void, false>::promise_type>{},
				uint64_t currentReconnectTries											  = 0);

			void weFailedToDownloadOrDecode(const song& songNew, std::coroutine_handle<co_routine<void, false>::promise_type> threadHandle, uint64_t recursionDepth);

			jsonifier::vector<song> searchForSong(jsonifier::string_view searchQuery, uint64_t limit);

			bool areWeWorking();

		  protected:
			std::atomic_bool areWeWorkingBool{ false };
			snowflake guildId{};
		};

		enum class search_type { single_song_with_id = 0, single_song_without_id = 1, playlist = 2 };

		struct transcoding {
			jsonifier::string preset{};
			jsonifier::string url{};
		};

		struct second_download_url {
			jsonifier::string url{};
		};

		struct media {
			std::vector<transcoding> transcodings{};
		}; 

		struct raw_sound_cloud_song {
			jsonifier::string trackAuthorization{};
			jsonifier::string description{};
			jsonifier::string artworkUrl{};
			jsonifier::string viewUrl{};
			jsonifier::string title{};
			uint64_t duration{};
			media mediaVal{};
		};

		struct sound_cloud_search_results {
			jsonifier::vector<raw_sound_cloud_song> collection{};
		};

		struct data_class {
			jsonifier::string trackAuthorization{};
			jsonifier::string description{};
			jsonifier::string artworkUrl{};
			jsonifier::string avatarUrl{};
			jsonifier::string viewUrl{};
			jsonifier::string title{};
			int64_t duration{};
			media mediaVal{};
		};

		struct welcome_element {
			jsonifier::raw_json_data data{};
			jsonifier::string hydratable{};
		};

		struct welcome {
			std::vector<welcome_element> data{};
		};
	};
};
