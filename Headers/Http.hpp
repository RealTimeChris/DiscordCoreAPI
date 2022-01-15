// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	class HttpConnection;
	class HttpHeader;
	struct HttpData;

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend class HttpConnection;
		friend class HttpClient;

		HttpData handleHeaders(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);

		string buildRequest(HttpWorkloadData& workload);

		void resetValues();

	protected:

		unordered_map<string, HttpHeader> headers{};
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		int64_t responseCode{ -1 };
		string contentFinal{ "" };
		int64_t contentSize{ -1 };
		bool isItChunked{ false };
		string rawInput{};

		bool checkForHeadersToParse();

		void parseHeaders();

		bool parseChunk();

		void parseSize();

		void clearCRLF();

		void parseCode();
	};

	class DiscordCoreAPI_Dll RateLimitData {
	public:

		friend class HttpRnRBuilder;
		friend class HttpHeader;
		friend class HttpClient;

		RateLimitData() = default;

	protected:

		HttpWorkloadType workloadType{ HttpWorkloadType::Unset };
		bool doWeHaveTotalTimePerTick{ false };
		bool isTheBucketActive{ false };
		int64_t totalTimePerTick{ 0 };
		recursive_mutex accessMutex{};
		int32_t getsRemaining{ 0 };
		string tempBucket{ "" };
		int32_t totalGets{ 0 };
		int64_t msRemain{ 0 };
		string bucket{ "" };
	};

	class DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public RateLimitData, public HttpRnRBuilder {
	public:

		HttpConnection() : HttpSSLClient(&this->rawInput) {};

		bool doWeConnect{ true };
	};

	class DiscordCoreAPI_Dll HttpHeader {
	public:

		HttpHeader(nullptr_t) {};

		HttpHeader(string key, string value) {
			this->value = value;
			this->key = key;
		}

		static void constructValues(shared_ptr<HttpConnection> httpConnection, unordered_map<string, HttpHeader> headers) {
			if (headers.contains("x-ratelimit-remaining")) {
				httpConnection->getsRemaining = stol(headers.at("x-ratelimit-remaining").value);
			}
			if (headers.contains("x-ratelimit-limit")) {
				httpConnection->totalGets = stol(headers.at("x-ratelimit-limit").value.c_str());
			}
			if (headers.contains("x-ratelimit-reset-after")) {
				if (!httpConnection->doWeHaveTotalTimePerTick) {
					httpConnection->doWeHaveTotalTimePerTick = true;
				}
				httpConnection->msRemain = static_cast<int64_t>(stod(headers.at("x-ratelimit-reset-after").value) * 1000.0f);
			}
			else {
				httpConnection->msRemain = 0;
			}
			if (headers.contains("x-ratelimit-bucket")) {
				httpConnection->bucket = headers.at("x-ratelimit-bucket").value;
			}
			else {
				httpConnection->bucket = string{};
			}
		};

		static HttpHeader getHeader(string key, string value) {
			return HttpHeader(key, value);
		};

		virtual ~HttpHeader() {};

		string value{ "" };
		string key{ "" };
	};

	struct DiscordCoreAPI_Dll HttpData {

		unordered_map<string, HttpHeader> responseHeaders{};
		string responseMessage{ "" };
		int64_t responseCode{ 0 };
		json responseData{};
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:

		friend class HttpRnRBuilder;

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			try {
				HttpClient::waitForMutex(1);
				shared_ptr<HttpConnection> httpConnection = make_shared<HttpConnection>();
				httpConnection->workloadType = workload.workloadType;
				if (HttpClient::httpConnectionBucketValues.contains(httpConnection->workloadType)) {
					httpConnection->bucket = HttpClient::httpConnectionBucketValues.at(httpConnection->workloadType);
					if (HttpClient::httpConnections.contains(httpConnection->bucket)) {
						httpConnection = HttpClient::httpConnections.at(httpConnection->bucket);
						httpConnection->resetValues();
					}
					else {
						HttpClient::httpConnections.insert_or_assign(httpConnection->bucket, httpConnection);
					}
				}
				else {
					httpConnection->tempBucket = to_string(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
					HttpClient::httpConnectionBucketValues.insert_or_assign(httpConnection->workloadType, httpConnection->tempBucket);
					HttpClient::httpConnections.insert_or_assign(httpConnection->tempBucket, httpConnection);
				}
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpData returnData = HttpClient::httpRequest(workload, httpConnection, true);
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
				HttpClient::waitForMutex(2);
				shared_ptr<HttpConnection> httpConnection = make_shared<HttpConnection>();
				httpConnection->workloadType = workload.workloadType;
				if (HttpClient::httpConnectionBucketValues.contains(httpConnection->workloadType)) {
					httpConnection->bucket = HttpClient::httpConnectionBucketValues.at(httpConnection->workloadType);
					if (HttpClient::httpConnections.contains(httpConnection->bucket)) {
						httpConnection = HttpClient::httpConnections.at(httpConnection->bucket);
						httpConnection->resetValues();
					}
					else {
						HttpClient::httpConnections.insert_or_assign(httpConnection->bucket, httpConnection);
					}
				}
				else {
					httpConnection->tempBucket = to_string(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
					HttpClient::httpConnectionBucketValues.insert_or_assign(httpConnection->workloadType, httpConnection->tempBucket);
					HttpClient::httpConnections.insert_or_assign(httpConnection->tempBucket, httpConnection);
				}
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpClient::httpRequest(workload, httpConnection, true);
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
				HttpClient::waitForMutex(3);
				shared_ptr<HttpConnection> httpConnection = make_shared<HttpConnection>();
				httpConnection->workloadType = workload.workloadType;
				if (HttpClient::httpConnectionBucketValues.contains(httpConnection->workloadType)) {
					httpConnection->bucket = HttpClient::httpConnectionBucketValues.at(httpConnection->workloadType);
					if (HttpClient::httpConnections.contains(httpConnection->bucket)) {
						httpConnection = HttpClient::httpConnections.at(httpConnection->bucket);
						httpConnection->resetValues();
					}
					else {
						HttpClient::httpConnections.insert_or_assign(httpConnection->bucket, httpConnection);
					}
				}
				else {
					httpConnection->tempBucket = to_string(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
					HttpClient::httpConnectionBucketValues.insert_or_assign(httpConnection->workloadType, httpConnection->tempBucket);
					HttpClient::httpConnections.insert_or_assign(httpConnection->tempBucket, httpConnection);
				}
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpData returnData = HttpClient::httpRequest(workload, httpConnection, false);
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

		static atomic<shared_ptr<DiscordCoreAPI::StopWatch<microseconds>>> stopWatch;
		static map<HttpWorkloadType, string> httpConnectionBucketValues;
		static map<string, shared_ptr<HttpConnection>> httpConnections;
		static atomic<shared_ptr<string>> botToken;
		static mutex theMutex01;
		static mutex theMutex02;
		static mutex theMutex03;

		static HttpData executeByRateLimitData(HttpWorkloadData& workload, shared_ptr<HttpConnection> httpConnection, bool printResult);

		static HttpData executeHttpRequest(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);
		
		static HttpData getResponse(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);

		static HttpData httpRequest(HttpWorkloadData&, shared_ptr<HttpConnection>, bool = false);
		
		static vector<HttpData> executeHttpRequest(vector<HttpWorkloadData>& workloadData);

		static vector<HttpData> httpRequest(vector<HttpWorkloadData>& workloadData);

		static void waitForMutex(int8_t);
	};
}