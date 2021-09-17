// SoundCloudStuff.hpp - Header for the "SoundCloud stuff".
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SOUNDCLOUD_STUFF_
#define _SOUNDCLOUD_STUFF_

#include "DiscordCoreClientBase.hpp"
#include "HttpStuff.hpp"
#include "DataParsingFunctions.hpp"
#include "DatabaseStuff.hpp"
#include "VoiceConnectionStuff.hpp"

namespace DiscordCoreAPI {

	class SoundCloudAPICore {
	public:

		SoundCloudAPICore(map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap, string guildIdNew, DiscordGuild* discordGuildNew, shared_ptr<VoiceConnection> voiceConnectionNew) {
			this->sendAudioDataBufferMap = sendAudioDataBufferMap;
			this->sendAudioDataBuffer = this->sendAudioDataBufferMap->at(guildIdNew);
			this->voiceConnection = voiceConnectionNew;
			this->discordGuild = discordGuildNew;
			this->guildId = guildIdNew;
		}

		vector<SongSearchResult> searchForSong(string songQuery) {
			collectClientId(songQuery);

			vector<SongSearchResult> results{};

			DiscordCoreInternal::HttpAgentResources agentResources{};
			agentResources.baseURL = this->baseSearchURL02;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
			CURLU* urlHandle = curl_url();
			curl_url_set(urlHandle, CURLUPart::CURLUPART_QUERY, songQuery.c_str(), CURLU_URLENCODE);
			char* charString{ nullptr };
			curl_url_get(urlHandle, CURLUPart::CURLUPART_QUERY, &charString, 0);
			workload.relativePath = to_string(to_hstring(charString)) + "&facet=model&client_id=" + this->clientId + "&limit=20&offset=0&linked_partitioning=1&app_version=" + this->appVersion + "&app_locale=en";
			curl_free(charString);
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "SoundCloudAPICore::searchForSong() ");
			vector<string> assetPaths{};
			for (auto value : returnData.data.at("collection")) {
				SongSearchResult result{};
				DiscordCoreInternal::DataParser::parseObject(value, &result);
				if (result.songTitle == "") {
					continue;
				}
				result.songDownloadURL = result.downloadProtoURL + "?client_id=" + this->clientId + "&track_authorization=" + result.trackAuthorization;
				result.songType = SongType::SoundCloud;
				if (result.thumbNailURL.find("https") == string::npos) {
					continue;
				}
				string newString = result.thumbNailURL.substr(0, result.thumbNailURL.find_last_of("-") + 1);
				newString += "t500x500.jpg";
				result.thumbNailURL = newString;
				cout << "SEARCH URL: " << result.songURL << endl;
				results.push_back(result);
			}
			return results;
		}

		Song collectDownloadInfo(SongSearchResult searchResult, GuildMember guildMember) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			DiscordCoreInternal::HttpWorkloadData workload02{};
			agentResources.baseURL = searchResult.songDownloadURL;
			DiscordCoreInternal::HttpRequestAgent requestAgent02(agentResources);
			workload02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload02.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
			DiscordCoreInternal::HttpData returnData02 = requestAgent02.submitWorkloadAndGetResult(workload02, "SoundCloudAPICore::searchForSong() ");
			DiscordCoreInternal::HttpWorkloadData workload03{};
			agentResources.baseURL = returnData02.data.at("url");
			DiscordCoreInternal::HttpRequestAgent requestAgent03(agentResources);
			workload03.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload03.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
			DiscordCoreInternal::HttpData returnData03 = requestAgent03.submitWorkloadAndGetResult(workload03, "SoundCloudAPICore::searchForSong() ");
			string newString = returnData03.returnMessage;
			Song song{};
			while (newString.find("#EXTINF:") != string::npos) {
				string newString02 = newString.substr(newString.find("#EXTINF:") + to_string(L"#EXTINF:").size());
				string newString00 = newString02.substr(0, newString02.find(","));
				string newString03 = newString02.substr(newString02.find(",") + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
				newString = newString02.substr(newString02.find(","));
				if (newString03.find("#EXT-X-ENDLIST") != string::npos) {
					newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
				}
				song.downloadURLs.push_back(newString03);
				cout << "DOWNLOAD URL: " << newString03 << endl;
			}
			
			song.addedById = guildMember.user.id;
			song.addedByUserName = guildMember.user.userName;
			song.description = searchResult.description;
			song.duration = searchResult.duration;
			song.formatDownloadURL = returnData02.data.at("url");
			song.imageURL = searchResult.thumbNailURL;
			song.songId = searchResult.songId;
			song.songType = SongType::SoundCloud;
			song.title = searchResult.songTitle;
			song.url = searchResult.songURL;
			return song;
			
		}

