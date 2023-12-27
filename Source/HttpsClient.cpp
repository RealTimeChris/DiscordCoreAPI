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
/// HttpsClient.cpp - Source file for the https class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file HttpsClient.cpp

#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		DCA_INLINE void rate_limit_queue::initialize() {
			for (int64_t enumOne = static_cast<int64_t>(https_workload_type::Unset); enumOne != static_cast<int64_t>(https_workload_type::Last); enumOne++) {
				auto tempBucket = jsonifier::toString(std::chrono::duration_cast<nanoseconds>(sys_clock::now().time_since_epoch()).count());
				buckets.emplace(static_cast<https_workload_type>(enumOne), tempBucket);
				rateLimits.emplace(tempBucket, makeUnique<rate_limit_data>())
					.getRawPtr()
					->second->sampledTimeInMs.store(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()));
				std::this_thread::sleep_for(1ms);
			}
		}

		DCA_INLINE rate_limit_data* rate_limit_queue::getEndpointAccess(https_workload_type workloadType) {
			stop_watch<milliseconds> stopWatch{ milliseconds{ 25000 } };
			stopWatch.reset();
			auto targetTime =
				std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(rateLimits[buckets[workloadType]]->sampledTimeInMs.load(std::memory_order_acquire)) +
				std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(rateLimits[buckets[workloadType]]->sRemain.load(std::memory_order_acquire));
			if (rateLimits[buckets[workloadType]]->getsRemaining.load(std::memory_order_acquire) <= 0) {
				auto newNow = std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(sys_clock::now().time_since_epoch());
				while ((newNow - targetTime).count() <= 0) {
					if (stopWatch.hasTimeElapsed()) {
						return nullptr;
					}
					newNow = std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(sys_clock::now().time_since_epoch());
					std::this_thread::sleep_for(1us);
				}
			}
			stopWatch.reset();
			while (!rateLimits[buckets[workloadType]]->accessMutex.try_lock()) {
				std::this_thread::sleep_for(1us);
				if (stopWatch.hasTimeElapsed()) {
					return nullptr;
				}
			}
			return rateLimits.at(buckets.at(workloadType)).get();
		}

		DCA_INLINE void rate_limit_queue::releaseEndPointAccess(https_workload_type type) {
			rateLimits.at(buckets.at(type))->accessMutex.unlock();
		}

		jsonifier::vector<jsonifier::string_view> tokenize(jsonifier::string_view in, const char* sep = "\r\n") {
			jsonifier::vector<jsonifier::string_view> result{};
			jsonifier::string_view::size_type b = 0;
			jsonifier::string_view::size_type e = 0;
			while ((b = in.findFirstNotOf(sep, e)) != jsonifier::string_view::npos) {
				e = in.findFirstOf(sep, b);
				if (e == jsonifier::string_view::npos) {
					break;
				}
				result.emplace_back(in.substr(b, e - b));
			}
			return result;
		}

		uint64_t parseCode(jsonifier::string_view string) {
			uint64_t start = string.find(' ');
			if (start == jsonifier::string_view::npos) {
				return 0;
			}

			while (std::isspace(string[start])) {
				start++;
			}

			uint64_t end = start;
			while (std::isdigit(string[end])) {
				end++;
			}
			jsonifier::string_view codeStr = string.substr(start, end - start);
			uint64_t code				   = jsonifier::strToUint64(codeStr.data());
			return code;
		}

		https_connection::https_connection(const jsonifier::string& baseUrlNew, const uint16_t portNew) : tcp_connection<https_connection>{ baseUrlNew, portNew } {
		}

		void https_connection::handleBuffer() {
			stop_watch<milliseconds> stopWatch{ 9500 };
			jsonifier::string_view_base<uint8_t> stringNew{};
			stopWatch.reset();
			do {
				stringNew = getInputBuffer();
				inputBufferReal += stringNew;
				switch (data.currentState) {
					case https_state::Collecting_Headers: {
						if (!parseHeaders()) {
							return;
						}
						break;
					}
					case https_state::Collecting_Contents: {
						if (!parseContents()) {
							return;
						}
						break;
					}
					case https_state::Collecting_Chunked_Contents: {
						if (!parseChunk()) {
							return;
						}
						break;
					}
					case https_state::complete: {
						inputBufferReal.clear();
						return;
					}
				}
			} while (stringNew.size() > 0 && !stopWatch.hasTimeElapsed());
			return;
		}

		bool https_connection::areWeConnected() {
			return tcp_connection::areWeStillConnected();
		}

		void https_connection::disconnect() {
			tcp_connection::disconnect();
			tcp_connection::reset();
		}

		void https_connection::resetValues(https_workload_data&& workloadDataNew, rate_limit_data* rateLimitDataNew) {
			currentRateLimitData = rateLimitDataNew;
			if (currentBaseUrl != workloadDataNew.baseUrl) {
				tcp_connection::reset();
				currentBaseUrl = workloadDataNew.baseUrl;
			}
			workload = std::move(workloadDataNew);
			if (workload.baseUrl == "") {
				workload.baseUrl = "https://discord.com/api/v10";
			}
			inputBufferReal.clear();
			data = https_response_data{};
		}

		void https_rnr_builder::updateRateLimitData(rate_limit_data& rateLimitData) {
			auto connection{ static_cast<https_connection*>(this) };
			if (connection->data.responseHeaders.contains("x-ratelimit-bucket")) {
				rateLimitData.bucket = connection->data.responseHeaders.at("x-ratelimit-bucket");
			}
			if (connection->data.responseHeaders.contains("x-ratelimit-reset-after")) {
				rateLimitData.sRemain.store(seconds{ static_cast<int64_t>(ceil(jsonifier::strToDouble(connection->data.responseHeaders.at("x-ratelimit-reset-after").data()))) },
					std::memory_order_release);
			}
			if (connection->data.responseHeaders.contains("x-ratelimit-remaining")) {
				rateLimitData.getsRemaining.store(static_cast<int64_t>(jsonifier::strToInt64(connection->data.responseHeaders.at("x-ratelimit-remaining").data())),
					std::memory_order_release);
			}
			if (rateLimitData.getsRemaining.load(std::memory_order_acquire) <= 1 || rateLimitData.areWeASpecialBucket.load(std::memory_order_acquire)) {
				rateLimitData.doWeWait.store(true, std::memory_order_release);
			}
		}

		https_response_data https_rnr_builder::finalizeReturnValues(rate_limit_data& rateLimitData) {
			auto connection{ static_cast<https_connection*>(this) };
			if (connection->data.responseData.size() >= connection->data.contentLength && connection->data.contentLength > 0) {
				connection->data.responseData = connection->data.responseData.substr(0, connection->data.contentLength);
			} else {
				auto pos1 = connection->data.responseData.findFirstOf('{');
				auto pos2 = connection->data.responseData.findLastOf('}');
				auto pos3 = connection->data.responseData.findFirstOf('[');
				auto pos4 = connection->data.responseData.findLastOf(']');
				if (pos1 != jsonifier::string_view::npos && pos2 != jsonifier::string_view::npos && pos1 < pos3) {
					connection->data.responseData = connection->data.responseData.substr(pos1, pos2 + 1);
				} else if (pos3 != jsonifier::string_view::npos && pos4 != jsonifier::string_view::npos) {
					connection->data.responseData = connection->data.responseData.substr(pos3, pos4 + 1);
				}
			}
			updateRateLimitData(rateLimitData);
			if (connection->data.responseCode != 204 && connection->data.responseCode != 200 && connection->data.responseCode != 201) {
				throw dca_exception{ "Sorry, but that https request threw the following error: " + connection->data.responseCode.operator jsonifier::string() +
					connection->data.responseData };
			}
			return std::move(connection->data);
		}

		jsonifier::string https_rnr_builder::buildRequest(const https_workload_data& workload) {
			jsonifier::string baseUrlNew{};
			if (workload.baseUrl.find(".com") != jsonifier::string_view::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + jsonifier::string_view("https://").size(),
					workload.baseUrl.find(".com") + jsonifier::string_view(".com").size() - jsonifier::string_view("https://").size());
			} else if (workload.baseUrl.find(".org") != jsonifier::string_view::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + jsonifier::string_view("https://").size(),
					workload.baseUrl.find(".org") + jsonifier::string_view(".org").size() - jsonifier::string_view("https://").size());
			}
			jsonifier::string returnString{};
			if (workload.workloadClass == https_workload_class::Get || workload.workloadClass == https_workload_class::Delete) {
				if (workload.workloadClass == https_workload_class::Get) {
					returnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == https_workload_class::Delete) {
					returnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value]: workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "Pragma: no-cache\r\n";
				returnString += "Connection: keep-alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n\r\n";
			} else {
				if (workload.workloadClass == https_workload_class::Patch) {
					returnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == https_workload_class::Post) {
					returnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == https_workload_class::Put) {
					returnString = "PUT " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value]: workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "Pragma: no-cache\r\n";
				returnString += "Connection: keep-alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n";
				returnString += "Content-Length: " + jsonifier::toString(workload.content.size()) + "\r\n\r\n";
				returnString += workload.content + "\r\n\r\n";
			}
			return returnString;
		}

		bool https_rnr_builder::parseHeaders() {
			auto connection{ static_cast<https_connection*>(this) };
			jsonifier::string& stringViewNew = connection->inputBufferReal;
			if (stringViewNew.find("\r\n\r\n") != jsonifier::string_view::npos) {
				auto headers = tokenize(stringViewNew);
				if (headers.size() && (headers.at(0).find("HTTP/1") != jsonifier::string_view::npos)) {
					uint64_t parseCodeNew{};
					try {
						parseCodeNew = parseCode(headers.at(0));
					} catch (const std::invalid_argument& error) {
						message_printer::printError<print_message_type::https>(error.what());
						connection->data.currentState = https_state::complete;
					}
					headers.erase(headers.begin());
					if (headers.size() >= 3 && parseCodeNew) {
						for (uint64_t x = 0; x < headers.size(); ++x) {
							jsonifier::string_view::size_type sep = headers.at(x).find(": ");
							if (sep != jsonifier::string_view::npos) {
								jsonifier::string key		 = static_cast<jsonifier::string>(headers.at(x).substr(0, sep));
								jsonifier::string_view value = headers.at(x).substr(sep + 2, headers.at(x).size());
								for (auto& valueNew: key) {
									valueNew = static_cast<char>(std::tolower(static_cast<int32_t>(valueNew)));
								}
								connection->data.responseHeaders.emplace(key, value);
							}
						}
						connection->data.responseCode = parseCodeNew;
						if (connection->data.responseCode == 302) {
							connection->workload.baseUrl = connection->data.responseHeaders.at("location");
							connection->disconnect();
							return false;
						}
						if (connection->data.responseCode == 204) {
							connection->data.currentState = https_state::complete;
						} else if (connection->data.responseHeaders.contains("content-length")) {
							connection->data.contentLength = jsonifier::strToUint64(connection->data.responseHeaders.at("content-length").data());
							connection->data.currentState  = https_state::Collecting_Contents;
						} else {
							connection->data.isItChunked   = true;
							connection->data.contentLength = std::numeric_limits<uint32_t>::max();
							connection->data.currentState  = https_state::Collecting_Chunked_Contents;
						}
						connection->inputBufferReal.erase(connection->inputBufferReal.begin() + static_cast<int64_t>(stringViewNew.find("\r\n\r\n")) + 4);
					}
				}
				return true;
			}
			return false;
		}

		bool https_rnr_builder::parseChunk() {
			auto connection{ static_cast<https_connection*>(this) };
			jsonifier::string_view stringViewNew01{ connection->inputBufferReal };
			if (auto finalPosition = stringViewNew01.find("\r\n0\r\n\r\n"); finalPosition != jsonifier::string_view::npos) {
				uint64_t pos{ 0 };
				while (pos < stringViewNew01.size() || connection->data.responseData.size() < connection->data.contentLength) {
					uint64_t lineEnd = stringViewNew01.find("\r\n", pos);
					if (lineEnd == jsonifier::string_view::npos) {
						break;
					}

					jsonifier::string_view sizeLine{ stringViewNew01.data() + pos, lineEnd - pos };
					uint64_t chunkSize = jsonifier::strToUint64<16>(static_cast<jsonifier::string>(sizeLine));
					connection->data.contentLength += chunkSize;

					if (chunkSize == 0) {
						break;
					}

					pos = lineEnd + 2;

					jsonifier::string_view newString{ stringViewNew01.data() + pos, chunkSize };
					connection->data.responseData += newString;
					pos += chunkSize + 2;
				}
				connection->data.currentState = https_state::complete;
				return true;
			}
			return false;
		}

		bool https_rnr_builder::parseContents() {
			auto connection{ static_cast<https_connection*>(this) };
			if (connection->inputBufferReal.size() >= connection->data.contentLength || !connection->data.contentLength) {
				connection->data.responseData += jsonifier::string_view{ connection->inputBufferReal.data(), connection->data.contentLength };
				connection->data.currentState = https_state::complete;
				return true;
			} else {
				return false;
			}
		}

		https_connection_manager::https_connection_manager(rate_limit_queue* rateLimitDataQueueNew) {
			rateLimitQueue = rateLimitDataQueueNew;
		}

		rate_limit_queue& https_connection_manager::getRateLimitQueue() {
			return *rateLimitQueue;
		}

		https_connection& https_connection_manager::getConnection(https_workload_type workloadType) {
			std::unique_lock lock{ accessMutex };
			if (!httpsConnections.contains(workloadType)) {
				httpsConnections.emplace(workloadType, makeUnique<https_connection>());
			}
			httpsConnections.at(workloadType)->currentReconnectTries = 0;
			return *httpsConnections.at(workloadType).get();
		}

		https_connection_stack_holder::https_connection_stack_holder(https_connection_manager& connectionManager, https_workload_data&& workload) {
			connection		   = &connectionManager.getConnection(workload.getWorkloadType());
			rateLimitQueue = &connectionManager.getRateLimitQueue();
			auto rateLimitData = connectionManager.getRateLimitQueue().getEndpointAccess(workload.getWorkloadType());
			if (!rateLimitData) {
				throw dca_exception{ "Failed to gain endpoint access." };
			}
			connection->resetValues(std::move(workload), rateLimitData);
			if (!connection->areWeConnected()) {
				*static_cast<tcp_connection<https_connection>*>(connection) = https_connection{ connection->workload.baseUrl, static_cast<uint16_t>(443) };
			}
		}

		https_connection_stack_holder::~https_connection_stack_holder() {
			rateLimitQueue->releaseEndPointAccess(connection->workload.getWorkloadType());
		}

		https_connection& https_connection_stack_holder::getConnection() {
			return *connection;
		}

		https_client::https_client(jsonifier::string_view botTokenNew) : https_client_core(botTokenNew), connectionManager(&rateLimitQueue) {
			rateLimitQueue.initialize();
		}

		https_response_data https_client::httpsRequest(https_connection& connection) {
			https_response_data resultData = executeByRateLimitData(connection);
			return resultData;
		}

		https_response_data https_client::executeByRateLimitData(https_connection& connection) {
			https_response_data returnData{};
			milliseconds timeRemaining{};
			milliseconds currentTime = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch());
			if (connection.workload.workloadType == https_workload_type::Delete_Message_Old) {
				connection.currentRateLimitData->sRemain.store(seconds{ 4 }, std::memory_order_release);
			}
			if (connection.workload.workloadType == https_workload_type::Post_Message || connection.workload.workloadType == https_workload_type::Patch_Message) {
				connection.currentRateLimitData->areWeASpecialBucket.store(true, std::memory_order_release);
			}
			if (connection.currentRateLimitData->areWeASpecialBucket.load(std::memory_order_acquire)) {
				connection.currentRateLimitData->sRemain.store(seconds{ static_cast<int64_t>(ceil(4.0f / 4.0f)) }, std::memory_order_release);
				milliseconds targetTime{ connection.currentRateLimitData->sampledTimeInMs.load(std::memory_order_acquire) +
					std::chrono::duration_cast<std::chrono::milliseconds>(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire)) };
				timeRemaining = targetTime - currentTime;
			} else if (connection.currentRateLimitData->doWeWait.load(std::memory_order_acquire)) {
				milliseconds targetTime{ connection.currentRateLimitData->sampledTimeInMs.load(std::memory_order_acquire) +
					std::chrono::duration_cast<std::chrono::milliseconds>(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire)) };
				timeRemaining = targetTime - currentTime;
				connection.currentRateLimitData->doWeWait.store(false, std::memory_order_release);
			}
			if (timeRemaining.count() > 0) {
				message_printer::printSuccess<print_message_type::https>("we're waiting on rate-limit: " + jsonifier::toString(timeRemaining.count()));
				milliseconds targetTime{ currentTime + timeRemaining };
				while (targetTime > currentTime && targetTime.count() > 0 && currentTime.count() > 0 && timeRemaining.count() > 0) {
					currentTime	  = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch());
					timeRemaining = targetTime - currentTime;
					if (timeRemaining.count() <= 20) {
						continue;
					} else {
						std::this_thread::sleep_for(milliseconds{ static_cast<int64_t>(static_cast<double>(timeRemaining.count()) * 80.0f / 100.0f) });
					}
				}
			}
			returnData = https_client::httpsRequestInternal(connection);
			connection.currentRateLimitData->sampledTimeInMs.store(std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(sys_clock::now().time_since_epoch()),
				std::memory_order_release);

			if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
				message_printer::printSuccess<print_message_type::https>(
					connection.workload.callStack + " success: " + static_cast<jsonifier::string>(returnData.responseCode) + ": " + returnData.responseData);
			} else if (returnData.responseCode == 429) {
				if (connection.data.responseHeaders.contains("x-ratelimit-retry-after")) {
					connection.currentRateLimitData->sRemain.store(seconds{ jsonifier::strToInt64(connection.data.responseHeaders.at("x-ratelimit-retry-after").data()) / 1000LL },
						std::memory_order_release);
				}
				connection.currentRateLimitData->doWeWait.store(true, std::memory_order_release);
				connection.currentRateLimitData->sampledTimeInMs.store(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()), std::memory_order_release);
				message_printer::printError<print_message_type::https>(connection.workload.callStack + "::httpsRequest(), we've hit rate limit! time remaining: " +
					jsonifier::toString(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire).count()));
				connection.resetValues(std::move(connection.workload), connection.currentRateLimitData);
				returnData = executeByRateLimitData(connection);
			}
			return returnData;
		}

		https_client_core::https_client_core(jsonifier::string_view botTokenNew) {
			botToken = botTokenNew;
		}

		https_response_data https_client_core::httpsRequestInternal(https_connection& connection) {
			if (connection.workload.baseUrl == "https://discord.com/api/v10") {
				connection.workload.headersToInsert.emplace("Authorization", "Bot " + botToken);
				connection.workload.headersToInsert.emplace("User-Agent", "DiscordCoreAPI (https://discordcoreapi.com/1.0)");
				if (connection.workload.payloadType == payload_type::Application_Json) {
					connection.workload.headersToInsert.emplace("Content-Type", "application/json");
				} else if (connection.workload.payloadType == payload_type::Multipart_Form) {
					connection.workload.headersToInsert.emplace("Content-Type", "multipart/form-data; boundary=boundary25");
				}
			}
			if (connection.currentReconnectTries >= connection.maxReconnectTries) {
				connection.disconnect();
				return https_response_data{};
			}
			if (!connection.areWeConnected()) {
				connection.currentBaseUrl									 = connection.workload.baseUrl;
				*static_cast<tcp_connection<https_connection>*>(&connection) = https_connection{ connection.workload.baseUrl, static_cast<uint16_t>(443) };
				if (connection.currentStatus != connection_status::NO_Error || !connection.areWeConnected()) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection);
				}
			}
			auto request = connection.buildRequest(connection.workload);
			if (connection.areWeConnected()) {
				connection.writeData(static_cast<jsonifier::string_view>(request), true);
				if (connection.currentStatus != connection_status::NO_Error || !connection.areWeConnected()) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection);
				}
				auto result = getResponse(connection);
				if (static_cast<int64_t>(result.responseCode) == -1 || !connection.areWeConnected()) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection);
				} else {
					return result;
				}
			} else {
				++connection.currentReconnectTries;
				connection.disconnect();
				return httpsRequestInternal(connection);
			}
		}

		https_response_data https_client_core::recoverFromError(https_connection& connection) {
			if (connection.currentReconnectTries >= connection.maxReconnectTries) {
				connection.disconnect();
				return connection.finalizeReturnValues(*connection.currentRateLimitData);
			}
			++connection.currentReconnectTries;
			connection.disconnect();
			std::this_thread::sleep_for(150ms);
			return httpsRequestInternal(connection);
		}

		https_response_data https_client_core::getResponse(https_connection& connection) {
			while (connection.data.currentState != https_state::complete) {
				if (connection.areWeConnected()) {
					auto newState = connection.processIO(10);
					switch (newState) {
						case connection_status::NO_Error: {
							continue;
						}
						case connection_status::CONNECTION_Error:
							[[fallthrough]];
						case connection_status::POLLERR_Error:
							[[fallthrough]];
						case connection_status::POLLHUP_Error:
							[[fallthrough]];
						case connection_status::POLLNVAL_Error:
							[[fallthrough]];
						case connection_status::READ_Error:
							[[fallthrough]];
						case connection_status::WRITE_Error:
							[[fallthrough]];
						case connection_status::SOCKET_Error:
							[[fallthrough]];
						default: {
							return recoverFromError(connection);
						}
					}
				} else {
					return recoverFromError(connection);
				}
			}
			return connection.finalizeReturnValues(*connection.currentRateLimitData);
		}
	}
}
