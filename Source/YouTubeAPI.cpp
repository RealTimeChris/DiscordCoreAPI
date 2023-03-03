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
#include <discordcoreapi/DataParsingFunctions.hpp>

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
		simdjson::ondemand::value partialSearchResultsJson{};

		std::vector<DiscordCoreAPI::Song> searchResults{};
		auto varInitFind = returnData.responseData.find("var ytInitialData = ");
		if (varInitFind != std::string::npos) {
			std::string newString00 = "var ytInitialData = ";
			std::string newString = returnData.responseData.substr(varInitFind + newString00.length());
			std::string stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			newString.reserve(newString.size() + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser parser{};
			partialSearchResultsJson = parser.iterate(newString.data(), newString.length(), newString.capacity());
			simdjson::ondemand::value objectContents{};
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
												searchResult.type = DiscordCoreAPI::SongType::YouTube;
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
			}
		}
		return searchResults;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth) {
		HttpsResponseData responseData{};
		try {
			DiscordCoreAPI::Jsonifier request{};
			request["videoId"] = newSong.songId;
			request["contentCheckOk"] = true;
			request["racyCheckOk"] = true;
			request["context"]["client"]["clientName"] = "ANDROID";
			request["context"]["client"]["clientScreen"] = "EMBED";
			request["context"]["client"]["clientVersion"] = "17.46.37";
			request["context"]["client"]["hl"] = "en";
			request["context"]["client"]["gl"] = "US";
			request["context"]["client"]["utcOffsetMinutes"] = 0;
			request["context"]["embedUrl"] = "https://www.youtube.com";
			HttpsWorkloadData dataPackage02{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage02.baseUrl = YouTubeRequestBuilder::baseUrl;
			dataPackage02.relativePath = "/youtubei/v1/player?key=" + YouTubeRequestBuilder::apiKey;
			request.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
			dataPackage02.content = request.operator std::string();
			dataPackage02.workloadClass = HttpsWorkloadClass::Post;
			responseData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
			if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200 &&
				this->configManager->doWePrintHttpsErrorMessages()) {
				cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData.responseCode
					 << ", " << responseData.responseData << DiscordCoreAPI::reset() << endl
					 << endl;
			}
			newSong.type = DiscordCoreAPI::SongType::YouTube;
			responseData.responseData.reserve(responseData.responseData.size() + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser parser{};
			simdjson::ondemand::value value{};
			if (parser.iterate(responseData.responseData.data(), responseData.responseData.length(), responseData.responseData.capacity())
					.get(value) == simdjson::error_code::SUCCESS) {
				DiscordCoreAPI::YouTubeFormatVector vector{ value };
				DiscordCoreAPI::YouTubeFormat format{};
				bool isOpusFound{};
				for (auto& value: static_cast<std::vector<DiscordCoreAPI::YouTubeFormat>>(vector)) {
					if (value.mimeType.find("opus") != std::string::npos) {
						if (value.audioQuality == "AUDIO_QUALITY_LOW") {
							isOpusFound = true;
							format = value;
						}
						if (value.audioQuality == "AUDIO_QUALITY_MEDIUM") {
							isOpusFound = true;
							format = value;
						}
						if (value.audioQuality == "AUDIO_QUALITY_HIGH") {
							isOpusFound = true;
							format = value;
						}
					}
				}
				if (isOpusFound) {
					newSong.format = format;
				}
			}
			std::string downloadBaseUrl{};
			auto httpsFind = newSong.format.downloadUrl.find("https://");
			auto videoPlaybackFind = newSong.format.downloadUrl.find("/videoplayback?");
			if (httpsFind != std::string::npos && videoPlaybackFind != std::string::npos) {
				std::string newString00 = "https://";
				downloadBaseUrl = newSong.format.downloadUrl.substr(httpsFind + newString00.length(), videoPlaybackFind - newString00.length());
			}
			std::string requestNew = "GET " + newSong.format.downloadUrl + " HTTP/1.1\n\rHost: " + downloadBaseUrl + "\n\r\n\r";
			newSong.finalDownloadUrls.resize(2);
			DiscordCoreAPI::DownloadUrl downloadUrl01{};
			downloadUrl01.contentSize = newSong.contentLength;
			downloadUrl01.urlPath = downloadBaseUrl;
			DiscordCoreAPI::DownloadUrl downloadUrl02{};
			downloadUrl02.contentSize = newSong.contentLength;
			downloadUrl02.urlPath = requestNew;
			newSong.finalDownloadUrls[0] = downloadUrl01;
			newSong.finalDownloadUrls[1] = downloadUrl02;
			newSong.viewUrl = newSong.firstDownloadUrl;
			newSong.contentLength = newSong.format.contentLength;
			newSong.type = DiscordCoreAPI::SongType::YouTube;
			return newSong;
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

	DiscordCoreAPI::Song YouTubeRequestBuilder::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		newSong.firstDownloadUrl = this->baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
		newSong = this->constructDownloadInfo(newSong, 0);
		return newSong;
	}

	std::string YouTubeRequestBuilder::collectApiKey() {
		HttpsWorkloadData dataPackage01{ HttpsWorkloadType::YouTubeGetSearchResults };
		dataPackage01.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage01.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData responseData01 = this->httpsClient->submitWorkloadAndGetResult(dataPackage01);
		std::string apiKey{};
		if (responseData01.responseData.find("\"innertubeApiKey\":\"") != std::string::npos) {
			std::string newString = responseData01.responseData.substr(
				responseData01.responseData.find("\"innertubeApiKey\":\"") + std::string{ "\"innertubeApiKey\":\"" }.size());
			std::string apiKeyNew = newString.substr(0, newString.find_first_of('"'));
			apiKey = apiKeyNew;
		}
		return apiKey;
	}

	YouTubeAPI::YouTubeAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClientNew, const DiscordCoreAPI::Snowflake guildIdNew) {
		this->configManager = configManagerNew;
		this->httpsClient = httpsClientNew;
		this->guildId = static_cast<DiscordCoreAPI::Snowflake>(guildIdNew);
		if (YouTubeRequestBuilder::apiKey == "") {
			YouTubeRequestBuilder::apiKey = this->collectApiKey();
		}
	}

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries) {
		++currentReconnectTries;
		DiscordCoreAPI::GuildMemberData guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMember({ .guildMemberId = newSong.addedByUserId, .guildId = this->guildId });
		DiscordCoreAPI::Song newerSong = newSong;
		if (currentReconnectTries > 9) {
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.tryReceive(frameData)) {
			};
			DiscordCoreAPI::SongCompletionEventData eventData{};
			auto returnValue = DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId);
			if (returnValue) {
				eventData.previousSong = returnValue->getCurrentSong(this->guildId);
			}
			eventData.wasItAFail = true;
			eventData.guildMember = guildMember;
			eventData.guild = DiscordCoreAPI::Guilds::getGuildAsync({ .guildId = this->guildId }).get();
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->onSongCompletionEvent(eventData);
		} else {
			newerSong = this->collectFinalSong(newerSong);
			YouTubeAPI::downloadAndStreamAudio(newerSong, token, currentReconnectTries);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries) {
		try {
			std::unique_ptr<WebSocketClient> streamSocket{ std::make_unique<WebSocketClient>(nullptr, 0, nullptr) };
			auto bytesRead{ static_cast<int32_t>(streamSocket->getBytesRead()) };
			if (newSong.finalDownloadUrls.size() > 0) {
				if (!static_cast<TCPSSLClient*>(streamSocket.get())
						 ->connect(newSong.finalDownloadUrls[0].urlPath, 443, this->configManager->doWePrintWebSocketErrorMessages(), true)) {
					std::this_thread::sleep_for(1s);
					this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					return;
				}
			} else {
				this->weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
				return;
			}
			bool areWeDoneHeaders{};
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
			streamSocket->writeData(string, true);
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
							iterator->guildMemberId = static_cast<DiscordCoreAPI::Song>(newSong).addedByUserId.operator size_t();
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

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		return YouTubeRequestBuilder::collectFinalSong(newSong);
	}

	std::string YouTubeRequestBuilder::apiKey{};
}