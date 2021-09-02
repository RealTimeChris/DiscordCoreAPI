// YouTubeStuff.hpp - Header for the "YouTube stuff".
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _YOUTUBE_STUFF_
#define _YOUTUBE_STUFF_

#include "HttpStuff.hpp"
#include "DataParsingFunctions.hpp"
#include "DiscordCoreClientBase.hpp"

namespace DiscordCoreAPI {

	string between(string body, string left, string right) {
		int positionStart = (int)body.find(left) + (int)left.length();
		int positionEnd = (int)body.find(right, positionStart);
		string newString = body.substr(positionStart, positionEnd - positionStart);
		return newString;
	}

	vector<char> splitString(string stringToSplit) {
		vector<char> charVector;
		for (auto value : stringToSplit) {
			charVector.push_back(value);
		};
		return charVector;
	}

	vector<char> sliceVector(vector<char> vectorToSlice, unsigned int firstElement, unsigned int lastElement = 0) {
		vector<char> newVector;
		if (lastElement == 0) {
			lastElement = (unsigned int)vectorToSlice.size();
		}
		if (lastElement > vectorToSlice.size()) {
			throw exception("Sorry, but you've claimed a size for the vector that is larger than the input vector!");
		}
		for (unsigned int x = firstElement; x < lastElement; x += 1) {
			newVector.push_back(vectorToSlice[x]);
		}
		return newVector;
	}

	vector<char> reverseString(vector<char> stringToReverse) {
		size_t n = stringToReverse.size();
		for (int i = 0; i < n / 2; i++)
			swap(stringToReverse[i], stringToReverse[n - i - 1]);
		return stringToReverse;
	}

	vector<char> swapHeadAndPosition(vector<char> inputVector, int position) {
		char first = inputVector[0];
		inputVector[0] = inputVector[position % inputVector.size()];
		inputVector[position] = first;
		return inputVector;
	}
	
	string joinString(vector<char> stringToJoin) {
		string newString;
		for (unsigned int x = 0; x < stringToJoin.size(); x += 1) {
			newString += stringToJoin[x];
		}
		return newString;
	}

	string setDownloadURL(YouTubeFormat format) {
		string downloadURL = format.downloadURL;
		CURLU* urlHandle = curl_url();
		int* outLength{ nullptr };
		char* charString = curl_easy_unescape(urlHandle, downloadURL.c_str(), (int)downloadURL.length(), outLength);
		downloadURL = charString;
		curl_free(charString);
		downloadURL += "&ratebypass=yes";
		if (format.signature != "") {
			downloadURL += "&sig=" + format.signature;
		}
		return downloadURL;
	}

