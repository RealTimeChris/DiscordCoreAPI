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
/// HttpsClient.hpp - Header file for the "Https stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file HttpsClient.hpp
#pragma once

#include <discordcoreapi/Utilities/RateLimitQueue.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		/// \brief Voice websocket close codes.
		class https_response_code {
		  public:
			/// \brief Voice websocket close codes.
			enum class https_response_codes : uint64_t {
				Unset				= std::numeric_limits<uint64_t>::max(),
				ok					= 200,///< The request completed successfully.
				created				= 201,///< The entity was created successfully.
				No_Content			= 204,///< The request completed successfully but returned no content.
				Not_Modifies		= 304,///< The entity was not modified (no action was taken).
				Bad_Request			= 400,///< The request was improperly formatted, or the server couldn't understand it.
				unauthorized		= 401,///< The authorization header was missing or invalid.
				forbidden			= 403,///< The authorization token you passed did not have permission to the resource.
				Not_Found			= 404,///< The resource at the location specified doesn't exist.
				Method_Not_Allowed	= 405,///< The https method used is not valid for the location specified.
				Too_Many_Requests	= 429,///< You are being rate limited, see rate limits.
				Gateway_Unavailable = 502,///< There was not a gateway available to process your request. wait a bit and retry.
			};

			DCA_INLINE static unordered_map<https_response_codes, jsonifier::string> outputErrorValues{
				{ static_cast<https_response_codes>(200), "The request completed successfully" }, { static_cast<https_response_codes>(201), "The entity was created successfully" },
				{ static_cast<https_response_codes>(204), "The request completed successfully but returned no content" },
				{ static_cast<https_response_codes>(304), "The entity was not modified (no action was taken)" },
				{ static_cast<https_response_codes>(400), "The request was improperly formatted, or the server couldn't understand it" },
				{ static_cast<https_response_codes>(401), "The authorization header was missing or invalid" },
				{ static_cast<https_response_codes>(403), "The authorization token you passed did not have permission to the resource" },
				{ static_cast<https_response_codes>(404), "The resource at the location specified doesn't exist" },
				{ static_cast<https_response_codes>(405), "The https method used is not valid for the location specified" },
				{ static_cast<https_response_codes>(429), "you are being rate limited, see rate limits" },
				{ static_cast<https_response_codes>(502), "There was not a gateway available to process your request.wait a bit and retry" },
				{ static_cast<https_response_codes>(500), "The server had an error processing your request(these are rare)" }
			};

			https_response_codes value{};

			DCA_INLINE https_response_code() = default;

			DCA_INLINE https_response_code& operator=(uint64_t valueNew) {
				value = static_cast<https_response_codes>(valueNew);
				return *this;
			}

			DCA_INLINE https_response_code(uint64_t value) {
				*this = value;
			}

			DCA_INLINE operator jsonifier::string() {
				return jsonifier::string{ "Code: " + jsonifier::toString(static_cast<uint32_t>(value)) + jsonifier::string{ ", message: " } +
					static_cast<jsonifier::string>(https_response_code::outputErrorValues[value]) };
			}

			DCA_INLINE operator uint64_t() {
				return static_cast<uint64_t>(value);
			}
		};

		class https_connection_manager;
		struct rate_limit_data;

		enum class https_state { Collecting_Headers = 0, Collecting_Contents = 1, Collecting_Chunked_Contents = 2, complete = 3 };

		class https_error : public dca_exception {
		  public:
			https_response_code errorCode{};
			DCA_INLINE https_error(const jsonifier::string_view& message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
		};

		struct DiscordCoreAPI_Dll https_response_data {
			friend class https_rnr_builder;
			friend class https_connection;
			friend class https_client;

			https_response_code responseCode{ std::numeric_limits<uint32_t>::max() };
			unordered_map<jsonifier::string, jsonifier::string> responseHeaders{};
			https_state currentState{ https_state::Collecting_Headers };
			jsonifier::string responseData{};
			uint64_t contentLength{};

		  protected:
			bool isItChunked{};
		};

		class DiscordCoreAPI_Dll https_rnr_builder {
		  public:
			friend class https_client;

			https_rnr_builder() = default;

			https_response_data finalizeReturnValues(rate_limit_data& rateLimitData);

			jsonifier::string buildRequest(const https_workload_data& workload);

			void updateRateLimitData(rate_limit_data& rateLimitData);

			bool parseHeaders();

			virtual ~https_rnr_builder() = default;

		  protected:
			bool parseContents();

			bool parseChunk();
		};

		class DiscordCoreAPI_Dll https_connection : public https_rnr_builder, public tcp_connection<https_connection> {
		  public:
			template<typename value_type> friend class https_tcp_connection;

			rate_limit_data* currentRateLimitData{};
			const int32_t maxReconnectTries{ 3 };
			jsonifier::string inputBufferReal{};
			jsonifier::string currentBaseUrl{};
			int32_t currentReconnectTries{};
			https_workload_data workload{};
			https_response_data data{};

			https_connection() = default;

			https_connection(const jsonifier::string& baseUrlNew, const uint16_t portNew);

			void resetValues(https_workload_data&& workloadNew, rate_limit_data* newRateLimitData);

			void handleBuffer() override;

			bool areWeConnected();

			void disconnect();

			virtual ~https_connection() = default;
		};

		/// @class https_connection_manager.
		/// @brief For managing the collection of Https connections.
		class DiscordCoreAPI_Dll https_connection_manager {
		  public:
			friend class https_client;

			https_connection_manager() = default;

			https_connection_manager(rate_limit_queue*);

			https_connection& getConnection(https_workload_type workloadType);

			rate_limit_queue& getRateLimitQueue();

		  protected:
			unordered_map<https_workload_type, unique_ptr<https_connection>> httpsConnections{};///< Collection of Https connections.
			rate_limit_queue* rateLimitQueue{};
			std::mutex accessMutex{};
		};

		class DiscordCoreAPI_Dll https_connection_stack_holder {
		  public:
			https_connection_stack_holder(https_connection_manager& connectionManager, https_workload_data&& workload);

			https_connection& getConnection();

			~https_connection_stack_holder();

		  protected:
			rate_limit_queue* rateLimitQueue{};
			https_connection* connection{};
		};

		class DiscordCoreAPI_Dll https_client_core {
		  public:
			https_client_core(jsonifier::string_view botTokenNew);

			DCA_INLINE https_response_data submitWorkloadAndGetResult(https_workload_data&& workloadNew) {
				https_connection connection{};
				rate_limit_data rateLimitData{};
				connection.resetValues(std::move(workloadNew), &rateLimitData);
				auto returnData = httpsRequestInternal(connection);
				if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
					jsonifier::string errorMessage{};
					if (connection.workload.callStack != "") {
						errorMessage += connection.workload.callStack + " ";
					}
					errorMessage += "Https error: " + returnData.responseCode.operator jsonifier::string() + "\nThe request: base url: " + connection.workload.baseUrl + "\n";
					if (!connection.workload.relativePath.empty()) {
						errorMessage += "Relative Url: " + connection.workload.relativePath + "\n";
					}
					if (!connection.workload.content.empty()) {
						errorMessage += "Content: " + connection.workload.content + "\n";
					}
					if (!returnData.responseData.empty()) {
						errorMessage += "The Response: " + static_cast<jsonifier::string>(returnData.responseData);
					}
					https_error theError{ errorMessage };
					theError.errorCode = returnData.responseCode;
				}
				return returnData;
			}

		  protected:
			jsonifier::string botToken{};

			https_response_data httpsRequestInternal(https_connection& connection);

			https_response_data recoverFromError(https_connection& connection);

			https_response_data getResponse(https_connection& connection);
		};

		/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

		/// @class https_client
		/// @brief For sending Https requests.
		class DiscordCoreAPI_Dll https_client : public https_client_core {
		  public:
			https_client(jsonifier::string_view botTokenNew);

			template<typename value_type, typename string_type> void getParseErrors(jsonifier::jsonifier_core<false>& parser, value_type& value, string_type& stringNew) {
				parser.parseJson(value, parser.minifyJson(parser.prettifyJson(stringNew)));
				if (auto result = parser.getErrors(); result.size() > 0) {
					for (auto& valueNew: result) {
						message_printer::printError<print_message_type::websocket>(valueNew.reportError());
					}
				}
			}

			template<typename workload_type, typename... args> void submitWorkloadAndGetResult(workload_type&& workload, args&... argsNew) {
				https_connection_stack_holder stackHolder{ connectionManager, std::move(workload) };
				https_response_data returnData = httpsRequest(stackHolder.getConnection());
				if (static_cast<uint32_t>(returnData.responseCode) != 200 && static_cast<uint32_t>(returnData.responseCode) != 204 &&
					static_cast<uint32_t>(returnData.responseCode) != 201) {
					jsonifier::string errorMessage{};
					if (stackHolder.getConnection().workload.callStack != "") {
						errorMessage += stackHolder.getConnection().workload.callStack + " ";
					}
					errorMessage +=
						"Https error: " + returnData.responseCode.operator jsonifier::string() + "\nThe request: base url: " + stackHolder.getConnection().workload.baseUrl + "\n";
					if (!stackHolder.getConnection().workload.relativePath.empty()) {
						errorMessage += "Relative Url: " + stackHolder.getConnection().workload.relativePath + "\n";
					}
					if (!stackHolder.getConnection().workload.content.empty()) {
						errorMessage += "Content: " + stackHolder.getConnection().workload.content + "\n";
					}
					if (!returnData.responseData.empty()) {
						errorMessage += "The Response: " + static_cast<jsonifier::string>(returnData.responseData);
					}
					https_error theError{ errorMessage };
					theError.errorCode = returnData.responseCode;
				}

				if constexpr ((( !std::is_void_v<args> ) || ...)) {
					if (returnData.responseData.size() > 0) {
						(getParseErrors(parser, argsNew, returnData.responseData), ...);
					}
				}
			}

		  protected:
			https_connection_manager connectionManager{};
			rate_limit_queue rateLimitQueue{};

			https_response_data executeByRateLimitData(https_connection& connection);

			https_response_data httpsRequest(https_connection& connection);
		};

		/**@}*/

	}// namespace discord_core_internal
}
