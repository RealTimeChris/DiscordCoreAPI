/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
#include <discordcoreapi/SSLClients.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreInternal::User> {
		using OTy2 = DiscordCoreInternal::User;
		static constexpr auto parseValue = object("lockedSafetyMode", &OTy2::lockedSafetyMode);
	};

	template<> struct Core<DiscordCoreInternal::Request> {
		using OTy2 = DiscordCoreInternal::Request;
		static constexpr auto parseValue = object("useSsl", &OTy2::useSSL);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeRequestClient> {
		using OTy = DiscordCoreInternal::YouTubeRequestClient;
		static constexpr auto parseValue = object("clientName", &OTy::clientName, "androidSdkVersion", &OTy::androidSdkVersion, "clientVersion",
			&OTy::clientVersion, "hl", &OTy::hl, "gl", &OTy::gl, "osName", &OTy::osName, "osVersion", &OTy::osVersion, "platform", &OTy::platform);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeRequestContext> {
		using OTy = DiscordCoreInternal::YouTubeRequestContext;
		static constexpr auto parseValue =
			object("client", &OTy::client, "captionParams", &OTy::captionParams, "request", &OTy::request, "user", &OTy::user);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeRequest> {
		using OTy = DiscordCoreInternal::YouTubeRequest;
		static constexpr auto parseValue = object("videoId", &OTy::videoId, "contentCheckOk", &OTy::contentCheckOk, "racyCheckOk", &OTy::racyCheckOk,
			"context", &OTy::context, "playlistId", &OTy::playlistId, "params", &OTy::params);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeSearchResult> {
		using OTy2 = DiscordCoreInternal::YouTubeSearchResult;
		static constexpr auto parseValue =
			object("type", &OTy2::type, "videoId", &OTy2::songId, "url", &OTy2::viewUrl, "descriptionSnippet", &OTy2::description);
	};

	template<> struct Core<DiscordCoreInternal::Thumbnail> {
		using OTy2 = DiscordCoreInternal::Thumbnail;
		static constexpr auto parseValue = object("url", &OTy2::url);
	};

	template<> struct Core<DiscordCoreInternal::ThumbNails> {
		using OTy2 = DiscordCoreInternal::ThumbNails;
		static constexpr auto parseValue = object("thumbnails", &OTy2::thumbNails);
	};

	template<> struct Core<DiscordCoreInternal::AccessibilityData> {
		using OTy2 = DiscordCoreInternal::AccessibilityData;
		static constexpr auto parseValue = object("label", &OTy2::label);
	};

	template<> struct Core<DiscordCoreInternal::Accessibility> {
		using OTy2 = DiscordCoreInternal::Accessibility;
		static constexpr auto parseValue = object("accessibilityData", &OTy2::accessibilityData);
	};

	template<> struct Core<DiscordCoreInternal::LengthText> {
		using OTy2 = DiscordCoreInternal::LengthText;
		static constexpr auto parseValue = object("accessibility", &OTy2::accessibility);
	};

	template<> struct Core<DiscordCoreInternal::Title> {
		using OTy2 = DiscordCoreInternal::Title;
		static constexpr auto parseValue = object("runs", &OTy2::runs);
	};

	template<> struct Core<DiscordCoreInternal::Text> {
		using OTy2 = DiscordCoreInternal::Text;
		static constexpr auto parseValue = object("text", &OTy2::text);
	};

	template<> struct Core<DiscordCoreInternal::SnippetText> {
		using OTy2 = DiscordCoreInternal::SnippetText;
		static constexpr auto parseValue = object("runs", &OTy2::runs);
	};

	template<> struct Core<DiscordCoreInternal::SnippetTextValue> {
		using OTy2 = DiscordCoreInternal::SnippetTextValue;
		static constexpr auto parseValue = object("snippetText", &OTy2::snippetText);
	};

	template<> struct Core<DiscordCoreInternal::VideoRenderer> {
		using OTy2 = DiscordCoreInternal::VideoRenderer;
		static constexpr auto parseValue = object("videoId", &OTy2::videoId, "thumbnail", &OTy2::thumbnails, "lengthText", &OTy2::lengthText,
			"detailedMetadataSnippets", &OTy2::detailedMetadataSnippets, "title", &OTy2::title);
	};

	template<> struct Core<DiscordCoreInternal::VideoRendererType> {
		using OTy2 = DiscordCoreInternal::VideoRendererType;
		static constexpr auto parseValue = object("videoRenderer", &OTy2::videoRenderer);
	};

	template<> struct Core<DiscordCoreInternal::ItemSectionRenderer> {
		using OTy2 = DiscordCoreInternal::ItemSectionRenderer;
		static constexpr auto parseValue = object("itemSectionRenderer", &OTy2::contents);
	};

	template<> struct Core<DiscordCoreInternal::SectionListRenderer> {
		using OTy2 = DiscordCoreInternal::SectionListRenderer;
		static constexpr auto parseValue = object("contents", &OTy2::contents);
	};

	template<> struct Core<DiscordCoreInternal::PrimaryContents> {
		using OTy2 = DiscordCoreInternal::PrimaryContents;
		static constexpr auto parseValue = object("sectionListRenderer", &OTy2::sectionListRenderer);
	};

	template<> struct Core<DiscordCoreInternal::TwoColumnSearchResultsRenderer> {
		using OTy2 = DiscordCoreInternal::TwoColumnSearchResultsRenderer;
		static constexpr auto parseValue = object("primaryContents", &OTy2::primaryContents);
	};

	template<> struct Core<DiscordCoreInternal::Contents01> {
		using OTy2 = DiscordCoreInternal::Contents01;
		static constexpr auto parseValue = object("twoColumnSearchResultsRenderer", &OTy2::twoColumnSearchResultsRenderer);
	};

	template<> struct Core<DiscordCoreInternal::YouTubeSearchResults> {
		using OTy2 = DiscordCoreInternal::YouTubeSearchResults;
		static constexpr auto parseValue = object("contents", &OTy2::contents);
	};

	template<> struct Core<DiscordCoreInternal::Format> {
		using OTy2 = DiscordCoreInternal::Format;
		static constexpr auto parseValue =
			object("url", &OTy2::url, "mimeType", &OTy2::mimeType, "bitrate", &OTy2::bitrate, "contentLength", &OTy2::contentLength);
	};

	template<> struct Core<DiscordCoreInternal::StreamingData> {
		using OTy2 = DiscordCoreInternal::StreamingData;
		static constexpr auto parseValue =
			object("formats", &OTy2::formats, "adaptiveFormats", &OTy2::adaptiveFormats);
	};

	template<> struct Core<DiscordCoreInternal::Data> {
		using OTy2 = DiscordCoreInternal::Data;
		static constexpr auto parseValue = object("streamingData", &OTy2::streamingData);
	};
}

namespace DiscordCoreInternal {

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
		HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
		dataPackage.baseUrl = this->baseUrl;
		dataPackage.relativePath = "/results?search_query=" + DiscordCoreAPI::urlEncode(searchQuery.c_str());
		dataPackage.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
		if (returnData.responseCode != 200 && this->configManager->doWePrintHttpsErrorMessages()) {
			cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::collectSearchResults() Error: " << returnData.responseCode
				 << returnData.responseData.c_str() << DiscordCoreAPI::reset() << endl
				 << endl;
		}

		std::vector<DiscordCoreAPI::Song> searchResults{};
		auto varInitFind = returnData.responseData.find("var ytInitialData = ");
		if (varInitFind != std::string::npos) {
			std::string newString00 = "var ytInitialData = ";
			std::string newString = returnData.responseData.substr(varInitFind + newString00.length());
			std::string stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			YouTubeSearchResults youtubeSearchResults{};
			parser.parseJson(youtubeSearchResults, newString);
			std::cout << "CURRENT RESPONSE DATA: " << newString << std::endl;
			for (auto& value: youtubeSearchResults.contents.twoColumnSearchResultsRenderer.primaryContents.sectionListRenderer.contents) {
				for (auto& value02: value.contents) {
					DiscordCoreAPI::Song newSong{};
					newSong.type = DiscordCoreAPI::SongType::YouTube;
					newSong.songId = value02.videoRenderer.videoId;
					if (value02.videoRenderer.title.runs.size() > 0) {
						newSong.songTitle = value02.videoRenderer.title.runs[0].text;
					}
					newSong.viewUrl = this->baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
					newSong.duration = value02.videoRenderer.lengthText.accessibility.accessibilityData.label;
					std::cout << "DURATION: " << newSong.duration << std::endl;
					
					if (value02.videoRenderer.detailedMetadataSnippets.size() > 0 &&
						value02.videoRenderer.detailedMetadataSnippets[0].snippetText.runs.size() > 0) {
						newSong.description = value02.videoRenderer.detailedMetadataSnippets[0].snippetText.runs[0].text;
					}
					//newSong.duration = value02.videoRenderer.detailedMetadataSnippets.jsonData;
					
					if (value02.videoRenderer.thumbnails.thumbNails.size() > 0) {
						newSong.thumbnailUrl = value02.videoRenderer.thumbnails.thumbNails[value02.videoRenderer.thumbnails.thumbNails.size() - 1].url;
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
			responseData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
			if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200 &&
				this->configManager->doWePrintHttpsErrorMessages()) {
				cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData.responseCode
					 << ", " << responseData.responseData << DiscordCoreAPI::reset() << endl
					 << endl;
			}
			
			Data dataNew{};
			std::vector<Format> potentialFormats{};
			parser.parseJson(dataNew, responseData.responseData);
			for (auto& value: dataNew.streamingData.adaptiveFormats) {
				if (value.mimeType == "audio/webm; codecs=\"opus\"") {
					potentialFormats.emplace_back(value);
				}
				std::cout << "CURRENT VALUE: " << value.mimeType << std::endl;
			}
			size_t currentMax{};
			size_t maxIndex{};
			for (size_t x = 0; x < potentialFormats.size(); ++x) {
				if (potentialFormats[x].bitrate> currentMax){
					maxIndex = x;
				}
			}
			DiscordCoreAPI::Song newerSong{ newSong };
			std::string downloadBaseUrl{};
			newerSong.type = DiscordCoreAPI::SongType::YouTube;
			if (maxIndex > 0) {
				auto httpsFind = potentialFormats[maxIndex].url.find("https://");
				auto videoPlaybackFind = potentialFormats[maxIndex].url.find("/videoplayback?");
				if (httpsFind != std::string::npos && videoPlaybackFind != std::string::npos) {
					std::string newString00 = "https://";
					downloadBaseUrl =
						potentialFormats[maxIndex].url.substr(httpsFind + newString00.length(), videoPlaybackFind - newString00.length());
				}
				std::string requestNew = "GET " + potentialFormats[maxIndex].url + " HTTP/1.1\n\rHost: " + downloadBaseUrl + "\n\r" +
					"Connection: Keep-Alive\n\r" + "User-Agent: com.google.android.youtube/17.10.35 (Linux; U; Android 12; US) gzip\n\r" +
					"Origin: https://music.youtube.com" + "\n\r\n\r";
				newerSong.finalDownloadUrls.resize(2);
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
		} catch (...) {
			if (currentRecursionDepth <= 10) {
				++currentRecursionDepth;
				return this->constructDownloadInfo(newSong, currentRecursionDepth);
			} else {
				if (this->configManager->doWePrintHttpsErrorMessages()) {
					DiscordCoreAPI::reportException("YouTubeRequestBuilder::constructDownloadInfo()");
				}
				return {};
			}
		}
		return {};
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::collectFinalSong(const DiscordCoreAPI::Song& newSong) {
		DiscordCoreAPI::Song newerSong{ newSong };
		newerSong.firstDownloadUrl = this->baseUrl + "/watch?v=" + newerSong.songId + "&hl=en";
		newerSong = this->constructDownloadInfo(newerSong, 0);
		return newerSong;
	}

	YouTubeAPI::YouTubeAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClientNew, const DiscordCoreAPI::Snowflake guildIdNew) {
		this->configManager = configManagerNew;
		this->httpsClient = httpsClientNew;
		this->guildId = static_cast<DiscordCoreAPI::Snowflake>(guildIdNew);
	}

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries) {
		++currentReconnectTries;
		DiscordCoreAPI::GuildMemberData guildMember{};
		DiscordCoreAPI::GuildMembers::getCachedGuildMember({ .guildMemberId = newSong.addedByUserId, .guildId = this->guildId });
		DiscordCoreAPI::Song newerSong = newSong;
		if (currentReconnectTries > 9) {
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.tryReceive(frameData)) {
			};
			DiscordCoreAPI::SongCompletionEventData eventData{};
			auto returnData = DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId);
			if (returnData) {
				eventData.previousSong = returnData->getCurrentSong(this->guildId);
			}
			eventData.wasItAFail = true;
			eventData.guildMember = guildMember;
			eventData.guild = DiscordCoreAPI::Guilds::getGuildAsync({ .guildId = this->guildId }).get();
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->onSongCompletionEvent(eventData);
		} else {
			newerSong = this->collectFinalSong(std::move(newerSong));
			YouTubeAPI::downloadAndStreamAudio(newerSong, token, currentReconnectTries);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries) {
		try {
			std::unique_ptr<WebSocketClient> streamSocket{ std::make_unique<WebSocketClient>(nullptr, 0, nullptr) };
			auto bytesRead{ static_cast<int32_t>(streamSocket->getBytesRead()) };
			if (newSong.finalDownloadUrls.size() > 0) {
				if (!static_cast<TCPConnection*>(streamSocket.get())
						 ->connect(newSong.finalDownloadUrls[0].urlPath, 443, this->configManager->doWePrintWebSocketErrorMessages(), true)) {
					std::this_thread::sleep_for(1s);
					this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					return;
				}
			} else {
				this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
				return;
			}
			std::cout << "WERE HERE THIS IS IT!" << std::endl;
			bool areWeDoneHeaders{};
			HttpsWorkloadData workloadData{};
			workloadData.baseUrl = "https://music.youtube.com/";
			int64_t remainingDownloadContentLength{ static_cast<int64_t>(newSong.contentLength) };
			int64_t bytesToRead{ static_cast<int64_t>(this->maxBufferSize) };
			int64_t bytesSubmittedPrevious{};
			int64_t bytesReadTotal{};
			const uint8_t maxReruns{ 200 };
			uint8_t currentReruns{};
			uint32_t counter{};
			uint32_t headerSize{};
			BuildAudioDecoderData dataPackage{};
			std::string currentString{};
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength);
			dataPackage.bufferMaxSize = this->maxBufferSize;
			dataPackage.configManager = this->configManager;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			std::string string = newSong.finalDownloadUrls[1].urlPath;
			if (streamSocket->writeData(string, true) != ProcessIOResult::Success) {
				std::this_thread::sleep_for(1s);
				this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
				return;
			}
			std::vector<DiscordCoreAPI::AudioFrameData> frames{};
			if (streamSocket->processIO(1000) != ProcessIOResult::Success) {
				std::this_thread::sleep_for(1s);
				this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
				return;
			}
			if (!streamSocket->areWeStillConnected()) {
				audioDecoder.reset(nullptr);
				streamSocket->disconnect();
				this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
				return;
			}
			while (remainingDownloadContentLength > 0) {
				std::this_thread::sleep_for(1ms);
				if (bytesSubmittedPrevious == bytesReadTotal) {
					++currentReruns;
				} else {
					currentReruns = 0;
				}
				if (currentReruns >= maxReruns) {
					DiscordCoreAPI::AudioFrameData frameData{};
					frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
					frameData.currentSize = 0;
					DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(frameData));
					streamSocket->disconnect();
					audioDecoder.reset(nullptr);
					this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					return;
				}
				bytesSubmittedPrevious = bytesReadTotal;
				if (audioDecoder->haveWeFailed()) {
					streamSocket->disconnect();
					audioDecoder.reset(nullptr);
					this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					return;
				}
				if (token.stop_requested()) {
					streamSocket->disconnect();
					audioDecoder.reset(nullptr);
					return;
				} else {
					if (!areWeDoneHeaders) {
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						if (streamSocket->processIO(10) != ProcessIOResult::Success) {
							std::this_thread::sleep_for(1s);
							this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
							return;
						}
						if (!streamSocket->areWeStillConnected()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
							return;
						}
						if (!token.stop_requested()) {
							if (streamSocket->areWeStillConnected()) {
								bytesReadTotal = streamSocket->getBytesRead() - headerSize;
								std::string streamBufferReal = static_cast<std::string>(streamSocket->getInputBuffer());
								headerSize = static_cast<int32_t>(streamBufferReal.size());
							}
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						areWeDoneHeaders = true;
					}
					if (token.stop_requested()) {
						streamSocket->disconnect();
						audioDecoder.reset(nullptr);
						return;
					}
					if (counter == 0) {
						if (streamSocket->processIO(10) != ProcessIOResult::Success) {
							std::this_thread::sleep_for(1s);
							this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
							return;
						}
						if (!streamSocket->areWeStillConnected()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
							return;
						}
						std::string streamBufferReal = static_cast<std::string>(streamSocket->getInputBuffer());
						if (streamBufferReal.size() > 0) {
							currentString.insert(currentString.end(), streamBufferReal.data(), streamBufferReal.data() + streamBufferReal.size());
							std::string submissionString{};
							if (currentString.size() >= this->maxBufferSize) {
								submissionString.insert(submissionString.begin(), currentString.data(), currentString.data() + this->maxBufferSize);
								currentString.erase(currentString.begin(), currentString.begin() + this->maxBufferSize);
							} else {
								submissionString = std::move(currentString);
								currentString.clear();
							}
							bytesReadTotal = streamSocket->getBytesRead();
							audioDecoder->submitDataForDecoding(std::move(submissionString));
						}
						audioDecoder->startMe();
					} else if (counter > 0) {
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						if (streamSocket->processIO(10) != ProcessIOResult::Success) {
							std::this_thread::sleep_for(1s);
							this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
							return;
						}
						if (!streamSocket->areWeStillConnected()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
							return;
						}
						std::string streamBufferReal = static_cast<std::string>(streamSocket->getInputBuffer());
						if (streamBufferReal.size() > 0) {
							currentString.insert(currentString.end(), streamBufferReal.data(), streamBufferReal.data() + streamBufferReal.size());
							while (currentString.size() > 0) {
								std::string submissionString{};
								if (currentString.size() >= this->maxBufferSize) {
									submissionString.insert(submissionString.begin(), currentString.data(),
										currentString.data() + this->maxBufferSize);
									currentString.erase(currentString.begin(), currentString.begin() + this->maxBufferSize);
								} else {
									submissionString = std::move(currentString);
									currentString.clear();
								}
								audioDecoder->submitDataForDecoding(std::move(submissionString));
								bytesReadTotal = streamSocket->getBytesRead();
							}
						}
						if (token.stop_requested()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							return;
						}
						while (true) {
							DiscordCoreAPI::AudioFrameData rawFrame{};
							if (!audioDecoder->getFrame(rawFrame)) {
								break;
							} else {
								if (rawFrame.currentSize == -5) {
									break;
								}
								if (rawFrame.currentSize > 3) {
									frames.emplace_back(std::move(rawFrame));
								}
							}
						}
						for (auto iterator = frames.begin(); iterator != frames.end();) {
							iterator->guildMemberId = static_cast<DiscordCoreAPI::Song>(newSong).addedByUserId.operator uint64_t();
							std::cout << "SENDING FRAMES SIZE: " << frames.size() << std::endl;
							DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(*iterator));
							iterator = frames.erase(iterator);
						}
					}
					if (remainingDownloadContentLength >= this->maxBufferSize) {
						bytesToRead = this->maxBufferSize;
					} else {
						bytesToRead = remainingDownloadContentLength;
					}
				}
				++counter;
			}
			DiscordCoreAPI::AudioFrameData frameData01{};
			while (audioDecoder->getFrame(frameData01)) {
			};
			audioDecoder.reset(nullptr);
			DiscordCoreAPI::AudioFrameData frameData{};
			frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
			frameData.currentSize = 0;
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(frameData));
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("YouTubeAPI::downloadAndStreamAudio()");
			}
			this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
		}
	}

	std::vector<DiscordCoreAPI::Song> YouTubeAPI::searchForSong(const std::string& searchQuery) {
		return this->collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(const DiscordCoreAPI::Song& newSong) {
		return YouTubeRequestBuilder::collectFinalSong(newSong);
	}

}
