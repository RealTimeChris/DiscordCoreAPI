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
/// YouTubeAPI.cpp - soure file for the you_tube api related stuff.
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
#include <regex>

namespace jsonifier {

	template<> struct core<discord_core_api::discord_core_internal::user> {
		using value_type				 = discord_core_api::discord_core_internal::user;
		static constexpr auto parseValue = createValue("lockedSafetyMode", &value_type::lockedSafetyMode);
	};

	template<> struct core<discord_core_api::discord_core_internal::request> {
		using value_type				 = discord_core_api::discord_core_internal::request;
		static constexpr auto parseValue = createValue("useSsl", &value_type::useSsl);
	};

	template<> struct core<discord_core_api::discord_core_internal::you_tube_request_client> {
		using value_type = discord_core_api::discord_core_internal::you_tube_request_client;
		static constexpr auto parseValue =
			createValue("clientName", &value_type::clientName, "androidSdkVersion", &value_type::androidSdkVersion, "clientVersion", &value_type::clientVersion, "hl",
				&value_type::hl, "gl", &value_type::gl, "osName", &value_type::osName, "osVersion", &value_type::osVersion, "platform", &value_type::platform);
	};

	template<> struct core<discord_core_api::discord_core_internal::you_tube_request_context> {
		using value_type = discord_core_api::discord_core_internal::you_tube_request_context;
		static constexpr auto parseValue =
			createValue("client", &value_type::client, "captionParams", &value_type::captionParams, "request", &value_type::requestVal, "user", &value_type::userVal);
	};

	template<> struct core<discord_core_api::discord_core_internal::you_tube_request> {
		using value_type				 = discord_core_api::discord_core_internal::you_tube_request;
		static constexpr auto parseValue = createValue("videoId", &value_type::videoId, "contentCheckOk", &value_type::contentCheckOk, "racyCheckOk", &value_type::racyCheckOk,
			"context", &value_type::context, "playlistId", &value_type::playlistId, "params", &value_type::params);
	};

	template<> struct core<discord_core_api::discord_core_internal::thumbnail_element> {
		using value_type				 = discord_core_api::discord_core_internal::thumbnail_element;
		static constexpr auto parseValue = createValue("url", &value_type::url, "width", &value_type::width);
	};

	template<> struct core<discord_core_api::discord_core_internal::video_details_thumbnail> {
		using value_type				 = discord_core_api::discord_core_internal::video_details_thumbnail;
		static constexpr auto parseValue = createValue("thumbnails", &value_type::thumbnails);
	};

	template<> struct core<discord_core_api::discord_core_internal::video_details> {
		using value_type				 = discord_core_api::discord_core_internal::video_details;
		static constexpr auto parseValue = createValue("title", &value_type::title, "videoId", &value_type::videoId, "thumbnail", &value_type::thumbnail, "shortDescription",
			&value_type::shortDescription, "lengthSeconds", &value_type::lengthSeconds);
	};

	template<> struct core<discord_core_api::discord_core_internal::format> {
		using value_type = discord_core_api::discord_core_internal::format;
		static constexpr auto parseValue =
			createValue("url", &value_type::url, "mimeType", &value_type::mimeType, "bitrate", &value_type::bitrate, "contentLength", &value_type::contentLength);
	};

	template<> struct core<discord_core_api::discord_core_internal::streaming_data> {
		using value_type				 = discord_core_api::discord_core_internal::streaming_data;
		static constexpr auto parseValue = createValue("adaptiveFormats", &value_type::adaptiveFormats);
	};

	template<> struct core<discord_core_api::discord_core_internal::data> {
		using value_type				 = discord_core_api::discord_core_internal::data;
		static constexpr auto parseValue = createValue("streamingData", &value_type::streamingData, "videoDetails", &value_type::videoDetails);
	};

	template<> struct core<discord_core_api::discord_core_internal::video_renderer> {
		using value_type				 = discord_core_api::discord_core_internal::video_renderer;
		static constexpr auto parseValue = createValue("videoId", &value_type::videoId);
	};

	template<> struct core<discord_core_api::discord_core_internal::video_renderer_type> {
		using value_type				 = discord_core_api::discord_core_internal::video_renderer_type;
		static constexpr auto parseValue = createValue("videoRenderer", &value_type::videoRenderer);
	};

