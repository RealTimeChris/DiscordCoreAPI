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
			std::string baseUrl{ "https://www.youtube.com" };

			Song constructDownloadInfo(const Song& songNew, uint64_t currentRecursionDepth);

			std::vector<Song> collectSearchResults(const std::string& string);

			virtual Song collectFinalSong(const Song& songNew);
		};

		class DiscordCoreAPI_Dll YouTubeAPI : public YouTubeRequestBuilder {
		  public:
			YouTubeAPI(ConfigManager* configManagerNew, const Snowflake guildId);

			CoRoutine<void, false> downloadAndStreamAudio(const Song songNew,
				NewThreadAwaiter<void, false> threadHandle = NewThreadAwaiter<void, false>{}, uint64_t currentReconnectTries = 0);

			void weFailedToDownloadOrDecode(const Song& songNew, NewThreadAwaiter<void, false> threadHandle, uint64_t currentRetries);

			Song collectFinalSong(const Song& songNew) override;

			std::vector<Song> searchForSong(const std::string& searchQuery);

			bool areWeWorking();

		  protected:
			std::atomic_bool areWeWorkingBool{ false };
			Snowflake guildId{};
		};

		struct DiscordCoreAPI_Dll YouTubeRequestClient {
			std::string clientVersion{ "17.10.35" };
			std::string androidSdkVersion{ "31" };
			std::string clientName{ "ANDROID" };
			std::string platform{ "MOBILE" };
			std::string osName{ "Android" };
			std::string osVersion{ "12" };
			std::string hl{ "en-GB" };
			std::string gl{ "US" };
		};

		struct DiscordCoreAPI_Dll Request {
			const bool useSsl{ true };
		};

		struct DiscordCoreAPI_Dll User {
			const bool lockedSafetyMode{};
		};

		struct DiscordCoreAPI_Dll YouTubeRequestContext {
			std::unordered_map<std::string, std::string> captionParams{};
			YouTubeRequestClient client{};
			Request request{};
			User user{};
		};

		struct DiscordCoreAPI_Dll YouTubeRequest {
			const bool contentCheckOk{ true };
			YouTubeRequestContext context{};
			const std::string playlistId{};
			const bool racyCheckOk{ true };
			const std::string params{};
			std::string videoId{};
		};

		struct DiscordCoreAPI_Dll Format {
			std::string contentLength{};
			std::string mimeType{};
			std::string url{};
			uint64_t bitrate{};
		};

		struct DiscordCoreAPI_Dll StreamingData {
			std::vector<Format> adaptiveFormats{};
			std::vector<Format> formats{};
		};

		struct DiscordCoreAPI_Dll Data {
			StreamingData streamingData{};
		};

		struct DiscordCoreAPI_Dll YouTubeSearchResult {
			std::string description{};
			std::string viewUrl{};
			std::string songId{};
			SongType type{};
		};

		struct DiscordCoreAPI_Dll Thumbnail {
			std::string url{};
		};

		struct DiscordCoreAPI_Dll ThumbNails {
			std::vector<Thumbnail> thumbNails{};
		};

		struct DiscordCoreAPI_Dll AccessibilityData {
			std::string label{};
		};

		struct DiscordCoreAPI_Dll Accessibility {
			AccessibilityData accessibilityData{};
		};

		struct DiscordCoreAPI_Dll LengthText {
			Accessibility accessibility{};
		};

		struct DiscordCoreAPI_Dll Text {
			std::string text{};
		};

		struct DiscordCoreAPI_Dll Title {
			std::vector<Text> runs{};
		};

		struct DiscordCoreAPI_Dll SnippetText {
			std::vector<Text> runs{};
		};

		struct DiscordCoreAPI_Dll SnippetTextValue {
			SnippetText snippetText{};
		};

		struct DiscordCoreAPI_Dll VideoRenderer {
			std::vector<SnippetTextValue> detailedMetadataSnippets{};
			ThumbNails thumbnails{};
			LengthText lengthText{};
			std::string videoId{};
			Title title{};
		};

		struct DiscordCoreAPI_Dll VideoRendererType {
			VideoRenderer videoRenderer{};
		};

		struct DiscordCoreAPI_Dll ItemSectionRendererContents {
			std::vector<DiscordCoreInternal::VideoRendererType> contents{};
		};

		struct DiscordCoreAPI_Dll ItemSectionRenderer {
			ItemSectionRendererContents itemSectionRendererContents{};
		};

		struct DiscordCoreAPI_Dll SectionListRenderer {
			std::vector<ItemSectionRenderer> contents{};
		};

		struct DiscordCoreAPI_Dll PrimaryContents {
			SectionListRenderer sectionListRenderer{};
		};

		struct DiscordCoreAPI_Dll TwoColumnSearchResultsRenderer {
			PrimaryContents primaryContents{};
		};

		struct DiscordCoreAPI_Dll Contents01 {
			TwoColumnSearchResultsRenderer twoColumnSearchResultsRenderer{};
		};

		struct DiscordCoreAPI_Dll YouTubeSearchResults {
			Contents01 contents{};
		};
	}
}
