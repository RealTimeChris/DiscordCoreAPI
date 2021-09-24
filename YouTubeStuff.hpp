// YouTubeStuff.hpp - Header for the "YouTube stuff".
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _YOUTUBE_STUFF_
#define _YOUTUBE_STUFF_

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

	YouTubeSong::YouTubeSong() {};

	YouTubeSong YouTubeSong::collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong) {
		newSong.firstDownloadURL = YouTubeSong::baseWatchURL + newSong.songId + "&hl=en";
		auto newerSong = newSong.collectDownloadInfo(addedByGuildMember, newSong);
		newerSong.addedByUserId = addedByGuildMember.user.id;
		newerSong.addedByUserName = addedByGuildMember.user.userName;
		return newerSong;
	}

	vector<YouTubeSong> YouTubeSong::searchForSong(string searchQuery) {
		DiscordCoreInternal::HttpAgentResources agentResources;
		agentResources.baseURL = YouTubeSong::baseSearchURL;
		DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
		DiscordCoreInternal::HttpWorkloadData workload;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::YOUTUBE_SEARCH;
		CURLU* urlHandle = curl_url();
		curl_url_set(urlHandle, CURLUPart::CURLUPART_QUERY, searchQuery.c_str(), CURLU_URLENCODE);
		char* charString{ nullptr };
		curl_url_get(urlHandle, CURLUPart::CURLUPART_QUERY, &charString, 0);
		workload.relativePath = charString;
		curl_free(charString);
		DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "YouTubeAPI::searchForSong");
		vector<YouTubeSong> searchResults;
		json partialSearchResultsJson = returnData.data;
		for (auto value : partialSearchResultsJson.at("contents").at("twoColumnSearchResultsRenderer").at("primaryContents").at("sectionListRenderer").at("contents").at(0).at("itemSectionRenderer").at("contents")) {
			YouTubeSong searchResult{};
			if (value.contains("videoRenderer")) {
				DiscordCoreInternal::DataParser::parseObject(value.at("videoRenderer"), &searchResult);
				searchResult.type = SongType::YouTube;
				searchResult.firstDownloadURL = YouTubeSong::baseWatchURL + searchResult.songId + "&hl=en";
				searchResult.viewURL = searchResult.firstDownloadURL;
				searchResults.push_back(searchResult);
			}
		}
		if (returnData.returnCode != 200) {
			cout << "this::searchForSong() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
		}
		return searchResults;
	}

	YouTubeSong YouTubeSong::collectDownloadInfo(GuildMemberData guildMember, YouTubeSong newSong) {
		string watchHTMLURL = newSong.firstDownloadURL;
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
		auto resultHTMLBody = httpClientGetHTMLBody.SendRequestAsync(requestMessageHTMLBody).get();
		hstring resultStringHTMLBody = resultHTMLBody.Content().ReadAsStringAsync().get();
		string resultStringStringHTMLBody = to_string(resultStringHTMLBody);
		newSong.html5Player = YouTubeSong::baseURL + resultStringStringHTMLBody.substr(resultStringStringHTMLBody.find("/s/player/"), resultStringStringHTMLBody.find("/player_ias.vflset/en_US/base.js") + to_string(L"/player_ias.vflset/en_US/base.js").length());
		newSong.html5Player = newSong.html5Player.substr(0, 73);
		newSong.playerResponse = between(to_string(resultStringHTMLBody), "ytInitialPlayerResponse = ", "</script>");
		newSong.playerResponse = newSong.playerResponse.substr(0, newSong.playerResponse.length() - 170);
		json jsonObject;
		if (newSong.playerResponse != "") {
			jsonObject = json::parse(newSong.playerResponse);
		}
		DiscordCoreInternal::DataParser::parseObject(jsonObject, &newSong.formats);
		YouTubeFormat format;
		for (auto value : newSong.formats) {
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
		requestPlayerFile.RequestUri(winrt::Windows::Foundation::Uri(to_hstring(newSong.html5Player)));
		auto responseMessage02 = httpClientGetHTMLBody.SendRequestAsync(requestPlayerFile).get();
		hstring responseToPlayerGet02 = responseMessage02.Content().ReadAsStringAsync().get();
		newSong.html5PlayerFile = to_string(responseToPlayerGet02);
		format = decipherFormat(format, newSong.html5PlayerFile);
		DownloadURL downloadURL{ .urlPath = format.downloadURL,.contentSize = newSong.contentLength };
		newSong.viewURL = watchHTMLURL;
		newSong.addedByUserName = guildMember.user.userName;
		newSong.contentLength = (int)format.contentLength;
		newSong.finalDownloadURLs.push_back(downloadURL);
		newSong.addedByUserId = guildMember.user.id;
		newSong.type = SongType::YouTube;
		return newSong;
	}

	class YouTubeAPI : public implements<YouTubeAPI, IInspectable> {
	public:

		template <class _Ty>
		friend 	_CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreClient;
		friend class YouTubeSong;
		friend class SongAPI;
		friend class Guild;

		YouTubeAPI() {};

		YouTubeAPI(string guildIdNew) {
			this->sendAudioDataBuffer = YouTubeAPI::sendAudioDataBufferMap->at(guildIdNew);
			this->voiceConnection = YouTubeAPI::voiceConnectionMap->at(guildIdNew);
			this->discordGuild = YouTubeAPI::discordGuildMap->at(guildIdNew);
			this->guildId = guildIdNew;
		}

		static void initialize(map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMapNew, map<string, DiscordGuild*>* discordGuildMapNew, map<string, shared_ptr<VoiceConnection>>* voiceConnectionMapNew) {
			YouTubeAPI::sendAudioDataBufferMap = sendAudioDataBufferMapNew;
			YouTubeAPI::voiceConnectionMap = voiceConnectionMapNew;
			YouTubeAPI::discordGuildMap = discordGuildMapNew;
			YouTubeAPI::youtubeAPIMap = youtubeAPIMapNew;
		};

	protected:

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMap;
		static map<string, DiscordGuild*>* discordGuildMap;
		static string baseSearchURL;
		static string baseWatchURL;
		static string clientId;

		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		cancellation_token_source cancelTokenSource{ cancellation_token_source() };
		cancellation_token cancelToken{ this->cancelTokenSource.get_token() };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		DiscordGuild* discordGuild{ new DiscordGuild };		
		concurrency::event readyToBeDoneEvent {};
		string appVersion{ "1631696495" };
		const string newLine{ "\n\r" };
		const int maxBufSize{ 4096 };
		bool areWeStopping{ false };
		task<void>* currentTask{};
		YouTubeSong theSong{};
		string guildId{ "" };

		bool stop() {
			if (this->voiceConnection->areWeCurrentlyPlaying()) {
				this->areWeStopping = true;
				this->cancelTokenSource.cancel();
				if (this->currentTask != nullptr && !this->currentTask->is_done()) {
					this->currentTask->then([](task<void> previousTask)->task<void> {
						try {
							previousTask.get();
							co_return;
						}
						catch (...) {
							rethrowException("YouTubeAPI::stop() Error: ");
							co_return;
						}
						}).get();
						this->currentTask = nullptr;
				}
				AudioFrameData dataFrame;
				while (try_receive(this->sendAudioDataBuffer.get(), dataFrame)) {};
				this->readyToBeDoneEvent.wait(2000);
				this->readyToBeDoneEvent.reset();
				return true;
			}
			else {
				return false;
			}
		}

		void sendNextSong(Song newSong) {
			this->cancelTokenSource = cancellation_token_source();
			this->cancelToken = this->cancelTokenSource.get_token();
			this->downloadAndStreamAudioWrapper(newSong, 0);
			return;
		}

		HRESULT GetRuntimeClassName(HSTRING*) {
			return HRESULT();
		}

		HRESULT GetTrustLevel(TrustLevel*) {
			return HRESULT();
		}

		task<void> downloadAndStreamAudioWrapper(YouTubeSong song, int retryCount = 0) {
			shared_ptr<DiscordCoreInternal::ThreadContext> threadContext = DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get();
			apartment_context mainThread{};
			co_await resume_foreground(*threadContext->dispatcherQueue);
			this->currentTask = new task<void>(create_task([=, strong_this{ get_strong() }]()->void {
				YouTubeAPI* thisPtr = strong_this.get();
				auto tokenNew = thisPtr->cancelTokenSource.get_token();
				auto newSong = song;
				thisPtr->areWeStopping = false;
				BuildSongDecoderData dataPackage{};;
				if (thisPtr->sendAudioDataBufferMap->contains(thisPtr->guildId)) {
					thisPtr->sendAudioDataBuffer = thisPtr->sendAudioDataBufferMap->at(thisPtr->guildId);
				}
				else {
					return;
				}
				string downloadBaseURL;
				if (newSong.finalDownloadURLs.at(0).urlPath.find("https://") != string::npos && newSong.finalDownloadURLs.at(0).urlPath.find("/videoplayback?") != string::npos) {
					downloadBaseURL = newSong.finalDownloadURLs.at(0).urlPath.substr(newSong.finalDownloadURLs.at(0).urlPath.find("https://") + to_string(L"https://").length(), newSong.finalDownloadURLs.at(0).urlPath.find("/videoplayback?") - to_string(L"https://").length());
				}
				// Creates GET request.
				string request = "GET " + newSong.finalDownloadURLs.at(0).urlPath + " HTTP/1.1" + thisPtr->newLine +
					"user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36" + thisPtr->newLine + thisPtr->newLine;
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
				int remainingDownloadContentLength{ newSong.contentLength };
				int contentLengthCurrent{ thisPtr->maxBufSize };
				int bytesReadTotal{ 0 };
				int counter{ 0 };
				dataPackage.sendEncodedAudioDataBuffer = new unbounded_buffer<vector<uint8_t>>();
				dataPackage.totalFileSize = newSong.contentLength - 585;
				dataPackage.bufferMaxSize = thisPtr->maxBufSize;
				SongDecoder* songDecoder = new SongDecoder(dataPackage);
				SongEncoder* songEncoder = new SongEncoder();
				send(dataPackage.sendEncodedAudioDataBuffer, vector<uint8_t>());
				while (newSong.contentLength > bytesReadTotal) {
					if (tokenNew.is_canceled()) {
						songDecoder->refreshTimeForBuffer = 10;
						thisPtr->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
						agent::wait(songDecoder);
						delete songDecoder;
						songDecoder = nullptr;
						thisPtr->readyToBeDoneEvent.set();
						threadContext->releaseGroup();
						cancel_current_task();
						return;
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
								if (thisPtr->areWeStopping) {
									break;
								}
							}
							if (!thisPtr->areWeStopping && bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Canceled) {
								bytesRead = bytesReadTask.get();
							}
							if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
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
									if (thisPtr->areWeStopping) {
										break;
									}
								}
								if (!thisPtr->areWeStopping && bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Canceled) {
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
						if (tokenNew.is_canceled()) {
							songDecoder->refreshTimeForBuffer = 10;
							thisPtr->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
							agent::wait(songDecoder);
							delete songDecoder;
							songDecoder = nullptr;
							thisPtr->readyToBeDoneEvent.set();
							threadContext->releaseGroup();
							cancel_current_task();
							return;
						}
						vector<RawFrameData> frames{};
						if (counter == 0) {
							if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
								DataReader streamDataReader(outputStream.GetInputStreamAt(0));
								auto bytesReadTask02 = streamDataReader.LoadAsync((uint32_t)contentLengthCurrent);
								while (bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
									wait(150);
									if (thisPtr->areWeStopping) {
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
								if (tokenNew.is_canceled()) {
									songDecoder->refreshTimeForBuffer = 10;
									thisPtr->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
									agent::wait(songDecoder);
									delete songDecoder;
									songDecoder = nullptr;
									thisPtr->readyToBeDoneEvent.set();
									threadContext->releaseGroup();
									cancel_current_task();
									return;
								}
								if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
									auto bytesReadTask = dataReader.LoadAsync((uint32_t)contentLengthCurrent);
									while (bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
										wait(150);
										if (thisPtr->areWeStopping) {
											break;
										}
									}
									if (!thisPtr->areWeStopping && bytesReadTask.Status() != winrt::Windows::Foundation::AsyncStatus::Canceled) {
										bytesRead = bytesReadTask.get();
									}
									bytesReadTotal += bytesRead;
									remainingDownloadContentLength -= bytesRead;
									if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
										auto buffer = dataReader.ReadBuffer((uint32_t)contentLengthCurrent);
										streamDataWriter.WriteBuffer(buffer);
										streamDataWriter.StoreAsync().get();
										DataReader streamDataReader(outputStream.GetInputStreamAt(0));
										auto bytesReadTask02 = streamDataReader.LoadAsync((uint32_t)contentLengthCurrent);
										while (bytesReadTask02.Status() != winrt::Windows::Foundation::AsyncStatus::Completed) {
											wait(150);
											if (thisPtr->areWeStopping) {
												break;
											}
										}
										auto streamBuffer = streamDataReader.ReadBuffer((uint32_t)contentLengthCurrent);
										vector<uint8_t> newVector{};
										for (unsigned int x = 0; x < streamBuffer.Length(); x += 1) {
											newVector.push_back(streamBuffer.data()[x]);
										}
										if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
											send(dataPackage.sendEncodedAudioDataBuffer, newVector);
										}
									}
								}
							}
							if (tokenNew.is_canceled()) {
								songDecoder->refreshTimeForBuffer = 10;
								thisPtr->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
								agent::wait(songDecoder);
								delete songDecoder;
								songDecoder = nullptr;
								thisPtr->readyToBeDoneEvent.set();
								threadContext->releaseGroup();
								cancel_current_task();
								return;
							}
							if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
								RawFrameData rawFrame{};
								rawFrame.data.resize(0);
								while (songDecoder->getFrame(&rawFrame)) {
									if (rawFrame.data.size() != 0) {
										frames.push_back(rawFrame);
									}
								}
								if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
									auto encodedFrames = songEncoder->encodeFrames(frames);
									for (auto value : encodedFrames) {
										if (!thisPtr->areWeStopping && !tokenNew.is_canceled()) {
											send(thisPtr->sendAudioDataBuffer.get(), value);
										}
										else {
											break;
										}

									}
								}
							}
						}

						if (remainingDownloadContentLength >= thisPtr->maxBufSize) {
							contentLengthCurrent = thisPtr->maxBufSize;
						}
						else {
							contentLengthCurrent = remainingDownloadContentLength;
						}
						counter += 1;
					}
				}
				if (!tokenNew.is_canceled() && !thisPtr->areWeStopping) {
					vector<uint8_t> newVector{};
					send(dataPackage.sendEncodedAudioDataBuffer, newVector);
					RawFrameData frameData01{};
					while (songDecoder->getFrame(&frameData01)) {};
					AudioFrameData frameData{};
					frameData.type = AudioFrameType::Cancel;
					frameData.rawFrameData.sampleCount = 0;
					frameData.encodedFrameData.sampleCount = 0;
					send(thisPtr->sendAudioDataBuffer.get(), frameData);
				}
				vector<uint8_t> newVector;
				send(dataPackage.sendEncodedAudioDataBuffer, newVector);
				songDecoder->refreshTimeForBuffer = 10;
				agent::wait(songDecoder);
				delete songDecoder;
				songDecoder = nullptr;
				thisPtr->readyToBeDoneEvent.set();
				threadContext->releaseGroup();
				return;
			}, this->cancelToken));
		};

		void sendEmptyingFrames(unbounded_buffer<vector<uint8_t>>* sendAudioDataBufferNew) {
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
		}

		static bool stop(string guildId) {
			if (!YouTubeAPI::youtubeAPIMap->at(guildId)->stop()) {
				return false;
			}
			YouTubeAPI::youtubeAPIMap->erase(guildId);
			shared_ptr<YouTubeAPI> youTubeAPI = make_shared<YouTubeAPI>(guildId);
			YouTubeAPI::youtubeAPIMap->insert_or_assign(guildId, youTubeAPI);
			return true;
		}

		static void sendNextSong(Song newSong, string guildId) {
			if (YouTubeAPI::youtubeAPIMap->contains(guildId)) {
				YouTubeAPI::youtubeAPIMap->at(guildId)->sendNextSong(newSong);
				return;

			}
			else {
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					shared_ptr<YouTubeAPI> youTubeAPI = make_shared<YouTubeAPI>(guildId);
					YouTubeAPI::youtubeAPIMap->insert_or_assign(guildId, youTubeAPI);
					YouTubeAPI::youtubeAPIMap->at(guildId)->sendNextSong(newSong, guildId);
					return;
				}
				else {
					return;
				}
			}
		}

		static vector<YouTubeSong> searchForSong(string searchQuery, string guildId) {
			if (YouTubeAPI::youtubeAPIMap->contains(guildId)) {
				YouTubeAPI::youtubeAPIMap->erase(guildId);
				shared_ptr<YouTubeAPI> youTubeAPI;

				youTubeAPI = make_shared<YouTubeAPI>(guildId);
				auto returnValue = youTubeAPI->theSong.searchForSong(searchQuery);
				YouTubeAPI::youtubeAPIMap->insert_or_assign(guildId, youTubeAPI);
				return returnValue;

			}
			else {
				shared_ptr<YouTubeAPI> youTubeAPI;
				if (YouTubeAPI::discordGuildMap->contains(guildId)) {
					youTubeAPI = make_shared<YouTubeAPI>(guildId);
					auto returnValue = youTubeAPI->theSong.searchForSong(searchQuery);
					YouTubeAPI::youtubeAPIMap->insert_or_assign(guildId, youTubeAPI);
					return returnValue;
				}
				else {
					return vector<YouTubeSong>();
				}
			}
		}
		
	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* YouTubeAPI::sendAudioDataBufferMap{ nullptr };
	string YouTubeAPI::baseSearchURL{ "https://www.youtube.com/results?search_query=" };
	map<string, shared_ptr<VoiceConnection>>* YouTubeAPI::voiceConnectionMap{ nullptr };
	map<string, shared_ptr<YouTubeAPI>>* YouTubeAPI::youtubeAPIMap{ nullptr };
	string YouTubeAPI::baseWatchURL{ "https://www.youtube.com/watch?v=" };
	map<string, DiscordGuild*>* YouTubeAPI::discordGuildMap{ nullptr };
	string YouTubeAPI::clientId{ "" };
};
#endif