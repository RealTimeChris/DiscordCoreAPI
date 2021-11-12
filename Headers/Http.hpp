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
			atomic<shared_ptr<RateLimitData>> rateLimitDataNew{ shared_ptr<RateLimitData>(new RateLimitData()) };
			try {
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
					rateLimitDataNew.load()->theSemaphore.acquire();
				}
				if (rateLimitDataNew.load()->getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew.load()->timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew.load()->msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew.load()->nextExecutionTime == 0) {
							rateLimitDataNew.load()->nextExecutionTime = currentTime;
						}
						rateLimitDataNew.load()->nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew.load()->nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew.load()->nextExecutionTime - currentTime;
						}
						rateLimitDataNew.load()->nextExecutionTime = 0;
						rateLimitDataNew.load()->msRemain = 0.0f;
						rateLimitDataNew.load()->timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew.load()->getsRemaining -= 1;
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew.load()->bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew.load()->bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew.load()->bucket, rateLimitDataNew.load()));
				if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
					cout << workload.callStack + " Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				else {
					cout << workload.callStack + " Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}	
				returnType returnObject{};
				DataParser::parseObject(returnData.data, &returnObject);
				rateLimitDataNew.load()->theSemaphore.release();
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			returnType returnObject{};
			rateLimitDataNew.load()->theSemaphore.release();
			return returnObject;
		}

		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			atomic<shared_ptr<RateLimitData>> rateLimitDataNew{ shared_ptr<RateLimitData>(new RateLimitData()) };
			try {
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
					rateLimitDataNew.load()->theSemaphore.acquire();
				}
				if (rateLimitDataNew.load()->getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew.load()->timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew.load()->msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew.load()->nextExecutionTime == 0) {
							rateLimitDataNew.load()->nextExecutionTime = currentTime;
						}
						rateLimitDataNew.load()->nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew.load()->nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew.load()->nextExecutionTime - currentTime;
						}
						rateLimitDataNew.load()->nextExecutionTime = 0;
						rateLimitDataNew.load()->msRemain = 0.0f;
						rateLimitDataNew.load()->timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew.load()->getsRemaining -= 1;
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew.load()->bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew.load()->bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew.load()->bucket, rateLimitDataNew.load()));
				if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
					cout << workload.callStack + " Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				else {
					cout << workload.callStack + " Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				rateLimitDataNew.load()->theSemaphore.release();
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			rateLimitDataNew.load()->theSemaphore.release();
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			atomic<shared_ptr<RateLimitData>> rateLimitDataNew{ shared_ptr<RateLimitData>(new RateLimitData()) };
			try {
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
					rateLimitDataNew.load()->theSemaphore.acquire();
				}
				if (rateLimitDataNew.load()->getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew.load()->timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew.load()->msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew.load()->nextExecutionTime == 0) {
							rateLimitDataNew.load()->nextExecutionTime = currentTime;
						}
						rateLimitDataNew.load()->nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew.load()->nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew.load()->nextExecutionTime - currentTime;
						}
						rateLimitDataNew.load()->nextExecutionTime = 0;
						rateLimitDataNew.load()->msRemain = 0.0f;
						rateLimitDataNew.load()->timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew.load()->getsRemaining -= 1;
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, rateLimitDataNew.load().get());
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew.load()->bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew.load()->bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew.load()->bucket, rateLimitDataNew.load()));
				rateLimitDataNew.load()->theSemaphore.release();
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			rateLimitDataNew.load()->theSemaphore.release();
			return HttpData();
		}

	protected:

		static concurrent_unordered_map<string, atomic<shared_ptr<RateLimitData>>> rateLimitData;
		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
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
		string baseURLInd{ "" };

		static bool executeByRateLimitData(RateLimitData* rateLimitDataNew);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

	};
}
