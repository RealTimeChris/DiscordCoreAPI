// HttpStuff.hpp - Header for all of the HTTP related stuff.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_STUFF_
#define _HTTP_STUFF_

#include "../pch.h"
#include "JSONifier.hpp"

namespace DiscordCoreInternal {

	class HttpRequestAgent : public agent {
	public:

		static void initialize() {
			HttpRequestAgent::threadContext = ThreadManager::getThreadContext().get();
			return;
		};

		static shared_ptr<ThreadContext> threadContext;
		unbounded_buffer<HttpWorkload> workSubmissionBuffer;
		unbounded_buffer<HttpData> workReturnBuffer;

		HttpRequestAgent(HttpAgentResources agentResources) 
			: agent(*HttpRequestAgent::threadContext->scheduler)
		{
			try {
				this->groupId = HttpRequestAgent::threadContext->createGroup(*ThreadManager::getThreadContext().get());
				this->baseURL = agentResources.baseURL;
				this->botToken = agentResources.botToken;
				Filters::HttpBaseProtocolFilter filter;
				Filters::HttpCacheControl cacheControl{ nullptr };
				cacheControl = filter.CacheControl();
				cacheControl.ReadBehavior(Filters::HttpCacheReadBehavior::NoCache);
				cacheControl.WriteBehavior(Filters::HttpCacheWriteBehavior::NoCache);
				this->getHttpClient = HttpClient(filter);
				this->getHeaders = this->getHttpClient.DefaultRequestHeaders();
				this->putHttpClient = HttpClient(filter);
				this->putHeaders = this->putHttpClient.DefaultRequestHeaders();
				this->postHttpClient = HttpClient(filter);
				this->postHeaders = this->postHttpClient.DefaultRequestHeaders();
				this->patchHttpClient = HttpClient(filter);
				this->patchHeaders = this->patchHttpClient.DefaultRequestHeaders();
				this->deleteHttpClient = HttpClient(filter);
				this->deleteHeaders = this->deleteHttpClient.DefaultRequestHeaders();
				hstring headerString = L"Bot ";
				hstring headerString2 = headerString + this->botToken;
				HttpCredentialsHeaderValue credentialValue(nullptr);
				credentialValue = credentialValue.Parse(headerString2.c_str());
				this->getHeaders.Authorization(credentialValue);
				this->putHeaders.Authorization(credentialValue);
				this->postHeaders.Authorization(credentialValue);
				this->patchHeaders.Authorization(credentialValue);
				this->deleteHeaders.Authorization(credentialValue);
				return;
			}
			catch (hresult_error ex) {
				wcout << "Error: " << ex.message().c_str() << endl << endl;
			}
			catch (const exception& e) {
				cout << "HttpRequestAgent() Error: " << e.what() << endl << endl;
			}
		}

