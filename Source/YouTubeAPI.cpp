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

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(const String& searchQuery) {
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
		simdjson::ondemand::value partialSearchResultsJson{};

		std::vector<DiscordCoreAPI::Song> searchResults{};
		auto varInitFind = returnData.responseMessage.find("var ytInitialData = ");
		if (varInitFind != String::npos) {
			String newString00 = "var ytInitialData = ";
			String newString = returnData.responseMessage.substr(varInitFind + newString00.length());
			String stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			newString.reserve(newString.size() + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser theParser{};
			partialSearchResultsJson = theParser.iterate(newString.data(), newString.length(), newString.capacity());
			simdjson::ondemand::value theObjectContents{};
			if (partialSearchResultsJson["contents"].get(theObjectContents) == simdjson::error_code::SUCCESS) {
				simdjson::ondemand::value theObjectContents02{};
				if (theObjectContents["twoColumnSearchResultsRenderer"].get(theObjectContents02) == simdjson::error_code::SUCCESS) {
					simdjson::ondemand::value theObjectContents03{};
					if (theObjectContents02["primaryContents"].get(theObjectContents03) == simdjson::error_code::SUCCESS) {
						simdjson::ondemand::value theObjectContents04{};
						if (theObjectContents03["sectionListRenderer"].get(theObjectContents04) == simdjson::error_code::SUCCESS) {
							simdjson::ondemand::array theObjectContents05{};
							if (theObjectContents04["contents"].get(theObjectContents05) == simdjson::error_code::SUCCESS) {
								simdjson::ondemand::value theObjectContents06{};
								if (theObjectContents05.at(0).get(theObjectContents06) == simdjson::error_code::SUCCESS) {
									simdjson::ondemand::value theObjectContents07{};
									if (theObjectContents06["itemSectionRenderer"].get(theObjectContents07) == simdjson::error_code::SUCCESS) {
										simdjson::ondemand::array theObjectContents08{};
										if (theObjectContents07["contents"].get(theObjectContents08) == simdjson::error_code::SUCCESS) {
											for (auto iterator: theObjectContents08) {
												DiscordCoreAPI::Song searchResult{};
												simdjson::ondemand::value theObject{};
												if (iterator["videoRenderer"].get(theObject) == simdjson::error_code::SUCCESS) {
													searchResult = DiscordCoreAPI::Song{ theObject };
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

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(DiscordCoreAPI::Song& newSong, Int32 currentRecursionDepth) {
		HttpsResponseData responseData{};
		try {
			DiscordCoreAPI::JsonObject theRequest{};
			theRequest["videoId"] = newSong.songId;
			theRequest["contentCheckOk"] = true;
			theRequest["racyCheckOk"] = true;
			theRequest["context"]["client"]["clientName"] = "ANDROID";
			theRequest["context"]["client"]["clientScreen"] = "EMBED";
			theRequest["context"]["client"]["clientVersion"] = "16.46.37";
			theRequest["context"]["client"]["hl"] = "en";
			theRequest["context"]["client"]["gl"] = "US";
			theRequest["context"]["client"]["utcOffsetMinutes"] = 0;
			theRequest["context"]["embedUrl"] = "https://www.youtube.com";
			HttpsWorkloadData dataPackage02{ HttpsWorkloadType::YouTubeGetSearchResults };
			dataPackage02.baseUrl = YouTubeRequestBuilder::baseUrl;
			dataPackage02.relativePath = "/youtubei/v1/player?key=" + YouTubeRequestBuilder::apiKey;
			dataPackage02.content = theRequest;
			dataPackage02.workloadClass = HttpsWorkloadClass::Post;
			responseData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
			if (responseData.responseCode != 204 && responseData.responseCode != 201 && responseData.responseCode != 200 && this->configManager->doWePrintHttpsErrorMessages()) {
				cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData.responseCode << ", "
					 << responseData.responseMessage << DiscordCoreAPI::reset() << endl
					 << endl;
			}
			newSong.type = DiscordCoreAPI::SongType::YouTube;
			responseData.responseMessage.reserve(responseData.responseMessage.size() + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser theParser{};
			simdjson::ondemand::value theValue{};
			if (theParser.iterate(responseData.responseMessage.data(), responseData.responseMessage.length(), responseData.responseMessage.capacity()).get(theValue) ==
				simdjson::error_code::SUCCESS) {
				DiscordCoreAPI::YouTubeFormatVector theVector{ theValue };
				DiscordCoreAPI::YouTubeFormat format{};
				Bool isOpusFound{ false };
				for (auto& value: static_cast<std::vector<DiscordCoreAPI::YouTubeFormat>>(theVector)) {
					if (value.mimeType.find("opus") != String::npos) {
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
			String downloadBaseUrl{};
			auto httpsFind = newSong.format.downloadUrl.find("https://");
			auto videoPlaybackFind = newSong.format.downloadUrl.find("/videoplayback?");
			if (httpsFind != String::npos && videoPlaybackFind != String::npos) {
				String newString00 = "https://";
				downloadBaseUrl = newSong.format.downloadUrl.substr(httpsFind + newString00.length(), videoPlaybackFind - newString00.length());
			}
			String request = "GET " + newSong.format.downloadUrl +
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

	String YouTubeRequestBuilder::collectApiKey() {
		HttpsWorkloadData dataPackage01{ HttpsWorkloadType::YouTubeGetSearchResults };
		dataPackage01.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage01.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData responseData01 = this->httpsClient->submitWorkloadAndGetResult(dataPackage01);
		String apiKey{};
		if (responseData01.responseMessage.find("\"innertubeApiKey\":\"") != String::npos) {
			String newString = responseData01.responseMessage.substr(responseData01.responseMessage.find("\"innertubeApiKey\":\"") + String{ "\"innertubeApiKey\":\"" }.size());
			String apiKeyNew = newString.substr(0, newString.find_first_of('"'));
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

	void YouTubeAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, Int32 currentReconnectTries) {
		currentReconnectTries++;
		DiscordCoreAPI::GuildMemberData guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = this->guildId }).get();
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
			YouTubeAPI::downloadAndStreamAudio(newerSong, stopToken, currentReconnectTries);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, Int32 currentReconnectTries) {
		try {
			std::unique_ptr<WebSocketSSLShard> streamSocket{ std::make_unique<WebSocketSSLShard>(nullptr, nullptr, 0, nullptr) };
			auto bytesRead{ static_cast<Int32>(streamSocket->getBytesRead()) };
			if (newSong.finalDownloadUrls.size() > 0) {
				if (!streamSocket->connect(newSong.finalDownloadUrls[0].urlPath, "443", this->configManager->doWePrintWebSocketErrorMessages(), true)) {
					this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
					return;
				}
			} else {
				this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
				return;
			}
			Bool areWeDoneHeaders{ false };
			Uint64 remainingDownloadContentLength{ newSong.contentLength };
			Uint64 bytesToRead{ static_cast<Uint64>(this->maxBufferSize) };
			Uint64 bytesSubmittedPrevious{ 0 };
			Uint64 bytesReadTotal{ 0 };
			const Uint8 maxReruns{ 200 };
			Uint8 currentReruns{ 0 };
			Uint32 counter{ 0 };
			Uint32 headerSize{ 0 };
			BuildAudioDecoderData dataPackage{};
			String theCurrentString{};
			dataPackage.totalFileSize = static_cast<Uint64>(newSong.contentLength);
			dataPackage.bufferMaxSize = this->maxBufferSize;
			dataPackage.configManager = this->configManager;
			std::unique_ptr<DiscordCoreAPI::AudioEncoder> audioEncoder{ std::make_unique<DiscordCoreAPI::AudioEncoder>() };
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			String theString = newSong.finalDownloadUrls[1].urlPath;
			streamSocket->writeData(theString, true);
			streamSocket->processIO(1000);
			if (!streamSocket->areWeStillConnected()) {
				audioDecoder.reset(nullptr);
				streamSocket->disconnect();
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
					DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(frameData));
					streamSocket->disconnect();
					audioDecoder.reset(nullptr);
					return;
				}
				bytesSubmittedPrevious = bytesReadTotal;
				if (audioDecoder->haveWeFailed()) {
					streamSocket->disconnect();
					audioDecoder.reset(nullptr);
					this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
					return;
				}
				if (stopToken.stop_requested()) {
					streamSocket->disconnect();
					audioDecoder.reset(nullptr);
					return;
				} else {
					if (!areWeDoneHeaders) {
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						streamSocket->processIO(10);
						if (!streamSocket->areWeStillConnected()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						if (!stopToken.stop_requested()) {
							if (streamSocket->areWeStillConnected()) {
								bytesReadTotal = streamSocket->getBytesRead() - headerSize;
								String streamBufferReal = static_cast<String>(streamSocket->getInputBuffer());
								headerSize = static_cast<Int32>(streamBufferReal.size());
							}
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal;
						areWeDoneHeaders = true;
					}
					if (stopToken.stop_requested()) {
						streamSocket->disconnect();
						audioDecoder.reset(nullptr);
						return;
					}
					if (counter == 0) {
						streamSocket->processIO(10);
						if (!streamSocket->areWeStillConnected()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						String streamBufferReal = static_cast<String>(streamSocket->getInputBuffer());
						if (streamBufferReal.size() > 0) {
							theCurrentString.insert(theCurrentString.end(), streamBufferReal.data(), streamBufferReal.data() + streamBufferReal.size());
							String submissionString{};
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
						streamSocket->processIO(10);
						if (!streamSocket->areWeStillConnected()) {
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
							return;
						}
						String streamBufferReal = static_cast<String>(streamSocket->getInputBuffer());

						if (streamBufferReal.size() > 0) {
							theCurrentString.insert(theCurrentString.end(), streamBufferReal.data(), streamBufferReal.data() + streamBufferReal.size());
							while (theCurrentString.size() > 0) {
								String submissionString{};
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
							streamSocket->disconnect();
							audioDecoder.reset(nullptr);
							return;
						}
						std::vector<DiscordCoreAPI::AudioFrameData> frames{};
						Bool doWeContinue{ true };
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
							auto encodedFrame = audioEncoder->encodeSingleAudioFrame(value);
							encodedFrame.guildMemberId = static_cast<DiscordCoreAPI::Song>(newSong).addedByUserId;
							DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(encodedFrame));
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
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(frameData));
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("YouTubeAPI::downloadAndStreamAudio()");
			}
			this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
		}
	}

	std::vector<DiscordCoreAPI::Song> YouTubeAPI::searchForSong(const String& searchQuery) {
		return this->collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		return YouTubeRequestBuilder::collectFinalSong(newSong);
	}

	String YouTubeRequestBuilder::apiKey{};
}