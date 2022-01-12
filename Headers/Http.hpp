// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	class HttpHeader;
	struct HttpData;

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend class  HttpConnection;

		string buildRequest(string& baseUrl, string& relativePath, string& content, unordered_map<string, string>& headers, HttpWorkloadClass workloadClass);

		HttpData handleHeaders(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);

	protected:

		unordered_map<string, HttpHeader> headers{};
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

		friend class HttpRnRBuilder;
		friend class HttpHeader;
		friend class HttpClient;		

		HttpConnection& operator=(HttpConnection&& other) noexcept {
			this->doWeHaveTotalTimePerTick = other.doWeHaveTotalTimePerTick;
			this->bucketStartTimeInsMs = other.bucketStartTimeInsMs;
			this->soundcloudCertPath = move(other.soundcloudCertPath);
			this->isTheBucketActive = move(other.isTheBucketActive);
			this->connectionBio = move(other.connectionBio);
			this->totalTimePerTick = other.totalTimePerTick;
			this->defaultCertPath = move(other.defaultCertPath);
			this->bucketResetInMs = other.bucketResetInMs;
			this->msRemainTotal = other.msRemainTotal;
			this->maxBufferSize = other.maxBufferSize;
			this->getsRemaining = other.getsRemaining;
			this->workloadType = other.workloadType;
			this->accessMutex = move(other.accessMutex);
			this->httpBuilder = HttpRnRBuilder{};
			this->context = move(other.context);
			this->tempBucket = move(other.tempBucket);
			this->totalGets = other.totalGets;
			this->msRemain = other.msRemain;
			this->bucket = other.bucket;
			this->ssl = move(other.ssl);
			return *this;
		}

		HttpConnection(HttpConnection&& other) noexcept {
			*this = move(other);
		}

		HttpConnection() = default;

		void sendRequest(string baseUrl, string& relativePath, string& content, unordered_map<string, string>& headers, HttpWorkloadClass workloadClass);

		HttpData getResponse(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);

		bool connect(string baseUrl);

	protected:

		string soundcloudCertPath{ "C:/SSL/certs/SoundCloudCert.pem" };
		string defaultCertPath{ "C:/SSL/certs/DiscordCert.pem" };
		HttpWorkloadType workloadType{ HttpWorkloadType::UNSET };
		string googleCertPath{ "C:/SSL/certs/GoogleCert.pem" };
		shared_ptr<mutex> accessMutex{ make_shared<mutex>() };
		bool doWeHaveTotalTimePerTick{ false };
		BIOWrapper connectionBio{ nullptr };
		int64_t maxBufferSize{ 16 * 1024 };
		int64_t bucketStartTimeInsMs{ 0 };
		SSL_CTXWrapper context{ nullptr };
		bool isTheBucketActive{ false };
		bool doWeReconnectBool{ true };
		int64_t totalTimePerTick{ 0 };
		int64_t bucketResetInMs{ 0 };
		HttpRnRBuilder httpBuilder{};
		int32_t getsRemaining{ 0 };
		int64_t msRemainTotal{ 0 };
		SSLWrapper ssl{ nullptr };
		string tempBucket{ "" };
		int32_t totalGets{ 0 };
		int64_t msRemain{ 0 };
		string bucket{ "" };
	};

	class DiscordCoreAPI_Dll HttpHeader {
	public:

		HttpHeader(nullptr_t) {};

		HttpHeader(string key, string value) {
			this->value = value;
			this->key = key;
		}

		static void constructValues(shared_ptr<HttpConnection>& httpConnection, unordered_map<string, HttpHeader> headers) {
			if (headers.contains("x-ratelimit-reset")) {
				httpConnection->bucketResetInMs = static_cast<int64_t>(stoll(headers.at("x-ratelimit-reset").value) * 1000);
			}
			else {
				httpConnection->bucketResetInMs = 0;
			}
			if (headers.contains("x-ratelimit-remaining")) {
				httpConnection->getsRemaining = stol(headers.at("x-ratelimit-remaining").value);
			}
			else {
				httpConnection->getsRemaining = 0;
			}
			if (headers.contains("x-ratelimit-limit")) {
				httpConnection->totalGets = stol(headers.at("x-ratelimit-limit").value.c_str());
			}
			if (headers.contains("x-ratelimit-reset-after")) {
				if (!httpConnection->doWeHaveTotalTimePerTick) {
					httpConnection->msRemainTotal = static_cast<int64_t>(stod(headers.at("x-ratelimit-reset-after").value) * 1000.0f);
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

		friend class  HttpRnRBuilder;

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			try {
				shared_ptr<HttpConnection> httpConnection = make_shared<HttpConnection>();
				httpConnection->workloadType = workload.workloadType;
				if (HttpClient::httpConnectionBucketValues.contains(workload.workloadType)) {
					httpConnection->bucket = HttpClient::httpConnectionBucketValues.at(workload.workloadType);
					if (HttpClient::httpConnections.contains(httpConnection->bucket)) {
						httpConnection = HttpClient::httpConnections.at(httpConnection->bucket);
					}
					else {
						HttpClient::httpConnections.insert_or_assign(httpConnection->bucket, httpConnection);
					}
				}
				else {
					httpConnection->tempBucket = to_string(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					HttpClient::httpConnectionBucketValues.insert_or_assign(workload.workloadType, httpConnection->tempBucket);
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
				shared_ptr<HttpConnection> httpConnection = make_shared<HttpConnection>();
				httpConnection->workloadType = workload.workloadType;
				if (HttpClient::httpConnectionBucketValues.contains(workload.workloadType)) {
					httpConnection->bucket = HttpClient::httpConnectionBucketValues.at(workload.workloadType);
					if (HttpClient::httpConnections.contains(httpConnection->bucket)) {
						httpConnection = HttpClient::httpConnections.at(httpConnection->bucket);
					}
					else {
						HttpClient::httpConnections.insert_or_assign(httpConnection->bucket, httpConnection);
					}
				}
				else {
					httpConnection->tempBucket = to_string(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					HttpClient::httpConnectionBucketValues.insert_or_assign(workload.workloadType, httpConnection->tempBucket);
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
				shared_ptr<HttpConnection> httpConnection = make_shared<HttpConnection>();
				httpConnection->workloadType = workload.workloadType;
				if (HttpClient::httpConnectionBucketValues.contains(workload.workloadType)) {
					httpConnection->bucket = HttpClient::httpConnectionBucketValues.at(workload.workloadType);
					if (HttpClient::httpConnections.contains(httpConnection->bucket)) {
						httpConnection = HttpClient::httpConnections.at(httpConnection->bucket);
					}
					else {
						HttpClient::httpConnections.insert_or_assign(httpConnection->bucket, httpConnection);
					}
				}
				else {
					httpConnection->tempBucket = to_string(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					HttpClient::httpConnectionBucketValues.insert_or_assign(workload.workloadType, httpConnection->tempBucket);
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

		static map<HttpWorkloadType, string> httpConnectionBucketValues;
		static map<string, shared_ptr<HttpConnection>> httpConnections;
		static atomic<shared_ptr<string>> botToken;

		static HttpData executeByRateLimitData(HttpWorkloadData& workload, shared_ptr<HttpConnection> httpConnection, bool printResult);

		static HttpData executehttpRequest(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);

		static HttpData httpRequest(HttpWorkloadData&, shared_ptr<HttpConnection>, bool = false);
		
		static vector<HttpData> executehttpRequest(vector<HttpWorkloadData>& workloadData);

		static vector<HttpData> httpRequest(vector<HttpWorkloadData>& workloadData);
	};
}