		bool getError(exception& error){
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		~HttpRequestAgent() {
			HttpRequestAgent::threadContext->releaseGroup(this->groupId);
		}

	protected:
		static concurrent_unordered_map<string, RateLimitData> rateLimitData;
		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		unbounded_buffer<exception> errorBuffer;

		bool executeByRateLimitData(DiscordCoreInternal::RateLimitData* rateLimitDataNew) {
			if (rateLimitDataNew->getsRemaining == 0) {
				float loopStartTime = rateLimitDataNew->timeStartedAt;
				float targetTime = loopStartTime + rateLimitDataNew->msRemain;
				float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
				float timeRemaining = currentTime - targetTime;
				if (timeRemaining <= -15000.0f) {
					cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining * -1.0f << "ms." << endl << endl;
					return true;
				}
				if (timeRemaining <= 0.0f) {
					cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining * -1.0f << "ms." << endl << endl;
					while (timeRemaining <= 0.0f) {
						currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						timeRemaining = currentTime - targetTime;
					}
					rateLimitDataNew->msRemain = 0.0f;
					rateLimitDataNew->timeStartedAt = 0.0f;
				}
			}
			return false;
		}
		
		void run() {
			try {
				transformer<HttpWorkload, HttpData> completeHttpRequest([this](HttpWorkload workload) -> HttpData {
					RateLimitData rateLimitData;
					rateLimitData.workloadType = workload.workloadType;
					auto bucketIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
					if (bucketIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
						string bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
						rateLimitData = HttpRequestAgent::rateLimitData.at(bucket);
					}
					if (rateLimitData.getsRemaining == 0) {
						float loopStartTime = rateLimitData.timeStartedAt;
						float targetTime = loopStartTime + rateLimitData.msRemain;
						float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						float timeRemaining = currentTime - targetTime;
						if (timeRemaining <= -15000.0f) {
							HttpData returnData;
							returnData.returnCode = 187;
							returnData.returnMessage = "Waiting on rate-limit, Time Remainiing: " + to_string(timeRemaining * -1.0f) + "ms";
							return returnData;
						}
						if (timeRemaining <= 0.0f) {
							cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining * -1.0f << "ms" << endl << endl;
							while (timeRemaining <= 0.0f) {
								currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
								timeRemaining = currentTime - targetTime;
							}
							rateLimitData.msRemain = 0.0f;
							rateLimitData.timeStartedAt = 0.0f;
						}
					}
					HttpData returnData;
					if (workload.workloadClass == HttpWorkloadClass::GET) {
						returnData = httpGETObjectDataAsync(workload.relativePath, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::POST) {
						returnData = httpPOSTObjectDataAsync(workload.relativePath, workload.content, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::PUT) {
						returnData = httpPUTObjectDataAsync(workload.relativePath, workload.content, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::PATCH) {
						returnData = httpPATCHObjectDataAsync(workload.relativePath, workload.content, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::DELETED) {
						returnData = httpDELETEObjectDataAsync(workload.relativePath, &rateLimitData);
					}
					auto bucketValueIterator = HttpRequestAgent::rateLimitDataBucketValues.find(workload.workloadType);
					if (bucketValueIterator != end(HttpRequestAgent::rateLimitDataBucketValues)) {
						HttpRequestAgent::rateLimitDataBucketValues.unsafe_erase(workload.workloadType);
					}
					HttpRequestAgent::rateLimitDataBucketValues.insert(make_pair(workload.workloadType, rateLimitData.bucket));
					auto rateLimitIterator = HttpRequestAgent::rateLimitData.find(rateLimitData.bucket);
					if (rateLimitIterator != end(HttpRequestAgent::rateLimitData)) {
						HttpRequestAgent::rateLimitData.unsafe_erase(rateLimitData.bucket);
					}
					HttpRequestAgent::rateLimitData.insert(make_pair(rateLimitData.bucket, rateLimitData));
					return returnData;
					});
				completeHttpRequest.link_target(&this->workReturnBuffer);
				HttpWorkload workload = receive(&this->workSubmissionBuffer);
				send(&completeHttpRequest, workload);
			}
			catch (const exception& e) {
				send(errorBuffer, e);
			}
			done();
		}

		HttpData httpGETObjectDataAsync(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData getData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpResponseMessage httpResponse;
			httpResponse = getHttpClient.TryGetAsync(requestUri).get().ResponseMessage();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			string returnMessage = to_string(httpResponse.Content().ReadAsStringAsync().get());
			getData.returnCode = (unsigned int)httpResponse.StatusCode();
			getData.returnMessage = returnMessage;
			json jsonValue;
			if (returnMessage != "") {
				jsonValue = jsonValue.parse(returnMessage);
			}
			getData.data = jsonValue;
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"retry-after").value().c_str()) * 1000;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				cout << "httpGETObjectDataAsync(), We've hit rate limit! Time Remaining: " << msRemainLocal << endl << endl;
				if (executeByRateLimitData(pRateLimitData)) {
					HttpData returnData;
					return returnData;
				}
				getData = httpGETObjectDataAsync(relativeURL, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return getData;
		}

		HttpData httpPUTObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData putData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentHeaderCollection contentHeaderCollection;
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			contentHeaderCollection.ContentDisposition(headerValue);
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			contentHeaderCollection.ContentType(typeHeaderValue);
			HttpStringContent contents(to_hstring(content), UnicodeEncoding::Utf8);
			contents.Headers().ContentDisposition(headerValue);
			contents.Headers().ContentType(typeHeaderValue);
			HttpResponseMessage httpResponse;
			httpResponse = putHttpClient.PutAsync(requestUri, contents).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			string returnMessage = to_string(httpResponse.Content().ReadAsStringAsync().get());
			putData.returnCode = (unsigned int)httpResponse.StatusCode();
			putData.returnMessage = returnMessage;
			json jsonValue;
			if (returnMessage != "") {
				jsonValue = jsonValue.parse(returnMessage);
			}
			putData.data = jsonValue;
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"retry-after").value().c_str()) * 1000;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				cout << "httpPUTObjectDataAsync(), We've hit rate limit! Time Remaining: " << msRemainLocal << endl << endl;
				if (executeByRateLimitData(pRateLimitData)) {
					HttpData returnData;
					return returnData;
				}
				putData = httpPUTObjectDataAsync(relativeURL, content, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return putData;
		}

		HttpData httpPOSTObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData postData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentHeaderCollection contentHeaderCollection;
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			contentHeaderCollection.ContentDisposition(headerValue);
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			contentHeaderCollection.ContentType(typeHeaderValue);
			HttpStringContent contents(to_hstring(content), UnicodeEncoding::Utf8);
			contents.Headers().ContentDisposition(headerValue);
			contents.Headers().ContentType(typeHeaderValue);
			HttpResponseMessage httpResponse;
			httpResponse = postHttpClient.PostAsync(requestUri, contents).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			string returnMessage = to_string(httpResponse.Content().ReadAsStringAsync().get());
			postData.returnCode = (unsigned int)httpResponse.StatusCode();
			postData.returnMessage = returnMessage;
			json jsonValue;
			if (returnMessage != "") {
				jsonValue = jsonValue.parse(returnMessage);
			}
			postData.data = jsonValue;
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"retry-after").value().c_str()) * 1000;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				cout << "httpPOSTObjectDataAsync(), We've hit rate limit! Time Remaining: " << msRemainLocal << endl << endl;
				if (executeByRateLimitData(pRateLimitData)) {
					HttpData returnData;
					return returnData;
				}
				postData = httpPOSTObjectDataAsync(relativeURL, content, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return postData;
		}

