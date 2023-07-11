/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// Https.hpp - Header file for the "Https Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.hpp

#pragma once

#include <discordcoreapi/TCPConnection.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <semaphore>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		class RateLimitManager;
		class HttpsConnection;
		struct RateLimitData;

		enum class HttpsState { Collecting_Headers = 0, Collecting_Contents = 1, Collecting_Chunked_Contents = 2, Complete = 3 };

		class HttpsError : public DCAException {
		  public:
			int32_t errorCode{};
			explicit HttpsError(std::string message, std::source_location = std::source_location::current());
		};

		struct HttpsResponseData {
			friend class HttpsRnRBuilder;
			friend class HttpsConnection;
			friend class HttpsClient;

			std::unordered_map<std::string, std::string> responseHeaders{};
			HttpsResponseCode responseCode{ static_cast<uint32_t>(-1) };
			HttpsState currentState{ HttpsState::Collecting_Headers };
			std::string responseData{};
			uint64_t contentLength{};

		  protected:
			bool isItChunked{};
		};

		class HttpsConnection;

		class HttpsTCPConnection : public TCPConnection<HttpsTCPConnection>, public SSLDataInterface<HttpsTCPConnection> {
		  public:
			HttpsTCPConnection() noexcept = default;

			HttpsTCPConnection(const std::string& baseUrlNew, const uint16_t portNew, HttpsConnection* ptrNew);

			void handleBuffer() noexcept;

			void disconnect() noexcept;

		  protected:
			HttpsConnection* ptr{};
		};

		class HttpsRnRBuilder {
		  public:
			friend class HttpsClient;

			HttpsRnRBuilder() noexcept;

			void updateRateLimitData(RateLimitData& connection, std::unordered_map<std::string, std::string>& headers) noexcept;

			HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData) noexcept;

			std::string buildRequest(const HttpsWorkloadData& workload) noexcept;

			bool parseHeaders() noexcept;

			virtual ~HttpsRnRBuilder() noexcept = default;

		  protected:
			bool parseContents() noexcept;

			bool parseChunk() noexcept;
		};

		struct RateLimitData {
			friend class RateLimitStackHolder;
			friend class RateLimitManager;
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

		class HttpsConnection : public HttpsRnRBuilder {
		  public:
			friend class HttpsTCPConnection;

			const int32_t maxReconnectTries{ 3 };
			HttpsTCPConnection tcpConnection{};
			int32_t currentReconnectTries{};
			HttpsWorkloadData workload{};
			std::string currentBaseUrl{};
			String inputBufferReal{};
			HttpsResponseData data{};

			HttpsConnection() noexcept;

			void resetValues(HttpsWorkloadData&& workloadNew) noexcept;

			bool areWeConnected() noexcept;

			void disconnect() noexcept;

			virtual ~HttpsConnection() noexcept = default;
		};

		inline thread_local std::unordered_map<HttpsWorkloadType, UniquePtr<HttpsConnection>> httpsConnections{};

		class RateLimitManager {
		  public:
			friend class HttpsClient;

			RateLimitManager() noexcept = default;

			HttpsConnection& getConnection(HttpsWorkloadType workloadType) noexcept;

			RateLimitData& getRateLimitData(HttpsWorkloadType workloadType) noexcept;

			void initialize() noexcept;

		  protected:
			std::unordered_map<std::string, UniquePtr<RateLimitData>> rateLimitValues{};
			std::unordered_map<HttpsWorkloadType, std::string> rateLimitValueBuckets{};
			std::mutex accessMutex{};
		};

		class HttpsConnectionStackHolder {
		  public:
			HttpsConnectionStackHolder(RateLimitManager& connectionManager, HttpsWorkloadData&& workload) noexcept;

			HttpsConnection& getConnection() noexcept;

			~HttpsConnectionStackHolder() noexcept;

		  protected:
			HttpsConnection* connection{};
		};

		class RateLimitStackHolder {
		  public:
			RateLimitStackHolder(RateLimitManager& connectionManager, HttpsWorkloadType workload) noexcept;

			RateLimitData& getRateLimitData() noexcept;

			~RateLimitStackHolder() noexcept;

		  protected:
			RateLimitData* rateLimitData{};
		};

		template<typename ObjectType>
		concept VoidT = std::same_as<ObjectType, void>;

		class HttpsClientCore {
		  public:
			HttpsClientCore& operator=(HttpsClientCore&& other) noexcept = default;
			HttpsClientCore(HttpsClientCore&& other) noexcept = default;

			HttpsClientCore(const std::string& botTokenNew) noexcept;

			inline HttpsResponseData submitWorkloadAndGetResult(HttpsWorkloadData&& workloadNew) {
				HttpsConnection connection{};
				RateLimitData rateLimitData{};
				connection.resetValues(std::move(workloadNew));
				auto returnData = httpsRequestInternal(connection, rateLimitData);
				if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
					std::string errorMessage{};
					if (connection.workload.callStack != "") {
						errorMessage += connection.workload.callStack + " ";
					}
					errorMessage += "Https Error: " + returnData.responseCode.operator std::string() +
						"\nThe Request: Base Url: " + connection.workload.baseUrl + "\nRelative Url: " + connection.workload.relativePath +
						"\nContent: " + connection.workload.content + "\nThe Response: " + returnData.responseData;
					HttpsError theError{ errorMessage };
					theError.errorCode = returnData.responseCode;
					throw theError;
				}
				return returnData;
			}

		  protected:
			std::string botToken{};

			HttpsResponseData httpsRequestInternal(HttpsConnection& connection, RateLimitData& rateLimitData) noexcept;

			HttpsResponseData getResponse(HttpsConnection& connection, RateLimitData& rateLimitData) noexcept;
		};

		class HttpsClient : public HttpsClientCore {
		  public:
			HttpsClient(const std::string& botTokenNew) noexcept;

			template<typename... Args> void submitWorkloadAndGetResult(Args... args);

			template<typename ReturnType> void submitWorkloadAndGetResult(HttpsWorkloadData&& workload, ReturnType& returnData) {
				HttpsConnectionStackHolder stackHolder{ connectionManager, std::move(workload) };
				auto& connection = stackHolder.getConnection();
				HttpsResponseData returnDataNew = httpsRequest(connection);

				if (static_cast<uint32_t>(returnDataNew.responseCode) != 200 && static_cast<uint32_t>(returnDataNew.responseCode) != 204 &&
					static_cast<uint32_t>(returnDataNew.responseCode) != 201) {
					std::string errorMessage{};
					if (connection.workload.callStack != "") {
						errorMessage += connection.workload.callStack + " ";
					}
					errorMessage += "Https Error: " + returnDataNew.responseCode.operator std::string() +
						"\nThe Request: Base Url: " + connection.workload.baseUrl + "\nRelative Url: " + connection.workload.relativePath +
						"\nContent: " + connection.workload.content + "\nThe Response: " + returnDataNew.responseData;
					HttpsError theError{ errorMessage };
					theError.errorCode = returnDataNew.responseCode;
					throw theError;
				}
				if (returnDataNew.responseData.size() > 0) {
					parser.parseJson<true, true>(returnData, returnDataNew.responseData);
				}
				return;
			}

			template<VoidT ReturnType> void submitWorkloadAndGetResult(HttpsWorkloadData&& workload) {
				HttpsConnectionStackHolder stackHolder{ connectionManager, std::move(workload) };
				auto& connection = stackHolder.getConnection();
				HttpsResponseData returnDataNew = httpsRequest(connection);

				if (static_cast<uint32_t>(returnDataNew.responseCode) != 200 && static_cast<uint32_t>(returnDataNew.responseCode) != 204 &&
					static_cast<uint32_t>(returnDataNew.responseCode) != 201) {
					std::string errorMessage{};
					if (connection.workload.callStack != "") {
						errorMessage += connection.workload.callStack + " ";
					}
					errorMessage += "Https Error: " + returnDataNew.responseCode.operator std::string() +
						"\nThe Request: Base Url: " + connection.workload.baseUrl + "\nRelative Url: " + connection.workload.relativePath +
						"\nContent: " + connection.workload.content + "\nThe Response: " + returnDataNew.responseData;
					HttpsError theError{ errorMessage };
					theError.errorCode = returnDataNew.responseCode;
					throw theError;
				}
				return;
			}

		  protected:
			RateLimitManager connectionManager{};

			HttpsResponseData executeByRateLimitData(HttpsConnection& connection, RateLimitData& rateLimitData) noexcept;

			HttpsResponseData httpsRequest(HttpsConnection& connection) noexcept;
		};

	}// namespace DiscordCoreInternal
}
