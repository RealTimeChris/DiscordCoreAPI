/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// Https.hpp - Header file for the "Https Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.hpp

#pragma once

#include <discordcoreapi/SSLClients.hpp>
#include <semaphore>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpsConnectionManager;
	struct DiscordCoreAPI_Dll RateLimitData;

	enum class HttpsState { Collecting_Code = 0, Collecting_Headers = 1, Collecting_Size = 2, Collecting_Contents = 3 };

	class DiscordCoreAPI_Dll HttpsError : public DiscordCoreAPI::DCAException {
	  public:
		int32_t errorCode{};
		explicit HttpsError(std::string message);
	};

	struct DiscordCoreAPI_Dll HttpsResponseData {
		friend class HttpsRnRBuilder;
		friend class HttpsConnection;
		friend class HttpsClient;

		std::unordered_map<std::string, std::string> responseHeaders{};
		HttpsResponseCode responseCode{ static_cast<uint32_t>(-1) };
		HttpsState currentState{ HttpsState::Collecting_Code };
		std::string responseData{};
		uint64_t contentLength{};

	  protected:
		bool isItChunked{};
	};

	class DiscordCoreAPI_Dll HttpsRnRBuilder {
	  public:
		friend class HttpsClient;

		HttpsRnRBuilder(bool doWePrintErrorMessages);

		void updateRateLimitData(RateLimitData& connection, std::unordered_map<std::string, std::string>& headers);

		HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData);

		std::string buildRequest(const HttpsWorkloadData& workload);

		uint64_t parseHeaders(StringBuffer& other);

		bool parseChunk(StringBuffer& other);

		virtual ~HttpsRnRBuilder() noexcept = default;

	  protected:
		bool doWePrintErrorMessages{};
		bool doWeHaveContentSize{};
		bool doWeHaveHeaders{};
		bool isItChunked{};

		uint64_t parseSize(StringBuffer& other);

		uint64_t parseCode(StringBuffer& other);

		void clearCRLF(StringBuffer& other);
	};

	struct DiscordCoreAPI_Dll RateLimitData {
		friend class HttpsConnectionManager;
		friend class HttpsRnRBuilder;
		friend class HttpsClient;

	  protected:
		std::atomic<Milliseconds> sampledTimeInMs{ Milliseconds{} };
		std::atomic<Milliseconds> msRemain{ Milliseconds{} };
		std::counting_semaphore<1> theSemaphore{ 1 };
		std::atomic_bool areWeASpecialBucket{};
		std::atomic_bool didWeHitRateLimit{};
		std::atomic_int64_t getsRemaining{};
		std::atomic_bool haveWeGoneYet{};
		std::atomic_bool doWeWait{};
		std::string tempBucket{};
		std::string bucket{};
	};

	class DiscordCoreAPI_Dll HttpsConnection : public TCPSSLClient, public HttpsRnRBuilder {
	  public:
		const int32_t maxReconnectTries{ 10 };
		simdjson::ondemand::parser parser{};
		std::atomic_bool areWeCheckedOut{};
		int32_t currentReconnectTries{};
		StringBuffer inputBufferReal{};
		std::string currentBaseUrl{};
		bool areWeDoneTheRequest{};
		HttpsResponseData data{};
		bool doWeConnect{ true };

		HttpsConnection(bool doWePrintErrorMessages);

		void handleBuffer() noexcept;

		void disconnect() noexcept;

		void resetValues();

		virtual ~HttpsConnection() noexcept = default;
	};

	class DiscordCoreAPI_Dll HttpsConnectionManager {
	  public:
		HttpsConnectionManager(DiscordCoreAPI::ConfigManager*);

		std::unordered_map<std::string, std::unique_ptr<RateLimitData>>& getRateLimitValues();

		std::unordered_map<HttpsWorkloadType, std::string>& getRateLimitValueBuckets();

		HttpsConnection* getConnection();

		void initialize();

	  protected:
		std::unordered_map<std::string, std::unique_ptr<RateLimitData>> rateLimitValues{};
		std::unordered_map<int64_t, std::unique_ptr<HttpsConnection>> httpsConnections{};
		std::unordered_map<HttpsWorkloadType, std::string> rateLimitValueBuckets{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		std::mutex accessMutex{};
		int64_t currentIndex{};
	};

	template<typename OTy>
	concept SameAsVoid = std::same_as<void, OTy>;

	class DiscordCoreAPI_Dll HttpsClient {
	  public:
		HttpsClient(DiscordCoreAPI::ConfigManager* configManager);

		template<typename RTy> RTy submitWorkloadAndGetResult(const HttpsWorkloadData& workload, RTy* returnValue) {
			workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
			auto httpsConnection = this->connectionManager.getConnection();
			HttpsResponseData returnData = this->httpsRequest(httpsConnection, workload);

			if (static_cast<uint32_t>(returnData.responseCode) != 200 && static_cast<uint32_t>(returnData.responseCode) != 204 &&
				static_cast<uint32_t>(returnData.responseCode) != 201) {
				HttpsError theError{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack +
					" Https Error: " + static_cast<std::string>(returnData.responseCode) + "\nThe Request: " + workload.content +
					DiscordCoreAPI::reset() + "" };
				theError.errorCode = returnData.responseCode;
				httpsConnection->areWeCheckedOut.store(false);
				throw theError;
			}
			if (returnData.responseData.size() > 0 && returnData.responseData.size() >= returnData.contentLength) {
				returnData.responseData.reserve(returnData.responseData.size() + simdjson::SIMDJSON_MAXSIZE_BYTES);
				simdjson::ondemand::document document{};
				if (httpsConnection->parser
						.iterate(returnData.responseData.data(), returnData.responseData.length(), returnData.responseData.capacity())
						.get(document) == simdjson::error_code::SUCCESS) {
					if (document.type() != simdjson::ondemand::json_type::null) {
						simdjson::ondemand::value object{};
						if (document.get(object) == simdjson::error_code::SUCCESS) {
							if (returnValue) {
								*returnValue = RTy{ object };
								httpsConnection->areWeCheckedOut.store(false);
								return *returnValue;
							} else {
								httpsConnection->areWeCheckedOut.store(false);
								return RTy{ object };
							}
						}
					}
				}
			}
			httpsConnection->areWeCheckedOut.store(false);
			return RTy{};
		}

		template<SameAsVoid RTy> RTy submitWorkloadAndGetResult(const HttpsWorkloadData& workload, RTy* returnValue = nullptr);

		HttpsResponseData submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew);

		HttpsResponseData httpsRequest(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload);

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		HttpsConnectionManager connectionManager{ nullptr };

		HttpsResponseData httpsRequestInternal(HttpsConnection* connection, const HttpsWorkloadData& workload,
			RateLimitData& rateLimitData);

		HttpsResponseData executeByRateLimitData(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload,
			RateLimitData& rateLimitData);

		HttpsResponseData getResponse(HttpsConnection* connection, RateLimitData& rateLimitData);
	};

}// namespace DiscordCoreInternal
