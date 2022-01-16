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

		HttpData handleHeaders(HttpWorkloadData& workloadData);

		string buildRequest(HttpWorkloadData& workload);

		void resetValues();

	protected:
		unordered_map<string, HttpHeader> prevHeaders{};
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
		recursive_mutex accessMutex{};
		int32_t getsRemaining{ 0 };
		int64_t sampledTime{ 0 };
		string tempBucket{ "" };
		int64_t msRemain{ 0 };
		string bucket{ "" };
	};

	template<typename ObjectType>
	class AtomicWrapper {
	public:

		AtomicWrapper& operator=(ObjectType other) {
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

		static map<string, AtomicWrapper<shared_ptr<HttpConnection>>> httpConnections;
		static map<HttpWorkloadType, string> httpConnectionBucketValues;
		static mutex theMutex;

		bool doWeConnect{ true };

		HttpConnection() : HttpSSLClient(&this->rawInput) {};

		static shared_ptr<HttpConnection> getConnection(HttpWorkloadType type);

		static void storeConnection(HttpWorkloadType data);

		static void initialize();

	};

	class DiscordCoreAPI_Dll HttpHeader {
	public:

		HttpHeader(nullptr_t) {};

		HttpHeader(string key, string value) {
			this->value = value;
			this->key = key;
		}

		static void constructValues(HttpWorkloadType type, unordered_map<string, HttpHeader> headers) {
			if (headers.contains("x-ratelimit-remaining")) {
				HttpConnection::getConnection(type)->getsRemaining = stol(headers.at("x-ratelimit-remaining").value);
			}
			if (headers.contains("x-ratelimit-reset-after")) {
				HttpConnection::getConnection(type)->sampledTime = duration_cast<milliseconds, int64_t>(system_clock::now().time_since_epoch()).count();
				HttpConnection::getConnection(type)->msRemain = static_cast<int64_t>(stod(headers.at("x-ratelimit-reset-after").value) * 1000.0f);
			}
			if (headers.contains("x-ratelimit-bucket")) {
				HttpConnection::getConnection(type)->bucket = headers.at("x-ratelimit-bucket").value;
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

		unordered_map<string, HttpHeader> prevResponseHeaders{};
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
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpData returnData = HttpClient::httpRequest(workload, true);
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
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				HttpClient::httpRequest(workload);
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
				workload.headersToInsert.insert(make_pair("Authorization", "Bot " + *HttpClient::botToken.load()));
				workload.headersToInsert.insert(make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
				workload.headersToInsert.insert(make_pair("Content-Type", "application/json"));
				return HttpClient::httpRequest(workload);
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

		static atomic<shared_ptr<string>> botToken;

		static HttpData getResponse(HttpWorkloadData& workloadData, shared_ptr<HttpConnection> httpConnection);

		static HttpData executeByRateLimitData(HttpWorkloadData& workload, bool printResult);

		static vector<HttpData> executeHttpRequest(vector<HttpWorkloadData>& workloadData);

		static vector<HttpData> httpRequest(vector<HttpWorkloadData>& workloadData);

		static HttpData executeHttpRequest(HttpWorkloadData& workloadData);
		
		static HttpData httpRequest(HttpWorkloadData&, bool = false);

		static HttpData getResponse(HttpWorkloadData& workloadData);

	};
}