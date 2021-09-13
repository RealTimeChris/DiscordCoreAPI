// YouTubeStuff.hpp - Header for the "YouTube stuff".
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _YOUTUBE_STUFF_
#define _YOUTUBE_STUFF_

#include "DiscordCoreClientBase.hpp"
#include "HttpStuff.hpp"
#include "DataParsingFunctions.hpp"
#include "DatabaseStuff.hpp"
#include "VoiceConnectionStuff.hpp"

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

	class YouTubeAPICore {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj)noexcept;
		friend class VoiceConnection;
		friend class YouTubeAPI;
		friend class Guild;

		YouTubeAPICore(map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap, string guildIdNew, DiscordGuild* discordGuildNew, shared_ptr<VoiceConnection> voiceConnectionNew) {
			this->sendAudioDataBufferMap = sendAudioDataBufferMap;
			this->sendAudioDataBuffer = this->sendAudioDataBufferMap->at(guildIdNew).get();
			this->discordGuild = discordGuildNew;
			this->guildId = guildIdNew;
			this->voiceConnection = voiceConnectionNew;
		}

	protected:

		map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap{ nullptr };
		const string baseSearchURL{ "https://www.youtube.com/results?search_query=" };
		cancellation_token_source cancelTokenSource{ cancellation_token_source() };
		cancellation_token cancelToken{ this->cancelTokenSource.get_token() };
		unbounded_buffer<AudioFrameData>* sendAudioDataBuffer{ nullptr };
		const string baseWatchURL{ "https://www.youtube.com/watch?v=" };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		const string baseURL{ "https://www.youtube.com" };
		DiscordGuild* discordGuild{ new DiscordGuild };
		unbounded_buffer<bool> readyBuffer{ nullptr };
		vector<YouTubeSong> songQueue{};
		const string newLine{ "\n\r" };
		string html5PlayerFile{ "" };
		const int maxBufSize{ 4096 };
		string playerResponse{ "" };
		bool areWeStopping{ false };
		SongEncoder songEncoder{};
		YouTubeSong currentSong{};
		string html5Player{ "" };
		task<bool> currentTask{};
		bool loopSong{ false };
		bool loopAll{ false };
		string guildId{ "" };

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

		void setQueue(vector<YouTubeSong> dataPackage) {
			this->songQueue = dataPackage;
		}

		vector<YouTubeSong>* getQueue() {
			return &this->songQueue;
		}

		bool stop() {
			if (this->voiceConnection->areWeCurrentlyPlaying()) {
				this->areWeStopping = true;
				this->cancelTokenSource.cancel();
				AudioFrameData dataFrame;
				this->currentTask.wait();
				while (try_receive(*this->sendAudioDataBuffer, dataFrame)) {};
				vector<YouTubeSong> newVector02;
				if (this->currentSong.description != "") {
					newVector02.push_back(this->currentSong);
					this->currentSong = YouTubeSong();
				}
				for (auto value : this->songQueue) {
					newVector02.push_back(value);
				}
				this->songQueue = newVector02;
				return true;
			}
			else {
				return false;
			}
		}

		void stopWithoutSaving() {
			if (this->voiceConnection->areWeCurrentlyPlaying()) {
				this->areWeStopping = true;
				this->cancelTokenSource.cancel();
				AudioFrameData dataFrame;
				this->currentTask.wait();
				while (try_receive(*this->sendAudioDataBuffer, dataFrame)) {};
				vector<YouTubeSong> newVector02;
				this->currentSong = YouTubeSong();
				return;
			}
			else {
				return;
			}
		}

		bool isThereAnySongs() {
			if (this->loopAll || this->loopSong) {
				if (this->songQueue.size() == 0 && this->currentSong.songId == "") {
					return false;
				}
				else {
					return true;
				}
			}
			else {
				if (this->songQueue.size() == 0) {
					return false;
				}
				else {
					return true;
				}
			}

		}

		void modifyQueue(int firstSongPosition, int secondSongPosition) {
			YouTubeSong tempSong = this->songQueue.at(firstSongPosition);
			this->songQueue.at(firstSongPosition) = this->songQueue.at(secondSongPosition);
			this->songQueue.at(secondSongPosition) = tempSong;
		}

		YouTubeSong getCurrentSong() {
			if (this->currentSong.videoId != "") {
				return this->currentSong;
			}
			else if (this->songQueue.size() > 0) {
				return this->songQueue.at(0);
			}
			else {
				return YouTubeSong();
			};
		}

		void setCurrentSong(YouTubeSong song){
			this->currentSong = song;
		}

		SendNextSongReturnData sendNextSong() {
			SendNextSongReturnData returnData;
			if (this->loopSong) {
				if (this->songQueue.size() > 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					for (int x = 0; x < this->songQueue.size(); x += 1) {
						if (x == this->songQueue.size() - 1) {
							break;
						}
						this->songQueue[x] = this->songQueue[x + 1];
					}
					this->songQueue.erase(this->songQueue.end() - 1, this->songQueue.end());
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() > 0 && this->currentSong.description != "") {
					this->currentSong = this->currentSong;
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.description != "" && this->songQueue.size() == 0) {
					this->currentSong = this->currentSong;
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() == 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.formatDownloadURL == "") {
					returnData.currentSong = this->currentSong;
					return returnData;
				}
			}
			else if (this->loopAll) {
				if (this->songQueue.size() > 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					for (int x = 0; x < this->songQueue.size(); x += 1) {
						if (x == this->songQueue.size() - 1) {
							break;
						}
						this->songQueue[x] = this->songQueue[x + 1];
					}
					this->songQueue.erase(this->songQueue.end() - 1, this->songQueue.end());
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() > 0 && this->currentSong.description != "") {
					auto tempSong02 = this->currentSong;
					this->currentSong = this->songQueue.at(0);
					for (int x = 0; x < this->songQueue.size(); x += 1) {
						if (x == this->songQueue.size() - 1) {
							break;
						}
						this->songQueue[x] = this->songQueue[x + 1];
					}
					this->songQueue.at(this->songQueue.size() - 1) = tempSong02;
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.description != "" && this->songQueue.size() == 0) {
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() == 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.videoId == "") {
					returnData.currentSong = this->currentSong;
					return returnData;
				}
			}
			else {
				if (this->songQueue.size() > 0 && (this->currentSong.description != "" || this->currentSong.description == "")) {
					this->currentSong = this->songQueue.at(0);
					for (int x = 0; x < this->songQueue.size() - 1; x += 1) {
						this->songQueue[x] = this->songQueue[x + 1];
					}
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					this->songQueue.erase(this->songQueue.end() - 1, this->songQueue.end());
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.description != "" && this->songQueue.size() == 0) {
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					this->currentSong = YouTubeSong();
					return returnData;
				}
				else if (this->songQueue.size() == 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					this->currentTask = create_task(this->downloadAndStreamAudio(this->currentSong, this->cancelToken));
					send(this->readyBuffer, true);
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.videoId == "") {
					returnData.currentSong = this->currentSong;
					return returnData;
				}
			}
			return returnData;
		}

		YouTubeSong addSongToQueue(YouTubeSearchResult searchResult, GuildMember guildMember) {
			string watchHTMLURL = this->baseWatchURL + searchResult.videoId + "&hl=en";
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
			this->html5Player = this->baseURL + resultStringStringHTMLBody.substr(resultStringStringHTMLBody.find("/s/player/"), resultStringStringHTMLBody.find("/player_ias.vflset/en_US/base.js") + to_string(L"/player_ias.vflset/en_US/base.js").length());
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
			song.songId = to_string((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			song.addedByUserName = guildMember.user.userName;
			song.contentLength = (int)format.contentLength;
			song.formatDownloadURL = format.downloadURL;
			song.description = searchResult.description;
			song.imageURL = searchResult.thumbNailURL;
			song.duration = searchResult.duration;
			song.addedById = guildMember.user.id;
			song.title = searchResult.videoTitle;
			song.videoId = searchResult.videoId;
			song.url = searchResult.videoURL;
			bool isItFound = false;
			for (auto value : this->songQueue) {
				if (value.songId == song.songId) {
					isItFound = true;
				}
			}
			this->songQueue.push_back(song);
			return song;
		}

		vector<YouTubeSearchResult> searchForVideo(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseSearchURL;
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
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "YouTubeAPICore::searchForVideo");
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

		task<bool> downloadAndStreamAudioToBeWrapped(YouTubeSong song, cancellation_token token, int retryCountNew = 0) {
			apartment_context mainThread{};
			shared_ptr<DiscordCoreInternal::ThreadContext> threadContext = DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get();
			co_await resume_foreground(*threadContext->dispatcherQueue);
			this->areWeStopping = false;
			BuildSongDecoderData dataPackage{};
			try {
				if (this->sendAudioDataBufferMap->contains(this->guildId)) {
					this->sendAudioDataBuffer = this->sendAudioDataBufferMap->at(this->guildId).get();
				}
				else {
					co_return false;
				}
				string downloadBaseURL;
				if (currentSong.formatDownloadURL.find("https://") != string::npos && currentSong.formatDownloadURL.find("/videoplayback?") != string::npos) {
					downloadBaseURL = currentSong.formatDownloadURL.substr(currentSong.formatDownloadURL.find("https://") + to_string(L"https://").length(), currentSong.formatDownloadURL.find("/videoplayback?") - to_string(L"https://").length());
				}
				// Creates GET request.
				string request = "GET " + currentSong.formatDownloadURL + " HTTP/1.1" + newLine +
					"user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36" + newLine + newLine;
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
				int remainingDownloadContentLength{ this->currentSong.contentLength };
				int contentLengthCurrent{ this->maxBufSize };
				int bytesReadTotal{ 0 };
				int counter{ 0 };
				dataPackage.sendEncodedAudioDataBuffer = new unbounded_buffer<vector<uint8_t>>();
				dataPackage.totalFileSize = this->currentSong.contentLength - 585;
				dataPackage.bufferMaxSize = this->maxBufSize;
				send(dataPackage.sendEncodedAudioDataBuffer, vector<uint8_t>());
				SongDecoder* songDecoder = new SongDecoder(dataPackage, DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get());
				while (this->currentSong.contentLength > bytesReadTotal) {
					if (token.is_canceled()) {
						songDecoder->refreshTimeForBuffer = 10;
						this->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
						agent::wait(songDecoder);
						delete songDecoder;
						songDecoder = nullptr;
						threadContext->releaseGroup();
						cancel_current_task();
						co_return true;
					}
					else {
						int bytesRead{ 0 };
						InMemoryRandomAccessStream outputStream{};
						DataWriter streamDataWriter(outputStream);
						streamDataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
						if (!areWeDoneHeaders) {
							auto bytesReadTask = dataReader.LoadAsync((uint32_t)contentLengthCurrent);
							while (bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
								wait(150);
								if (this->areWeStopping) {
									break;
								}
							}
							if (!this->areWeStopping && bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Canceled) {
								bytesRead = bytesReadTask.get();
							}
							if (!token.is_canceled() && !this->areWeStopping) {
								bytesReadTotal += bytesRead;
								remainingDownloadContentLength -= bytesRead;
								auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
								stringstream bufferStream;
								bufferStream << buffer.data();
								string headers = bufferStream.str();
								int headerLength = (int)headers.find("gvs 1.0") + (int)to_string(L"gvs 1.0").length();
								auto bytesReadTask02 = dataReader.LoadAsync((uint32_t)contentLengthCurrent);
								while (bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
									wait(150);
									if (this->areWeStopping) {
										break;
									}
								}
								if (!this->areWeStopping && bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Canceled) {
									bytesRead = bytesReadTask02.get();
								}
								bytesReadTotal += bytesRead;
								remainingDownloadContentLength -= bytesRead;
								auto buffer02 = dataReader.ReadBuffer((uint32_t)headerLength + 4);
								headers = headers.substr(0, headers.find("gvs 1.0") + to_string(L"gvs 1.0").length());
								streamDataWriter.WriteBuffer(buffer, headerLength + 4, (uint32_t)contentLengthCurrent - 4 - headerLength);
								streamDataWriter.WriteBuffer(buffer02, 0, (uint32_t)headerLength + 4);
								streamDataWriter.StoreAsync().get();
								areWeDoneHeaders = true;
								songDecoder->startMe();
							}
						}
						if (token.is_canceled()) {
							songDecoder->refreshTimeForBuffer = 10;
							this->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
							agent::wait(songDecoder);
							delete songDecoder;
							songDecoder = nullptr;
							threadContext->releaseGroup();
							cancel_current_task();
							co_return true;
						}
						vector<RawFrameData> frames{};
						if (counter == 0) {
							if (!token.is_canceled() && !this->areWeStopping) {
								DataReader streamDataReader(outputStream.GetInputStreamAt(0));
								auto bytesReadTask02 = streamDataReader.LoadAsync((uint32_t)contentLengthCurrent);
								while (bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
									wait(150);
									if (this->areWeStopping) {
										break;
									}
								}
								auto streamBuffer = streamDataReader.ReadBuffer((uint32_t)contentLengthCurrent);
								vector<uint8_t> newVector{};
								for (unsigned int x = 0; x < streamBuffer.Length(); x += 1) {
									newVector.push_back(streamBuffer.data()[x]);
								}
								send(dataPackage.sendEncodedAudioDataBuffer, newVector);
							}
						}
						if (counter > 0) {
							if (contentLengthCurrent > 0) {
								if (token.is_canceled()) {
									songDecoder->refreshTimeForBuffer = 10;
									this->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
									agent::wait(songDecoder);
									delete songDecoder;
									songDecoder = nullptr;
									threadContext->releaseGroup();
									cancel_current_task();
									co_return true;
								}
								if (!token.is_canceled() && !this->areWeStopping) {
									auto bytesReadTask = dataReader.LoadAsync((uint32_t)contentLengthCurrent);
									while (bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
										wait(150);
										if (this->areWeStopping) {
											break;
										}
									}
									if (!this->areWeStopping && bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Canceled) {
										bytesRead = bytesReadTask.get();
									}
									bytesReadTotal += bytesRead;
									remainingDownloadContentLength -= bytesRead;
									if (!token.is_canceled() && !this->areWeStopping) {
										auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
										streamDataWriter.WriteBuffer(buffer);
										streamDataWriter.StoreAsync().get();
										DataReader streamDataReader(outputStream.GetInputStreamAt(0));
										auto bytesReadTask02 = streamDataReader.LoadAsync((uint32_t)contentLengthCurrent);
										while (bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
											wait(150);
											if (this->areWeStopping) {
												break;
											}
										}
										auto streamBuffer = streamDataReader.ReadBuffer((uint32_t)contentLengthCurrent);
										vector<uint8_t> newVector{};
										for (unsigned int x = 0; x < streamBuffer.Length(); x += 1) {
											newVector.push_back(streamBuffer.data()[x]);
										}
										if (!token.is_canceled() && !this->areWeStopping) {
											send(dataPackage.sendEncodedAudioDataBuffer, newVector);
										}
									}
								}
							}
							if (token.is_canceled()) {
								songDecoder->refreshTimeForBuffer = 10;
								this->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
								agent::wait(songDecoder);
								delete songDecoder;
								songDecoder = nullptr;
								threadContext->releaseGroup();
								cancel_current_task();
								co_return true;
							}
							if (!token.is_canceled() && !this->areWeStopping) {
								RawFrameData rawFrame{};
								rawFrame.data.resize(0);
								while (songDecoder->getFrame(&rawFrame)) {
									if (rawFrame.data.size() != 0) {
										frames.push_back(rawFrame);
									}
								}
								if (!token.is_canceled() && !this->areWeStopping) {
									auto encodedFrames = this->songEncoder.encodeFrames(frames);
									for (auto value : encodedFrames) {
										if (this->sendAudioDataBuffer != nullptr && !this->areWeStopping) {
											send(*this->sendAudioDataBuffer, value);
										}
										else {
											break;
										}

									}
								}
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
				}
				if (!token.is_canceled() && !this->areWeStopping) {
					vector<uint8_t> newVector;
					send(dataPackage.sendEncodedAudioDataBuffer, newVector);
					RawFrameData frameData01;
					while (songDecoder->getFrame(&frameData01));
					AudioFrameData frameData02;
					frameData02.encodedFrameData.sampleCount = 0;
					frameData02.rawFrameData.sampleCount = 0;
					frameData02.type = AudioFrameType::Cancel;
					send(*this->sendAudioDataBuffer, frameData02);
				}
				vector<uint8_t> newVector;
				send(dataPackage.sendEncodedAudioDataBuffer, newVector);
				songDecoder->refreshTimeForBuffer = 10;
				agent::wait(songDecoder);
				delete songDecoder;
				songDecoder = nullptr;
				threadContext->releaseGroup();
				co_await mainThread;
				co_return true;
			}
			catch (...) {
				if (retryCountNew < 5) {
					retryCountNew += 1;
					vector<uint8_t> newVector;
					AudioFrameData frameData{};
					frameData.type = AudioFrameType::Cancel;
					send(this->sendAudioDataBuffer, frameData);
					send(dataPackage.sendEncodedAudioDataBuffer, newVector);
					this->currentTask = this->downloadAndStreamAudioToBeWrapped(this->currentSong, token, retryCountNew);
					auto exceptionNew = current_exception();
					try {
						rethrow_exception(exceptionNew);
					}
					catch (exception& e) {
						cout << "YouTubeAPICore::downloadAndStreamAudioToBeWrapped() Error: " << e.what() << endl;
					}
					co_return false;
				}
				else {
					vector<uint8_t> newVector;
					send(dataPackage.sendEncodedAudioDataBuffer, newVector);
					this->stopWithoutSaving();
					this->voiceConnection->onSongCompletionEvent(this->voiceConnection.get());
					retryCountNew = 0;
					auto exceptionNew = current_exception();
					try {
						rethrow_exception(exceptionNew);
					}
					catch (exception& e) {
						cout << "YouTubeAPICore::downloadAndStreamAudioToBeWrapped() Final Error: " << e.what() << endl;
					}
					co_return false;
				}
			}
		};

		void sendEmptyingFrames(unbounded_buffer<vector<uint8_t>>* sendAudioDataBufferNew){
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
		}

		task<bool> downloadAndStreamAudio(YouTubeSong song, cancellation_token token) {
			auto lambda = [&, this](YouTubeSong song) ->task<bool> {
				this->currentTask = create_task(this->downloadAndStreamAudioToBeWrapped(song, token));
				return this->currentTask;
			};
			return lambda(song).then([](task<bool> returnTask)->task<bool> {
				try {
					returnTask.get();
					co_return true;
				}
				catch (...) {
					auto exceptionNew = current_exception();
					try {
						rethrow_exception(exceptionNew);
					}
					catch (exception& e) {
						cout << "YouTubeAPICore::downloadAndStreamAudio() Error: " << e.what() << endl;
					}
					co_return false;
				}
				});
		}

		~YouTubeAPICore() {}

	};

	class YouTubeAPI {
	public:

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>> voiceConnectionMap;
		static map<string, shared_ptr<YouTubeAPICore>>* youtubeAPICoreMap;
		static map<string, DiscordGuild*>* discordGuildMap;

		static void initialize(map<string, shared_ptr<YouTubeAPICore>>* youtubeAPICoreMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMapNew, map<string, DiscordGuild*>* discordGuildMapNew) {
			YouTubeAPI::sendAudioDataBufferMap = sendAudioDataBufferMapNew;
			YouTubeAPI::youtubeAPICoreMap = youtubeAPICoreMapNew;
			YouTubeAPI::discordGuildMap = discordGuildMapNew;
		};

		static bool stop(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				if (!YouTubeAPI::youtubeAPICoreMap->at(guildId)->stop()) {
					return false;
				}
				bool isSongLooped = YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopSongEnabled();
				bool isAllLooped = YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopAllEnabled();
				auto songQueue = *YouTubeAPI::youtubeAPICoreMap->at(guildId)->getQueue();
				YouTubeAPI::youtubeAPICoreMap->erase(guildId);
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				youtubeAPI->setLoopSongStatus(isSongLooped);
				youtubeAPI->setLoopAllStatus(isAllLooped);
				youtubeAPI->setQueue(songQueue);
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return true;
			}
			else {
				return false;
			}
		}

		static bool skip(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				bool isSongLooped = YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopSongEnabled();
				bool isAllLooped = YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopAllEnabled();
				auto currentSong = YouTubeAPI::youtubeAPICoreMap->at(guildId)->getCurrentSong();
				auto songQueue = *YouTubeAPI::youtubeAPICoreMap->at(guildId)->getQueue();
				if (!YouTubeAPI::youtubeAPICoreMap->at(guildId)->stop()) {
					return false;
				}
				YouTubeAPI::youtubeAPICoreMap->erase(guildId);
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				youtubeAPI->setLoopSongStatus(isSongLooped);
				youtubeAPI->setLoopAllStatus(isAllLooped);
				youtubeAPI->currentSong = currentSong;
				youtubeAPI->setQueue(songQueue);
				youtubeAPI->sendNextSong();
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return true;
			}
			else {
				return false;
			}
		}

		static void setLoopAllStatus(bool enabled, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setLoopAllStatus(enabled);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setLoopAllStatus(enabled);
			}
		}

		static bool isLoopAllEnabled(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopAllEnabled();
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopAllEnabled();
			}
		}

		static void setLoopSongStatus(bool enabled, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setLoopSongStatus(enabled);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setLoopSongStatus(enabled);
			}
		}

		static bool isLoopSongEnabled(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopSongEnabled();
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->isLoopSongEnabled();
			}
		}

		static bool isThereAnySongs(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->isThereAnySongs();
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->isThereAnySongs();
			}
		}

		static YouTubeSong addSongToQueue(YouTubeSearchResult searchResult, GuildMember guildMember, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->addSongToQueue(searchResult, guildMember);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return YouTubeSong();
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->addSongToQueue(searchResult, guildMember);
			}
		}

		static void setQueue(vector<YouTubeSong> dataPackage, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setQueue(dataPackage);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setQueue(dataPackage);
			}
		}

		static vector<YouTubeSong>* getQueue(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->getQueue();
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return nullptr;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->getQueue();
			}
		}

		static void modifyQueue(int firstSongPosition, int secondSongPosition, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->modifyQueue(firstSongPosition, secondSongPosition);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->modifyQueue(firstSongPosition, secondSongPosition);
			}

		}

		static YouTubeSong getCurrentSong(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->getCurrentSong();
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return YouTubeSong();
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->getCurrentSong();
			}
		}

		static void setCurrentSong(YouTubeSong song, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setCurrentSong(song);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				YouTubeAPI::youtubeAPICoreMap->at(guildId)->setCurrentSong(song);
			}
		}

		static SendNextSongReturnData sendNextSong(string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->sendNextSong();
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return  SendNextSongReturnData();
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->sendNextSong();
			}
		}

		static vector<YouTubeSearchResult> searchForVideo(string searchQuery, string guildId) {
			if (YouTubeAPI::youtubeAPICoreMap->contains(guildId)) {
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->searchForVideo(searchQuery);
			}
			else {
				shared_ptr<YouTubeAPICore> youtubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = YouTubeAPI::discordGuildMap->at(guildId);
					youtubeAPI = make_shared<YouTubeAPICore>(YouTubeAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, YouTubeAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return vector<YouTubeSearchResult>();
				}
				YouTubeAPI::youtubeAPICoreMap->insert_or_assign(guildId, youtubeAPI);
				return YouTubeAPI::youtubeAPICoreMap->at(guildId)->searchForVideo(searchQuery);
			}
		}
	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* YouTubeAPI::sendAudioDataBufferMap{ nullptr };
	map<string, shared_ptr<YouTubeAPICore>>* YouTubeAPI::youtubeAPICoreMap{ nullptr };
	map<string, shared_ptr<VoiceConnection>> YouTubeAPI::voiceConnectionMap{};
	map<string, DiscordGuild*>* YouTubeAPI::discordGuildMap{};
};
#endif