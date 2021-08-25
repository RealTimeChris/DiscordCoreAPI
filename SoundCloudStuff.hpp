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

		vector<SoundCloudSearchResult> searchForSong(string searchQuery) {
			DiscordCoreInternal::HttpAgentResources agentResources;
			agentResources.baseURL = this->baseURL;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::SOUNDCLOUD_SEARCH;
			workload.relativePath += searchQuery;
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			DiscordCoreInternal::HttpData returnData = receive(requestAgent.workReturnBuffer);
			vector<DiscordCoreAPI::SoundCloudSearchResult> searchResults;
			json partialSearchResultsJson = returnData.data;
			cout << partialSearchResultsJson << endl;
			if (returnData.returnCode != 200) {
				cout << "SoundCloudAPI::searchForSong() Error 01: " << returnData.returnCode << returnData.returnMessage.c_str() << endl;
			}
			return searchResults;
		}
	protected:
		string baseURL{ "https://soundcloud.com/search?q=" };

	};

}

#endif