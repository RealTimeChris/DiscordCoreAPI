// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_STUFF_
#define _HTTP_STUFF_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpRequestAgent : public agent {
	public:

		static shared_ptr<ThreadContext> threadContext;

		HttpRequestAgent();

		HttpRequestAgent(HttpAgentResources agentResources);

		static void initialize(string botTokenNew, string baseURLNew);

		HttpData submitWorkloadAndGetResult(HttpWorkloadData workload);

		~HttpRequestAgent();

	protected:

		static map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static map<string, RateLimitData> rateLimitData;
		static string botToken;
		static string baseURL;

		unbounded_buffer<HttpWorkloadData> workSubmissionBuffer{ nullptr };
		unbounded_buffer<HttpData> workReturnBuffer{ nullptr };
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

		static bool executeByRateLimitData(DiscordCoreInternal::RateLimitData* rateLimitDataNew);

		void run();

		HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPUTObjectData(string relativeURL, string content, RateLimitData* pRateLimitData);

		HttpData httpPOSTObjectData(string relativeURL, string content, RateLimitData* pRateLimitData);

		HttpData httpPATCHObjectData(string relativeURL, string content, RateLimitData* pRateLimitData);

		HttpData httpDELETEObjectData(string relativeURL, HttpWorkloadType workloadType, RateLimitData* pRateLimitData);

	};
}
#endif
