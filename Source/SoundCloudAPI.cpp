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
/// SoundCloudAPI.cpp - Source file for the SoundCloud api related stuff.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file SoundCloudAPI.cpp

#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreInternal {

	std::vector<DiscordCoreAPI::Song> SoundCloudRequestBuilder::collectSearchResults(const std::string& songQuery) {
		try {
			std::unordered_map<std::string, std::string> theHeaders{
				std::pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36"),
			};
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
			dataPackage.baseUrl = this->baseUrl02;
			dataPackage.relativePath = "/search?q=" + DiscordCoreAPI::urlEncode(songQuery.c_str()) + "&facet=model&client_id=" + SoundCloudRequestBuilder::clientId +
				"&limit=20&offset=0&linked_partitioning=1&app_version=" + this->appVersion + "&app_locale=en";
			dataPackage.headersToInsert = theHeaders;
			dataPackage.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
			nlohmann::json data = nlohmann::json::parse(returnData.responseMessage);
			std::vector<DiscordCoreAPI::Song> results{};
			if (data.contains("collection") && !data["collection"].is_null()) {
				for (auto& value: data["collection"]) {
					DiscordCoreAPI::Song newSong{ value };
					if (!newSong.doWeGetSaved || newSong.songTitle == "") {
						continue;
					}
					newSong.type = DiscordCoreAPI::SongType::SoundCloud;
					newSong.firstDownloadUrl += "?client_id=" + SoundCloudRequestBuilder::clientId + "&track_authorization=" + newSong.trackAuthorization;
					if (newSong.thumbnailUrl.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ") != std::string::npos &&
						newSong.thumbnailUrl.find("https") != std::string::npos) {
						std::string newString = newSong.thumbnailUrl.substr(0, newSong.thumbnailUrl.find_last_of("-t") + 1);
						newString += "t500x500.jpg";
						newSong.thumbnailUrl = newString;
					}
					results.push_back(newSong);
				}
			}
			return results;
		} catch (...) {
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("SoundCloudRequestBuilder::collectSearchResults()");
			}
		}
		return {};
	}

	DiscordCoreAPI::Song SoundCloudRequestBuilder::constructDownloadInfo(DiscordCoreAPI::Song& newSong) {
		try {
			HttpsWorkloadData dataPackage01{ HttpsWorkloadType::SoundCloudGetSearchResults };
			dataPackage01.baseUrl = newSong.firstDownloadUrl;
			dataPackage01.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData results = this->httpsClient->submitWorkloadAndGetResult(dataPackage01);
			if (results.responseData.contains("url")) {
				newSong.secondDownloadUrl = results.responseData["url"];
			}
			if (newSong.secondDownloadUrl.find("/playlist") != std::string::npos) {
				HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
				dataPackage.baseUrl = newSong.secondDownloadUrl;
				dataPackage.workloadClass = HttpsWorkloadClass::Get;
				HttpsResponseData results = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
				std::string newString{};
				newString.insert(newString.begin(), results.responseMessage.begin(), results.responseMessage.end());
				newSong.finalDownloadUrls.clear();
				while (newString.find("#EXTINF:") != std::string::npos) {
					std::string newString01 = "#EXTINF:";
					std::string newString02 = newString.substr(newString.find("#EXTINF:") + newString01.size());
					auto commandFind = newString02.find(",");
					std::string newString00 = newString02.substr(0, commandFind);
					std::string newString03 = newString02.substr(commandFind + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
					newString = newString02.substr(commandFind);
					if (newString03.find("#EXT-X-ENDLIST") != std::string::npos) {
						newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
					}
					std::string newString04 = newString03.substr(newString03.find_first_of("1234567890"));
					int32_t firstNumber01 = stoi(newString04.substr(0, newString04.find("/")));
					std::string newString05 = newString04.substr(newString04.find("/") + 1);
					int32_t secondNumber = stoi(newString05.substr(0, newString05.find("/")));
					DiscordCoreAPI::DownloadUrl downloadUrl{};
					downloadUrl.urlPath = newString03;
					downloadUrl.contentSize = secondNumber - firstNumber01;
					newSong.finalDownloadUrls.push_back(downloadUrl);
				}
				for (auto& value: newSong.finalDownloadUrls) {
					newSong.contentLength += value.contentSize;
				}
			} else {
				std::unordered_map<std::string, std::string> theHeaders{
					std::pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36"),
					std::pair("Path", newSong.secondDownloadUrl)
				};
				HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetSearchResults };
				dataPackage02.baseUrl = newSong.secondDownloadUrl;
				dataPackage02.headersToInsert = theHeaders;
				dataPackage02.workloadClass = HttpsWorkloadClass::Get;
				auto headersNew = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
				auto valueBitRate = stoll(headersNew.responseHeaders.find("x-amz-meta-bitrate")->second);
				auto valueLength = stoll(headersNew.responseHeaders.find("x-amz-meta-duration")->second);
				DiscordCoreAPI::DownloadUrl downloadUrl{};
				downloadUrl.contentSize = static_cast<int32_t>(((valueBitRate * valueLength) / 8) - 193);
				downloadUrl.urlPath = newSong.secondDownloadUrl;
				newSong.finalDownloadUrls.push_back(downloadUrl);
			}
			return newSong;
		} catch (...) {
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("SoundCloudRequestBuilder::constructDownloadInfo()");
			}
		}
		return {};
	}

	DiscordCoreAPI::Song SoundCloudRequestBuilder::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		newSong = constructDownloadInfo(newSong);
		return newSong;
	}

	std::string SoundCloudRequestBuilder::collectClientId() {
		std::unordered_map<std::string, std::string> theHeaders{
			std::pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36"),
			std::pair("Path", "/search?q=testValue")
		};
		HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetSearchResults };
		dataPackage02.baseUrl = this->baseUrl;
		dataPackage02.relativePath = "/search?q=testValue";
		dataPackage02.headersToInsert = theHeaders;
		dataPackage02.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
		std::vector<std::string> assetPaths{};
		std::string newString01 = "crossorigin src=";
		std::string newerString{};
		newerString = returnData.responseMessage;
		std::string newString = newerString.substr(newerString.find("crossorigin src=") + newString01.size());
		std::string newString02 = newString.substr(1, newString.find(".js") + 2);
		assetPaths.push_back(newString02);
		while (newString.find("crossorigin src=") != std::string::npos) {
			std::string newString03 = newString.substr(1, newString.find(".js") + 2);
			newString = newString.substr(newString.find("crossorigin src=") + newString01.size());
			assetPaths.push_back(newString03);
		}
		HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetSearchResults };
		dataPackage03.baseUrl = assetPaths[5];
		dataPackage03.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData02 = this->httpsClient->submitWorkloadAndGetResult(dataPackage03);
		std::string newerString02{};
		newerString02.insert(newerString02.begin(), returnData02.responseMessage.begin(), returnData02.responseMessage.end());
		std::string newString03 = newerString02.substr(newerString02.find("JSON.stringify({client_id:\"") + std::string{ "JSON.stringify({client_id:\"" }.size());
		std::string clientIdNew{};
		if (newString03.find("\",nonce:_})))),s.push(") != std::string::npos) {
			clientIdNew = newString03.substr(0, newString03.find("\",nonce:_})))),s.push("));
		}
		if (returnData02.responseCode not_eq 200 && this->configManager->doWePrintHttpsErrorMessages()) {
			cout << DiscordCoreAPI::shiftToBrightRed() << "SoundCloudAPI::searchForSong Error: " << returnData.responseCode << newerString02.c_str() << DiscordCoreAPI::reset()
				 << endl
				 << endl;
		}
		return clientIdNew;
	}

	SoundCloudAPI::SoundCloudAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClient, const DiscordCoreAPI::Snowflake& guildIdNew) {
		this->configManager = configManagerNew;
		this->httpsClient = httpsClient;
		this->guildId = guildIdNew;
		if (SoundCloudRequestBuilder::clientId == "") {
			SoundCloudRequestBuilder::clientId = this->collectClientId();
		}
	}

	void SoundCloudAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token& stopToken, int32_t currentReconnectTries) {
		currentReconnectTries++;
		DiscordCoreAPI::GuildMember guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = this->guildId }).get();
		auto newerSong = newSong;
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
			SoundCloudAPI::downloadAndStreamAudio(newerSong, stopToken, currentReconnectTries);
		}
	}

	void SoundCloudAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token& stopToken, int32_t currentReconnectTries) {
		try {
			int32_t counter{ 0 };
			BuildAudioDecoderData dataPackage{};
			HttpsConnection theConnection{};
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength);
			dataPackage.bufferMaxSize = this->maxBufferSize;
			dataPackage.configManager = this->configManager;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			AudioEncoder audioEncoder{};
			bool didWeGetZero{ true };
			while (counter < newSong.finalDownloadUrls.size()) {
				if (counter == newSong.finalDownloadUrls.size() && didWeGetZero) {
					audioDecoder.reset(nullptr);
					SoundCloudAPI::weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
					return;
				}
				std::this_thread::sleep_for(1ms);
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					return;
				}
				if (audioDecoder->haveWeFailed()) {
					audioDecoder.reset(nullptr);
					SoundCloudAPI::weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
					return;
				}
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					return;
				}
				HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetSearchResults };
				std::string baseUrl = newSong.finalDownloadUrls[counter].urlPath.substr(0, std::string{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
				std::string relativeUrl = newSong.finalDownloadUrls[counter].urlPath.substr(std::string{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());

				dataPackage03.baseUrl = baseUrl;
				dataPackage03.relativePath = relativeUrl;
				dataPackage03.workloadClass = HttpsWorkloadClass::Get;
				auto result = this->httpsClient->submitWorkloadAndGetResult(dataPackage03);
				if (result.responseMessage.size() != 0) {
					didWeGetZero = false;
				}
				int64_t amountToSubmitRemaining{ static_cast<int64_t>(result.responseMessage.size()) };
				int64_t amountToSubmitRemainingFinal{ 0 };
				int64_t amountSubmitted{ 0 };
				while (amountToSubmitRemaining > 0) {
					std::this_thread::sleep_for(1ms);
					std::string newerVector{};
					if (amountToSubmitRemaining >= this->maxBufferSize) {
						for (int64_t x = 0; x < this->maxBufferSize; x++) {
							newerVector.push_back(result.responseMessage[amountSubmitted]);
							amountSubmitted++;
							amountToSubmitRemaining--;
						}
					} else {
						amountToSubmitRemainingFinal = amountToSubmitRemaining;
						for (int64_t x = 0; x < amountToSubmitRemainingFinal; x++) {
							newerVector.push_back(result.responseMessage[amountSubmitted]);
							amountSubmitted++;
							amountToSubmitRemaining--;
						}
					}
					audioDecoder->submitDataForDecoding(newerVector);
				}
				if (counter == 0) {
					audioDecoder->startMe();
				}
				std::vector<DiscordCoreAPI::RawFrameData> frames{};
				DiscordCoreAPI::RawFrameData rawFrame{};
				while (audioDecoder->getFrame(rawFrame)) {
					if (rawFrame.data.size() != 0) {
						frames.push_back(rawFrame);
					}
				}
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					return;
				} else {
					auto encodedFrames = audioEncoder.encodeFrames(frames);
					for (auto& value: encodedFrames) {
						value.guildMemberId = newSong.addedByUserId;
						DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->audioDataBuffer.send(value);
					}
				}
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					return;
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
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("SoundCloudAPI::downloadAndStreamAudio()");
			}
			currentReconnectTries++;
			DiscordCoreAPI::getVoiceConnectionMap()[this->guildId]->clearAudioData();
			this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
		}
	};

	std::vector<DiscordCoreAPI::Song> SoundCloudAPI::searchForSong(const std::string& searchQuery) {
		return this->collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song SoundCloudAPI::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		return SoundCloudRequestBuilder::collectFinalSong(newSong);
	}

	std::string SoundCloudRequestBuilder::clientId{};
};