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
/// YouTubeAPI.hpp - Header for the YouTube API related stuff.
/// Jun 30, 2021
/// https://discordcoreapi.com
/// \file YouTubeAPI.hpp

#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreInternal {

	class YouTubeRequestBuilder : public HttpsClientCore {
	  public:
		YouTubeRequestBuilder& operator=(YouTubeRequestBuilder&& other) noexcept = default;
		YouTubeRequestBuilder(YouTubeRequestBuilder&& other) noexcept = default;

		YouTubeRequestBuilder(DiscordCoreAPI::ConfigManager* configManagerNew) noexcept;

	  protected:
		std::string baseUrl{ "https://www.youtube.com" };

		DiscordCoreAPI::Song constructDownloadInfo(const DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth);

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const std::string& string);

		virtual DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong);
	};

	class YouTubeAPI : public YouTubeRequestBuilder {
	  public:
		YouTubeAPI& operator=(YouTubeAPI&& other) noexcept = default;
		YouTubeAPI(YouTubeAPI&& other) noexcept = default;

		YouTubeAPI(DiscordCoreAPI::ConfigManager* configManagerNew, const DiscordCoreAPI::Snowflake guildId);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token& token, int32_t currentReconnectTries);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token&, int32_t currentRetries);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong) override;

		std::vector<DiscordCoreAPI::Song> searchForSong(const std::string& searchQuery);

		bool areWeWorking() noexcept;

	  protected:
		std::atomic_bool areWeWorkingBool{ false };
		DiscordCoreAPI::Snowflake guildId{};
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
		std::unordered_map<std::string, std::string> captionParams{};
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
		int32_t bitrate{};
	};

	struct StreamingData {
		std::vector<Format> adaptiveFormats{};
		std::vector<Format> formats{};
	};

	struct Data {
		StreamingData streamingData{};
	};

	struct YouTubeSearchResult {
		DiscordCoreAPI::SongType type{};
		std::string description{};
		std::string viewUrl{};
		std::string songId{};
	};

	struct Thumbnail {
		std::string url{};
	};

	struct ThumbNails {
		std::vector<Thumbnail> thumbNails{};
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
		std::vector<Text> runs{};
	};

	struct SnippetText {
		std::vector<Text> runs{};
	};

	struct SnippetTextValue {
		SnippetText snippetText{};
	};

	struct VideoRenderer {
		std::vector<SnippetTextValue> detailedMetadataSnippets{};
		ThumbNails thumbnails{};
		LengthText lengthText{};
		std::string videoId{};
		Title title{};
	};

	struct VideoRendererType {
		VideoRenderer videoRenderer{};
	};

	struct ItemSectionRendererContents {
		std::vector<DiscordCoreInternal::VideoRendererType> contents{};
	};

	struct ItemSectionRenderer {
		ItemSectionRendererContents itemSectionRendererContents{};
	};

	struct SectionListRenderer {
		std::vector<ItemSectionRenderer> contents{};
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
