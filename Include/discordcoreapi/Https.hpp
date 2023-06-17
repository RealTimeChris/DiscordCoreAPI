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

namespace DiscordCoreInternal {

	class RateLimitManager;
	class HttpsConnection;
	struct RateLimitData;

	enum class HttpsState { Collecting_Headers = 0, Collecting_Contents = 1, Collecting_Chunked_Contents = 2, Complete = 3 };

	class HttpsError : public DiscordCoreAPI::DCAException {
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

		bool parseSize() noexcept;
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

		DiscordCoreAPI::String inputBufferReal{};
		const int32_t maxReconnectTries{ 3 };
		HttpsTCPConnection tcpConnection{};
		int32_t currentReconnectTries{};
		HttpsWorkloadData workload{};
		std::string currentBaseUrl{};
		HttpsResponseData data{};

		HttpsConnection() noexcept;

		void resetValues(HttpsWorkloadData&& workloadNew) noexcept;

		bool areWeConnected() noexcept;

		void disconnect() noexcept;

		virtual ~HttpsConnection() noexcept = default;
	};

	inline thread_local std::unordered_map<HttpsWorkloadType, DiscordCoreAPI::UniquePtr<HttpsConnection>> httpsConnections{};

	class RateLimitManager {
	  public:
		friend class HttpsClient;

		RateLimitManager() noexcept = default;

		HttpsConnection& getConnection(HttpsWorkloadType workloadType) noexcept;

		RateLimitData& getRateLimitData(HttpsWorkloadType workloadType) noexcept;

		void initialize() noexcept;

	  protected:
		std::unordered_map<std::string, DiscordCoreAPI::UniquePtr<RateLimitData>> rateLimitValues{};
		std::unordered_map<HttpsWorkloadType, std::string> rateLimitValueBuckets{};
		std::mutex accessMutex{};

		std::unordered_map<std::string, DiscordCoreAPI::UniquePtr<RateLimitData>>& getRateLimitValues() noexcept;

		std::unordered_map<HttpsWorkloadType, std::string>& getRateLimitValueBuckets() noexcept;
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
