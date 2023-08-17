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
/// HttpsClient.hpp - Header file for the "Https Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file HttpsClient.hpp

#pragma once

#include <discordcoreapi/Utilities/RateLimitQueue.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/// \brief Voice Websocket close codes.
		class HttpsResponseCode {
		  public:
			/// \brief Voice Websocket close codes.
			enum class HttpsResponseCodes : uint32_t {
				Ok = 200,///< The request completed successfully.
				Created = 201,///< The entity was created successfully.
				No_Content = 204,///< The request completed successfully but returned no content.
				Not_Modifies = 304,///< The entity was not modified (no action was taken).
				Bad_Request = 400,///< The request was improperly formatted, or the server couldn't understand it.
				Unauthorized = 401,///< The Authorization header was missing or invalid.
				Forbidden = 403,///< The Authorization token you passed did not have permission to the resource.
				Not_Found = 404,///< The resource at the location specified doesn't exist.
				Method_Not_Allowed = 405,///< The HTTPS method used is not valid for the location specified.
				Too_Many_Requests = 429,///< You are being rate limited, see Rate Limits.
				Gatewat_Unavailable = 502,///< There was not a gateway available to process your request. Wait a bit and retry.
			};

			static inline UnorderedMap<HttpsResponseCodes, std::string_view> outputErrorValues{ { static_cast<HttpsResponseCodes>(200),
																									"The request completed successfully" },
				{ static_cast<HttpsResponseCodes>(201), "The entity was created successfully" },
				{ static_cast<HttpsResponseCodes>(204), "The request completed successfully but returned no content" },
				{ static_cast<HttpsResponseCodes>(304), "The entity was not modified (no action was taken)" },
				{ static_cast<HttpsResponseCodes>(400), "The request was improperly formatted, or the server couldn't understand it" },
				{ static_cast<HttpsResponseCodes>(401), "The Authorization header was missing or invalid" },
				{ static_cast<HttpsResponseCodes>(403), "The Authorization token you passed did not have permission to the resource" },
				{ static_cast<HttpsResponseCodes>(404), "The resource at the location specified doesn't exist" },
				{ static_cast<HttpsResponseCodes>(405), "The HTTPS method used is not valid for the location specified" },
				{ static_cast<HttpsResponseCodes>(429), "You are being rate limited, see Rate Limits" },
				{ static_cast<HttpsResponseCodes>(502), "There was not a gateway available to process your request.Wait a bit and retry" },
				{ static_cast<HttpsResponseCodes>(500), "The server had an error processing your request(these are rare)" } };

			HttpsResponseCodes value{};

			inline HttpsResponseCode& operator=(uint32_t valueNew) {
				value = static_cast<HttpsResponseCodes>(valueNew);
				return *this;
			}

			inline HttpsResponseCode(uint32_t value) {
				*this = value;
			}

			inline operator std::string() {
				return std::string{ "Code: " + std::to_string(static_cast<uint32_t>(value)) + std::string{ ", Message: " } +
					static_cast<std::string>(HttpsResponseCode::outputErrorValues[value]) };
			}

			inline operator uint64_t() {
				return static_cast<uint64_t>(value);
			}
		};

		class HttpsConnectionManager;
		struct RateLimitData;

		enum class HttpsState { Collecting_Headers = 0, Collecting_Contents = 1, Collecting_Chunked_Contents = 2, Complete = 3 };

		class HttpsError : public DCAException {
		  public:
			int32_t errorCode{};
			inline HttpsError(std::string message, std::source_location location = std::source_location::current())
				: DCAException{ message, location } {};
		};

		struct HttpsResponseData {
			friend class HttpsRnRBuilder;
			friend class HttpsConnection;
			friend class HttpsClient;

			UnorderedMap<std::string, std::string> responseHeaders{};
			HttpsResponseCode responseCode{ static_cast<uint32_t>(-1) };
			HttpsState currentState{ HttpsState::Collecting_Headers };
			std::string responseData{};
			uint64_t contentLength{};

		  protected:
			bool isItChunked{};
		};

		class HttpsConnection;

		class HttpsTCPConnection : public TCPConnection<HttpsTCPConnection> {
		  public:
			HttpsTCPConnection() = default;

			HttpsTCPConnection(const std::string& baseUrlNew, const uint16_t portNew, HttpsConnection* ptrNew);

			void handleBuffer();

		  protected:
			HttpsConnection* ptr{};
		};

		class HttpsRnRBuilder {
		  public:
			friend class HttpsClient;

			HttpsRnRBuilder() = default;

			HttpsResponseData finalizeReturnValues(RateLimitData& rateLimitData);

			std::string buildRequest(const HttpsWorkloadData& workload);

			void updateRateLimitData(RateLimitData& rateLimitData);

			bool parseHeaders();

			virtual ~HttpsRnRBuilder() = default;

		  protected:
			bool parseContents();

			bool parseChunk();
		};

		class HttpsConnection : public HttpsRnRBuilder {
		  public:
			friend class HttpsTCPConnection;

			const int32_t maxReconnectTries{ 3 };
			Jsonifier::String inputBufferReal{};
			HttpsTCPConnection tcpConnection{};
			int32_t currentReconnectTries{};
			HttpsWorkloadData workload{};
			std::string currentBaseUrl{};
			HttpsResponseData data{};

			HttpsConnection() = default;

			void resetValues(HttpsWorkloadData&& workloadNew);

			bool areWeConnected();

			void disconnect();

			virtual ~HttpsConnection() = default;
		};

		class HttpsConnectionManager {
		  public:
			friend class HttpsClient;

			HttpsConnectionManager() = default;

			HttpsConnection& getConnection(HttpsWorkloadType workloadType);

		  protected:
			UnorderedMap<HttpsWorkloadType, UniquePtr<HttpsConnection>> httpsConnections{};
			UnorderedMap<std::string, UniquePtr<RateLimitData>> rateLimitValues{};
			UnorderedMap<HttpsWorkloadType, std::string> rateLimitValueBuckets{};
			std::mutex accessMutex{};
		};

		class HttpsConnectionStackHolder {
		  public:
			HttpsConnectionStackHolder(HttpsConnectionManager& connectionManager, HttpsWorkloadData&& workload);

			HttpsConnection& getConnection();

			~HttpsConnectionStackHolder();

		  protected:
			HttpsConnection* connection{};
		};

		class RateLimitStackHolder {
		  public:
			RateLimitStackHolder(HttpsConnectionManager& connectionManager, HttpsWorkloadType workload);

			RateLimitData& getRateLimitData();

			~RateLimitStackHolder();

		  protected:
			RateLimitData* rateLimitData{};
		};

		template<typename ObjectType>
		concept VoidT = std::same_as<ObjectType, void>;

		class DiscordCoreAPI_Dll HttpsClientCore {
		  public:
			HttpsClientCore(const std::string& botTokenNew);

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
					errorMessage +=
						"Https Error: " + returnData.responseCode.operator std::string() + "\nThe Request: Base Url: " + connection.workload.baseUrl;
					if (!connection.workload.relativePath.empty()) {
						errorMessage += "\nRelative Url: " + connection.workload.relativePath;
					}
					if (!connection.workload.content.empty()) {
						errorMessage += "\nContent: " + connection.workload.content;
					}
					if (!returnData.responseData.empty()) {
						errorMessage += "\nThe Response: " + returnData.responseData;
					}
					HttpsError theError{ errorMessage };
					theError.errorCode = returnData.responseCode;
					throw theError;
				}
				return returnData;
			}

		  protected:
			std::string botToken{};

			HttpsResponseData httpsRequestInternal(HttpsConnection& connection, RateLimitData& rateLimitData);

			HttpsResponseData recoverFromError(HttpsConnection& connection, RateLimitData& rateLimitData);

			HttpsResponseData getResponse(HttpsConnection& connection, RateLimitData& rateLimitData);
		};

		class DiscordCoreAPI_Dll HttpsClient : public HttpsClientCore {
		  public:
			HttpsClient(const std::string& botTokenNew);

			template<typename... Args> void submitWorkloadAndGetResult(HttpsWorkloadData&& workload, Args&... args) {
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
						"\nThe Request: Base Url: " + connection.workload.baseUrl + "\n";
					if (!connection.workload.relativePath.empty()) {
						errorMessage += "Relative Url: " + connection.workload.relativePath + "\n";
					}
					if (!connection.workload.content.empty()) {
						errorMessage += "Content: " + connection.workload.content + "\n";
					}
					if (!returnDataNew.responseData.empty()) {
						errorMessage += "The Response: " + returnDataNew.responseData;
					}
					HttpsError theError{ errorMessage };
					theError.errorCode = returnDataNew.responseCode;
					throw theError;
				}

				if constexpr ((( !std::is_void_v<Args> ) || ...)) {
					if (returnDataNew.responseData.size() > 0) {
						(parser.parseJson<true, true>(args, returnDataNew.responseData), ...);
					}
				}
			}

		  protected:
			HttpsConnectionManager connectionManager{};
			RateLimitQueue rateLimitQueue{};

			HttpsResponseData executeByRateLimitData(HttpsConnection& connection, RateLimitData& rateLimitData);

			HttpsResponseData httpsRequest(HttpsConnection& connection);
		};

	}// namespace DiscordCoreInternal
}
