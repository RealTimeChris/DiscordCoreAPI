// YouTubeAPI.cpp - Soure file for the YouTube API related stuff.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "YouTubeAPI.hpp"
#include "Http.hpp"
#include "DiscordCoreClient.hpp"
#include "SSLClients.hpp"
#include "VoiceConnection.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreAPI {

	YouTubeRequestBuilder::YouTubeRequestBuilder(DiscordCoreInternal::HttpClient* theClient) {
		YouTubeRequestBuilder::httpClient = theClient;
	}

	std::vector<YouTubeSong> YouTubeRequestBuilder::collectSearchResults(std::string searchQuery) {
		DiscordCoreInternal::HttpWorkloadData dataPackage{};
		dataPackage.baseUrl = YouTubeRequestBuilder::baseUrl;
		dataPackage.relativePath = "/results?search_query=" + urlEncode(searchQuery.c_str());
		dataPackage.workloadClass= DiscordCoreInternal::HttpWorkloadClass::Get;
		std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage);
		std::vector<DiscordCoreInternal::HttpData> returnData = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
		if (returnData[0].responseCode != 200) {
			std::cout << "YouTubeSong::searchForSong Error: " << returnData[0].responseCode << returnData[0].responseMessage.c_str() << std::endl;
		}
		nlohmann::json partialSearchResultsJson{};
		if (returnData[0].responseMessage.find("var ytInitialData = ") != std::string::npos) {
			std::string newString00 = "var ytInitialData = ";
			std::string newString = returnData[0].responseMessage.substr(returnData[0].responseMessage.find("var ytInitialData = ") + newString00.length());
			std::string stringSequence = ";</script><script nonce=";
			newString = newString.substr(0, newString.find(stringSequence));
			partialSearchResultsJson = nlohmann::json::parse(newString);
		}
		std::vector<YouTubeSong> searchResults{};
		std::vector< DiscordCoreInternal::HttpData> responseData{};
		if (partialSearchResultsJson.contains("contents") && !partialSearchResultsJson.at("contents").is_null()) {
			std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector{};
			for (auto& value : partialSearchResultsJson.at("contents").at("twoColumnSearchResultsRenderer").at("primaryContents").at("sectionListRenderer").at("contents").at(0).at("itemSectionRenderer").at("contents")) {
				YouTubeSong searchResult{};
				if (value.contains("videoRenderer") && !value.at("videoRenderer").is_null()) {
					DiscordCoreInternal::DataParser::parseObject(value.at("videoRenderer"), &searchResult);
					searchResult.type = SongType::YouTube;
					searchResult.viewUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + searchResult.songId + "&hl=en";
					searchResults.push_back(searchResult);
					DiscordCoreInternal::HttpWorkloadData dataPackage{};
					dataPackage.baseUrl = YouTubeRequestBuilder::baseUrl;
					dataPackage.relativePath = "/watch?v=" + searchResult.songId + "&hl=en";
					dataPackage.workloadClass= DiscordCoreInternal::HttpWorkloadClass::Get;
					workloadVector.push_back(dataPackage);
				}
				
			}
			responseData = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector);
		}
		for (uint32_t x = 0; x < responseData.size(); x += 1) {
			std::string resultStringHTMLBody{};
			if (responseData[x].responseCode != 204 && responseData[x].responseCode != 201 && responseData[x].responseCode != 200) {
				std::cout << "YouTubeSong::collectDownloadInfo() 01 Error: " << responseData[x].responseCode << ", " << responseData[x].responseMessage << std::endl << std::endl;
			}
			resultStringHTMLBody.insert(resultStringHTMLBody.begin(), responseData[x].responseMessage.begin(), responseData[x].responseMessage.end());
			std::string resultStringStringHTMLBody = resultStringHTMLBody;
			std::string newString00 = "/player_ias.vflset/en_US/base.js";
			searchResults[x].html5Player = YouTubeRequestBuilder::baseUrl + resultStringStringHTMLBody.substr(resultStringStringHTMLBody.find("/s/player/"), resultStringStringHTMLBody.find(newString00) + newString00.length());
			searchResults[x].html5Player = searchResults[x].html5Player.substr(0, 73);
			searchResults[x].playerResponse = between(resultStringHTMLBody, "ytInitialPlayerResponse = ", "</script>");
			searchResults[x].playerResponse = searchResults[x].playerResponse.substr(0, searchResults[x].playerResponse.length() - 1);
			searchResults[x].firstDownloadUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + searchResults[x].songId + "&hl=en";
			searchResults[x].type = SongType::YouTube;
			nlohmann::json jsonObject;
			if (searchResults[x].playerResponse != "") {
				jsonObject = nlohmann::json::parse(searchResults[x].playerResponse);
			}
			std::vector<YouTubeFormat> theVector{};
			DiscordCoreInternal::DataParser::parseObject(jsonObject, &theVector);
			YouTubeFormat format{};
			bool isOpusFound{ false };
			for (auto& value : theVector) {
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
				searchResults[x].format = format;
			}
		}
		
		return searchResults;
	}

	YouTubeSong YouTubeRequestBuilder::constructDownloadInfo(GuildMemberData guildMember, YouTubeSong newSong) {
		DiscordCoreInternal::HttpWorkloadData dataPackage{};
		dataPackage.baseUrl = newSong.firstDownloadUrl;
		dataPackage.workloadClass= DiscordCoreInternal::HttpWorkloadClass::Get;
		std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector01{};
		workloadVector01.push_back(dataPackage);
		auto resultHTMLBody = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector01);
		std::string resultStringHTMLBody{};
		if (resultHTMLBody[0].responseCode != 204 && resultHTMLBody[0].responseCode != 201 && resultHTMLBody[0].responseCode != 200) {
			std::cout << "YouTubeSong::collectDownloadInfo() 01 Error: " << resultHTMLBody[0].responseCode << ", " << resultHTMLBody[0].responseMessage << std::endl << std::endl;
		}

		resultStringHTMLBody.insert(resultStringHTMLBody.begin(), resultHTMLBody[0].responseMessage.begin(), resultHTMLBody[0].responseMessage.end());
		std::string resultStringStringHTMLBody = resultStringHTMLBody;
		std::string newString00 = "/player_ias.vflset/en_US/base.js";
		newSong.html5Player = YouTubeRequestBuilder::baseUrl + resultStringStringHTMLBody.substr(resultStringStringHTMLBody.find("/s/player/"), resultStringStringHTMLBody.find(newString00) + newString00.length());
		newSong.html5Player = newSong.html5Player.substr(0, 73);
		newSong.playerResponse = between(resultStringHTMLBody, "ytInitialPlayerResponse = ", "</script>");
		newSong.playerResponse = newSong.playerResponse.substr(0, newSong.playerResponse.length() - 1);
		nlohmann::json jsonObject;
		if (newSong.playerResponse != "") {
			jsonObject = nlohmann::json::parse(newSong.playerResponse);
		}
		std::vector<YouTubeFormat> theVector{};
		DiscordCoreInternal::DataParser::parseObject(jsonObject, &theVector);
		YouTubeFormat format{};
		bool isOpusFound{ false };
		for (auto& value : theVector) {
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
		DiscordCoreInternal::HttpWorkloadData dataPackage02{};
		dataPackage02.baseUrl = newSong.html5Player;
		dataPackage02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		std::vector<DiscordCoreInternal::HttpWorkloadData> workloadVector{};
		workloadVector.push_back(dataPackage02);
		auto responseMessage02 = DiscordCoreInternal::submitWorkloadAndGetResult(*this->httpClient, workloadVector);
		std::string responseToPlayerGet02{};
		if (responseMessage02[0].responseCode != 204 && responseMessage02[0].responseCode != 201 && responseMessage02[0].responseCode != 200) {
			std::cout << "YouTubeSong::collectDownloadInfo() 02 Error: " << responseMessage02[0].responseCode << ", " << responseMessage02[0].responseMessage << std::endl << std::endl;
		}
		responseToPlayerGet02.insert(responseToPlayerGet02.begin(), responseMessage02[0].responseMessage.begin(), responseMessage02[0].responseMessage.end());
		newSong.html5PlayerFile = responseToPlayerGet02;
		newSong.format = decipherFormat(newSong.format, newSong.html5PlayerFile);
		DownloadUrl downloadUrl{ .contentSize = newSong.contentLength,.urlPath = newSong.format.downloadUrl };
		newSong.viewUrl = newSong.firstDownloadUrl;
		newSong.addedByUserName = guildMember.user.userName;
		newSong.contentLength = static_cast<int32_t>(newSong.format.contentLength);
		newSong.finalDownloadUrls.push_back(downloadUrl);
		newSong.addedByUserId = guildMember.user.id;
		newSong.type = SongType::YouTube;
		return newSong;
	}

	YouTubeSong YouTubeRequestBuilder::constructFinalDownloadUrl(Song newSong) {
		std::string downloadBaseUrl{};
		if (newSong.finalDownloadUrls.at(0).urlPath.find("https://") != std::string::npos && newSong.finalDownloadUrls.at(0).urlPath.find("/videoplayback?") != std::string::npos) {
			std::string newString00 = "https://";
			downloadBaseUrl = newSong.finalDownloadUrls.at(0).urlPath.substr(newSong.finalDownloadUrls.at(0).urlPath.find("https://") + newString00.length(), newSong.finalDownloadUrls.at(0).urlPath.find("/videoplayback?") - newString00.length());
		}
		std::string request = "GET " + newSong.finalDownloadUrls.at(0).urlPath + " HTTP/1.1\n\ruser-agent: Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.54 Mobile Safari/537.36\n\r" +
			"Host: " + newSong.finalDownloadUrls.at(0).urlPath.substr(0, newSong.finalDownloadUrls.at(0).urlPath.find(".com") + 4) + "\n\r\n\r";
		DownloadUrl downloadUrl01{};
		downloadUrl01.contentSize = 0;
		downloadUrl01.urlPath = downloadBaseUrl;
		DownloadUrl downloadUrl02{};
		downloadUrl02.contentSize = 0;
		downloadUrl02.urlPath = request;
		newSong.finalDownloadUrls[0] = downloadUrl01;
		newSong.finalDownloadUrls.push_back(downloadUrl02);
		return newSong;
	}

	std::vector<char> YouTubeRequestBuilder::sliceVector(std::vector<char> vectorToSlice, int32_t firstElement, int32_t lastElement) {
		std::vector<char> newVector;
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

	YouTubeFormat YouTubeRequestBuilder::decipherFormat(YouTubeFormat format, std::string html5playerFile) {
		format.signature = urlDecode(format.signature);
		YouTubeFormat decipheredFormat = format;
		std::vector<std::string> tokens = YouTubeRequestBuilder::getTokens(html5playerFile);
		std::string newString00 = "s=";
		if (decipheredFormat.signature.length() >= newString00.length()) {
			decipheredFormat.signature = decipheredFormat.signature.substr(newString00.length());
			decipheredFormat.signature = YouTubeRequestBuilder::decipher(tokens, decipheredFormat.signature);
		}
		decipheredFormat.downloadUrl = YouTubeRequestBuilder::setDownloadUrl(decipheredFormat);
		return decipheredFormat;
	};

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
		std::string jsVarStr{ "[a-zA-Z_\\$][a-zA-Z_0-9]*" };
		std::string jsSingleQuoteStr{ "'[^'\\]*(:?\\[\\S\\S][^'\\]*)*'" };
		std::string jsDoubleQuoteStr{ "\"[^\"\\]*(:?\\[\\S\\S][^\"\\]*)*\"" };
		std::string jsQuoteStr{ "(?:" + jsSingleQuoteStr + "|" + jsDoubleQuoteStr + ")" };
		std::string jsKeyStr{ "(?:" + jsVarStr + "|" + jsQuoteStr + ")" };
		std::string jsPropStr{ "(?:\\." + jsVarStr + "|" + "\\[" + jsQuoteStr + "\\])" };
		std::string jsEmptyStr{ "(?:''|\"\")" };
		std::string reverseStr{ ":function\\(a\\)\\{(?:return)?a\\.reverse\\(\\)\\}" };
		std::string sliceStr{ ":function\\(a,b\\)\\{return a\\.slice\\(b\\)\\}" };
		std::string spliceStr{ ":function\\(a,b\\)\\{a\\.splice\\(0,b\\)\\}" };
		std::string swapStr{ ":function\\(a,b\\)\\{var c=a\\[0\\];a\\[0\\]=a\\[b(?:%a\\.length)?\\];a\\[b(?:%a\\.length)?\\]=c(?:;return a)?\\}" };
		std::regex reverseRegexp{ "(?:^|,)(" + jsKeyStr + ")" + reverseStr };
		std::regex sliceRegexp{ "(?:^|,)(" + jsKeyStr + ")" + sliceStr };
		std::regex spliceRegexp{ "(?:^|,)(" + jsKeyStr + ")" + spliceStr };
		std::regex swapRegexp{ "(?:^|,)(" + jsKeyStr + ")" + swapStr };
		std::string newString{ html5PlayerPageBody };
		std::vector<std::string> tokenActions{};
		std::regex actionsObjRegexp{ "var (" + jsVarStr + ")=\\{((?:(?:" + jsKeyStr + reverseStr + "|" + jsKeyStr + sliceStr + "|" + jsKeyStr + spliceStr + "|" + jsKeyStr + swapStr + "),?\r?\n?)+)\\};", std::regex_constants::ECMAScript };
		std::smatch actionsObjectRegexMatch{};
		regex_search(newString, actionsObjectRegexMatch, actionsObjRegexp);
		for (auto const& value : actionsObjectRegexMatch) {
			std::string actionsObjectResult = value.str();
			if (actionsObjectResult != "") {
				tokenActions.push_back(actionsObjectResult);
			}
		}

		std::regex actionsFuncRegexp("function(?:[a-zA-Z_\\$][a-zA-Z_0-9]*)?\\(a\\)\\{a=a\\.split\\((?:''|\"\")\\);\\S*((?:(?:a=)?[a-zA-Z_\\$][a-zA-Z_0-9]*(?:\\.[a-zA-Z_\\$][a-zA-Z_0-9]*|\\[(?:'[^'\\]*(:?\\[\\S\\S][^'\\]*)*'|\"[^\"\\]*(:?\\[\\S\\S][^\"\\]*)*\")\\])\\(a,\\d+\\);)+)return a\\.join\\((?:''|\"\")\\)\\}", std::regex_constants::ECMAScript);
		std::smatch actionFuncRegexMatch{};
		regex_search(newString, actionFuncRegexMatch, actionsFuncRegexp);
		for (auto const& value : actionFuncRegexMatch) {
			std::string actionFuncResult = value.str();
			if (actionFuncResult != "") {
				tokenActions.push_back(actionFuncResult);
			}
		}
		std::string object{ tokenActions[1] };
		std::string objectBody{ tokenActions[2] };
		std::string functionBody{ tokenActions[3] };
		std::smatch matchResultsNew01{};
		std::string reverseKey{};
		regex_search(objectBody, matchResultsNew01, reverseRegexp);
		if (!matchResultsNew01.empty() && matchResultsNew01.size() > 0) {
			reverseKey = regex_replace(matchResultsNew01[1].str(), std::regex("/g,'\\$"), "");
			reverseKey = regex_replace(reverseKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::smatch matchResultsNew02{};
		std::string sliceKey{};
		regex_search(objectBody, matchResultsNew02, sliceRegexp);
		if (!matchResultsNew02.empty() && matchResultsNew02.size() > 0) {
			sliceKey = regex_replace(matchResultsNew02[1].str(), std::regex("/g,'\\$"), "");
			sliceKey = regex_replace(sliceKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::smatch matchResultsNew03{};
		std::string spliceKey{};
		regex_search(objectBody, matchResultsNew03, spliceRegexp);
	
		if (!matchResultsNew03.empty() && matchResultsNew03.size() > 0) {
			spliceKey = regex_replace(matchResultsNew03[1].str(), std::regex("/g,'\\$"), "");
			spliceKey = regex_replace(spliceKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::smatch matchResultsNew04{};
		std::string swapKey{};
		regex_search(objectBody, matchResultsNew04, swapRegexp);
		if (!matchResultsNew04.empty() && matchResultsNew04.size() > 0) {
			swapKey = regex_replace(matchResultsNew04[1].str(), std::regex("/g,'\\$"), "");
			swapKey = regex_replace(swapKey, std::regex("\\$|^'|^\"|'$|\"$"), "");
		}
		std::vector<std::string> keys{};
		std::string keysNew{ "" };
		keys.push_back(reverseKey);
		keys.push_back(sliceKey);
		keys.push_back(spliceKey);
		keys.push_back(swapKey);
		for (uint32_t x = 0; x < keys.size(); x += 1) {
			if (x == 0 && keys[x] != "") {
				keysNew += keys[x];
			}
			else if (x > 0 && keys[x] != "") {
				keysNew += "|" + keys[x];
			}
			else if (keys[x] == "") {
				continue;
			}
		}
		std::regex tokenizeRegexp("(\" + object + \"\\.(" + keysNew + ")\\(a,(\\d{1,3})\\))");
		std::smatch tokenMatchResults{};
		std::vector<std::string> tokens{};
		std::string::const_iterator text_iter{ functionBody.cbegin() };
		while (regex_search(text_iter, functionBody.cend(), tokenMatchResults, tokenizeRegexp)) {
			std::string newString02 = std::string(tokenMatchResults[0].first, tokenMatchResults[0].second);
			if (newString02.find(reverseKey) != std::string::npos) {
				tokens.push_back("r");
			}
			else if (newString02.find(spliceKey) != std::string::npos) {
				tokens.push_back("p" + tokenMatchResults[3].str());
			}
			else if (newString02.find(swapKey) != std::string::npos) {
				tokens.push_back("w" + tokenMatchResults[3].str());
			}
			else if (newString02.find(sliceKey) != std::string::npos) {
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
		uint64_t positionStart{ body.find(left) + left.length() };
		uint64_t positionEnd{ body.find(right, positionStart) };
		std::string newString{ body.substr(positionStart, positionEnd - positionStart) };
		return newString;
	}

	std::vector<std::string> YouTubeRequestBuilder::getTokens(std::string html5PlayerFile) {
		std::vector<std::string> tokens{ YouTubeRequestBuilder::extractActions(html5PlayerFile) };
		if (tokens.size() == 0) {
			throw std::runtime_error("Could not extract signature deciphering actions");
		}
		return tokens;
	};

	std::vector<char> YouTubeRequestBuilder::splitString(std::string stringToSplit) {
		std::vector<char> charVector{};
		for (auto value : stringToSplit) {
			charVector.push_back(value);
		};
		return charVector;
	}

	std::string YouTubeRequestBuilder::joinString(std::vector<char> stringToJoin) {
		std::string newString{ };
		for (int32_t x = 0; x < stringToJoin.size(); x += 1) {
			newString += stringToJoin[x];
		}
		return newString;
	}

	std::string YouTubeRequestBuilder::setDownloadUrl(YouTubeFormat format) {
		std::string downloadUrl{ urlDecode(format.downloadUrl) + "&ratebypass=yes" };
		if (format.signature != "") {
			downloadUrl += "&sig=" + format.signature;
		}
		return downloadUrl;
	}

	YouTubeSong YouTubeRequestBuilder::collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong) {
		newSong.firstDownloadUrl = YouTubeRequestBuilder::baseUrl + "/watch?v=" + newSong.songId + "&hl=en";
		auto newerSong = YouTubeRequestBuilder::constructDownloadInfo(addedByGuildMember, newSong);
		newerSong = YouTubeRequestBuilder::constructFinalDownloadUrl(newerSong);
		newerSong.addedByUserId = addedByGuildMember.user.id;
		newerSong.addedByUserName = addedByGuildMember.user.userName;
		return newerSong;
	}

	YouTubeAPI::YouTubeAPI(std::string guildIdNew, DiscordCoreInternal::HttpClient* httpClient) : requestBuilder(httpClient) {
		this->guildId = guildIdNew;
	}

	YouTubeSong YouTubeAPI::collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong) {
		return this->requestBuilder.collectFinalSong(addedByGuildMember, newSong);
	}

	bool YouTubeAPI::stop() {
		if (getVoiceConnectionMap()->at(this->guildId)->areWeCurrentlyPlaying()) {
			this->cancelCurrentSong();
			AudioFrameData dataFrame{};
			while (getAudioBufferMap()->at(this->guildId)->tryReceive(dataFrame)) {};
			return true;
		}
		else {
			return false;
		}
	}

	void YouTubeAPI::cancelCurrentSong() {
		if (getSongAPIMap()->at(this->guildId) != nullptr) {
			if (getSongAPIMap()->at(this->guildId)->theTask != nullptr) {
				if (getSongAPIMap()->at(this->guildId)->theTask->getStatus() == CoRoutineStatus::Running) {
					getSongAPIMap()->at(this->guildId)->theTask->cancel();
					getSongAPIMap()->at(this->guildId)->theTask->get();
				}
				getSongAPIMap()->at(this->guildId)->theTask.reset(nullptr);
			}
		}
	}

	CoRoutine<void> YouTubeAPI::downloadAndStreamAudio(Song newSong, YouTubeAPI* youtubeAPI) {
		try {
			auto coroutineHandle = co_await NewThreadAwaitable<void>();
			std::vector<uint8_t> theVector{};
			DiscordCoreInternal::WebSocketSSLClient streamSocket{ newSong.finalDownloadUrls[0].urlPath,"443", &theVector, this->maxBufferSize };
			bool areWeDoneHeaders{ false };
			int64_t remainingDownloadContentLength{ newSong.contentLength };
			int64_t contentLengthCurrent{ youtubeAPI->maxBufferSize };
			int64_t bytesReadTotal01{ 0 };
			int32_t counter{ 0 };
			BuildAudioDecoderData dataPackage{};
			std::vector<uint8_t> requestNew{};
			std::cout << newSong.finalDownloadUrls[0].urlPath << std::endl;
			requestNew.insert(requestNew.begin(), newSong.finalDownloadUrls[1].urlPath.begin(), newSong.finalDownloadUrls[1].urlPath.end());
			std::cout << "THE PATH: " << newSong.finalDownloadUrls[1].urlPath << std::endl;
			streamSocket.processIO(true);
			streamSocket.writeData(requestNew);
			streamSocket.processIO(true);
			dataPackage.totalFileSize = static_cast<uint64_t>(newSong.contentLength) - static_cast<int64_t>(581);
			dataPackage.bufferMaxSize = youtubeAPI->maxBufferSize;
			std::unique_ptr<AudioDecoder> audioDecoder = std::make_unique<AudioDecoder>(dataPackage);
			AudioEncoder audioEncoder = AudioEncoder();
		breakOutPlayMore:
			if (counter > 45 && !getVoiceConnectionMap()->at(youtubeAPI->guildId)->areWeCurrentlyPlaying()) {
				audioDecoder.reset(nullptr);
				AudioFrameData frameData{};
				while (getAudioBufferMap()->at(youtubeAPI->guildId)->tryReceive(frameData)) {};
				frameData.type = AudioFrameType::Cancel;
				frameData.rawFrameData.sampleCount = 0;
				frameData.rawFrameData.data.clear();
				frameData.encodedFrameData.sampleCount = 0;
				frameData.encodedFrameData.data.clear();
				getAudioBufferMap()->at(youtubeAPI->guildId)->send(std::move(frameData));
				SongCompletionEventData eventData{};
				auto resultValue = getSongAPIMap()->at(youtubeAPI->guildId).get();
				if (resultValue != nullptr) {
					eventData.previousSong = resultValue->getCurrentSong(youtubeAPI->guildId);
				}
				eventData.voiceConnection = getVoiceConnectionMap()->at(youtubeAPI->guildId).get();
				eventData.wasItAFail = true;
				getSongAPIMap()->at(youtubeAPI->guildId)->onSongCompletionEvent(eventData);
				co_return;
			}
		breakOut:
			if (coroutineHandle.promise().isItStopped()) {
				audioDecoder.reset(nullptr);
				AudioFrameData frameData{};
				while (getAudioBufferMap()->at(youtubeAPI->guildId)->tryReceive(frameData)) {};
				frameData.type = AudioFrameType::Cancel;
				frameData.rawFrameData.sampleCount = 0;
				frameData.rawFrameData.data.clear();
				frameData.encodedFrameData.sampleCount = 0;
				frameData.encodedFrameData.data.clear();
				getAudioBufferMap()->at(youtubeAPI->guildId)->send(std::move(frameData));
				co_return;
			}
			while (newSong.contentLength > bytesReadTotal01) {
				if (coroutineHandle.promise().isItStopped()) {
					goto breakOut;
				}
				if (audioDecoder->haveWeFailed()) {
					goto breakOutPlayMore;
				}
				if (coroutineHandle.promise().isItStopped()) {
					goto breakOut;
				}
				else {
					if (!areWeDoneHeaders) {
						if (!coroutineHandle.promise().isItStopped()) {
							bytesReadTotal01 = streamSocket.getBytesRead();
						}
						if (coroutineHandle.promise().isItStopped()) {
							goto breakOut;
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal01;
						streamSocket.processIO(true);
						auto newData = streamSocket.getData();
						int64_t headerLength = newData.size();
						if (!coroutineHandle.promise().isItStopped()) {
							bytesReadTotal01 = streamSocket.getBytesRead();
						}
						if (coroutineHandle.promise().isItStopped()) {
							goto breakOut;
						}
						remainingDownloadContentLength = newSong.contentLength - bytesReadTotal01;
						areWeDoneHeaders = true;
					}
					if (coroutineHandle.promise().isItStopped()) {
						goto breakOut;
					}
					if (counter == 0) {
						streamSocket.processIO(true);
						auto streamBuffer = streamSocket.getData();
						audioDecoder->submitDataForDecoding(streamBuffer);
						audioDecoder->startMe();
					}
					if (counter > 0) {
						if (contentLengthCurrent > 0) {
							if (coroutineHandle.promise().isItStopped()) {
								goto breakOut;
							}
							bytesReadTotal01 = streamSocket.getBytesRead();
							remainingDownloadContentLength = newSong.contentLength - bytesReadTotal01;
							streamSocket.processIO(true);
							auto streamBuffer = streamSocket.getData();
							std::vector<uint8_t> newVector{};
							for (uint32_t x = 0; x < streamBuffer.size(); x += 1) {
								newVector.push_back(streamBuffer.data()[x]);
							}
							if (newVector.size() == 0) {
								counter += 1;
								continue;
							}
							audioDecoder->submitDataForDecoding(newVector);
						}
						if (coroutineHandle.promise().isItStopped()) {
							goto breakOut;
						}
						std::vector<RawFrameData> frames{};
						RawFrameData rawFrame{};
						while (audioDecoder->getFrame(rawFrame)) {
							if (rawFrame.data.size() != 0) {
								frames.push_back(rawFrame);
							}
						}
						if (frames.size() == 0) {
							counter += 1;
							continue;
						}
						if (coroutineHandle.promise().isItStopped()) {
							goto breakOut;
						}
						auto encodedFrames = audioEncoder.encodeFrames(frames);
						for (auto& value : encodedFrames) {
							getAudioBufferMap()->at(youtubeAPI->guildId)->send(std::move(value));
						}
					}
					if (remainingDownloadContentLength >= youtubeAPI->maxBufferSize) {
						contentLengthCurrent = youtubeAPI->maxBufferSize;
					}
					else {
						contentLengthCurrent = remainingDownloadContentLength;
					}
				}
				counter += 1;
			}
			RawFrameData frameData01{};
			while (audioDecoder->getFrame(frameData01)) {};
			audioDecoder.reset(nullptr);
			AudioFrameData frameData{};
			frameData.type = AudioFrameType::Cancel;
			frameData.rawFrameData.sampleCount = 0;
			frameData.encodedFrameData.sampleCount = 0;
			getAudioBufferMap()->at(youtubeAPI->guildId)->send(std::move(frameData));
			co_return;
		}
		catch (...) {
			reportException("YouTubeAPI::downloadAndStreamAudio()");
		}
	};

	void YouTubeAPI::sendNextSong(Song newSong) {
		this->cancelCurrentSong();
		AudioFrameData frameData{};
		while (getVoiceConnectionMap()->at(this->guildId)->getAudioBuffer().tryReceive(frameData)) {};
		getSongAPIMap()->at(this->guildId)->theTask = std::make_unique<CoRoutine<void>>(this->downloadAndStreamAudio(newSong, this));
	}

	std::vector<YouTubeSong> YouTubeAPI::searchForSong(std::string searchQuery, std::string guildId) {
		return this->requestBuilder.collectSearchResults(searchQuery);
	}

	YouTubeAPI::~YouTubeAPI() {};

};