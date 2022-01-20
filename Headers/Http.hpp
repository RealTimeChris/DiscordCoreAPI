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
		std::atomic<ObjectType> theValue{};
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend HttpClient;

		HttpData handleHeaders(HttpWorkloadData& workload, HttpConnection& theConnection);

		std::string buildRequest(HttpWorkloadData& workload);

		void resetValues();

		virtual ~HttpRnRBuilder() = default;

	protected:

		std::unordered_map<std::string, HttpHeader> headers{};
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		int64_t responseCode{ -1 };
		std::string contentFinal{ "" };
		int64_t contentSize{ -1 };
		bool isItChunked{ false };
		std::string rawInput{};

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

		std::binary_semaphore semaphore{ 1 };
		int64_t sampledTimeInMs{ 0 };
		int32_t getsRemaining{ 0 };
		std::string tempBucket{ "" };
		int64_t msRemain{ 0 };
		std::string bucket{ "" };
	};

	class DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public HttpRnRBuilder {
	public:

		RateLimitData* rateLimitData{ nullptr };
		std::recursive_mutex accessMutex{};
		bool doWeConnect{ true };
		std::string bucket{ "" };

		HttpConnection() : HttpSSLClient(&this->rawInput) {};

	};

	class DiscordCoreAPI_Dll HttpConnectionManager {
	public:

		std::unique_ptr<std::unordered_map<HttpWorkloadType, std::unique_ptr<HttpConnection>>> httpConnections{ std::make_unique<std::unordered_map<HttpWorkloadType, std::unique_ptr<HttpConnection>>>() };
		std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<RateLimitData>>> rateLimitValues{ std::make_unique<std::unordered_map<std::string, std::unique_ptr<RateLimitData>>>() };

		HttpConnection& getConnection(HttpWorkloadType type);

		void storeConnection(HttpWorkloadType type);

		void initialize();
	};

	class DiscordCoreAPI_Dll HttpHeader {
	public:

		friend class DiscordCoreAPI::SoundCloudRequestBuilder;
		friend HttpRnRBuilder;
		friend HttpClient;

		HttpHeader(std::string key, std::string value);

		HttpHeader(nullptr_t);

		static void constructValues(std::unordered_map<std::string, HttpHeader>& headers, RateLimitData* theConnection);

	protected:

		std::string value{ "" };
		std::string key{ "" };
	};

	struct DiscordCoreAPI_Dll HttpData {

		std::unordered_map<std::string, HttpHeader> responseHeaders{};
		std::string responseMessage{ "" };
		int64_t responseCode{ 0 };
		nlohmann::json responseData{};
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:

		HttpClient(std::string);

		template<typename returnType>
		returnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			try {
				while (!this->theStopWatch01.load().hasTimePassed()) {};
				workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
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
				workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
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
				workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
				return this->httpRequest(workload, false);
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return HttpData();
		}

		std::vector<HttpData> submitWorkloadAndGetResult(std::vector<HttpWorkloadData>& workload) {
			try {
				auto returnData = this->httpRequest(workload);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload[0].callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			return std::vector<HttpData>();
		}

	protected:

		std::atomic<DiscordCoreAPI::StopWatch<std::chrono::milliseconds>> theStopWatch01{ std::chrono::milliseconds{10} };
		std::atomic<DiscordCoreAPI::StopWatch<std::chrono::milliseconds>> theStopWatch02{ std::chrono::milliseconds{10} };
		std::atomic<DiscordCoreAPI::StopWatch<std::chrono::milliseconds>> theStopWatch03{ std::chrono::milliseconds{10} };
		HttpConnectionManager connectionManager{};
		const std::string botToken{};

		HttpData executeByRateLimitData(HttpWorkloadData&, bool, HttpConnection& theConnection);

		HttpData executeHttpRequest(HttpWorkloadData&, HttpConnection& theConnection);

		HttpData getResponse(HttpWorkloadData&, HttpConnection& theConnection);

		std::vector<HttpData> executeHttpRequest(std::vector<HttpWorkloadData>&);

		std::vector<HttpData> httpRequest(std::vector<HttpWorkloadData>&);

		HttpData httpRequest(HttpWorkloadData&, bool);

	};

}