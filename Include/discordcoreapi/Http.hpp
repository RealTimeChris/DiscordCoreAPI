/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// Http.hpp - Header file for the "Http Stuff".
/// May 12, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file Http.hpp

#pragma once

#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <semaphore>

namespace DiscordCoreInternal {

	class HttpConnectionManager;
	struct RateLimitData;
	struct HttpData;

	struct HttpError : public std::runtime_error {
	  public:
		explicit HttpError(std::string&& message) : std::runtime_error(message.c_str()){};

		explicit HttpError(std::string& message) : std::runtime_error(message.c_str()){};
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	  public:
		friend HttpClient;

		void constructHeaderValues(std::unordered_map<std::string, std::string>& headers, RateLimitData* theConnection);

		HttpData handleHeaders(RateLimitData* rateLimitDataPtr);

		std::string buildRequest(HttpWorkloadData& workload);

		bool checkForHeadersToParse(std::string&);

		void parseHeaders(std::string&);

		bool parseChunk(std::string&);

		void resetValues();

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
		friend HttpConnectionManager;
		friend HttpRnRBuilder;
		friend HttpClient;

		RateLimitData() = default;

	  protected:
		std::counting_semaphore<1> theSemaphore{ 1 };
		std::atomic_int32_t currentWorkloadId{ 0 };
		bool haveWeCollectedTime{ false };
		bool areWeASpecialBucket{ false };
		int64_t getsRemainingTotal{ 0 };
		std::string tempBucket{ "" };
		int64_t sampledTimeInMs{ 0 };
		int32_t getsRemaining{ 0 };
		int64_t msRemainTotal{ 0 };
		std::string bucket{ "" };
		int64_t msRemain{ 0 };
	};

	struct DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public HttpRnRBuilder {
	  public:
		int32_t currentRecursionDepth{ 0 };
		int32_t maxRecursion{ 10 };
		int64_t lastTimeUsed{ 0 };
		bool doWeConnect{ true };
		virtual ~HttpConnection() = default;
	};

	namespace Globals {
		static std::unordered_map<std::thread::id, std::unique_ptr<HttpConnection>> httpConnections;
	}

	class DiscordCoreAPI_Dll HttpConnectionManager {
	  public:
		int64_t currentIndex{ 0 };
		std::mutex theMutex{};

		HttpConnection* getConnection();

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
		HttpClient() = default;

		HttpClient(std::string, bool doWePrintHttp, bool doWePrintFFMPEG);

		std::vector<HttpData> httpRequest(std::vector<HttpWorkloadData>&);

		HttpData httpRequest(HttpWorkloadData&);

		bool getDoWePrintFFMPEG();

		std::string getBotToken();

	  protected:
		HttpConnectionManager connectionManager{};
		bool doWePrintFFMPEG{ false };
		const std::string botToken{};
		bool doWePrintHttp{ false };

		HttpData executeHttpRequest(HttpWorkloadData& workload, HttpConnection& theConnection, RateLimitData* rateLimitDatPtr);

		HttpData executeByRateLimitData(HttpWorkloadData& workload, HttpConnection& theConnection);

		HttpData getResponse(HttpConnection& theConnection, RateLimitData* rateLimitDataPtr);

		std::vector<HttpData> executeHttpRequest(std::vector<HttpWorkloadData>&);
	};

	template<typename ReturnType> ReturnType submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload) {
		ReturnType returnObject{};
		try {
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + httpClient.getBotToken()));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://discordcoreapi.com 1.0)"));
			workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			HttpData returnData = httpClient.httpRequest(workload);
			DataParser::parseObject(returnData.responseData, returnObject);
			return returnObject;
		} catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()", nullptr, true);
		}
		return returnObject;
	}

	template<> void submitWorkloadAndGetResult<void>(HttpClient& httpClient, HttpWorkloadData& workload);

	HttpData submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload);

	std::vector<HttpData> submitWorkloadAndGetResult(HttpClient& httpClient, std::vector<HttpWorkloadData>& workload);

}// namespace DiscordCoreInternal
