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
/// Https.cpp - Source file for the Http class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.cpp

#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	HttpsError::HttpsError(std::string message) : DCAException(message){};

	HttpsRnRBuilder::HttpsRnRBuilder(bool doWePrintErrorMessagesNew) {
		this->doWePrintErrorMessages = doWePrintErrorMessagesNew;
	}

	void HttpsRnRBuilder::updateRateLimitData(RateLimitData& rateLimitData, std::unordered_map<std::string, std::string>& headersNew) {
		if (static_cast<HttpsConnection*>(this)->data.responseHeaders.contains("x-ratelimit-bucket")) {
			rateLimitData.bucket = static_cast<HttpsConnection*>(this)->data.responseHeaders["x-ratelimit-bucket"];
		}
		if (static_cast<HttpsConnection*>(this)->data.responseHeaders.contains("x-ratelimit-reset-after")) {
			rateLimitData.msRemain.store(Milliseconds{
				static_cast<int64_t>(ceil(stod(static_cast<HttpsConnection*>(this)->data.responseHeaders["x-ratelimit-reset-after"])) * 1000.0f) });
		}
		if (static_cast<HttpsConnection*>(this)->data.responseHeaders.contains("x-ratelimit-remaining")) {
			rateLimitData.getsRemaining.store(
				static_cast<int64_t>(stoi(static_cast<HttpsConnection*>(this)->data.responseHeaders["x-ratelimit-remaining"])));
		}
		if (rateLimitData.getsRemaining.load() <= 1 || rateLimitData.areWeASpecialBucket.load()) {
			rateLimitData.doWeWait.store(true);
		}
	};

	HttpsResponseData HttpsRnRBuilder::finalizeReturnValues(RateLimitData& rateLimitData) {
		if (static_cast<HttpsConnection*>(this)->data.responseMessage.size() >= static_cast<HttpsConnection*>(this)->data.contentLength &&
			static_cast<HttpsConnection*>(this)->data.contentLength > 0) {
			std::string string =
				static_cast<HttpsConnection*>(this)->data.responseMessage.substr(0, static_cast<HttpsConnection*>(this)->data.contentLength);
			static_cast<HttpsConnection*>(this)->data.responseMessage = std::move(string);
		} else {
			static_cast<HttpsConnection*>(this)->data.responseMessage = static_cast<std::string>(
				static_cast<HttpsConnection*>(this)
					->inputBufferReal[LengthData{ .offSet = 0, .length = static_cast<HttpsConnection*>(this)->data.contentLength }]);
		}
		this->updateRateLimitData(rateLimitData, static_cast<HttpsConnection*>(this)->data.responseHeaders);
		return static_cast<HttpsConnection*>(this)->data;
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

	uint64_t HttpsRnRBuilder::parseHeaders(StringBuffer& other) {
		if (static_cast<std::string_view>(other).find("\r\n\r\n") != std::string::npos) {
			DiscordCoreAPI::StopWatch stopWatch{ 1500ms };
			std::string newString{};
			newString.insert(newString.begin(), other.begin(),
				other.begin() + static_cast<std::string_view>(other).find("\r\n\r\n") + std::string("\r\n\r\n").size());
			other.erase(newString.size());
			while (newString.size() > 0 && newString.find(":") != std::string::npos && newString.find("\r\n") != std::string::npos) {
				if (stopWatch.hasTimePassed()) {
					break;
				}
				int64_t currentOffset{};
				std::string lineString = newString.substr(0, newString.find("\r\n") + 2);
				currentOffset = lineString.size();
				std::string key = lineString.substr(0, lineString.find(":"));
				std::string value = lineString.substr(lineString.find(":") + 2, lineString.size() - key.size() - 2);
				value = value.substr(0, value.find("\r\n"));
				static_cast<HttpsConnection*>(this)->data.responseHeaders[key] = value;
				std::string tempString{};
				tempString.insert(tempString.begin(), newString.begin() + currentOffset, newString.end());
				newString = tempString;
			}

			if (static_cast<HttpsConnection*>(this)->data.responseHeaders.contains("Transfer-Encoding") &&
					static_cast<HttpsConnection*>(this)->data.responseHeaders["Transfer-Encoding"] == "chunked" ||
				static_cast<HttpsConnection*>(this)->data.responseHeaders.contains("transfer-encoding") &&
					static_cast<HttpsConnection*>(this)->data.responseHeaders["transfer-encoding"] == "chunked") {
				this->isItChunked = true;
			}
			this->doWeHaveHeaders = true;
			static_cast<HttpsConnection*>(this)->data.currentState = HttpsState::Collecting_Size;
			return newString.size();
		}
		return 0;
	}

	bool HttpsRnRBuilder::parseChunk(StringBuffer& other) {
		if (this->isItChunked) {
			if (static_cast<std::string_view>(other).find("\r\n0\r\n\r\n") != std::string::npos) {
				DiscordCoreAPI::StopWatch stopWatch{ 1500ms };
				while (static_cast<std::string_view>(other).find("\r\n") != static_cast<std::string_view>(other).find("\r\n0\r\n\r\n")) {
					if (stopWatch.hasTimePassed()) {
						break;
					}
					this->clearCRLF(other);
					this->parseSize(other);
					this->clearCRLF(other);
					if (static_cast<std::string_view>(other).find("\r\n") != static_cast<std::string_view>(other).find("\r\n0\r\n\r\n")) {
						static_cast<HttpsConnection*>(this)->data.responseMessage.insert(
							static_cast<HttpsConnection*>(this)->data.responseMessage.end(), other.begin(),
							other.begin() + static_cast<std::string_view>(other).find("\r\n"));
						other.erase(static_cast<std::string_view>(other).find("\r\n") + 2);
					}
				}
				static_cast<HttpsConnection*>(this)->data.responseMessage.insert(static_cast<HttpsConnection*>(this)->data.responseMessage.end(),
					other.begin(), other.begin() + static_cast<std::string_view>(other).find("\r\n0\r\n\r\n"));
				return false;
			} else {
				return true;
			}
		} else {
			if (!this->doWeHaveContentSize) {
				this->parseSize(other);
			}
			if (static_cast<HttpsConnection*>(this)->data.contentLength == 0) {
				return false;
			}
			if (other.size() >= static_cast<uint64_t>(static_cast<HttpsConnection*>(this)->data.contentLength)) {
				static_cast<HttpsConnection*>(this)->data.responseMessage.insert(static_cast<HttpsConnection*>(this)->data.responseMessage.end(),
					other.begin(), other.begin() + static_cast<HttpsConnection*>(this)->data.contentLength);
				return false;
			} else {
				return true;
			}
		}
	}

	uint64_t HttpsRnRBuilder::parseSize(StringBuffer& other) {
		if (static_cast<HttpsConnection*>(this)->data.responseHeaders.contains("Content-Length")) {
			static_cast<HttpsConnection*>(this)->data.contentLength =
				stoll(static_cast<HttpsConnection*>(this)->data.responseHeaders["Content-Length"]);
			this->doWeHaveContentSize = true;
			static_cast<HttpsConnection*>(this)->data.currentState = HttpsState::Collecting_Contents;
			return 0;
		}
		std::string valueString{};
		uint64_t hexIndex{};
		bool isThereHexValues{};
		for (uint64_t x = 0; x < other.size(); ++x) {
			if (isxdigit(other[x]) != 0 && static_cast<int32_t>(other[x]) != EOF) {
				isThereHexValues = true;
				valueString.push_back(other[x]);
			} else {
				hexIndex = x;
				break;
			}
		}
		if (!isThereHexValues) {
			static_cast<HttpsConnection*>(this)->data.contentLength += 0;
			return 0;
		} else {
			static_cast<HttpsConnection*>(this)->data.contentLength += stoll(valueString, nullptr, 16);
			other.erase(hexIndex);
			this->doWeHaveContentSize = true;
			static_cast<HttpsConnection*>(this)->data.currentState = HttpsState::Collecting_Contents;
			return hexIndex;
		}
	}

	uint64_t HttpsRnRBuilder::parseCode(StringBuffer& other) {
		if (static_cast<std::string_view>(other).find("HTTP/1.") != std::string::npos) {
			uint64_t firstNumberIndex{};
			uint64_t lastNumberIndex{};
			bool haveWeStarted{};
			for (uint64_t x = static_cast<std::string_view>(other).find("HTTP/1.") + std::string("HTTP/1.").size() + 1;
				 x < static_cast<std::string_view>(other).size(); ++x) {
				if (!haveWeStarted && (isalnum(static_cast<uint8_t>(other[x])) != 0)) {
					firstNumberIndex = x;
					haveWeStarted = true;
				} else if (haveWeStarted && (isalnum(static_cast<uint8_t>(other[x])) == 0)) {
					lastNumberIndex = x;
					break;
				}
			}
			static_cast<HttpsConnection*>(this)->data.responseCode =
				stoll(static_cast<std::string>(other[LengthData{ firstNumberIndex, lastNumberIndex - firstNumberIndex }]));
			other.erase(static_cast<std::string_view>(other).find("\r\n"));
			static_cast<HttpsConnection*>(this)->data.currentState = HttpsState::Collecting_Headers;
			return static_cast<std::string_view>(other).find("\r\n");
		} else if (static_cast<std::string_view>(other).size() > 200 && static_cast<std::string_view>(other).find("HTTP/1.") == std::string::npos) {
			static_cast<HttpsConnection*>(this)->data.responseCode = 200;
			static_cast<HttpsConnection*>(this)->data.currentState = HttpsState::Collecting_Contents;
			return 0;
		}
		return 0;
	}

	void HttpsRnRBuilder::clearCRLF(StringBuffer& other) {
		uint64_t count{};
		for (uint64_t x = 0; x < other.size(); ++x) {
			if (isspace(static_cast<uint8_t>(other[x])) != 0) {
				++count;
			} else {
				break;
			}
		}
		other.erase(count);
	}

	HttpsConnection::HttpsConnection(bool doWePrintErrorMessages) : HttpsRnRBuilder(doWePrintErrorMessages){};

	void HttpsConnection::handleBuffer() noexcept {
		this->data.stopWatch.resetTimer();
		while (true) {
			switch (this->data.currentState) {
				case HttpsState::Collecting_Code: {
					if (this->data.stopWatch.hasTimePassed()) {
						this->areWeDoneTheRequest = true;
						return;
					}
					auto stringView = this->getInputBuffer();
					if (stringView.size() > 0) {
						this->inputBufferReal.writeData(stringView.data(), stringView.size());
					}
					this->parseCode(this->inputBufferReal);
					if (this->data.responseCode == 400) {
					}
					this->data.stopWatch.resetTimer();
					if (this->data.responseCode == 204) {
						this->areWeDoneTheRequest = true;
						return;
					}
					return;
				}
				case HttpsState::Collecting_Headers: {
					if (this->data.stopWatch.hasTimePassed()) {
						this->areWeDoneTheRequest = true;
						return;
					}
					if (!this->doWeHaveHeaders) {
						auto stringView = this->getInputBuffer();
						if (stringView.size() > 0) {
							this->inputBufferReal.writeData(stringView.data(), stringView.size());
						}
						this->parseHeaders(this->inputBufferReal);
						this->data.stopWatch.resetTimer();
					}
					return;
				}
				case HttpsState::Collecting_Size: {
					if (this->data.stopWatch.hasTimePassed()) {
						this->areWeDoneTheRequest = true;
						return;
					}
					if (!this->doWeHaveContentSize) {
						auto stringView = this->getInputBuffer();
						if (stringView.size() > 0) {
							this->inputBufferReal.writeData(stringView.data(), stringView.size());
						}
						this->clearCRLF(this->inputBufferReal);
						this->parseSize(this->inputBufferReal);
						this->clearCRLF(this->inputBufferReal);
						this->data.stopWatch.resetTimer();
					}
					return;
				}
				case HttpsState::Collecting_Contents: {
					auto stringView = this->getInputBuffer();
					if (stringView.size() > 0) {
						this->inputBufferReal.writeData(stringView.data(), stringView.size());
					}
					auto result = this->parseChunk(this->inputBufferReal);
					if ((this->data.responseMessage.size() >= this->data.contentLength && !result) || this->data.stopWatch.hasTimePassed() ||
						!result || (this->data.responseCode == -5 && this->data.contentLength == -5)) {
						this->areWeDoneTheRequest = true;
						return;
					} else {
						this->data.stopWatch.resetTimer();
					}
					return;
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		return;
	}

	void HttpsConnection::disconnect() noexcept {
		if (this->socket != SOCKET_ERROR) {
			this->socket = SOCKET_ERROR;
		}
		this->resetValues();
	}

	void HttpsConnection::resetValues() {
		this->bytesRead = 0;
		this->currentBaseUrl = "";
		this->isItChunked = false;
		this->inputBufferReal.clear();
		this->outputBuffer.clear();
		this->inputBuffer.clear();
		this->data = HttpsResponseData{};
		this->doWeHaveContentSize = false;
		this->areWeDoneTheRequest = false;
		SSLDataInterface::bytesRead = 0;
		this->doWeHaveHeaders = false;
		this->isItChunked = false;
	}

	HttpsConnectionManager::HttpsConnectionManager(DiscordCoreAPI::ConfigManager* configManagerNew) {
		this->configManager = configManagerNew;
	}

	std::unordered_map<std::string, std::unique_ptr<RateLimitData>>& HttpsConnectionManager::getRateLimitValues() {
		return this->rateLimitValues;
	}

	std::unordered_map<HttpsWorkloadType, std::string>& HttpsConnectionManager::getRateLimitValueBuckets() {
		return this->rateLimitValueBuckets;
	}

	HttpsConnection* HttpsConnectionManager::getConnection() {
		std::unique_lock lock{ this->accessMutex };
		for (auto& [key, value]: this->httpsConnections) {
			if (!value->areWeCheckedOut.load()) {
				value->areWeCheckedOut.store(true);
				value->currentReconnectTries = 0;
				value->resetValues();
				return value.get();
			}
		}
		++this->currentIndex;
		this->httpsConnections[this->currentIndex] = std::make_unique<HttpsConnection>(this->configManager->doWePrintHttpsErrorMessages());
		return this->httpsConnections[this->currentIndex].get();
	}

	void HttpsConnectionManager::initialize() {
		for (int64_t enumOne = static_cast<int64_t>(HttpsWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpsWorkloadType::LAST); ++enumOne) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			rateLimitData->tempBucket = std::to_string(std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count());
			this->getRateLimitValueBuckets()[static_cast<HttpsWorkloadType>(enumOne)] = rateLimitData->tempBucket;
			this->getRateLimitValues()[rateLimitData->tempBucket] = std::move(rateLimitData);
			std::this_thread::sleep_for(1ms);
		}
	}

	HttpsClient::HttpsClient(DiscordCoreAPI::ConfigManager* configManagerNew) : configManager(configManagerNew), connectionManager(configManagerNew) {
		this->connectionManager.initialize();
	};

	template<> void HttpsClient::submitWorkloadAndGetResult<void>(const HttpsWorkloadData& workload, void* returnValue) {
		workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
		workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
		if (workload.payloadType == PayloadType::Application_Json) {
			workload.headersToInsert["Content-Type"] = "application/json";
		} else if (workload.payloadType == PayloadType::Multipart_Form) {
			workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
		}
		auto returnData = this->httpsRequest(workload);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string errorMessage{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack + " Https Error: " +
				static_cast<std::string>(returnData.responseCode) + "\nThe Request: " + workload.content + DiscordCoreAPI::reset() + "\n\n" };
			HttpsError theError{ errorMessage };
			theError.errorCode = returnData.responseCode;
			throw theError;
		}
		return;
	}

	HttpsResponseData HttpsClient::submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew) {
		RateLimitData rateLimitData{};
		auto connection = this->connectionManager.getConnection();
		auto returnData = this->httpsRequestInternal(*connection, workloadNew, rateLimitData);
		connection->areWeCheckedOut.store(false);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string errorMessage{ DiscordCoreAPI::shiftToBrightRed() + workloadNew.callStack + " Https Error: " +
				static_cast<std::string>(returnData.responseCode) + "\nThe Request: " + workloadNew.content + DiscordCoreAPI::reset() + "\n\n" };
			HttpsError theError{ errorMessage };
			theError.errorCode = returnData.responseCode;
			throw theError;
		}
		return returnData;
	}

	HttpsResponseData HttpsClient::httpsRequest(const HttpsWorkloadData& workload) {
		if (workload.baseUrl == "") {
			workload.baseUrl = "https://discord.com/api/v10";
		}

		RateLimitData& rateLimitData =
			*this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]].get();
		if (!rateLimitData.haveWeGoneYet.load()) {
			std::this_thread::sleep_for(100ms);
			rateLimitData.haveWeGoneYet.store(true);
		}
		if (HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load() >= workload.thisWorkerId.load() &&
			!rateLimitData.theSemaphore.try_acquire()) {
			HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->store(0);
			HttpsWorkloadData::workloadIdsExternal[workload.workloadType]->store(0);
			workload.thisWorkerId.store(1);
		}
		rateLimitData.theSemaphore.release();

		while (HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load() < workload.thisWorkerId.load() &&
			workload.thisWorkerId.load() != 0) {
			std::this_thread::sleep_for(1ms);
		}

		while (!rateLimitData.theSemaphore.try_acquire()) {
			std::this_thread::sleep_for(1ms);
		}

		HttpsResponseData resultData = this->executeByRateLimitData(workload, rateLimitData);
		auto value = HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load();
		HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->store(value + 1);
		rateLimitData.theSemaphore.release();
		return resultData;
	}

	HttpsResponseData HttpsClient::httpsRequestInternal(HttpsConnection& httpsConnection, const HttpsWorkloadData& workload,
		RateLimitData& rateLimitData) {
		httpsConnection.resetValues();
		if (httpsConnection.currentReconnectTries >= httpsConnection.maxReconnectTries) {
			httpsConnection.currentReconnectTries = 0;
			httpsConnection.disconnect();
			return HttpsResponseData{};
		}
		if (workload.baseUrl != httpsConnection.currentBaseUrl || !httpsConnection.areWeStillConnected() || httpsConnection.doWeConnect) {
			httpsConnection.currentBaseUrl = workload.baseUrl;
			if (!httpsConnection.connect(workload.baseUrl, 443, this->configManager->doWePrintHttpsErrorMessages(), false)) {
				++httpsConnection.currentReconnectTries;
				httpsConnection.doWeConnect = true;
				return this->httpsRequestInternal(httpsConnection, workload, rateLimitData);
			}
			httpsConnection.doWeConnect = false;
		}
		auto request = httpsConnection.buildRequest(workload);
		DiscordCoreAPI::StopWatch stopWatch{ 5000ms };
		ProcessIOResult result{};
		do {
			if (stopWatch.hasTimePassed()) {
				break;
			}
			result = httpsConnection.writeData(request, true);
		} while (result == ProcessIOResult::Error);
		if (result == ProcessIOResult::Error) {
			++httpsConnection.currentReconnectTries;
			httpsConnection.doWeConnect = true;
			return this->httpsRequestInternal(httpsConnection, workload, rateLimitData);
		};
		auto resultNew = this->getResponse(httpsConnection, rateLimitData);
		if (resultNew.responseCode == -1) {
			++httpsConnection.currentReconnectTries;
			httpsConnection.doWeConnect = true;
			return this->httpsRequestInternal(httpsConnection, workload, rateLimitData);
		} else {
			httpsConnection.currentReconnectTries = 0;
			return resultNew;
		}
	}

	HttpsResponseData HttpsClient::executeByRateLimitData(const HttpsWorkloadData& workload, RateLimitData& rateLimitData) {
		HttpsResponseData returnData{};
		Milliseconds timeRemaining{};
		Milliseconds currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
		if (workload.workloadType == HttpsWorkloadType::Delete_Message_Old) {
			rateLimitData.msRemain.store(Milliseconds{ 4000 });
		} else if (workload.workloadType == HttpsWorkloadType::Delete_Message || workload.workloadType == HttpsWorkloadType::Patch_Message) {
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
			if (this->configManager->doWePrintHttpsSuccessMessages()) {
				cout << DiscordCoreAPI::shiftToBrightBlue() << "We're waiting on rate-limit: " << timeRemaining.count() << DiscordCoreAPI::reset()
					 << endl
					 << endl;
			}
			Milliseconds targetTime{ currentTime + timeRemaining };
			while (targetTime > currentTime && targetTime.count() > 0 && currentTime.count() > 0 && timeRemaining.count() > 0) {
				currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
				timeRemaining = targetTime - currentTime;
				if (timeRemaining.count() <= 20) {
					continue;
				} else {
					std::this_thread::sleep_for(Milliseconds{ static_cast<int64_t>(static_cast<double>(timeRemaining.count()) * 80.0f / 100.0f) });
				}
			}
		}
		auto httpsConnection = this->connectionManager.getConnection();
		returnData = HttpsClient::httpsRequestInternal(*httpsConnection, workload, rateLimitData);

		httpsConnection->areWeCheckedOut.store(false);
		rateLimitData.sampledTimeInMs.store(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()));

		if (rateLimitData.tempBucket != "") {
			rateLimitData.tempBucket = "";
		}
		std::string currentBucket = rateLimitData.bucket;
		if (!this->connectionManager.getRateLimitValues().contains(rateLimitData.bucket)) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]].swap(
				rateLimitData);
			this->connectionManager.getRateLimitValueBuckets()[workload.workloadType] = currentBucket;
			this->connectionManager.getRateLimitValues()[currentBucket] = std::move(rateLimitData);
		}
		if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
			if (this->configManager->doWePrintHttpsSuccessMessages()) {
				cout << DiscordCoreAPI::shiftToBrightGreen() << workload.callStack + " Success: " << static_cast<std::string>(returnData.responseCode)
					 << ": " << returnData.responseMessage << DiscordCoreAPI::reset() << endl
					 << endl;
			}
		} else {
			if (returnData.responseCode == 429) {
				returnData.responseMessage.reserve(returnData.responseMessage.size() + simdjson::SIMDJSON_PADDING);
				auto document =
					parser.iterate(returnData.responseMessage.data(), returnData.responseMessage.length(), returnData.responseMessage.capacity());
				double doubleVal{};
				if (returnData.responseMessage.size() > 0 && document["retry_after"].get(doubleVal) == simdjson::error_code::SUCCESS) {
					rateLimitData.msRemain.store(Milliseconds{ static_cast<int64_t>(ceil(doubleVal)) * 1000 });
				}

				rateLimitData.didWeHitRateLimit.store(true);
				rateLimitData.sampledTimeInMs.store(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()));
				if (this->configManager->doWePrintHttpsErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << workload.callStack + "::httpsRequest(), We've hit rate limit! Time Remaining: "
						 << std::to_string(this->connectionManager
											   .getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]]
											   ->msRemain.load()
											   .count())
						 << DiscordCoreAPI::reset() << endl
						 << endl;
				}
				returnData = this->executeByRateLimitData(workload, rateLimitData);
			}
		}
		return returnData;
	}

	HttpsResponseData HttpsClient::getResponse(HttpsConnection& connection, RateLimitData& rateLimitData) {
		connection.resetValues();
		ProcessIOResult result{};
		DiscordCoreAPI::StopWatch stopWatch{ 5000ms };
		while (!connection.areWeDoneTheRequest && result != ProcessIOResult::Error && !stopWatch.hasTimePassed()) {
			result = connection.processIO(1000);
		}
		auto data = connection.finalizeReturnValues(rateLimitData);
		connection.resetValues();
		return data;
	}
}