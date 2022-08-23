/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
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

namespace DiscordCoreInternal {

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
		HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
		dataPackage.baseUrl = this->baseUrl;
		dataPackage.relativePath = "/results?search_query=" + DiscordCoreAPI::urlEncode(searchQuery.c_str());
		dataPackage.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
		if (returnData.responseCode != 200 && this->configManager->doWePrintHttpsErrorMessages()) {
			cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::collectSearchResults() Error: " << returnData.responseCode << returnData.responseMessage.c_str()
				 << DiscordCoreAPI::reset() << endl
				 << endl;
		}
		nlohmann::json partialSearchResultsJson{};
		auto varInitFind = returnData.responseMessage.find("var ytInitialData = ");
		if (varInitFind != std::string::npos) {
			std::string newString00 = "var ytInitialData = ";
			std::string newString = returnData.responseMessage.substr(varInitFind + newString00.length());
			std::string stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			partialSearchResultsJson = nlohmann::json::parse(newString);
		}
		std::vector<DiscordCoreAPI::Song> searchResults{};
		if (partialSearchResultsJson.contains("contents") && !partialSearchResultsJson["contents"].is_null()) {
			for (auto& value: partialSearchResultsJson["contents"]["twoColumnSearchResultsRenderer"]["primaryContents"]["sectionListRenderer"]["contents"][0]["itemSectionRenderer"]
													  ["contents"]) {
				if (value.contains("videoRenderer") && !value["videoRenderer"].is_null()) {
					DiscordCoreAPI::Song searchResult{ &value["videoRenderer"] };
					searchResult.type = DiscordCoreAPI::SongType::YouTube;
					searchResult.viewUrl = this->baseUrl + "/watch?v=" + searchResult.songId + "&hl=en";
					searchResults.emplace_back(searchResult);
				}
			}
		}
		return searchResults;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth) {
		try {
			nlohmann::json theRequest{};
			theRequest["videoId"] = newSong.songId;
			theRequest["contentCheckOk"] = true;
			theRequest["racyCheckOk"] = true;
			theRequest["context"];
			theRequest["context"]["client"];
			theRequest["context"]["client"]["clientName"] = "ANDROID";
			theRequest["context"]["client"]["clientScreen"] = "EMBED";
			theRequest["context"]["client"]["clientVersion"] = "16.46.37";
			theRequest["context"]["client"]["hl"] = "en";
			theRequest["context"]["client"]["gl"] = "US";
			theRequest["context"]["client"]["utcOffsetMinutes"] = 0;
			theRequest["context"]["thirdParty"];
			theRequest["context"]["thirdParty"]["embedUrl"] = "https://www.youtube.com";
			HttpsWorkloadData dataPackage02{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage02.baseUrl = YouTubeRequestBuilder::baseUrl;
			dataPackage02.relativePath = "/youtubei/v1/player?key=" + YouTubeRequestBuilder::apiKey;
			dataPackage02.content = theRequest.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
			;
			dataPackage02.workloadClass = HttpsWorkloadClass::Post;
			HttpsResponseData responseData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
			if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200 && this->configManager->doWePrintHttpsErrorMessages()) {
				cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData.responseCode << ", "
					 << responseData.responseMessage << DiscordCoreAPI::reset() << endl
					 << endl;
			}
			newSong.type = DiscordCoreAPI::SongType::YouTube;
			nlohmann::json jsonObject = nlohmann::json::parse(responseData.responseMessage);
			DiscordCoreAPI::YouTubeFormatVector theVector{ &jsonObject };
			DiscordCoreAPI::YouTubeFormat format{};
			bool isOpusFound{ false };
			for (auto& value: static_cast<std::vector<DiscordCoreAPI::YouTubeFormat>>(theVector)) {
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
			std::string downloadBaseUrl{};
			auto httpsFind = newSong.format.downloadUrl.find("https://");
			auto videoPlaybackFind = newSong.format.downloadUrl.find("/videoplayback?");
			if (httpsFind != std::string::npos && videoPlaybackFind != std::string::npos) {
				std::string newString00 = "https://";
				downloadBaseUrl = newSong.format.downloadUrl.substr(httpsFind + newString00.length(), videoPlaybackFind - newString00.length());
			}
			std::string request = "GET " + newSong.format.downloadUrl +
				" HTTP/1.1\n\rUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
				"(KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36\n\r";
			request += "Host: " + downloadBaseUrl + "\n\r\n\r";
			newSong.finalDownloadUrls.resize(2);
			DiscordCoreAPI::DownloadUrl downloadUrl01{};
			downloadUrl01.contentSize = newSong.contentLength;
			downloadUrl01.urlPath = downloadBaseUrl;
			DiscordCoreAPI::DownloadUrl downloadUrl02{};
			downloadUrl02.contentSize = newSong.contentLength;
			downloadUrl02.urlPath = request;
			newSong.finalDownloadUrls[0] = downloadUrl01;
			newSong.finalDownloadUrls[1] = downloadUrl02;
			newSong.viewUrl = newSong.firstDownloadUrl;
			newSong.contentLength = newSong.format.contentLength;
			newSong.type = DiscordCoreAPI::SongType::YouTube;
			return newSong;
		} catch (...) {
			if (currentRecursionDepth <= 10) {
				currentRecursionDepth++;
				return this->constructDownloadInfo(newSong, currentRecursionDepth);
			} else {
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
		if (responseData01.responseMessage.find("\"innertubeApiKey\":\"") != std::string::npos) {
			std::string newString =
				responseData01.responseMessage.substr(responseData01.responseMessage.find("\"innertubeApiKey\":\"") + std::string{ "\"innertubeApiKey\":\"" }.size());
			std::string apiKeyNew = newString.substr(0, newString.find_first_of('"'));
			apiKey = apiKeyNew;
		}
		return apiKey;
	}

	YouTubeAPI::YouTubeAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClientNew, const DiscordCoreAPI::Snowflake guildIdNew) {
		this->configManager = configManagerNew;
		this->httpsClient = httpsClientNew;
		this->guildId = guildIdNew;
		if (YouTubeRequestBuilder::apiKey == "") {
			YouTubeRequestBuilder::apiKey = this->collectApiKey();
		}
	}

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, int32_t currentReconnectTries) {
		currentReconnectTries++;
		DiscordCoreAPI::GuildMemberData guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = this->guildId }).get();
		DiscordCoreAPI::Song newerSong = newSong;
		if (currentReconnectTries > 9) {
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.tryReceive(frameData)) {
			};
			DiscordCoreAPI::SongCompletionEventData eventData{};
			auto returnValue = DiscordCoreAPI::getSongAPIMap()[this->guildId].get();
			if (returnValue) {
				eventData.previousSong = returnValue->getCurrentSong(this->guildId);
			}
			eventData.wasItAFail = true;
			eventData.guildMember = guildMember;
			eventData.guild = DiscordCoreAPI::Guilds::getGuildAsync({ .guildId = this->guildId }).get();
			DiscordCoreAPI::getSongAPIMap()[this->guildId]->onSongCompletionEvent(eventData);
		} else {
			newerSong = this->collectFinalSong(newerSong);
			YouTubeAPI::downloadAndStreamAudio(newerSong, stopToken, currentReconnectTries);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, int32_t currentReconnectTries) {
		try {
			std::unique_ptr<WebSocketSSLShard> streamSocket{ std::make_unique<WebSocketSSLShard>(nullptr, nullptr, 0, nullptr) };
			auto bytesRead{ static_cast<int32_t>(streamSocket->getBytesRead()) };
			if (newSong.finalDownloadUrls.size() > 0) {
				if (!streamSocket->connect(newSong.finalDownloadUrls[0].urlPath, "443", this->configManager->doWePrintWebSocketErrorMessages())) {
					return;
				}
			} else {
				return;
			}
			bool areWeDoneHeaders{ false };
			uint64_t remainingDownloadContentLength{ newSong.contentLength };
			uint64_t bytesToRead{ static_cast<uint64_t>(this->maxBufferSize) };
			uint64_t bytesSubmittedPrevious{ 0 };
			uint64_t bytesReadTotal{ 0 };
			const uint8_t maxReruns{ 35 };
			uint8_t currentReruns{ 0 };
			uint32_t counter{ 0 };
			uint32_t headerSize{ 0 };
			BuildAudioDecoderData dataPackage{};
			std::string theCurrentString{};
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength);
			dataPackage.bufferMaxSize = this->maxBufferSize;
			dataPackage.configManager = this->configManager;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			DiscordCoreAPI::AudioEncoder audioEncoder{};
			std::string theString = newSong.finalDownloadUrls[1].urlPath;
			streamSocket->writeData(theString, false);
			streamSocket->processIO(1000000);
			if (!streamSocket->areWeStillConnected()) {
				audioDecoder.reset(nullptr);
				streamSocket->disconnect(false);
				this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
				return;
			}
			while (bytesToRead > 0) {
				std::this_thread::sleep_for(1ms);
				if (bytesSubmittedPrevious == bytesReadTotal) {
					currentReruns++;
				} else {
					currentReruns = 0;
				}
				if (currentReruns >= maxReruns) {
					DiscordCoreAPI::AudioFrameData frameData{};
					frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
					frameData.sampleCount = 0;
					DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(std::move(frameData));
					audioDecoder.reset(nullptr);
					streamSocket->disconnect(false);
					return;
				}
				bytesSubmittedPrevious = bytesReadTotal;
				if (audioDecoder->haveWeFailed()) {
					audioDecoder.reset(nullptr);
					streamSocket->disconnect(false);
					this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
					return;
				}
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					streamSocket->disconnect(false);
					return;
				} else {
					if (!areWeDoneHeaders) {
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						streamSocket->processIO(100000);
						if (!streamSocket->areWeStillConnected()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						if (!stopToken.stop_requested()) {
							if (streamSocket->areWeStillConnected()) {
								bytesReadTotal = streamSocket->getBytesRead() - headerSize;
								std::string streamBuffer = streamSocket->getInputBufferCopy();
								headerSize = static_cast<int32_t>(streamBuffer.size());
							}
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						areWeDoneHeaders = true;
					}
					if (stopToken.stop_requested()) {
						audioDecoder.reset(nullptr);
						streamSocket->disconnect(false);
						return;
					}
					if (counter == 0) {
						streamSocket->processIO(10000);
						if (!streamSocket->areWeStillConnected()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						std::string streamBuffer = streamSocket->getInputBufferCopy();
						if (streamBuffer.size() > 0) {
							theCurrentString.insert(theCurrentString.end(), streamBuffer.data(), streamBuffer.data() + streamBuffer.size());
							std::string submissionString{};
							if (theCurrentString.size() >= this->maxBufferSize) {
								submissionString.insert(submissionString.begin(), theCurrentString.data(), theCurrentString.data() + this->maxBufferSize);
								theCurrentString.erase(theCurrentString.begin(), theCurrentString.begin() + this->maxBufferSize);
							} else {
								submissionString = std::move(theCurrentString);
								theCurrentString.clear();
							}
							bytesReadTotal = streamSocket->getBytesRead();
							audioDecoder->submitDataForDecoding(std::move(submissionString));
						}
						audioDecoder->startMe();
					} else if (counter > 0) {
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						streamSocket->processIO(10000);
						if (!streamSocket->areWeStillConnected()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						std::string streamBuffer = streamSocket->getInputBufferCopy();
						if (streamBuffer.size() > 0) {
							theCurrentString.insert(theCurrentString.end(), streamBuffer.data(), streamBuffer.data() + streamBuffer.size());
							while (theCurrentString.size() > 0) {
								std::string submissionString{};
								if (theCurrentString.size() >= this->maxBufferSize) {
									submissionString.insert(submissionString.begin(), theCurrentString.data(), theCurrentString.data() + this->maxBufferSize);
									theCurrentString.erase(theCurrentString.begin(), theCurrentString.begin() + this->maxBufferSize);
								} else {
									submissionString = std::move(theCurrentString);
									theCurrentString.clear();
								}
								audioDecoder->submitDataForDecoding(std::move(submissionString));
								bytesReadTotal = streamSocket->getBytesRead();
							}
						}
						if (stopToken.stop_requested()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							return;
						}
						std::vector<DiscordCoreAPI::AudioFrameData> frames{};
						bool doWeContinue{ true };
						while (doWeContinue) {
							DiscordCoreAPI::AudioFrameData rawFrame{};
							doWeContinue = audioDecoder->getFrame(rawFrame);
							if (rawFrame.sampleCount == -5) {
								doWeContinue = false;
								break;
							}
							if (rawFrame.data.size() != 0) {
								frames.emplace_back(std::move(rawFrame));
							}
						}
						for (auto& value: frames) {
							auto encodedFrame = audioEncoder.encodeSingleAudioFrame(value);
							encodedFrame.guildMemberId = newSong.addedByUserId;
							DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(std::move(encodedFrame));
						}
					}
					if (remainingDownloadContentLength >= this->maxBufferSize) {
						bytesToRead = this->maxBufferSize;
					} else {
						bytesToRead = remainingDownloadContentLength;
					}
				}
				counter++;
			}
			DiscordCoreAPI::AudioFrameData frameData01{};
			while (audioDecoder->getFrame(frameData01)) {
			};
			audioDecoder.reset(nullptr);
			DiscordCoreAPI::AudioFrameData frameData{};
			frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
			frameData.sampleCount = 0;
			DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(std::move(frameData));
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("YouTubeAPI::downloadAndStreamAudio()");
			}
			this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
		}
	}

	std::vector<DiscordCoreAPI::Song> YouTubeAPI::searchForSong(const std::string& searchQuery) {
		return this->collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		return YouTubeRequestBuilder::collectFinalSong(newSong);
	}

	std::string YouTubeRequestBuilder::apiKey{};
};