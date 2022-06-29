/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// Https.hpp - Header file for the "Http Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.hpp

#pragma once

#include <discordcoreapi/SSLClients.hpp>
#include <semaphore>

namespace Globals {};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpsConnectionManager;
	struct DiscordCoreAPI_Dll RateLimitData;

	enum class HttpsState { Collecting_Code = 0, Collecting_Headers = 1, Collecting_Size = 2, Collecting_Contents = 3 };

	struct DiscordCoreAPI_Dll HttpsResponseData {
		friend class HttpsRnRBuilder;
		friend class HttpsClient;

		std::unordered_map<std::string, std::string> responseHeaders{};
		HttpsState theCurrentState{ HttpsState::Collecting_Headers };
		std::string responseMessage{};
		nlohmann::json responseData{};
		int64_t responseCode{ -1 };

	  protected:
		int64_t contentSize{ 0 };
	};

	class DiscordCoreAPI_Dll HttpsRnRBuilder {
	  public:
		friend HttpsClient;

		void updateRateLimitData(std::unordered_map<std::string, std::string>& headers, RateLimitData& theConnection);

		HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData, HttpsResponseData& theData);

		std::string buildRequest(const HttpsWorkloadData& workload);

		bool checkForHeadersToParse(const std::string&);

		void parseHeaders(std::string&, HttpsResponseData& theData);

		bool parseChunk(std::string&, HttpsResponseData& theData);

		void resetValues();

		virtual ~HttpsRnRBuilder() = default;

	  protected:
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		std::string inputBufferReal{};
		bool isItChunked{ false };

		void parseSize(std::string&, HttpsResponseData& theData);

		void parseCode(std::string&, HttpsResponseData& theData);

		void clearCRLF(std::string&);
	};

	struct DiscordCoreAPI_Dll RateLimitData {
		friend HttpsConnectionManager;
		friend HttpsRnRBuilder;
		friend HttpsClient;

	  protected:
		std::counting_semaphore<1> theSemaphore{ 1 };
		bool areWeASpecialBucket{ false };
		bool didWeHitRateLimit{ false };
		int64_t sampledTimeInMs{ 0 };
		bool haveWeGoneYet{ false };
		int64_t getsRemaining{ 0 };
		std::string tempBucket{};
		bool doWeWait{ false };
		int64_t msRemain{ 0 };
		std::string bucket{};
	};

	class DiscordCoreAPI_Dll HttpsConnection : public HttpsSSLClient, public HttpsRnRBuilder {
	  public:
		std::atomic_bool areWeCheckedOut{ false };
		int32_t currentRecursionDepth{ 0 };
		const int32_t maxRecursion{ 10 };
		std::string currentBaseUrl{};
		bool doWeConnect{ true };

		virtual ~HttpsConnection() = default;
	};

	class DiscordCoreAPI_Dll HttpsConnectionManager {
	  public:
		int64_t currentIndex{};
		std::mutex theMutex{};

		HttpsConnection* getConnection();

		void initialize();
	};

	class HttpsClient {
	  public:
		HttpsClient() = default;

		HttpsClient(DiscordCoreAPI::ConfigManager* configManager);

		HttpsResponseData httpRequest(HttpsWorkloadData&);

		template<typename ReturnType> ReturnType submitWorkloadAndGetResult(HttpsWorkloadData& workload) {
			workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
			HttpsResponseData returnData = this->httpRequest(workload);
			ReturnType returnObject{ returnData.responseData };
			return returnObject;
		}

		template<std::same_as<void> Type> Type submitWorkloadAndGetResult(HttpsWorkloadData& workload);

		HttpsResponseData submitWorkloadAndGetResult(HttpsWorkloadData* workloadNew);

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		HttpsConnectionManager connectionManager{};

		HttpsResponseData httpRequestInternal(const HttpsWorkloadData& workload, RateLimitData& rateLimitData);

		HttpsResponseData getResponse(RateLimitData& rateLimitData, HttpsConnection* theConnection);

		HttpsResponseData executeByRateLimitData(const HttpsWorkloadData& workload);
	};



}// namespace DiscordCoreInternal
