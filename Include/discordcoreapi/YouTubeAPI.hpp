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
/// YouTubeAPI.hpp - Header for the YouTube API related stuff.
/// Jun 30, 2021
/// https://discordcoreapi.com
/// \file YouTubeAPI.hpp

#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		class DiscordCoreAPI_Dll YouTubeRequestBuilder : public HttpsClientCore {
		  public:
			YouTubeRequestBuilder(ConfigManager* configManagerNew);

		  protected:
			inline static constexpr jsonifier::string_view baseUrl{ "https://www.youtube.com" };

			Song constructDownloadInfo(const Song& songNew, uint64_t currentRecursionDepth);

			jsonifier::vector<Song> collectSearchResults(jsonifier::string_view string);

			virtual Song collectFinalSong(const Song& songNew);

			Song collectSingleResult(jsonifier::string_view string);

			virtual ~YouTubeRequestBuilder() = default;
		};

		class DiscordCoreAPI_Dll YouTubeAPI : public YouTubeRequestBuilder {
		  public:
			YouTubeAPI(ConfigManager* configManagerNew, const Snowflake guildId);

			CoRoutine<void, false> downloadAndStreamAudio(const Song songNew, NewThreadAwaiter<void, false> threadHandle = NewThreadAwaiter<void, false>{},
				uint64_t currentReconnectTries = 0);

			jsonifier::vector<Song> searchForSong(jsonifier::string_view searchQuery);

			void weFailedToDownloadOrDecode(const Song& songNew);

			Song collectFinalSong(const Song& songNew) override;

			Song collectSingleResult(jsonifier::string_view string);

			bool areWeWorking();

		  protected:
			std::atomic_bool areWeWorkingBool{ false };
			Snowflake guildId{};
		};

		struct YouTubeRequestClient {
			inline static constexpr jsonifier::string_view clientVersion{ "17.10.35" };
			inline static constexpr jsonifier::string_view androidSdkVersion{ "31" };
			inline static constexpr jsonifier::string_view clientName{ "ANDROID" };
			inline static constexpr jsonifier::string_view platform{ "MOBILE" };
			inline static constexpr jsonifier::string_view osName{ "Android" };
			inline static constexpr jsonifier::string_view osVersion{ "12" };
			inline static constexpr jsonifier::string_view hl{ "en-GB" };
			inline static constexpr jsonifier::string_view gl{ "US" };
		};

		struct Request {
			const bool useSsl{ true };
		};

		struct User {
			const bool lockedSafetyMode{};
		};

		struct YouTubeRequestContext {
			UnorderedMap<jsonifier::string, jsonifier::string> captionParams{};
			YouTubeRequestClient client{};
			Request request{};
			User user{};
		};

		struct Format {
			int64_t itag;
			jsonifier::string url;
			jsonifier::string mimeType;
			int64_t bitrate;
			jsonifier::string contentLength;
			int64_t averageBitrate;
			jsonifier::string audioQuality;
		};

		struct StreamingData {
			std::vector<Format> adaptiveFormats;
		};

		struct ThumbnailElement {
			jsonifier::string url;
			int64_t width;
			int64_t height;
		};

		struct VideoDetailsThumbnail {
			std::vector<ThumbnailElement> thumbnails;
		};

		struct VideoDetails {
			VideoDetailsThumbnail thumbnail;
			jsonifier::string lengthSeconds;
			jsonifier::string videoId;
			jsonifier::string title;
		};

		struct Data {
			StreamingData streamingData;
			VideoDetails videoDetails;
		};

		struct YouTubeRequest {
			const jsonifier::string playlistId{};
			const bool contentCheckOk{ true };
			const jsonifier::string params{};
			YouTubeRequestContext context{};
			const bool racyCheckOk{ true };
			jsonifier::string videoId{};
		};

		struct YouTubeSearchResult {
			jsonifier::string description{};
			jsonifier::string viewUrl{};
			jsonifier::string songId{};
			SongType type{};
		};

		struct VideoRenderer {
			jsonifier::string videoId{};
		};

		struct VideoRendererType {
			VideoRenderer videoRenderer{};
		};

		struct ItemSectionRendererContents {
			jsonifier::vector<DiscordCoreInternal::VideoRendererType> contents{};
		};

		struct ItemSectionRenderer {
			ItemSectionRendererContents itemSectionRendererContents{};
		};

		struct SectionListRenderer {
			jsonifier::vector<ItemSectionRenderer> contents{};
		};

		struct PrimaryContents {
			SectionListRenderer sectionListRenderer{};
		};

		struct TwoColumnSearchResultsRenderer {
			PrimaryContents primaryContents{};
		};

		struct Contents01 {
			TwoColumnSearchResultsRenderer twoColumnSearchResultsRenderer{};
		};

		struct YouTubeSearchResults {
			Contents01 contents{};
		};
	}
}
