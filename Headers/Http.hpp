// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpRequestAgent  {
	public:

		static void initialize(string botTokenNew);

		static DiscordCoreAPI::CoRoutine<void> run(DiscordCoreAPI::RateLimitData* rateLimitDataNew);

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			atomic<shared_ptr<DiscordCoreAPI::RateLimitData>> rateLimitDataNew{ shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData()) };
			try {
				string bucket{};
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
				}
				string nonce = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
				rateLimitDataNew.load()->inputBuffer.insert(make_pair(nonce, workload));
				if (rateLimitDataNew.load()->theTask.getStatus() != DiscordCoreAPI::CoRoutineStatus::Running) {
					rateLimitDataNew.load()->theTask = HttpRequestAgent::run(rateLimitDataNew.load().get());
				}
				while (!rateLimitDataNew.load()->returnBuffer.contains(nonce)) {
					concurrency::wait(15);
				}
				HttpRequestAgent::rateLimitData.unsafe_erase(bucket);
				HttpRequestAgent::rateLimitData.insert(make_pair(bucket, shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData(*rateLimitDataNew.load()))));
				HttpData returnData = rateLimitDataNew.load()->returnBuffer[nonce];
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
			atomic<shared_ptr<DiscordCoreAPI::RateLimitData>> rateLimitDataNew{ shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData()) };
			try {
				string bucket{};
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
				}
				string nonce = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
				rateLimitDataNew.load()->inputBuffer.insert(make_pair(nonce, workload));
				if (rateLimitDataNew.load()->theTask.getStatus() != DiscordCoreAPI::CoRoutineStatus::Running) {
					rateLimitDataNew.load()->theTask = HttpRequestAgent::run(rateLimitDataNew.load().get());
				}
				while (!rateLimitDataNew.load()->returnBuffer.contains(nonce)) {
					concurrency::wait(15);
				}
				HttpRequestAgent::rateLimitData.unsafe_erase(bucket);
				HttpRequestAgent::rateLimitData.insert(make_pair(bucket, shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData(*rateLimitDataNew.load()))));
				HttpData returnData = rateLimitDataNew.load()->returnBuffer[nonce];
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			atomic<shared_ptr<DiscordCoreAPI::RateLimitData>> rateLimitDataNew{ shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData()) };
			try {
				string bucket{};
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
				}
				string nonce = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
				rateLimitDataNew.load()->inputBuffer.insert(make_pair(nonce, workload));
				if (rateLimitDataNew.load()->theTask.getStatus() != DiscordCoreAPI::CoRoutineStatus::Running) {
					rateLimitDataNew.load()->theTask = HttpRequestAgent::run(rateLimitDataNew.load().get());
				}
				while (!rateLimitDataNew.load()->returnBuffer.contains(nonce)) {
					concurrency::wait(15);
				}
				HttpRequestAgent::rateLimitData.unsafe_erase(bucket);
				HttpRequestAgent::rateLimitData.insert(make_pair(bucket, shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData(*rateLimitDataNew.load()))));
				HttpData returnData = rateLimitDataNew.load()->returnBuffer[nonce];
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return HttpData();
		}
		/*
		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			atomic<shared_ptr<DiscordCoreAPI::RateLimitData>> rateLimitDataNew{ shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData()) };
			unique_lock<mutex>* workloadLock{ nullptr };
			try {
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
					workloadLock = new unique_lock<mutex>(*rateLimitDataNew.load()->condMutex, defer_lock_t());
					if (!workloadLock->try_lock()) {
						rateLimitDataNew.load()->isItUnlocked = false;
						rateLimitDataNew.load()->condVar->wait(*workloadLock, [&]() {return rateLimitDataNew.load()->isItUnlocked; });
						workloadLock->unlock();
					}
					workloadLock->unlock();
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
				if (workloadLock != nullptr) {
					workloadLock->unlock();
				}
				rateLimitDataNew.load()->isItUnlocked = true;
				rateLimitDataNew.load()->condVar->notify_one();
				return;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			if (workloadLock != nullptr) {
				workloadLock->unlock();
			}
			rateLimitDataNew.load()->isItUnlocked = true;
			rateLimitDataNew.load()->condVar->notify_one();
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			atomic<shared_ptr<DiscordCoreAPI::RateLimitData>> rateLimitDataNew{ shared_ptr<DiscordCoreAPI::RateLimitData>(new DiscordCoreAPI::RateLimitData()) };
			unique_lock<mutex>* workloadLock{ nullptr };
			try {
				rateLimitDataNew.load()->workloadType = workload.workloadType;
				auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
				if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
					string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					rateLimitDataNew.store(HttpRequestAgent::rateLimitData.at(bucket));
					workloadLock = new unique_lock<mutex>(*rateLimitDataNew.load()->condMutex, defer_lock_t());
					if (!workloadLock->try_lock()) {
						rateLimitDataNew.load()->isItUnlocked = false;
						rateLimitDataNew.load()->condVar->wait(*workloadLock, [&]() {return rateLimitDataNew.load()->isItUnlocked; });
						workloadLock->unlock();
					}
					workloadLock->unlock();
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
				if (workloadLock != nullptr) {
					workloadLock->unlock();
				}
				rateLimitDataNew.load()->isItUnlocked = true;
				rateLimitDataNew.load()->condVar->notify_one();
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			if (workloadLock != nullptr) {
				workloadLock->unlock();
			}
			rateLimitDataNew.load()->isItUnlocked = true;
			rateLimitDataNew.load()->condVar->notify_one();
			return HttpData();
		}
*/
	protected:

		static concurrent_unordered_map<string, atomic<shared_ptr<DiscordCoreAPI::RateLimitData>>> rateLimitData;
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

		static bool executeByRateLimitData(DiscordCoreAPI::RateLimitData* rateLimitDataNew);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, DiscordCoreAPI::RateLimitData* pRateLimitData);

		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, DiscordCoreAPI::RateLimitData* pRateLimitData);

		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, DiscordCoreAPI::RateLimitData* pRateLimitData);

		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, DiscordCoreAPI::RateLimitData* pRateLimitData);

		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, DiscordCoreAPI::RateLimitData* pRateLimitData);

	};
}