	template<> struct core<discord_core_api::discord_core_internal::item_section_renderer_contents> {
		using value_type				 = discord_core_api::discord_core_internal::item_section_renderer_contents;
		static constexpr auto parseValue = createValue("contents", &value_type::contents);
	};

	template<> struct core<discord_core_api::discord_core_internal::item_section_renderer> {
		using value_type				 = discord_core_api::discord_core_internal::item_section_renderer;
		static constexpr auto parseValue = createValue("itemSectionRenderer", &value_type::itemSectionRendererContents);
	};

	template<> struct core<discord_core_api::discord_core_internal::section_list_renderer> {
		using value_type				 = discord_core_api::discord_core_internal::section_list_renderer;
		static constexpr auto parseValue = createValue("contents", &value_type::contents);
	};

	template<> struct core<discord_core_api::discord_core_internal::primary_contents> {
		using value_type				 = discord_core_api::discord_core_internal::primary_contents;
		static constexpr auto parseValue = createValue("sectionListRenderer", &value_type::sectionListRenderer);
	};

	template<> struct core<discord_core_api::discord_core_internal::two_column_search_results_renderer> {
		using value_type				 = discord_core_api::discord_core_internal::two_column_search_results_renderer;
		static constexpr auto parseValue = createValue("primaryContents", &value_type::primaryContents);
	};

	template<> struct core<discord_core_api::discord_core_internal::contents01> {
		using value_type				 = discord_core_api::discord_core_internal::contents01;
		static constexpr auto parseValue = createValue("twoColumnSearchResultsRenderer", &value_type::twoColumnSearchResultsRenderer);
	};

	template<> struct core<discord_core_api::discord_core_internal::you_tube_search_results> {
		using value_type				 = discord_core_api::discord_core_internal::you_tube_search_results;
		static constexpr auto parseValue = createValue("contents", &value_type::contents);
	};
}

namespace discord_core_api { 

	struct you_tube_request_client {
		static constexpr jsonifier::string_view clientVersion{ "17.10.35" };
		static constexpr jsonifier::string_view androidSdkVersion{ "31" };
		static constexpr jsonifier::string_view clientName{ "android" };
		static constexpr jsonifier::string_view platform{ "mobile" };
		static constexpr jsonifier::string_view osName{ "android" };
		static constexpr jsonifier::string_view osVersion{ "12" };
		static constexpr jsonifier::string_view hl{ "en-gb" };
		static constexpr jsonifier::string_view gl{ "us" };
	};

	struct request {
		const bool useSsl{ true };
	};

	struct user {
		const bool lockedSafetyMode{};
	};

	struct you_tube_request_context {
		unordered_map<jsonifier::string, jsonifier::string> captionParams{};
		you_tube_request_client client{};
		request requestVal{};
		user userVal{};
	};

	struct you_tube_request {
		const jsonifier::string playlistId{};
		you_tube_request_context context{};
		const bool contentCheckOk{ true };
		const jsonifier::string params{};
		const bool racyCheckOk{ true };
		jsonifier::string videoId{};
	};
};
 
namespace discord_core_api {

	namespace discord_core_internal {

		you_tube_request_builder::you_tube_request_builder(config_manager* configManagerNew) : https_client_core{ jsonifier::string{ configManagerNew->getBotToken() } } {};

		inline bool collectVideoIdFromSearchQuery(jsonifier::string_view url, jsonifier::string& stringNew) {
			std::regex pattern("v=([a-zA-Z0-9_\\-]+)");
			std::smatch match;

			auto newUrl = static_cast<std::string>(url);
			if (std::regex_search(newUrl, match, pattern)) {
				stringNew = match[1].str();
				return true;
			}

			return false;
		}

		song you_tube_request_builder::collectSingleResult(jsonifier::string_view searchQuery) {

			song songNew{};
			songNew.type   = song_type::YouTube;
			songNew.songId = searchQuery;
			songNew		   = constructDownloadInfo(songNew, 0);
			return songNew;
		}

