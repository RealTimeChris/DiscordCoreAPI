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
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		std::string baseUrl{ "https://www.youtube.com" };
		HttpsClient* httpsClient{ nullptr };
		Jsonifier::JsonifierCore parser{};

		DiscordCoreAPI::Song constructDownloadInfo(const DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth);

		std::vector<DiscordCoreAPI::Song> collectSearchResults(const std::string& string);

		virtual DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong);
	};

	class DiscordCoreAPI_Dll YouTubeAPI : public YouTubeRequestBuilder {
	  public:
		YouTubeAPI(DiscordCoreAPI::ConfigManager*, HttpsClient* httpsClient, const DiscordCoreAPI::Snowflake guildId);

		void weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries);

		void downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries);

		std::vector<DiscordCoreAPI::Song> searchForSong(const std::string& searchQuery);

		DiscordCoreAPI::Song collectFinalSong(const DiscordCoreAPI::Song& newSong) override;

	  protected:
		DiscordCoreAPI::Song theSong{};
		DiscordCoreAPI::Snowflake guildId{};
		const int32_t maxBufferSize{ 4096 };
	};
	/*

	if (partialSearchResultsJson["contents"].get(objectContents) == simdjson::error_code::SUCCESS) {
		if (objectContents["twoColumnSearchResultsRenderer"].get(objectContents) == simdjson::error_code::SUCCESS) {
			if (objectContents["primaryContents"].get(objectContents) == simdjson::error_code::SUCCESS) {
				if (objectContents["sectionListRenderer"].get(objectContents) == simdjson::error_code::SUCCESS) {
					if (objectContents["contents"].get(objectContents) == simdjson::error_code::SUCCESS) {
						if (objectContents.at(0).get(objectContents) == simdjson::error_code::SUCCESS) {
							if (objectContents["itemSectionRenderer"].get(objectContents) == simdjson::error_code::SUCCESS) {
								if (objectContents["contents"].get(objectContents) == simdjson::error_code::SUCCESS) {
									for (auto iterator: objectContents) {
										DiscordCoreAPI::Song searchResult{};
										simdjson::ondemand::value object{};
										if (iterator["videoRenderer"].get(object) == simdjson::error_code::SUCCESS) {
											searchResult = DiscordCoreAPI::Song{ object };
										}
										searchResult.type = OTy;
										searchResult.viewUrl = this->baseUrl + "/watch?v=" + searchResult.songId + "&hl=en";
										if (searchResult.description == "" || searchResult.viewUrl == "") {
											continue;
										}
										searchResults.emplace_back(searchResult);
									}
								}
							}
						}
					}
				}
			}
		}
	};

		Song::Song(simdjson::ondemand::value jsonObjectData) {
		this->duration = getString(getObject(getObject(getObject(jsonObjectData, "lengthText"), "accessibility"), "accessibilityData"), "label");
		std::string newString = getString(
			getObject(getArray(getObject(getObject(getArray(jsonObjectData, "detailedMetadataSnippets"), 0), "snippetText"), "runs"), 0), "text");
		if (newString.size() > 256) {
			newString = newString.substr(0, 256);
		}
		this->description = utf8MakeValid(newString);

		this->thumbnailUrl = getString(getObject(getArray(getObject(jsonObjectData, "thumbnail"), "thumbnails"), 0), "url");
		std::string newTitle01 = getString(getObject(getArray(getObject(jsonObjectData, "title"), "runs"), 0), "text");
		if (newTitle01.size() > 256) {
			newTitle01 = newTitle01.substr(0, 256);
		}
		if (newTitle01.size() > 0) {
			this->songTitle = utf8MakeValid(newTitle01);
		}
		std::string newTitle02 = getString(getObject(jsonObjectData, "title"), "simpleText");
		if (newTitle02.size() > 256) {
			newTitle02 = newTitle02.substr(0, 256);
		}
		if (newTitle02.size() > 0) {
			this->songTitle = utf8MakeValid(newTitle02);
		}

		if (newTitle01 != "") {
			this->songTitle = newTitle01;
		} else {
			this->songTitle = newTitle02;
		}

		this->songId = getString(jsonObjectData, "videoId");

		this->trackAuthorization = getString(jsonObjectData, "track_authorization");

		std::vector<MediaTranscoding> theMedia{};
		auto arrayValueNew = getArray(getObject(jsonObjectData, "media"), "transcodings");
		if (arrayValueNew.didItSucceed) {
			for (auto& value: arrayValueNew.arrayValue) {
				MediaTranscoding dataNew{ value.value() };
				theMedia.emplace_back(dataNew);
			}
		}

		bool isItFound{};
		for (auto& value: theMedia) {
			if (value.thePreset == "opus_0_0") {
				isItFound = true;
				this->firstDownloadUrl = value.theUrl;
				this->songId = value.theUrl;
				this->doWeGetSaved = true;
			}
		}
		bool isItFound2{};
		if (!isItFound) {
			for (auto& value: theMedia) {
				if (value.thePreset == "mp3_0_0") {
					this->firstDownloadUrl = value.theUrl;
					this->songId = value.theUrl;
					isItFound2 = true;
				}
			}
		}
		if (!isItFound2 && !isItFound) {
			for (auto& value: theMedia) {
				this->firstDownloadUrl = value.theUrl;
				this->songId = value.theUrl;
			}
		}

		newString = getString(jsonObjectData, "title");
		if (newString.size() > 0) {
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->songTitle = utf8MakeValid(newString);
		}

		newString = getString(jsonObjectData, "description");
		if (newString.size() > 0) {
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->description = utf8MakeValid(newString);
			this->description += ".." ;
		}

		newString = getString(jsonObjectData, "artwork_url");
		if (newString.size() > 0) {
			this->thumbnailUrl = newString;
		}

		newString = getString(getObject(jsonObjectData, "user"), "avatar_url");
		if (newString.size() > 0) {
			this->thumbnailUrl = newString;
		}

		uint32_t theDuration = getUint32(jsonObjectData, "duration");
		if (theDuration != 0) {
			this->duration = TimeStamp::convertMsToDurationString(theDuration);
		}

		newString = getString(jsonObjectData, "permalink_url");
		if (newString.size() > 0) {
			this->viewUrl = newString;
		}
	}*/

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
		const bool useSSL{ true };
	};

	struct DiscordCoreAPI_Dll User {
		const bool lockedSafetyMode{ false };
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
		int bitrate{};
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