		HttpData httpPATCHObjectDataAsync(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData patchData;
			string connectionPath = to_string(this->baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpContentDispositionHeaderValue headerValue(L"payload_json");
			HttpMediaTypeHeaderValue typeHeaderValue(L"application/json");
			auto contentHeaderCollection = HttpRequestHeaderCollection(nullptr);
			HttpStringContent contents(to_hstring(content), UnicodeEncoding::Utf8);
			contents.Headers().ContentDisposition(headerValue);
			contents.Headers().ContentType(typeHeaderValue);
			HttpRequestMessage httpRequest;
			httpRequest.Method(HttpMethod::Patch());
			httpRequest.Content(contents);
			httpRequest.RequestUri(requestUri);
			HttpResponseMessage httpResponse;
			HttpCompletionOption completionOption;
			httpResponse = patchHttpClient.SendRequestAsync(httpRequest, completionOption).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			string returnMessage = to_string(httpResponse.Content().ReadAsStringAsync().get());
			patchData.returnCode = (unsigned int)httpResponse.StatusCode();
			patchData.returnMessage = returnMessage;
			json jsonValue;
			if (returnMessage != "") {
				jsonValue = jsonValue.parse(returnMessage);
			}
			patchData.data = jsonValue;
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"retry-after").value().c_str()) * 1000;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				cout << "httpPATCHObjectDataAsync(), We've hit rate limit! Time Remaining: " << msRemainLocal << endl << endl;
				if (executeByRateLimitData(pRateLimitData)) {
					HttpData returnData;
					return returnData;
				}
				patchData = httpPATCHObjectDataAsync(relativeURL, content, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return patchData;
		}

		HttpData httpDELETEObjectDataAsync(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData deleteData;
			string connectionPath = to_string(baseURL) + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpResponseMessage httpResponse;
			httpResponse = deleteHttpClient.DeleteAsync(requestUri).get();
			unsigned int getsRemainingLocal;
			float currentMSTimeLocal;
			float msRemainLocal;
			string bucket;
			currentMSTimeLocal = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			string returnMessage = to_string(httpResponse.Content().ReadAsStringAsync().get());
			deleteData.returnCode = (unsigned int)httpResponse.StatusCode();
			deleteData.returnMessage = returnMessage;
			json jsonValue;
			if (returnMessage != "") {
				jsonValue = jsonValue.parse(returnMessage);
			}
			deleteData.data = jsonValue;
			if (httpResponse.Headers().HasKey(L"x-ratelimit-remaining")) {
				getsRemainingLocal = stoi(httpResponse.Headers().TryLookup(L"x-ratelimit-remaining").value().c_str());
			}
			else {
				getsRemainingLocal = 0;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-reset-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"x-ratelimit-reset-after").value().c_str()) * 1000;
			}
			else {
				msRemainLocal = 250;
			}
			if (httpResponse.Headers().HasKey(L"x-ratelimit-bucket")) {
				bucket = to_string(httpResponse.Headers().TryLookup(L"x-ratelimit-bucket").value().c_str());
			}
			else {
				bucket = "";
			}
			if (httpResponse.Headers().HasKey(L"retry-after")) {
				msRemainLocal = stof(httpResponse.Headers().TryLookup(L"retry-after").value().c_str()) * 1000;
			}
			pRateLimitData->bucket = bucket;
			pRateLimitData->msRemain = msRemainLocal;
			pRateLimitData->timeStartedAt = currentMSTimeLocal;
			pRateLimitData->getsRemaining = getsRemainingLocal;
			if ((int)httpResponse.StatusCode() == 429) {
				cout << "httpDELETEObjectDataAsync(), We've hit rate limit! Time Remaining: " << msRemainLocal << endl << endl;
				if (executeByRateLimitData(pRateLimitData)) {
					HttpData returnData;
					return returnData;
				}
				deleteData = httpDELETEObjectDataAsync(relativeURL, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return deleteData;
		}
		
		unsigned int groupId;
		Uri baseURI{ nullptr };
		hstring baseURL = L"";
		hstring botToken = L"";
		HttpRequestHeaderCollection getHeaders{ nullptr };
		HttpRequestHeaderCollection putHeaders{ nullptr };
		HttpRequestHeaderCollection postHeaders{ nullptr };
		HttpRequestHeaderCollection patchHeaders{ nullptr };
		HttpRequestHeaderCollection deleteHeaders{ nullptr };
		HttpClient getHttpClient;
		HttpClient putHttpClient;
		HttpClient postHttpClient;
		HttpClient patchHttpClient;
		HttpClient deleteHttpClient;
	};
	concurrent_unordered_map<HttpWorkloadType, string> HttpRequestAgent::rateLimitDataBucketValues;
	concurrent_unordered_map<string, RateLimitData> HttpRequestAgent::rateLimitData;
	shared_ptr<ThreadContext> HttpRequestAgent::threadContext;
}
#endif
