// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpRequestAgent  {
	public:

		static void initialize(string);

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.find(rateLimitDataNew->bucket) != end(HttpRequestAgent::rateLimitData)) {
						rateLimitDataNew = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket);
					}
					else {
						HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->bucket, rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew->tempBucket));
					HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->tempBucket, rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataNew, true);
				returnType returnObject{};				
				DataParser::parseObject(returnData.data, &returnObject);
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			returnType returnObject{};
			return returnObject;
		}

		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.find(rateLimitDataNew->bucket) != end(HttpRequestAgent::rateLimitData)) {
						rateLimitDataNew = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket);
					}
					else {
						HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->bucket, rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew->tempBucket));
					HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->tempBucket, rateLimitDataNew));
				}
				HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataNew, true);
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.find(rateLimitDataNew->bucket) != end(HttpRequestAgent::rateLimitData)) {
						rateLimitDataNew = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket);
					}
					else {
						HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->bucket, rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew->tempBucket));
					HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->tempBucket, rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataNew, false);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return HttpData();
		}

	protected:

		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static concurrent_unordered_map<string, shared_ptr<RateLimitData>> rateLimitData;
		static string botToken;
		static string baseURL;

		static HttpRequestHeaderCollection deleteHeaders;
		static HttpRequestHeaderCollection patchHeaders;
		static HttpRequestHeaderCollection postHeaders;
		static HttpRequestHeaderCollection putHeaders;
		static HttpRequestHeaderCollection getHeaders;
		static HttpClient deleteHttpClient;
		static HttpClient patchHttpClient;
		static HttpClient postHttpClient;
		static HttpClient putHttpClient;
		static HttpClient getHttpClient;

		static HttpData executeByRateLimitData(HttpWorkloadData workload, shared_ptr<RateLimitData> rateLimitDataNew, bool printResult);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, shared_ptr<RateLimitData> pRateLimitData);

		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, shared_ptr<RateLimitData> pRateLimitData);

		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, shared_ptr<RateLimitData> pRateLimitData);

		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, shared_ptr<RateLimitData> pRateLimitData);

		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, shared_ptr<RateLimitData> pRateLimitData);

	};
}
