// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpRequestAgent {
	public:
		static void initialize(string);
		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.returnValue(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.returnValue(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.storeValue(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.storeValue(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.storeValue(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, true);
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
		void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.returnValue(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.returnValue(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.storeValue(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.storeValue(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.storeValue(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, true);
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return;
		}

		template<>
		HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.returnValue(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.returnValue(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.storeValue(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.storeValue(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.storeValue(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, false);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return HttpData();
		}

	protected:

		static DiscordCoreAPI::ObjectCache<HttpWorkloadType, string> rateLimitDataBucketValues;
		static DiscordCoreAPI::ObjectCache<string, unique_ptr<RateLimitData>> rateLimitData;
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

		static HttpData executeByRateLimitData(HttpWorkloadData workload, RateLimitData* rateLimitDataNew, bool printResult);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
	};
}