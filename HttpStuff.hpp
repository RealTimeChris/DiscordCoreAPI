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

	class HttpRequestAgent : ThreadContext,  public agent {
	public:

		static void initialize(string botTokenNew, string baseURLNew) {
			HttpRequestAgent::botToken = botTokenNew;
			HttpRequestAgent::baseURL = baseURLNew;
		};

		unbounded_buffer<HttpWorkload> workSubmissionBuffer;
		unbounded_buffer<HttpData> workReturnBuffer;
		static string botToken;
		static string baseURL;

		HttpRequestAgent(HttpAgentResources agentResources, shared_ptr<ThreadContext> threadContextNew)
			: agent(*threadContextNew->scheduler), ThreadContext(threadContextNew.get())
		{
			try {
				this->baseURL = HttpRequestAgent::baseURL;
				Filters::HttpBaseProtocolFilter filter;
				Filters::HttpCacheControl cacheControl{ nullptr };
				cacheControl = filter.CacheControl();
				cacheControl.ReadBehavior(Filters::HttpCacheReadBehavior::NoCache);
				cacheControl.WriteBehavior(Filters::HttpCacheWriteBehavior::NoCache);
				this->getHttpClient = HttpClient(filter);
				this->getHeaders = this->getHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->getHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->putHttpClient = HttpClient(filter);
				this->putHeaders = this->putHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->putHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->postHttpClient = HttpClient(filter);
				this->postHeaders = this->postHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->postHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->patchHttpClient = HttpClient(filter);
				this->patchHeaders = this->patchHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->patchHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->deleteHttpClient = HttpClient(filter);
				this->deleteHeaders = this->deleteHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->deleteHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				if (HttpRequestAgent::botToken != "") {
					this->botToken = HttpRequestAgent::botToken;
					hstring headerString = L"Bot ";
					hstring headerString2 = headerString + to_hstring(HttpRequestAgent::botToken);
					HttpCredentialsHeaderValue credentialValue(nullptr);
					credentialValue = credentialValue.Parse(headerString2.c_str());
					this->getHeaders.Authorization(credentialValue);
					this->putHeaders.Authorization(credentialValue);
					this->postHeaders.Authorization(credentialValue);
					this->patchHeaders.Authorization(credentialValue);
					this->deleteHeaders.Authorization(credentialValue);
				}
			}
			catch (hresult_error ex) {
				wcout << "Error: " << ex.message().c_str() << endl << endl;
			}
			catch (const exception& e) {
				cout << "HttpRequestAgent() Error: " << e.what() << endl << endl;
			}
		}

		HttpRequestAgent(HttpAgentResources agentResources) 
			:  agent(*HttpRequestAgent::ThreadContext::scheduler) , ThreadContext(DiscordCoreInternal::ThreadManager::getThreadContext().get().get())
		{
			try {
				this->baseURL = HttpRequestAgent::baseURL;
				Filters::HttpBaseProtocolFilter filter;
				Filters::HttpCacheControl cacheControl{ nullptr };
				cacheControl = filter.CacheControl();
				cacheControl.ReadBehavior(Filters::HttpCacheReadBehavior::NoCache);
				cacheControl.WriteBehavior(Filters::HttpCacheWriteBehavior::NoCache);
				this->getHttpClient = HttpClient(filter);
				this->getHeaders = this->getHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->getHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}				
				this->putHttpClient = HttpClient(filter);
				this->putHeaders = this->putHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->putHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->postHttpClient = HttpClient(filter);
				this->postHeaders = this->postHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->postHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->patchHttpClient = HttpClient(filter);
				this->patchHeaders = this->patchHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->patchHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				this->deleteHttpClient = HttpClient(filter);
				this->deleteHeaders = this->deleteHttpClient.DefaultRequestHeaders();
				if (agentResources.userAgent != L"") {
					this->deleteHeaders.UserAgent().TryParseAdd(agentResources.userAgent);
				}
				if (HttpRequestAgent::botToken != "") {
					this->botToken = HttpRequestAgent::botToken;
					hstring headerString = L"Bot ";
					hstring headerString2 = headerString + to_hstring(HttpRequestAgent::botToken);
					HttpCredentialsHeaderValue credentialValue(nullptr);
					credentialValue = credentialValue.Parse(headerString2.c_str());
					this->getHeaders.Authorization(credentialValue);
					this->putHeaders.Authorization(credentialValue);
					this->postHeaders.Authorization(credentialValue);
					this->patchHeaders.Authorization(credentialValue);
					this->deleteHeaders.Authorization(credentialValue);
				}
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
			HttpRequestAgent::ThreadContext::releaseGroup();
		}

	protected:
		static concurrent_unordered_map<string, RateLimitData> rateLimitData;
		static concurrent_unordered_map<HttpWorkloadType, string> rateLimitDataBucketValues;
		unbounded_buffer<exception> errorBuffer;

		static bool executeByRateLimitData(DiscordCoreInternal::RateLimitData* rateLimitDataNew) {
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
					if (rateLimitData.getsRemaining <= 0) {
						float loopStartTime = rateLimitData.timeStartedAt;
						float targetTime = loopStartTime + rateLimitData.msRemain;
						float currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
						float timeRemaining = targetTime - currentTime;
						if (timeRemaining > 0.0f) {
							if (rateLimitData.nextExecutionTime == 0) {
								rateLimitData.nextExecutionTime = currentTime;
							}
							rateLimitData.nextExecutionTime += timeRemaining;
							timeRemaining = (float)rateLimitData.nextExecutionTime - currentTime;
							cout << "Waiting on rate-limit, Time Remainiing: " << timeRemaining << "ms." << endl << endl;
							while (timeRemaining > 0.0f) {
								currentTime = static_cast<float>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
								timeRemaining = rateLimitData.nextExecutionTime - currentTime;
							}
							rateLimitData.nextExecutionTime = 0;
							rateLimitData.msRemain = 0.0f;
							rateLimitData.timeStartedAt = 0.0f;
						}
					}
					rateLimitData.getsRemaining -= 1;
					HttpData returnData;
					if (workload.workloadClass == HttpWorkloadClass::GET) {
						returnData = httpGETObjectData(workload.relativePath, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::POST) {
						returnData = httpPOSTObjectData(workload.relativePath, workload.content, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::PUT) {
						returnData = httpPUTObjectData(workload.relativePath, workload.content, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::PATCH) {
						returnData = httpPATCHObjectData(workload.relativePath, workload.content, &rateLimitData);
					}
					else if (workload.workloadClass == HttpWorkloadClass::DELETED) {
						returnData = httpDELETEObjectData(workload.relativePath, &rateLimitData);
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

		HttpData httpGETObjectData(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData getData;
			string connectionPath = HttpRequestAgent::baseURL + relativeURL;
			Uri requestUri = Uri(to_hstring(connectionPath.c_str()));
			HttpResponseMessage httpResponse;
			httpResponse = getHttpClient.GetAsync(requestUri).get();
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
				if (returnMessage.find("var ytInitialData = ") != string::npos) {
					string newString = returnMessage.substr(returnMessage.find("var ytInitialData = ") + to_string(L"var ytInitialData = ").length());
					const char* stringSequence = ";</script><script nonce=";
					newString = newString.substr(0, newString.find(stringSequence));
					jsonValue = jsonValue.parse(newString);
				}
				else {
					jsonValue = jsonValue.parse(returnMessage);
				}
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
				msRemainLocal = 10;
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
				getData = httpGETObjectData(relativeURL, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return getData;
		}

		HttpData httpPUTObjectData(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData putData;
			string connectionPath = HttpRequestAgent::baseURL + relativeURL;
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
				msRemainLocal = 10;
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
				putData = httpPUTObjectData(relativeURL, content, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return putData;
		}

		HttpData httpPOSTObjectData(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData postData;
			string connectionPath = HttpRequestAgent::baseURL+ relativeURL;
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
				msRemainLocal = 10;
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
				postData = httpPOSTObjectData(relativeURL, content, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return postData;
		}

		HttpData httpPATCHObjectData(string relativeURL, string content, RateLimitData* pRateLimitData) {
			HttpData patchData;
			string connectionPath = HttpRequestAgent::baseURL+ relativeURL;
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
				msRemainLocal = 10;
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
				patchData = httpPATCHObjectData(relativeURL, content, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return patchData;
		}

		HttpData httpDELETEObjectData(string relativeURL, RateLimitData* pRateLimitData) {
			HttpData deleteData;
			string connectionPath = HttpRequestAgent::baseURL + relativeURL;
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
				msRemainLocal = 10;
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
				deleteData = httpDELETEObjectData(relativeURL, pRateLimitData);
			}
			if (returnMessage != "") {
				if (jsonValue.contains("retry-after") && !jsonValue.at("retry-after").is_null()) {
					pRateLimitData->msRemain = jsonValue.at("retry-after") * 1000.0f;
				}
			}
			return deleteData;
		}

		Uri baseURI{ nullptr };
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
	string HttpRequestAgent::botToken;
	string HttpRequestAgent::baseURL;
}
#endif
