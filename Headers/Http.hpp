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

		static void initialize(string botTokenNew);

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			try {
				RateLimitData* rateLimitDataNew{ new RateLimitData() };
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.find(bucket) != end(HttpRequestAgent::rateLimitData)) {
						rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket).get();
					}
					else {
						string tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
						HttpRequestAgent::rateLimitData.insert(make_pair(tempBucket, make_unique<RateLimitData>(*rateLimitDataNew)));
						HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, tempBucket));
					}
				}
				else {
					string tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitData.insert(make_pair(tempBucket, make_unique<RateLimitData>(*rateLimitDataNew)));
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, tempBucket));
				}
				auto returnData = HttpRequestAgent::executeByRateLimitData(*rateLimitDataNew, workload, true);
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
				RateLimitData* rateLimitDataNew{ new RateLimitData() };
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.find(bucket) != end(HttpRequestAgent::rateLimitData)) {
						rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket).get();
					}
					else {
						string tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
						HttpRequestAgent::rateLimitData.insert(make_pair(tempBucket, make_unique<RateLimitData>(*rateLimitDataNew)));
						HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, tempBucket));
					}
				}
				else {
					string tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitData.insert(make_pair(tempBucket, make_unique<RateLimitData>(*rateLimitDataNew)));
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, tempBucket));
				}
				auto returnData = HttpRequestAgent::executeByRateLimitData(*rateLimitDataNew, workload, true);
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
				RateLimitData* rateLimitDataNew{ new RateLimitData() };
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.find(bucket) != end(HttpRequestAgent::rateLimitData)) {
						rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket).get();
					}
					else {
						string tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
						HttpRequestAgent::rateLimitData.insert(make_pair(tempBucket, make_unique<RateLimitData>(*rateLimitDataNew)));
						HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, tempBucket));
					}
				}
				else {
					string tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitData.insert(make_pair(tempBucket, make_unique<RateLimitData>(*rateLimitDataNew)));
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, tempBucket));
				}
				auto returnData = HttpRequestAgent::executeByRateLimitData(*rateLimitDataNew, workload, false);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return HttpData();
		}

	protected:

		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static concurrent_unordered_map<string, unique_ptr<RateLimitData>> rateLimitData;
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

		static HttpData executeByRateLimitData(RateLimitData rateLimitDataNew, HttpWorkloadData workload, bool printResult);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

	};
}
