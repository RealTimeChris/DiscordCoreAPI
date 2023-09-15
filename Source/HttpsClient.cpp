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
/// HttpsClient.cpp - Source file for the Https class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file HttpsClient.cpp

#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		HttpsTCPConnection::HttpsTCPConnection(const std::string& baseUrlNew, const uint16_t portNew, HttpsConnection* ptrNew)
			: TCPConnection<HttpsTCPConnection>{ baseUrlNew, portNew } {
			ptr = ptrNew;
		}

		jsonifier::vector<std::string> tokenize(std::string const& in, const char* sep = "\r\n") {
			std::string::size_type b = 0;
			jsonifier::vector<std::string> result{};

			while ((b = in.find_first_not_of(sep, b)) != std::string::npos) {
				auto e = in.find(sep, b);
				result.emplace_back(static_cast<std::string>(in.substr(b, e - b)));
				b = e;
			}
			return result;
		}

		void HttpsTCPConnection::handleBuffer() {
			std::basic_string_view<uint8_t> stringView{};
			do {
				stringView = getInputBuffer();
				if (stringView.size() > 0) {
					auto oldSize = ptr->inputBufferReal.size();
					ptr->inputBufferReal.resize(oldSize + stringView.size());
					std::memcpy(ptr->inputBufferReal.data() + oldSize, stringView.data(), stringView.size());
				}
				switch (ptr->data.currentState) {
					case HttpsState::Collecting_Headers: {
						if (!ptr->parseHeaders()) {
							return;
						}
						break;
					}
					case HttpsState::Collecting_Contents: {
						if (!ptr->parseContents()) {
							return;
						}
						break;
					}
					case HttpsState::Collecting_Chunked_Contents: {
						if (!ptr->parseChunk()) {
							return;
						}
						break;
					}
					case HttpsState::Complete: {
						ptr->inputBufferReal.clear();
						return;
					}
				}
			} while (stringView.size() > 0);
			return;
		}

		HttpsClientCore::HttpsClientCore(const std::string& botTokenNew) {
			botToken = botTokenNew;
		}

		void HttpsRnRBuilder::updateRateLimitData(RateLimitData& rateLimitData) {
			auto connection{ static_cast<HttpsConnection*>(this) };
			if (connection->data.responseHeaders.contains("x-ratelimit-bucket")) {
				rateLimitData.bucket = connection->data.responseHeaders.at("x-ratelimit-bucket");
			}
			if (connection->data.responseHeaders.contains("x-ratelimit-reset-after")) {
				rateLimitData.sRemain.store(Seconds{ static_cast<int64_t>(ceil(stod(connection->data.responseHeaders.at("x-ratelimit-reset-after")))) }, std::memory_order_release);
			}
			if (connection->data.responseHeaders.contains("x-ratelimit-remaining")) {
				rateLimitData.getsRemaining.store(static_cast<int64_t>(stoi(connection->data.responseHeaders.at("x-ratelimit-remaining"))), std::memory_order_release);
			}
			if (rateLimitData.getsRemaining.load(std::memory_order_acquire) <= 1 || rateLimitData.areWeASpecialBucket.load(std::memory_order_acquire)) {
				rateLimitData.doWeWait.store(true, std::memory_order_release);
			}
		}

		HttpsResponseData HttpsRnRBuilder::finalizeReturnValues(RateLimitData& rateLimitData) {
			auto connection{ static_cast<HttpsConnection*>(this) };
			if (connection->data.responseData.size() >= connection->data.contentLength && connection->data.contentLength > 0) {
				connection->data.responseData = connection->data.responseData.substr(0, connection->data.contentLength);
			} else {
				auto pos1 = connection->data.responseData.find_first_of('{');
				auto pos2 = connection->data.responseData.find_last_of('}');
				auto pos3 = connection->data.responseData.find_first_of('[');
				auto pos4 = connection->data.responseData.find_last_of(']');
				if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 < pos3) {
					connection->data.responseData = connection->data.responseData.substr(pos1, pos2 + 1);
				} else if (pos3 != std::string::npos && pos4 != std::string::npos) {
					connection->data.responseData = connection->data.responseData.substr(pos3, pos4 + 1);
				}
			}
			updateRateLimitData(rateLimitData);
			return std::move(connection->data);
		}

		std::string HttpsRnRBuilder::buildRequest(const HttpsWorkloadData& workload) {
			std::string baseUrlNew{};
			if (workload.baseUrl.find(".com") != std::string::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(),
					workload.baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
			} else if (workload.baseUrl.find(".org") != std::string::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(),
					workload.baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
			}
			std::string returnString{};
			if (workload.workloadClass == HttpsWorkloadClass::Get || workload.workloadClass == HttpsWorkloadClass::Delete) {
				if (workload.workloadClass == HttpsWorkloadClass::Get) {
					returnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == HttpsWorkloadClass::Delete) {
					returnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value]: workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "pragma: no-cache\r\n";
				returnString += "Connection: keep-alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n\r\n";
			} else {
				if (workload.workloadClass == HttpsWorkloadClass::Patch) {
					returnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == HttpsWorkloadClass::Post) {
					returnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == HttpsWorkloadClass::Put) {
					returnString = "PUT " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value]: workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "pragma: no-cache\r\n";
				returnString += "Connection: keep-alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n";
				returnString += "Content-Length: " + std::to_string(workload.content.size()) + "\r\n\r\n";
				returnString += workload.content;
			}
			return returnString;
		}

		bool HttpsRnRBuilder::parseHeaders() {
			auto connection{ static_cast<HttpsConnection*>(this) };
			auto stringViewNew = static_cast<std::string>(connection->inputBufferReal);
			if (stringViewNew.find("\r\n\r\n") != std::string::npos) {
				auto headers		   = tokenize(stringViewNew);
				std::string statusLine = headers.at(0);
				headers.erase(headers.begin());
				if (headers.size()) {
					jsonifier::vector<std::string> requestStatus = tokenize(statusLine, " ");
					if (requestStatus.size() >= 3 && (requestStatus.at(0) == "HTTP/1.1" || requestStatus.at(0) == "HTTP/1.0") && atoi(requestStatus.at(1).c_str())) {
						for (uint64_t x = 0; x < headers.size(); ++x) {
							std::string::size_type sep = headers.at(x).find(": ");
							if (sep != std::string::npos) {
								std::string key	  = headers.at(x).substr(0, sep);
								std::string value = headers.at(x).substr(sep + 2, headers.at(x).length());
								for (auto& valueNew: key) {
									valueNew = static_cast<char>(std::tolower(static_cast<int32_t>(valueNew)));
								}
								connection->data.responseHeaders.emplace(key, value);
							}
						}
						if (connection->data.responseHeaders.contains("content-length")) {
							connection->data.contentLength = stoull(connection->data.responseHeaders.at("content-length"));
						} else {
							connection->data.contentLength = std::numeric_limits<uint32_t>::max();
							connection->data.currentState  = HttpsState::Collecting_Chunked_Contents;
						}
						connection->data.isItChunked = false;
						if (connection->data.responseHeaders.contains("transfer-encoding")) {
							if (connection->data.responseHeaders.at("transfer-encoding").find("chunked") != std::string::npos) {
								connection->data.isItChunked   = true;
								connection->data.contentLength = 0;
								connection->data.currentState  = HttpsState::Collecting_Chunked_Contents;
							}
						}
						connection->data.responseCode = static_cast<uint32_t>(atoi(requestStatus.at(1).c_str()));
						if (connection->data.responseCode == 302) {
							connection->workload.baseUrl = connection->data.responseHeaders.at("location");
							connection->disconnect();
							return false;
						}
						if (connection->data.responseCode != 200 && connection->data.responseCode != 201 && connection->data.responseCode != static_cast<int32_t>(-1)) {
							connection->inputBufferReal.erase(stringViewNew.find("\r\n\r\n") + 4);
							connection->data.currentState = HttpsState::Complete;
							return true;
						} else if (!connection->data.isItChunked) {
							connection->data.currentState = HttpsState::Collecting_Contents;
							connection->inputBufferReal.erase(stringViewNew.find("\r\n\r\n") + 4);
							return true;
						} else {
							connection->inputBufferReal.erase(stringViewNew.find("\r\n\r\n") + 4);
							return true;
						}
					}
				}
				return true;
			}
			return false;
		}

		bool HttpsRnRBuilder::parseChunk() {
			auto connection{ static_cast<HttpsConnection*>(this) };
			auto stringViewNew01{ static_cast<std::string>(connection->inputBufferReal) };
			if (auto finalPosition = stringViewNew01.find("\r\n0\r\n\r\n"); finalPosition != std::string::npos) {
				std::string_view responseData{ stringViewNew01.data(), stringViewNew01.size() };

				uint64_t pos{ 0 };
				while (pos < responseData.size()) {
					uint64_t lineEnd = responseData.find("\r\n", pos);
					if (lineEnd == std::string::npos) {
						break;
					}

					std::string sizeLine{ responseData.data() + pos, lineEnd - pos };
					uint64_t chunkSize = std::stoul(sizeLine, nullptr, 16);

					if (chunkSize == 0) {
						break;
					}

					pos = lineEnd + 2;

					std::string newString{};
					newString.append(responseData.data() + pos, chunkSize);
					connection->data.responseData += newString;
					pos += chunkSize + 2;
				}
				connection->data.currentState = HttpsState::Complete;
				return true;
			}
			return false;
		}

		bool HttpsRnRBuilder::parseContents() {
			auto connection{ static_cast<HttpsConnection*>(this) };
			if (connection->inputBufferReal.size() >= connection->data.contentLength || !connection->data.contentLength) {
				connection->data.responseData += connection->inputBufferReal.stringView(0, connection->data.contentLength);
				connection->data.currentState = HttpsState::Complete;
				return true;
			} else {
				return false;
			}
		}

		bool HttpsConnection::areWeConnected() {
			return tcpConnection.areWeStillConnected();
		}

		void HttpsConnection::disconnect() {
			tcpConnection.disconnect();
			tcpConnection.reset();
		}

		void HttpsConnection::resetValues(HttpsWorkloadData&& workloadDataNew, RateLimitData*rateLimitDataNew) {
			currentRateLimitData = rateLimitDataNew;
			if (currentBaseUrl != workloadDataNew.baseUrl) {
				tcpConnection.reset();
				currentBaseUrl = workloadDataNew.baseUrl;
			}
			workload = std::move(workloadDataNew);
			if (workload.baseUrl == "") {
				workload.baseUrl = "https://discord.com/api/v10";
			}
			currentReconnectTries = 0;
			inputBufferReal.clear();
			data = HttpsResponseData{};
		}

		HttpsConnectionManager::HttpsConnectionManager(RateLimitQueue*rateLimitDataQueueNew) {
			rateLimitQueue = rateLimitDataQueueNew;
		}

		RateLimitQueue& HttpsConnectionManager::getRateLimitQueue() {
			return *rateLimitQueue;
		}

		HttpsConnection& HttpsConnectionManager::getConnection(HttpsWorkloadType workloadType) {
			std::unique_lock lock{ accessMutex };
			if (!httpsConnections.contains(workloadType)) {
				httpsConnections.emplace(workloadType, makeUnique<HttpsConnection>());
			}
			return *httpsConnections.at(workloadType).get();
		}

		HttpsConnectionStackHolder::HttpsConnectionStackHolder(HttpsConnectionManager& connectionManager, HttpsWorkloadData&& workload) {
			connection		   = &connectionManager.getConnection(workload.getWorkloadType());
			auto rateLimitData = connectionManager.getRateLimitQueue().getEndpointAccess(workload.getWorkloadType());
			while (!rateLimitData->accessMutex.try_lock()) {
				std::this_thread::sleep_for(1us);
			}
			connection->resetValues(std::move(workload), rateLimitData);
			if (!connection->areWeConnected()) {
				connection->tcpConnection = HttpsTCPConnection{ connection->workload.baseUrl, static_cast<uint16_t>(443), connection };
			}
		}

		HttpsConnectionStackHolder::~HttpsConnectionStackHolder() {
			HttpsWorkloadData::workloadIdsInternal.at(connection->workload.getWorkloadType())->fetch_add(1, std::memory_order_release);
			connection->currentRateLimitData->accessMutex.unlock();
		}

		HttpsConnection& HttpsConnectionStackHolder::getConnection() {
			return *connection;
		}

		HttpsClient::HttpsClient(const std::string& botTokenNew) : HttpsClientCore(botTokenNew), connectionManager(&rateLimitQueue) {
			rateLimitQueue.initialize();
		}

		HttpsResponseData HttpsClient::httpsRequest(HttpsConnection& connection) {

			HttpsResponseData resultData = executeByRateLimitData(connection);
			return resultData;
		}

		HttpsResponseData HttpsClientCore::httpsRequestInternal(HttpsConnection& connection) {
			if (connection.workload.baseUrl == "https://discord.com/api/v10") {
				connection.workload.headersToInsert.emplace("Authorization", "Bot " + botToken);
				connection.workload.headersToInsert.emplace("User-Agent", "DiscordBot (https://discordcoreapi.com/ 1.0)");
				if (connection.workload.payloadType == PayloadType::Application_Json) {
					connection.workload.headersToInsert.emplace("Content-Type", "application/json");
				} else if (connection.workload.payloadType == PayloadType::Multipart_Form) {
					connection.workload.headersToInsert.emplace("Content-Type", "multipart/form-data; boundary=boundary25");
				}
			}
			if (connection.currentReconnectTries >= connection.maxReconnectTries) {
				connection.disconnect();
				return HttpsResponseData{};
			}
			if (!connection.areWeConnected()) {
				connection.currentBaseUrl = connection.workload.baseUrl;
				connection.tcpConnection  = HttpsTCPConnection{ connection.workload.baseUrl, static_cast<uint16_t>(443), &connection };
				if (connection.tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection);
				} else {
					connection.currentReconnectTries = 0;
				}
			}
			auto request = connection.buildRequest(connection.workload);
			if (connection.areWeConnected()) {
				connection.tcpConnection.writeData(static_cast<std::string_view>(request), true);
				if (connection.tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection);
				}

				auto result = getResponse(connection);
				if (static_cast<int64_t>(result.responseCode) == -1) {
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

		HttpsResponseData HttpsClient::executeByRateLimitData(HttpsConnection& connection) {
			HttpsResponseData returnData{};
			Milliseconds timeRemaining{};
			Milliseconds currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
			if (connection.workload.workloadType == HttpsWorkloadType::Delete_Message_Old) {
				connection.currentRateLimitData->sRemain.store(Seconds{ 4 }, std::memory_order_release);
			}
			if (connection.workload.workloadType == HttpsWorkloadType::Post_Message || connection.workload.workloadType == HttpsWorkloadType::Patch_Message) {
				connection.currentRateLimitData->areWeASpecialBucket.store(true, std::memory_order_release);
			}
			if (connection.currentRateLimitData->areWeASpecialBucket.load(std::memory_order_acquire)) {
				connection.currentRateLimitData->sRemain.store(Seconds{ static_cast<int64_t>(ceil(4.0f / 4.0f)) }, std::memory_order_release);
				Milliseconds targetTime{ connection.currentRateLimitData->sampledTimeInMs.load(std::memory_order_acquire) + connection.currentRateLimitData->sRemain.load(std::memory_order_acquire) };
				timeRemaining = targetTime - currentTime;
			} else if (connection.currentRateLimitData->doWeWait.load(std::memory_order_acquire)) {
				Milliseconds targetTime{ connection.currentRateLimitData->sampledTimeInMs.load(std::memory_order_acquire) + connection.currentRateLimitData->sRemain.load(std::memory_order_acquire) };
				timeRemaining = targetTime - currentTime;
				connection.currentRateLimitData->doWeWait.store(false, std::memory_order_release);
			}
			if (timeRemaining.count() > 0) {
				MessagePrinter::printSuccess<PrintMessageType::Https>(jsonifier::string{ "We're waiting on rate-limit: " + std::to_string(timeRemaining.count()) });
				Milliseconds targetTime{ currentTime + timeRemaining };
				while (targetTime > currentTime && targetTime.count() > 0 && currentTime.count() > 0 && timeRemaining.count() > 0) {
					currentTime	  = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
					timeRemaining = targetTime - currentTime;
					if (timeRemaining.count() <= 20) {
						continue;
					} else {
						std::this_thread::sleep_for(Milliseconds{ static_cast<int64_t>(static_cast<double>(timeRemaining.count()) * 80.0f / 100.0f) });
					}
				}
			}
						
			returnData = HttpsClient::httpsRequestInternal(connection);
			connection.currentRateLimitData->sampledTimeInMs.store(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()), std::memory_order_release);

			if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
				MessagePrinter::printSuccess<PrintMessageType::Https>(
					jsonifier::string{ connection.workload.callStack + " Success: " + static_cast<std::string>(returnData.responseCode) + ": " + returnData.responseData });
			} else if (returnData.responseCode == 429) {
				if (connection.data.responseHeaders.contains("x-ratelimit-retry-after")) {
					connection.currentRateLimitData->sRemain.store(Seconds{ stoll(connection.data.responseHeaders.at("x-ratelimit-retry-after")) / 1000LL }, std::memory_order_release);
				}
				connection.currentRateLimitData->doWeWait.store(true, std::memory_order_release);
				connection.currentRateLimitData->sampledTimeInMs.store(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()), std::memory_order_release);
				MessagePrinter::printError<PrintMessageType::Https>(jsonifier::string{ connection.workload.callStack + "::httpsRequest(), We've hit rate limit! Time Remaining: " +
					std::to_string(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire).count()) });
				connection.resetValues(std::move(connection.workload), connection.currentRateLimitData);
				returnData = executeByRateLimitData(connection);
			}
			return returnData;
		}

		HttpsResponseData HttpsClientCore::recoverFromError(HttpsConnection& connection) {
			if (connection.currentReconnectTries >= connection.maxReconnectTries) {
				connection.disconnect();
				return connection.finalizeReturnValues(*connection.currentRateLimitData);
			}
			++connection.currentReconnectTries;
			connection.disconnect();
			return httpsRequestInternal(connection);
		}

		HttpsResponseData HttpsClientCore::getResponse(HttpsConnection& connection) {
			StopWatch<Milliseconds> stopWatch{ 10000ms };
			stopWatch.reset();
			while (connection.data.currentState != HttpsState::Complete && !stopWatch.hasTimeElapsed()) {
				if (connection.areWeConnected()) {
					auto newState = connection.tcpConnection.processIO(10);
					switch (newState) {
						case ConnectionStatus::NO_Error: {
							continue;
						}
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
