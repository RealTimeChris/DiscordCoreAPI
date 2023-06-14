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

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll YouTubeRequestBuilder {
	  public:
		YouTubeRequestBuilder() noexcept = default;

	  protected:
		std::string baseUrl{ "https://www.youtube.com" };
		DiscordCoreAPI::ConfigManager* configManager{};
		Jsonifier::JsonifierCore parser{};
		HttpsClient* httpsClient{};

		DiscordCoreAPI::Song constructDownloadInfo(const DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth);

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const std::string& string);

		virtual DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong);
	};

	class DiscordCoreAPI_Dll YouTubeAPI : public YouTubeRequestBuilder {
	  public:
		YouTubeAPI(DiscordCoreAPI::DiscordCoreClient*, HttpsClient* httpsClient, const DiscordCoreAPI::Snowflake guildId);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token& token, int32_t currentReconnectTries);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token&, int32_t currentRetries);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong) override;

		std::vector<DiscordCoreAPI::Song> searchForSong(const std::string& searchQuery);

		bool areWeWorking() noexcept;

	  protected:
		DiscordCoreAPI::DiscordCoreClient* discordCoreClient{};
		std::atomic_bool areWeWorkingBool{ false };
		DiscordCoreAPI::Snowflake guildId{};
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
		int32_t bitrate{};
	};

	struct DiscordCoreAPI_Dll StreamingData {
		std::vector<Format> adaptiveFormats{};
		std::vector<Format> formats{};
	};

	struct DiscordCoreAPI_Dll Data {
		StreamingData streamingData{};
	};

	struct DiscordCoreAPI_Dll YouTubeSearchResult {
		DiscordCoreAPI::SongType type{};
		std::string description{};
		std::string viewUrl{};
		std::string songId{};
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

	struct DiscordCoreAPI_Dll ItemSectionRenderer {
		std::vector<VideoRendererType> contents{};
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
