// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "SSLClients.hpp"

namespace DiscordCoreInternal {

	class HttpConnectionManager;
	class HttpConnection;
	class HttpHeader;
	struct HttpData;

	template<typename ObjectType>
	class AtomicWrapper {
	public:

		AtomicWrapper& operator=(ObjectType& other) {
			this->theValue.store(other);
			return *this;
		}

		AtomicWrapper(ObjectType other) {
			*this = other;
		}

		AtomicWrapper& operator=(const AtomicWrapper& other) {
			this->theValue.store(other.theValue.load());
			return *this;
		}

		AtomicWrapper(const AtomicWrapper& other) {
			*this = other;
		}

		AtomicWrapper& operator=(AtomicWrapper& other) {
			this->theValue.store(other.theValue.load());
			return *this;
		}

		AtomicWrapper(AtomicWrapper& other) {
			*this = other;
		}

		ObjectType getValue() {
			return this->theValue.load();
		}

	protected:
		atomic<ObjectType> theValue{};
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend HttpClient;

		HttpData handleHeaders(HttpWorkloadData& workload, HttpConnection* theConnection);

		string buildRequest(HttpWorkloadData& workload);

		void resetValues();

		virtual ~HttpRnRBuilder() = default;

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

	struct DiscordCoreAPI_Dll RateLimitData {
	public:

		friend HttpConnectionManager;
		friend HttpConnection;
		friend HttpRnRBuilder;
		friend HttpClient;
		friend HttpHeader;

		RateLimitData& operator=(RateLimitData& other) {
			this->sampledTimeInMs = other.sampledTimeInMs;
			this->getsRemaining = other.getsRemaining;
			this->tempBucket = other.tempBucket;
			this->msRemain = other.msRemain;
			this->bucket = other.bucket;
			return *this;
		}

		RateLimitData(RateLimitData& other) {
			*this = other;
		}

		RateLimitData() = default;

		virtual ~RateLimitData() = default;

	protected:

		binary_semaphore semaphore{ 1 };
		int64_t sampledTimeInMs{ 0 };
		int32_t getsRemaining{ 0 };
		string tempBucket{ "" };
		int64_t msRemain{ 0 };
		string bucket{ "" };
	};

	class DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public HttpRnRBuilder {
	public:

		RateLimitData* rateLimitData{ nullptr };
		recursive_mutex accessMutex{};
		bool doWeConnect{ true };
		string bucket{ "" };

		HttpConnection& operator=(HttpConnection& other) {
			this->bucket = other.bucket;
			this->connectionBio = move(other.connectionBio);
			this->contentFinal = other.contentFinal;
			this->contentSize = other.contentSize;
			this->context = move(other.context);
			this->currentRecursionDepth = other.currentRecursionDepth;
			this->defaultCertPath = other.defaultCertPath;
			this->doWeConnect = other.doWeConnect;
			this->doWeHaveContentSize = other.doWeHaveContentSize;
			this->doWeHaveHeaders = other.doWeHaveHeaders;
			this->googleCertPath = other.googleCertPath;
			this->headers = other.headers;
			this->isItChunked = other.isItChunked;
			this->maxBufferSize = other.maxBufferSize;
			this->rateLimitData = other.rateLimitData;
			this->rawInput = other.rawInput;
			this->responseCode = other.responseCode;
			this->soundcloudCertPath = other.soundcloudCertPath;
			this->ssl = move(other.ssl);
			this->theInputVector = other.theInputVector;
			return *this;
		}

		HttpConnection(HttpConnection& other) {
			*this = other;
		}

		HttpConnection() : HttpSSLClient(&this->rawInput) {};

	};

	class DiscordCoreAPI_Dll HttpConnectionManager {
	public:

		unique_ptr<unordered_map<HttpWorkloadType, unique_ptr<HttpConnection>>> httpConnections{ make_unique<unordered_map<HttpWorkloadType, unique_ptr<HttpConnection>>>() };
		unique_ptr<unordered_map<string, unique_ptr<RateLimitData>>> rateLimitValues{ make_unique<unordered_map<string, unique_ptr<RateLimitData>>>() };

		HttpConnection* getConnection(HttpWorkloadType type);

		void storeConnection(HttpWorkloadType type);

		void initialize();
	};

	class DiscordCoreAPI_Dll HttpHeader {
	public:

		friend class DiscordCoreAPI::SoundCloudRequestBuilder;
		friend HttpRnRBuilder;
		friend HttpClient;

		HttpHeader(string key, string value);

		HttpHeader(nullptr_t);

		static void constructValues(unordered_map<string, HttpHeader>& headers, RateLimitData* theConnection);

	protected:

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

		HttpClient(string);

		template<typename returnType>
		returnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			try {
				while (!this->theStopWatch01.load().hasTimePassed()) {};
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpData returnData = this->httpRequest(workload, true);
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
		void submitWorkloadAndGetResult<void>(HttpWorkloadData& workload) {
			try {
				while (!this->theStopWatch02.load().hasTimePassed()) {};
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				this->httpRequest(workload, true);
				return;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return;
		}

		template<>
		HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData& workload) {
			try {
				while (!this->theStopWatch03.load().hasTimePassed()) {};
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				return this->httpRequest(workload, false);
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return HttpData();
		}

		vector<HttpData> submitWorkloadAndGetResult(vector<HttpWorkloadData>& workload) {
			try {
				auto returnData = this->httpRequest(workload);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload[0].callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return vector<HttpData>();
		}

	protected:

		atomic<DiscordCoreAPI::StopWatch<milliseconds>> theStopWatch01{ milliseconds{10} };
		atomic<DiscordCoreAPI::StopWatch<milliseconds>> theStopWatch02{ milliseconds{10} };
		atomic<DiscordCoreAPI::StopWatch<milliseconds>> theStopWatch03{ milliseconds{10} };
		HttpConnectionManager connectionManager{};
		const string botToken{};

		HttpData executeByRateLimitData(HttpWorkloadData&, bool, HttpConnection* theConnection);

		HttpData executeHttpRequest(HttpWorkloadData&, HttpConnection* theConnection);

		HttpData getResponse(HttpWorkloadData&, HttpConnection* theConnection);

		vector<HttpData> executeHttpRequest(vector<HttpWorkloadData>&);

		vector<HttpData> httpRequest(vector<HttpWorkloadData>&);

		HttpData httpRequest(HttpWorkloadData&, bool);

	};

}