	vector<string> extractActions(string html5PlayerPageBody) {
		string jsVarStr = "[a-zA-Z_\\$][a-zA-Z_0-9]*";
		string jsSingleQuoteStr = "'[^'\\\\]*(:?\\\\[\\s\\S][^'\\\\]*)*'";
		string jsDoubleQuoteStr = "\"[^\"\\\\]*(:?\\\\[\\s\\S][^\"\\\\]*)*\"";
		string jsQuoteStr = "(?:" + jsSingleQuoteStr + "|" + jsDoubleQuoteStr + ")";
		string jsKeyStr = "(?:" + jsVarStr + "|" + jsQuoteStr + ")";
		string jsPropStr = "(?:\\." + jsVarStr + "|" + "\\[" + jsQuoteStr + "\\])";
		string jsEmptyStr = "(?:''|\"\")";
		string reverseStr = ":function\\(a\\)\\{(?:return)?a\\.reverse\\(\\)\\}";
		string sliceStr = ":function\\(a,b\\)\\{return a\\.slice\\(b\\)\\}";
		string spliceStr = ":function\\(a,b\\)\\{a\\.splice\\(0,b\\)\\}";
		string swapStr = ":function\\(a,b\\)\\{var c=a\\[0\\];a\\[0\\]=a\\[b(?:%a\\.length)?\\];a\\[b(?:%a\\.length)?\\]=c(?:;return a)?\\}";

		regex reverseRegexp("(?:^|,)(" + jsKeyStr + ")" + reverseStr);
		regex sliceRegexp("(?:^|,)(" + jsKeyStr + ")" + sliceStr);
		regex spliceRegexp("(?:^|,)(" + jsKeyStr + ")" + spliceStr);
		regex swapRegexp("(?:^|,)(" + jsKeyStr + ")" + swapStr);

		string newString = html5PlayerPageBody;
		vector<string> tokenActions;
		regex actionsObjRegexp("var (" + jsVarStr + ")=\\{((?:(?:" + jsKeyStr + reverseStr + "|" + jsKeyStr + sliceStr + "|" + jsKeyStr + spliceStr + "|" + jsKeyStr + swapStr + "),?\\r?\\n?)+)\\};", regex_constants::ECMAScript);
		smatch actionsObjectRegexMatch;
		regex_search(newString, actionsObjectRegexMatch, actionsObjRegexp, regex_constants::match_flag_type::match_any | regex_constants::match_flag_type::match_not_null | regex_constants::match_flag_type::match_prev_avail);
		for (auto value : actionsObjectRegexMatch) {
			string actionsObjectResult = value.str();
			if (actionsObjectResult != "") {
				tokenActions.push_back(actionsObjectResult);
			}
		}

		regex actionsFuncRegexp("function(?:[a-zA-Z_\\$][a-zA-Z_0-9]*)?\\(a\\)\\{a=a\\.split\\((?:''|\"\")\\);\\s*((?:(?:a=)?[a-zA-Z_\\$][a-zA-Z_0-9]*(?:\\.[a-zA-Z_\\$][a-zA-Z_0-9]*|\\[(?:'[^'\\\\]*(:?\\\\[\\s\\S][^'\\\\]*)*'|\"[^\"\\\\]*(:?\\\\[\\s\\S][^\"\\\\]*)*\")\\])\\(a,\\d+\\);)+)return a\\.join\\((?:''|\"\")\\)\\}", regex_constants::ECMAScript);
		smatch actionFuncRegexMatch;
		regex_search(newString, actionFuncRegexMatch, actionsFuncRegexp, regex_constants::match_flag_type::match_any | regex_constants::match_flag_type::match_not_null | regex_constants::match_flag_type::match_prev_avail);
		for (auto value : actionFuncRegexMatch) {
			string actionFuncResult = value.str();
			if (actionFuncResult != "") {
				tokenActions.push_back(actionFuncResult);
			}
		}

		string object = tokenActions[1];
		string objectBody = tokenActions[2];
		string functionBody = tokenActions[4];

		smatch matchResultsNew01;
		string reverseKey;
		regex_search(objectBody, matchResultsNew01, reverseRegexp);
		if (!matchResultsNew01.empty() && matchResultsNew01.size() > 0) {
			reverseKey = regex_replace(matchResultsNew01[1].str(), regex("/g,'\\$"), "");
			reverseKey = regex_replace(reverseKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}
		smatch matchResultsNew02;
		string sliceKey;
		regex_search(objectBody, matchResultsNew02, sliceRegexp);
		if (!matchResultsNew02.empty() && matchResultsNew02.size() > 0) {
			sliceKey = regex_replace(matchResultsNew02[1].str(), regex("/g,'\\$"), "");
			sliceKey = regex_replace(sliceKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}
		smatch matchResultsNew03;
		string spliceKey;
		regex_search(objectBody, matchResultsNew03, spliceRegexp);
		if (!matchResultsNew03.empty() && matchResultsNew03.size() > 0) {
			spliceKey = regex_replace(matchResultsNew03[1].str(), regex("/g,'\\$"), "");
			spliceKey = regex_replace(spliceKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}
		smatch matchResultsNew04;
		string swapKey;
		regex_search(objectBody, matchResultsNew04, swapRegexp);
		if (!matchResultsNew04.empty() && matchResultsNew04.size() > 0) {
			swapKey = regex_replace(matchResultsNew04[1].str(), regex("/g,'\\$"), "");
			swapKey = regex_replace(swapKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}

		string keys = "(" + reverseKey + "|" + sliceKey + "|" + spliceKey + "|" + swapKey + ")";
		regex tokenizeRegexp("(?:a=)?" + object + "(?:\\." + keys + "|\\['" + keys + "'\\]|\\[" + keys + "\\])\\(a,(\\d+)\\)");
		smatch tokenMatchResults;
		vector<string> tokens;
		std::string::const_iterator text_iter = functionBody.cbegin();
		while (regex_search(text_iter, functionBody.cend(), tokenMatchResults, tokenizeRegexp, regex_constants::match_flag_type::match_not_null | regex_constants::match_flag_type::match_prev_avail | regex_constants::match_flag_type::format_no_copy)) {
			string newString02 = string(tokenMatchResults[0].first, tokenMatchResults[0].second);

			if (newString02.find(reverseKey) != string::npos) {
				tokens.push_back("r");
			}
			else if (newString02.find(spliceKey) != string::npos) {
				tokens.push_back("p" + tokenMatchResults[4].str());
			}
			else if (newString02.find(swapKey) != string::npos) {
				tokens.push_back("w" + tokenMatchResults[4].str());
			}
			else if (newString02.find(sliceKey) != string::npos) {
				tokens.push_back("s" + tokenMatchResults[4].str());
			}
			text_iter = tokenMatchResults[0].second;
		};

		return tokens;
	}

	vector<string> getTokens(string html5PlayerFile) {
		string body = html5PlayerFile;
		vector<string> tokens = extractActions(body);
		if (tokens.size() == 0) {
			throw exception("Could not extract signature deciphering actions");
		}
		return tokens;
	};

	string decipher(vector<string> tokens, string cipherSignature) {
		vector<char> signatureNew = splitString(cipherSignature);

		for (unsigned int x = 0, len = (unsigned int)tokens.size(); x < len; x += 1) {
			string token = tokens[x];
			int position;
			switch (token[0]) {
			case 'r':
				signatureNew = reverseString(signatureNew);
				break;
			case 'w':
				position = stol(token.substr(1));
				signatureNew = swapHeadAndPosition(signatureNew, position);
				break;
			case 's':
				position = stol(token.substr(1));
				signatureNew = sliceVector(signatureNew, position);
				break;
			case 'p':
				position = stol(token.substr(1));
				signatureNew = sliceVector(signatureNew, position);
				break;
			}
		}
		string signatureNewString = joinString(signatureNew);
		return signatureNewString;
	}

	YouTubeFormat decipherFormat(YouTubeFormat format, string html5playerFile) {
		CURLU* urlHandle = curl_url();
		int* outLength{ nullptr };
		char* charString = curl_easy_unescape(urlHandle, format.signature.c_str(), (int)format.signature.length(), outLength);
		format.signature = charString;
		curl_free(charString);
		YouTubeFormat decipheredFormat = format;
		vector<string> tokens = getTokens(html5playerFile);
		if (decipheredFormat.signature.length() >= to_string(L"s=").length()) {
			decipheredFormat.signature = decipheredFormat.signature.substr(to_string(L"s=").length());
			decipheredFormat.signature = decipher(tokens, decipheredFormat.signature);
		}
		decipheredFormat.downloadURL = setDownloadURL(decipheredFormat);
		return decipheredFormat;
	};

	class YouTubeAPI{
	public:

		friend class Guild;

		YouTubeAPI(shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioBufferNew, string guildIdNew, map<string, shared_ptr<YouTubeAPI>>* youtubeAPIPointer){
			this->sendAudioBuffer = sendAudioBufferNew;
			this->youtubeAPIMap = youtubeAPIPointer;
			this->guildId = guildIdNew;
			YouTubeAPI::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
		}

		void setLoopAllStatus(bool enabled) {
			this->loopAll = enabled;
		}

		bool isLoopAllEnabled() {
			return this->loopAll;
		}

		void setLoopSongStatus(bool enabled) {
			this->loopSong = enabled;
		}

		bool isLoopSongEnabled() {
			return this->loopSong;
		}

		bool stop() {
			if (this->areWePlaying) {
				vector<uint8_t> newVector;
				send(this->currentDataSendBuffer, newVector);
				send(this->currentDataSendBuffer, newVector);
				send(this->currentDataSendBuffer, newVector);
				receive(this->completionCallbackBuffer);
				AudioFrameData dataFrame;
				while (try_receive(*this->sendAudioBuffer, dataFrame)) {};
				vector<YouTubeSong> newVector02;
				if (this->lastSong.description != "") {
					newVector02.push_back(this->lastSong);
					this->lastSong = YouTubeSong();
				}
				else {
					newVector02.push_back(this->currentSong);
				}
				for (auto value : this->songQueue) {
					newVector02.push_back(value);
				}
				this->songQueue = newVector02;
				this->currentSong = YouTubeSong();
				return true;
			}
			else {
				return false;
			}
		}

		bool skip() {
			if (this->areWePlaying) {
				vector<uint8_t> newVector;
				send(this->currentDataSendBuffer, newVector);
				send(this->currentDataSendBuffer, newVector);
				send(this->currentDataSendBuffer, newVector);
				receive(this->completionCallbackBuffer);
				AudioFrameData dataFrame;
				while (try_receive(*this->sendAudioBuffer, dataFrame)) {};
				return true;
			}
			else {
				return false;
			}
		}

		bool isThereAnySongs() {
			if (this->songQueue.size() == 0 && this->currentSong.songId == "") {
				return false;
			}
			else {
				return true;
			}
		}

		void addSongToQueue(YouTubeSearchResult searchResult, GuildMember guildMember) {
			string watchHTMLURL = to_string(this->baseWatchURL) + searchResult.videoId + "&hl=en";
			Filters::HttpBaseProtocolFilter filter;
			filter.AutomaticDecompression(true);
			Filters::HttpCacheControl cacheControl = filter.CacheControl();
			cacheControl.ReadBehavior(Filters::HttpCacheReadBehavior::NoCache);
			cacheControl.WriteBehavior(Filters::HttpCacheWriteBehavior::NoCache);
			winrt::Windows::Web::Http::HttpClient httpClientGetHTMLBody(filter);
			HttpRequestMessage requestMessageHTMLBody;
			auto headersNewHTMLBody = requestMessageHTMLBody.Headers();
			headersNewHTMLBody.Append(L"user-agent", L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
			requestMessageHTMLBody.RequestUri(winrt::Windows::Foundation::Uri(to_hstring(watchHTMLURL)));
			allocator<csub_match> allocator;
			cmatch html5PlayerMatchResults(allocator);
			hstring resultStringHTMLBody;
			auto resultHTMLBody = httpClientGetHTMLBody.SendRequestAsync(requestMessageHTMLBody).get();
			resultStringHTMLBody = resultHTMLBody.Content().ReadAsStringAsync().get();
			string resultStringStringHTMLBody = to_string(resultStringHTMLBody);
			this->html5Player = to_string(this->baseURL) + resultStringStringHTMLBody.substr(resultStringStringHTMLBody.find("/s/player/"), resultStringStringHTMLBody.find("/player_ias.vflset/en_US/base.js") + to_string(L"/player_ias.vflset/en_US/base.js").length());
			this->html5Player = this->html5Player.substr(0, 73);
			this->playerResponse = between(to_string(resultStringHTMLBody), "ytInitialPlayerResponse = ", "</script>");
			this->playerResponse = this->playerResponse.substr(0, this->playerResponse.length() - 170);
			json jsonObject;
			if (this->playerResponse != "") {
				jsonObject = json::parse(this->playerResponse);
			}
			DiscordCoreInternal::DataParser::parseObject(jsonObject, &searchResult.formats);
			YouTubeFormat format;
			for (auto value : searchResult.formats) {
				if (value.mimeType.find("opus") != string::npos) {
					format = value;
					if (value.audioQuality == "AUDIO_QUALITY_MEDIUM" && format.audioQuality == "AUDIO_QUALITY_LOW") {
						format = value;
					}
					if (value.audioQuality == "AUDIO_QUALITY_HIGH") {
						format = value;
					}
				}
			}

			HttpRequestMessage requestPlayerFile;
			auto requestPlayerFileHeaders = requestPlayerFile.Headers();
			requestPlayerFileHeaders.Append(L"user-agent", L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
			requestPlayerFile.RequestUri(winrt::Windows::Foundation::Uri(to_hstring(this->html5Player)));
			auto responseMessage02 = httpClientGetHTMLBody.SendRequestAsync(requestPlayerFile).get();
			hstring responseToPlayerGet02 = responseMessage02.Content().ReadAsStringAsync().get();
			this->html5PlayerFile = to_string(responseToPlayerGet02);
			format = decipherFormat(format, this->html5PlayerFile);
			YouTubeSong song;
			song.description = searchResult.description;
			song.duration = searchResult.duration;
			song.formatDownloadURL = format.downloadURL;
			song.imageURL = searchResult.thumbNailURL;
			song.contentLength = (int)format.contentLength;
			song.songId = to_string((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			song.title = searchResult.videoTitle;
			song.url = searchResult.videoURL;
			song.videoId = searchResult.videoId;
			song.addedById = guildMember.user.id;
			song.addedByUserName = guildMember.user.username;
			bool isItFound = false;
			for (auto value : this->songQueue) {
				if (value.songId == song.songId) {
					isItFound = true;
				}
			}
			this->songQueue.push_back(song);
		}

		void setQueue(vector<YouTubeSong> dataPackage) {
			this->songQueue = dataPackage;
		}

		vector<YouTubeSong>* getQueue() {
			return &this->songQueue;
		}

		task<void> modifyQueue(int songOldPosition, int songNewPosition) {
			shared_ptr<YouTubeAPI> youtubeAPI = this->youtubeAPIMap->at(this->guildId);
			int currentSongCount = (int)youtubeAPI->songQueue.size();
			for (int x = 0; x < currentSongCount; x += 1) {
				YouTubeSong tempSong = youtubeAPI->songQueue.at(songOldPosition);
				if (x < songNewPosition && x > songOldPosition) {
					youtubeAPI->songQueue.at(x - 1) = youtubeAPI->songQueue.at(x);
				}
				youtubeAPI->songQueue.at(songNewPosition) = tempSong;
			}
			this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
		}

		YouTubeSong getCurrentSong() {
			shared_ptr<YouTubeAPI> youtubeAPI = this->youtubeAPIMap->at(this->guildId);
			if (youtubeAPI->lastSong.description != "") {
				return youtubeAPI->lastSong;
			}
			else if (youtubeAPI->currentSong.videoId != "") {
				return youtubeAPI->currentSong;
			}
			else if (youtubeAPI->songQueue.size() > 0) {
				return youtubeAPI->songQueue.at(0);
			}
			else {
				return YouTubeSong();
			};
		}

		SendNextSongReturnData sendNextSong() {
			shared_ptr<YouTubeAPI> youtubeAPI = this->youtubeAPIMap->at(this->guildId);
			SendNextSongReturnData returnData;
			if (youtubeAPI->loopSong) {
				if (youtubeAPI->songQueue.size() > 1 && youtubeAPI->currentSong.description == "") {
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					for (int x = 0; x < youtubeAPI->songQueue.size(); x += 1) {
						if (x == youtubeAPI->songQueue.size() - 1) {
							break;
						}
						youtubeAPI->songQueue[x] = youtubeAPI->songQueue[x + 1];
					}
					youtubeAPI->songQueue.erase(youtubeAPI->songQueue.end() - 1, youtubeAPI->songQueue.end());
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->songQueue.size() > 0 && youtubeAPI->currentSong.description != "") {
					youtubeAPI->currentSong = youtubeAPI->currentSong;
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->currentSong.description != "" && youtubeAPI->songQueue.size() == 0) {
					youtubeAPI->currentSong = youtubeAPI->currentSong;
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->songQueue.size() == 1 && youtubeAPI->currentSong.description == "") {
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					youtubeAPI->songQueue.erase(youtubeAPI->songQueue.begin(), youtubeAPI->songQueue.begin() + 1);
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->currentSong.formatDownloadURL == "") {
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
			}
			else if (youtubeAPI->loopAll) {
				if (youtubeAPI->songQueue.size() > 1 && youtubeAPI->currentSong.description == "") {
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					for (int x = 0; x < youtubeAPI->songQueue.size(); x += 1) {
						if (x == youtubeAPI->songQueue.size() - 1) {
							break;
						}
						youtubeAPI->songQueue[x] = youtubeAPI->songQueue[x + 1];
					}
					youtubeAPI->songQueue.erase(youtubeAPI->songQueue.end() - 1, youtubeAPI->songQueue.end());
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->songQueue.size() > 0 && youtubeAPI->currentSong.description != "") {
					auto tempSong02 = youtubeAPI->currentSong;
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					for (int x = 0; x < youtubeAPI->songQueue.size(); x += 1) {
						if (x == youtubeAPI->songQueue.size() - 1) {
							break;
						}
						youtubeAPI->songQueue[x] = youtubeAPI->songQueue[x + 1];
					}
					youtubeAPI->songQueue.at(youtubeAPI->songQueue.size() - 1) = tempSong02;
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->currentSong.description != "" && youtubeAPI->songQueue.size() == 0) {
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->songQueue.size() == 1 && youtubeAPI->currentSong.description == "") {
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					youtubeAPI->songQueue.erase(youtubeAPI->songQueue.begin(), youtubeAPI->songQueue.begin() + 1);
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->currentSong.videoId == "") {
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
			}
			else {
				if (youtubeAPI->songQueue.size() > 0 && (youtubeAPI->currentSong.description != "" || youtubeAPI->currentSong.description == "")) {
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					for (int x = 0; x < youtubeAPI->songQueue.size()-1; x += 1) {
						youtubeAPI->songQueue[x] = youtubeAPI->songQueue[x + 1];
					}
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->songQueue.erase(youtubeAPI->songQueue.end() - 1, youtubeAPI->songQueue.end());
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->currentSong.description != "" && youtubeAPI->songQueue.size() == 0) {
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					returnData.currentSong = youtubeAPI->currentSong;
					youtubeAPI->lastSong = youtubeAPI->currentSong;
					youtubeAPI->currentSong = YouTubeSong();
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->songQueue.size() == 1 && youtubeAPI->currentSong.description == "") {
					youtubeAPI->currentSong = youtubeAPI->songQueue.at(0);
					youtubeAPI->songQueue.erase(youtubeAPI->songQueue.begin(), youtubeAPI->songQueue.begin() + 1);
					youtubeAPI->downloadAndStreamAudio(youtubeAPI->currentSong);
					youtubeAPI->lastSong = YouTubeSong();
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
				else if (youtubeAPI->currentSong.videoId == "") {
					returnData.currentSong = youtubeAPI->currentSong;
					this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
					return returnData;
				}
			}
			this->youtubeAPIMap->insert_or_assign(this->guildId, youtubeAPI);
			return returnData;
		}

		vector<YouTubeSearchResult> searchForVideo(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = to_string(this->baseSearchURL);
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::YOUTUBE_SEARCH;
			CURLU* urlHandle = curl_url();
			curl_url_set(urlHandle, CURLUPart::CURLUPART_QUERY, searchQuery.c_str(), CURLU_URLENCODE);
			char* charString{ nullptr };
			curl_url_get(urlHandle, CURLUPart::CURLUPART_QUERY, &charString, 0);
			workload.relativePath = charString;
			curl_free(charString);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreInternal::HttpData returnData = receive(requestAgent.workReturnBuffer);
			vector<DiscordCoreAPI::YouTubeSearchResult> searchResults;
			json partialSearchResultsJson = returnData.data;
			for (auto value : partialSearchResultsJson.at("contents").at("twoColumnSearchResultsRenderer").at("primaryContents").at("sectionListRenderer").at("contents").at(0).at("itemSectionRenderer").at("contents")) {
				DiscordCoreAPI::YouTubeSearchResult searchResult;
				if (value.contains("videoRenderer")) {
					DiscordCoreInternal::DataParser::parseObject(value.at("videoRenderer"), &searchResult);
					searchResults.push_back(searchResult);
				}
			}
			if (returnData.returnCode != 200) {
				cout << "this::searchForVideo() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
			}
			return searchResults;
		}

		static void cleanup() {
			YouTubeAPI::threadContext->releaseGroup();
		}

		~YouTubeAPI() {}

	protected:
		static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
		const hstring baseSearchURL{ L"https://www.youtube.com/results?search_query=" };
		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioBuffer{ nullptr };
		unbounded_buffer<vector<uint8_t>>* currentDataSendBuffer{ nullptr };
		const hstring baseWatchURL{ L"https://www.youtube.com/watch?v=" };
		map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMap{ nullptr };
		unbounded_buffer<bool> completionCallbackBuffer{ nullptr };
		const hstring baseURL{ L"https://www.youtube.com" };
		vector<YouTubeSong> songQueue{};
		const string newLine{ "\n\r" };
		string html5PlayerFile{ "" };
		const int maxBufSize{ 4096 };
		string playerResponse{ "" };
		bool areWePlaying{ false };
		YouTubeSong currentSong{};
		string html5Player{ "" };
		bool loopSong{ false };
		YouTubeSong lastSong{};
		bool loopAll{ false };
		string guildId{ "" };

		task<void> downloadAndStreamAudio(YouTubeSong song, int retryCount = 0) {
			apartment_context mainThread;
			co_await resume_foreground(*YouTubeAPI::threadContext->dispatcherQueue);
			try {
				string downloadBaseURL;
				if (song.formatDownloadURL.find("https://") != string::npos && song.formatDownloadURL.find("/videoplayback?") != string::npos) {
					downloadBaseURL = song.formatDownloadURL.substr(song.formatDownloadURL.find("https://") + to_string(L"https://").length(), song.formatDownloadURL.find("/videoplayback?") - to_string(L"https://").length());
				}
				// Creates GET request.
				string request = "GET " + song.formatDownloadURL + " HTTP/1.1" + newLine +
					"user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
					+ newLine + newLine;
				StreamSocket streamSocket = StreamSocket();
				streamSocket.Control().QualityOfService(SocketQualityOfService::LowLatency);
				streamSocket.Control().NoDelay(true);
				streamSocket.Control().SerializeConnectionAttempts(false);
				winrt::Windows::Networking::HostName hostName(to_hstring(downloadBaseURL));
				streamSocket.ConnectAsync(streamSocket.GetEndpointPairsAsync(hostName, L"443").get().First().Current()).get();
				streamSocket.UpgradeToSslAsync(SocketProtectionLevel::Tls12, hostName).get();
				DataReader dataReader(streamSocket.InputStream());
				dataReader.InputStreamOptions(InputStreamOptions::None);
				dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
				DataWriter dataWriter(streamSocket.OutputStream());
				dataWriter.WriteString(to_hstring(request));
				dataWriter.StoreAsync().get();
				bool areWeDoneHeaders{ false };
				int remainingDownloadContentLength{ song.contentLength };
				int contentLengthCurrent{ this->maxBufSize };
				int bytesReadTotal{ 0 };
				int counter{ 0 };
				BuildSongDecoderData dataPackage{};
				dataPackage.dataBuffer = new unbounded_buffer<vector<uint8_t>>();
				dataPackage.totalFileSize = song.contentLength - 585;
				dataPackage.bufferMaxSize = this->maxBufSize;
				this->currentDataSendBuffer = dataPackage.dataBuffer;
				send(dataPackage.dataBuffer, vector<uint8_t>());
				SongDecoder* songDecoder = new SongDecoder(dataPackage, DiscordCoreInternal::ThreadManager::getThreadContext().get());
				SongEncoder* songEncoder = new SongEncoder();
				while (song.contentLength > bytesReadTotal || songDecoder->status() != agent_done) {
					this->areWePlaying = true;
					int bytesRead{ 0 };
					InMemoryRandomAccessStream outputStream;
					DataWriter streamDataWriter(outputStream);
					streamDataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
					if (!areWeDoneHeaders) {
						bytesRead = dataReader.LoadAsync((uint32_t)contentLengthCurrent).get();
						bytesReadTotal += bytesRead;
						remainingDownloadContentLength -= bytesRead;
						auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
						stringstream bufferStream;
						bufferStream << buffer.data();
						string headers = bufferStream.str();
						int headerLength = (int)headers.find("gvs 1.0") + (int)to_string(L"gvs 1.0").length();
						bytesRead = dataReader.LoadAsync((uint32_t)headerLength + 4).get();
						bytesReadTotal += bytesRead;
						remainingDownloadContentLength -= bytesRead;
						auto buffer02 = dataReader.ReadBuffer((uint32_t)headerLength + 4);
						headers = headers.substr(0, headers.find("gvs 1.0") + to_string(L"gvs 1.0").length());
						streamDataWriter.WriteBuffer(buffer, headerLength + 4, (uint32_t)contentLengthCurrent - 4 - headerLength);
						streamDataWriter.WriteBuffer(buffer02, 0, (uint32_t)headerLength + 4);
						streamDataWriter.StoreAsync().get();
						areWeDoneHeaders = true;
					}
					vector<RawFrameData> frames;
					if (counter == 0) {
						DataReader streamDataReader(outputStream.GetInputStreamAt(0));
						streamDataReader.LoadAsync((uint32_t)contentLengthCurrent).get();
						auto streamBuffer = streamDataReader.ReadBuffer((uint32_t)contentLengthCurrent);
						vector<uint8_t> newVector;
						for (unsigned int x = 0; x < streamBuffer.Length(); x += 1) {
							newVector.push_back(streamBuffer.data()[x]);
						}
						send(dataPackage.dataBuffer, newVector);
					}
					if (counter > 0) {
						if (contentLengthCurrent > 0) {
							bytesRead = dataReader.LoadAsync((uint32_t)contentLengthCurrent).get();
							bytesReadTotal += bytesRead;
							remainingDownloadContentLength -= bytesRead;
							auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
							streamDataWriter.WriteBuffer(buffer);
							streamDataWriter.StoreAsync().get();
							DataReader streamDataReader(outputStream.GetInputStreamAt(0));
							streamDataReader.LoadAsync((uint32_t)contentLengthCurrent).get();
							auto streamBuffer = streamDataReader.ReadBuffer((uint32_t)contentLengthCurrent);
							vector<uint8_t> newVector;
							for (unsigned int x = 0; x < streamBuffer.Length(); x += 1) {
								newVector.push_back(streamBuffer.data()[x]);
							}
							send(dataPackage.dataBuffer, newVector);
						}
						RawFrameData rawFrame;
						rawFrame.data.resize(0);
						bool doWeBreak{ false };
						while (songDecoder->getFrame(&rawFrame)) {
							if (rawFrame.frameStatus == FrameStatus::Stopped) {
								doWeBreak = true;
								break;
							}
							if (rawFrame.data.size() != 0) {
								frames.push_back(rawFrame);
							}
						}
						if (doWeBreak) {
							break;
						}
						auto encodedFrames = songEncoder->encodeFrames(frames);
						for (auto value : encodedFrames) {
							send(*this->sendAudioBuffer, value);
						}
					}

					if (remainingDownloadContentLength >= this->maxBufSize) {
						contentLengthCurrent = this->maxBufSize;
					}
					else {
						contentLengthCurrent = remainingDownloadContentLength;
					}
					counter += 1;
				}
				vector<uint8_t> newVector;
				send(dataPackage.dataBuffer, newVector);
				RawFrameData frameData01;
				songDecoder->getFrame(&frameData01);
				AudioFrameData frameData02;
				frameData02.encodedFrameData.sampleCount = 0;
				frameData02.rawFrameData.sampleCount = 0;
				frameData02.frameStatus = FrameStatus::Stopped;
				send(*this->sendAudioBuffer, frameData02);
				send(this->completionCallbackBuffer, true);
				co_await mainThread;
				co_return;
			}
			catch (hresult_out_of_bounds& e) {
				retryCount += 1;
				if (retryCount < 5) {
					cout << "this::downloadAudio()::Out of bounds " << to_string(e.message()) << endl;
					downloadAndStreamAudio(song, retryCount);
				}
				else {
					co_return;
				}
			}
		}

	};
	shared_ptr<DiscordCoreInternal::ThreadContext> YouTubeAPI::threadContext{ nullptr };
};
#endif