		jsonifier::vector<song> you_tube_request_builder::collectSearchResults(jsonifier::string_view searchQuery, uint64_t limit) {
			https_workload_data dataPackage{ https_workload_type::YouTube_Get_Search_Results };
			jsonifier::vector<song> searchResults{};
			jsonifier::string theId{};
			if (collectVideoIdFromSearchQuery(searchQuery, theId)) {
				searchResults.emplace_back(collectSingleResult(theId));
				return searchResults;
			}
			dataPackage.baseUrl						  = baseUrl;
			dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36";
			dataPackage.relativePath	   = "/results?search_query=" + urlEncode(searchQuery);
			dataPackage.workloadClass	   = https_workload_class::Get;
			https_response_data returnData = submitWorkloadAndGetResult(std::move(dataPackage));
			if (returnData.responseCode != 200) {
				message_printer::printError<print_message_type::https>(
					"you_tube_request_builder::collectSearchResults() error: " + jsonifier::toString(returnData.responseCode.operator uint64_t()) + returnData.responseData);
			}
			auto varInitFind = returnData.responseData.find("var ytInitialData = ");
			if (varInitFind != jsonifier::string::npos) {
				jsonifier::string newString00	 = "var ytInitialData = ";
				jsonifier::string newString		 = returnData.responseData.substr(varInitFind + newString00.size());
				jsonifier::string stringSequence = ";</script><script nonce=";
				newString						 = newString.substr(0, newString.find(stringSequence));
				you_tube_search_results you_tubeSearchResults{};
				parser.parseJson(you_tubeSearchResults, newString);
				for (auto& value: parser.getErrors()) {
					message_printer::printError<print_message_type::https>(value.reportError());
				}
				for (auto& value: you_tubeSearchResults.contents.twoColumnSearchResultsRenderer.primaryContents.sectionListRenderer.contents) {
					for (auto& value02: value.itemSectionRendererContents.contents) {
						if (value02.videoRenderer.videoId != "") {
							song songNew{};
							songNew.type   = song_type::YouTube;
							songNew.songId = value02.videoRenderer.videoId;
							searchResults.emplace_back(constructDownloadInfo(songNew, 0));
						}
						if (searchResults.size() >= limit) {
							break;
						}
					}
				}
			}
			return searchResults;
		}

