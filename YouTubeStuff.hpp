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
	
	enum class VideoTypes {
		_3PG = 0,
		FLV = 1,
		MP4 = 2,
		WEBM = 3
	};

	enum class Resolution {
		NA = 0,
		_144P = 144,
		_240P = 240,
		_270_360P = 270,
		_360P = 360,
		_480P = 480,
		_720P = 720,
		_1080P = 1080,
		_3072P = 3072
	};

	bool validateId(string Id) {
		regex idRegex("^[a - zA - Z0 - 9 - _]{11}");
		if (regex_search(Id, idRegex)) {
			return true;
		}
		else { 
			return false;
		}
	}

	struct SearchOptions {
		unsigned int limit{ 100 };
		bool safeSearch{ false };
	};

	class YouTubeAPI {
	public:

		hstring userAgentString = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";
		hstring baseSearchURL = L"https://www.youtube.com/results?search_query=";
		hstring baseURL = L"https://www.youtube.com/watch?v=";

		vector<YouTubeSearchResult> searchForVideo(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = baseSearchURL;
			CURLU* urlHandle = curl_url();
			curl_url_set(urlHandle, CURLUPart::CURLUPART_QUERY, searchQuery.c_str(), CURLU_URLENCODE);
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::YOUTUBE_SEARCH;
			char* charString;
			curl_url_get(urlHandle, CURLUPart::CURLUPART_QUERY, &charString, 0);
			workload.relativePath = charString;
			curl_free(charString);
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreInternal::HttpData returnData = receive(requestAgent.workReturnBuffer);
			vector<DiscordCoreAPI::YouTubeSearchResult> searchResults;
			json partialSearchResultsJson = json::parse(to_string(returnData.data));
			cout << partialSearchResultsJson.dump() << endl;
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

		vector<string> ageRestrictedURLs{
			"support.google.com/youtube/?p=age_restrictions",
				"youtube.com/t/community_guidelines"
		};
		
		fstream downloadVideo() {


		}

	protected:
		static string youTubeAPIKey;
	};
	string YouTubeAPI::youTubeAPIKey;
}

#endif