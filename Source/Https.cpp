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
/// Https.cpp - Source file for the Https class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.cpp

#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/ThreadPool.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		HttpsError::HttpsError(std::string message, std::source_location location) : DCAException(message, location){};

		HttpsTCPConnection::HttpsTCPConnection(const std::string& baseUrlNew, const uint16_t portNew, HttpsConnection* ptrNew)
			: TCPConnection<HttpsTCPConnection>{ baseUrlNew, portNew } {
			ptr = ptrNew;
		};

		std::vector<std::string> tokenize(std::string const& in, const char* sep = "\r\n") {
			std::string::size_type b = 0;
			std::vector<std::string> result;

			while ((b = in.find_first_not_of(sep, b)) != std::string::npos) {
				auto e = in.find(sep, b);
				result.push_back(static_cast<std::string>(in.substr(b, e - b)));
				b = e;
			}
			return result;
		}

		void HttpsTCPConnection::handleBuffer() noexcept {
			std::string stringView{};
			do {
				stringView = getInputBuffer();
				if (stringView.size() > 0) {
					ptr->inputBufferReal.writeData(reinterpret_cast<const char*>(stringView.data()), stringView.size());
				}
				if (ptr->data.responseData.size() >= ptr->data.contentLength && ptr->data.contentLength > 0) {
					ptr->data.currentState = HttpsState::Complete;
					return;
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

		void HttpsTCPConnection::disconnect() noexcept {
			currentStatus = ConnectionStatus::CONNECTION_Error;
			socket = INVALID_SOCKET;
			ssl = nullptr;
		}

		HttpsRnRBuilder::HttpsRnRBuilder() noexcept {};

		HttpsClientCore::HttpsClientCore(const std::string& botTokenNew) noexcept {
			botToken = botTokenNew;
		}

		void HttpsRnRBuilder::updateRateLimitData(RateLimitData& rateLimitData, std::unordered_map<std::string, std::string>& headersNew) noexcept {
			auto& connection{ *static_cast<HttpsConnection*>(this) };
			if (connection.data.responseHeaders.contains("x-ratelimit-bucket")) {
				rateLimitData.bucket = connection.data.responseHeaders["x-ratelimit-bucket"];
			}
			if (connection.data.responseHeaders.contains("x-ratelimit-reset-after")) {
				rateLimitData.msRemain.store(
					Milliseconds{ static_cast<int64_t>(ceil(stod(connection.data.responseHeaders["x-ratelimit-reset-after"])) * 1000.0f) });
			}
			if (connection.data.responseHeaders.contains("x-ratelimit-remaining")) {
				rateLimitData.getsRemaining.store(static_cast<int64_t>(stoi(connection.data.responseHeaders["x-ratelimit-remaining"])));
			}
			if (rateLimitData.getsRemaining.load() <= 1 || rateLimitData.areWeASpecialBucket.load()) {
				rateLimitData.doWeWait.store(true);
			}
		};

		HttpsResponseData HttpsRnRBuilder::finalizeReturnValues(RateLimitData& rateLimitData) noexcept {
			auto& connection{ *static_cast<HttpsConnection*>(this) };
			if (connection.data.responseData.size() >= connection.data.contentLength && connection.data.contentLength > 0) {
				connection.data.responseData = connection.data.responseData.substr(0, connection.data.contentLength);
			} else {
				auto pos1 = connection.data.responseData.find_first_of('{');
				auto pos2 = connection.data.responseData.find_last_of('}');
				auto pos3 = connection.data.responseData.find_first_of('[');
				auto pos4 = connection.data.responseData.find_last_of(']');
				if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 < pos3) {
					connection.data.responseData = connection.data.responseData.substr(pos1, pos2 + 1);
				} else if (pos3 != std::string::npos && pos4 != std::string::npos) {
					connection.data.responseData = connection.data.responseData.substr(pos3, pos4 + 1);
				}
			}
			updateRateLimitData(rateLimitData, connection.data.responseHeaders);
			return connection.data;
		}

		std::string HttpsRnRBuilder::buildRequest(const HttpsWorkloadData& workload) noexcept {
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
				returnString += "Connection: Keep-Alive\r\n";
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
				returnString += "Connection: Keep-Alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n";
				returnString += "Content-Length: " + std::to_string(workload.content.size()) + "\r\n\r\n";
				returnString += workload.content;
			}
			return returnString;
		}

		bool HttpsRnRBuilder::parseHeaders() noexcept {
			auto& connection{ *static_cast<HttpsConnection*>(this) };
			auto stringViewNew = static_cast<std::string>(connection.inputBufferReal);
			if (stringViewNew.find("\r\n\r\n") != std::string::npos) {
				auto headers = tokenize(stringViewNew);
				std::string statusLine = headers[0];
				headers.erase(headers.begin());
				if (headers.size()) {
					headers.erase(headers.begin());
					std::vector<std::string> requestStatus = tokenize(statusLine, " ");
					if (requestStatus.size() >= 3 && (requestStatus[0] == "HTTP/1.1" || requestStatus[0] == "HTTP/1.0") &&
						atoi(requestStatus[1].c_str())) {
						for (auto& hd: headers) {
							std::string::size_type sep = hd.find(": ");
							if (sep != std::string::npos) {
								std::string key = hd.substr(0, sep);
								std::string value = hd.substr(sep + 2, hd.length());
								std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
									return std::tolower(c);
								});
								connection.data.responseHeaders.emplace(key, value);
							}
						}
						if (connection.data.responseHeaders.contains("content-length")) {
							connection.data.contentLength = stoull(connection.data.responseHeaders["content-length"]);
						} else {
							connection.data.contentLength = std::numeric_limits<uint32_t>::max();
						}
						connection.data.isItChunked = false;
						if (connection.data.responseHeaders.contains("transfer-encoding")) {
							if (connection.data.responseHeaders["transfer-encoding"].find("chunked") != std::string::npos) {
								connection.data.isItChunked = true;
								connection.data.contentLength = 0;
								connection.data.currentState = HttpsState::Collecting_Chunked_Contents;
							}
						}
						connection.data.responseCode = atoi(requestStatus[1].c_str());
						if (connection.data.responseCode == 302) {
							connection.workload.baseUrl = connection.data.responseHeaders["location"];
							connection.disconnect();
							return false;
						}
						if (connection.data.responseCode != 200 && connection.data.responseCode != 201) {
							connection.inputBufferReal.erase(stringViewNew.find("\r\n\r\n") + 4);
							connection.data.currentState = HttpsState::Complete;
							return true;
						} else if (!connection.data.isItChunked) {
							connection.data.currentState = HttpsState::Collecting_Contents;
							connection.inputBufferReal.erase(stringViewNew.find("\r\n\r\n") + 4);
							return true;
						} else {
							connection.inputBufferReal.erase(stringViewNew.find("\r\n\r\n") + 4);
							return true;
						}
					}
				}
				return true;
			}
			return false;
		}

		bool HttpsRnRBuilder::parseChunk() noexcept {
			auto& connection{ *static_cast<HttpsConnection*>(this) };
			auto stringViewNew01{ static_cast<std::string>(connection.inputBufferReal) };
			if (auto finalPosition = stringViewNew01.find("\r\n0\r\n\r\n"); finalPosition != std::string::npos) {
				std::string responseData{};

				responseData.assign(stringViewNew01.data(), stringViewNew01.size());

				size_t pos{ 0 };
				while (pos < responseData.size()) {
					size_t lineEnd = responseData.find("\r\n", pos);
					if (lineEnd == std::string::npos) {
						break;
					}

					std::string sizeLine{ responseData.data() + pos, lineEnd - pos };
					size_t chunkSize = std::stoul(sizeLine, nullptr, 16);

					if (chunkSize == 0) {
						break;
					}

					pos = lineEnd + 2;

					std::string newString{};
					newString.append(responseData.data() + pos, chunkSize);
					connection.data.responseData += newString;
					pos += chunkSize + 2;
				}
				connection.data.currentState = HttpsState::Complete;
				return true;
			}
			return false;
		}

		bool HttpsRnRBuilder::parseContents() noexcept {
			auto& connection{ *static_cast<HttpsConnection*>(this) };
			if (connection.inputBufferReal.size() >= connection.data.contentLength || !connection.data.contentLength) {
				connection.data.responseData += connection.inputBufferReal.stringView(0, connection.data.contentLength);
				connection.data.currentState = HttpsState::Complete;
				return true;
			} else {
				return false;
			}
		}

		HttpsConnection::HttpsConnection() noexcept {};

		bool HttpsConnection::areWeConnected() noexcept {
			if (tcpConnection.areWeStillConnected()) {
				return true;
			} else {
				return false;
			}
		}

		void HttpsConnection::disconnect() noexcept {
			tcpConnection.disconnect();
			reset();
		}

		void HttpsConnection::resetValues(HttpsWorkloadData&& workloadDataNew) noexcept {
			if (currentBaseUrl != workloadDataNew.baseUrl) {
				reset();
				currentBaseUrl = workloadDataNew.baseUrl;
			}
			workload = std::move(workloadDataNew);
			currentReconnectTries = 0;
			inputBufferReal.clear();
			data = HttpsResponseData{};
		}

		HttpsConnection& RateLimitManager::getConnection(HttpsWorkloadType workloadType) noexcept {
			std::unique_lock lock{ accessMutex };
			if (!httpsConnections.contains(workloadType)) {
				httpsConnections[workloadType] = makeUnique<HttpsConnection>();
			}
			return *httpsConnections[workloadType].get();
		}

		RateLimitData& RateLimitManager::getRateLimitData(HttpsWorkloadType workloadType) noexcept {
			std::unique_lock lock{ accessMutex };
			return *rateLimitValues[rateLimitValueBuckets[workloadType]].get();
		}

		HttpsConnectionStackHolder::HttpsConnectionStackHolder(RateLimitManager& connectionManager, HttpsWorkloadData&& workload) noexcept {
			while (HttpsWorkloadData::workloadIdsInternal[workload.getWorkloadType()]->load() < workload.thisWorkerId.load() &&
				workload.thisWorkerId.load() != 0) {
				std::this_thread::sleep_for(1ms);
			}

			connection = &connectionManager.getConnection(workload.getWorkloadType());
			connection->resetValues(std::move(workload));
		}

		HttpsConnectionStackHolder::~HttpsConnectionStackHolder() noexcept {
			auto value = HttpsWorkloadData::workloadIdsInternal[connection->workload.getWorkloadType()]->load();
			HttpsWorkloadData::workloadIdsInternal[connection->workload.getWorkloadType()]->store(value + 1);
		}

		HttpsConnection& HttpsConnectionStackHolder::getConnection() noexcept {
			return *connection;
		}

		RateLimitStackHolder::RateLimitStackHolder(RateLimitManager& connectionManager, HttpsWorkloadType workloadType) noexcept {
			rateLimitData = &connectionManager.getRateLimitData(workloadType);
			rateLimitData->theSemaphore.acquire();
		}

		RateLimitStackHolder::~RateLimitStackHolder() noexcept {
			rateLimitData->theSemaphore.release();
		}

		RateLimitData& RateLimitStackHolder::getRateLimitData() noexcept {
			return *rateLimitData;
		}

		void RateLimitManager::initialize() noexcept {
			for (int64_t enumOne = static_cast<int64_t>(HttpsWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpsWorkloadType::LAST);
				 enumOne++) {
				UniquePtr<RateLimitData> rateLimitData{ makeUnique<RateLimitData>() };
				rateLimitData->tempBucket = std::to_string(std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count());
				rateLimitValueBuckets[static_cast<HttpsWorkloadType>(enumOne)] = rateLimitData->tempBucket;
				rateLimitValues[rateLimitData->tempBucket] = std::move(rateLimitData);
				std::this_thread::sleep_for(1ms);
			}
		}

		HttpsClient::HttpsClient(const std::string& botTokenNew) noexcept : HttpsClientCore(botTokenNew), connectionManager() {
			connectionManager.initialize();
		};

		HttpsResponseData HttpsClient::httpsRequest(HttpsConnection& connection) noexcept {
			RateLimitStackHolder rateLimitData{ connectionManager, connection.workload.workloadType };

			HttpsResponseData resultData = executeByRateLimitData(connection, rateLimitData.getRateLimitData());
			return resultData;
		}

		HttpsResponseData HttpsClientCore::httpsRequestInternal(HttpsConnection& connection, RateLimitData& rateLimitData) noexcept {
			if (connection.workload.baseUrl == "") {
				connection.workload.baseUrl = "https://discord.com/api/v10";
			}
			if (connection.workload.baseUrl == "https://discord.com/api/v10") {
				connection.workload.headersToInsert["Authorization"] = "Bot " + botToken;
				connection.workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
				if (connection.workload.payloadType == PayloadType::Application_Json) {
					connection.workload.headersToInsert["Content-Type"] = "application/json";
				} else if (connection.workload.payloadType == PayloadType::Multipart_Form) {
					connection.workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
				}
			}
			if (connection.currentReconnectTries >= connection.maxReconnectTries) {
				connection.disconnect();
				return HttpsResponseData{};
			}
			if (!connection.areWeConnected()) {
				connection.currentBaseUrl = connection.workload.baseUrl;
				connection.tcpConnection = HttpsTCPConnection{ connection.workload.baseUrl, static_cast<uint16_t>(443), &connection };
				if (connection.tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection, rateLimitData);
				}
			}
			auto request = connection.buildRequest(connection.workload);
			if (connection.areWeConnected()) {
				connection.tcpConnection.writeData(request, true);
				if (connection.tcpConnection.currentStatus != ConnectionStatus::NO_Error) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection, rateLimitData);
				}

				auto resultNew = getResponse(connection, rateLimitData);
				if (resultNew.responseCode == -1) {
					++connection.currentReconnectTries;
					connection.disconnect();
					return httpsRequestInternal(connection, rateLimitData);
				} else {
					return resultNew;
				}
			} else {
				++connection.currentReconnectTries;
				return httpsRequestInternal(connection, rateLimitData);
			}
		}

		HttpsResponseData HttpsClient::executeByRateLimitData(HttpsConnection& connection, RateLimitData& rateLimitData) noexcept {
			HttpsResponseData returnData{};
			Milliseconds timeRemaining{};
			Milliseconds currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
			if (connection.workload.workloadType == HttpsWorkloadType::Delete_Message_Old) {
				rateLimitData.msRemain.store(Milliseconds{ 4000 });
			} else if (connection.workload.workloadType == HttpsWorkloadType::Delete_Message ||
				connection.workload.workloadType == HttpsWorkloadType::Patch_Message) {
				rateLimitData.areWeASpecialBucket.store(true);
			}
			if (rateLimitData.areWeASpecialBucket.load()) {
				rateLimitData.msRemain.store(Milliseconds{ static_cast<int64_t>(ceil(5000.0f / 4.0f)) });
				Milliseconds targetTime{ rateLimitData.sampledTimeInMs.load() + rateLimitData.msRemain.load() };
				timeRemaining = targetTime - currentTime;
			} else if (rateLimitData.didWeHitRateLimit.load()) {
				Milliseconds targetTime{ rateLimitData.sampledTimeInMs.load() + rateLimitData.msRemain.load() };
				timeRemaining = targetTime - currentTime;
				rateLimitData.didWeHitRateLimit.store(false);
			} else if (rateLimitData.doWeWait.load()) {
				Milliseconds targetTime{ rateLimitData.sampledTimeInMs.load() + rateLimitData.msRemain.load() };
				timeRemaining = targetTime - currentTime;
				rateLimitData.doWeWait.store(false);
			}
			if (timeRemaining.count() > 0) {
				MessagePrinter::printSuccess<PrintMessageType::Https>("We're waiting on rate-limit: " + std::to_string(timeRemaining.count()));
				Milliseconds targetTime{ currentTime + timeRemaining };
				while (targetTime > currentTime && targetTime.count() > 0 && currentTime.count() > 0 && timeRemaining.count() > 0) {
					currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
					timeRemaining = targetTime - currentTime;
					if (timeRemaining.count() <= 20) {
						continue;
					} else {
						std::this_thread::sleep_for(
							Milliseconds{ static_cast<int64_t>(static_cast<double>(timeRemaining.count()) * 80.0f / 100.0f) });
					}
				}
			}

			returnData = HttpsClient::httpsRequestInternal(connection, rateLimitData);
			rateLimitData.sampledTimeInMs.store(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()));

			if (rateLimitData.tempBucket != "") {
				rateLimitData.tempBucket = "";
			}
			std::string currentBucket = rateLimitData.bucket;
			if (!connectionManager.rateLimitValues.contains(rateLimitData.bucket)) {
				UniquePtr<RateLimitData> rateLimitData{ makeUnique<RateLimitData>() };
				connectionManager.rateLimitValues[connectionManager.rateLimitValueBuckets[connection.workload.workloadType]].swap(rateLimitData);
				connectionManager.rateLimitValueBuckets[connection.workload.workloadType] = currentBucket;
				connectionManager.rateLimitValues[currentBucket] = std::move(rateLimitData);
			}
			if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
				MessagePrinter::printSuccess<PrintMessageType::Https>(connection.workload.callStack +
					" Success: " + static_cast<std::string>(returnData.responseCode) + ": " + returnData.responseData);
			} else if (returnData.responseCode == 429) {
				connection.updateRateLimitData(rateLimitData, connection.data.responseHeaders);
				rateLimitData.didWeHitRateLimit = true;
				if (connection.data.responseHeaders.contains("x-ratelimit-retry-after")) {
					rateLimitData.msRemain.store(std::chrono::milliseconds{ stoll(connection.data.responseHeaders["x-ratelimit-retry-after"]) });
				}
				rateLimitData.didWeHitRateLimit.store(true);
				rateLimitData.sampledTimeInMs.store(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()));
				MessagePrinter::printError<PrintMessageType::Https>(connection.workload.callStack +
					"::httpsRequest(), We've hit rate limit! Time Remaining: " + std::to_string(rateLimitData.msRemain.load().count()));
				connection.resetValues(std::move(connection.workload));
				returnData = executeByRateLimitData(connection, rateLimitData);
			}
			return returnData;
		}

		HttpsResponseData HttpsClientCore::getResponse(HttpsConnection& connection, RateLimitData& rateLimitData) noexcept {
			StopWatch stopWatch{ 10000ms };
			while (connection.data.currentState != HttpsState::Complete && !stopWatch.hasTimePassed()) {
				if (connection.areWeConnected()) {
					switch (connection.tcpConnection.processIO(10)) {
						case ConnectionStatus::NO_Error: {
							continue;
						}
						case ConnectionStatus::POLLHUP_Error: {
							return connection.finalizeReturnValues(rateLimitData);
						}
						default: {
							if (connection.currentReconnectTries >= connection.maxReconnectTries) {
								return connection.finalizeReturnValues(rateLimitData);
							}
							++connection.currentReconnectTries;
							return httpsRequestInternal(connection, rateLimitData);
						}
					}
				} else {
					if (connection.currentReconnectTries >= connection.maxReconnectTries) {
						return connection.finalizeReturnValues(rateLimitData);
					}
					++connection.currentReconnectTries;
					return httpsRequestInternal(connection, rateLimitData);
				}
			}
			return connection.finalizeReturnValues(rateLimitData);
		}
	}
}
