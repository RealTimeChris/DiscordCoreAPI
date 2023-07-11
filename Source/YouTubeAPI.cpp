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

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::User> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::User;
		static constexpr auto parseValue = object("lockedSafetyMode", &ValueType::lockedSafetyMode);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Request> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Request;
		static constexpr auto parseValue = object("useSsl", &ValueType::useSsl);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestClient> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestClient;
		static constexpr auto parseValue = object("clientName", &ValueType::clientName, "androidSdkVersion", &ValueType::androidSdkVersion,
			"clientVersion", &ValueType::clientVersion, "hl", &ValueType::hl, "gl", &ValueType::gl, "osName", &ValueType::osName, "osVersion",
			&ValueType::osVersion, "platform", &ValueType::platform);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestContext> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestContext;
		static constexpr auto parseValue = object("client", &ValueType::client, "captionParams", &ValueType::captionParams, "request",
			&ValueType::request, "user", &ValueType::user);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::YouTubeRequest> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeRequest;
		static constexpr auto parseValue = object("videoId", &ValueType::videoId, "contentCheckOk", &ValueType::contentCheckOk, "racyCheckOk",
			&ValueType::racyCheckOk, "context", &ValueType::context, "playlistId", &ValueType::playlistId, "params", &ValueType::params);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResult> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResult;
		static constexpr auto parseValue = object("type", &ValueType::type, "videoId", &ValueType::songId, "url", &ValueType::viewUrl,
			"descriptionSnippet", &ValueType::description);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Thumbnail> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Thumbnail;
		static constexpr auto parseValue = object("url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::ThumbNails> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::ThumbNails;
		static constexpr auto parseValue = object("thumbnails", &ValueType::thumbNails);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::AccessibilityData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::AccessibilityData;
		static constexpr auto parseValue = object("label", &ValueType::label);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Accessibility> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Accessibility;
		static constexpr auto parseValue = object("accessibilityData", &ValueType::accessibilityData);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::LengthText> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::LengthText;
		static constexpr auto parseValue = object("accessibility", &ValueType::accessibility);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Title> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Title;
		static constexpr auto parseValue = object("runs", &ValueType::runs);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Text> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Text;
		static constexpr auto parseValue = object("text", &ValueType::text);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::SnippetText> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::SnippetText;
		static constexpr auto parseValue = object("runs", &ValueType::runs);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::SnippetTextValue> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::SnippetTextValue;
		static constexpr auto parseValue = object("snippetText", &ValueType::snippetText);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::VideoRenderer> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::VideoRenderer;
		static constexpr auto parseValue = object("videoId", &ValueType::videoId, "thumbnail", &ValueType::thumbnails, "lengthText",
			&ValueType::lengthText, "detailedMetadataSnippets", &ValueType::detailedMetadataSnippets, "title", &ValueType::title);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::VideoRendererType> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::VideoRendererType;
		static constexpr auto parseValue = object("videoRenderer", &ValueType::videoRenderer);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::ItemSectionRendererContents> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::ItemSectionRendererContents;
		static constexpr auto parseValue = object("contents", &ValueType::contents);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::ItemSectionRenderer> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::ItemSectionRenderer;
		static constexpr auto parseValue = object("itemSectionRenderer", &ValueType::itemSectionRendererContents);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::SectionListRenderer> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::SectionListRenderer;
		static constexpr auto parseValue = object("contents", &ValueType::contents);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::PrimaryContents> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::PrimaryContents;
		static constexpr auto parseValue = object("sectionListRenderer", &ValueType::sectionListRenderer);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::TwoColumnSearchResultsRenderer> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::TwoColumnSearchResultsRenderer;
		static constexpr auto parseValue = object("primaryContents", &ValueType::primaryContents);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Contents01> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Contents01;
		static constexpr auto parseValue = object("twoColumnSearchResultsRenderer", &ValueType::twoColumnSearchResultsRenderer);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResults> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResults;
		static constexpr auto parseValue = object("contents", &ValueType::contents);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Format> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Format;
		static constexpr auto parseValue = object("url", &ValueType::url, "mimeType", &ValueType::mimeType, "bitrate", &ValueType::bitrate,
			"contentLength", &ValueType::contentLength);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::StreamingData> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::StreamingData;
		static constexpr auto parseValue = object("formats", &ValueType::formats, "adaptiveFormats", &ValueType::adaptiveFormats);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::Data> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Data;
		static constexpr auto parseValue = object("streamingData", &ValueType::streamingData);
	};
}

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		YouTubeRequestBuilder::YouTubeRequestBuilder(ConfigManager* configManagerNew) noexcept : HttpsClientCore{ configManagerNew->getBotToken() } {
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

		std::vector<Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage.baseUrl = baseUrl;
			dataPackage.relativePath = "/results?search_query=" + urlEncode(collectVideoIdFromSearchQuery(searchQuery).c_str());
			dataPackage.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage));
			if (returnData.responseCode != 200) {
				MessagePrinter::printError<PrintMessageType::Https>(
					"YouTubeRequestBuilder::collectSearchResults() Error: " + std::to_string(returnData.responseCode) + returnData.responseData);
			}
			std::vector<Song> searchResults{};
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
						Song songNew{};
						songNew.type = SongType::YouTube;
						songNew.songId = value02.videoRenderer.videoId;
						if (value02.videoRenderer.title.runs.size() > 0) {
							songNew.songTitle = value02.videoRenderer.title.runs[0].text;
						}
						songNew.viewUrl = baseUrl + "/watch?v=" + songNew.songId + "&hl=en";
						songNew.duration = value02.videoRenderer.lengthText.accessibility.accessibilityData.label;
						if (value02.videoRenderer.detailedMetadataSnippets.size() > 0 &&
							value02.videoRenderer.detailedMetadataSnippets[0].snippetText.runs.size() > 0) {
							songNew.description = value02.videoRenderer.detailedMetadataSnippets[0].snippetText.runs[0].text;
						} else {
							continue;
						}

						if (value02.videoRenderer.thumbnails.thumbNails.size() > 0) {
							songNew.thumbnailUrl =
								value02.videoRenderer.thumbnails.thumbNails[value02.videoRenderer.thumbnails.thumbNails.size() - 1].url;
							songNew.thumbnailUrl = songNew.thumbnailUrl.substr(0, songNew.thumbnailUrl.find(".jpg") + 4);
							searchResults.emplace_back(songNew);
						}
					}
				}
			}
			return searchResults;
		}

		Song YouTubeRequestBuilder::constructDownloadInfo(const Song& songNew, int32_t currentRecursionDepth) {
			HttpsResponseData responseData{};
			try {
				YouTubeRequest requestData{};
				requestData.videoId = songNew.songId;
				HttpsWorkloadData dataPackage02{ HttpsWorkloadType::YouTubeGetSearchResults };
				dataPackage02.baseUrl = "https://music.youtube.com/";
				dataPackage02.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
				dataPackage02.headersToInsert["Origin"] = "https://music.youtube.com";
				dataPackage02.relativePath = "/youtubei/v1/player?key=AIzaSyA8eiZmM1FaDVjRy-df2KTyQ_vz_yYM39w";
				parser.serializeJson(requestData, dataPackage02.content);
				dataPackage02.workloadClass = HttpsWorkloadClass::Post;
				responseData = submitWorkloadAndGetResult(std::move(dataPackage02));
				if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200) {
					MessagePrinter::printError<PrintMessageType::Https>("YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " +
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
				Song newerSong{ songNew };
				std::string downloadBaseUrl{};
				newerSong.type = SongType::YouTube;
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
					DownloadUrl downloadUrl01{};
					downloadUrl01.contentSize = stoull(potentialFormats[maxIndex].contentLength);
					downloadUrl01.urlPath = downloadBaseUrl;
					DownloadUrl downloadUrl02{};
					downloadUrl02.contentSize = stoull(potentialFormats[maxIndex].contentLength);
					downloadUrl02.urlPath = requestNew;
					newerSong.finalDownloadUrls[0] = downloadUrl01;
					newerSong.finalDownloadUrls[1] = downloadUrl02;
					newerSong.viewUrl = newerSong.firstDownloadUrl;
					newerSong.contentLength = downloadUrl02.contentSize;
					newerSong.type = SongType::YouTube;
				}
				return newerSong;
			} catch (const HttpsError& error) {
				if (currentRecursionDepth <= 10) {
					++currentRecursionDepth;
					return constructDownloadInfo(songNew, currentRecursionDepth);
				} else {
					MessagePrinter::printError<PrintMessageType::Https>(
						"YouTubeRequestBuilder::constructDownloadInfo() Error: " + std::string{ error.what() });
					return {};
				}
			}
			return {};
		}

		Song YouTubeRequestBuilder::collectFinalSong(const Song& songNew) {
			Song newerSong{ songNew };
			newerSong.firstDownloadUrl = baseUrl + "/watch?v=" + newerSong.songId + "&hl=en";
			newerSong = constructDownloadInfo(newerSong, 0);
			return newerSong;
		}

		YouTubeAPI::YouTubeAPI(ConfigManager* configManagerNew, const Snowflake guildIdNew) : YouTubeRequestBuilder{ configManagerNew } {
			guildId = guildIdNew;
		}

		void YouTubeAPI::weFailedToDownloadOrDecode(const Song& songNew, std::stop_token& stopToken, int32_t currentRetries) {
			std::this_thread::sleep_for(1s);
			++currentRetries;
			if (currentRetries <= 10) {
				downloadAndStreamAudio(songNew, stopToken, currentRetries);
			} else {
				GuildMemberData guildMember{ GuildMembers::getCachedGuildMember({ .guildMemberId = songNew.addedByUserId, .guildId = guildId }) };
				DiscordCoreClient::getSongAPI(guildId).skip(guildMember, true);
			}
		}

		void YouTubeAPI::downloadAndStreamAudio(const Song& songNew, std::stop_token& token, int32_t currentReconnectTries) {
			try {
				areWeWorkingBool.store(true);
				size_t intervalCount{ songNew.contentLength / (1024 * 1024) + 1 };
				size_t remainder{ songNew.contentLength % (1024 * 1024) };
				size_t currentStart{};
				size_t currentEnd{ intervalCount > 1 ? (1024 * 1024) : remainder };
				std::vector<HttpsWorkloadData> workloadVector{};
				for (size_t x = 0; x < intervalCount; ++x) {
					HttpsWorkloadData workloadData{ HttpsWorkloadType::YouTubeGetSearchResults };
					if (songNew.finalDownloadUrls.size() > 0) {
						if (songNew.finalDownloadUrls[0].urlPath.find(".com") != std::string::npos) {
							workloadData.baseUrl =
								songNew.finalDownloadUrls[0].urlPath.substr(0, songNew.finalDownloadUrls[0].urlPath.find(".com") + 4);
						}
					} else {
						weFailedToDownloadOrDecode(songNew, token, currentReconnectTries);
						areWeWorkingBool.store(false);
						return;
					}
					workloadData.workloadClass = HttpsWorkloadClass::Get;
					workloadData.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
					workloadData.headersToInsert["Connection"] = "Keep-Alive";
					workloadData.headersToInsert["Host"] = songNew.finalDownloadUrls[0].urlPath;
					workloadData.headersToInsert["Origin"] = "https://music.youtube.com";
					workloadData.relativePath =
						songNew.finalDownloadUrls[1].urlPath + "&range=" + std::to_string(currentStart) + "-" + std::to_string(currentEnd);
					workloadVector.emplace_back(std::move(workloadData));
					currentStart = currentEnd;
					currentEnd += x == intervalCount - 2 ? remainder : (1024 * 1024);
				}
				std::basic_string<uint8_t> buffer{};
				MatroskaDemuxer demuxer{};
				size_t index{};
				while (index < intervalCount || !demuxer.areWeDone()) {
					if (index < intervalCount) {
						HttpsResponseData result{ submitWorkloadAndGetResult(std::move(workloadVector[index])) };
						if (result.responseCode != 200) {
							areWeWorkingBool.store(false);
							weFailedToDownloadOrDecode(songNew, token, currentReconnectTries);
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

					AudioFrameData frameData{};
					while (demuxer.collectFrame(frameData)) {
						if (frameData.currentSize != 0) {
							frameData.guildMemberId = songNew.addedByUserId.operator size_t();
							DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
						}
					}
					std::this_thread::sleep_for(1ms);
				}
				areWeWorkingBool.store(false);
				DiscordCoreClient::getVoiceConnection(guildId).skip(false);
				AudioFrameData frameData{};
				frameData.guildMemberId = songNew.addedByUserId.operator size_t();
				DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("YouTubeAPI::downloadAndStreamAudio() Error: " + std::string{ error.what() });
				weFailedToDownloadOrDecode(songNew, token, currentReconnectTries);
				areWeWorkingBool.store(false);
			}
		}

		bool YouTubeAPI::areWeWorking() noexcept {
			return areWeWorkingBool.load();
		}

		std::vector<Song> YouTubeAPI::searchForSong(const std::string& searchQuery) {
			return collectSearchResults(searchQuery);
		}

		Song YouTubeAPI::collectFinalSong(const Song& songNew) {
			return YouTubeRequestBuilder::collectFinalSong(songNew);
		}

	}
}
