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
/// SoundCloudAPI.cpp - Source file for the SoundCloud api related stuff.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file SoundCloudAPI.cpp

#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreInternal {

	std::vector<DiscordCoreAPI::Song> SoundCloudRequestBuilder::collectSearchResults(const String& songQuery) {
		try {
			std::unordered_map<String, String> theHeaders{
				std::pair("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36"),
			};
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
			dataPackage.baseUrl = this->baseUrl02;
			dataPackage.relativePath = "/search?q=" + DiscordCoreAPI::urlEncode(songQuery.c_str()) + "&facet=model&client_id=" + SoundCloudRequestBuilder::clientId +
				"&limit=20&offSet=0&linked_partitioning=1&app_version=" + this->appVersion + "&app_locale=en";
			dataPackage.headersToInsert = theHeaders;
			dataPackage.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
			std::vector<DiscordCoreAPI::Song> results{};
			simdjson::ondemand::parser theParser{};
			returnData.responseMessage.reserve(returnData.responseMessage.size() + simdjson::SIMDJSON_PADDING);
			auto theDocument = theParser.iterate(returnData.responseMessage.data(), returnData.responseMessage.length(), returnData.responseMessage.capacity());
			simdjson::ondemand::array theArray{};
			auto theResult = theDocument.get_value().value().get_object().value()["collection"].get(theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				for (auto value: theArray) {
					DiscordCoreAPI::Song newSong{ value.value() };
					if (newSong.songTitle == "") {
						continue;
					}
					newSong.type = DiscordCoreAPI::SongType::SoundCloud;
					newSong.firstDownloadUrl += "?client_id=" + SoundCloudRequestBuilder::clientId + "&track_authorization=" + newSong.trackAuthorization;
					if (newSong.thumbnailUrl.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ") != String::npos &&
						newSong.thumbnailUrl.find("https") != String::npos) {
						String newString = newSong.thumbnailUrl.substr(0, newSong.thumbnailUrl.find_last_of("-t") + 1);
						newString += "t500x500.jpg";
						newSong.thumbnailUrl = newString;
					}
					results.emplace_back(newSong);
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
			simdjson::ondemand::parser theParser{};
			results.responseMessage.reserve(results.responseMessage.size() + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::object theDocument = theParser.iterate(results.responseMessage).get_value().value().get_object().value();
			StringView theUrl{};
			auto theResult = theDocument["url"].get(theUrl);
			if (theResult == simdjson::error_code::SUCCESS) {
				newSong.secondDownloadUrl = static_cast<String>(theUrl);
			}
			if (newSong.secondDownloadUrl.find("/playlist") != String::npos) {
				HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
				dataPackage.baseUrl = newSong.secondDownloadUrl;
				dataPackage.workloadClass = HttpsWorkloadClass::Get;
				HttpsResponseData results = this->httpsClient->submitWorkloadAndGetResult(dataPackage);
				String newString{};
				newString.insert(newString.begin(), results.responseMessage.begin(), results.responseMessage.end());
				newSong.finalDownloadUrls.clear();
				while (newString.find("#EXTINF:") != String::npos) {
					String newString01 = "#EXTINF:";
					String newString02 = newString.substr(newString.find("#EXTINF:") + newString01.size());
					auto commandFind = newString02.find(",");
					String newString00 = newString02.substr(0, commandFind);
					String newString03 = newString02.substr(commandFind + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
					newString = newString02.substr(commandFind);
					if (newString03.find("#EXT-X-ENDLIST") != String::npos) {
						newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
					}
					String newString04 = newString03.substr(newString03.find_first_of("1234567890"));
					Int32 firstNumber01 = stoi(newString04.substr(0, newString04.find("/")));
					String newString05 = newString04.substr(newString04.find("/") + 1);
					Int32 secondNumber = stoi(newString05.substr(0, newString05.find("/")));
					DiscordCoreAPI::DownloadUrl downloadUrl{};
					downloadUrl.urlPath = newString03;
					downloadUrl.contentSize = secondNumber - firstNumber01;
					newSong.finalDownloadUrls.emplace_back(downloadUrl);
				}
				for (auto& value: newSong.finalDownloadUrls) {
					newSong.contentLength += value.contentSize;
				}
			} else {
				std::unordered_map<String, String> theHeaders{
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
				downloadUrl.contentSize = static_cast<Int32>(((valueBitRate * valueLength) / 8) - 193);
				downloadUrl.urlPath = newSong.secondDownloadUrl;
				newSong.finalDownloadUrls.emplace_back(downloadUrl);
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

	String SoundCloudRequestBuilder::collectClientId() {
		std::unordered_map<String, String> theHeaders{ std::pair("User-Agent",
														   "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36"),
			std::pair("Path", "/search?q=testValue") };
		HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetSearchResults };
		dataPackage02.baseUrl = this->baseUrl;
		dataPackage02.relativePath = "/search?q=testValue";
		dataPackage02.headersToInsert = theHeaders;
		dataPackage02.workloadClass = HttpsWorkloadClass::Get;
		HttpsResponseData returnData = this->httpsClient->submitWorkloadAndGetResult(dataPackage02);
		std::vector<String> assetPaths{};
		String newString01 = "crossorigin src=";
		String newerString{};
		newerString = returnData.responseMessage;
		String clientIdNew{};
		if (newerString.find(newString01) != String::npos) {
			String newString = newerString.substr(newerString.find(newString01) + newString01.size());
			String newString02 = newString.substr(1, newString.find(".js") + 2);
			assetPaths.emplace_back(newString02);
			while (newString.find("crossorigin src=") != String::npos) {
				String newString03 = newString.substr(1, newString.find(".js") + 2);
				newString = newString.substr(newString.find("crossorigin src=") + newString01.size());
				assetPaths.emplace_back(newString03);
			}
			HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetSearchResults };
			dataPackage03.baseUrl = assetPaths[5];
			dataPackage03.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData returnData02 = this->httpsClient->submitWorkloadAndGetResult(dataPackage03);
			String newerString02{};
			newerString02.insert(newerString02.begin(), returnData02.responseMessage.begin(), returnData02.responseMessage.end());

			String newString03 = newerString02.substr(newerString02.find("JSON.stringify({client_id:\"") + String{ "JSON.stringify({client_id:\"" }.size());


			if (newString03.find("\",nonce:e.nonce}))))") != String::npos) {
				clientIdNew = newString03.substr(0, newString03.find("\",nonce:e.nonce}))))"));
			}
			if (returnData02.responseCode not_eq 200 && this->configManager->doWePrintHttpsErrorMessages()) {
				cout << DiscordCoreAPI::shiftToBrightRed() << "SoundCloudAPI::searchForSong Error: " << returnData.responseCode << newerString02.c_str() << DiscordCoreAPI::reset()
					 << endl
					 << endl;
			}
		}
		return clientIdNew;
	}

	SoundCloudAPI::SoundCloudAPI(DiscordCoreAPI::ConfigManager* configManagerNew, HttpsClient* httpsClient, const DiscordCoreAPI::Snowflake guildIdNew) {
		this->configManager = configManagerNew;
		this->httpsClient = httpsClient;
		this->guildId = static_cast<DiscordCoreAPI::Snowflake>(guildIdNew);
		if (SoundCloudRequestBuilder::clientId == "") {
			SoundCloudRequestBuilder::clientId = this->collectClientId();
		}
	}

	void SoundCloudAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, Int32 currentReconnectTries) {
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
			SoundCloudAPI::downloadAndStreamAudio(newerSong, stopToken, currentReconnectTries);
		}
	}

	void SoundCloudAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, Int32 currentReconnectTries) {
		try {
			Int32 counter{ 0 };
			BuildAudioDecoderData dataPackage{};
			dataPackage.totalFileSize = static_cast<Uint64>(newSong.contentLength);
			dataPackage.bufferMaxSize = this->maxBufferSize;
			dataPackage.configManager = this->configManager;
			std::unique_ptr<DiscordCoreAPI::AudioEncoder> audioEncoder{ std::make_unique<DiscordCoreAPI::AudioEncoder>() };
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			Bool didWeGetZero{ true };
			std::vector<std::vector<Uint8>> submittedFrames{};
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
				String baseUrl = newSong.finalDownloadUrls[counter].urlPath.substr(0, String{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
				String relativeUrl = newSong.finalDownloadUrls[counter].urlPath.substr(String{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
				dataPackage03.baseUrl = baseUrl;
				dataPackage03.relativePath = relativeUrl;
				dataPackage03.workloadClass = HttpsWorkloadClass::Get;
				auto result = this->httpsClient->submitWorkloadAndGetResult(dataPackage03);
				if (result.responseMessage.size() != 0) {
					didWeGetZero = false;
				}
				Uint64 amountToSubmitRemaining{ result.contentLength };
				Uint64 amountSubmitted{ 0 };
				while (amountToSubmitRemaining > 0) {
					std::this_thread::sleep_for(1ms);
					String newerVector{};
					if (amountToSubmitRemaining >= this->maxBufferSize) {
						for (Uint64 x = 0; x < this->maxBufferSize; ++x) {
							newerVector.push_back(result.responseMessage[amountSubmitted]);
							amountSubmitted++;
							amountToSubmitRemaining--;
						}
					} else {
						Uint64 amountToSubmitRemainingFinal{ amountToSubmitRemaining };
						for (Uint64 x = 0; x < amountToSubmitRemainingFinal; ++x) {
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
				std::vector<DiscordCoreAPI::AudioFrameData> frames{};
				Bool doWeBreak{ false };
				while (!doWeBreak) {
					DiscordCoreAPI::AudioFrameData rawFrame{};
					doWeBreak = !audioDecoder->getFrame(rawFrame);
					if (rawFrame.sampleCount == -5) {
						doWeBreak = true;
						break;
					}
					if (rawFrame.data.size() != 0) {
						submittedFrames.emplace_back(rawFrame.data);
						frames.emplace_back(std::move(rawFrame));
					}
				}
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					return;
				} else {
					for (auto& value: frames) {
						auto encodedFrame = audioEncoder->encodeSingleAudioFrame(value);
						encodedFrame.guildMemberId = static_cast<DiscordCoreAPI::Song>(newSong).addedByUserId;
						DiscordCoreAPI::DiscordCoreClient::getSongAPI(this->guildId)->audioDataBuffer.send(std::move(encodedFrame));
					}
				}
				if (stopToken.stop_requested()) {
					audioDecoder.reset(nullptr);
					return;
				}
				counter++;
				std::this_thread::sleep_for(1ms);
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
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("SoundCloudAPI::downloadAndStreamAudio()");
			}
			currentReconnectTries++;
			DiscordCoreAPI::DiscordCoreClient::getVoiceConnection(this->guildId)->clearAudioData();
			this->weFailedToDownloadOrDecode(newSong, stopToken, currentReconnectTries);
		}
	};

	std::vector<DiscordCoreAPI::Song> SoundCloudAPI::searchForSong(const String& searchQuery) {
		return this->collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song SoundCloudAPI::collectFinalSong(DiscordCoreAPI::Song& newSong) {
		return SoundCloudRequestBuilder::collectFinalSong(newSong);
	}

	String SoundCloudRequestBuilder::clientId{};
};