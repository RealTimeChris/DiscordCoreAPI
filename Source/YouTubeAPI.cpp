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

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::ThumbnailElement> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::ThumbnailElement;
		static constexpr auto parseValue = createObject("url", &ValueType::url, "width", &ValueType::width);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoDetailsThumbnail> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoDetailsThumbnail;
		static constexpr auto parseValue = createObject("thumbnails", &ValueType::thumbnails);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoDetails> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoDetails;
		static constexpr auto parseValue = createObject("title", &ValueType::title, "video_id", &ValueType::videoId, "thumbnail", &ValueType::thumbnail);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::VideoRenderer> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::VideoRenderer;
		static constexpr auto parseValue = createObject("videoId", &ValueType::videoId);
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
		static constexpr auto parseValue = createObject("adaptiveFormats", &ValueType::adaptiveFormats);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Data> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Data;
		static constexpr auto parseValue = createObject("streamingData", &ValueType::streamingData, "videoDetails", &ValueType::videoDetails);
	};
}

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		YouTubeRequestBuilder::YouTubeRequestBuilder(ConfigManager* configManagerNew) : HttpsClientCore{ jsonifier::string{ configManagerNew->getBotToken() } } {};

		jsonifier::string collectVideoIdFromSearchQuery(jsonifier::string_view string) {
			if (string.find("youtube") != jsonifier::string::npos) {
				jsonifier::string returnString{};
				auto findValue = string.find("v=") + 2;
				returnString   = string.substr(findValue, string.size() - findValue);
				return returnString;
			} else {
				return static_cast<jsonifier::string>(string);
			}
		}

		Song YouTubeAPI::collectSingleResult(jsonifier::string_view searchQuery) {
			return YouTubeRequestBuilder::collectSingleResult(searchQuery);
		}

		Song YouTubeRequestBuilder::collectSingleResult(jsonifier::string_view searchQuery) {
			Song songNew{};
			songNew.type	= SongType::YouTube;
			songNew.songId = collectVideoIdFromSearchQuery(searchQuery);
			songNew			= constructDownloadInfo(songNew, 0);
			return songNew;
		}

		jsonifier::vector<Song> YouTubeRequestBuilder::collectSearchResults(jsonifier::string_view searchQuery) {
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage.baseUrl			 = baseUrl;
			dataPackage.relativePath	 = "/results?search_query=" + urlEncode(collectVideoIdFromSearchQuery(searchQuery));
			dataPackage.workloadClass	 = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage));
			if (returnData.responseCode != 200) {
				MessagePrinter::printError<PrintMessageType::Https>(
					"YouTubeRequestBuilder::collectSearchResults() Error: " + jsonifier::toString(returnData.responseCode.operator uint64_t()) + returnData.responseData);
			}
			jsonifier::vector<Song> searchResults{};
			auto varInitFind = returnData.responseData.find("var ytInitialData = ");
			if (varInitFind != jsonifier::string::npos) {
				jsonifier::string newString00	   = "var ytInitialData = ";
				jsonifier::string newString	   = returnData.responseData.substr(varInitFind + newString00.size());
				jsonifier::string stringSequence = ";</script><script nonce=";
				newString				   = newString.substr(0, newString.find(stringSequence));
				YouTubeSearchResults youtubeSearchResults{};
				parser.parseJson<true, true>(youtubeSearchResults, newString);
				for (auto& value: youtubeSearchResults.contents.twoColumnSearchResultsRenderer.primaryContents.sectionListRenderer.contents) {
					for (auto& value02: value.itemSectionRendererContents.contents) {
						Song songNew{};
						songNew.type   = SongType::YouTube;
						songNew.songId = value02.videoRenderer.videoId;
						searchResults.emplace_back(constructDownloadInfo(songNew, 0));
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
					MessagePrinter::printError<PrintMessageType::Https>("YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " +
						jsonifier::toString(responseData.responseCode.operator uint64_t()) + ", " + responseData.responseData);
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
				jsonifier::string songTitle{};
				if (dataNew.videoDetails.title != "") {
					songTitle = dataNew.videoDetails.title;
				}

				jsonifier::string thumbnailUrl{};
				if (dataNew.videoDetails.thumbnail.thumbnails.size() > 0) {
					int32_t currentLargestThumbnailWidth{};
					int32_t currentThumbnailIndex{};
					for (int32_t x = 0; x < dataNew.videoDetails.thumbnail.thumbnails.size(); ++x) {
						if (dataNew.videoDetails.thumbnail.thumbnails[x].width > currentLargestThumbnailWidth) {
							currentThumbnailIndex = x;
						}
					}
					thumbnailUrl = dataNew.videoDetails.thumbnail.thumbnails[currentThumbnailIndex].url;
				}
				Song newerSong{ songNew };
				jsonifier::string downloadBaseUrl{};
				newerSong.type = SongType::YouTube;
				if (maxIndex > -1) {
					auto httpsFind		   = potentialFormats[maxIndex].url.find("https://");
					auto videoPlaybackFind = potentialFormats[maxIndex].url.find("/videoplayback?");
					if (httpsFind != jsonifier::string::npos && videoPlaybackFind != jsonifier::string::npos) {
						jsonifier::string newString00 = "https://";
						downloadBaseUrl				  = potentialFormats[maxIndex].url.substr(httpsFind + newString00.size(), videoPlaybackFind - newString00.size());
					}
					jsonifier::string requestNew = potentialFormats[maxIndex].url.substr(potentialFormats[maxIndex].url.find(".com") + 4);
					newerSong.finalDownloadUrls.resize(3);
					DownloadUrl downloadUrl01{};
					downloadUrl01.contentSize = std::stoull(potentialFormats[maxIndex].contentLength.data());
					downloadUrl01.urlPath	  = downloadBaseUrl;
					DownloadUrl downloadUrl02{};
					downloadUrl02.contentSize		  = std::stoull(potentialFormats[maxIndex].contentLength.data());
					downloadUrl02.urlPath			  = requestNew;
					newerSong.finalDownloadUrls.at(0) = downloadUrl01;
					newerSong.finalDownloadUrls.at(1) = downloadUrl02;
					newerSong.viewUrl				  = newerSong.firstDownloadUrl;
					newerSong.viewUrl				  = "https://www.youtube.com/watch?v=" + songNew.songId;
					newerSong.contentLength			  = downloadUrl02.contentSize;
					newerSong.thumbnailUrl			  = thumbnailUrl;
					newerSong.songTitle				  = songTitle;
					newerSong.type					  = SongType::YouTube;
				}
				return newerSong;
			} catch (const HttpsError& error) {
				if (currentRecursionDepth <= 10) {
					++currentRecursionDepth;
					return constructDownloadInfo(songNew, currentRecursionDepth);
				} else {
					MessagePrinter::printError<PrintMessageType::Https>("YouTubeRequestBuilder::constructDownloadInfo() Error: " + jsonifier::string{ error.what() });
					return {};
				}
			}
		}

		Song YouTubeRequestBuilder::collectFinalSong(const Song& songNew) {
			Song newerSong{ songNew };
			newerSong.firstDownloadUrl = static_cast<jsonifier::string>(baseUrl) + "/watch?v=" + newerSong.songId + "&hl=en";
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
						if (songNew.finalDownloadUrls.at(0).urlPath.find(".com") != jsonifier::string::npos) {
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
					workloadData.relativePath = songNew.finalDownloadUrls.at(1).urlPath + "&range=" + jsonifier::toString(currentStart) + "-" + jsonifier::toString(currentEnd);
					workloadVector.emplace_back(std::move(workloadData));
					currentStart = currentEnd;
					currentEnd += x == intervalCount - 2 ? remainder : (1024ull * 1024ull);
				}
				jsonifier::string_base<uint8_t> buffer{};
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
				MessagePrinter::printError<PrintMessageType::Https>("YouTubeAPI::downloadAndStreamAudio() Error: " + jsonifier::string{ error.what() });
				weFailedToDownloadOrDecode(songNew);
				areWeWorkingBool.store(false, std::memory_order_release);
			}
			co_return;
		}

		bool YouTubeAPI::areWeWorking() {
			return areWeWorkingBool.load(std::memory_order_acquire);
		}

		jsonifier::vector<Song> YouTubeAPI::searchForSong(jsonifier::string_view searchQuery) {
			return collectSearchResults(searchQuery);
		}

		Song YouTubeAPI::collectFinalSong(const Song& songNew) {
			return YouTubeRequestBuilder::collectFinalSong(songNew);
		}

	}
}