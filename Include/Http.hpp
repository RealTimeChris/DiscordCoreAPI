/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// Http.hpp - Header file for the "Http Stuff".
/// May 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities.hpp>
#include <DataParsingFunctions.hpp>
#include <SSLClients.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpConnectionManager;
	struct DiscordCoreAPI_Dll HttpConnection;
	struct DiscordCoreAPI_Dll RateLimitData;
	struct DiscordCoreAPI_Dll HttpData;

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	public:
		friend HttpClient;

		void constructHeaderValues(std::unordered_map<std::string, std::string>& headers, RateLimitData* theConnection);

		HttpData handleHeaders(HttpConnection& theConnection);

		std::string buildRequest(HttpWorkloadData& workload);

		bool checkForHeadersToParse(std::string&);

		void parseHeaders(std::string&);

		void resetValues(std::string&);

		bool parseChunk(std::string&);

		virtual ~HttpRnRBuilder() = default;

	protected:

		std::unordered_map<std::string, std::string> headers{};
		bool doWeHaveContentSize{ false };
		std::string contentFinal{ "" };
		bool doWeHaveHeaders{ false };
		int64_t responseCode{ -1 };
		int64_t contentSize{ -1 };
		bool isItChunked{ false };

		void parseSize(std::string&);

		void clearCRLF(std::string&);

		void parseCode(std::string&);
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

		std::binary_semaphore semaphore{ 1 };
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

	};

	class DiscordCoreAPI_Dll HttpConnectionManager {
	public:

		std::unordered_map<HttpWorkloadType, DiscordCoreAPI::UniquePtrWrapper<HttpConnection>> httpConnections{};
		std::unordered_map<std::string, DiscordCoreAPI::UniquePtrWrapper<RateLimitData>> rateLimitValues{};

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

		template<typename ReturnType>
		friend ReturnType submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload);
		friend void submitWorkloadAndGetResult(HttpWorkloadData& workload, HttpClient& httpClient);
		friend std::vector<HttpData> submitWorkloadAndGetResult(HttpClient& httpClient, std::vector<HttpWorkloadData>& workload);
		friend HttpData submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload);

		HttpClient(std::string);

	protected:

		HttpConnectionManager connectionManager{};
		const std::string botToken{};

		HttpData executeByRateLimitData(HttpWorkloadData&, bool, HttpConnection& theConnection);

		HttpData executeHttpRequest(HttpWorkloadData&, HttpConnection& theConnection);

		std::vector<HttpData> executeHttpRequest(std::vector<HttpWorkloadData>&);

		std::vector<HttpData> httpRequest(std::vector<HttpWorkloadData>&);

		HttpData getResponse(HttpConnection& theConnection);

		HttpData httpRequest(HttpWorkloadData&, bool);
	};

	template<typename ReturnType>
	ReturnType submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload) {
		try {
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + httpClient.botToken));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
			workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			HttpData returnData = httpClient.httpRequest(workload, true);
			ReturnType returnObject{};
			DataParser::parseObject(returnData.responseData, returnObject);
			return returnObject;
		}
		catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
		}
		ReturnType returnObject{};
		return returnObject;
	}

	template<>
	void submitWorkloadAndGetResult<void>(HttpClient& httpClient, HttpWorkloadData& workload);

	HttpData submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload);

	std::vector<HttpData> submitWorkloadAndGetResult(HttpClient& httpClient, std::vector<HttpWorkloadData>& workload);

}
