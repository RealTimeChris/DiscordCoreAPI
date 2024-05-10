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
/// YouTubeAPI.hpp - Header for the you_tube api related stuff.
/// Jun 30, 2021
/// https://discordcoreapi.com
/// \file YouTubeAPI.hpp
#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		class DiscordCoreAPI_Dll you_tube_request_builder : public https_client_core {
		  public:
			you_tube_request_builder(config_manager* configManagerNew);

			song collectFinalSong(const song& songNew);

		  protected:
			static constexpr jsonifier::string_view baseUrl{ "https://www.youtube.com" };

			song constructDownloadInfo(const song& songNew, uint64_t currentRecursionDepth);

			jsonifier::vector<song> collectSearchResults(jsonifier::string_view string, uint64_t limit = 20);

			song collectSingleResult(jsonifier::string_view string);

			virtual ~you_tube_request_builder() = default;
		};

		class DiscordCoreAPI_Dll you_tube_api : public you_tube_request_builder {
		  public:
			you_tube_api(config_manager* configManagerNew, const snowflake guildId);

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

		struct you_tube_request_client {
			jsonifier::string_view clientVersion{ "17.13.3" };
			jsonifier::string_view androidSdkVersion{ "31" };
			jsonifier::string_view clientName{ "android" };
			jsonifier::string_view platform{ "mobile" };
			jsonifier::string_view osName{ "android" };
			jsonifier::string_view osVersion{ "12" };
			jsonifier::string_view hl{ "en-gb" };
			jsonifier::string_view gl{ "us" };
		};

		struct request {
			const bool useSsl{ true };
		};

		struct user {
			const bool lockedSafetyMode{};
		};

		struct you_tube_request_context {
			unordered_map<jsonifier::string, jsonifier::string> captionParams{};
			you_tube_request_client client{};
			request requestVal{};
			user userVal{};
		};

		struct format {
			jsonifier::string contentLength{};
			jsonifier::string audioQuality{};
			jsonifier::string mimeType{};
			int64_t averageBitrate{};
			jsonifier::string url{};
			int64_t bitrate{};
			int64_t itag{};
		};

		struct streaming_data {
			jsonifier::vector<format> adaptiveFormats{};
		};

		struct thumbnail_element {
			jsonifier::string url{};
			int64_t width{};
		};

		struct video_details_thumbnail {
			jsonifier::vector<thumbnail_element> thumbnails{};
		};

		struct video_details {
			jsonifier::string shortDescription{};
			video_details_thumbnail thumbnail{};
			jsonifier::string lengthSeconds{};
			jsonifier::string videoId{};
			jsonifier::string title{};
		};

		struct data {
			streaming_data streamingData{};
			video_details videoDetails{};
		};

		struct you_tube_request {
			const jsonifier::string playlistId{};
			you_tube_request_context context{};
			const bool contentCheckOk{ true };
			const jsonifier::string params{};
			const bool racyCheckOk{ true };
			jsonifier::string videoId{};
		};

		struct video_renderer {
			jsonifier::string videoId{};
		};

		struct video_renderer_type {
			video_renderer videoRenderer{};
		};

		struct item_section_renderer_contents {
			jsonifier::vector<discord_core_internal::video_renderer_type> contents{};
		};

		struct item_section_renderer {
			item_section_renderer_contents itemSectionRendererContents{};
		};

		struct section_list_renderer {
			jsonifier::vector<item_section_renderer> contents{};
		};

		struct primary_contents {
			section_list_renderer sectionListRenderer{};
		};

		struct two_column_search_results_renderer {
			primary_contents primaryContents{};
		};

		struct contents01 {
			two_column_search_results_renderer twoColumnSearchResultsRenderer{};
		};

		struct you_tube_search_results {
			contents01 contents{};
		};
	}
}
