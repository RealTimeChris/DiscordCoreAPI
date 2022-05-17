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
/// SoundCloudAPI.cpp - Source file for the SoundCloud api related stuff.
/// Aug 25, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file SoundCloudAPI.cpp

#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreInternal {

	DiscordCoreAPI::Song SoundCloudRequestBuilder::collectFinalSong(DiscordCoreAPI::GuildMemberData addedByGuildMember, DiscordCoreAPI::Song newSong) {
		auto newerSong = constructSecondDownloadUrl(newSong);
		auto newestSong = constructFinalDownloadUrl(newerSong);
		newestSong.addedByUserId = addedByGuildMember.user.id;
		newestSong.addedByUserName = addedByGuildMember.user.userName;
		return newestSong;
	}

	std::vector<DiscordCoreAPI::Song> SoundCloudRequestBuilder::collectSearchResults(const std::string& songQuery) {
		std::vector<DiscordCoreAPI::Song> results{};
		std::unordered_map<std::string, std::string> theHeaders{
			std::make_pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36"),
		};
		HttpWorkloadData dataPackage{};
		dataPackage.baseUrl = SoundCloudRequestBuilder::baseUrl02;
		dataPackage.relativePath = "/search?q=" + DiscordCoreAPI::urlEncode(songQuery.c_str()) + "&facet=model&client_id=" + SoundCloudRequestBuilder::clientId +
			"&limit=20&offset=0&linked_partitioning=1&app_version=" + SoundCloudRequestBuilder::appVersion + "&app_locale=en";
		dataPackage.headersToInsert = theHeaders;
		dataPackage.workloadClass = HttpWorkloadClass::Get;
		std::vector<HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage);
		std::vector<HttpData> returnData = this->submitWorkloadAndGetResultNew(workloadVector01);
		nlohmann::json data = nlohmann::json::parse(returnData[0].responseMessage);
		if (data.contains("collection") && !data["collection"].is_null()) {
			for (auto& value: data["collection"]) {
				DiscordCoreAPI::Song newSong{};
				DataParser::parseObject(value, newSong);
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
	}

	DiscordCoreAPI::Song SoundCloudRequestBuilder::constructSecondDownloadUrl(DiscordCoreAPI::Song newSong) {
		HttpWorkloadData dataPackage{};
		dataPackage.baseUrl = newSong.firstDownloadUrl;
		dataPackage.workloadClass = HttpWorkloadClass::Get;
		std::vector<HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage);
		auto results = this->submitWorkloadAndGetResultNew(workloadVector01);
		nlohmann::json data = nlohmann::json::parse(results[0].responseMessage);
		if (data.contains("url")) {
			newSong.secondDownloadUrl = data["url"];
		}
		return newSong;
	}

	DiscordCoreAPI::Song SoundCloudRequestBuilder::constructFinalDownloadUrl(DiscordCoreAPI::Song newSong) {
		if (newSong.secondDownloadUrl.find("/playlist") != std::string::npos) {
			HttpWorkloadData dataPackage{};
			dataPackage.baseUrl = newSong.secondDownloadUrl;
			dataPackage.workloadClass = HttpWorkloadClass::Get;
			std::vector<HttpWorkloadData> workloadVector01{};
			workloadVector01.push_back(dataPackage);
			std::vector<HttpData> results{ submitWorkloadAndGetResult(*this->httpClient, workloadVector01) };
			std::string newString{};
			newString.insert(newString.begin(), (results)[0].responseMessage.begin(), (results)[0].responseMessage.end());
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
				std::make_pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36"),
				std::make_pair("Path", newSong.secondDownloadUrl)
			};
			HttpWorkloadData dataPackage02{};
			dataPackage02.baseUrl = newSong.secondDownloadUrl;
			dataPackage02.headersToInsert = theHeaders;
			dataPackage02.workloadClass = HttpWorkloadClass::Get;
			std::vector<HttpWorkloadData> workloadVector{};
			workloadVector.push_back(dataPackage02);
			auto headersNew = submitWorkloadAndGetResult(*this->httpClient, workloadVector);
			auto valueBitRate = stoll(headersNew[0].responseHeaders.find("x-amz-meta-bitrate")->second);
			auto valueLength = stoll(headersNew[0].responseHeaders.find("x-amz-meta-duration")->second);
			DiscordCoreAPI::DownloadUrl downloadUrl{};
			downloadUrl.contentSize = static_cast<int32_t>(((valueBitRate * valueLength) / 8) - 193);
			downloadUrl.urlPath = newSong.secondDownloadUrl;
			newSong.finalDownloadUrls.push_back(downloadUrl);
		}

		return newSong;
	}

	std::string SoundCloudRequestBuilder::collectClientId() {
		std::unordered_map<std::string, std::string> theHeaders{
			std::make_pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36"),
			std::make_pair("Path", "/search?q=testValue")
		};
		HttpWorkloadData dataPackage02{};
		dataPackage02.baseUrl = SoundCloudRequestBuilder::baseUrl;
		dataPackage02.relativePath = "/search?q=testValue";
		dataPackage02.headersToInsert = theHeaders;
		dataPackage02.workloadClass = HttpWorkloadClass::Get;
		std::vector<HttpWorkloadData> workloadVector{};
		workloadVector.push_back(dataPackage02);
		std::vector<HttpData> returnData = submitWorkloadAndGetResult(*this->httpClient, workloadVector);
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
		HttpWorkloadData dataPackage03{};
		dataPackage03.baseUrl = assetPaths[5];
		dataPackage03.workloadClass = HttpWorkloadClass::Get;
		std::vector<HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage03);
		std::vector<HttpData> returnData02 = submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
		std::string newerString02{};
		newerString02.insert(newerString02.begin(), returnData02[0].responseMessage.begin(), returnData02[0].responseMessage.end());

		std::string newString03 = newerString02.substr(newerString02.find("client_id=") + std::string{ "client_id=" }.size());
		std::string clientIdNew{};
		if (newString03.find("\"),i.push") != std::string::npos) {
			clientIdNew = newString03.substr(0, newString03.find("\"),i.push"));
		} else if (newString03.find("\"),o.push") != std::string::npos) {
			clientIdNew = newString03.substr(0, newString03.find("\"),o.push"));
		}
		if (returnData[0].responseCode != 200) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "SoundCloudAPI::searchForSong Error: " << returnData[0].responseCode << newerString02.c_str()
					  << DiscordCoreAPI::reset() << std::endl;
		}
		return clientIdNew;
	}

	SoundCloudRequestBuilder::SoundCloudRequestBuilder(HttpClient* httpClient) {
		this->httpClient = httpClient;
		if (SoundCloudRequestBuilder::clientId == "") {
			SoundCloudRequestBuilder::clientId = this->collectClientId();
		}
	}

	SoundCloudAPI::SoundCloudAPI(const std::string& guildIdNew, HttpClient* httpClient) : requestBuilder(httpClient) {
		this->guildId = guildIdNew;
		this->doWePrintSuccess = httpClient->getDoWePrintFFMPEGSuccess();
		this->doWePrintError = httpClient->getDoWePrintFFMPEGError();
	}

	DiscordCoreAPI::Song SoundCloudAPI::collectFinalSong(DiscordCoreAPI::GuildMemberData addedByGuildMember, DiscordCoreAPI::Song newSong) {
		return this->requestBuilder.collectFinalSong(addedByGuildMember, newSong);
	}

	void SoundCloudAPI::cancelCurrentSong() {
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

	std::vector<HttpData> SoundCloudRequestBuilder::submitWorkloadAndGetResultNew(std::vector<HttpWorkloadData> workload) {
		return submitWorkloadAndGetResult(*this->httpClient, workload);
	}

	void SoundCloudAPI::weFailedToDownloadOrDecode(DiscordCoreAPI::Song newSong, SoundCloudAPI* soundCloudAPI, std::stop_token theToken, int32_t currentRecursionDepth) {
		currentRecursionDepth += 1;
		DiscordCoreAPI::GuildMember guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = soundCloudAPI->guildId }).get();
		if (currentRecursionDepth > 9) {
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.tryReceive(frameData)) {
			};
			DiscordCoreAPI::SongCompletionEventData eventData{};
			auto resultValue = DiscordCoreAPI::getSongAPIMap()[soundCloudAPI->guildId].get();
			if (resultValue) {
				eventData.previousSong = resultValue->getCurrentSong(soundCloudAPI->guildId);
			}
			eventData.wasItAFail = true;
			eventData.guildMember = guildMember;
			eventData.guild = DiscordCoreAPI::Guilds::getGuildAsync({ .guildId = soundCloudAPI->guildId }).get();
			DiscordCoreAPI::getSongAPIMap()[soundCloudAPI->guildId]->onSongCompletionEvent(eventData);
			return;
		} else {
			auto newerSong = soundCloudAPI->requestBuilder.collectFinalSong(guildMember, newSong);
			SoundCloudAPI::downloadAndStreamAudio(newerSong, soundCloudAPI, theToken, currentRecursionDepth);
		}
	}

	void SoundCloudAPI::breakOut(std::stop_token theToken, std::unique_ptr<AudioDecoder> audioDecoder, SoundCloudAPI* soundCloudAPI) {
		if (theToken.stop_requested()) {
			audioDecoder.reset(nullptr);
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.tryReceive(frameData)) {
			}
			frameData.type = DiscordCoreAPI::AudioFrameType::Unset;
			frameData.rawFrameData.sampleCount = 0;
			frameData.rawFrameData.data.clear();
			frameData.encodedFrameData.sampleCount = 0;
			frameData.encodedFrameData.data.clear();
			DiscordCoreAPI::getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.send(frameData);
			return;
		}
	}

	void SoundCloudAPI::breakOutPlayMore(std::stop_token theToken, std::unique_ptr<AudioDecoder> audioDecoder, bool haveWeFailed, int32_t counter, SoundCloudAPI* soundCloudAPI, DiscordCoreAPI::Song newSong, int32_t currentRecursionDepth) {
		if (haveWeFailed && counter > 45 && !DiscordCoreAPI::getVoiceConnectionMap()[soundCloudAPI->guildId]->areWeCurrentlyPlaying()) {
			audioDecoder.reset(nullptr);
			SoundCloudAPI::weFailedToDownloadOrDecode(newSong, soundCloudAPI, theToken, currentRecursionDepth);
			return;
		}
	}

	void SoundCloudAPI::downloadAndStreamAudio(DiscordCoreAPI::Song newSong, SoundCloudAPI* soundCloudAPI, std::stop_token theToken, int32_t currentRecursionDepth) {
		int32_t counter{ 0 };
		BuildAudioDecoderData dataPackage{};
		dataPackage.totalFileSize = newSong.contentLength;
		dataPackage.bufferMaxSize = soundCloudAPI->maxBufferSize;
		const int32_t bytesTotal{ 8192 };
		std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage, soundCloudAPI->doWePrintSuccess, soundCloudAPI->doWePrintError);
		AudioEncoder audioEncoder = AudioEncoder();
		bool haveWeFailed{ false };
		while (counter < newSong.finalDownloadUrls.size()) {
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
			}
			HttpWorkloadData dataPackage03{};
			dataPackage03.baseUrl = newSong.finalDownloadUrls[counter].urlPath;
			dataPackage03.workloadClass = HttpWorkloadClass::Get;
			std::vector<HttpWorkloadData> workloadVector{};
			workloadVector.push_back(dataPackage03);
			auto result = this->requestBuilder.submitWorkloadAndGetResultNew(workloadVector);
			std::vector<uint8_t> newVector{};
			for (uint64_t x = 0; x < result[0].responseMessage.size(); x += 1) {
				newVector.push_back(result[0].responseMessage[x]);
			}
			int64_t amountToSubmitRemaining{ static_cast<int64_t>(result[0].responseMessage.size()) };
			int64_t amountToSubmitRemainingFinal{ 0 };
			int64_t amountSubmitted{ 0 };
			while (amountToSubmitRemaining > 0) {
				std::string newerVector{};
				if (amountToSubmitRemaining >= bytesTotal) {
					for (int64_t x = 0; x < bytesTotal; x += 1) {
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
			std::vector<DiscordCoreAPI::RawFrameData> frames{};
			DiscordCoreAPI::RawFrameData rawFrame{};
			while (audioDecoder->getFrame(rawFrame)) {
				if (rawFrame.data.size() != 0) {
					frames.push_back(rawFrame);
				}
			}
			if (theToken.stop_requested()) {
				this->breakOut(theToken, std::move(audioDecoder), this);
				return;
			} else {
				auto encodedFrames = audioEncoder.encodeFrames(frames);
				for (auto& value: encodedFrames) {
					value.guildMemberId = newSong.addedByUserId;
					DiscordCoreAPI::getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.send(value);
				}
			}
			if (theToken.stop_requested()) {
				this->breakOut(theToken, std::move(audioDecoder), this);
				return;
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
		DiscordCoreAPI::getVoiceConnectionMap()[soundCloudAPI->guildId]->audioBuffer.send(frameData);
		return;
	};

	std::vector<DiscordCoreAPI::Song> SoundCloudAPI::searchForSong(const std::string& searchQuery) {
		return this->requestBuilder.collectSearchResults(searchQuery);
	}

};