		song you_tube_request_builder::constructDownloadInfo(const song& songNew, uint64_t currentRecursionDepth) {
			https_response_data responseData{};
			try {
				discord_core_api::you_tube_request requestData{};
				requestData.videoId = songNew.songId;
				https_workload_data dataPackage02{ https_workload_type::YouTube_Get_Search_Results };
				dataPackage02.baseUrl						= "https://music.you_tube.com";
				dataPackage02.headersToInsert["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36";
				dataPackage02.headersToInsert["Origin"]		= "https://music.you_tube.com";
				dataPackage02.headersToInsert["Content-Type"] = "application/json; charset=utf-8";
				dataPackage02.relativePath					= "/you_tubei/v1/player?key=AIzaSyA8eiZmM1FaDVjRy-df2KTyQ_vz_yYM39w";
				parser.serializeJson(requestData, dataPackage02.content);
				dataPackage02.content =
					"{\"context\":{\"client\":{\"clientName\":\"IOS\",\"clientVersion\":\"17.13.3\",\"hl\":\"en\"},\"user\":{\"lockedSafetyMode\":false},\"request\":{\"useSsl\":"
					"true},\"captionParams\":{}},\"params\":{}}";
				std::cout << "REQUEST CONTENT: " << dataPackage02.content << std::endl;
				dataPackage02.workloadClass = https_workload_class::Post;
				responseData				= submitWorkloadAndGetResult(std::move(dataPackage02));
				if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200) {
					message_printer::printError<print_message_type::https>("you_tube_request_builder::constructDownloadInfo() 01 error: " +
						jsonifier::toString(responseData.responseCode.operator uint64_t()) + ", " + responseData.responseData);
				}
				data dataNew{};
				jsonifier::vector<format> potentialFormats{};
				parser.parseJson(dataNew, responseData.responseData);
				for (auto& value: parser.getErrors()) {
					message_printer::printError<print_message_type::https>(value.reportError());
				}
				for (auto& value: dataNew.streamingData.adaptiveFormats) {
					if (value.mimeType == "audio/webm; codecs=\"opus\"") {
						potentialFormats.emplace_back(value);
					}
				}
				uint64_t currentMax{};
				int64_t maxIndex{ static_cast<int64_t>(std::numeric_limits<uint64_t>::max()) };
				for (uint64_t x = 0; x < potentialFormats.size(); ++x) {
					if (potentialFormats.at(x).bitrate > static_cast<int64_t>(currentMax)) {
						maxIndex = static_cast<int64_t>(x);
					}
				}

				song newerSong{ songNew };
				jsonifier::string downloadBaseUrl{};
				newerSong.type = song_type::YouTube;
				if (maxIndex > -1) {
					jsonifier::string thumbnailUrl{};
					if (dataNew.videoDetails.thumbnail.thumbnails.size() > 0) {
						uint32_t currentLargestThumbnailWidth{};
						uint32_t currentThumbnailIndex{};
						for (uint32_t x = 0; x < dataNew.videoDetails.thumbnail.thumbnails.size(); ++x) {
							if (dataNew.videoDetails.thumbnail.thumbnails[x].width > currentLargestThumbnailWidth) {
								currentThumbnailIndex = x;
							}
						}
						thumbnailUrl = dataNew.videoDetails.thumbnail.thumbnails[currentThumbnailIndex].url;
					}
					auto httpsFind		   = potentialFormats[static_cast<uint64_t>(maxIndex)].url.find("https://");
					auto videoPlaybackFind = potentialFormats[static_cast<uint64_t>(maxIndex)].url.find("/videoplayback?");
					if (httpsFind != jsonifier::string::npos && videoPlaybackFind != jsonifier::string::npos) {
						jsonifier::string newString00 = "https://";
						downloadBaseUrl = potentialFormats[static_cast<uint64_t>(maxIndex)].url.substr(httpsFind + newString00.size(), videoPlaybackFind - newString00.size());
					}
					auto& newString = dataNew.videoDetails.shortDescription;
					if (newString.size() > 0) {
						if (newString.size() > 256) {
							newString = newString.substr(0, 256);
						}
						newerSong.description = utf8MakeValid(newString);
						newerSong.description += "...";
					}
					jsonifier::string requestNew = potentialFormats[static_cast<uint64_t>(maxIndex)].url.substr(potentialFormats[static_cast<uint64_t>(maxIndex)].url.find(".com") + 4);
					newerSong.finalDownloadUrls.resize(3);
					download_url downloadUrl01{};
					downloadUrl01.contentSize = jsonifier::strToUint64(potentialFormats[static_cast<uint64_t>(maxIndex)].contentLength.data());
					downloadUrl01.urlPath	  = downloadBaseUrl;
					download_url downloadUrl02{};
					downloadUrl02.contentSize		  = jsonifier::strToUint64(potentialFormats[static_cast<uint64_t>(maxIndex)].contentLength.data());
					downloadUrl02.urlPath			  = requestNew;
					newerSong.finalDownloadUrls.at(0) = downloadUrl01;
					newerSong.finalDownloadUrls.at(1) = downloadUrl02;
					newerSong.viewUrl				  = newerSong.firstDownloadUrl;
					newerSong.duration				  = time_stamp::convertMsToDurationString(jsonifier::strToUint64(dataNew.videoDetails.lengthSeconds) * 1000);
					newerSong.viewUrl				  = "https://www.you_tube.com/watch?v=" + songNew.songId;
					newerSong.contentLength			  = downloadUrl02.contentSize;
					newerSong.thumbnailUrl			  = thumbnailUrl;
					newerSong.songTitle				  = dataNew.videoDetails.title;
					newerSong.type					  = song_type::YouTube;
				}
				return newerSong;
			} catch (const https_error& error) {
				if (currentRecursionDepth <= 10) {
					++currentRecursionDepth;
					return constructDownloadInfo(songNew, currentRecursionDepth);
				} else {
					message_printer::printError<print_message_type::https>("you_tube_request_builder::constructDownloadInfo() error: " + jsonifier::string{ error.what() });
					return {};
				}
			}
		}

		song you_tube_request_builder::collectFinalSong(const song& songNew) {
			song newerSong{ songNew };
			newerSong.firstDownloadUrl = static_cast<jsonifier::string>(baseUrl) + "/watch?v=" + newerSong.songId + "&hl=en";
			newerSong				   = constructDownloadInfo(newerSong, 0);
			return newerSong;
		}

		you_tube_api::you_tube_api(config_manager* configManagerNew, const snowflake guildIdNew) : you_tube_request_builder{ configManagerNew } {
			guildId = guildIdNew;
		}

		void you_tube_api::weFailedToDownloadOrDecode(const song& songNew, std::coroutine_handle<co_routine<void, false>::promise_type> threadHandle, uint64_t recursionDepth) {
			std::this_thread::sleep_for(1s);
			if (recursionDepth < 10) {
				++recursionDepth;
				song songNewer = constructDownloadInfo(songNew, 0);
				downloadAndStreamAudio(songNewer, threadHandle, recursionDepth);
			} else {
				discord_core_client::getVoiceConnection(guildId).skip(true);
			}
		}

