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

			Jsonifier::Vector<Song> collectSearchResults(const std::string& string);

			virtual Song collectFinalSong(const Song& songNew);
		};

		class DiscordCoreAPI_Dll YouTubeAPI : public YouTubeRequestBuilder {
		  public:
			YouTubeAPI(ConfigManager* configManagerNew, const Snowflake guildId);

			CoRoutine<void, false> downloadAndStreamAudio(const Song songNew, NewThreadAwaiter<void, false> threadHandle = NewThreadAwaiter<void, false>{},
				uint64_t currentReconnectTries = 0);

			void weFailedToDownloadOrDecode(const Song& songNew, NewThreadAwaiter<void, false> threadHandle, uint64_t currentRetries);

			Song collectFinalSong(const Song& songNew) override;

			Jsonifier::Vector<Song> searchForSong(const std::string& searchQuery);

			bool areWeWorking();

		  protected:
			std::atomic_bool areWeWorkingBool{ false };
			Snowflake guildId{};
		};

		struct YouTubeRequestClient {
			std::string clientVersion{ "17.10.35" };
			std::string androidSdkVersion{ "31" };
			std::string clientName{ "ANDROID" };
			std::string platform{ "MOBILE" };
			std::string osName{ "Android" };
			std::string osVersion{ "12" };
			std::string hl{ "en-GB" };
			std::string gl{ "US" };
		};

		struct Request {
			const bool useSsl{ true };
		};

		struct User {
			const bool lockedSafetyMode{};
		};

		struct YouTubeRequestContext {
			UnorderedMap<std::string, std::string> captionParams{};
			YouTubeRequestClient client{};
			Request request{};
			User user{};
		};

		struct YouTubeRequest {
			const bool contentCheckOk{ true };
			YouTubeRequestContext context{};
			const std::string playlistId{};
			const bool racyCheckOk{ true };
			const std::string params{};
			std::string videoId{};
		};

		struct Format {
			std::string contentLength{};
			std::string mimeType{};
			std::string url{};
			uint64_t bitrate{};
		};

		struct StreamingData {
			Jsonifier::Vector<Format> adaptiveFormats{};
			Jsonifier::Vector<Format> formats{};
		};

		struct Data {
			StreamingData streamingData{};
		};

		struct YouTubeSearchResult {
			std::string description{};
			std::string viewUrl{};
			std::string songId{};
			SongType type{};
		};

		struct Thumbnail {
			std::string url{};
		};

		struct ThumbNails {
			Jsonifier::Vector<Thumbnail> thumbNails{};
		};

		struct AccessibilityData {
			std::string label{};
		};

		struct Accessibility {
			AccessibilityData accessibilityData{};
		};

		struct LengthText {
			Accessibility accessibility{};
		};

		struct Text {
			std::string text{};
		};

		struct Title {
			Jsonifier::Vector<Text> runs{};
		};

		struct SnippetText {
			Jsonifier::Vector<Text> runs{};
		};

		struct SnippetTextValue {
			SnippetText snippetText{};
		};

		struct VideoRenderer {
			Jsonifier::Vector<SnippetTextValue> detailedMetadataSnippets{};
			ThumbNails thumbnails{};
			LengthText lengthText{};
			std::string videoId{};
			Title title{};
		};

		struct VideoRendererType {
			VideoRenderer videoRenderer{};
		};

		struct ItemSectionRendererContents {
			Jsonifier::Vector<DiscordCoreInternal::VideoRendererType> contents{};
		};

		struct ItemSectionRenderer {
			ItemSectionRendererContents itemSectionRendererContents{};
		};

		struct SectionListRenderer {
			Jsonifier::Vector<ItemSectionRenderer> contents{};
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
