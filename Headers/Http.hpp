// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "CoRoutine.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpRequestAgent  {
	public:

		HttpRequestAgent();

		HttpRequestAgent(HttpAgentResources agentResources);

		static void initialize(string botTokenNew);

		HttpData submitWorkloadAndGetResult(HttpWorkloadData workload);

		~HttpRequestAgent();

	protected:

		static concurrent_unordered_map<string, concurrent_queue<HttpWorkloadData>> workloadMap;
		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static concurrent_unordered_map<string, RateLimitData> rateLimitData;
		static string botToken;
		static string baseURL;

		HttpRequestHeaderCollection deleteHeaders{ nullptr };
		HttpRequestHeaderCollection patchHeaders{ nullptr };
		HttpRequestHeaderCollection postHeaders{ nullptr };
		HttpRequestHeaderCollection putHeaders{ nullptr };
		HttpRequestHeaderCollection getHeaders{ nullptr };
		HttpClient deleteHttpClient{ nullptr };
		HttpClient patchHttpClient{ nullptr };
		HttpClient postHttpClient{ nullptr };
		HttpClient putHttpClient{ nullptr };
		HttpClient getHttpClient{ nullptr };
		string baseURLInd{ "" };

		bool executeByRateLimitData(DiscordCoreInternal::RateLimitData* rateLimitDataNew);

		HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

	};
}
