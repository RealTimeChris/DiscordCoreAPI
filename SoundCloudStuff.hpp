// SoundCloudStuff.hpp - Header for the "SoundCloud stuff".
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SOUNDCLOUD_STUFF_
#define _SOUNDCLOUD_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI {

	class SoundCloudAPI {
	public:
		SoundCloudAPI() {

		}

		vector<SoundCloudSearchResult> searchForSong(string songQuery) {
			collectClientId(songQuery);

			vector<SoundCloudSearchResult> results{};
		
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
			workload.relativePath = to_string(to_hstring(charString)) + "&facet=model&client_id=" + this->clientId + "&limit=20&offset=0&linked_partitioning=1&app_version=" + this->appVersion + "&app_locale=en";;
			curl_free(charString);
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "SoundCloudAPI::searchForSong() ");
			vector<string> assetPaths{};;
			cout << returnData.returnMessage << endl;

			for (auto value: returnData.data.at("collection")) {
				SoundCloudSearchResult result{};
				DiscordCoreInternal::DataParser::parseObject(value, &result);
				if (result.songTitle == "") {
					continue;
				}
				results.push_back(result);
			}

			for (auto value : results) {
				cout << value.description << endl;
				cout << value.duration << endl;
				cout << value.songTitle << endl;
				cout << value.thumbNailURL << endl;
				cout << value.songURL << endl;
				cout << value.songId << endl;
			}
			return results;
		}

		void collectClientId(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources{};
			agentResources.baseURL = this->baseSearchURL;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
			workload.relativePath = searchQuery;
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "SoundCloudAPI::searchForSong() ");
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
			DiscordCoreInternal::HttpData returnData02 = requestAgent02.submitWorkloadAndGetResult(workload02, "SoundCloudAPI::searchForSong() ");
			vector<DiscordCoreAPI::SoundCloudSearchResult> searchResults;
			string newString03 = returnData02.returnMessage.substr(returnData02.returnMessage.find("?client_id=") + to_string(L"?client_id=").size());
			string clientIdNew = newString03.substr(0, newString03.find("&"));
			this->clientId = clientIdNew;
			if (returnData.returnCode != 200) {
				cout << "SoundCloudAPI::searchForSong() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
			}
			return;
		}

	protected:
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

	};
}
#endif