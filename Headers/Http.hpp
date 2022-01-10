// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpHeader {
	public:

		HttpHeader(nullptr_t) {};

		HttpHeader(string key, string value) {
			this->value = value;
			this->key = key;
		}

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, unordered_map<string, shared_ptr<HttpHeader>> headers) {};

		virtual ~HttpHeader() {};

		string value{ "" };
		string key{ "" };
	};

	class DiscordCoreAPI_Dll RateLimitRetryAfter : public HttpHeader {
	public:

		RateLimitRetryAfter(string key, string value) : HttpHeader(key, value) {};

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, unordered_map<string, shared_ptr<HttpHeader>> headers) {
			if (headers.contains("x-ratelimit-retry-after")) {
				pRateLimitData->msRemain = static_cast<int64_t>(static_cast<float>(stoll(headers.at("x-ratelimit-retry-after")->value)) * 1000.0f);
			}
		}
	};

	class DiscordCoreAPI_Dll RateLimitMsRemain : public HttpHeader {
	public:

		RateLimitMsRemain(string key, string value) : HttpHeader(key, value) {};

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, unordered_map<string, shared_ptr<HttpHeader>> headers) {
			if (headers.contains("x-ratelimit-remaining")) {
				pRateLimitData->getsRemaining = stol(headers.at("x-ratelimit-remaining")->value);
			}
		}

	};

	class DiscordCoreAPI_Dll RateLimitLimit : public HttpHeader {
	public:

		RateLimitLimit(string key, string value) : HttpHeader(key, value) {};

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, unordered_map<string, shared_ptr<HttpHeader>> headers) {
			if (headers.contains("x-ratelimit-limit")) {
				pRateLimitData->totalGets = stol(headers.at("x-ratelimit-limit")->value.c_str());
			}
		}

	};

	class DiscordCoreAPI_Dll RateLimitResetAfter : public HttpHeader {
	public:

		RateLimitResetAfter(string key, string value) : HttpHeader(key, value) {};

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, unordered_map<string, shared_ptr<HttpHeader>> headers) {
			if (headers.contains("x-ratelimit-reset-after")) {
				pRateLimitData->msRemain = static_cast<int64_t>(stod(headers.at("x-ratelimit-reset-after")->value) * 1000.0f);
			}
		}

	};

	class DiscordCoreAPI_Dll RateLimitBucket : public HttpHeader {
	public:

		RateLimitBucket(string key, string value) : HttpHeader(key, value) {};

		virtual void constructValue(shared_ptr<RateLimitData> pRateLimitData, unordered_map<string, shared_ptr<HttpHeader>> headers) {
			if (headers.contains("x-ratelimit-bucket")) {
				pRateLimitData->bucket = headers.at("x-ratelimit-bucket")->value;
			}
		}

	};

	class DiscordCoreAPI_Dll HttpHeaderBuilder {
	public:

		static shared_ptr<HttpHeader> getHeader(string key, string value) {
			if (key.find("x-ratelimit-remaining") != string::npos) {
				return make_shared<RateLimitMsRemain>(key, value);
			}
			else if (key.find("x-ratelimit-reset-after") != string::npos) {
				return make_shared<RateLimitResetAfter>(key, value);
			}
			else if (key.find("x-ratelimit-bucket") != string::npos) {
				return make_shared<RateLimitBucket>(key, value);
			}
			else if (key.find("x-ratelimit-limit") != string::npos) {
				return make_shared<RateLimitLimit>(key, value);
			}
			else if (key.find("x-ratelimit-retry-after") != string::npos) {
				return make_shared<RateLimitRetryAfter>(key, value);
			}
			else {
				return make_shared<HttpHeader>(key, value);
			}
		};
	};

	struct DiscordCoreAPI_Dll HttpData {

		unordered_map<string, shared_ptr<HttpHeader>> responseHeaders{};
		string responseMessage{ "" };
		int64_t responseCode{ 0 };
		json responseData{};
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend class  HttpConnection;

		string buildRequest(string& baseUrl, string& relativePath, string& content, unordered_map<string, string>& headers, HttpWorkloadClass workloadClass);

		HttpData handleHeaders(HttpWorkloadData& workloadData, shared_ptr<RateLimitData> pRateLimitData);

		unordered_map<string, shared_ptr<HttpHeader>> getResponseHeaders();

		const int64_t getResponseCode();

		const string getFinalContent();

	protected:

		unordered_map<string, shared_ptr<HttpHeader>> headers{};
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		int64_t responseCode{ -1 };
		string contentFinal{ "" };
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

	class DiscordCoreAPI_Dll HttpConnection {
	public:

		bool sendRequest(string baseUrl, string& relativePath, string& content, unordered_map<string, string>& headers, HttpWorkloadClass workloadClass);

		HttpData getResponse(HttpWorkloadData& workloadData, shared_ptr<RateLimitData> pRateLimitData);

		bool connect(string baseUrl);

	protected:

		string soundcloudCertPath{ "C:/SSL/certs/SoundCloudCert.pem" };
		string defaultCertPath{ "C:/SSL/certs/DiscordCert.pem" };
		string googleCertPath{ "C:/SSL/certs/GoogleCert.pem" };
		BIOWrapper connectionBio{ nullptr };
		int64_t maxBufferSize{ 16 * 1024 };
		SSL_CTXWrapper context{ nullptr };
		HttpRnRBuilder httpBuilder{};
		SSLWrapper ssl{ nullptr };
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:

		friend class  HttpRnRBuilder;

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpClient::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpClient::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpClient::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataNew = HttpClient::rateLimitData.at(rateLimitDataNew->bucket);
					}
					else {
						HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->bucket, rateLimitDataNew);
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					HttpClient::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->tempBucket, rateLimitDataNew);
				}
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpData returnData = HttpClient::httpRequest(workload, rateLimitDataNew, true);
				returnType returnObject{};
				DataParser::parseObject(returnData.responseData, &returnObject);
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			returnType returnObject{};
			return returnObject;
		}

		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData& workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpClient::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpClient::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpClient::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataNew = HttpClient::rateLimitData.at(rateLimitDataNew->bucket);
					}
					else {
						HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->bucket, rateLimitDataNew);
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					HttpClient::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->tempBucket, rateLimitDataNew);
				}
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpClient::httpRequest(workload, rateLimitDataNew, true);
				return;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData& workload) {
			try {
				shared_ptr<RateLimitData> rateLimitDataNew = make_shared<RateLimitData>();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpClient::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpClient::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpClient::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataNew = HttpClient::rateLimitData.at(rateLimitDataNew->bucket);
					}
					else {
						HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->bucket, rateLimitDataNew);
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					HttpClient::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpClient::rateLimitData.insert_or_assign(rateLimitDataNew->tempBucket, rateLimitDataNew);
				}
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpData returnData = HttpClient::httpRequest(workload, rateLimitDataNew, false);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return HttpData();
		}

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(vector<HttpWorkloadData>& workload) {
			try {
				auto returnData = HttpClient::httpRequest(workload);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload[0].callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return vector<HttpData>();
		}

		template<>
		static vector<HttpData> submitWorkloadAndGetResult<vector<HttpData>>(vector<HttpWorkloadData>& workload) {
			try {
				auto returnData = HttpClient::httpRequest(workload);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload[0].callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return vector<HttpData>();
		}

		static void initialize(string);

	protected:

		static map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static map<string, shared_ptr<RateLimitData>> rateLimitData;
		static atomic<shared_ptr<string>> botToken;
		static atomic<shared_ptr<string>> baseUrl;

		static HttpData executeByRateLimitData(HttpWorkloadData& workload, shared_ptr<RateLimitData> rateLimitDataNew, bool printResult);

		static HttpData executehttpRequest(HttpWorkloadData& workloadData, shared_ptr<RateLimitData> pRateLimitData);

		static HttpData httpRequest(HttpWorkloadData&, shared_ptr<RateLimitData>, bool = false);
		
		static vector<HttpData> executehttpRequest(vector<HttpWorkloadData>& workloadData);

		static vector<HttpData> httpRequest(vector<HttpWorkloadData>& workloadData);
	};
}