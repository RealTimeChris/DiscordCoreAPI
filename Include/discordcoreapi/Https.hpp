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

#ifndef HTTPS
#define HTTPS

#include <discordcoreapi/SSLClients.hpp>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpsConnectionManager;
	struct DiscordCoreAPI_Dll RateLimitData;

	enum class HttpsState { Collecting_Code = 0, Collecting_Headers = 1, Collecting_Size = 2, Collecting_Contents = 3 };

	class HttpError : public std::runtime_error {
	  public:
		int32_t errorCode{};
		explicit HttpError(std::string theMessage);
	};

	struct DiscordCoreAPI_Dll HttpsResponseData {
		friend class HttpsRnRBuilder;
		friend class HttpsConnection;
		friend class HttpsClient;

		std::unordered_map<std::string, std::string> responseHeaders{};
		HttpsState theCurrentState{ HttpsState::Collecting_Code };
		std::string responseMessage{};
		std::string responseData{};
		int64_t responseCode{ -1 };

	  protected:
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> theStopWatch{ 500ms };
		uint64_t contentSize{ 0 };
		bool isItChunked{ false };
	};

	class DiscordCoreAPI_Dll HttpsRnRBuilder {
	  public:
		friend class HttpsClient;

		HttpsRnRBuilder(bool doWePrintErrorMessages);

		void updateRateLimitData(RateLimitData& theConnection, std::unordered_map<std::string, std::string>& headers);

		HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData);

		std::string buildRequest(const HttpsWorkloadData& workload);

		size_t parseHeaders(std::string&);

		bool parseChunk(std::string&);

		virtual ~HttpsRnRBuilder() noexcept = default;

	  protected:
		bool doWePrintErrorMessages{ false };
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		bool isItChunked{ false };

		size_t parseSize(std::string&);

		size_t parseCode(std::string& otherNew);

		void clearCRLF(std::string&);
	};

	struct DiscordCoreAPI_Dll RateLimitData {
		friend class HttpsConnectionManager;
		friend class HttpsRnRBuilder;
		friend class HttpsClient;

	  protected:
		std::atomic_bool areWeASpecialBucket{ false };
		std::counting_semaphore<1> theSemaphore{ 1 };
		std::atomic_bool didWeHitRateLimit{ false };
		std::atomic_int64_t sampledTimeInMs{ 0 };
		std::atomic_bool haveWeGoneYet{ false };
		std::atomic_int64_t getsRemaining{ 0 };
		std::atomic_bool doWeWait{ false };
		std::atomic_int64_t msRemain{ 0 };
		std::string tempBucket{};
		std::string bucket{};
	};

	class DiscordCoreAPI_Dll HttpsConnection : public SSLClient, public HttpsRnRBuilder {
	  public:
		std::atomic_bool areWeCheckedOut{ false };
		const int32_t maxReconnectTries{ 10 };
		int32_t currentReconnectTries{ 0 };
		bool areWeDoneTheRequest{ false };
		std::string theInputBufferReal{};
		std::string currentBaseUrl{};
		HttpsResponseData theData{};
		bool doWeConnect{ true };

		HttpsConnection(bool doWePrintErrorMessages);

		bool handleBuffer() noexcept;

		void disconnect(bool) noexcept;

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
		int64_t currentIndex{};
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
				std::string theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
					", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
				HttpError theError{ theErrorMessage };
				theError.errorCode = returnData.responseCode;
				throw theError;
			}
			if (theReturnValue == nullptr) {
				ReturnType theReturnValueNew{};
				simdjson::ondemand::parser theParser{};
				if (returnData.responseData.size() > 0) {
					returnData.responseData.reserve(returnData.responseData.size() + simdjson::SIMDJSON_PADDING);
					auto theDocument = theParser.iterate(returnData.responseData.data(), returnData.responseData.length(), returnData.responseData.capacity());
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
				if (returnData.responseData.size() > 0) {
					returnData.responseData.reserve(returnData.responseData.size() + simdjson::SIMDJSON_PADDING);
					auto theDocument = theParser.iterate(returnData.responseData.data(), returnData.responseData.length(), returnData.responseData.capacity());
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