		co_routine<void, false> you_tube_api::downloadAndStreamAudio(const song songNew, std::coroutine_handle<co_routine<void, false>::promise_type> threadHandle,
			uint64_t currentReconnectTries) {
			try {
				areWeWorkingBool.store(true, std::memory_order_release);
				if (currentReconnectTries == 0) {
					threadHandle = co_await newThreadAwaitable<void, false>();
				}
				if (songNew.type != song_type::YouTube) {
					message_printer::printError<print_message_type::general>("Failed to have the correct song type.");
					co_return;
				}
				uint64_t intervalCount{ songNew.contentLength / (1024ULL * 1024ULL) + 1ULL };
				uint64_t remainder{ songNew.contentLength % (1024ULL * 1024ULL) };
				uint64_t currentStart{};
				uint64_t currentEnd{ intervalCount > 1 ? (1024ULL * 1024ULL) : remainder };
				jsonifier::vector<https_workload_data> workloadVector{};
				for (uint64_t x = 0; x < intervalCount; ++x) {
					https_workload_data workloadData{ https_workload_type::YouTube_Get_Search_Results };
					if (songNew.finalDownloadUrls.size() > 0) {
						if (songNew.finalDownloadUrls.at(0).urlPath.find(".com") != jsonifier::string::npos) {
							workloadData.baseUrl = songNew.finalDownloadUrls.at(0).urlPath.substr(0, songNew.finalDownloadUrls.at(0).urlPath.find(".com") + 4);
						}
					} else {
						weFailedToDownloadOrDecode(songNew, threadHandle, currentReconnectTries);
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}
					workloadData.workloadClass				   = https_workload_class::Get;
					workloadData.headersToInsert["User-Agent"] = "com.google.android.you_tube/17.10.35 (Linux; U; Android 12; US) gzip";
					workloadData.headersToInsert["Connection"] = "Keep-Alive";
					workloadData.headersToInsert["Host"]	   = songNew.finalDownloadUrls.at(0).urlPath;
					workloadData.headersToInsert["Origin"]	   = "https://music.you_tube.com";
					workloadData.relativePath = songNew.finalDownloadUrls.at(1).urlPath + "&range=" + jsonifier::toString(currentStart) + "-" + jsonifier::toString(currentEnd);
					workloadVector.emplace_back(std::move(workloadData));
					currentStart = currentEnd;
					currentEnd += x == intervalCount - 2 ? remainder : (1024ULL * 1024ULL);
				}
				jsonifier::string_base<uint8_t> buffer{};
				matroska_demuxer demuxer{};
				uint64_t index{};
				while (index < intervalCount || !demuxer.areWeDone() && !threadHandle.promise().stopRequested()) {
					if (index < intervalCount) {
						https_response_data result{ submitWorkloadAndGetResult(std::move(workloadVector[index])) };
						if (result.responseCode != 200) {
							weFailedToDownloadOrDecode(songNew, threadHandle, currentReconnectTries);
							areWeWorkingBool.store(false, std::memory_order_release);
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
						audio_frame_data frameData{};
						didWeReceive = demuxer.collectFrame(frameData);
						if (threadHandle.promise().stopRequested()) {
							areWeWorkingBool.store(false, std::memory_order_release);
							co_return;
						}
						if (frameData.currentSize != 0) {
							discord_core_client::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
						}
					} while (didWeReceive);
					std::this_thread::sleep_for(1ms);
				}
				areWeWorkingBool.store(false, std::memory_order_release);
				discord_core_client::getVoiceConnection(guildId).skip(false);
				audio_frame_data frameData{};
				discord_core_client::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
				co_return;
			} catch (const https_error& error) {
				message_printer::printError<print_message_type::https>("you_tube_api::downloadAndStreamAudio() error: " + jsonifier::string{ error.what() });
				weFailedToDownloadOrDecode(songNew, threadHandle, currentReconnectTries);
				areWeWorkingBool.store(false, std::memory_order_release);
			}
			co_return;
		}

		bool you_tube_api::areWeWorking() {
			return areWeWorkingBool.load(std::memory_order_acquire);
		}

		jsonifier::vector<song> you_tube_api::searchForSong(jsonifier::string_view searchQuery, uint64_t limit) {
			return collectSearchResults(searchQuery, limit);
		}

	}
}
