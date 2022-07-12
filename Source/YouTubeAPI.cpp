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
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreInternal {

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
		HttpsWorkloadData dataPackage{ HttpsWorkloadType::YouTubeGetSearchResults };
		dataPackage.baseUrl = this->baseUrl;
		dataPackage.relativePath = "/results?search_query=" + DiscordCoreAPI::urlEncode(searchQuery.c_str());
		dataPackage.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
		if (returnData.responseCode != 200 && this->configManager->doWePrintHttpsErrorMessages()) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::collectSearchResults() Error: " << returnData.responseCode
					  << returnData.responseMessage.c_str() << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
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
					DiscordCoreAPI::Song searchResult{ value["videoRenderer"] };
					searchResult.type = DiscordCoreAPI::SongType::YouTube;
					searchResult.viewUrl = this->baseUrl + "/watch?v=" + searchResult.songId + "&hl=en";
					searchResults.push_back(searchResult);
				}
			}
		}
		return searchResults;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(DiscordCoreAPI::Song& newSong) {
		try {
			std::string apiKey{ "AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8" };
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
			dataPackage02.relativePath = "/youtubei/v1/player?key=" + apiKey;
			dataPackage02.content = theRequest.dump();
			dataPackage02.workloadClass = HttpsWorkloadClass::Post;
			HttpsResponseData responseData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
			if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200 && this->configManager->doWePrintHttpsErrorMessages()) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData.responseCode << ", "
						  << responseData.responseMessage << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
			newSong.type = DiscordCoreAPI::SongType::YouTube;
			nlohmann::json jsonObject = nlohmann::json::parse(responseData.responseMessage);
			DiscordCoreAPI::YouTubeFormatVector theVector{ jsonObject };
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
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("YouTubeRequestBuilder::constructDownloadInfo()");
			}
		}
		return DiscordCoreAPI::Song{};
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		newSong.firstDownloadUrl = this->baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
		newSong = this->constructDownloadInfo(newSong);
		return newSong;
	}

	YouTubeAPI::YouTubeAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClientNew, const DiscordCoreAPI::Snowflake& guildIdNew) {
		this->configManager = configManagerNew;
		this->httpsClient = httpsClientNew;
		this->guildId = guildIdNew;
	}

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		return YouTubeRequestBuilder::collectFinalSong(newSong);
	}

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, int32_t currentReconnectTries) {
		currentReconnectTries++;
		DiscordCoreAPI::GuildMember guildMember =
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
			std::unique_ptr<WebSocketSSLShard> streamSocket{ std::make_unique<WebSocketSSLShard>(nullptr, nullptr, 0, 0, this->configManager) };
			auto bytesRead{ static_cast<int32_t>(streamSocket->getBytesRead()) };
			if (newSong.finalDownloadUrls.size() > 0) {
				if (!streamSocket->connect(newSong.finalDownloadUrls[0].urlPath, "443")) {
					return;
				}
			} else {
				return;
			}
			bool areWeDoneHeaders{ false };
			int64_t remainingDownloadContentLength{ newSong.contentLength };
			int64_t bytesToRead{ this->maxBufferSize };
			int64_t bytesSubmittedPrevious{ 0 };
			int64_t bytesReadTotal{ 0 };
			const int8_t maxReruns{ 35 };
			int8_t currentReruns{ 0 };
			int32_t counter{ 0 };
			int32_t headerSize{ 0 };
			const int32_t ms1000{ 1000000 };
			BuildAudioDecoderData dataPackage{};
			std::string theCurrentString{};
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength);
			dataPackage.bufferMaxSize = this->maxBufferSize;
			dataPackage.configManager = this->configManager;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			AudioEncoder audioEncoder{};
			std::string theString = newSong.finalDownloadUrls[1].urlPath;
			streamSocket->writeData(theString, false);
			streamSocket->processIO(ms1000);
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
					frameData.rawFrameData.sampleCount = 0;
					frameData.encodedFrameData.sampleCount = 0;
					DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(frameData);
					audioDecoder.reset(nullptr);
					streamSocket->disconnect(false);
					return;
				}
				bytesSubmittedPrevious = bytesReadTotal;
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					streamSocket->disconnect(false);
					return;
				}
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
						if (stopToken.stop_requested()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							return;
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						streamSocket->processIO(ms1000);
						if (!streamSocket->areWeStillConnected()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						if (!stopToken.stop_requested()) {
							if (streamSocket->areWeStillConnected()) {
								bytesReadTotal = streamSocket->getBytesRead() - headerSize;
								std::string newData = streamSocket->getInputBuffer();
								headerSize = static_cast<int32_t>(newData.size());
							}
						}
						if (stopToken.stop_requested()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							return;
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
						streamSocket->processIO(ms1000);
						if (!streamSocket->areWeStillConnected()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						std::string streamBuffer{};
						streamBuffer = streamSocket->getInputBuffer();
						if (streamBuffer.size() > 0) {
							theCurrentString.insert(theCurrentString.end(), streamBuffer.begin(), streamBuffer.end());
							std::string submissionString{};
							if (theCurrentString.size() >= this->maxBufferSize) {
								submissionString.insert(submissionString.begin(), theCurrentString.begin(), theCurrentString.begin() + this->maxBufferSize);
								theCurrentString.erase(theCurrentString.begin(), theCurrentString.begin() + this->maxBufferSize);
							} else {
								submissionString = theCurrentString;
								theCurrentString.erase(theCurrentString.begin(), theCurrentString.end());
							}
							bytesReadTotal = streamSocket->getBytesRead() - headerSize;
							audioDecoder->submitDataForDecoding(submissionString);
						}
						audioDecoder->startMe();
					} else if (counter > 0) {
						if (stopToken.stop_requested()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							return;
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						streamSocket->processIO(ms1000);
						if (!streamSocket->areWeStillConnected()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						std::string newVector{};
						newVector = streamSocket->getInputBuffer();
						if (newVector.size() > 0) {
							theCurrentString.insert(theCurrentString.end(), newVector.begin(), newVector.end());
							std::string submissionString{};
							while (theCurrentString.size() > 0) {
								if (theCurrentString.size() >= this->maxBufferSize) {
									submissionString.insert(submissionString.begin(), theCurrentString.begin(), theCurrentString.begin() + this->maxBufferSize);
									theCurrentString.erase(theCurrentString.begin(), theCurrentString.begin() + this->maxBufferSize);
								} else {
									submissionString = theCurrentString;
									theCurrentString.erase(theCurrentString.begin(), theCurrentString.end());
								}
								audioDecoder->submitDataForDecoding(submissionString);
								bytesReadTotal = streamSocket->getBytesRead() - headerSize;
							}
						}
						if (stopToken.stop_requested()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							return;
						}
						std::vector<DiscordCoreAPI::RawFrameData> frames{};
						DiscordCoreAPI::RawFrameData rawFrame{};
						bool doWeBreak{ false };
						while (audioDecoder->getFrame(rawFrame)) {
							if (rawFrame.sampleCount == -5) {
								doWeBreak = true;
								break;
							}
							if (rawFrame.data.size() != 0) {
								frames.push_back(rawFrame);
							}
						}
						if (stopToken.stop_requested()) {
							audioDecoder.reset(nullptr);
							streamSocket->disconnect(false);
							return;
						}
						if (doWeBreak) {
							continue;
						}
						auto encodedFrames = audioEncoder.encodeFrames(frames);
						for (auto& value: encodedFrames) {
							value.guildMemberId = newSong.addedByUserId;
							DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(value);
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
			DiscordCoreAPI::RawFrameData frameData01{};
			while (audioDecoder->getFrame(frameData01)) {
			};
			audioDecoder.reset(nullptr);
			DiscordCoreAPI::AudioFrameData frameData{};
			frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
			frameData.rawFrameData.sampleCount = 0;
			frameData.encodedFrameData.sampleCount = 0;
			DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(frameData);
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

};