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
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/AudioEncoder.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/Utilities/Demuxers.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ThumbnailElement> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ThumbnailElement;
		static constexpr auto parseValue = createObject("url", &ValueType::url);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Background> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Background;
		static constexpr auto parseValue = createObject("thumbnails", &ValueType::thumbnails);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoOwnerRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoOwnerRenderer;
		static constexpr auto parseValue = createObject("thumbnail", &ValueType::thumbnail);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Owner> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Owner;
		static constexpr auto parseValue = createObject("videoOwnerRenderer", &ValueType::video_owner_renderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoSecondaryInfoRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoSecondaryInfoRenderer;
		static constexpr auto parseValue = createObject("owner", &ValueType::owner);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ResultsContent> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ResultsContent;
		static constexpr auto parseValue = createObject("videoPrimaryInfoRenderer", &ValueType::video_secondary_info_renderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ResultsResults> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ResultsResults;
		static constexpr auto parseValue = createObject("contents", &ValueType::contents);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Results> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Results;
		static constexpr auto parseValue = createObject("results", &ValueType::results);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::TwoColumnWatchNextResults> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::TwoColumnWatchNextResults;
		static constexpr auto parseValue = createObject("results", &ValueType::results);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Contents> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Contents;
		static constexpr auto parseValue = createObject("twoColumnWatchNextResults", &ValueType::two_column_watch_next_results);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::PlayerOverlayAutoplayRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::PlayerOverlayAutoplayRenderer;
		static constexpr auto parseValue = createObject("background", &ValueType::background, "videoId", &ValueType::video_id);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::PlayerOverlayRendererAutoplay> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::PlayerOverlayRendererAutoplay;
		static constexpr auto parseValue = createObject("playerOverlayAutoplayRenderer", &ValueType::player_overlay_autoplay_renderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::PlayerOverlayRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::PlayerOverlayRenderer;
		static constexpr auto parseValue = createObject("autoplay", &ValueType::autoplay);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::PlayerOverlays> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::PlayerOverlays;
		static constexpr auto parseValue = createObject("playerOverlayRenderer", &ValueType::player_overlay_renderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Welcome> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Welcome;
		static constexpr auto parseValue = createObject("contents", &ValueType::contents, "playerOverlays", &ValueType::player_overlays);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::User> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::User;
		static constexpr auto parseValue = createObject("lockedSafetyMode", &ValueType::lockedSafetyMode);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Request> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Request;
		static constexpr auto parseValue = createObject("useSsl", &ValueType::useSsl);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestClient> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestClient;
		static constexpr auto parseValue =
			createObject("clientName", &ValueType::clientName, "androidSdkVersion", &ValueType::androidSdkVersion, "clientVersion", &ValueType::clientVersion, "hl", &ValueType::hl,
				"gl", &ValueType::gl, "osName", &ValueType::osName, "osVersion", &ValueType::osVersion, "platform", &ValueType::platform);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestContext> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeRequestContext;
		static constexpr auto parseValue =
			createObject("client", &ValueType::client, "captionParams", &ValueType::captionParams, "request", &ValueType::request, "user", &ValueType::user);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::YouTubeRequest> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::YouTubeRequest;
		static constexpr auto parseValue = createObject("videoId", &ValueType::videoId, "contentCheckOk", &ValueType::contentCheckOk, "racyCheckOk", &ValueType::racyCheckOk,
			"context", &ValueType::context, "playlistId", &ValueType::playlistId, "params", &ValueType::params);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResult> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResult;
		static constexpr auto parseValue =
			createObject("type", &ValueType::type, "videoId", &ValueType::songId, "url", &ValueType::viewUrl, "descriptionSnippet", &ValueType::description);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Thumbnail> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Thumbnail;
		static constexpr auto parseValue = createObject("url", &ValueType::url);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ThumbNails> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ThumbNails;
		static constexpr auto parseValue = createObject("thumbnails", &ValueType::thumbNails);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::AccessibilityData> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::AccessibilityData;
		static constexpr auto parseValue = createObject("label", &ValueType::label);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Accessibility> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Accessibility;
		static constexpr auto parseValue = createObject("accessibilityData", &ValueType::accessibilityData);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::LengthText> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::LengthText;
		static constexpr auto parseValue = createObject("accessibility", &ValueType::accessibility);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Title> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Title;
		static constexpr auto parseValue = createObject("runs", &ValueType::runs);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Text> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Text;
		static constexpr auto parseValue = createObject("text", &ValueType::text);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::SnippetText> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::SnippetText;
		static constexpr auto parseValue = createObject("runs", &ValueType::runs);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::SnippetTextValue> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::SnippetTextValue;
		static constexpr auto parseValue = createObject("snippetText", &ValueType::snippetText);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoRenderer;
		static constexpr auto parseValue = createObject("videoId", &ValueType::videoId, "thumbnail", &ValueType::thumbnails, "lengthText", &ValueType::lengthText,
			"detailedMetadataSnippets", &ValueType::detailedMetadataSnippets, "title", &ValueType::title);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoRendererType> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoRendererType;
		static constexpr auto parseValue = createObject("videoRenderer", &ValueType::videoRenderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ItemSectionRendererContents> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ItemSectionRendererContents;
		static constexpr auto parseValue = createObject("contents", &ValueType::contents);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ItemSectionRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ItemSectionRenderer;
		static constexpr auto parseValue = createObject("itemSectionRenderer", &ValueType::itemSectionRendererContents);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::SectionListRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::SectionListRenderer;
		static constexpr auto parseValue = createObject("contents", &ValueType::contents);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::PrimaryContents> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::PrimaryContents;
		static constexpr auto parseValue = createObject("sectionListRenderer", &ValueType::sectionListRenderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::TwoColumnSearchResultsRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::TwoColumnSearchResultsRenderer;
		static constexpr auto parseValue = createObject("primaryContents", &ValueType::primaryContents);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Contents01> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Contents01;
		static constexpr auto parseValue = createObject("twoColumnSearchResultsRenderer", &ValueType::twoColumnSearchResultsRenderer);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResults> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::YouTubeSearchResults;
		static constexpr auto parseValue = createObject("contents", &ValueType::contents);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Format> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::Format;
		static constexpr auto parseValue =
			createObject("url", &ValueType::url, "mimeType", &ValueType::mimeType, "bitrate", &ValueType::bitrate, "contentLength", &ValueType::contentLength);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::StreamingData> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::StreamingData;
		static constexpr auto parseValue = createObject("formats", &ValueType::formats, "adaptiveFormats", &ValueType::adaptiveFormats);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Data> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Data;
		static constexpr auto parseValue = createObject("streamingData", &ValueType::streamingData);
	};
}

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		YouTubeRequestBuilder::YouTubeRequestBuilder(ConfigManager* configManagerNew) : HttpsClientCore{ configManagerNew->getBotToken() } {
		}

		std::string collectVideoIdFromSearchQuery(const std::string& string) {
			if (string.find("youtube") != std::string::npos) {
				std::string returnString{};
				returnString = string.substr(string.find("v=") + 2);
				return returnString;
			} else {
				return string;
			}
		}

		Song YouTubeAPI::collectSingleResult(const std::string& searchQuery) {
			return YouTubeRequestBuilder::collectSingleResult(searchQuery);
		}

		Song YouTubeRequestBuilder::collectSingleResult(const std::string& searchQuery) {
			Song songNew{};
			songNew.type	= SongType::YouTube;
			songNew.songId	= collectVideoIdFromSearchQuery(searchQuery);
			songNew			= constructDownloadInfo(songNew, 0);
			auto newResults = collectSearchResults(searchQuery);
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage.headersToInsert["Connection"] = "Keep-Alive";
			dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36";
			dataPackage.baseUrl						  = baseUrl;
			dataPackage.relativePath				  = "/watch?v=" + songNew.songId;
			dataPackage.workloadClass				  = HttpsWorkloadClass::Get;
			HttpsResponseData returnData			  = submitWorkloadAndGetResult(std::move(dataPackage));
			if (returnData.responseCode != 200) {
				MessagePrinter::printError<PrintMessageType::Https>(
					"YouTubeRequestBuilder::collectSearchResults() Error: " + std::to_string(returnData.responseCode) + returnData.responseData);
			}
			auto varInitFind = returnData.responseData.find("var ytInitialData = ");
			if (varInitFind != std::string::npos) {
				std::string newString00	   = "var ytInitialData = ";
				std::string newString	   = returnData.responseData.substr(varInitFind + newString00.length());
				std::string stringSequence = ";</script><script nonce=";
				newString				   = newString.substr(0, newString.find(stringSequence));
				Welcome youtubeSearchResults{};
				parser.parseJson<true, true>(youtubeSearchResults, newString);
				for (auto& value: newResults) {
					if (value.songId == songNew.songId) {
						songNew.addedByUserId = value.addedByUserId;
						songNew.contentLength = value.contentLength;
						songNew.description	  = value.description;
						songNew.duration	  = value.duration;
						songNew.songTitle	  = value.songTitle;
						songNew.thumbnailUrl  = value.thumbnailUrl;
						songNew.viewUrl		  = value.viewUrl;
						break;
					}
				}
			}
			return songNew;
		}

		jsonifier::vector<Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage.baseUrl			 = baseUrl;
			dataPackage.relativePath	 = "/results?search_query=" + urlEncode(collectVideoIdFromSearchQuery(searchQuery).c_str());
			dataPackage.workloadClass	 = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage));
			if (returnData.responseCode != 200) {
				MessagePrinter::printError<PrintMessageType::Https>(
					"YouTubeRequestBuilder::collectSearchResults() Error: " + std::to_string(returnData.responseCode) + returnData.responseData);
			}
			jsonifier::vector<Song> searchResults{};
			auto varInitFind = returnData.responseData.find("var ytInitialData = ");
			if (varInitFind != std::string::npos) {
				std::string newString00	   = "var ytInitialData = ";
				std::string newString	   = returnData.responseData.substr(varInitFind + newString00.length());
				std::string stringSequence = ";</script><script nonce=";
				newString				   = newString.substr(0, newString.find(stringSequence));
				YouTubeSearchResults youtubeSearchResults{};
				parser.parseJson<true, true>(youtubeSearchResults, newString);
				for (auto& value: youtubeSearchResults.contents.twoColumnSearchResultsRenderer.primaryContents.sectionListRenderer.contents) {
					for (auto& value02: value.itemSectionRendererContents.contents) {
						Song songNew{};
						songNew.type   = SongType::YouTube;
						songNew.songId = value02.videoRenderer.videoId;
						if (value02.videoRenderer.title.runs.size() > 0) {
							songNew.songTitle = value02.videoRenderer.title.runs.at(0).text;
						}
						songNew.viewUrl	 = static_cast<std::string>(baseUrl) + std::string{ "/watch?v=" } + songNew.songId + "&hl=en";
						songNew.duration = value02.videoRenderer.lengthText.accessibility.accessibilityData.label;
						if (value02.videoRenderer.detailedMetadataSnippets.size() > 0 && value02.videoRenderer.detailedMetadataSnippets.at(0).snippetText.runs.size() > 0) {
							songNew.description = value02.videoRenderer.detailedMetadataSnippets.at(0).snippetText.runs.at(0).text;
						} else {
							continue;
						}

						if (value02.videoRenderer.thumbnails.thumbNails.size() > 0) {
							songNew.thumbnailUrl = value02.videoRenderer.thumbnails.thumbNails[value02.videoRenderer.thumbnails.thumbNails.size() - 1].url;
							songNew.thumbnailUrl = songNew.thumbnailUrl.substr(0, songNew.thumbnailUrl.find(".jpg") + 4);
							searchResults.emplace_back(songNew);
						}
					}
				}
			}
			return searchResults;
		}

		Song YouTubeRequestBuilder::constructDownloadInfo(const Song& songNew, uint64_t currentRecursionDepth) {
			HttpsResponseData responseData{};
			try {
				YouTubeRequest requestData{};
				requestData.videoId = songNew.songId;
				HttpsWorkloadData dataPackage02{ HttpsWorkloadType::YouTubeGetSearchResults };
				dataPackage02.baseUrl						= "https://music.youtube.com/";
				dataPackage02.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
				dataPackage02.headersToInsert["Origin"]		= "https://music.youtube.com";
				dataPackage02.relativePath					= "/youtubei/v1/player?key=AIzaSyA8eiZmM1FaDVjRy-df2KTyQ_vz_yYM39w";
				parser.serializeJson<true>(requestData, dataPackage02.content);
				dataPackage02.workloadClass = HttpsWorkloadClass::Post;
				responseData				= submitWorkloadAndGetResult(std::move(dataPackage02));
				if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200) {
					MessagePrinter::printError<PrintMessageType::Https>(
						"YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " + std::to_string(responseData.responseCode) + ", " + responseData.responseData);
				}

				Data dataNew{};
				jsonifier::vector<Format> potentialFormats{};
				parser.parseJson<true, true>(dataNew, responseData.responseData);
				for (auto& value: dataNew.streamingData.adaptiveFormats) {
					if (value.mimeType == "audio/webm; codecs=\"opus\"") {
						potentialFormats.emplace_back(value);
					}
				}
				uint64_t currentMax{};
				int64_t maxIndex{ -1 };
				for (uint64_t x = 0; x < potentialFormats.size(); ++x) {
					if (potentialFormats.at(x).bitrate > currentMax) {
						maxIndex = static_cast<int64_t>(x);
					}
				}
				Song newerSong{ songNew };
				std::string downloadBaseUrl{};
				newerSong.type = SongType::YouTube;
				if (maxIndex > -1) {
					auto httpsFind		   = potentialFormats[maxIndex].url.find("https://");
					auto videoPlaybackFind = potentialFormats[maxIndex].url.find("/videoplayback?");
					if (httpsFind != std::string::npos && videoPlaybackFind != std::string::npos) {
						std::string newString00 = "https://";
						downloadBaseUrl			= potentialFormats[maxIndex].url.substr(httpsFind + newString00.length(), videoPlaybackFind - newString00.length());
					}
					std::string requestNew = potentialFormats[maxIndex].url.substr(potentialFormats[maxIndex].url.find(".com") + 4);
					newerSong.finalDownloadUrls.resize(3);
					DownloadUrl downloadUrl01{};
					downloadUrl01.contentSize = stoull(potentialFormats[maxIndex].contentLength);
					downloadUrl01.urlPath	  = downloadBaseUrl;
					DownloadUrl downloadUrl02{};
					downloadUrl02.contentSize		  = stoull(potentialFormats[maxIndex].contentLength);
					downloadUrl02.urlPath			  = requestNew;
					newerSong.finalDownloadUrls.at(0) = downloadUrl01;
					newerSong.finalDownloadUrls.at(1) = downloadUrl02;
					newerSong.viewUrl				  = newerSong.firstDownloadUrl;
					newerSong.contentLength			  = downloadUrl02.contentSize;
					newerSong.type					  = SongType::YouTube;
				}
				return newerSong;
			} catch (const HttpsError& error) {
				if (currentRecursionDepth <= 10) {
					++currentRecursionDepth;
					return constructDownloadInfo(songNew, currentRecursionDepth);
				} else {
					MessagePrinter::printError<PrintMessageType::Https>("YouTubeRequestBuilder::constructDownloadInfo() Error: " + std::string{ error.what() });
					return {};
				}
			}
		}

		Song YouTubeRequestBuilder::collectFinalSong(const Song& songNew) {
			Song newerSong{ songNew };
			newerSong.firstDownloadUrl = static_cast<std::string>(baseUrl) + "/watch?v=" + newerSong.songId + "&hl=en";
			newerSong				   = constructDownloadInfo(newerSong, 0);
			return newerSong;
		}

		YouTubeAPI::YouTubeAPI(ConfigManager* configManagerNew, const Snowflake guildIdNew) : YouTubeRequestBuilder{ configManagerNew } {
			guildId = guildIdNew;
		}

		void YouTubeAPI::weFailedToDownloadOrDecode(const Song& songNew) {
			std::this_thread::sleep_for(1s);
			GuildMemberData guildMember{ GuildMembers::getCachedGuildMember({ .guildMemberId = songNew.addedByUserId, .guildId = guildId }) };
			DiscordCoreClient::getSongAPI(guildId).play(songNew, guildMember);
		}

		CoRoutine<void, false> YouTubeAPI::downloadAndStreamAudio(const Song songNew, NewThreadAwaiter<void, false> threadHandle, uint64_t currentReconnectTries) {
			try {
				areWeWorkingBool.store(true, std::memory_order_release);
				std::coroutine_handle<CoRoutine<void, false>::promise_type> coroHandle{};
				if (currentReconnectTries == 0) {
					threadHandle = NewThreadAwaitable<void, false>();
				}
				coroHandle = co_await threadHandle;
				uint64_t intervalCount{ songNew.contentLength / (1024ull * 1024ull) + 1ull };
				uint64_t remainder{ songNew.contentLength % (1024ull * 1024ull) };
				uint64_t currentStart{};
				uint64_t currentEnd{ intervalCount > 1 ? (1024ull * 1024ull) : remainder };
				jsonifier::vector<HttpsWorkloadData> workloadVector{};
				for (uint64_t x = 0; x < intervalCount; ++x) {
					HttpsWorkloadData workloadData{ HttpsWorkloadType::YouTubeGetSearchResults };
					if (songNew.finalDownloadUrls.size() > 0) {
						if (songNew.finalDownloadUrls.at(0).urlPath.find(".com") != std::string::npos) {
							workloadData.baseUrl = songNew.finalDownloadUrls.at(0).urlPath.substr(0, songNew.finalDownloadUrls.at(0).urlPath.find(".com") + 4);
						}
					} else {
						weFailedToDownloadOrDecode(songNew);
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}
					workloadData.workloadClass				   = HttpsWorkloadClass::Get;
					workloadData.headersToInsert["User-Agent"] = "com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip";
					workloadData.headersToInsert["Connection"] = "Keep-Alive";
					workloadData.headersToInsert["Host"]	   = songNew.finalDownloadUrls.at(0).urlPath;
					workloadData.headersToInsert["Origin"]	   = "https://music.youtube.com";
					workloadData.relativePath = songNew.finalDownloadUrls.at(1).urlPath + "&range=" + std::to_string(currentStart) + "-" + std::to_string(currentEnd);
					workloadVector.emplace_back(std::move(workloadData));
					currentStart = currentEnd;
					currentEnd += x == intervalCount - 2 ? remainder : (1024ull * 1024ull);
				}
				std::basic_string<uint8_t> buffer{};
				MatroskaDemuxer demuxer{};
				uint64_t index{};
				while (index < intervalCount || !demuxer.areWeDone()) {
					if (index < intervalCount) {
						HttpsResponseData result{ submitWorkloadAndGetResult(std::move(workloadVector[index])) };
						if (result.responseCode != 200) {
							areWeWorkingBool.store(false, std::memory_order_release);
							weFailedToDownloadOrDecode(songNew);
							co_return;
						}
						if (result.responseData.size() > 0) {
							++index;
							auto oldSize = buffer.size();
							buffer.resize(buffer.size() + result.responseData.size());
							std::memcpy(buffer.data() + oldSize, result.responseData.data(), result.responseData.size());
							demuxer.writeData({ buffer.data(), buffer.size() });
							demuxer.proceedDemuxing();
						}
					}
					bool didWeReceive{ true };
					do {
						AudioFrameData frameData{};
						didWeReceive = demuxer.collectFrame(frameData);
						if (coroHandle.promise().stopRequested()) {
							areWeWorkingBool.store(false, std::memory_order_release);
							co_return;
						}
						if (frameData.currentSize != 0) {
							frameData.guildMemberId = songNew.addedByUserId.operator const uint64_t&();
							DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
						}
					} while (didWeReceive);
					std::this_thread::sleep_for(1ms);
				}
				areWeWorkingBool.store(false, std::memory_order_release);
				DiscordCoreClient::getVoiceConnection(guildId).skip(false);
				AudioFrameData frameData{};
				frameData.guildMemberId = songNew.addedByUserId.operator const uint64_t&();
				DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
				co_return;
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("YouTubeAPI::downloadAndStreamAudio() Error: " + std::string{ error.what() });
				weFailedToDownloadOrDecode(songNew);
				areWeWorkingBool.store(false, std::memory_order_release);
			}
			co_return;
		}

		bool YouTubeAPI::areWeWorking() {
			return areWeWorkingBool.load(std::memory_order_acquire);
		}

		jsonifier::vector<Song> YouTubeAPI::searchForSong(const std::string& searchQuery) {
			return collectSearchResults(searchQuery);
		}

		Song YouTubeAPI::collectFinalSong(const Song& songNew) {
			return YouTubeRequestBuilder::collectFinalSong(songNew);
		}

	}
}