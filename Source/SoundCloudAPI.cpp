/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
#include <discordcoreapi/Utilities/AudioEncoder.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/Utilities/Demuxers.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreInternal::Media> {
		using ValueType = DiscordCoreInternal::Media;
		constexpr static auto parseValue = object("transcodings", &ValueType::transcodings);
	};

	template<> struct Core<DiscordCoreInternal::SecondDownloadUrl> {
		using ValueType = DiscordCoreInternal::SecondDownloadUrl;
		constexpr static auto parseValue = object("url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreInternal::Transcoding> {
		using ValueType = DiscordCoreInternal::Transcoding;
		constexpr static auto parseValue = object("preset", &ValueType::preset, "url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreInternal::RawSoundCloudSong> {
		using ValueType = DiscordCoreInternal::RawSoundCloudSong;
		constexpr static auto parseValue = object("artwork_url", &ValueType::artworkUrl, "description", &ValueType::description, "duration",
			&ValueType::duration, "media", &ValueType::media, "title", &ValueType::title, "track_authorization", &ValueType::trackAuthorization,
			"permalink_url", &ValueType::viewUrl);
	};

	template<> struct Core<DiscordCoreInternal::SoundCloudSearchResults> {
		using ValueType = DiscordCoreInternal::SoundCloudSearchResults;
		constexpr static auto parseValue = object("collection", &ValueType::collection);
	};
}

namespace DiscordCoreInternal {

	SoundCloudRequestBuilder::SoundCloudRequestBuilder(DiscordCoreAPI::ConfigManager* configManagerNew) noexcept
		: HttpsClientCore{ configManagerNew->getBotToken() } {
	}

	std::string collectSongIdFromSearchQuery(const std::string& string) {
		if (string.find("soundcloud.com") != std::string::npos) {
			std::string returnString{};
			returnString = string.substr(string.find("soundcloud.com/") + std::string{ "soundcloud.com/" }.size());
			return returnString;
		} else {
			return string;
		}
	}

	std::vector<DiscordCoreAPI::Song> SoundCloudRequestBuilder::collectSearchResults(const std::string& songQuery) {
		if (clientId == "") {
			SoundCloudRequestBuilder::collectClientId();
		}
		try {
			HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
			dataPackage.baseUrl = baseUrl02;
			dataPackage.headersToInsert["User-Agent"] =
				"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
			dataPackage.relativePath = "/search?q=";
			dataPackage.relativePath += DiscordCoreAPI::urlEncode(collectSongIdFromSearchQuery(songQuery).c_str());
			dataPackage.relativePath += "&facet=model&client_id=";
			dataPackage.relativePath += SoundCloudRequestBuilder::clientId;
			dataPackage.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage));
			std::vector<DiscordCoreAPI::Song> results{};
			SoundCloudSearchResults resultsNew{};
			parser.parseJson<true, true>(resultsNew, returnData.responseData);
			for (auto& value: resultsNew.collection) {
				DiscordCoreAPI::Song newSong{};
				if (value.title == "") {
					continue;
				}
				bool isItFound{};
				for (auto& valueNew: value.media.transcodings) {
					if (valueNew.preset == "opus_0_0") {
						isItFound = true;
						newSong.firstDownloadUrl = valueNew.url;
						newSong.songId = valueNew.url;
					}
				}
				if (isItFound) {
					std::string newString = value.title;
					if (newString.size() > 0) {
						if (newString.size() > 256) {
							newString = newString.substr(0, 256);
						}
						newSong.songTitle = DiscordCoreAPI::utf8MakeValid(newString);
					}
					newString = value.description;
					if (newString.size() > 0) {
						if (newString.size() > 256) {
							newString = newString.substr(0, 256);
						}
						newSong.description = DiscordCoreAPI::utf8MakeValid(newString);
						newSong.description += "...";
					}
					newString = value.artworkUrl;
					if (newString.size() > 0) {
						newSong.thumbnailUrl = newString;
					}
					newSong.viewUrl = value.viewUrl;
					newSong.duration = DiscordCoreAPI::TimeStamp::convertMsToDurationString(value.duration);
					newSong.firstDownloadUrl +=
						"?client_id=" + SoundCloudRequestBuilder::clientId + "&track_authorization=" + value.trackAuthorization;
					if (newSong.thumbnailUrl.find("-") != std::string::npos) {
						std::string newString = newSong.thumbnailUrl.substr(0, newSong.thumbnailUrl.find_last_of("-") + 1);
						newString += "t500x500.jpg";
						newSong.thumbnailUrl = newString;
						results.emplace_back(newSong);
					}
				}
			}
			return results;
		} catch (const HttpsError& error) {
			Globals::MessagePrinter::printError<Globals::MessageType::Https>(
				"SoundCloudRequestBuilder::collectSearchResults() Error: " + std::string{ error.what() });
		}
		return {};
	}

	DiscordCoreAPI::Song SoundCloudRequestBuilder::constructDownloadInfo(const DiscordCoreAPI::Song& newSong) {
		try {
			HttpsWorkloadData dataPackage01{ HttpsWorkloadType::SoundCloudGetDownloadLinks };
			if (newSong.firstDownloadUrl.find(".com") != std::string::npos) {
				dataPackage01.baseUrl = newSong.firstDownloadUrl.substr(0, newSong.firstDownloadUrl.find(".com") + 4);
				dataPackage01.relativePath = newSong.firstDownloadUrl.substr(newSong.firstDownloadUrl.find(".com") + 4);
			} else {
				return {};
			}
			dataPackage01.workloadClass = HttpsWorkloadClass::Get;
			dataPackage01.headersToInsert["Connection"] = "Keep-Alive";
			dataPackage01.headersToInsert["User-Agent"] =
				"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
			HttpsResponseData results = submitWorkloadAndGetResult(std::move(dataPackage01));
			SecondDownloadUrl downloadUrl{};
			DiscordCoreAPI::Song newerSong{ newSong };
			parser.parseJson<true, true>(downloadUrl, results.responseData);
			newerSong.secondDownloadUrl = downloadUrl.url;
			if (newerSong.secondDownloadUrl.find("/playlist") != std::string::npos) {
				HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetDownloadLinks };
				dataPackage.baseUrl = newerSong.secondDownloadUrl;
				dataPackage.workloadClass = HttpsWorkloadClass::Get;
				HttpsResponseData results = submitWorkloadAndGetResult(std::move(dataPackage));
				std::string newString{ results.responseData };
				newerSong.finalDownloadUrls.clear();
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
					newerSong.finalDownloadUrls.emplace_back(downloadUrl);
				}
				for (size_t x = 0; x < newerSong.finalDownloadUrls.size(); ++x) {
					if (x == newerSong.finalDownloadUrls.size() - 1) {
						newerSong.finalDownloadUrls[x].urlPath =
							newerSong.finalDownloadUrls[x].urlPath.substr(0, newerSong.finalDownloadUrls[x].urlPath.size() - 1);
					}
				}
				for (auto& value: newerSong.finalDownloadUrls) {
					newerSong.contentLength += value.contentSize;
				}
			} else {
				HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetSearchResults };
				dataPackage02.baseUrl = newerSong.secondDownloadUrl;
				dataPackage02.workloadClass = HttpsWorkloadClass::Get;
				dataPackage02.headersToInsert["Connection"] = "Keep-Alive";
				dataPackage02.headersToInsert["User-Agent"] =
					"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
				auto headersNew = submitWorkloadAndGetResult(std::move(dataPackage02));
				int64_t valueBitRate{};
				int64_t valueLength{};
				if (headersNew.responseHeaders.find("x-amz-meta-bitrate") != headersNew.responseHeaders.end()) {
					valueBitRate = stoll(headersNew.responseHeaders.find("x-amz-meta-bitrate")->second);
				}
				if (headersNew.responseHeaders.find("x-amz-meta-duration") != headersNew.responseHeaders.end()) {
					valueLength = stoll(headersNew.responseHeaders.find("x-amz-meta-duration")->second);
				}
				DiscordCoreAPI::DownloadUrl downloadUrl{};
				downloadUrl.contentSize = static_cast<int32_t>(((valueBitRate * valueLength) / 8) - 193);
				downloadUrl.urlPath = newerSong.secondDownloadUrl;
				newerSong.finalDownloadUrls.emplace_back(downloadUrl);
			}
			return newerSong;
		} catch (const HttpsError& error) {
			Globals::MessagePrinter::printError<Globals::MessageType::Https>(
				"SoundCloudRequestBuilder::constructDownloadInfo() Error: " + std::string{ error.what() });
		}
		return {};
	}

	DiscordCoreAPI::Song SoundCloudRequestBuilder::collectFinalSong(const DiscordCoreAPI::Song& newSong) {
		return constructDownloadInfo(newSong);
	}

	std::string SoundCloudRequestBuilder::collectClientId() {
		std::string clientIdNew{};
		try {
			HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetClientId };
			dataPackage02.baseUrl = baseUrl;
			dataPackage02.relativePath = "/search?q=testValue";
			dataPackage02.headersToInsert["User-Agent"] =
				"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
			dataPackage02.workloadClass = HttpsWorkloadClass::Get;
			HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage02));
			std::vector<std::string> assetPaths{};
			std::string newString01 = "crossorigin src=";
			std::string newerString{};
			newerString = returnData.responseData;
			if (newerString.find(newString01) != std::string::npos) {
				std::string newString = newerString.substr(newerString.find(newString01) + newString01.size());
				std::string newString02 = newString.substr(1, newString.find(".js") + 2);
				assetPaths.emplace_back(newString02);
				while (newString.find("crossorigin src=") != std::string::npos) {
					std::string newString03 = newString.substr(1, newString.find(".js") + 2);
					newString = newString.substr(newString.find("crossorigin src=") + newString01.size());
					assetPaths.emplace_back(newString03);
				}
				HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetClientId };
				dataPackage03.baseUrl = assetPaths[5];
				dataPackage03.workloadClass = HttpsWorkloadClass::Get;
				HttpsResponseData returnData02 = submitWorkloadAndGetResult(std::move(dataPackage03));
				std::string newerString02{};
				newerString02.insert(newerString02.begin(), returnData02.responseData.begin(), returnData02.responseData.end());

				std::string newString03 = newerString02.substr(
					newerString02.find("JSON.stringify({client_id:\"") + std::string_view{ "JSON.stringify({client_id:\"" }.size());

				if (newString03.find("\",nonce:e.nonce}))))") != std::string::npos) {
					clientIdNew = newString03.substr(0, newString03.find("\",nonce:e.nonce}))))"));
				}
				if (returnData02.responseCode != 200) {
					Globals::MessagePrinter::printError<Globals::MessageType::Https>(
						"SoundCloudAPI::collectClientID() Error: " + std::to_string(returnData.responseCode) + std::string{ newerString02.c_str() });
				}
			}
		} catch (const HttpsError& error) {
			Globals::MessagePrinter::printError<Globals::MessageType::Https>(
				"SoundCloudRequestBuilder::collectClientId() Error" + std::string{ error.what() });
		}
		return clientIdNew;
	}

	SoundCloudAPI::SoundCloudAPI(DiscordCoreAPI::ConfigManager* configManagerNew, const DiscordCoreAPI::Snowflake guildIdNew)
		: SoundCloudRequestBuilder{ configManagerNew } {
		guildId = static_cast<DiscordCoreAPI::Snowflake>(guildIdNew);
		if (SoundCloudRequestBuilder::clientId == "") {
			SoundCloudRequestBuilder::clientId = collectClientId();
		}
	}

	void SoundCloudAPI::weFailedToDownloadOrDecode(const DiscordCoreAPI::Song& newSong, std::stop_token stopToken, int32_t currentRetries) {
		std::this_thread::sleep_for(1s);
		++currentRetries;
		if (currentRetries <= 10) {
			downloadAndStreamAudio(newSong, stopToken, currentRetries);
		} else {
			DiscordCoreAPI::GuildMemberData guildMember{ DiscordCoreAPI::GuildMembers::getCachedGuildMember(
				{ .guildMemberId = newSong.addedByUserId, .guildId = guildId }) };
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(guildId).skip(newSong, guildMember);
		}
	}

	bool SoundCloudAPI::areWeWorking() noexcept {
		return areWeWorkingBool.load();
	}

	void SoundCloudAPI::downloadAndStreamAudio(const DiscordCoreAPI::Song& newSong, std::stop_token token, int32_t currentReconnectTries) {
		try {
			areWeWorkingBool.store(true);
			int32_t counter{};
			std::vector<HttpsWorkloadData> workloadVector{};
			for (size_t x = 0; x < newSong.finalDownloadUrls.size(); ++x) {
				HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetSearchResults };
				if (counter < newSong.finalDownloadUrls.size()) {
					std::string baseUrl =
						newSong.finalDownloadUrls[x].urlPath.substr(0, std::string_view{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
					std::string relativeUrl =
						newSong.finalDownloadUrls[x].urlPath.substr(std::string_view{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
					dataPackage03.baseUrl = baseUrl;
					dataPackage03.relativePath = relativeUrl;
				}
				dataPackage03.workloadClass = HttpsWorkloadClass::Get;
				workloadVector.emplace_back(std::move(dataPackage03));
			}
			std::vector<std::string> buffer{};
			OggDemuxer demuxer{};
			for (size_t x = 0; x < newSong.finalDownloadUrls.size(); ++x) {
				HttpsResponseData result{ submitWorkloadAndGetResult(std::move(workloadVector[x])) };
				if (result.responseCode != 200) {
					areWeWorkingBool.store(false);
					weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
					return;
				}

				if (result.responseData.size() > 0) {
					buffer.emplace_back(std::move(result.responseData));
					demuxer.writeData({ reinterpret_cast<char*>(buffer.back().data()), buffer.back().size() });
					demuxer.proceedDemuxing();
				}
				if (token.stop_requested()) {
					areWeWorkingBool.store(false);
					return;
				}
				DiscordCoreAPI::AudioFrameData frameData{};
				while (demuxer.collectFrame(frameData)) {
					if (token.stop_requested()) {
						areWeWorkingBool.store(false);
						return;
					}
					if (frameData.currentSize != 0) {
						DiscordCoreAPI::DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
					}
				}
				if (token.stop_requested()) {
					areWeWorkingBool.store(false);
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			areWeWorkingBool.store(false);
			DiscordCoreAPI::DiscordCoreClient::getVoiceConnection(guildId).doWeSkip.store(true);
		} catch (const HttpsError& error) {
			Globals::MessagePrinter::printError<Globals::MessageType::Https>(
				"SoundCloudRequestBuilder::downloadAndStreamAudio() Error: " + std::string{ error.what() });
			areWeWorkingBool.store(false);
			weFailedToDownloadOrDecode(newSong, token, currentReconnectTries);
		}
	};

	std::vector<DiscordCoreAPI::Song> SoundCloudAPI::searchForSong(const std::string& searchQuery) {
		return collectSearchResults(searchQuery);
	}

	DiscordCoreAPI::Song SoundCloudAPI::collectFinalSong(const DiscordCoreAPI::Song& newSong) {
		return SoundCloudRequestBuilder::collectFinalSong(newSong);
	}

	std::string SoundCloudRequestBuilder::clientId{};
};
