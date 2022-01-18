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

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:

		friend HttpClient;

		HttpData handleHeaders(HttpWorkloadData& workload, shared_ptr<HttpConnection> theConnection);

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

	class DiscordCoreAPI_Dll RateLimitData {
	public:

		friend class HttpConnectionManager;
		friend class HttpClient;
		friend class HttpHeader;

		RateLimitData() = default;

		virtual ~RateLimitData() = default;

	protected:

		recursive_mutex accessMutex{};
		int64_t sampledTimeInMs{ 0 };
		int32_t getsRemaining{ 0 };
		int64_t msRemain{ 0 };
	};

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

	class DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public RateLimitData, public HttpRnRBuilder {
	public:

		bool doWeConnect{ true };
		string tempBucket{ "" };
		string bucket{ "" };

		HttpConnection() : HttpSSLClient(&this->rawInput) {};

	};

	class DiscordCoreAPI_Dll HttpConnectionManager {
	public:

		map<string, AtomicWrapper<shared_ptr<HttpConnection>>> httpConnections{};
		map<HttpWorkloadType, string> httpConnectionBucketValues{};
		mutex theMutex{};

		shared_ptr<HttpConnection> getConnection(HttpWorkloadType type);

		void storeConnection(HttpWorkloadType type);

		void initialize();

	};
	
	class DiscordCoreAPI_Dll HttpHeader {
	public:
		
		friend class DiscordCoreAPI::SoundCloudRequestBuilder;
		friend class HttpRnRBuilder;

		HttpHeader(nullptr_t);

		HttpHeader(string key, string value);

		static void constructValues(unordered_map<string, HttpHeader>& headers, shared_ptr<HttpConnection> theConnection);

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
				while (!this->theStopWatch.load().hasTimePassed()) {};
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
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
				while (!this->theStopWatch.load().hasTimePassed()) {};
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
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
				while (!this->theStopWatch.load().hasTimePassed()) {};
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
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

		atomic<DiscordCoreAPI::StopWatch<milliseconds>> theStopWatch{ milliseconds{10} };
		atomic<shared_ptr<string>> botToken{};
		HttpConnectionManager connectionManager{};

		HttpData executeByRateLimitData(HttpWorkloadData&, bool, shared_ptr<HttpConnection>);

		HttpData executeHttpRequest(HttpWorkloadData&, shared_ptr<HttpConnection>);

		HttpData getResponse(HttpWorkloadData&, shared_ptr<HttpConnection>);

		vector<HttpData> executeHttpRequest(vector<HttpWorkloadData>&);

		vector<HttpData> httpRequest(vector<HttpWorkloadData>&);

		HttpData httpRequest(HttpWorkloadData&, bool);
		
	};

}