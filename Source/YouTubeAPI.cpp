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
/// YouTubeAPI.cpp - Soure file for the YouTube API related stuff.
/// Jun 30, 2021
/// https://discordcoreapi.com
/// \file YouTubeAPI.cpp

#include <discordcoreapi/YouTubeAPI.hpp>
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/TCPConnection.hpp>
#include <discordcoreapi/Utilities/AudioEncoder.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/Utilities/Demuxers.hpp>

namespace DiscordCoreAPI {

	namespace Globals {
		extern std::atomic_bool doWeQuit;
	}

}

namespace Jsonifier {

	template<> struct Core<DiscordCoreInternal::User> {
		using ValueType = DiscordCoreInternal::User;
		static constexpr auto parseValue = object("lockedSafetyMode", &ValueType::lockedSafetyMode);
	};

	template<> struct Core<DiscordCoreInternal::Request> {
		using ValueType = DiscordCoreInternal::Request;
		static constexpr auto parseValue = object("useSsl", &ValueType::useSsl);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeRequestClient> {
		using ValueType = DiscordCoreInternal::YouTubeRequestClient;
		static constexpr auto parseValue = object("clientName", &ValueType::clientName, "androidSdkVersion", &ValueType::androidSdkVersion,
			"clientVersion", &ValueType::clientVersion, "hl", &ValueType::hl, "gl", &ValueType::gl, "osName", &ValueType::osName, "osVersion",
			&ValueType::osVersion, "platform", &ValueType::platform);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeRequestContext> {
		using ValueType = DiscordCoreInternal::YouTubeRequestContext;
		static constexpr auto parseValue = object("client", &ValueType::client, "captionParams", &ValueType::captionParams, "request",
			&ValueType::request, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeRequest> {
		using ValueType = DiscordCoreInternal::YouTubeRequest;
		static constexpr auto parseValue = object("videoId", &ValueType::videoId, "contentCheckOk", &ValueType::contentCheckOk, "racyCheckOk",
			&ValueType::racyCheckOk, "context", &ValueType::context, "playlistId", &ValueType::playlistId, "params", &ValueType::params);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeSearchResult> {
		using ValueType = DiscordCoreInternal::YouTubeSearchResult;
		static constexpr auto parseValue = object("type", &ValueType::type, "videoId", &ValueType::songId, "url", &ValueType::viewUrl,
			"descriptionSnippet", &ValueType::description);
	};

	template<> struct Core<DiscordCoreInternal::Thumbnail> {
		using ValueType = DiscordCoreInternal::Thumbnail;
		static constexpr auto parseValue = object("url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreInternal::ThumbNails> {
		using ValueType = DiscordCoreInternal::ThumbNails;
		static constexpr auto parseValue = object("thumbnails", &ValueType::thumbNails);
	};

	template<> struct Core<DiscordCoreInternal::AccessibilityData> {
		using ValueType = DiscordCoreInternal::AccessibilityData;
		static constexpr auto parseValue = object("label", &ValueType::label);
	};

	template<> struct Core<DiscordCoreInternal::Accessibility> {
		using ValueType = DiscordCoreInternal::Accessibility;
		static constexpr auto parseValue = object("accessibilityData", &ValueType::accessibilityData);
	};

	template<> struct Core<DiscordCoreInternal::LengthText> {
		using ValueType = DiscordCoreInternal::LengthText;
		static constexpr auto parseValue = object("accessibility", &ValueType::accessibility);
	};

	template<> struct Core<DiscordCoreInternal::Title> {
		using ValueType = DiscordCoreInternal::Title;
		static constexpr auto parseValue = object("runs", &ValueType::runs);
	};

	template<> struct Core<DiscordCoreInternal::Text> {
		using ValueType = DiscordCoreInternal::Text;
		static constexpr auto parseValue = object("text", &ValueType::text);
	};

	template<> struct Core<DiscordCoreInternal::SnippetText> {
		using ValueType = DiscordCoreInternal::SnippetText;
		static constexpr auto parseValue = object("runs", &ValueType::runs);
	};

	template<> struct Core<DiscordCoreInternal::SnippetTextValue> {
		using ValueType = DiscordCoreInternal::SnippetTextValue;
		static constexpr auto parseValue = object("snippetText", &ValueType::snippetText);
	};

	template<> struct Core<DiscordCoreInternal::VideoRenderer> {
		using ValueType = DiscordCoreInternal::VideoRenderer;
		static constexpr auto parseValue = object("videoId", &ValueType::videoId, "thumbnail", &ValueType::thumbnails, "lengthText",
			&ValueType::lengthText, "detailedMetadataSnippets", &ValueType::detailedMetadataSnippets, "title", &ValueType::title);
	};

	template<> struct Core<DiscordCoreInternal::VideoRendererType> {
		using ValueType = DiscordCoreInternal::VideoRendererType;
		static constexpr auto parseValue = object("videoRenderer", &ValueType::videoRenderer);
	};

	template<> struct Core<DiscordCoreInternal::ItemSectionRendererContents> {
		using ValueType = DiscordCoreInternal::ItemSectionRendererContents;
		static constexpr auto parseValue = object("contents", &ValueType::contents);
	};

	template<> struct Core<DiscordCoreInternal::ItemSectionRenderer> {
		using ValueType = DiscordCoreInternal::ItemSectionRenderer;
		static constexpr auto parseValue = object("itemSectionRenderer", &ValueType::itemSectionRendererContents);
	};

	template<> struct Core<DiscordCoreInternal::SectionListRenderer> {
		using ValueType = DiscordCoreInternal::SectionListRenderer;
		static constexpr auto parseValue = object("contents", &ValueType::contents);
	};

	template<> struct Core<DiscordCoreInternal::PrimaryContents> {
		using ValueType = DiscordCoreInternal::PrimaryContents;
		static constexpr auto parseValue = object("sectionListRenderer", &ValueType::sectionListRenderer);
	};

	template<> struct Core<DiscordCoreInternal::TwoColumnSearchResultsRenderer> {
		using ValueType = DiscordCoreInternal::TwoColumnSearchResultsRenderer;
		static constexpr auto parseValue = object("primaryContents", &ValueType::primaryContents);
	};

	template<> struct Core<DiscordCoreInternal::Contents01> {
		using ValueType = DiscordCoreInternal::Contents01;
		static constexpr auto parseValue = object("twoColumnSearchResultsRenderer", &ValueType::twoColumnSearchResultsRenderer);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeSearchResults> {
		using ValueType = DiscordCoreInternal::YouTubeSearchResults;
		static constexpr auto parseValue = object("contents", &ValueType::contents);
	};

	template<> struct Core<DiscordCoreInternal::Format> {
		using ValueType = DiscordCoreInternal::Format;
		static constexpr auto parseValue = object("url", &ValueType::url, "mimeType", &ValueType::mimeType, "bitrate", &ValueType::bitrate,
			"contentLength", &ValueType::contentLength);
	};

	template<> struct Core<DiscordCoreInternal::StreamingData> {
		using ValueType = DiscordCoreInternal::StreamingData;
		static constexpr auto parseValue = object("formats", &ValueType::formats, "adaptiveFormats", &ValueType::adaptiveFormats);
	};

	template<> struct Core<DiscordCoreInternal::Data> {
		using ValueType = DiscordCoreInternal::Data;
		static constexpr auto parseValue = object("streamingData", &ValueType::streamingData);
	};
}

namespace DiscordCoreInternal {

	YouTubeRequestBuilder::YouTubeRequestBuilder(DiscordCoreAPI::ConfigManager* configManagerNew) noexcept
		: HttpsClientCore{ configManagerNew->getBotToken() } {
	}

	std::string collectVideoIdFromSearchQuery(const std::string& string) {
		if (string.find("youtube.com") != std::string::npos) {
			std::string returnString{};
			returnString = string.substr(string.find("v=") + 2);
			return returnString;
		} else {
			return string;
		}
	}

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
		HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
		dataPackage.baseUrl = baseUrl;
		dataPackage.relativePath = "/results?search_query=" + DiscordCoreAPI::urlEncode(collectVideoIdFromSearchQuery(searchQuery).c_str());
		dataPackage.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage));
		if (returnData.responseCode != 200) {
			Globals::MessagePrinter::printError<Globals::MessageType::Https>(
				"YouTubeRequestBuilder::collectSearchResults() Error: " + std::to_string(returnData.responseCode) + returnData.responseData);
		}
		std::vector<DiscordCoreAPI::Song> searchResults{};
		auto varInitFind = returnData.responseData.find("var ytInitialData = ");
		if (varInitFind != std::string::npos) {
			std::string newString00 = "var ytInitialData = ";
			std::string newString = returnData.responseData.substr(varInitFind + newString00.length());
			std::string stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			YouTubeSearchResults youtubeSearchResults{};
			parser.parseJson<true, true>(youtubeSearchResults, newString);
			for (auto& value: youtubeSearchResults.contents.twoColumnSearchResultsRenderer.primaryContents.sectionListRenderer.contents) {
				for (auto& value02: value.itemSectionRendererContents.contents) {
					DiscordCoreAPI::Song newSong{};
					newSong.type = DiscordCoreAPI::SongType::YouTube;
					newSong.songId = value02.videoRenderer.videoId;
					if (value02.videoRenderer.title.runs.size() > 0) {
						newSong.songTitle = value02.videoRenderer.title.runs[0].text;
					}
					newSong.viewUrl = baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
					newSong.duration = value02.videoRenderer.lengthText.accessibility.accessibilityData.label;
					if (value02.videoRenderer.detailedMetadataSnippets.size() > 0 &&
						value02.videoRenderer.detailedMetadataSnippets[0].snippetText.runs.size() > 0) {
						newSong.description = value02.videoRenderer.detailedMetadataSnippets[0].snippetText.runs[0].text;
					} else {
						continue;
					}

					if (value02.videoRenderer.thumbnails.thumbNails.size() > 0) {
						newSong.thumbnailUrl =
							value02.videoRenderer.thumbnails.thumbNails[value02.videoRenderer.thumbnails.thumbNails.size() - 1].url;
						newSong.thumbnailUrl = newSong.thumbnailUrl.substr(0, newSong.thumbnailUrl.find(".jpg") + 4);
						searchResults.emplace_back(newSong);
					}
				}
			}
		}
		return searchResults;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(const DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth) {
		HttpsResponseData responseData{};
		try {
			YouTubeRequest requestData{};
			requestData.videoId = newSong.songId;
			HttpsWorkloadData dataPackage02{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage02.baseUrl = "https://music.youtube.com/";
			dataPackage02.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
			dataPackage02.headersToInsert["Origin"] = "https://music.youtube.com";
			dataPackage02.relativePath = "/youtubei/v1/player?key=AIzaSyA8eiZmM1FaDVjRy-df2KTyQ_vz_yYM39w";
			parser.serializeJson(requestData, dataPackage02.content);
			dataPackage02.workloadClass = HttpsWorkloadClass::Post;
			responseData = submitWorkloadAndGetResult(std::move(dataPackage02));
			if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200) {
				Globals::MessagePrinter::printError<Globals::MessageType::Https>("YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " +
					std::to_string(responseData.responseCode) + ", " + responseData.responseData);
			}

			Data dataNew{};
			std::vector<Format> potentialFormats{};
			parser.parseJson<true, true>(dataNew, responseData.responseData);
			for (auto& value: dataNew.streamingData.adaptiveFormats) {
				if (value.mimeType == "audio/webm; codecs=\"opus\"") {
					potentialFormats.emplace_back(value);
				}
			}
			size_t currentMax{};
			int64_t maxIndex{ -1 };
			for (size_t x = 0; x < potentialFormats.size(); ++x) {
				if (potentialFormats[x].bitrate > currentMax) {
					maxIndex = static_cast<int64_t>(x);
				}
			}
			DiscordCoreAPI::Song newerSong{ newSong };
			std::string downloadBaseUrl{};
			newerSong.type = DiscordCoreAPI::SongType::YouTube;
			if (maxIndex > -1) {
				auto httpsFind = potentialFormats[maxIndex].url.find("https://");
				auto videoPlaybackFind = potentialFormats[maxIndex].url.find("/videoplayback?");
				if (httpsFind != std::string::npos && videoPlaybackFind != std::string::npos) {
					std::string newString00 = "https://";
					downloadBaseUrl =
						potentialFormats[maxIndex].url.substr(httpsFind + newString00.length(), videoPlaybackFind - newString00.length());
				}
				std::string requestNew = potentialFormats[maxIndex].url.substr(potentialFormats[maxIndex].url.find(".com") + 4);
				newerSong.finalDownloadUrls.resize(3);
				DiscordCoreAPI::DownloadUrl downloadUrl01{};
				downloadUrl01.contentSize = stoull(potentialFormats[maxIndex].contentLength);
				downloadUrl01.urlPath = downloadBaseUrl;
				DiscordCoreAPI::DownloadUrl downloadUrl02{};
				downloadUrl02.contentSize = stoull(potentialFormats[maxIndex].contentLength);
				downloadUrl02.urlPath = requestNew;
				newerSong.finalDownloadUrls[0] = downloadUrl01;
				newerSong.finalDownloadUrls[1] = downloadUrl02;
				newerSong.viewUrl = newerSong.firstDownloadUrl;
				newerSong.contentLength = downloadUrl02.contentSize;
				newerSong.type = DiscordCoreAPI::SongType::YouTube;
			}
			return newerSong;
		} catch (const HttpsError& error) {
			if (currentRecursionDepth <= 10) {
				++currentRecursionDepth;
				return constructDownloadInfo(newSong, currentRecursionDepth);
			} else {
				Globals::MessagePrinter::printError<Globals::MessageType::Https>(
					"YouTubeRequestBuilder::constructDownloadInfo() Error: " + std::string{ error.what() });
				return {};
			}
		}
		return {};
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::collectFinalSong(const DiscordCoreAPI::Song& newSong) {
		DiscordCoreAPI::Song newerSong{ newSong };
		newerSong.firstDownloadUrl = baseUrl + "/watch?v=" + newerSong.songId + "&hl=en";
		newerSong = constructDownloadInfo(newerSong, 0);
		return newerSong;
	}

	YouTubeAPI::YouTubeAPI(DiscordCoreAPI::ConfigManager* configManagerNew, const DiscordCoreAPI::Snowflake guildIdNew)
		: YouTubeRequestBuilder{ configManagerNew } {
		guildId = guildIdNew;
	}

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token& stopToken, int32_t currentRetries) {
		std::this_thread::sleep_for(1s);
		++currentRetries;
		if (currentRetries <= 10) {
			downloadAndStreamAudio(newSong, stopToken, currentRetries);
		} else {
			DiscordCoreAPI::GuildMemberData guildMember{ DiscordCoreAPI::GuildMembers::getCachedGuildMember(
				{ .guildMemberId = newSong.addedByUserId, .guildId = guildId }) };
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(guildId).skip(newSong, guildMember);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token& token, int32_t currentReconnectTries) {
		try {
			areWeWorkingBool.store(true);
			size_t intervalCount{ newSong.contentLength / (1024 * 1024) + 1 };
			size_t remainder{ newSong.contentLength % (1024 * 1024) };
			size_t currentStart{};
			size_t currentEnd{ intervalCount > 1 ? (1024 * 1024) : remainder };
			std::vector<HttpsWorkloadData> workloadVector{};
			for (size_t x = 0; x < intervalCount; ++x) {
				HttpsWorkloadData workloadData{ HttpsWorkloadType::YouTubeGetSearchResults };
				if (newSong.finalDownloadUrls.size() > 0) {
					if (newSong.finalDownloadUrls[0].urlPath.find(".com") != std::string::npos) {
						workloadData.baseUrl = newSong.finalDownloadUrls[0].urlPath.substr(0, newSong.finalDownloadUrls[0].urlPath.find(".com") + 4);
					}
				} else {
					weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					areWeWorkingBool.store(false);
					return;
				}
				workloadData.workloadClass = HttpsWorkloadClass::Get;
				workloadData.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
				workloadData.headersToInsert["Connection"] = "Keep-Alive";
				workloadData.headersToInsert["Host"] = newSong.finalDownloadUrls[0].urlPath;
				workloadData.headersToInsert["Origin"] = "https://music.youtube.com";
				workloadData.relativePath =
					newSong.finalDownloadUrls[1].urlPath + "&range=" + std::to_string(currentStart) + "-" + std::to_string(currentEnd);
				workloadVector.emplace_back(std::move(workloadData));
				currentStart = currentEnd;
				currentEnd += x == intervalCount - 2 ? remainder : (1024 * 1024);
			}
			if (intervalCount == 0 && remainder > 0) {
				HttpsWorkloadData workloadData{ HttpsWorkloadType::YouTubeGetSearchResults };
				if (newSong.finalDownloadUrls.size() > 0) {
					if (newSong.finalDownloadUrls[0].urlPath.find(".com") != std::string::npos) {
						workloadData.baseUrl = newSong.finalDownloadUrls[0].urlPath.substr(0, newSong.finalDownloadUrls[0].urlPath.find(".com") + 4);
					}
				} else {
					weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					areWeWorkingBool.store(false);
					return;
				}
				workloadData.workloadClass = HttpsWorkloadClass::Get;
				workloadData.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
				workloadData.headersToInsert["Connection"] = "Keep-Alive";
				workloadData.headersToInsert["Host"] = newSong.finalDownloadUrls[0].urlPath;
				workloadData.headersToInsert["Origin"] = "https://music.youtube.com";
				workloadData.relativePath =
					newSong.finalDownloadUrls[1].urlPath + "&range=" + std::to_string(currentStart) + "-" + std::to_string(currentEnd);
				workloadVector.emplace_back(std::move(workloadData));
				currentStart = currentEnd;
				intervalCount = 1;
				currentEnd += remainder;
			}
			std::basic_string<uint8_t> buffer{};
			MatroskaDemuxer demuxer{};
			size_t index{};
			while (index < intervalCount || !demuxer.areWeDone()) {
				if (index < intervalCount) {
					HttpsResponseData result{ submitWorkloadAndGetResult(std::move(workloadVector[index])) };
					if (result.responseCode != 200) {
						areWeWorkingBool.store(false);
						weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
						return;
					}
					if (result.responseData.size() > 0) {
						++index;
						auto oldSize = buffer.size();
						buffer.resize(buffer.size() + result.responseData.size());
						std::memcpy(buffer.data() + oldSize, result.responseData.data(), result.responseData.size());
						demuxer.writeData({ reinterpret_cast<uint8_t*>(buffer.data()), buffer.size() });
						demuxer.proceedDemuxing();
					}
				}

				DiscordCoreAPI::AudioFrameData frameData{};
				while (demuxer.collectFrame(frameData)) {
					if (frameData.currentSize != 0) {
						DiscordCoreAPI::DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
					}
				}
				std::this_thread::sleep_for(1ms);
			}
			areWeWorkingBool.store(false);
			DiscordCoreAPI::DiscordCoreClient::getVoiceConnection(guildId).doWeSkip.store(true);
		} catch (const HttpsError& error) {
			Globals::MessagePrinter::printError<Globals::MessageType::Https>(
				"YouTubeAPI::downloadAndStreamAudio() Error: " + std::string{ error.what() });
			weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
			areWeWorkingBool.store(false);
		}
	}

	bool YouTubeAPI::areWeWorking() noexcept {
		return areWeWorkingBool.load();
	}

	std::vector<DiscordCoreAPI::Song> YouTubeAPI::searchForSong(const std::string& searchQuery) {
		return collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(const DiscordCoreAPI::Song& newSong) {
		return YouTubeRequestBuilder::collectFinalSong(newSong);
	}

}
