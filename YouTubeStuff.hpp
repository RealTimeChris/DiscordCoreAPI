// YouTubeStuff.hpp - Header for the "YouTube stuff".
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _YOUTUBE_STUFF_
#define _YOUTUBE_STUFF_

#include "HttpStuff.hpp"
#include "DataParsingFunctions.hpp"

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

	class YouTubeAPI {
	public:

		YouTubeAPI(shared_ptr<unbounded_buffer<vector<RawFrame>*>> sendAudioBufferNew) {
			this->sendAudioBuffer = sendAudioBufferNew;
		}

		vector<YouTubeSong>* getQueue() {
			return &this->songQueue;
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
				cout << "YouTubeAPI::searchForVideo() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
			}
			return searchResults;
		}

		task<void> modifyQueue(int songOldPosition, int songNewPosition) {
			int currentSongCount = (int)this->songQueue.size();
			for (int x = 0; x < currentSongCount; x += 1) {
				YouTubeSong tempSong = this->songQueue.at(songOldPosition);
				if (x < songNewPosition && x > songOldPosition) {
					this->songQueue.at(x - 1) = this->songQueue.at(x);
				}
				this->songQueue.at(songNewPosition) = tempSong;
			}
		}

		Playlist downloadAudio(YouTubeSearchResult videoSearchResult, Playlist playlist) {
			string watchHTMLURL = to_string(this->baseWatchURL) + videoSearchResult.videoId + "&hl=en";
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
			DiscordCoreInternal::DataParser::parseObject(jsonObject, &videoSearchResult.formats);
			YouTubeFormat format;
			for (auto value : videoSearchResult.formats) {
				//if (value.mimeType.find("mp4a.40.2") != string::npos) {
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
			string downloadBaseURL;
			if (format.downloadURL.find("https://") != string::npos && format.downloadURL.find("/videoplayback?") != string::npos) {
				downloadBaseURL = format.downloadURL.substr(format.downloadURL.find("https://") + to_string(L"https://").length(), format.downloadURL.find("/videoplayback?") - to_string(L"https://").length());
			}

			// Creates GET request.
			string request = "GET " + format.downloadURL + " HTTP/1.1" + newLine +
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
			string headers;
			InMemoryRandomAccessStream outputStream;
			DataWriter streamDataWriter(outputStream);
			streamDataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
			this->maxBufSize = format.contentLength;
			if (format.contentLength < this->maxBufSize) {
				this->maxBufSize = format.contentLength;
			}
			__int64 remainingDownloadContentLength = format.contentLength;
			__int64 contentLengthCurrent = this->maxBufSize;
			int bytesReadTotal = 0;
			int bytesRead = 0;
			int bytesWritten = 0;
			string playerId = to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			SongDecoder* songDecoder{ nullptr };
			Playlist dataPackage03 = playlist;
			while (remainingDownloadContentLength > 0) {
				if (contentLengthCurrent > 0) {
					dataReader.LoadAsync((uint32_t)contentLengthCurrent).get();
				}
				auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
				IBuffer streamBuffer;
				stringstream bufferStream;
				bufferStream << buffer.data();
				headers = bufferStream.str();
				int headerLength = (int)headers.find("gvs 1.0") + (int)to_string(L"gvs 1.0").length();
				dataReader.LoadAsync(headerLength).get();
				auto buffer02 = dataReader.ReadBuffer((uint32_t)headerLength);
				headers = headers.substr(0, headers.find("gvs 1.0") + to_string(L"gvs 1.0").length());
				contentLengthCurrent -= headerLength;
				streamDataWriter.WriteBuffer(buffer, headerLength + 4, (uint32_t)contentLengthCurrent - 4);
				bytesWritten += streamDataWriter.StoreAsync().get();
				streamDataWriter.WriteBuffer(buffer02, 0, (uint32_t)headerLength);
				bytesWritten += streamDataWriter.StoreAsync().get();
				vector<uint8_t> vectorNew;
				vectorNew.push_back(0);
				vectorNew.push_back(0);
				vectorNew.push_back(0);
				vectorNew.push_back(0);
				streamDataWriter.WriteBytes(vectorNew);
				bytesWritten += streamDataWriter.StoreAsync().get();
				remainingDownloadContentLength -= bytesWritten;
				DataReader streamDataReader(outputStream.GetInputStreamAt(bytesReadTotal));
				bytesRead += streamDataReader.LoadAsync(bytesWritten).get();
				bytesReadTotal += bytesRead;
				streamBuffer = streamDataReader.ReadBuffer(bytesRead);
				BuildSongDecoderData dataPackage;
				dataPackage.dataBuffer = streamBuffer;
				dataPackage.totalFileSize = format.contentLength;
				songDecoder = new SongDecoder(dataPackage);
				auto frames = songDecoder->getFrames();
				bytesWritten = 0;
				bytesRead = 0;
				YouTubeSong youtubeSong;
				youtubeSong.frames = frames;
				youtubeSong.imageURL = videoSearchResult.thumbNailURL;
				youtubeSong.duration = videoSearchResult.duration;
				youtubeSong.description = videoSearchResult.description;
				youtubeSong.title = videoSearchResult.videoTitle;
				youtubeSong.url = videoSearchResult.videoURL;
				youtubeSong.songId = to_string((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				YouTubeSongDB youtubeSongDB;
				youtubeSongDB.contentLength = (int)format.contentLength;
				youtubeSongDB.description = videoSearchResult.description;
				youtubeSongDB.duration = videoSearchResult.duration;
				youtubeSongDB.formatDownloadURL = format.downloadURL;
				youtubeSongDB.imageURL = videoSearchResult.thumbNailURL;
				youtubeSongDB.title = videoSearchResult.videoTitle;
				youtubeSongDB.url = videoSearchResult.videoURL;
				youtubeSongDB.songId = to_string((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				youtubeSongDB.videoId = videoSearchResult.videoId;
				dataPackage03.songs.push_back(youtubeSongDB);
				bool isItFound = false;
				for (auto value : this->songQueue) {
					if (value.songId == youtubeSongDB.songId) {
						isItFound = true;
						break;
					}
				}
				if (!isItFound) {
					this->songQueue.push_back(youtubeSong);
				}
				if (remainingDownloadContentLength >= this->maxBufSize) {
					contentLengthCurrent = this->maxBufSize;
				}
				else {
					contentLengthCurrent = remainingDownloadContentLength;
				}
			}
			return dataPackage03;
		}
		
		Playlist downloadAudio(YouTubeSongDB dataPackage01, Playlist playlist) {
			string downloadBaseURL;
			string downloadURLIntermed = dataPackage01.formatDownloadURL;
			if (downloadURLIntermed.find("https://") != string::npos && downloadURLIntermed.find("/videoplayback?") != string::npos) {
				downloadBaseURL = downloadURLIntermed.substr(downloadURLIntermed.find("https://") + to_string(L"https://").length(), downloadURLIntermed.find("/videoplayback?") - to_string(L"https://").length());
			}
			
			// Creates GET request.
			string request = "GET " + dataPackage01.formatDownloadURL+ " HTTP/1.1" + newLine +
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
			string headers;
			InMemoryRandomAccessStream outputStream;
			DataWriter streamDataWriter(outputStream);
			streamDataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
			this->maxBufSize = dataPackage01.contentLength;
			if (dataPackage01.contentLength < this->maxBufSize) {
				this->maxBufSize = dataPackage01.contentLength;
			}
			__int64 remainingDownloadContentLength = dataPackage01.contentLength;
			__int64 contentLengthCurrent = this->maxBufSize;
			int bytesReadTotal = 0;
			int bytesRead = 0;
			int bytesWritten = 0;
			string playerId = to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			SongDecoder* songDecoder{ nullptr };
			Playlist dataPackage03 = playlist;
			while (remainingDownloadContentLength > 0) {
				if (contentLengthCurrent > 0) {
					dataReader.LoadAsync((uint32_t)contentLengthCurrent).get();
				}
				auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
				IBuffer streamBuffer;
				stringstream bufferStream;
				bufferStream << buffer.data();
				headers = bufferStream.str();
				int headerLength = (int)headers.find("gvs 1.0") + (int)to_string(L"gvs 1.0").length();
				dataReader.LoadAsync(headerLength).get();
				auto buffer02 = dataReader.ReadBuffer((uint32_t)headerLength);
				headers = headers.substr(0, headers.find("gvs 1.0") + to_string(L"gvs 1.0").length());
				contentLengthCurrent -= headerLength;
				streamDataWriter.WriteBuffer(buffer, headerLength + 4, (uint32_t)contentLengthCurrent - 4);
				bytesWritten += streamDataWriter.StoreAsync().get();
				streamDataWriter.WriteBuffer(buffer02, 0, (uint32_t)headerLength);
				bytesWritten += streamDataWriter.StoreAsync().get();
				vector<uint8_t> vectorNew;
				vectorNew.push_back(0);
				vectorNew.push_back(0);
				vectorNew.push_back(0);
				vectorNew.push_back(0);
				streamDataWriter.WriteBytes(vectorNew);
				bytesWritten += streamDataWriter.StoreAsync().get();
				remainingDownloadContentLength -= bytesWritten;
				DataReader streamDataReader(outputStream.GetInputStreamAt(bytesReadTotal));
				bytesRead += streamDataReader.LoadAsync(bytesWritten).get();
				bytesReadTotal += bytesRead;
				streamBuffer = streamDataReader.ReadBuffer(bytesRead);
				BuildSongDecoderData dataPackage;
				dataPackage.dataBuffer = streamBuffer;
				dataPackage.totalFileSize = dataPackage01.contentLength;
				songDecoder = new SongDecoder(dataPackage);
				auto frames = songDecoder->getFrames();
				bytesWritten = 0;
				bytesRead = 0;
				YouTubeSong youtubeSong;
				youtubeSong.frames = frames;
				youtubeSong.imageURL = dataPackage01.imageURL;
				youtubeSong.duration = dataPackage01.duration;
				youtubeSong.description = dataPackage01.description;
				youtubeSong.title = dataPackage01.title;
				youtubeSong.songId = dataPackage01.songId;
				youtubeSong.url = dataPackage01.url;
				YouTubeSongDB youtubeSongDB;
				youtubeSongDB.contentLength = (int)dataPackage01.contentLength;
				youtubeSongDB.description = dataPackage01.description;
				youtubeSongDB.duration = dataPackage01.duration;
				youtubeSongDB.formatDownloadURL = dataPackage01.formatDownloadURL;
				youtubeSongDB.imageURL = dataPackage01.imageURL;
				youtubeSongDB.title = dataPackage01.title;
				youtubeSongDB.songId = dataPackage01.songId;
				youtubeSongDB.url = dataPackage01.url;
				youtubeSongDB.videoId = dataPackage01.videoId;
				bool isItFound = false;
				for (int x = 0; x < dataPackage03.songs.size();x+=1) {
					if (dataPackage01.songId== dataPackage03.songs[x].songId) {
						isItFound = true;
						break;
					}
				}
				if (!isItFound) {
					dataPackage03.songs.push_back(youtubeSongDB);
				}
				isItFound = false;
				for (auto value : this->songQueue) {
					if (value.songId == youtubeSongDB.songId) {
						isItFound = true;
						break;
					}
				}
				if (!isItFound) {
					this->songQueue.push_back(youtubeSong);
				}
				if (remainingDownloadContentLength >= this->maxBufSize) {
					contentLengthCurrent = this->maxBufSize;
				}
				else {
					contentLengthCurrent = remainingDownloadContentLength;
				}
			}
			return dataPackage03;
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
			}
		}

		Playlist stopPlaying(Playlist dataPackage) {
			vector<YouTubeSong> newVector;
			newVector.push_back(this->currentSong);
			for (auto value : this->songQueue) {
				newVector.push_back(value);
			}
			this->songQueue = newVector;
			this->currentSong = YouTubeSong();
			vector<YouTubeSongDB> newVector02;
			newVector02.push_back(dataPackage.currentSong);
			for (auto value : dataPackage.songs) {
				newVector02.push_back(value);
			}
			Playlist playlist;
			playlist.songs = newVector02;
			playlist.currentSong = YouTubeSongDB();
			playlist.loopAll = dataPackage.loopAll;
			playlist.loopSong = dataPackage.loopSong;
			playlist.textChannelId = dataPackage.textChannelId;
			playlist.voiceChannelId = dataPackage.voiceChannelId;
			return playlist;
		}

		SendNextSongReturnData sendNextSong(Playlist* dataPackage) {
			SendNextSongReturnData returnData;
			if (this->currentSong.videoId == "") {
				for (auto value : this->songQueue) {
					if (value.videoId == dataPackage->currentSong.videoId) {
						this->currentSong = value;
					}
				}
			}
			if (dataPackage->loopSong) {
				if (dataPackage->currentSong.videoId != "") {
					dataPackage->currentSong = dataPackage->currentSong;
					this->currentSong = this->currentSong;
					vector<RawFrame>* frames = &this->currentSong.frames;
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else if (dataPackage->songs.size() > 0) {
					dataPackage->currentSong = dataPackage->songs.at(0);
					dataPackage->songs.erase(dataPackage->songs.begin(), dataPackage->songs.begin() + 1);
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					vector<RawFrame>* frames = &this->currentSong.frames;
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else {
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = false;
					return returnData;
				}
			}
			else if (dataPackage->loopAll) {
				if (this->currentSong.videoId != "" && dataPackage->songs.size() > 0 && this->songQueue.size() > 0) {
					auto tempSong02 = dataPackage->currentSong;
					auto tempSong = this->currentSong;
					this->currentSong = this->songQueue.at(0);
					for (int x = 0; x < this->songQueue.size(); x += 1) {
						if (x == this->songQueue.size() - 1) {
							break;
						}
						this->songQueue[x] = this->songQueue[x + 1];
					}
					this->songQueue.at(this->songQueue.size() - 1) = tempSong;
					for (int x = 0; x < dataPackage->songs.size(); x += 1) {
						if (x == dataPackage->songs.size() - 1) {
							break;
						}
						dataPackage->songs[x] = dataPackage->songs[x + 1];
					}
					dataPackage->songs.at(dataPackage->songs.size() - 1) = tempSong02;
					dataPackage->currentSong = this->currentSong;
					vector<RawFrame>* frames = &this->currentSong.frames;
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else if (dataPackage->currentSong.videoId == "" && dataPackage->songs.size() > 0) {
					dataPackage->currentSong = dataPackage->songs.at(0);
					dataPackage->songs.erase(dataPackage->songs.begin(), dataPackage->songs.begin() + 1);
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					dataPackage->currentSong = this->currentSong;
					vector<RawFrame>* frames = &this->currentSong.frames;
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else if (dataPackage->songs.size() == 0) {
					this->currentSong = this->currentSong;
					dataPackage->currentSong = this->currentSong;
					vector<RawFrame>* frames = &this->currentSong.frames;
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else {
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = false;
					return returnData;
				}
			}
			else {
				if (dataPackage->songs.size() > 1 && this->songQueue.size() > 1) {
					dataPackage->currentSong = dataPackage->songs[0];
					this->currentSong = this->songQueue[0];
					vector<RawFrame>* frames = &this->songQueue[0].frames;
					for (int x = 0; x < this->songQueue.size(); x += 1) {
						if (x == this->songQueue.size() - 1) {
							break;
						}
						this->songQueue[x] = this->songQueue[x+1];
					}
					for (int x = 0; x < this->songQueue.size(); x += 1) {
						if (x == this->songQueue.size() - 1) {
							break;
						}
						dataPackage->songs[x] = dataPackage->songs[x + 1];
					}
					dataPackage->songs.erase(dataPackage->songs.begin() + dataPackage->songs.size() - 1);
					this->songQueue.erase(this->songQueue.begin() + this->songQueue.size() - 1);
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else if (dataPackage->songs.size() > 0 && this->songQueue.size() > 0) {
					dataPackage->currentSong = dataPackage->songs[0];
					this->currentSong = this->songQueue[0];
					dataPackage->songs.erase(dataPackage->songs.begin());
					this->songQueue.erase(this->songQueue.begin());
					vector<RawFrame>* frames = &this->currentSong.frames;
					send(*this->sendAudioBuffer, frames);
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = true;
					return returnData;
				}
				else {
					returnData.dataPackage = *dataPackage;
					returnData.didItSend = false;
					return returnData;
				}
			}
		}

	protected:
		shared_ptr<unbounded_buffer<vector<RawFrame>*>> sendAudioBuffer;
		const hstring baseSearchURL = L"https://www.youtube.com/results?search_query=";
		const hstring baseWatchURL = L"https://www.youtube.com/watch?v=";
		const hstring baseURL = L"https://www.youtube.com";
		__int64 maxBufSize = 4096;
		const string newLine = "\n\r";
		vector<YouTubeSong> songQueue;
		YouTubeSong currentSong;
		string playerResponse = "";
		string html5PlayerFile = "";
		string html5Player = "";
	};
};
#endif