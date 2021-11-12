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
			mutex workloadMutex{};
			scoped_lock<mutex> workloadLock{ workloadMutex };
			shared_ptr<RateLimitData> rateLimitDataNew{ new RateLimitData() };
			try {
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket);
					rateLimitDataNew->theSemaphore->acquire();
				}
				if (rateLimitDataNew->getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew->timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew->msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew->nextExecutionTime == 0) {
							rateLimitDataNew->nextExecutionTime = currentTime;
						}
						rateLimitDataNew->nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew->nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew->nextExecutionTime - currentTime;
						}
						rateLimitDataNew->nextExecutionTime = 0;
						rateLimitDataNew->msRemain = 0.0f;
						rateLimitDataNew->timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew->getsRemaining -= 1;
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, rateLimitDataNew.get());
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew->bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew->bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->bucket, rateLimitDataNew));
				if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
					cout << workload.callStack + " Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				else {
					cout << workload.callStack + " Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}	
				returnType returnObject{};
				DataParser::parseObject(returnData.data, &returnObject);
				rateLimitDataNew->theSemaphore->release();
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			returnType returnObject{};
			rateLimitDataNew->theSemaphore->release();
			return returnObject;
		}

		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			mutex workloadMutex{};
			scoped_lock<mutex> workloadLock{ workloadMutex };
			shared_ptr<RateLimitData> rateLimitDataNew{ new RateLimitData() };
			try {
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket);
					rateLimitDataNew->theSemaphore->acquire();
				}
				if (rateLimitDataNew->getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew->timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew->msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew->nextExecutionTime == 0) {
							rateLimitDataNew->nextExecutionTime = currentTime;
						}
						rateLimitDataNew->nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew->nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew->nextExecutionTime - currentTime;
						}
						rateLimitDataNew->nextExecutionTime = 0;
						rateLimitDataNew->msRemain = 0.0f;
						rateLimitDataNew->timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew->getsRemaining -= 1;
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, rateLimitDataNew.get());
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew->bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew->bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->bucket, rateLimitDataNew));
				if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
					cout << workload.callStack + " Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				else {
					cout << workload.callStack + " Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				rateLimitDataNew->theSemaphore->release();
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			rateLimitDataNew->theSemaphore->release();
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			mutex workloadMutex{};
			scoped_lock<mutex> workloadLock{ workloadMutex };
			shared_ptr<RateLimitData> rateLimitDataNew{ new RateLimitData() };
			try {
				if (HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType) != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket);
					rateLimitDataNew->theSemaphore->acquire();
				}
				if (rateLimitDataNew->getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew->timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew->msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew->nextExecutionTime == 0) {
							rateLimitDataNew->nextExecutionTime = currentTime;
						}
						rateLimitDataNew->nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew->nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew->nextExecutionTime - currentTime;
						}
						rateLimitDataNew->nextExecutionTime = 0;
						rateLimitDataNew->msRemain = 0.0f;
						rateLimitDataNew->timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew->getsRemaining -= 1;
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, rateLimitDataNew.get());
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, rateLimitDataNew.get());
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew->bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew->bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew->bucket, rateLimitDataNew));
				rateLimitDataNew->theSemaphore->release();
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			rateLimitDataNew->theSemaphore->release();
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
		string baseURLInd{ "" };

		static bool executeByRateLimitData(RateLimitData* rateLimitDataNew);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

	};
}
