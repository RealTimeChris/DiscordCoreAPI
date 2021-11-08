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

		HttpRequestAgent();

		HttpRequestAgent(HttpAgentResources agentResources);

		static void initialize(string botTokenNew);

		template<typename returnType>
		returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			mutex workloadMutex{};
			try {
				workloadMutex.lock();
				RateLimitData rateLimitDataNew{};
				rateLimitDataNew.workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket);
					if (HttpRequestAgent::workloadMap.find(bucket) == end(HttpRequestAgent::workloadMap)) {
						HttpRequestAgent::workloadMap.insert(make_pair(bucket, concurrent_queue<HttpWorkloadData>()));
					}
					HttpRequestAgent::workloadMap.at(bucket).push(workload);
				}
				if (rateLimitDataNew.getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew.timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew.msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew.nextExecutionTime == 0) {
							rateLimitDataNew.nextExecutionTime = currentTime;
						}
						rateLimitDataNew.nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew.nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew.nextExecutionTime - currentTime;
						}
						rateLimitDataNew.nextExecutionTime = 0;
						rateLimitDataNew.msRemain = 0.0f;
						rateLimitDataNew.timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew.getsRemaining -= 1;
				if (rateLimitDataNew.bucket != "") {
					HttpRequestAgent::workloadMap.at(rateLimitDataNew.bucket).try_pop(workload);
				}
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, &rateLimitDataNew);
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew.bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew.bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew.bucket, rateLimitDataNew));
				if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
					cout << workload.callStack + " Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				else {
					cout << workload.callStack + " Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				workloadMutex.unlock();
				returnType returnObject{};
				DataParser::parseObject(returnData.data, &returnObject);
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult() Error: ");
			}
			workloadMutex.unlock();
			returnType returnObject{};
			return returnObject;
		}

		template<>
		void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			mutex workloadMutex{};
			try {
				workloadMutex.lock();
				RateLimitData rateLimitDataNew{};
				rateLimitDataNew.workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket);
					if (HttpRequestAgent::workloadMap.find(bucket) == end(HttpRequestAgent::workloadMap)) {
						HttpRequestAgent::workloadMap.insert(make_pair(bucket, concurrent_queue<HttpWorkloadData>()));
					}
					HttpRequestAgent::workloadMap.at(bucket).push(workload);
				}
				if (rateLimitDataNew.getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew.timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew.msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew.nextExecutionTime == 0) {
							rateLimitDataNew.nextExecutionTime = currentTime;
						}
						rateLimitDataNew.nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew.nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew.nextExecutionTime - currentTime;
						}
						rateLimitDataNew.nextExecutionTime = 0;
						rateLimitDataNew.msRemain = 0.0f;
						rateLimitDataNew.timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew.getsRemaining -= 1;
				if (rateLimitDataNew.bucket != "") {
					HttpRequestAgent::workloadMap.at(rateLimitDataNew.bucket).try_pop(workload);
				}
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, &rateLimitDataNew);
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew.bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew.bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew.bucket, rateLimitDataNew));
				if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
					cout << workload.callStack + " Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				else {
					cout << workload.callStack + " Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
				}
				workloadMutex.unlock();
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult() Error: ");
			}
			workloadMutex.unlock();
			return;
		}

		template<>
		HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			mutex workloadMutex{};
			try {
				workloadMutex.lock();
				RateLimitData rateLimitDataNew{};
				rateLimitDataNew.workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew = HttpRequestAgent::rateLimitData.at(bucket);
					if (HttpRequestAgent::workloadMap.find(bucket) == end(HttpRequestAgent::workloadMap)) {
						HttpRequestAgent::workloadMap.insert(make_pair(bucket, concurrent_queue<HttpWorkloadData>()));
					}
					HttpRequestAgent::workloadMap.at(bucket).push(workload);
				}
				if (rateLimitDataNew.getsRemaining <= 0) {
					float loopStartTime = rateLimitDataNew.timeStartedAt;
					float targetTime = loopStartTime + rateLimitDataNew.msRemain;
					float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
					float timeRemaining = targetTime - currentTime;
					if (timeRemaining > 0.0f) {
						if (rateLimitDataNew.nextExecutionTime == 0) {
							rateLimitDataNew.nextExecutionTime = currentTime;
						}
						rateLimitDataNew.nextExecutionTime += timeRemaining;
						timeRemaining = (float)rateLimitDataNew.nextExecutionTime - currentTime;
						cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
						while (timeRemaining > 0.0f) {
							currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
							timeRemaining = rateLimitDataNew.nextExecutionTime - currentTime;
						}
						rateLimitDataNew.nextExecutionTime = 0;
						rateLimitDataNew.msRemain = 0.0f;
						rateLimitDataNew.timeStartedAt = 0.0f;
					}
				}
				rateLimitDataNew.getsRemaining -= 1;
				if (rateLimitDataNew.bucket != "") {
					HttpRequestAgent::workloadMap.at(rateLimitDataNew.bucket).try_pop(workload);
				}
				HttpData returnData{};
				switch (workload.workloadClass) {
				case HttpWorkloadClass::GET: {
					returnData = httpGETObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::POST: {
					returnData = httpPOSTObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::PUT: {
					returnData = httpPUTObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::PATCH: {
					returnData = httpPATCHObjectData(workload, &rateLimitDataNew);
					break;
				}
				case HttpWorkloadClass::DELETE: {
					returnData = httpDELETEObjectData(workload, &rateLimitDataNew);
					break;
				}
				default: {
					break;
				}
				}
				HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
				HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitDataNew.bucket);
				HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitDataNew.bucket));
				HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitDataNew.bucket, rateLimitDataNew));
				workloadMutex.unlock();
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult() Error: ");
			}
			workloadMutex.unlock();
			return HttpData();
		}

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

		bool executeByRateLimitData(RateLimitData* rateLimitDataNew);

		HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

		HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);

	};
}
