/*
*
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
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>

namespace DiscordCoreInternal {

	YouTubeRequestBuilder::YouTubeRequestBuilder(DiscordCoreInternal::HttpClient* theClient) {
		this->httpClient = theClient;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::collectFinalSong(const DiscordCoreAPI::GuildMemberData& addedByGuildMember, DiscordCoreAPI::Song& newSong) {
		newSong.firstDownloadUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
		auto newerSong = YouTubeRequestBuilder::constructDownloadInfo(addedByGuildMember, newSong);
		return newerSong;
	}

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(const std::string& searchQuery) {
		DiscordCoreInternal::HttpWorkloadData dataPackage{};
		dataPackage.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage.relativePath = "/results?search_query=" + DiscordCoreAPI::urlEncode(searchQuery.c_str());
		dataPackage.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage);
		std::vector<DiscordCoreInternal::HttpResponseData> returnData = this->httpClient->submitWorkloadAndGetResult<std::vector<HttpResponseData>>(workloadVector01);
		if (returnData.size() < 1) {
			return std::vector<DiscordCoreAPI::Song>{};
		}
		if (returnData[0].responseCode != 200 && this->httpClient->getDoWePrintHttpErrorMessages()) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::collectSearchResults() Error: " << returnData[0].responseCode
					  << returnData[0].responseMessage.c_str() << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
		}
		nlohmann::json partialSearchResultsJson{};
		if (returnData[0].responseMessage.find("var ytInitialData = ") != std::string::npos) {
			std::string newString00 = "var ytInitialData = ";
			std::string newString = returnData[0].responseMessage.substr(returnData[0].responseMessage.find("var ytInitialData = ") + newString00.length());
			std::string stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			partialSearchResultsJson = nlohmann::json::parse(newString);
		}
		std::vector<DiscordCoreAPI::Song> searchResults{};
		if (partialSearchResultsJson.contains("contents") && !partialSearchResultsJson["contents"].is_null()) {
			for (auto& value: partialSearchResultsJson["contents"]["twoColumnSearchResultsRenderer"]["primaryContents"]["sectionListRenderer"]["contents"][0]["itemSectionRenderer"]
													  ["contents"]) {
				DiscordCoreAPI::Song searchResult{};
				if (value.contains("videoRenderer") && !value["videoRenderer"].is_null()) {
					DiscordCoreInternal::DataParser::parseObject(value["videoRenderer"], searchResult);
					searchResult.type = DiscordCoreAPI::SongType::YouTube;
					searchResult.viewUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + searchResult.songId + "&hl=en";
					searchResults.push_back(searchResult);
				}
			}
		}
		return searchResults;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(const DiscordCoreAPI::GuildMemberData& guildMember, DiscordCoreAPI::Song& newSong) {
		if (newSong.firstDownloadUrl != "") {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
		}
		std::vector<DiscordCoreInternal::HttpWorkloadData> dataPackageWorkload{};
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
		DiscordCoreInternal::HttpWorkloadData dataPackage02{};
		dataPackage02.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage02.relativePath = "/youtubei/v1/player?key=" + apiKey;
		dataPackage02.content = theRequest.dump();
		dataPackage02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		dataPackageWorkload.push_back(dataPackage02);
		std::vector<DiscordCoreInternal::HttpResponseData> responseData = this->httpClient->submitWorkloadAndGetResult<std::vector<HttpResponseData>>(dataPackageWorkload);
		if (responseData.size() < 1) {
			return DiscordCoreAPI::Song{};
		}
		if (responseData[0].responseCode != 204 && responseData[0].responseCode != 201 && responseData[0].responseCode != 200 &&
			this->httpClient->getDoWePrintHttpErrorMessages()) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData[0].responseCode << ", "
					  << responseData[0].responseMessage << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
		}
		newSong.type = DiscordCoreAPI::SongType::YouTube;
		nlohmann::json jsonObject = nlohmann::json::parse(responseData[0].responseMessage);
		std::vector<DiscordCoreAPI::YouTubeFormat> theVector{};
		DiscordCoreInternal::DataParser::parseObject(jsonObject, theVector);
		DiscordCoreAPI::YouTubeFormat format{};
		bool isOpusFound{ false };
		for (auto& value: theVector) {
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
		if (newSong.format.downloadUrl.find("https://") != std::string::npos && newSong.format.downloadUrl.find("/videoplayback?") != std::string::npos) {
			std::string newString00 = "https://";
			downloadBaseUrl = newSong.format.downloadUrl.substr(newSong.format.downloadUrl.find("https://") + newString00.length(),
				newSong.format.downloadUrl.find("/videoplayback?") - newString00.length());
		}
		std::string request = "GET " + newSong.format.downloadUrl +
			" HTTP/1.1\n\rUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36\n\r";
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
		DiscordCoreAPI::StringWrapper theString = guildMember.userName;
		newSong.addedByUserName = theString; 
		newSong.contentLength = newSong.format.contentLength;
		newSong.addedByUserId = guildMember.id;
		newSong.type = DiscordCoreAPI::SongType::YouTube;
		return newSong;
	}

	YouTubeAPI::YouTubeAPI(const uint64_t& guildIdNew, DiscordCoreInternal::HttpClient* httpClient) : requestBuilder(httpClient) {
		this->doWePrintWebSocketErrorMessages = httpClient->getDoWePrintWebSocketErrorMessages();
		this->doWePrintFFMPEGSuccessMessages = httpClient->getDoWePrintFFMPEGSuccessMessages();
		this->doWePrintFFMPEGErrorMessages = httpClient->getDoWePrintFFMPEGErrorMessages();
		this->guildId = guildIdNew;
	}

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, YouTubeAPI* youtubeAPI, std::stop_token theToken, int32_t currentRecursionDepth) {
		DiscordCoreAPI::Song newerSong = newSong;
		currentRecursionDepth += 1;
		DiscordCoreAPI::GuildMember guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = youtubeAPI->guildId }).get();
		if (currentRecursionDepth > 9) {
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.tryReceive(frameData)) {
			};
			DiscordCoreAPI::SongCompletionEventData eventData{};
			auto returnValue = DiscordCoreAPI::getSongAPIMap()[youtubeAPI->guildId].get();
			if (returnValue) {
				eventData.previousSong = returnValue->getCurrentSong(youtubeAPI->guildId);
			}
			eventData.wasItAFail = true;
			eventData.guildMember = guildMember;
			eventData.guild = DiscordCoreAPI::Guilds::getGuildAsync({ .guildId = youtubeAPI->guildId }).get();
			DiscordCoreAPI::getSongAPIMap()[youtubeAPI->guildId]->onSongCompletionEvent(eventData);
			return;
		} else {
			DiscordCoreAPI::getSongAPIMap()[youtubeAPI->guildId].get()->sendNextSong();
			auto thePtr = DiscordCoreAPI::getSongAPIMap()[youtubeAPI->guildId].get();
			auto newerSong = thePtr->getCurrentSong(youtubeAPI->guildId);
			newerSong = youtubeAPI->requestBuilder.collectFinalSong(guildMember, newerSong);
			YouTubeAPI::downloadAndStreamAudio(newerSong, youtubeAPI, theToken, currentRecursionDepth);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, YouTubeAPI* youtubeAPI, std::stop_token theToken, int32_t currentRecursionDepth) {
		try {
			DiscordCoreInternal::WebSocketSSLClient streamSocket{ youtubeAPI->maxBufferSize };
			streamSocket.connect(newSong.finalDownloadUrls[0].urlPath, "443");
			bool areWeDoneHeaders{ false };
			bool haveWeFailed{ false };
			int64_t remainingDownloadContentLength{ newSong.contentLength };
			int64_t contentLengthCurrent{ youtubeAPI->maxBufferSize };
			int64_t bytesSubmittedPrevious{ 0 };
			int64_t bytesSubmittedTotal{ 0 };
			const int8_t maxReruns{ 10 };
			int8_t currentReruns{ 0 };
			int32_t counter{ 0 };
			const int32_t ms500{ 500000 };
			const int32_t ms1000{ 1000000 };
			BuildAudioDecoderData dataPackage{};
			std::string theCurrentString{};
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength);
			dataPackage.bufferMaxSize = youtubeAPI->maxBufferSize;
			dataPackage.doWePrintSuccessMessages = this->doWePrintFFMPEGSuccessMessages;
			dataPackage.doWePrintErrorMessages = this->doWePrintFFMPEGErrorMessages;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			AudioEncoder audioEncoder{};
			std::string theString = newSong.finalDownloadUrls[1].urlPath;
			streamSocket.writeData(theString);
			try {
				streamSocket.processIO(ms1000);
				while (newSong.contentLength > bytesSubmittedTotal) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
					if (bytesSubmittedPrevious == bytesSubmittedTotal) {
						currentReruns += 1;
					} else {
						currentReruns = 0;
					}
					if (currentReruns >= maxReruns) {
						DiscordCoreAPI::AudioFrameData frameData{};
						frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
						frameData.rawFrameData.sampleCount = 0;
						frameData.encodedFrameData.sampleCount = 0;
						DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.send(frameData);
						return;
					}
					bytesSubmittedPrevious = bytesSubmittedTotal;
					if (theToken.stop_requested()) {
						this->breakOut(theToken, std::move(audioDecoder), this);
						return;
					}
					if (audioDecoder->haveWeFailed()) {
						haveWeFailed = true;
						this->breakOutPlayMore(theToken, std::move(audioDecoder), haveWeFailed, counter, this, newSong, currentRecursionDepth);
						return;
					}
					if (theToken.stop_requested()) {
						this->breakOut(theToken, std::move(audioDecoder), this);
						return;
					} else {
						if (!areWeDoneHeaders) {
							if (theToken.stop_requested()) {
								this->breakOut(theToken, std::move(audioDecoder), this);
								return;
							}
							remainingDownloadContentLength = newSong.contentLength - bytesSubmittedTotal;
							streamSocket.processIO(ms1000);
							if (!theToken.stop_requested()) {
								bytesSubmittedTotal = streamSocket.getBytesRead();
							}
							std::string newData = streamSocket.getInputBuffer();
							if (theToken.stop_requested()) {
								this->breakOut(theToken, std::move(audioDecoder), this);
								return;
							}
							remainingDownloadContentLength = newSong.contentLength - bytesSubmittedTotal;
							areWeDoneHeaders = true;
						}
						if (theToken.stop_requested()) {
							this->breakOut(theToken, std::move(audioDecoder), this);
							return;
						}
						if (counter == 0) {
							streamSocket.processIO(ms500);
							std::string streamBuffer = streamSocket.getInputBuffer();
							if (streamBuffer.size() > 0) {
								theCurrentString.insert(theCurrentString.end(), streamBuffer.begin(), streamBuffer.end());
								std::string submissionString{};
								if (theCurrentString.size() >= youtubeAPI->maxBufferSize) {
									submissionString.insert(submissionString.begin(), theCurrentString.begin(), theCurrentString.begin() + youtubeAPI->maxBufferSize);
									theCurrentString.erase(theCurrentString.begin(), theCurrentString.begin() + youtubeAPI->maxBufferSize);
								} else {
									submissionString = theCurrentString;
									theCurrentString.erase(theCurrentString.begin(), theCurrentString.end());
								}
								bytesSubmittedTotal += submissionString.size();
								audioDecoder->submitDataForDecoding(submissionString);
							} else {
								continue;
							}
							audioDecoder->startMe();
						}
						if (counter > 0) {
							if (contentLengthCurrent > 0) {
								if (theToken.stop_requested()) {
									this->breakOut(theToken, std::move(audioDecoder), this);
									return;
								}
								remainingDownloadContentLength = newSong.contentLength - bytesSubmittedTotal;
								streamSocket.processIO(ms500);
								std::string newVector = streamSocket.getInputBuffer();
								if (newVector.size() == 0) {
									counter += 1;
									continue;
								}
								if (newVector.size() > 0) {
									theCurrentString.insert(theCurrentString.end(), newVector.begin(), newVector.end());
									std::string submissionString{};
									if (theCurrentString.size() >= youtubeAPI->maxBufferSize) {
										submissionString.insert(submissionString.begin(), theCurrentString.begin(), theCurrentString.begin() + youtubeAPI->maxBufferSize);
										theCurrentString.erase(theCurrentString.begin(), theCurrentString.begin() + youtubeAPI->maxBufferSize);
									} else {
										submissionString = theCurrentString;
										theCurrentString.erase(theCurrentString.begin(), theCurrentString.end());
									}
									audioDecoder->submitDataForDecoding(submissionString);
									bytesSubmittedTotal += submissionString.size();
								}
							}
							if (theToken.stop_requested()) {
								this->breakOut(theToken, std::move(audioDecoder), this);
								return;
							}
							std::vector<DiscordCoreAPI::RawFrameData> frames{};
							DiscordCoreAPI::RawFrameData rawFrame{};
							while (audioDecoder->getFrame(rawFrame)) {
								if (rawFrame.data.size() != 0) {
									frames.push_back(rawFrame);
								}
							}
							if (frames.size() == 0) {
								counter += 1;
								continue;
							}
							if (theToken.stop_requested()) {
								this->breakOut(theToken, std::move(audioDecoder), this);
								return;
							}
							auto encodedFrames = audioEncoder.encodeFrames(frames);
							for (auto& value: encodedFrames) {
								value.guildMemberId = newSong.addedByUserId;
								DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.send(value);
							}
						}
						if (remainingDownloadContentLength >= youtubeAPI->maxBufferSize) {
							contentLengthCurrent = youtubeAPI->maxBufferSize;
						} else {
							contentLengthCurrent = remainingDownloadContentLength;
						}
					}
					counter += 1;
				}
				DiscordCoreAPI::RawFrameData frameData01{};
				while (audioDecoder->getFrame(frameData01)) {
				};
				audioDecoder.reset(nullptr);
				DiscordCoreAPI::AudioFrameData frameData{};
				frameData.type = DiscordCoreAPI::AudioFrameType::Skip;
				frameData.rawFrameData.sampleCount = 0;
				frameData.encodedFrameData.sampleCount = 0;
				DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.send(frameData);
			} catch (ProcessingError&) {
				if (this->doWePrintWebSocketErrorMessages) {
					DiscordCoreAPI::reportException("YouTubeAPI::downloadAndStreamAudio()");
				}
				haveWeFailed = true;
				this->breakOutPlayMore(theToken, std::move(audioDecoder), haveWeFailed, counter, this, newSong, currentRecursionDepth);
			}
		} catch (std::runtime_error&) {
			if (this->doWePrintWebSocketErrorMessages) {
				DiscordCoreAPI::reportException("YouTubeAPI::downloadAndStreamAudio()");
			}
			this->weFailedToDownloadOrDecode(newSong, youtubeAPI, theToken, currentRecursionDepth);
		}
	};

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(const DiscordCoreAPI::GuildMemberData& addedByGuildMember, DiscordCoreAPI::Song& newSong) {
		return this->requestBuilder.collectFinalSong(addedByGuildMember, newSong);
	}

	std::vector<DiscordCoreAPI::Song> YouTubeAPI::searchForSong(const std::string& searchQuery) {
		return this->requestBuilder.collectSearchResults(searchQuery);
	}

	void YouTubeAPI::cancelCurrentSong() {
		if (DiscordCoreAPI::getSongAPIMap().contains(this->guildId)) {
			if (DiscordCoreAPI::getSongAPIMap()[this->guildId]) {
				if (DiscordCoreAPI::getSongAPIMap()[this->guildId]->theTask) {
					DiscordCoreAPI::getSongAPIMap()[this->guildId]->theTask->request_stop();
					if (DiscordCoreAPI::getSongAPIMap()[this->guildId]->theTask->joinable()) {
						DiscordCoreAPI::getSongAPIMap()[this->guildId]->theTask->join();
					}
					DiscordCoreAPI::getSongAPIMap()[this->guildId]->theTask.reset(nullptr);
				}
			}
		}
		DiscordCoreAPI::AudioFrameData dataFrame{};
		while (DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioBuffer.tryReceive(dataFrame)) {
		};
	}

	void YouTubeAPI::breakOutPlayMore(std::stop_token theToken, std::unique_ptr<AudioDecoder> audioDecoder, bool haveWeFailed, int32_t counter, YouTubeAPI* youtubeAPI,
		const DiscordCoreAPI::Song& newSong, int32_t currentRecursionDepth) {
		if (haveWeFailed) {
			audioDecoder.reset(nullptr);
			YouTubeAPI::weFailedToDownloadOrDecode(newSong, youtubeAPI, theToken, currentRecursionDepth);
		}
	}

	void YouTubeAPI::breakOut(std::stop_token theToken, std::unique_ptr<AudioDecoder> audioDecoder, YouTubeAPI* youtubeAPI) {
		if (theToken.stop_requested()) {
			audioDecoder.reset(nullptr);
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.tryReceive(frameData)) {
			};
			frameData.type = DiscordCoreAPI::AudioFrameType::Unset;
			frameData.rawFrameData.sampleCount = 0;
			frameData.rawFrameData.data.clear();
			frameData.encodedFrameData.sampleCount = 0;
			frameData.encodedFrameData.data.clear();
			DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.send(frameData);
		}
	}

};