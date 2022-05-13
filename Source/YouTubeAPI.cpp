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
/// Chris M.
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

	std::vector<DiscordCoreAPI::Song> YouTubeRequestBuilder::collectSearchResults(std::string searchQuery) {
		DiscordCoreInternal::HttpWorkloadData dataPackage{};
		dataPackage.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage.relativePath = "/results?search_query=" + DiscordCoreAPI::urlEncode(searchQuery.c_str());
		dataPackage.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage);
		std::vector<DiscordCoreInternal::HttpData> returnData = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
		if (returnData[0].responseCode != 200) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::collectSearchResults() Error: " << returnData[0].responseCode
					  << returnData[0].responseMessage.c_str() << DiscordCoreAPI::reset() << std::endl;
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

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructDownloadInfo(DiscordCoreAPI::GuildMemberData guildMember, DiscordCoreAPI::Song newSong) {
		if (newSong.firstDownloadUrl != "") {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
		}
		std::vector<DiscordCoreInternal::HttpWorkloadData> dataPackageWorkload{};
		DiscordCoreInternal::HttpWorkloadData dataPackage02{};
		dataPackage02.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage02.relativePath = "/watch?v=" + newSong.songId + "&hl=en";
		dataPackage02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		dataPackageWorkload.push_back(dataPackage02);
		std::vector<DiscordCoreInternal::HttpData> responseData = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, dataPackageWorkload);

		std::string resultStringHTMLBody{};
		if (responseData[0].responseCode != 204 && responseData[0].responseCode != 201 && responseData[0].responseCode != 200) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 01 Error: " << responseData[0].responseCode << ", "
					  << responseData[0].responseMessage << std::endl
					  << DiscordCoreAPI::reset() << std::endl;
		}
		resultStringHTMLBody.insert(resultStringHTMLBody.begin(), responseData[0].responseMessage.begin(), responseData[0].responseMessage.end());
		std::string resultStringStringHTMLBody = resultStringHTMLBody;
		std::string newString00 = "/player_ias.vflset/en_US/base.js";
		newSong.html5Player = YouTubeRequestBuilder::baseUrl +
			resultStringStringHTMLBody.substr(resultStringStringHTMLBody.find("/s/player/"), resultStringStringHTMLBody.find(newString00) + newString00.length());
		newSong.html5Player = newSong.html5Player.substr(0, 73);
		newSong.playerResponse = between(resultStringHTMLBody, "ytInitialPlayerResponse = ", "</script>");
		newSong.playerResponse = newSong.playerResponse.substr(0, newSong.playerResponse.length() - 1);
		newSong.firstDownloadUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
		newSong.type = DiscordCoreAPI::SongType::YouTube;
		nlohmann::json jsonObject;
		if (newSong.playerResponse != "") {
			jsonObject = nlohmann::json::parse(newSong.playerResponse);
		}
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
		std::vector<DiscordCoreInternal::HttpWorkloadData> dataPackageWorkload02{};
		DiscordCoreInternal::HttpWorkloadData dataPackage03{};
		dataPackage03.baseUrl = newSong.html5Player;
		dataPackage03.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		dataPackageWorkload02.push_back(dataPackage03);
		auto responseMessage02 = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, dataPackageWorkload02);
		std::string responseToPlayerGet02{};
		if (responseMessage02[0].responseCode != 204 && responseMessage02[0].responseCode != 201 && responseMessage02[0].responseCode != 200) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "YouTubeRequestBuilder::constructDownloadInfo() 02 Error: " << responseMessage02[0].responseCode << ", "
					  << responseMessage02[0].responseMessage << std::endl
					  << DiscordCoreAPI::reset() << std::endl;
		}
		responseToPlayerGet02.insert(responseToPlayerGet02.begin(), responseMessage02[0].responseMessage.begin(), responseMessage02[0].responseMessage.end());
		newSong.html5PlayerFile = responseToPlayerGet02;
		newSong.format = decipherFormat(newSong.format, newSong.html5PlayerFile);
		DiscordCoreAPI::DownloadUrl downloadUrl{ .contentSize = newSong.contentLength, .urlPath = newSong.format.downloadUrl };
		newSong.viewUrl = newSong.firstDownloadUrl;
		newSong.addedByUserName = guildMember.user.userName;
		newSong.contentLength = static_cast<int32_t>(newSong.format.contentLength);
		std::vector<DiscordCoreAPI::DownloadUrl> theUrls{};
		newSong.finalDownloadUrls = theUrls;
		newSong.finalDownloadUrls.push_back(downloadUrl);
		newSong.addedByUserId = guildMember.user.id;
		newSong.type = DiscordCoreAPI::SongType::YouTube;
		return newSong;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::constructFinalDownloadUrl(DiscordCoreAPI::Song newSong) {
		std::string downloadBaseUrl{};
		if (newSong.finalDownloadUrls[0].urlPath.find("https://") != std::string::npos && newSong.finalDownloadUrls[0].urlPath.find("/videoplayback?") != std::string::npos) {
			std::string newString00 = "https://";
			downloadBaseUrl = newSong.finalDownloadUrls[0].urlPath.substr(
				newSong.finalDownloadUrls[0].urlPath.find("https://") + newString00.length(), newSong.finalDownloadUrls[0].urlPath.find("/videoplayback?") - newString00.length());
		}
		std::string request = "GET " + newSong.finalDownloadUrls[0].urlPath +
			" HTTP/1.1\n\rUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36\n\r" +
			"Host: " + newSong.finalDownloadUrls[0].urlPath.substr(0, newSong.finalDownloadUrls[0].urlPath.find(".com") + 4) + "\n\r\n\r";
		DiscordCoreAPI::DownloadUrl downloadUrl01{};
		downloadUrl01.contentSize = 0;
		downloadUrl01.urlPath = downloadBaseUrl;
		DiscordCoreAPI::DownloadUrl downloadUrl02{};
		downloadUrl02.contentSize = 0;
		downloadUrl02.urlPath = request;
		newSong.finalDownloadUrls[0] = downloadUrl01;
		newSong.finalDownloadUrls.push_back(downloadUrl02);
		return newSong;
	}

	std::vector<char> YouTubeRequestBuilder::sliceVector(std::vector<char> vectorToSlice, int32_t firstElement, int32_t lastElement) {
		std::vector<char> newVector{};
		if (lastElement == 0) {
			lastElement = static_cast<int32_t>(vectorToSlice.size());
		}
		if (lastElement > vectorToSlice.size()) {
			throw std::runtime_error("Sorry, but you've claimed a size for the std::vector that is larger than the input std::vector!");
		}
		for (int32_t x = firstElement; x < lastElement; x += 1) {
			newVector.push_back(vectorToSlice[x]);
		}
		return newVector;
	}

	std::vector<char> YouTubeRequestBuilder::swapHeadAndPosition(std::vector<char> inputVector, int32_t position) {
		char first = inputVector[0];
		inputVector[0] = inputVector[position % inputVector.size()];
		inputVector[position] = first;
		return inputVector;
	}

	DiscordCoreAPI::YouTubeFormat YouTubeRequestBuilder::decipherFormat(DiscordCoreAPI::YouTubeFormat format, std::string html5playerFile) {
		format.signature = DiscordCoreAPI::urlDecode(format.signature);
		DiscordCoreAPI::YouTubeFormat decipheredFormat = format;
		std::vector<std::string> tokens = YouTubeRequestBuilder::getTokens(html5playerFile);
		std::string newString00 = "s=";
		if (decipheredFormat.signature.length() >= newString00.length()) {
			decipheredFormat.signature = decipheredFormat.signature.substr(newString00.length());
			decipheredFormat.signature = YouTubeRequestBuilder::decipher(tokens, decipheredFormat.signature);
		}
		decipheredFormat.downloadUrl = YouTubeRequestBuilder::setDownloadUrl(decipheredFormat);
		return decipheredFormat;
	}

	std::string YouTubeRequestBuilder::decipher(std::vector<std::string> tokens, std::string cipherSignature) {
		std::vector<char> signatureNew = YouTubeRequestBuilder::splitString(cipherSignature);
		for (int32_t x = 0, len = static_cast<int32_t>(tokens.size()); x < len; x += 1) {
			std::string token = tokens[x];
			int32_t position{};
			switch (token[0]) {
				case 'r':
					signatureNew = YouTubeRequestBuilder::reverseString(signatureNew);
					break;
				case 'w':
					position = stol(token.substr(1));
					signatureNew = YouTubeRequestBuilder::swapHeadAndPosition(signatureNew, position);
					break;
				case 's':
					position = stol(token.substr(1));
					signatureNew = YouTubeRequestBuilder::sliceVector(signatureNew, position);
					break;
				case 'p':
					position = stol(token.substr(1));
					signatureNew = YouTubeRequestBuilder::sliceVector(signatureNew, position);
					break;
			}
		}
		std::string signatureNewString = YouTubeRequestBuilder::joinString(signatureNew);
		return signatureNewString;
	}

	std::vector<std::string> YouTubeRequestBuilder::extractActions(std::string html5PlayerPageBody) {
		std::string jsVarStr = "[a-zA-Z_\\$][a-zA-Z_0-9]*";
		std::string jsSingleQuoteStr = "'[^'\\\\]*(:?\\\\[\\s\\S][^'\\\\]*)*'";
		std::string jsDoubleQuoteStr = "\"[^\"\\\\]*(:?\\\\[\\s\\S][^\"\\\\]*)*\"";
		std::string jsQuoteStr = "(?:" + jsSingleQuoteStr + "|" + jsDoubleQuoteStr + ")";
		std::string jsKeyStr = "(?:" + jsVarStr + "|" + jsQuoteStr + ")";
		std::string jsPropStr = "(?:\\." + jsVarStr + "|" + "\\[" + jsQuoteStr + "\\])";
		std::string jsEmptyStr = "(?:''|\"\")";
		std::string reverseStr = ":function\\(a\\)\\{(?:return)?a\\.reverse\\(\\)\\}";
		std::string sliceStr = ":function\\(a,b\\)\\{return a\\.slice\\(b\\)\\}";
		std::string spliceStr = ":function\\(a,b\\)\\{a\\.splice\\(0,b\\)\\}";
		std::string swapStr = ":function\\(a,b\\)\\{var c=a\\[0\\];a\\[0\\]=a\\[b(?:%a\\.length)?\\];a\\[b(?:%a\\.length)?\\]=c(?:;return a)?\\}";
		std::regex reverseRegexp(jsVarStr + reverseStr);
		std::regex sliceRegexp(jsVarStr + sliceStr);
		std::regex spliceRegexp(jsVarStr + spliceStr);
		std::regex swapRegexp(jsVarStr + swapStr);
		std::string newString = html5PlayerPageBody;
		std::vector<std::string> tokenActions;
		std::regex actionsObjRegexp(
			"var (" + jsVarStr + ")=\\{((?:(?:" + jsKeyStr + reverseStr + "|" + jsKeyStr + sliceStr + "|" + jsKeyStr + spliceStr + "|" + jsKeyStr + swapStr + "),?\\r?\\n?)+)\\};",
			std::regex_constants::ECMAScript);
		std::smatch actionsObjectRegexMatch;
		regex_search(newString, actionsObjectRegexMatch, actionsObjRegexp, std::regex_constants::match_any | std::regex_constants::match_not_null);
		for (auto const& value: actionsObjectRegexMatch) {
			std::string actionsObjectResult = value.str();
			if (actionsObjectResult != "") {
				tokenActions.push_back(actionsObjectResult);
			}
		}
		std::regex actionsFuncRegexp("function(?:[a-zA-Z_\\$][a-zA-Z_0-9]*)?\\(a\\)\\{a=a\\.split\\((?:''|\"\")\\);\\s*((?:(?:a=)?[a-zA-Z_\\$][a-zA-Z_0-9]*(?:"
									 "\\.[a-zA-Z_\\$][a-zA-Z_0-9]*|\\[(?:'[^'\\\\]*(:?\\\\[\\s\\S]["
									 "^'\\\\]*)*'|\"[^\"\\\\]*(:?\\\\[\\s\\S][^\"\\\\]*)*\")\\])\\(a,\\d+\\);)+)return a\\.join\\((?:''|\"\")\\)\\}",
			std::regex_constants::ECMAScript);
		std::smatch actionFuncRegexMatch;
		regex_search(newString, actionFuncRegexMatch, actionsFuncRegexp, std::regex_constants::match_any | std::regex_constants::match_not_null);
		for (auto const& value: actionFuncRegexMatch) {
			std::string actionFuncResult = value.str();
			if (actionFuncResult != "") {
				tokenActions.push_back(actionFuncResult);
			}
		}
		std::string object = tokenActions[1];
		std::string objectBody = tokenActions[2];
		std::string functionBody = tokenActions[3];
		std::smatch matchResultsNew01;
		std::string reverseKey;
		regex_search(objectBody, matchResultsNew01, reverseRegexp, std::regex_constants::match_not_null);
		if (!matchResultsNew01.empty() && matchResultsNew01.size() > 0) {
			reverseKey = regex_replace(matchResultsNew01[0].str().substr(0, matchResultsNew01[0].str().find(":")), std::regex("/g,'\\$"), "");
			reverseKey = regex_replace(reverseKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::smatch matchResultsNew02;
		std::string sliceKey;
		regex_search(objectBody, matchResultsNew02, sliceRegexp, std::regex_constants::match_not_null);
		if (!matchResultsNew02.empty() && matchResultsNew02.size() > 0) {
			sliceKey = regex_replace(matchResultsNew02[0].str().substr(0, matchResultsNew02[0].str().find(":")), std::regex("/g,'\\$"), "");
			sliceKey = regex_replace(sliceKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::smatch matchResultsNew03;
		std::string spliceKey;
		regex_search(objectBody, matchResultsNew03, spliceRegexp, std::regex_constants::match_not_null);
		if (!matchResultsNew03.empty() && matchResultsNew03.size() > 0) {
			spliceKey = regex_replace(matchResultsNew03[0].str().substr(0, matchResultsNew03[0].str().find(":")), std::regex("/g,'\\$"), "");
			spliceKey = regex_replace(spliceKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::smatch matchResultsNew04;
		std::string swapKey;
		regex_search(objectBody, matchResultsNew04, swapRegexp, std::regex_constants::match_not_null);
		if (!matchResultsNew04.empty() && matchResultsNew04.size() > 0) {
			swapKey = regex_replace(matchResultsNew04[0].str().substr(0, matchResultsNew04[0].str().find(":")), std::regex("/g,'\\$"), "");
			swapKey = regex_replace(swapKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::vector<std::string> keys{};
		std::string keysNew{};
		keys.push_back(reverseKey);
		keys.push_back(sliceKey);
		keys.push_back(spliceKey);
		keys.push_back(swapKey);
		for (uint32_t x = 0; x < keys.size(); x += 1) {
			if (x == 0 && keys[x] != "") {
				keysNew += keys[x];
			} else if (x > 0 && keys[x] != "") {
				keysNew += "|" + keys[x];
			} else if (keys[x] == "") {
				continue;
			}
		}
		std::regex tokenizeRegexp("(" + object + "\\.(" + keysNew + ")\\(a,(\\d{1,3})\\))");
		std::smatch tokenMatchResults;
		std::vector<std::string> tokens;
		std::string::const_iterator text_iter = functionBody.cbegin();
		while (regex_search(text_iter, functionBody.cend(), tokenMatchResults, tokenizeRegexp, std::regex_constants::match_not_null | std::regex_constants::match_any)) {
			std::string newString02 = std::string(tokenMatchResults[0].first, tokenMatchResults[0].second);
			if (newString02.find(reverseKey) != std::string::npos) {
				tokens.push_back("r");
			} else if (newString02.find(spliceKey) != std::string::npos) {
				tokens.push_back("p" + tokenMatchResults[3].str());
			} else if (newString02.find(swapKey) != std::string::npos) {
				tokens.push_back("w" + tokenMatchResults[3].str());
			} else if (newString02.find(sliceKey) != std::string::npos) {
				tokens.push_back("s" + tokenMatchResults[3].str());
			}
			text_iter = tokenMatchResults[0].second;
		};
		return tokens;
	}

	std::vector<char> YouTubeRequestBuilder::reverseString(std::vector<char> stringToReverse) {
		uint64_t n = stringToReverse.size();
		for (int32_t x = 0; x < n / 2; x++) {
			std::swap(stringToReverse[x], stringToReverse[n - x - 1]);
		}
		return stringToReverse;
	}

	std::string YouTubeRequestBuilder::between(std::string body, std::string left, std::string right) {
		int64_t positionStart = body.find(left) + left.length();
		int64_t positionEnd = body.find(right, positionStart);
		std::string newString = body.substr(positionStart, positionEnd - positionStart);
		return newString;
	}

	std::vector<std::string> YouTubeRequestBuilder::getTokens(std::string html5PlayerFile) {
		std::vector<std::string> tokens = YouTubeRequestBuilder::extractActions(html5PlayerFile);
		if (tokens.size() == 0) {
			throw std::runtime_error("Could not extract signature deciphering actions");
		}
		return tokens;
	}

	std::vector<char> YouTubeRequestBuilder::splitString(std::string stringToSplit) {
		std::vector<char> charVector{};
		for (auto value: stringToSplit) {
			charVector.push_back(value);
		};
		return charVector;
	}

	std::string YouTubeRequestBuilder::joinString(std::vector<char> stringToJoin) {
		std::string newString;
		for (int32_t x = 0; x < stringToJoin.size(); x += 1) {
			newString += stringToJoin[x];
		}
		return newString;
	}

	std::string YouTubeRequestBuilder::setDownloadUrl(DiscordCoreAPI::YouTubeFormat format) {
		std::string downloadUrl = format.downloadUrl;
		downloadUrl = DiscordCoreAPI::urlDecode(downloadUrl) + "&ratebypass=yes";
		if (format.signature != "") {
			downloadUrl += "&sig=" + format.signature;
		}
		return downloadUrl;
	}

	DiscordCoreAPI::Song YouTubeRequestBuilder::collectFinalSong(DiscordCoreAPI::GuildMemberData addedByGuildMember, DiscordCoreAPI::Song newSong) {
		newSong.firstDownloadUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
		auto newerSong = YouTubeRequestBuilder::constructDownloadInfo(addedByGuildMember, newSong);
		newerSong = YouTubeRequestBuilder::constructFinalDownloadUrl(newerSong);
		newerSong.addedByUserId = addedByGuildMember.user.id;
		newerSong.addedByUserName = addedByGuildMember.user.userName;
		return newerSong;
	}

	YouTubeAPI::YouTubeAPI(std::string guildIdNew, DiscordCoreInternal::HttpClient* httpClient) : requestBuilder(httpClient) {
		this->doWePrint = httpClient->getDoWePrintFFMPEG();
		this->guildId = guildIdNew;
	}

	DiscordCoreAPI::Song YouTubeAPI::collectFinalSong(DiscordCoreAPI::GuildMemberData addedByGuildMember, DiscordCoreAPI::Song newSong) {
		return this->requestBuilder.collectFinalSong(addedByGuildMember, newSong);
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

	void YouTubeAPI::weFailedToDownloadOrDecode(DiscordCoreAPI::Song newSong, YouTubeAPI* youtubeAPI, std::stop_token theToken, int32_t currentRecursionDepth) {
		currentRecursionDepth += 1;
		DiscordCoreAPI::GuildMember guildMember =
			DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newSong.addedByUserId, .guildId = youtubeAPI->guildId }).get();
		if (currentRecursionDepth > 9) {
			DiscordCoreAPI::AudioFrameData frameData{};
			while (DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->audioBuffer.tryReceive(frameData)) {
			};
			DiscordCoreAPI::SongCompletionEventData eventData{};
			auto resultValue = DiscordCoreAPI::getSongAPIMap()[youtubeAPI->guildId].get();
			if (resultValue) {
				eventData.previousSong = resultValue->getCurrentSong(youtubeAPI->guildId);
			}
			eventData.wasItAFail = true;
			eventData.guildMember = guildMember;
			eventData.guild = DiscordCoreAPI::Guilds::getGuildAsync({ .guildId = youtubeAPI->guildId }).get();
			DiscordCoreAPI::getSongAPIMap()[youtubeAPI->guildId]->onSongCompletionEvent(eventData);
			return;
		} else {
			auto newerSong = youtubeAPI->requestBuilder.collectFinalSong(guildMember, newSong);
			YouTubeAPI::downloadAndStreamAudio(newerSong, youtubeAPI, theToken, currentRecursionDepth);
		}
	}

	void YouTubeAPI::downloadAndStreamAudio(DiscordCoreAPI::Song newSong, YouTubeAPI* youtubeAPI, std::stop_token theToken, int32_t currentRecursionDepth) {
		try {
			DiscordCoreInternal::WebSocketSSLClient streamSocket{ newSong.finalDownloadUrls[0].urlPath, "443", this->maxBufferSize };
			bool areWeDoneHeaders{ false };
			bool haveWeFailed{ false };
			int64_t remainingDownloadContentLength{ newSong.contentLength };
			int64_t contentLengthCurrent{ youtubeAPI->maxBufferSize };
			int64_t bytesReadLastIteration{ 1 };
			int64_t bytesReadTotal01{ 0 };
			int32_t sameCounter{ 0 };
			int32_t counter{ 0 };
			BuildAudioDecoderData dataPackage{};
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength);
			dataPackage.bufferMaxSize = youtubeAPI->maxBufferSize;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage, youtubeAPI->doWePrint);
			AudioEncoder audioEncoder = AudioEncoder();
			streamSocket.writeData(newSong.finalDownloadUrls[1].urlPath);
			if (!streamSocket.processIO(600000)) {
				haveWeFailed = true;
				goto breakOutPlayMore;
			};
		breakOutPlayMore:
			if (haveWeFailed && !DiscordCoreAPI::getVoiceConnectionMap()[youtubeAPI->guildId]->areWeCurrentlyPlaying()) {
				audioDecoder.reset(nullptr);
				YouTubeAPI::weFailedToDownloadOrDecode(newSong, youtubeAPI, theToken, currentRecursionDepth);
				return;
			}
		breakOut:
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
				return;
			}
			while (newSong.contentLength > bytesReadTotal01) {
				if (bytesReadLastIteration == bytesReadTotal01) {
					sameCounter += 1;
				} else {
					sameCounter = 0;
				}
				if (sameCounter > 9 && counter >= 20) {
					break;
				} else if (sameCounter > 9) {
					haveWeFailed = true;
					goto breakOutPlayMore;
				}
				bytesReadLastIteration = bytesReadTotal01;
				if (theToken.stop_requested()) {
					goto breakOut;
				}
				if (audioDecoder->haveWeFailed()) {
					haveWeFailed = true;
					goto breakOutPlayMore;
				}
				if (theToken.stop_requested()) {
					goto breakOut;
				} else {
					if (!areWeDoneHeaders) {
						if (!theToken.stop_requested()) {
							bytesReadTotal01 = streamSocket.getBytesRead();
						}
						if (theToken.stop_requested()) {
							goto breakOut;
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal01;
						if (!streamSocket.processIO(1000000)) {
							haveWeFailed = true;
							goto breakOutPlayMore;
						};
						std::string newData = streamSocket.getInputBuffer();
						streamSocket.getInputBuffer().clear();
						if (!theToken.stop_requested()) {
							bytesReadTotal01 = streamSocket.getBytesRead();
						}
						if (theToken.stop_requested()) {
							goto breakOut;
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal01;
						areWeDoneHeaders = true;
					}
					if (theToken.stop_requested()) {
						goto breakOut;
					}
					if (counter == 0) {
						if (!streamSocket.processIO(1000000)) {
							haveWeFailed = true;
							goto breakOutPlayMore;
						};
						std::string streamBuffer = streamSocket.getInputBuffer();
						streamSocket.getInputBuffer().clear();
						audioDecoder->submitDataForDecoding(streamBuffer);
						audioDecoder->startMe();
					}
					if (counter > 0) {
						if (contentLengthCurrent > 0) {
							if (theToken.stop_requested()) {
								goto breakOut;
							}
							bytesReadTotal01 = streamSocket.getBytesRead();
							remainingDownloadContentLength = newSong.contentLength - bytesReadTotal01;
							if (!streamSocket.processIO(600000)) {
								haveWeFailed = true;
								goto breakOutPlayMore;
							};
							std::string newVector = streamSocket.getInputBuffer();
							streamSocket.getInputBuffer().clear();
							if (newVector.size() == 0) {
								counter += 1;
								continue;
							}
							audioDecoder->submitDataForDecoding(newVector);
						}
						if (theToken.stop_requested()) {
							goto breakOut;
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
							goto breakOut;
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
			return;
		} catch (...) {
			DiscordCoreAPI::reportException("YouTubeAPI::downloadAndStreamAudio()");
		}
	};

	std::vector<DiscordCoreAPI::Song> YouTubeAPI::searchForSong(std::string searchQuery) {
		return this->requestBuilder.collectSearchResults(searchQuery);
	}

};