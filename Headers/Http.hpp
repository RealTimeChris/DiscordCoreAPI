// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "SSLClients.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll HttpConnection;
	struct DiscordCoreAPI_Dll RateLimitData;
	struct DiscordCoreAPI_Dll HttpData;

	class SemaphoreWrapper :public DiscordCoreAPI::ReferenceCountingBase {
	public:

		SemaphoreWrapper() = default;

		void acquire() {
			this->semaphore.acquire();
		}

		void release() {
			this->semaphore.release();
		}

		~SemaphoreWrapper() {
			this->semaphore.release();
		}

	protected:
		std::binary_semaphore semaphore{ 1 };
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend HttpClient;

		void constructHeaderValues(std::unordered_map<std::string, std::string>& headers, RateLimitData* theConnection);

		HttpData handleHeaders(HttpWorkloadData& workload, HttpConnection& theConnection);

		std::string buildRequest(HttpWorkloadData& workload);

		bool checkForHeadersToParse();

		void parseHeaders();

		void resetValues();

		bool parseChunk();

		virtual ~HttpRnRBuilder() = default;

	protected:

		std::unordered_map<std::string, std::string> headers{};
		bool doWeHaveContentSize{ false };
		std::string contentFinal{ "" };
		bool doWeHaveHeaders{ false };
		int64_t responseCode{ -1 };
		int64_t contentSize{ -1 };
		std::string inputBuffer{};
		bool isItChunked{ false };

		void parseSize();

		void clearCRLF();

		void parseCode();
	};

	struct DiscordCoreAPI_Dll RateLimitData {
	public:

		friend HttpRnRBuilder;
		friend HttpClient;

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

	protected:

		std::unique_ptr<SemaphoreWrapper> semaphore{ std::make_unique<SemaphoreWrapper>() };
		int64_t sampledTimeInMs{ 0 };
		std::string tempBucket{ "" };
		int32_t getsRemaining{ 0 };
		std::string bucket{ "" };
		int64_t msRemain{ 0 };
	};

	struct DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public HttpRnRBuilder {
	public:

		RateLimitData* rateLimitDataPtr{ nullptr };
		std::string bucket{ "" };
		bool doWeConnect{ true };
		std::mutex accessMutex{};

		HttpConnection() : HttpSSLClient(&this->inputBuffer) {};

	};

	class DiscordCoreAPI_Dll HttpConnectionManager {
	public:

		std::unique_ptr<std::unordered_map<HttpWorkloadType, std::unique_ptr<HttpConnection>>> httpConnections{ std::make_unique<std::unordered_map<HttpWorkloadType, std::unique_ptr<HttpConnection>>>() };
		std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<RateLimitData>>> rateLimitValues{ std::make_unique<std::unordered_map<std::string, std::unique_ptr<RateLimitData>>>() };

		HttpConnection& getConnection(HttpWorkloadType type);

		void storeConnection(HttpWorkloadType type);

		void initialize();
	};

	struct DiscordCoreAPI_Dll HttpData {

		std::unordered_map<std::string, std::string> responseHeaders{};
		std::string responseMessage{ "" };
		nlohmann::json responseData{};
		int64_t responseCode{ 0 };
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:

		HttpClient(std::string);

		template<typename ReturnType>
		ReturnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			try {
				while (!this->theStopWatch01.load().hasTimePassed()) {};
				workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + HttpClient::botToken));
				workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
				HttpData returnData = this->httpRequest(workload, true);
				ReturnType returnObject{};
				DataParser::parseObject(returnData.responseData, &returnObject);
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
			}
			ReturnType returnObject{};
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

		std::vector<HttpData> executeHttpRequest(std::vector<HttpWorkloadData>&);

		HttpData getResponse(HttpWorkloadData&, HttpConnection& theConnection);

		std::vector<HttpData> httpRequest(std::vector<HttpWorkloadData>&);

		HttpData httpRequest(HttpWorkloadData&, bool);

	};

}