		void collectClientId(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources{};
			agentResources.baseURL = this->baseSearchURL;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
			workload.relativePath = searchQuery;
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "SoundCloudAPICore::searchForSong() ");
			vector<string> assetPaths{};
			string newString = returnData.returnMessage.substr(returnData.returnMessage.find("crossorigin src=") + to_string(L"crossorigin src=").size());
			string newString02 = newString.substr(1, newString.find(".js") + 2);
			assetPaths.push_back(newString02);
			while (newString.find("crossorigin src=") != string::npos) {
				string newString03 = newString.substr(1, newString.find(".js") + 2);
				newString = newString.substr(newString.find("crossorigin src=") + to_string(L"crossorigin src=").size());
				assetPaths.push_back(newString03);
			}
			DiscordCoreInternal::HttpAgentResources agentResources02;
			agentResources02.baseURL = assetPaths.at(5);
			DiscordCoreInternal::HttpRequestAgent requestAgent02(agentResources02);
			DiscordCoreInternal::HttpWorkloadData workload02;
			workload02.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload02.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_AUTH;
			workload02.relativePath;;
			DiscordCoreInternal::HttpData returnData02 = requestAgent02.submitWorkloadAndGetResult(workload02, "SoundCloudAPICore::searchForSong() ");
			vector<DiscordCoreAPI::SongSearchResult> searchResults;
			string newString03 = returnData02.returnMessage.substr(returnData02.returnMessage.find("?client_id=") + to_string(L"?client_id=").size());
			string clientIdNew = newString03.substr(0, newString03.find("&"));
			this->clientId = clientIdNew;
			if (returnData.returnCode != 200) {
				cout << "SoundCloudAPICore::searchForSong() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
			}
			return;
		}

		~SoundCloudAPICore() {};

		string baseSearchURL02{ "https://api-v2.soundcloud.com/search?q=" };
		string baseSearchURL{ "https://soundcloud.com/search?q=" };
		string baseSoundCloudURL{ "https://soundcloud.com/" };
		string appVersion{ "1631696495" };
		string clientId{ "" };
		json URLs = {
			{"playlists-liked","https://api-v2.soundcloud.com/users/{0}/playlists/liked_and_owned?limit=200"},
			{"favorites","https://api-v2.soundcloud.com/users/{0}/track_likes?limit=200"},
			{"commented","https://api-v2.soundcloud.com/users/{0}/comments"},
			{"tracks","https://api-v2.soundcloud.com/users/{0}/tracks?limit=200"},
			{"all","https://api-v2.soundcloud.com/profile/soundcloud:users:{0}?limit=200"},
			{"playlists","https://api-v2.soundcloud.com/users/{0}/playlists?limit=5"},
			{"resolve","https://api-v2.soundcloud.com/resolve?url={0}"},
			{"trackinfo","https://api-v2.soundcloud.com/tracks/{0}"},
			{"original_download","https://api-v2.soundcloud.com/tracks/{0}/download"},
			{"user","https://api-v2.soundcloud.com/users/{0}"},
			{"me","https://api-v2.soundcloud.com/me?oauth_token={0}"}
		};
	
		map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap{ nullptr };
		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		cancellation_token_source cancelTokenSource{ cancellation_token_source() };
		cancellation_token cancelToken{ this->cancelTokenSource.get_token() };
		const string baseWatchURL{ "https://www.SoundCloud.com/watch?v=" };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		const string baseURL{ "https://www.SoundCloud.com" };
		DiscordGuild* discordGuild{ new DiscordGuild };
		concurrency::event readyToBeDoneEvent {};
		task<void>* currentTask{ nullptr };
		vector<Song> songQueue{};
		const string newLine{ "\n\r" };
		const int maxBufSize{ 4096 };
		string playerResponse{ "" };
		bool areWeStopping{ false };
		Song currentSong{};
		bool loopSong{ false };
		bool loopAll{ false };
		string guildId{ "" };

		void setLoopAllStatus(bool enabled) {
			this->loopAll = enabled;
		}

		bool isLoopAllEnabled() {
			return this->loopAll;
		}

		void setPlaylist(Playlist playlist) {
			this->loopAll = playlist.isLoopAllEnabled;
			this->loopSong = playlist.isLoopSongEnabled;
			this->songQueue = playlist.songQueue;
		}

		void setLoopSongStatus(bool enabled) {
			this->loopSong = enabled;
		}

		bool isLoopSongEnabled() {
			return this->loopSong;
		}

		void setQueue(vector<Song> dataPackage) {
			this->songQueue = dataPackage;
		}

		vector<Song>* getQueue() {
			return &this->songQueue;
		}

		bool stop() {
			if (this->voiceConnection->areWeCurrentlyPlaying()) {
				this->areWeStopping = true;
				this->cancelTokenSource.cancel();
				AudioFrameData dataFrame;
				if (this->currentTask != nullptr) {
					this->currentTask->then([](task<void> previousTask)->task<void> {
						auto exceptionNew = current_exception();
						try {
							previousTask.get();
							co_return;
						}
						catch (...) {
							rethrowException("SoundCloudAPICoreCore::stop() Error: ");
							co_return;
						}
						}).get();
						delete this->currentTask;
						this->currentTask = nullptr;
				}
				while (try_receive(this->sendAudioDataBuffer.get(), dataFrame)) {};
				vector<Song> newVector02;
				if (this->currentSong.description != "") {
					newVector02.push_back(this->currentSong);
					this->currentSong = Song();
				}
				for (auto value : this->songQueue) {
					newVector02.push_back(value);
				}
				this->songQueue = newVector02;
				this->readyToBeDoneEvent.wait();
				this->readyToBeDoneEvent.reset();
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
				if (this->currentTask != nullptr) {
					this->currentTask->then([](task<void> previousTask)->task<void> {
						auto exceptionNew = current_exception();
						try {
							previousTask.get();
							co_return;
						}
						catch (...) {
							rethrowException("SoundCloudAPICoreCore::stopWithoutSaving() Error: ");
							co_return;
						}
						}).get();
						delete this->currentTask;
						this->currentTask = nullptr;
				}
				while (try_receive(this->sendAudioDataBuffer.get(), dataFrame)) {};
				vector<Song> newVector02;
				this->currentSong = Song();
				this->readyToBeDoneEvent.wait();
				this->readyToBeDoneEvent.reset();
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
			Song tempSong = this->songQueue.at(firstSongPosition);
			this->songQueue.at(firstSongPosition) = this->songQueue.at(secondSongPosition);
			this->songQueue.at(secondSongPosition) = tempSong;
		}

		Song getCurrentSong() {
			if (this->currentSong.videoId != "") {
				return this->currentSong;
			}
			else if (this->songQueue.size() > 0) {
				return this->songQueue.at(0);
			}
			else {
				return Song();
			};
		}

		void setCurrentSong(Song song) {
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
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() > 0 && this->currentSong.description != "") {
					this->currentSong = this->currentSong;
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.description != "" && this->songQueue.size() == 0) {
					this->currentSong = this->currentSong;
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() == 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
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
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
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
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.description != "" && this->songQueue.size() == 0) {
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->songQueue.size() == 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
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
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					this->songQueue.erase(this->songQueue.end() - 1, this->songQueue.end());
					returnData.currentSong = this->currentSong;
					return returnData;
				}
				else if (this->currentSong.description != "" && this->songQueue.size() == 0) {
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
					returnData.currentSong = this->currentSong;
					this->currentSong = Song();
					return returnData;
				}
				else if (this->songQueue.size() == 1 && this->currentSong.description == "") {
					this->currentSong = this->songQueue.at(0);
					this->songQueue.erase(this->songQueue.begin(), this->songQueue.begin() + 1);
					this->cancelTokenSource = cancellation_token_source();
					this->cancelToken = this->cancelTokenSource.get_token();
					auto currentTaskNew = this->downloadAndStreamAudio(this->currentSong, this->cancelToken);
					this->currentTask = new task<void>();
					*this->currentTask = currentTaskNew;
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

		Song addSongToQueue(SongSearchResult searchResult, GuildMember guildMember) {
			auto newSong = collectDownloadInfo(searchResult, guildMember);
			Song song;
			song.songId = to_string((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			song.addedByUserName = guildMember.user.userName;
			song.downloadURLs = newSong.downloadURLs;
			song.formatDownloadURL = searchResult.songURL;
			song.description = searchResult.description;
			song.imageURL = searchResult.thumbNailURL;
			song.duration = searchResult.duration;
			song.addedById = guildMember.user.id;
			song.title = searchResult.songTitle;
			song.videoId = searchResult.songId;
			song.url = searchResult.songURL;
			bool isItFound = false;
			for (auto value : this->songQueue) {
				if (value.songId == song.songId) {
					isItFound = true;
				}
			}
			this->songQueue.push_back(song);
			return song;
		}

		task<void> downloadAndStreamAudio(Song song, cancellation_token token, int retryCountNew = 0) {
			const std::function<task<void>(void)> theFunction = [&, this, retryCountNew]()->task<void> {
				SoundCloudAPICore* thisPtr = this;
				auto tokenNew = thisPtr->cancelTokenSource.get_token();
				apartment_context mainThread{};
				shared_ptr<DiscordCoreInternal::ThreadContext> threadContext = DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get();
				co_await resume_foreground(*threadContext->dispatcherQueue);
				thisPtr->areWeStopping = false;
				BuildSongDecoderData dataPackage{};
				try {
					if (thisPtr->sendAudioDataBufferMap->contains(thisPtr->guildId)) {
						thisPtr->sendAudioDataBuffer = thisPtr->sendAudioDataBufferMap->at(thisPtr->guildId);
					}
					else {
						co_return;
					}
					string downloadBaseURL;
					if (thisPtr->currentSong.formatDownloadURL.find("https://") != string::npos && thisPtr->currentSong.formatDownloadURL.find("/videoplayback?") != string::npos) {
						downloadBaseURL = thisPtr->currentSong.formatDownloadURL.substr(thisPtr->currentSong.formatDownloadURL.find("https://") + to_string(L"https://").length(), thisPtr->currentSong.formatDownloadURL.find("/videoplayback?") - to_string(L"https://").length());
					}
					// Creates GET request.
					string request = "GET " + thisPtr->currentSong.formatDownloadURL + " HTTP/1.1" + thisPtr->newLine +
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
					int remainingDownloadContentLength{ thisPtr->currentSong.contentLength };
					int contentLengthCurrent{ thisPtr->maxBufSize };
					int bytesReadTotal{ 0 };
					int counter{ 0 };
					dataPackage.sendEncodedAudioDataBuffer = new unbounded_buffer<vector<uint8_t>>();
					dataPackage.totalFileSize = thisPtr->currentSong.contentLength - 585;
					dataPackage.bufferMaxSize = thisPtr->maxBufSize;
					SongDecoder* songDecoder = new SongDecoder(dataPackage);
					SongEncoder* songEncoder = new SongEncoder();
					send(dataPackage.sendEncodedAudioDataBuffer, vector<uint8_t>());
					while (thisPtr->currentSong.contentLength > bytesReadTotal) {
						if (tokenNew.is_canceled()) {
							songDecoder->refreshTimeForBuffer = 10;
							thisPtr->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
							agent::wait(songDecoder);
							delete songDecoder;
							songDecoder = nullptr;
							thisPtr->readyToBeDoneEvent.set();
							delete thisPtr->currentTask;
							thisPtr->currentTask = nullptr;
							threadContext->releaseGroup();
							cancel_current_task();
							co_return;
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
								delete thisPtr->currentTask;
								thisPtr->currentTask = nullptr;
								threadContext->releaseGroup();
								cancel_current_task();
								co_return;
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
										delete thisPtr->currentTask;
										thisPtr->currentTask = nullptr;
										threadContext->releaseGroup();
										cancel_current_task();
										co_return;
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
									delete thisPtr->currentTask;
									thisPtr->currentTask = nullptr;
									threadContext->releaseGroup();
									cancel_current_task();
									co_return;
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
					delete thisPtr->currentTask;
					thisPtr->currentTask = nullptr;
					threadContext->releaseGroup();
					co_await mainThread;
					co_return;
				}
				catch (...) {
					if (retryCountNew < 5) {
						int retryCountNewer = retryCountNew;
						retryCountNewer += 1;
						thisPtr->downloadAndStreamAudio(thisPtr->currentSong, tokenNew, retryCountNew).get();
						rethrowException("SoundCloudAPICoreCore::downloadAndStreamAudioToBeWrapped() Error: ");
						thisPtr->readyToBeDoneEvent.set();
						co_return;
					}
					else {
						vector<uint8_t> newVector;
						send(dataPackage.sendEncodedAudioDataBuffer, newVector);
						AudioFrameData frameData{};
						frameData.encodedFrameData.sampleCount = 0;
						frameData.type = AudioFrameType::Cancel;
						frameData.rawFrameData.sampleCount = 0;
						send(thisPtr->sendAudioDataBuffer.get(), frameData);
						thisPtr->stopWithoutSaving();
						thisPtr->voiceConnection->onSongCompletionEvent(thisPtr->voiceConnection.get());
						rethrowException("SoundCloudAPICoreCore::downloadAndStreamAudioToBeWrapped() Final Error: ");
						thisPtr->readyToBeDoneEvent.set();
						co_return;
					}
				};

			};
			return concurrency::task<void>(theFunction, this->cancelTokenSource.get_token());

		};

		void sendEmptyingFrames(unbounded_buffer<vector<uint8_t>>* sendAudioDataBufferNew) {
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
		}

	};

	class SoundCloudAPI {
	public:

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>> voiceConnectionMap;
		static map<string, shared_ptr<SoundCloudAPICore>>* SoundCloudAPICoreMap;
		static map<string, DiscordGuild*>* discordGuildMap;

		static void initialize(map<string, shared_ptr<SoundCloudAPICore>>* SoundCloudAPICoreMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMapNew, map<string, DiscordGuild*>* discordGuildMapNew) {
			SoundCloudAPI::sendAudioDataBufferMap = sendAudioDataBufferMapNew;
			SoundCloudAPI::SoundCloudAPICoreMap = SoundCloudAPICoreMapNew;
			SoundCloudAPI::discordGuildMap = discordGuildMapNew;
		};

		static bool stop(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				if (!SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->stop()) {
					return false;
				}
				bool isSongLooped = SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopSongEnabled();
				bool isAllLooped = SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopAllEnabled();
				auto songQueue = *SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getQueue();
				SoundCloudAPI::SoundCloudAPICoreMap->erase(guildId);
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				SoundCloudAPI->setLoopSongStatus(isSongLooped);
				SoundCloudAPI->setLoopAllStatus(isAllLooped);
				SoundCloudAPI->setQueue(songQueue);
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return true;
			}
			else {
				return false;
			}
		}

		static bool skip(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				bool isSongLooped = SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopSongEnabled();
				bool isAllLooped = SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopAllEnabled();
				auto currentSong = SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getCurrentSong();
				auto songQueue = *SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getQueue();
				if (!SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->stop()) {
					return false;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->erase(guildId);
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				SoundCloudAPI->setLoopSongStatus(isSongLooped);
				SoundCloudAPI->setLoopAllStatus(isAllLooped);
				SoundCloudAPI->currentSong = currentSong;
				SoundCloudAPI->setQueue(songQueue);
				SoundCloudAPI->sendNextSong();
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return true;
			}
			else {
				return false;
			}
		}

		static void setLoopAllStatus(bool enabled, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setLoopAllStatus(enabled);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setLoopAllStatus(enabled);
			}
		}

		static bool isLoopAllEnabled(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopAllEnabled();
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopAllEnabled();
			}
		}

		static void setLoopSongStatus(bool enabled, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setLoopSongStatus(enabled);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setLoopSongStatus(enabled);
			}
		}

		static bool isLoopSongEnabled(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopSongEnabled();
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isLoopSongEnabled();
			}
		}

		static bool isThereAnySongs(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isThereAnySongs();
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return false;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->isThereAnySongs();
			}
		}

		static Song addSongToQueue(SongSearchResult searchResult, GuildMember guildMember, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->addSongToQueue(searchResult, guildMember);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return Song();
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->addSongToQueue(searchResult, guildMember);
			}
		}

		static void setQueue(vector<Song> dataPackage, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setQueue(dataPackage);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setQueue(dataPackage);
			}
		}

		static vector<Song>* getQueue(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getQueue();
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return nullptr;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getQueue();
			}
		}

		static void setPlaylist(Playlist playlist, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setPlaylist(playlist);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setPlaylist(playlist);
			}
		}

		static void modifyQueue(int firstSongPosition, int secondSongPosition, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->modifyQueue(firstSongPosition, secondSongPosition);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->modifyQueue(firstSongPosition, secondSongPosition);
			}

		}

		static Song getCurrentSong(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getCurrentSong();
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return Song();
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->getCurrentSong();
			}
		}

		static void setCurrentSong(Song song, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setCurrentSong(song);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return;
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->setCurrentSong(song);
			}
		}

		static SendNextSongReturnData sendNextSong(string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->sendNextSong();
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return  SendNextSongReturnData();
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->sendNextSong();
			}
		}

		static vector<SongSearchResult> searchForSong(string searchQuery, string guildId) {
			if (SoundCloudAPI::SoundCloudAPICoreMap->contains(guildId)) {
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->searchForSong(searchQuery);
			}
			else {
				shared_ptr<SoundCloudAPICore> SoundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					auto discordGuildPtr = SoundCloudAPI::discordGuildMap->at(guildId);
					SoundCloudAPI = make_shared<SoundCloudAPICore>(SoundCloudAPI::sendAudioDataBufferMap, guildId, discordGuildPtr, SoundCloudAPI::voiceConnectionMap.at(guildId));
				}
				else {
					return vector<SongSearchResult>();
				}
				SoundCloudAPI::SoundCloudAPICoreMap->insert_or_assign(guildId, SoundCloudAPI);
				return SoundCloudAPI::SoundCloudAPICoreMap->at(guildId)->searchForSong(searchQuery);
			}
		}
	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* SoundCloudAPI::sendAudioDataBufferMap{ nullptr };
	map<string, shared_ptr<SoundCloudAPICore>>* SoundCloudAPI::SoundCloudAPICoreMap{ nullptr };
	map<string, shared_ptr<VoiceConnection>> SoundCloudAPI::voiceConnectionMap{};
	map<string, DiscordGuild*>* SoundCloudAPI::discordGuildMap{ nullptr };

};
#endif