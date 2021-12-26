// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	int32_t parseCode(string rawString);

	class HttpClient;

	struct HttpResponseData {

		friend size_t writeDataCallBack(char* ptr, size_t size, size_t nmemb, void* userData);
		friend size_t readDataCallBack(char* ptr, size_t size, size_t nmemb, void* userData);
		friend class HttpClient;

		map<string, string> headers{};
		int32_t responseCode{ 0 };
		string contentReal{ "" };
	protected:
		string content{};
	};

	struct HttpInputData {
		int32_t writtenBytes{ 0 };
		string theData{};
	};

	class HttpGetClient {
	public:
		HttpGetClient();

		HttpResponseData executeHttpRequest(string baseUrl, string relativePath, map<string, string> headers);

	protected:
		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket() };
		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr };
		unique_ptr<SSL, SSLDeleter> ssl{ nullptr };
		vector<char> inputBuffer{};
		fd_set readfds{};
	};

	class HttpClient {
	public:

		HttpClient& operator=(HttpClient&& other);

		HttpClient(HttpClient&& other);

		HttpClient& operator=(HttpClient& other) = delete;

		HttpClient(HttpClient& other) = delete;

		HttpClient() = default;

		HttpClient(string, HttpWorkloadClass workloadType);

		static HttpResponseData executeHttpRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		static void addHeader(string, string);

		static void removeHeader(string);

		static map<string, string> getHeaders();

		~HttpClient();

	protected:		

		static unique_ptr<curl_slist> headerList;
		static map<string, string> headers;
		static mutex accessMutex;
	};

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
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
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
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
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
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
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

		static DiscordCoreAPI::map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static DiscordCoreAPI::map<string, unique_ptr<RateLimitData>> rateLimitData;
		static string botToken;
		static string baseURL;

		static HttpClient deleteClient;
		static HttpClient patchClient;
		static HttpClient postClient;
		static HttpClient putClient;
		static HttpClient getClient;

		static HttpData executeByRateLimitData(HttpWorkloadData workload, RateLimitData* rateLimitDataNew, bool printResult);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
	};
}