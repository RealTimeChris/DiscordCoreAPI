/*
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

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpsConnectionManager;
	struct DiscordCoreAPI_Dll RateLimitData;

	enum class HttpsState { Collecting_Code = 0, Collecting_Headers = 1, Collecting_Size = 2, Collecting_Contents = 3 };

	class HttpError : public std::runtime_error {
	  public:
		int32_t errorCode{};
		HttpError(std::string theMessage);
	};

	struct DiscordCoreAPI_Dll HttpsResponseData {
		friend class HttpsRnRBuilder;
		friend class HttpsClient;

		std::unordered_map<std::string, std::string> responseHeaders{};
		HttpsState theCurrentState{ HttpsState::Collecting_Code };
		std::string responseMessage{};
		nlohmann::json responseData{};
		int64_t responseCode{ -1 };

	  protected:
		int64_t contentSize{ 0 };
	};

	class DiscordCoreAPI_Dll HttpsRnRBuilder {
	  public:
		friend class HttpsClient;

		void updateRateLimitData(std::unordered_map<std::string, std::string>& headers, RateLimitData& theConnection);

		HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData, HttpsResponseData& theData);

		std::string buildRequest(const HttpsWorkloadData& workload);

		void parseHeaders(std::string&, HttpsResponseData& theData);

		bool parseChunk(std::string&, HttpsResponseData& theData);

		bool checkForHeadersToParse(const std::string&);

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
		friend class HttpsConnectionManager;
		friend class HttpsRnRBuilder;
		friend class HttpsClient;

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

	class DiscordCoreAPI_Dll HttpsConnection : public SSLClient, public HttpsRnRBuilder {
	  public:
		std::atomic_bool areWeCheckedOut{ false };
		const int32_t maxReconnectTries{ 10 };
		int32_t currentReconnectTries{ 0 };
		std::string currentBaseUrl{};
		bool doWeConnect{ true };

		void disconnect(bool) noexcept;

		virtual ~HttpsConnection() = default;
	};

	class DiscordCoreAPI_Dll HttpsConnectionManager {
	  public:
		std::unordered_map<std::string, std::unique_ptr<RateLimitData>>& getRateLimitValues();

		std::unordered_map<HttpsWorkloadType, std::string>& getRateLimitValueBuckets();

		HttpsConnection* getConnection();

		void initialize();

	  protected:
		std::unordered_map<std::string, std::unique_ptr<RateLimitData>> rateLimitValues{};
		std::unordered_map<int64_t, std::unique_ptr<HttpsConnection>> httpsConnections{};
		std::unordered_map<HttpsWorkloadType, std::string> rateLimitValueBuckets{};
		int64_t currentIndex{};
		std::mutex theMutex{};
	};

	class DiscordCoreAPI_Dll HttpsClient {
	  public:
		HttpsClient(DiscordCoreAPI::ConfigManager* configManager);

		template<typename ReturnType> ReturnType submitWorkloadAndGetResult(HttpsWorkloadData& workload) {
			workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
			HttpsResponseData returnData = this->httpRequest(workload);
			if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
				std::string theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
					", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
				HttpError theError{ theErrorMessage };
				theError.errorCode = returnData.responseCode;
				throw theError;
			}
			ReturnType returnObject{ returnData.responseData };
			return returnObject;
		}

		template<std::same_as<void> Type> Type submitWorkloadAndGetResult(HttpsWorkloadData& workload);

		HttpsResponseData submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew);

		HttpsResponseData httpRequest(HttpsWorkloadData&);

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		HttpsConnectionManager connectionManager{};

		HttpsResponseData httpRequestInternal(const HttpsWorkloadData& workload, RateLimitData& rateLimitData);

		HttpsResponseData getResponse(RateLimitData& rateLimitData, HttpsConnection& theConnection);

		bool sendData(const std::string& dataToSend, bool priority, HttpsConnection& theConnection);

		HttpsResponseData executeByRateLimitData(const HttpsWorkloadData& workload);
	};



}// namespace DiscordCoreInternal
