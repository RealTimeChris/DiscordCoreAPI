// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	const string soundcloudCertPath{ "C:/Program Files/Common Files/SSL/certs/Root-R3.pem" };
	const string youtubeCertPath{ "C:/Program Files/Common Files/SSL/certs/gtsr1.pem" };
	const string googleCertPath{ "C:/Program Files/Common Files/SSL/certs/gtsr1.pem" };

	class HttpHeader {
	public:

		HttpHeader(nullptr_t) {};

		HttpHeader(string key, string value){
			this->stringValue = value;
			this->key = key;
		}

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, map<string, shared_ptr<HttpHeader>> headers) {
			if (headers.contains("x-ratelimit-retry-after")) {
				pRateLimitData->msRemain = static_cast<int64_t>(static_cast<float>(stoll(headers.at("x-ratelimit-retry-after")->stringValue)) * 1000.0f);
			}
			else if (headers.contains("x-ratelimit-remaining")) {
				pRateLimitData->getsRemaining = stol(headers.at("x-ratelimit-remaining")->stringValue);
			}
			else if(headers.contains("x-ratelimit-limit")) {
				pRateLimitData->totalGets = stol(headers.at("x-ratelimit-limit")->stringValue.c_str());
			}
			else if (headers.contains("x-ratelimit-reset-after")) {
				pRateLimitData->msRemain = static_cast<int64_t>(stod(headers.at("x-ratelimit-reset-after")->stringValue) * 1000.0f);
			}
			else if (headers.contains("x-ratelimit-bucket")) {
				pRateLimitData->bucket = headers.at("x-ratelimit-bucket")->stringValue;
			}
		};

		virtual ~HttpHeader() {};

		string stringValue{ "" };
		string key{ "" };
	};

	class HttpRnRBuilder;

	class DiscordCoreAPI_Dll HttpConnection {
	public:

		HttpConnection(string baseUrl, string relativePath);

		bool sendRequest(string request);

		HttpRnRBuilder getResponse();

		bool connect();

	protected:

		BIOWrapper connectionBio{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		vector<char> inputBuffer{};
		SSLWrapper ssl{ nullptr };
		string relativePath{ "" };
		string baseUrl{ "" };
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend class HttpConnection;
		friend class HttpClient;

		string contentFinalReal{ "" };
		int64_t responseCode{ -1 };

		static string buildRequest(string& baseUrl, string& relativePath, string& content, map<string, string>& headers, HttpWorkloadClass workloadClass);

		HttpData handleHeaders(HttpWorkloadData& workloadData, shared_ptr<RateLimitData> pRateLimitData);

		map<string, shared_ptr<HttpHeader>> getResponseHeaders();

	protected:

		map<string, shared_ptr<HttpHeader>> headers{};
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		int64_t contentSize{ -1 };
		bool isItChunked{ false };
		string rawInput{ "" };

		bool checkForHeadersToParse();

		void parseHeaders();

		bool parseChunk();

		void parseSize();

		void clearCRLF();

		void parseCode();
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:

		friend class HttpRnRBuilder;

		static HttpRnRBuilder executeHttpRequest(string& baseUrl, string& relativePath, string& content, map<string, string>& headers, HttpWorkloadClass workloadClass);

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpClient::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpClient::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpClient::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataNew = HttpClient::rateLimitData.at(rateLimitDataNew->bucket).load();
					}
					else {
						HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->bucket, rateLimitDataNew);
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());
					HttpClient::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->tempBucket, rateLimitDataNew);
				}
				HttpData returnData = HttpClient::executeByRateLimitData(workload, rateLimitDataNew, true);
				returnType returnObject{};
				DataParser::parseObject(returnData.data, &returnObject);
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult");
			}
			returnType returnObject{};
			return returnObject;
		}

		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpClient::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpClient::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpClient::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataNew = HttpClient::rateLimitData.at(rateLimitDataNew->bucket).load();
					}
					else {
						HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->bucket, rateLimitDataNew);
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());
					HttpClient::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->tempBucket, rateLimitDataNew);
				}
				HttpClient::executeByRateLimitData(workload, rateLimitDataNew, true);
				return;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult");
			}
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpClient::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpClient::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpClient::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataNew = HttpClient::rateLimitData.at(rateLimitDataNew->bucket).load();
					}
					else {
						HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->bucket, rateLimitDataNew);
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());
					HttpClient::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->tempBucket, rateLimitDataNew);
				}
				HttpData returnData = HttpClient::executeByRateLimitData(workload, rateLimitDataNew, true);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult");
			}
			return HttpData();
		}

		static void initialize(string);

	protected:

		static map<string, atomic<shared_ptr<RateLimitData>>> rateLimitData;
		static map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static atomic<shared_ptr<string>> botToken;
		static atomic<shared_ptr<string>> baseURL;

		static HttpData executeByRateLimitData(HttpWorkloadData workload, shared_ptr<RateLimitData> rateLimitDataNew, bool printResult);

		static HttpData HttpRequest(HttpWorkloadData&, shared_ptr<RateLimitData>);


	};
}