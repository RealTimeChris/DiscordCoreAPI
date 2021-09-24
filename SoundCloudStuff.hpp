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

	void SoundCloudSong::initialize() {
		SoundCloudSong::clientId = SoundCloudSong::collectClientId("testvalue");
	}

	vector<SoundCloudSong> SoundCloudSong::searchForSong(string songQuery) {
		vector<SoundCloudSong> results{};
		DiscordCoreInternal::HttpAgentResources agentResources{};
		agentResources.baseURL = SoundCloudSong::baseSearchURL02;
		DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
		CURLU* urlHandle = curl_url();
		curl_url_set(urlHandle, CURLUPart::CURLUPART_QUERY, songQuery.c_str(), CURLU_URLENCODE);
		char* charString{ nullptr };
		curl_url_get(urlHandle, CURLUPart::CURLUPART_QUERY, &charString, 0);
		workload.relativePath = to_string(to_hstring(charString)) + "&facet=model&client_id=" + SoundCloudSong::clientId + "&limit=20&offset=0&linked_partitioning=1&app_version=" + SoundCloudSong::appVersion + "&app_locale=en";
		curl_free(charString);
		DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "SoundCloudAPI::searchForSong() ");
		for (auto value : returnData.data.at("collection")) {
			SoundCloudSong newSong = SoundCloudSong{};
			DiscordCoreInternal::DataParser::parseObject(value, &newSong);
			if (newSong.songTitle == "") {
				continue;
			}
			newSong.type = SongType::SoundCloud;
			if (newSong.thumbnailURL.find("https") == string::npos) {
				continue;
			}
			newSong.firstDownloadURL += "?client_id=" + SoundCloudSong::clientId + "&track_authorization=" + newSong.trackAuthorization;
			string newString = newSong.thumbnailURL.substr(0, newSong.thumbnailURL.find_last_of("-") + 1);
			newString += "t500x500.jpg";
			newSong.thumbnailURL = newString;
			results.push_back(newSong);
		}
		return results;
	}

	SoundCloudSong::SoundCloudSong() {
		this->httpClientForGettingFinalURLs = HttpClient();
		this->httpClientForGettingSecondURL = HttpClient();
		this->headerCollectionForSecondURL = this->httpClientForGettingSecondURL.DefaultRequestHeaders();
		auto acceptHeader = this->headerCollectionForSecondURL.Accept();
		acceptHeader.TryParseAdd(L"*/*");
		this->headerCollectionForSecondURL.Host(winrt::Windows::Networking::HostName(L"api-v2.soundcloud.com"));
		auto userAgent = this->headerCollectionForSecondURL.UserAgent();
		userAgent.TryParseAdd(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.82 Safari/537.36");
		this->headerCollectionForFinalURLs = this->httpClientForGettingFinalURLs.DefaultRequestHeaders();
		this->headerCollectionForFinalURLs.Insert(L"authority", L"cf-hls-media.sndcdn.com");
		auto userAgent02 = this->headerCollectionForFinalURLs.UserAgent();
		userAgent02.TryParseAdd(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.82 Safari/537.36");
	}

	SoundCloudSong SoundCloudSong::collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong) {
		auto newerSong = findSecondDownloadURL(newSong);
		auto newestSong = findFinalDownloadURLs(newerSong);
		newestSong.addedByUserId = addedByGuildMember.user.id;
		newestSong.addedByUserName = addedByGuildMember.user.userName;
		*this = newestSong;
		return newestSong;
	}

	SoundCloudSong SoundCloudSong::findSecondDownloadURL(SoundCloudSong newSong) {
		winrt::Windows::Foundation::Uri uri(to_hstring(newSong.firstDownloadURL));
		auto results = newSong.httpClientForGettingSecondURL.TryGetAsync(uri).get();
		string resultString = to_string(results.ResponseMessage().Content().ReadAsStringAsync().get());
		json jsonData = jsonData.parse(resultString);
		if (jsonData.contains("url")) {
			newSong.secondDownloadURL = jsonData.at("url");
		}
		return newSong;
	}

	SoundCloudSong SoundCloudSong::findFinalDownloadURLs(SoundCloudSong newSong) {
		winrt::Windows::Foundation::Uri uri(to_hstring(newSong.secondDownloadURL));
		string newPathString = newSong.secondDownloadURL.substr(newSong.secondDownloadURL.find("/playlist"));
		newSong.headerCollectionForFinalURLs.Insert(L"path", to_hstring(newPathString));
		auto results = newSong.httpClientForGettingFinalURLs.TryGetAsync(uri).get();
		string newString = to_string(results.ResponseMessage().Content().ReadAsStringAsync().get());
		while (newString.find("#EXTINF:") != string::npos) {
			string newString02 = newString.substr(newString.find("#EXTINF:") + to_string(L"#EXTINF:").size());
			string newString00 = newString02.substr(0, newString02.find(","));
			string newString03 = newString02.substr(newString02.find(",") + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
			newString = newString02.substr(newString02.find(","));
			if (newString03.find("#EXT-X-ENDLIST") != string::npos) {
				newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
			}
			string newString04 = newString03.substr(newString03.find_first_of("1234567890"));
			int firstNumber01 = stoi(newString04.substr(0, newString04.find("/")));
			string newString05 = newString04.substr(newString04.find("/") + 1);
			int secondNumber = stoi(newString05.substr(0, newString05.find("/")));
			DownloadURL downloadURL{};
			downloadURL.urlPath = newString03;
			downloadURL.contentSize = secondNumber - firstNumber01;
			newSong.finalDownloadURLs.push_back(downloadURL);
		}
		for (auto value : newSong.finalDownloadURLs) {
			newSong.contentLength += value.contentSize;
		}
		return newSong;
	}

	string SoundCloudSong::collectClientId(string searchQuery) {
		DiscordCoreInternal::HttpAgentResources agentResources{};
		agentResources.baseURL = SoundCloudSong::baseSearchURL;
		DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
		workload.relativePath = searchQuery;
		DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "SoundCloudAPI::collectClientId() ");
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
		DiscordCoreInternal::HttpData returnData02 = requestAgent02.submitWorkloadAndGetResult(workload02, "SoundCloudAPI::collectClientId() ");
		string newString03 = returnData02.returnMessage.substr(returnData02.returnMessage.find("?client_id=") + to_string(L"?client_id=").size());
		string clientIdNew = newString03.substr(0, newString03.find("&"));
		SoundCloudSong::clientId = clientIdNew;
		if (returnData.returnCode != 200) {
			cout << "SoundCloudAPI::searchForSong() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
		}
		return clientIdNew;
	}

	class SoundCloudAPI : public implements<SoundCloudAPI, IInspectable> {
	public:

		template <class _Ty>
		friend 	_CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreClient;
		friend class SoundCloudAPI;
		friend class SongAPI;
		friend class SongAPI;
		friend class Guild;

		SoundCloudAPI() {};

		SoundCloudAPI(string guildIdNew) {
			this->sendAudioDataBuffer = SoundCloudAPI::sendAudioDataBufferMap->at(guildIdNew);
			this->voiceConnection = SoundCloudAPI::voiceConnectionMap->at(guildIdNew);
			this->discordGuild = SoundCloudAPI::discordGuildMap->at(guildIdNew);
			this->guildId = guildIdNew;
		}

		static void initialize(map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMapNew, map<string, DiscordGuild*>* discordGuildMapNew, map<string, shared_ptr<VoiceConnection>>* voiceConnectionMapNew) {
			SoundCloudAPI::sendAudioDataBufferMap = sendAudioDataBufferMapNew;
			SoundCloudAPI::voiceConnectionMap = voiceConnectionMapNew;
			SoundCloudAPI::soundCloudAPIMap = soundCloudAPIMapNew;
			SoundCloudAPI::discordGuildMap = discordGuildMapNew;
		};

	protected:

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMap;
		static map<string, DiscordGuild*>* discordGuildMap;
		static string baseSearchURL02;
		static string baseSearchURL;
		static string clientId;

		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		cancellation_token_source cancelTokenSource{ cancellation_token_source() };
		cancellation_token cancelToken{ this->cancelTokenSource.get_token() };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		DiscordGuild* discordGuild{ new DiscordGuild };
		concurrency::event readyToBeDoneEvent {};
		string appVersion{ "1631696495" };
		bool areWeStopping{ false };
		SoundCloudSong theSong{};
		task<void>* currentTask{};
		string guildId{ "" };
		int retryCount{ 0 };

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
			this->downloadAndStreamAudioWrapper(newSong);
		}

		HRESULT GetRuntimeClassName(HSTRING*) {
			return HRESULT();
		}

		HRESULT GetTrustLevel(TrustLevel*) {
			return HRESULT();
		}

		task<void> downloadAndStreamAudioWrapper(SoundCloudSong newSong, int retryCountNew = 0) {
			shared_ptr<DiscordCoreInternal::ThreadContext> threadContext = DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get();
			apartment_context mainThread{};
			co_await resume_foreground(*threadContext->dispatcherQueue);
			this->currentTask = new task<void>(create_task([=, strong_this{ get_strong() }]()->void {
				auto tokenNew = strong_this->cancelTokenSource.get_token();
				auto song = newSong;
				strong_this->areWeStopping = false;
				BuildSongDecoderData dataPackage{};				
				if (strong_this->sendAudioDataBufferMap->contains(strong_this->guildId)) {
					strong_this->sendAudioDataBuffer = strong_this->sendAudioDataBufferMap->at(strong_this->guildId);
				}
				else {
					return;
				}
				int counter{ 0 };
				Filters::HttpBaseProtocolFilter filter;
				Filters::HttpCacheControl cacheControl{ nullptr };
				cacheControl = filter.CacheControl();
				cacheControl.ReadBehavior(Filters::HttpCacheReadBehavior::NoCache);
				cacheControl.WriteBehavior(Filters::HttpCacheWriteBehavior::NoCache);
				auto getHttpClient = HttpClient(filter);
				auto getHeaders = getHttpClient.DefaultRequestHeaders();
				winrt::Windows::Foundation::Uri uri(to_hstring(song.finalDownloadURLs.at(counter).urlPath));
				auto results00 = getHttpClient.TryGetBufferAsync(uri).get();

				DiscordCoreInternal::HttpAgentResources agentResources01{};
				agentResources01.baseURL = song.finalDownloadURLs.at(counter).urlPath;
				DiscordCoreInternal::HttpRequestAgent requestAgent01(agentResources01);
				DiscordCoreInternal::HttpWorkloadData workloadData01{};
				workloadData01.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
				workloadData01.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SONG_GET;
				auto result01 = requestAgent01.submitWorkloadAndGetResult(workloadData01, "SoundCloudAPI::downloadAndStream Audio() Error: ");
				dataPackage.sendEncodedAudioDataBuffer = new unbounded_buffer<vector<uint8_t>>();
				dataPackage.totalFileSize = song.contentLength;
				dataPackage.bufferMaxSize = song.contentLength;
				SongDecoder* songDecoder = new SongDecoder(dataPackage);
				SongEncoder* songEncoder = new SongEncoder();
				send(dataPackage.sendEncodedAudioDataBuffer, vector<uint8_t>());
				while (counter < song.finalDownloadURLs.size()) {
					if (tokenNew.is_canceled()) {
						songDecoder->refreshTimeForBuffer = 10;
						strong_this->sendEmptyingFrames(dataPackage.sendEncodedAudioDataBuffer);
						agent::wait(songDecoder);
						strong_this->readyToBeDoneEvent.set();
						threadContext->releaseGroup();
						cancel_current_task();
						return;
					}
					DiscordCoreInternal::HttpAgentResources agentResources{};
					agentResources.baseURL = song.finalDownloadURLs.at(counter).urlPath;
					DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
					DiscordCoreInternal::HttpWorkloadData workloadData{};
					workloadData.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
					workloadData.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SONG_GET;
					auto result = requestAgent.submitWorkloadAndGetResult(workloadData, "SoundCloudAPI::downloadAndStreamAudio() Error: ");
					vector<uint8_t> newVector{};
					for (unsigned int x = 0; x < result.returnMessage.size(); x += 1) {
						newVector.push_back(result.returnMessage[x]);
					}
					if (counter == 0) {
						songDecoder->startMe();
					}
					songDecoder->submitDataForDecoding(newVector);
					if (!tokenNew.is_canceled() && !strong_this->areWeStopping) {
						vector<RawFrameData> frames{};
						RawFrameData rawFrame{};
						rawFrame.data.resize(0);
						while (songDecoder->getFrame(&rawFrame)) {
							if (rawFrame.data.size() != 0) {
								frames.push_back(rawFrame);
							}
						}
						if (!tokenNew.is_canceled() && !strong_this->areWeStopping) {
							auto encodedFrames = songEncoder->encodeFrames(frames);
							for (auto value : encodedFrames) {
								if (!strong_this->areWeStopping && !tokenNew.is_canceled()) {
									send(strong_this->sendAudioDataBuffer.get(), value);
								}

							}
						}
					}
					StopWatch stopWatch(1);
					while (!stopWatch.hasTimePassed()) {
						wait(1);
					}
					counter += 1;
				}

				if (!tokenNew.is_canceled() && !strong_this->areWeStopping) {
					vector<uint8_t> newVector{};
					send(dataPackage.sendEncodedAudioDataBuffer, newVector);
					RawFrameData frameData01{};
					while (songDecoder->getFrame(&frameData01)) {};
					AudioFrameData frameData{};
					frameData.type = AudioFrameType::Cancel;
					frameData.rawFrameData.sampleCount = 0;
					frameData.encodedFrameData.sampleCount = 0;
					send(strong_this->sendAudioDataBuffer.get(), frameData);
				}
				vector<uint8_t> newVector;
				send(dataPackage.sendEncodedAudioDataBuffer, newVector);
				songDecoder->refreshTimeForBuffer = 10;
				agent::wait(songDecoder);
				delete songDecoder;
				songDecoder = nullptr;
				strong_this->readyToBeDoneEvent.set();
				threadContext->releaseGroup();
				strong_this->currentTask = nullptr;
				return;
			}, this->cancelToken));
			co_await mainThread;
		};

		void sendEmptyingFrames(unbounded_buffer<vector<uint8_t>>* sendAudioDataBufferNew) {
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
			send(sendAudioDataBufferNew, vector<uint8_t>());
		}

		static bool stop(string guildId) {
			if (!SoundCloudAPI::soundCloudAPIMap->at(guildId)->stop()) {
				return false;
			}
			SoundCloudAPI::soundCloudAPIMap->erase(guildId);
			shared_ptr<SoundCloudAPI> soundCloudAPI = make_shared<SoundCloudAPI>(guildId);
			SoundCloudAPI::soundCloudAPIMap->insert_or_assign(guildId, soundCloudAPI);
			return true;
		}

		static void sendNextSong(Song newSong, string guildId) {
			if (SoundCloudAPI::soundCloudAPIMap->contains(guildId)) {
				SoundCloudAPI::soundCloudAPIMap->at(guildId)->sendNextSong(newSong);
				return;

			}
			else {
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					shared_ptr<SoundCloudAPI> soundCloudAPI = make_shared<SoundCloudAPI>(guildId);
					SoundCloudAPI::soundCloudAPIMap->insert_or_assign(guildId, soundCloudAPI);
					soundCloudAPI->sendNextSong(newSong);
					return;
				}
				else {
					return;
				}
			}
		}

		static vector<SoundCloudSong> searchForSong(string searchQuery, string guildId) {
			if (SoundCloudAPI::soundCloudAPIMap->contains(guildId)) {
				SoundCloudAPI::soundCloudAPIMap->erase(guildId);
				shared_ptr<SoundCloudAPI> soundCloudAPI;

				soundCloudAPI = make_shared<SoundCloudAPI>(guildId);
				auto returnValue = soundCloudAPI->theSong.searchForSong(searchQuery);
				SoundCloudAPI::soundCloudAPIMap->insert_or_assign(guildId, soundCloudAPI);
				return returnValue;

			}
			else {
				shared_ptr<SoundCloudAPI> soundCloudAPI;
				if (SoundCloudAPI::discordGuildMap->contains(guildId)) {
					soundCloudAPI = make_shared<SoundCloudAPI>(guildId);
					auto returnValue = soundCloudAPI->theSong.searchForSong(searchQuery);
					SoundCloudAPI::soundCloudAPIMap->insert_or_assign(guildId, soundCloudAPI);
					return returnValue;
				}
				else {
					return vector<SoundCloudSong>();
				}
			}
		}
	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* SoundCloudAPI::sendAudioDataBufferMap{ new map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>() };
	map<string, shared_ptr<VoiceConnection>>* SoundCloudAPI::voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
	map<string, shared_ptr<SoundCloudAPI>>* SoundCloudAPI::soundCloudAPIMap{ new map<string, shared_ptr<SoundCloudAPI>>() };
	map<string, DiscordGuild*>* SoundCloudAPI::discordGuildMap{ new map<string, DiscordGuild*>() };
	string SoundCloudAPI::baseSearchURL02{ "https://api-v2.soundcloud.com/search?q=" };
	string SoundCloudAPI::baseSearchURL{ "https://soundcloud.com/search?q=" };
	string SoundCloudAPI::clientId{ "" };

};
#endif
