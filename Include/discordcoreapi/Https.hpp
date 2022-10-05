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
/// Https.hpp - Header file for the "Http Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.hpp

#pragma once

#ifndef HTTPS
	#define HTTPS

	#include <discordcoreapi/SSLClients.hpp>
	#include <semaphore>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpsConnectionManager;
	struct DiscordCoreAPI_Dll RateLimitData;

	enum class HttpsState { Collecting_Code = 0, Collecting_Headers = 1, Collecting_Size = 2, Collecting_Contents = 3 };

	class HttpError : public std::runtime_error {
	  public:
		Int32 errorCode{};
		explicit HttpError(String theMessage);
	};

	struct DiscordCoreAPI_Dll HttpsResponseData {
		friend class HttpsRnRBuilder;
		friend class HttpsConnection;
		friend class HttpsClient;

		std::unordered_map<String, String> responseHeaders{};
		HttpsState theCurrentState{ HttpsState::Collecting_Code };
		String responseMessage{};
		Int64 responseCode{ -1 };
		Uint64 contentLength{};

	  protected:
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ 500ms };
		Bool isItChunked{ false };
	};

	class DiscordCoreAPI_Dll HttpsRnRBuilder {
	  public:
		friend class HttpsClient;

		HttpsRnRBuilder(Bool doWePrintErrorMessages);

		void updateRateLimitData(RateLimitData& theConnection, std::unordered_map<String, String>& headers);

		HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData);

		String buildRequest(const HttpsWorkloadData& workload);

		Uint64 parseHeaders(StringBuffer& other);

		Bool parseChunk(StringBuffer& other);

		virtual ~HttpsRnRBuilder() noexcept = default;

	  protected:
		Bool doWePrintErrorMessages{ false };
		Bool doWeHaveContentSize{ false };
		Bool doWeHaveHeaders{ false };
		Bool isItChunked{ false };

		Uint64 parseSize(StringBuffer& other);

		Uint64 parseCode(StringBuffer& other);

		void clearCRLF(StringBuffer& other);
	};

	struct DiscordCoreAPI_Dll RateLimitData {
		friend class HttpsConnectionManager;
		friend class HttpsRnRBuilder;
		friend class HttpsClient;

	  protected:
		AtomicBool areWeASpecialBucket{ false };
		std::counting_semaphore<1> theSemaphore{ 1 };
		AtomicBool didWeHitRateLimit{ false };
		AtomicInt64 sampledTimeInMs{ 0 };
		AtomicBool haveWeGoneYet{ false };
		AtomicInt64 getsRemaining{ 0 };
		AtomicBool doWeWait{ false };
		AtomicInt64 msRemain{ 0 };
		String tempBucket{};
		String bucket{};
	};

	class DiscordCoreAPI_Dll HttpsConnection : public SSLClient, public HttpsRnRBuilder {
	  public:
		AtomicBool areWeCheckedOut{ false };
		const Int32 maxReconnectTries{ 10 };
		Int32 currentReconnectTries{ 0 };
		Bool areWeDoneTheRequest{ false };
		StringBuffer theInputBufferReal{};
		String currentBaseUrl{};
		HttpsResponseData theData{};
		Bool doWeConnect{ true };

		HttpsConnection(Bool doWePrintErrorMessages);

		void disconnect(bool) noexcept;

		void handleBuffer() noexcept;

		void resetValues();

		virtual ~HttpsConnection() noexcept = default;
	};

	class DiscordCoreAPI_Dll HttpsConnectionManager {
	  public:
		HttpsConnectionManager(DiscordCoreAPI::ConfigManager*);

		std::unordered_map<String, std::unique_ptr<RateLimitData>>& getRateLimitValues();

		std::unordered_map<HttpsWorkloadType, String>& getRateLimitValueBuckets();

		HttpsConnection* getConnection();

		void initialize();

	  protected:
		std::unordered_map<String, std::unique_ptr<RateLimitData>> rateLimitValues{};
		std::unordered_map<Int64, std::unique_ptr<HttpsConnection>> httpsConnections{};
		std::unordered_map<HttpsWorkloadType, String> rateLimitValueBuckets{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		Int64 currentIndex{};
		std::mutex theMutex{};
	};

	template<typename ObjectType>
	concept SameAsVoid = std::same_as<void, ObjectType>;

	class DiscordCoreAPI_Dll HttpsClient {
	  public:
		HttpsClient(DiscordCoreAPI::ConfigManager* configManager);

		template<typename ReturnType> ReturnType submitWorkloadAndGetResult(const HttpsWorkloadData& workload, ReturnType* theReturnValue = nullptr) {
			workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
			HttpsResponseData returnData = this->httpRequest(workload);
			if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
				String theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
					", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
				HttpError theError{ theErrorMessage };
				theError.errorCode = returnData.responseCode;
				throw theError;
			}
			if (theReturnValue == nullptr) {
				ReturnType theReturnValueNew{};
				simdjson::ondemand::parser theParser{};
				if (returnData.responseMessage.size() > 0) {
					returnData.responseMessage.reserve(returnData.responseMessage.size() + simdjson::SIMDJSON_PADDING);
					auto theDocument = theParser.iterate(returnData.responseMessage.data(), returnData.responseMessage.length(), returnData.responseMessage.capacity());
					if (theDocument.type() != simdjson::ondemand::json_type::null) {
						simdjson::ondemand::value theObject{};
						auto theResult = theDocument.get(theObject);
						if (theObject.type() != simdjson::ondemand::json_type::null) {
							theReturnValueNew = ReturnType{ theObject };
						}
					}
				}
				return theReturnValueNew;
			} else {
				simdjson::ondemand::parser theParser{};
				if (returnData.responseMessage.size() > 0) {
					returnData.responseMessage.reserve(returnData.responseMessage.size() + simdjson::SIMDJSON_PADDING);
					auto theDocument = theParser.iterate(returnData.responseMessage.data(), returnData.responseMessage.length(), returnData.responseMessage.capacity());
					if (theDocument.type() != simdjson::ondemand::json_type::null) {
						simdjson::ondemand::value theObject{};
						auto theResult = theDocument.get(theObject);
						if (theObject.type() != simdjson::ondemand::json_type::null) {
							*theReturnValue = ReturnType{ theObject };
						}
					}
				}
				return *theReturnValue;
			}
		}

		template<SameAsVoid ReturnType> ReturnType submitWorkloadAndGetResult(const HttpsWorkloadData& workload, ReturnType* theReturnValue = nullptr);

		HttpsResponseData submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew);

		HttpsResponseData httpRequest(const HttpsWorkloadData& workload);

	  protected:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		HttpsConnectionManager connectionManager{ nullptr };

		HttpsResponseData httpRequestInternal(HttpsConnection& theConnection, const HttpsWorkloadData& workload, RateLimitData& rateLimitData);

		HttpsResponseData executeByRateLimitData(const HttpsWorkloadData& workload, RateLimitData& rateLimitData);

		HttpsResponseData getResponse(HttpsConnection& theConnection, RateLimitData& rateLimitData);
	};

}// namespace DiscordCoreInternal
#endif