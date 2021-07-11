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

	template <typename type>
	vector<typename type> sliceVector(vector<typename type> vectorToSlice, unsigned int firstElement,unsigned int lastElement = 0) {
		vector<type> newVector;
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

	vector<char> splitString(string stringToSplit) {
		vector<char> charVector;
		for (auto value : stringToSplit) {
			charVector.push_back(value);
		};
		return charVector;
	}

	string joinString(vector<char> stringToJoin) {
		string newString;
		for (unsigned int x = 0; x < stringToJoin.size(); x+=1) {
			newString += stringToJoin[x];
		}
		return newString;
	}

	vector<char> swapHeadAndPosition(vector<char> inputVector, int position) {
		char first = inputVector[0];
		inputVector[0] = inputVector[position % inputVector.size()];
		inputVector[position] = first;
		return inputVector;
	};

	string setDownloadURL(YouTubeFormat format) {
		string downloadURL = format.downloadURL;
		CURLU* urlHandle = curl_url();
		int* outLength{ nullptr };
		char* charString = curl_easy_unescape(urlHandle, downloadURL.c_str(), (int)downloadURL.length(), outLength);
		downloadURL = charString;
		curl_free(charString);
		downloadURL += "&ratebypass=yes";
		if (format.signature.find("D3%") != string::npos) {
			format.signature = format.signature.substr(0, format.signature.find("D3%"));
		}
		downloadURL += "&sig=" + format.signature;
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
			reverseKey = regex_replace(matchResultsNew01[1].str(), regex("\$/g,'\\$"), "");
			reverseKey = regex_replace(reverseKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}
		smatch matchResultsNew02;
		string sliceKey;
		regex_search(objectBody, matchResultsNew02, sliceRegexp);
		if (!matchResultsNew02.empty() && matchResultsNew02.size() > 0) {
			sliceKey = regex_replace(matchResultsNew02[1].str(), regex("\$/g,'\\$"), "");
			sliceKey = regex_replace(sliceKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}
		smatch matchResultsNew03;
		string spliceKey;
		regex_search(objectBody, matchResultsNew03, spliceRegexp);
		if (!matchResultsNew03.empty() && matchResultsNew03.size() > 0) {
			spliceKey = regex_replace(matchResultsNew03[1].str(), regex("\$/g,'\\$"), "");
			spliceKey = regex_replace(spliceKey, regex("\\$|^'|^\"|'$|\"$"), "");
		}
		smatch matchResultsNew04;
		string swapKey;
		regex_search(objectBody, matchResultsNew04, swapRegexp);
		if (!matchResultsNew04.empty() && matchResultsNew04.size() > 0) {
			swapKey= regex_replace(matchResultsNew04[1].str(), regex("\$/g,'\\$"), "");
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

		for (unsigned int x = 0, len = (unsigned int)tokens.size(); x < len; x+=1) {
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

	YouTubeFormat decipherFormat(YouTubeFormat format, string html5playerFile){
		YouTubeFormat decipheredFormat = format;
		vector<string> tokens = getTokens(html5playerFile);
		if (decipheredFormat.signature.length() >= to_string(L"s=").length()) {
			decipheredFormat.signature = decipheredFormat.signature.substr(to_string(L"s=").length());
		}
		else {
			cout << "Nope you do not have the requirements for doing this!" << endl;
			return decipheredFormat;
		}
		decipheredFormat.signature = decipher(tokens, decipheredFormat.signature);
		decipheredFormat.downloadURL = setDownloadURL(decipheredFormat);
		cout << "URL FINAL: " << decipheredFormat.downloadURL << endl;
		return decipheredFormat;
	};

	class YouTubeAPI {
	public:

		const hstring baseSearchURL = L"https://www.youtube.com/results?search_query=";
		const hstring baseWatchURL = L"https://www.youtube.com/watch?v=";
		const hstring baseURL = L"https://www.youtube.com";
		int maxBufSize = 4096;
		const int downloadChunkSize = 1024 * 1024 * 10;
		const string newLine = "\n\r";
		string playerResponse = "";
		string html5PlayerFile = "";
		string html5Player = "";

		vector<YouTubeSearchResult> searchForVideo(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = baseSearchURL;
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
			json partialSearchResultsJson = json::parse(to_string(returnData.data));
			for (auto value : partialSearchResultsJson.at("contents").at("twoColumnSearchResultsRenderer").at("primaryContents").at("sectionListRenderer").at("contents").at(0).at("itemSectionRenderer").at("contents")){
				DiscordCoreAPI::YouTubeSearchResult searchResult;
				if (value.contains("videoRenderer")) {
					DiscordCoreInternal::parseObject(value.at("videoRenderer"), &searchResult);
					searchResults.push_back(searchResult);
				}
			}
			if (returnData.returnCode != 200) {
				cout << "YouTubeAPI::searchForVideo() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
			}
			return searchResults;
		}

		task<IBuffer> downloadVideo(YouTubeSearchResult videoSearchResult) {
			apartment_context mainThread;
			co_await resume_background();
			regex html5PlayerRegex("/s/player/\\S{1,12}/player_ias.vflset/en_US/base.js");
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
			requestMessageHTMLBody.RequestUri(Uri(to_hstring(watchHTMLURL)));
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
			DiscordCoreInternal::parseObject(jsonObject, &videoSearchResult.formats);
			YouTubeFormat format;
			for (auto value : videoSearchResult.formats) {
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
			requestPlayerFile.RequestUri(Uri(to_hstring(this->html5Player)));
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
				"range: bytes=" + to_string(0) + "-" + to_string(this->downloadChunkSize) + newLine +
				"accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,video/mp4,*//*;q=0.8,application/signed-exchange;v=b3;q=0.9" + newLine +
				"origin: https://www.youtube.com" + newLine +
				"accept-encoding: gzip, deflate" + newLine +
				"content-type: application/json" + newLine +
				"accept-language: en-US,en;q=0.9" + newLine +
				"cache-control: no-cache" + newLine +
				"pragma: no-cache" + newLine +
				"sec-ch-ua: \" Not;A Brand\";v=\"99\",\"Google Chrome\";v=\"91\",\"Chromium\";v=\"91\"" + newLine +
				"referer: " + "https://www.youtube.com" + newLine +
				"sec-ch-ua-mobile: ?0" + newLine +
				"sec-fetch-dest: empty" + newLine +
				"sec-fetch-mode: cors" + newLine +
				"sec-fetch-site: cross-site" + newLine +
				"sec-fetch-user: ?1" + newLine +
				"service-worker-navigation-preload: true" + newLine +
				"upgrade-insecure-requests: 1" + newLine +
				"user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
				+ newLine + newLine;

			StreamSocket streamSocket;
			StreamSocketControl streamSocketControl = streamSocket.Control();
			streamSocketControl.MinProtectionLevel(SocketProtectionLevel::Tls12);
			streamSocketControl.SerializeConnectionAttempts(true);
			streamSocketControl.QualityOfService(SocketQualityOfService::LowLatency);
			Windows::Networking::HostName hostName(to_hstring(downloadBaseURL));
			auto endpointPair = streamSocket.GetEndpointPairsAsync(hostName, L"443").get();
			streamSocket.ConnectAsync(endpointPair.First().Current()).get();
			streamSocket.UpgradeToSslAsync(SocketProtectionLevel::Tls12, hostName).get();
			auto inputStream = streamSocket.InputStream();
			auto outputStream = streamSocket.OutputStream();
			DataWriter dataWriter(outputStream);
			DataReader dataReader(inputStream);
			dataReader.InputStreamOptions(InputStreamOptions::None);
			dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
			dataWriter.WriteString(to_hstring(request));
			dataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
			dataWriter.StoreAsync().get();
			string headers;
			string downloadStreamRaw;
			InMemoryRandomAccessStream finalFileOutput;
			DataWriter dataWriterOutput(finalFileOutput);
			dataWriterOutput.UnicodeEncoding(UnicodeEncoding::Utf8);
			bool headersFinished = false;
			if (format.contentLength < this->maxBufSize) {
				this->maxBufSize = format.contentLength;
			}
			dataReader.LoadAsync(this->maxBufSize).get();
			int remainingDownloadContentLength = format.contentLength;
			int contentLengthCurrent = dataReader.UnconsumedBufferLength();
			int headerLength = 0;
			while (remainingDownloadContentLength > 0) {
				auto buffer = dataReader.ReadBuffer(contentLengthCurrent);
				stringstream stringStream;
				stringStream << buffer.data();
				downloadStreamRaw = stringStream.str();
				if (!headersFinished) {
					headers = downloadStreamRaw.substr(0, downloadStreamRaw.find("gvs 1.0") + to_string(L"gvs 1.0").length());
					headerLength = (int)downloadStreamRaw.find("gvs 1.0") + (int)to_string(L"gvs 1.0").length();
					downloadStreamRaw = downloadStreamRaw.substr(headerLength);
					headersFinished = true;
					remainingDownloadContentLength -= contentLengthCurrent - headerLength;
					dataWriterOutput.WriteBuffer(buffer, headerLength + 4, (uint32_t)buffer.Length() - headerLength - 4);
				}
				else {
					dataWriterOutput.WriteBuffer(buffer, 0, (uint32_t)buffer.Length());
					remainingDownloadContentLength -= contentLengthCurrent;
				}

				dataWriterOutput.StoreAsync().get();

				if (remainingDownloadContentLength >= this->maxBufSize) {
					contentLengthCurrent = this->maxBufSize;
				}
				else {
					contentLengthCurrent = remainingDownloadContentLength;
				}
				if (contentLengthCurrent > 0) {
					dataReader.LoadAsync(contentLengthCurrent).get();
				}
			}
			dataWriterOutput.StoreAsync().get();
			DataReader dataReader00(finalFileOutput.GetInputStreamAt(0));
			dataReader00.UnicodeEncoding(UnicodeEncoding::Utf8);
			dataReader00.LoadAsync((uint32_t)finalFileOutput.Size()).get();
			auto readBuffer = dataReader00.ReadBuffer((uint32_t)finalFileOutput.Size());
			winrt::Windows::Storage::StorageFile storageFile(nullptr);
			auto fileToOutput = storageFile.GetFileFromPathAsync(L"C:\\Users\\Chris\\Downloads\\test.weba").get();
			winrt::Windows::Storage::FileIO::WriteBufferAsync(fileToOutput, readBuffer).get();
			co_await mainThread;
			co_return readBuffer;
		}
	};
}
#endif
