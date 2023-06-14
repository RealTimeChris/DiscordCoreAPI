/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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

#include <discordcoreapi/TCPConnection.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <semaphore>
#include <stack>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpsConnectionManager;
	class DiscordCoreAPI_Dll HttpsConnection;
	struct DiscordCoreAPI_Dll RateLimitData;

	enum class HttpsState { Collecting_Code = 0, Collecting_Headers = 1, Collecting_Contents = 3, Collecting_Size = 4 };

	class DiscordCoreAPI_Dll HttpsError : public DiscordCoreAPI::DCAException {
	  public:
		int32_t errorCode{};
		explicit HttpsError(std::string message, std::source_location = std::source_location::current());
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
		bool isThereJsonData{};

	  protected:
		bool isItChunked{};
	};

	class DiscordCoreAPI_Dll HttpsTCPConnection : public TCPConnection {
	  public:
		HttpsTCPConnection(const std::string& baseUrlNew, const uint16_t portNew, bool doWePrintErrorsNew, HttpsConnection* ptrNew);

		void handleBuffer() noexcept;

		void disconnect() noexcept;

	  protected:
		HttpsConnection* ptr{};
	};

	class DiscordCoreAPI_Dll HttpsRnRBuilder {
	  public:
		friend class HttpsClient;

		HttpsRnRBuilder(bool doWePrintErrors);

		void updateRateLimitData(RateLimitData& connection, std::unordered_map<std::string, std::string>& headers);

		HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData);

		std::string buildRequest(const HttpsWorkloadData& workload);

		uint64_t parseHeaders();

		virtual ~HttpsRnRBuilder() noexcept = default;

	  protected:
		bool doWePrintErrors{};
		bool doWeHaveContentSize{};
		bool doWeHaveHeaders{};
		bool isItChunked{};

		void parseChunkHeaders();

		void parseChunkSize();

		void parseContents();

		void parseChunk();

		void parseSize();

		void parseCode();

		void clearCRLF();
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

	inline thread_local Jsonifier::JsonifierCore httpsParser{};

	class DiscordCoreAPI_Dll HttpsConnection : public HttpsRnRBuilder {
	  public:
		friend class HttpsTCPConnection;

		std::unique_ptr<HttpsTCPConnection> tcpConnection{};
		DiscordCoreAPI::String inputBufferReal{};
		const int32_t maxReconnectTries{ 10 };
		std::atomic_bool areWeCheckedOut{};
		int32_t currentReconnectTries{};
		std::string currentBaseUrl{};
		bool areWeDoneTheRequest{};
		HttpsResponseData data{};
		bool doWeConnect{ true };

		HttpsConnection(bool doWePrintErrors);

		void disconnect() noexcept;

		void resetValues();

		virtual ~HttpsConnection() noexcept = default;
	};

	class DiscordCoreAPI_Dll HttpsConnectionManager {
	  public:
		HttpsConnectionManager() noexcept = default;

		HttpsConnectionManager(DiscordCoreAPI::ConfigManager*);

		std::unordered_map<std::string, std::unique_ptr<RateLimitData>>& getRateLimitValues();

		std::unordered_map<HttpsWorkloadType, std::string>& getRateLimitValueBuckets();

		HttpsConnection* getConnection();

		void initialize();

	  protected:
		std::unordered_map<std::string, std::unique_ptr<RateLimitData>> rateLimitValues{};
		std::unordered_map<int64_t, std::unique_ptr<HttpsConnection>> httpsConnections{};
		std::unordered_map<HttpsWorkloadType, std::string> rateLimitValueBuckets{};
		DiscordCoreAPI::ConfigManager* configManager{};
		std::mutex accessMutex{};
		int64_t currentIndex{};
	};

	template<typename ObjectType>
	concept IsVoid = std::same_as<ObjectType, void>;

	class DiscordCoreAPI_Dll HttpsClient {
	  public:
		HttpsClient(DiscordCoreAPI::ConfigManager* configManager);

		template<typename... Args> void SubmitWorkloadAndGetResult(Args... args);

		template<typename RTy> void submitWorkloadAndGetResult(const HttpsWorkloadData& workload, RTy& returnData) {
			workload.headersToInsert["Authorization"] = "Bot " + configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
			auto httpsConnection = connectionManager.getConnection();
			HttpsResponseData returnDataNew = httpsRequest(httpsConnection, workload);

			if (static_cast<uint32_t>(returnDataNew.responseCode) != 200 && static_cast<uint32_t>(returnDataNew.responseCode) != 204 &&
				static_cast<uint32_t>(returnDataNew.responseCode) != 201) {
				HttpsError theError{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack +
					" Https Error: " + returnDataNew.responseCode.operator std::string() + "\nThe Request: " + workload.baseUrl +
					workload.relativePath + "\n" + workload.content + DiscordCoreAPI::reset() + "\nThe Response: " + returnDataNew.responseData };
				theError.errorCode = returnDataNew.responseCode;
				httpsConnection->areWeCheckedOut.store(false);
				throw theError;
			}
			if (returnDataNew.responseData.size() > 0) {
				httpsParser.parseJson<true>(returnData, returnDataNew.responseData);
			}
			httpsConnection->areWeCheckedOut.store(false);
			return;
		}

		template<IsVoid RTy> void submitWorkloadAndGetResult(const HttpsWorkloadData& workload) {
			workload.headersToInsert["Authorization"] = "Bot " + configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
			auto httpsConnection = connectionManager.getConnection();
			HttpsResponseData returnDataNew = httpsRequest(httpsConnection, workload);

			if (static_cast<uint32_t>(returnDataNew.responseCode) != 200 && static_cast<uint32_t>(returnDataNew.responseCode) != 204 &&
				static_cast<uint32_t>(returnDataNew.responseCode) != 201) {
				HttpsError theError{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack +
					" Https Error: " + returnDataNew.responseCode.operator std::string() + "\nThe Request: " + workload.baseUrl +
					workload.relativePath + "\n" + workload.content + DiscordCoreAPI::reset() + "\nThe Response: " + returnDataNew.responseData };
				theError.errorCode = returnDataNew.responseCode;
				httpsConnection->areWeCheckedOut.store(false);
				throw theError;
			}
			httpsConnection->areWeCheckedOut.store(false);
			return;
		}

		HttpsResponseData submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew);

		HttpsResponseData httpsRequest(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload);

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{};
		HttpsConnectionManager connectionManager{};

		HttpsResponseData httpsRequestInternal(HttpsConnection* connection, const HttpsWorkloadData& workload, RateLimitData& rateLimitData);

		HttpsResponseData executeByRateLimitData(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload, RateLimitData& rateLimitData);

		HttpsResponseData getResponse(HttpsConnection* connection, RateLimitData& rateLimitData);
	};

}// namespace DiscordCoreInternal
