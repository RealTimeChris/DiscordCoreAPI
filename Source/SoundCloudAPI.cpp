/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

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
/// Chris M.
/// https://discordcoreapi.com
/// \file SoundCloudAPI.cpp

#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreAPI {

	Song SoundCloudRequestBuilder::collectFinalSong(GuildMemberData addedByGuildMember, Song newSong) {
		try {
			auto newerSong = constructSecondDownloadUrl(newSong);
			auto newestSong = constructFinalDownloadUrl(newerSong);
			newestSong.addedByUserId = addedByGuildMember.user.id;
			newestSong.addedByUserName = addedByGuildMember.user.userName;
			return newestSong;
		} catch (...) {
			reportException("SoundCloudRequestBuilder::collectFinalSong()");
		}
		return Song{};
	}

	std::vector<Song> SoundCloudRequestBuilder::collectSearchResults(std::string songQuery) {
		try {
			std::vector<Song> results{};
			std::unordered_map<std::string, std::string> theHeaders{
				std::make_pair(
					"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36"),
			};
			DiscordCoreInternal::HttpWorkloadData dataPackage{};
			dataPackage.baseUrl = SoundCloudRequestBuilder::baseUrl02;
			dataPackage.relativePath = "/search?q=" + urlEncode(songQuery.c_str()) + "&facet=model&client_id=" + SoundCloudRequestBuilder::clientId +
				"&limit=20&offset=0&linked_partitioning=1&app_version=" + SoundCloudRequestBuilder::appVersion + "&app_locale=en";
			dataPackage.headersToInsert = theHeaders;
			dataPackage.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
			workloadVector01.push_back(dataPackage);
			std::vector<DiscordCoreInternal::HttpData> returnData = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
			nlohmann::json data = nlohmann::json::parse(returnData[0].responseMessage);
			if (data.contains("collection") && !data["collection"].is_null()) {
				for (auto& value: data["collection"]) {
					Song newSong{};
					DiscordCoreInternal::DataParser::parseObject(value, newSong);
					if (!newSong.doWeGetSaved || newSong.songTitle == "") {
						continue;
					}
					newSong.type = SongType::SoundCloud;
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
			reportException("SoundCloudRequestBuilder::collectSearchResults()");
		}
		return std::vector<Song>();
	}

	Song SoundCloudRequestBuilder::constructSecondDownloadUrl(Song newSong) {
		try {
			DiscordCoreInternal::HttpWorkloadData dataPackage{};
			dataPackage.baseUrl = newSong.firstDownloadUrl;
			dataPackage.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
			workloadVector01.push_back(dataPackage);
			auto results = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
			nlohmann::json data = nlohmann::json::parse(results[0].responseMessage);
			if (data.contains("url")) {
				newSong.secondDownloadUrl = data["url"];
			}
			return newSong;
		} catch (...) {
			reportException("SoundCloudRequestBuilder::constructSecondDownloadUrl()");
		}
		return Song();
	}

	Song SoundCloudRequestBuilder::constructFinalDownloadUrl(Song newSong) {
		try {
			if (newSong.secondDownloadUrl.find("/playlist") != std::string::npos) {
				std::unique_ptr<DiscordCoreInternal::HttpWorkloadData> dataPackage{ std::make_unique<DiscordCoreInternal::HttpWorkloadData>() };
				dataPackage->baseUrl = newSong.secondDownloadUrl;
				dataPackage->workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
				std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
				workloadVector01.push_back(*dataPackage);
				std::unique_ptr<std::vector<DiscordCoreInternal::HttpData>> results = std::make_unique<std::vector<DiscordCoreInternal::HttpData>>(
					DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01));
				std::string newString{};
				newString.insert(newString.begin(), (*results)[0].responseMessage.begin(), (*results)[0].responseMessage.end());
				while (newString.find("#EXTINF:") != std::string::npos) {
					std::string newString01 = "#EXTINF:";
					std::string newString02 = newString.substr(newString.find("#EXTINF:") + newString01.size());
					std::string newString00 = newString02.substr(0, newString02.find(","));
					std::string newString03 = newString02.substr(newString02.find(",") + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
					newString = newString02.substr(newString02.find(","));
					if (newString03.find("#EXT-X-ENDLIST") != std::string::npos) {
						newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
					}
					std::string newString04 = newString03.substr(newString03.find_first_of("1234567890"));
					int32_t firstNumber01 = stoi(newString04.substr(0, newString04.find("/")));
					std::string newString05 = newString04.substr(newString04.find("/") + 1);
					int32_t secondNumber = stoi(newString05.substr(0, newString05.find("/")));
					DownloadUrl downloadUrl{};
					downloadUrl.urlPath = newString03;
					downloadUrl.contentSize = secondNumber - firstNumber01;
					newSong.finalDownloadUrls.push_back(downloadUrl);
				}
				for (auto& value: newSong.finalDownloadUrls) {
					newSong.contentLength += value.contentSize;
				}
			} else {
				std::unordered_map<std::string, std::string> theHeaders{
					std::make_pair(
						"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36"),
					std::make_pair("Path", newSong.secondDownloadUrl)
				};
				DiscordCoreInternal::HttpWorkloadData dataPackage02{};
				dataPackage02.baseUrl = newSong.secondDownloadUrl;
				dataPackage02.headersToInsert = theHeaders;
				dataPackage02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
				std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector{};
				workloadVector.push_back(dataPackage02);
				auto headersNew = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector);
				auto valueBitRate = stoll(headersNew[0].responseHeaders.find("x-amz-meta-bitrate")->second);
				auto valueLength = stoll(headersNew[0].responseHeaders.find("x-amz-meta-duration")->second);
				DownloadUrl downloadUrl{};
				downloadUrl.contentSize = static_cast<int32_t>(((valueBitRate * valueLength) / 8) - 193);
				downloadUrl.urlPath = newSong.secondDownloadUrl;
				newSong.finalDownloadUrls.push_back(downloadUrl);
			}

			return newSong;
		} catch (...) {
			reportException("SoundCloudRequestBuilder::constructFinalDownloadUrl()");
		}
		return Song();
	}

	std::string SoundCloudRequestBuilder::collectClientId() {
		try {
			std::unordered_map<std::string, std::string> theHeaders{
				std::make_pair(
					"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36"),
				std::make_pair("Path", "/search?q=testValue")
			};
			DiscordCoreInternal::HttpWorkloadData dataPackage02{};
			dataPackage02.baseUrl = SoundCloudRequestBuilder::baseUrl;
			dataPackage02.relativePath = "/search?q=testValue";
			dataPackage02.headersToInsert = theHeaders;
			dataPackage02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector{};
			workloadVector.push_back(dataPackage02);
			std::vector<DiscordCoreInternal::HttpData> returnData = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector);
			std::vector<std::string> assetPaths{};
			std::string newString01 = "crossorigin src=";
			std::string newerString{};
			newerString = returnData[0].responseMessage;
			std::string newString = newerString.substr(newerString.find("crossorigin src=") + newString01.size());
			std::string newString02 = newString.substr(1, newString.find(".js") + 2);
			assetPaths.push_back(newString02);
			while (newString.find("crossorigin src=") != std::string::npos) {
				std::string newString03 = newString.substr(1, newString.find(".js") + 2);
				newString = newString.substr(newString.find("crossorigin src=") + newString01.size());
				assetPaths.push_back(newString03);
			}
			DiscordCoreInternal::HttpWorkloadData dataPackage03{};
			dataPackage03.baseUrl = assetPaths[5];
			dataPackage03.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
			workloadVector01.push_back(dataPackage03);
			std::vector<DiscordCoreInternal::HttpData> returnData02 = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
			std::string newerString02{};
			newerString02.insert(newerString02.begin(), returnData02[0].responseMessage.begin(), returnData02[0].responseMessage.end());
			std::string newString03 = newerString02.substr(newerString02.find("client_id=") + std::string{ "client_id=" }.size());
			std::string clientIdNew = newString03.substr(0, newString03.find("\"),o.push"));
			if (returnData[0].responseCode != 200) {
				std::cout << shiftToBrightRed() << "SoundCloudAPI::searchForSong Error: " << returnData[0].responseCode << newerString02.c_str() << reset()
						  << std::endl;
			}
			return clientIdNew;
		} catch (...) {
			reportException("SoundCloudRequestBuilder::collectClientId()");
		}
		return std::string();
	}

	SoundCloudRequestBuilder::SoundCloudRequestBuilder(DiscordCoreInternal::HttpClient* httpClient) {
		this->httpClient = httpClient;
		if (SoundCloudRequestBuilder::clientId == "") {
			SoundCloudRequestBuilder::clientId = this->collectClientId();
		}
	}

	SoundCloudAPI::SoundCloudAPI(std::string guildIdNew, DiscordCoreInternal::HttpClient* httpClient) : requestBuilder(httpClient) {
		this->httpClient = httpClient;
		this->guildId = guildIdNew;
	}

	Song SoundCloudAPI::collectFinalSong(GuildMemberData addedByGuildMember, Song newSong) {
		return this->requestBuilder.collectFinalSong(addedByGuildMember, newSong);
	}

	void SoundCloudAPI::stop() {
		try {
			this->cancelCurrentSong();
			AudioFrameData dataFrame{};
			while (getVoiceConnectionMap()[this->guildId]->audioBuffer.tryReceive(dataFrame)) {
			};
		} catch (...) {
			reportException("SoundCloudAPI::stop()");
		}
	}

	void SoundCloudAPI::cancelCurrentSong() {
		if (getSongAPIMap().contains(this->guildId)) {
			if (getSongAPIMap()[this->guildId] != nullptr) {
				if (getSongAPIMap()[this->guildId]->theTask != nullptr) {
					getSongAPIMap()[this->guildId]->theTask->request_stop();
					if (getSongAPIMap()[this->guildId]->theTask->joinable()) {
						getSongAPIMap()[this->guildId]->theTask->join();
					}
					getSongAPIMap()[this->guildId]->theTask.reset(nullptr);
				}
			}
		}
	}

	std::vector<DiscordCoreInternal::HttpData> SoundCloudRequestBuilder::submitWorkloadAndGetResult(
		std::vector<DiscordCoreInternal::HttpWorkloadData> workload) {
		return DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workload);
	}

	void SoundCloudAPI::downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI, std::stop_token theToken) {
		try {
			int32_t counter{ 0 };
			BuildAudioDecoderData dataPackage{};
			dataPackage.totalFileSize = newSong.contentLength;
			dataPackage.bufferMaxSize = soundCloudAPI->maxBufferSize;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage, soundCloudAPI->httpClient->getDoWePrintFFMPEG());
			AudioEncoder audioEncoder = AudioEncoder();
		breakOutPlayMore:
			if (counter > 45 && !getVoiceConnectionMap()[soundCloudAPI->guildId]->areWeCurrentlyPlaying()) {
				audioDecoder.reset(nullptr);
				AudioFrameData frameData{};
				while (getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.tryReceive(frameData)) {
				};
				SongCompletionEventData eventData{};
				auto resultValue = getSongAPIMap()[soundCloudAPI->guildId].get();
				if (resultValue != nullptr) {
					eventData.previousSong = resultValue->getCurrentSong(soundCloudAPI->guildId);
				}
				eventData.wasItAFail = true;
				eventData.guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = soundCloudAPI->guildId }).get();
				eventData.guild = Guilds::getGuildAsync({ .guildId = soundCloudAPI->guildId }).get();
				getSongAPIMap()[soundCloudAPI->guildId]->onSongCompletionEvent(eventData);
				return;
			}
		breakOut:
			if (theToken.stop_requested()) {
				audioDecoder.reset(nullptr);
				AudioFrameData frameData{};
				while (getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.tryReceive(frameData)) {
				};
				frameData.type = AudioFrameType::Unset;
				frameData.rawFrameData.sampleCount = 0;
				frameData.rawFrameData.data.clear();
				frameData.encodedFrameData.sampleCount = 0;
				frameData.encodedFrameData.data.clear();
				getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.send(std::move(frameData));
				return;
			}
			while (counter < newSong.finalDownloadUrls.size()) {
				if (theToken.stop_requested()) {
					goto breakOut;
				}
				if (audioDecoder->haveWeFailed()) {
					goto breakOutPlayMore;
				}
				if (theToken.stop_requested()) {
					goto breakOut;
				}
				DiscordCoreInternal::HttpWorkloadData dataPackage03{};
				dataPackage03.baseUrl = newSong.finalDownloadUrls[counter].urlPath;
				dataPackage03.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
				std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector{};
				workloadVector.push_back(dataPackage03);
				auto result = this->requestBuilder.submitWorkloadAndGetResult(workloadVector);
				std::vector<uint8_t> newVector{};
				for (uint64_t x = 0; x < result[0].responseMessage.size(); x += 1) {
					newVector.push_back(result[0].responseMessage[x]);
				}
				int64_t amountToSubmitRemaining{ static_cast<int64_t>(result[0].responseMessage.size()) };
				int64_t amountToSubmitRemainingFinal{ 0 };
				int64_t amountSubmitted{ 0 };
				while (amountToSubmitRemaining > 0) {
					std::string newerVector{};
					if (amountToSubmitRemaining >= 8192) {
						for (int64_t x = 0; x < 8192; x += 1) {
							newerVector.push_back(newVector[amountSubmitted]);
							amountSubmitted += 1;
							amountToSubmitRemaining -= 1;
						}
					} else {
						amountToSubmitRemainingFinal = amountToSubmitRemaining;
						for (int64_t x = 0; x < amountToSubmitRemainingFinal; x += 1) {
							newerVector.push_back(newVector[amountSubmitted]);
							amountSubmitted += 1;
							amountToSubmitRemaining -= 1;
						}
					}
					audioDecoder->submitDataForDecoding(newerVector);
				}
				if (counter == 0) {
					audioDecoder->startMe();
				}
				std::vector<RawFrameData> frames{};
				RawFrameData rawFrame{};
				while (audioDecoder->getFrame(rawFrame)) {
					if (rawFrame.data.size() != 0) {
						frames.push_back(rawFrame);
					}
				}
				if (theToken.stop_requested()) {
					goto breakOut;
				} else {
					auto encodedFrames = audioEncoder.encodeFrames(frames);
					for (auto& value: encodedFrames) {
						value.guildMemberId = newSong.addedByUserId;
						getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.send(std::move(value));
					}
				}
				if (theToken.stop_requested()) {
					goto breakOut;
				}
				counter += 1;
			}
			RawFrameData frameData01{};
			while (audioDecoder->getFrame(frameData01)) {
			};
			audioDecoder.reset(nullptr);
			AudioFrameData frameData{};
			frameData.type = AudioFrameType::Skip;
			frameData.rawFrameData.sampleCount = 0;
			frameData.encodedFrameData.sampleCount = 0;
			getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.send(frameData);
			return;
		} catch (...) {
			reportException("SoundCloudAPI::downloadAndStreamAudio()");
		}
	};

	std::vector<Song> SoundCloudAPI::searchForSong(std::string searchQuery) {
		return this->requestBuilder.collectSearchResults(searchQuery);
	}

};