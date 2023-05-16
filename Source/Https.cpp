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
/// Https.cpp - Source file for the Https class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.cpp

#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	HttpsError::HttpsError(std::string message, std::source_location location) : DCAException(message, location){};

	HttpsRnRBuilder::HttpsRnRBuilder(bool doWePrintErrorMessagesNew) {
		this->doWePrintErrorMessages = doWePrintErrorMessagesNew;
	}

	void HttpsRnRBuilder::updateRateLimitData(RateLimitData& rateLimitData, std::unordered_map<std::string, std::string>& headersNew) {
		auto connection = static_cast<HttpsConnection*>(this);
		if (connection->data.responseHeaders.contains("x-ratelimit-bucket")) {
			rateLimitData.bucket = connection->data.responseHeaders["x-ratelimit-bucket"];
		}
		if (connection->data.responseHeaders.contains("x-ratelimit-reset-after")) {
			rateLimitData.msRemain.store(Milliseconds{
				static_cast<int64_t>(ceil(stod(connection->data.responseHeaders["x-ratelimit-reset-after"])) * 1000.0f) });
		}
		if (connection->data.responseHeaders.contains("x-ratelimit-remaining")) {
			rateLimitData.getsRemaining.store(
				static_cast<int64_t>(stoi(connection->data.responseHeaders["x-ratelimit-remaining"])));
		}
		if (rateLimitData.getsRemaining.load() <= 1 || rateLimitData.areWeASpecialBucket.load()) {
			rateLimitData.doWeWait.store(true);
		}
	};

	HttpsResponseData HttpsRnRBuilder::finalizeReturnValues(RateLimitData& rateLimitData) {
		auto connection = static_cast<HttpsConnection*>(this);
		if (connection->data.responseData.size() >= connection->data.contentLength && connection->data.contentLength > 0) {
			std::string string = connection->data.responseData.substr(0, connection->data.contentLength);
			connection->data.responseData = std::move(string);
		} else {
			auto pos1 = connection->data.responseData.find_first_of('{');
			auto pos2 = connection->data.responseData.find_last_of('}');
			if (pos1 != std::string::npos && pos2 != std::string::npos) {
				connection->data.responseData = connection->data.responseData.substr(pos1, pos2);
			}
		}
		this->updateRateLimitData(rateLimitData, connection->data.responseHeaders);
		return connection->data;
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
		std::cout << "CURRENT REQUEST: " << returnString << std::endl;
		return returnString;
	}

	uint64_t HttpsRnRBuilder::parseHeaders() {
		auto connection = static_cast<HttpsConnection*>(this);
		if (static_cast<std::string_view>(connection->inputBufferReal).find("\r\n\r\n") != std::string::npos) {
			DiscordCoreAPI::StopWatch stopWatch{ 1500ms };
			std::string newString{};
			newString.insert(newString.begin(), connection->inputBufferReal.begin(),
				connection->inputBufferReal.begin() + static_cast<std::string_view>(connection->inputBufferReal).find("\r\n\r\n") + std::string("\r\n\r\n").size());
			connection->inputBufferReal.erase(newString.size());
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
				connection->data.responseHeaders[key] = value;
				std::string tempString{};
				tempString.insert(tempString.begin(), newString.begin() + currentOffset, newString.end());
				newString = tempString;
			}
			for (auto& [key, value]: connection->data.responseHeaders) {
				std::cout << "KEY: " << key << ", VALUE: " << value << std::endl;
			}
			if (connection->data.responseHeaders.contains("Transfer-Encoding") &&
					connection->data.responseHeaders["Transfer-Encoding"] == "chunked" ||
				connection->data.responseHeaders.contains("transfer-encoding") &&
					connection->data.responseHeaders["transfer-encoding"] == "chunked") {
				this->isItChunked = true;
			}
			this->doWeHaveHeaders = true;
			connection->data.currentState = HttpsState::Collecting_Size;
			return newString.size();
		}
		return 0;
	}

	void HttpsRnRBuilder::parseChunk() {
		auto connection = static_cast<HttpsConnection*>(this);
		if (this->isItChunked) {
			if (static_cast<std::string_view>(connection->inputBufferReal).find("\r\n0\r\n\r\n") != std::string::npos) {
				DiscordCoreAPI::StopWatch stopWatch{ 1500ms };
				while (static_cast<std::string_view>(connection->inputBufferReal).find("\r\n") != static_cast<std::string_view>(connection->inputBufferReal).find("\r\n0\r\n\r\n")) {
					if (stopWatch.hasTimePassed()) {
						break;
					}
					this->clearCRLF();
					this->parseSize();
					this->clearCRLF();
					if (static_cast<std::string_view>(connection->inputBufferReal).find("\r\n") != static_cast<std::string_view>(connection->inputBufferReal).find("\r\n0\r\n\r\n")) {
						connection->data.responseData.insert(connection->data.responseData.end(),
							connection->inputBufferReal.begin(), connection->inputBufferReal.begin() + static_cast<std::string_view>(connection->inputBufferReal).find("\r\n"));
						connection->inputBufferReal.erase(static_cast<std::string_view>(connection->inputBufferReal).find("\r\n") + 2);
					}
				}
				connection->data.responseData.insert(connection->data.responseData.end(),
					connection->inputBufferReal.begin(), connection->inputBufferReal.begin() + static_cast<std::string_view>(connection->inputBufferReal).find("\r\n0\r\n\r\n"));
				connection->inputBufferReal.erase(static_cast<std::string_view>(connection->inputBufferReal).find("\r\n0\r\n\r\n"));
				return ;
			} else {
				return ;
			}
		} else {
			if (!this->doWeHaveContentSize) {
				this->parseSize();
			}
			if (connection->data.contentLength == 0) {
				return ;
			}
			if (connection->inputBufferReal.size() >= static_cast<uint64_t>(connection->data.contentLength)) {
				connection->data.responseData.insert(connection->data.responseData.end(),
					connection->inputBufferReal.begin(), connection->inputBufferReal.begin() + connection->data.contentLength);
				connection->inputBufferReal.erase(connection->data.contentLength);
				return;
			} else {
				return;
			}
		}
	}

	void HttpsRnRBuilder::parseSize() {
		auto connection = static_cast<HttpsConnection*>(this);
		if (connection->data.responseHeaders.contains("Content-Length")) {
			connection->data.contentLength =
				stoll(connection->data.responseHeaders["Content-Length"]);
			this->doWeHaveContentSize = true;
			connection->data.currentState = HttpsState::Collecting_Contents;
			return;
		}
		std::string valueString{};
		uint64_t hexIndex{};
		bool isThereHexValues{};
		for (uint64_t x = 0; x < connection->inputBufferReal.size(); ++x) {
			if (isxdigit(connection->inputBufferReal[x]) != 0 && static_cast<int32_t>(connection->inputBufferReal[x]) != EOF) {
				isThereHexValues = true;
				valueString.push_back(connection->inputBufferReal[x]);
			} else {
				hexIndex = x;
				break;
			}
		}
		if (!isThereHexValues) {
			connection->data.contentLength += 0;
			connection->data.currentState = HttpsState::Collecting_Contents;
			return ;
		} else {
			connection->data.contentLength += stoll(valueString, nullptr, 16);
			connection->inputBufferReal.erase(hexIndex);
			this->doWeHaveContentSize = true;
			connection->data.currentState = HttpsState::Collecting_Contents;
			return ;
		}
	}

	void HttpsRnRBuilder::parseCode() {
		auto connection = static_cast<HttpsConnection*>(this);
		if (static_cast<std::string_view>(connection->inputBufferReal).find("HTTP/1.") != std::string::npos) {
			uint64_t firstNumberIndex{};
			uint64_t lastNumberIndex{};
			bool haveWeStarted{};
			for (uint64_t x = static_cast<std::string_view>(connection->inputBufferReal).find("HTTP/1.") + std::string("HTTP/1.").size() + 1;
				 x < static_cast<std::string_view>(connection->inputBufferReal).size(); ++x) {
				if (!haveWeStarted && (isalnum(static_cast<uint8_t>(connection->inputBufferReal[x])) != 0)) {
					firstNumberIndex = x;
					haveWeStarted = true;
				} else if (haveWeStarted && (isalnum(static_cast<uint8_t>(connection->inputBufferReal[x])) == 0)) {
					lastNumberIndex = x;
					break;
				}
			}
			connection->data.responseCode =
				stoll(static_cast<std::string>(connection->inputBufferReal.stringView(firstNumberIndex, lastNumberIndex - firstNumberIndex)));
			connection->inputBufferReal.erase(static_cast<std::string_view>(connection->inputBufferReal).find("\r\n"));
			connection->data.currentState = HttpsState::Collecting_Headers;
			return;
		} else if (static_cast<std::string_view>(connection->inputBufferReal).size() > 200 && static_cast<std::string_view>(connection->inputBufferReal).find("HTTP/1.") == std::string::npos) {
			connection->data.responseCode = 200;
			connection->data.currentState = HttpsState::Collecting_Contents;
			return;
		}
		return;
	}

	void HttpsRnRBuilder::clearCRLF() {
		auto connection = static_cast<HttpsConnection*>(this);
		uint64_t count{};
		for (uint64_t x = 0; x < connection->inputBufferReal.size(); ++x) {
			if (isspace(static_cast<uint8_t>(connection->inputBufferReal[x])) != 0) {
				++count;
			} else {
				break;
			}
		}
		connection->inputBufferReal.erase(count);
	}

	HttpsConnection::HttpsConnection(bool doWePrintErrorMessages) : HttpsRnRBuilder(doWePrintErrorMessages){};

	void HttpsConnection::handleBuffer() noexcept {
		auto stringView = this->getInputBuffer();
		if (stringView.size() > 0) {
			this->inputBufferReal.writeData(stringView.data(), stringView.size());
		}
		switch (this->data.currentState) {
			case HttpsState::Collecting_Code: {
				if (inputBufferReal.size() > 0) {
					this->parseCode();
				}
				if (this->data.responseCode == 400 || this->data.responseCode == 204) {
					this->areWeDoneTheRequest = true;
					return;
				}
				return;
			}
			case HttpsState::Collecting_Headers: {
				if (!this->doWeHaveHeaders) {
					if (inputBufferReal.size() > 0) {
						this->parseHeaders();
					}
				}
				return;
			}
			case HttpsState::Collecting_Size: {
				if (!this->doWeHaveContentSize) {
					if (inputBufferReal.size() > 0) {
						this->clearCRLF();
						this->parseSize();
						this->clearCRLF();
					}
				}
				return;
			}
			case HttpsState::Collecting_Contents: {
				if (inputBufferReal.size() > 0) {
					this->parseChunk();
				}
				if (this->data.responseData.size() >= this->data.contentLength && this->data.contentLength > 0) {
					this->areWeDoneTheRequest = true;
					return;
				}
				return;
			}
		}
		return;
	}

	void HttpsConnection::disconnect() noexcept {
		this->socket = INVALID_SOCKET;
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

	HttpsResponseData HttpsClient::submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew) {
		RateLimitData rateLimitData{};
		auto connection = this->connectionManager.getConnection();
		auto returnData = this->httpsRequestInternal(connection, workloadNew, rateLimitData);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string errorMessage{ DiscordCoreAPI::shiftToBrightRed() };
			if (workloadNew.callStack != "") {
				errorMessage += workloadNew.callStack + " ";
			}
			errorMessage += "Https Error: " + static_cast<std::string>(returnData.responseCode) + "\nThe Request's Address: " + workloadNew.baseUrl +
				workloadNew.relativePath + "\nThe Request's Contents: " + workloadNew.content + DiscordCoreAPI::reset();
			HttpsError theError{ errorMessage };
			theError.errorCode = returnData.responseCode;
			connection->areWeCheckedOut.store(false);
			throw theError;
		}
		connection->areWeCheckedOut.store(false);
		return returnData;
	}

	HttpsResponseData HttpsClient::httpsRequest(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload) {

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

		HttpsResponseData resultData = this->executeByRateLimitData(httpsConnection, workload, rateLimitData);
		auto value = HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load();
		HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->store(value + 1);
		rateLimitData.theSemaphore.release();
		return resultData;
	}

	HttpsResponseData HttpsClient::httpsRequestInternal(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload,
		RateLimitData& rateLimitData) {
		if (workload.baseUrl == "") {
			workload.baseUrl = "https://discord.com/api/v10";
		}
		if (workload.baseUrl == "https://discord.com/api/v10") {
			workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
			workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert["Content-Type"] = "application/json";
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
			}
		}
		httpsConnection->resetValues();
		if (httpsConnection->currentReconnectTries >= httpsConnection->maxReconnectTries) {
			httpsConnection->currentReconnectTries = 0;
			httpsConnection->disconnect();
			return HttpsResponseData{};
		}
		if (workload.baseUrl != httpsConnection->currentBaseUrl || !httpsConnection->areWeStillConnected() || httpsConnection->doWeConnect) {
			httpsConnection->currentBaseUrl = workload.baseUrl;
			if (!httpsConnection->connect(workload.baseUrl, 443, this->configManager->doWePrintHttpsErrorMessages(), false)) {
				++httpsConnection->currentReconnectTries;
				httpsConnection->doWeConnect = true;
				return this->httpsRequestInternal(httpsConnection, workload, rateLimitData);
			}
			httpsConnection->doWeConnect = false;
		}
		auto request = httpsConnection->buildRequest(workload);
		DiscordCoreAPI::StopWatch stopWatch{ 5000ms };
		ProcessIOResult result{};
		do {
			if (stopWatch.hasTimePassed()) {
				break;
			}
			result = httpsConnection->writeData(request, true);
		} while (result == ProcessIOResult::Error);
		if (result == ProcessIOResult::Error) {
			++httpsConnection->currentReconnectTries;
			httpsConnection->doWeConnect = true;
			return this->httpsRequestInternal(httpsConnection, workload, rateLimitData);
		};
		auto resultNew = this->getResponse(httpsConnection, rateLimitData);
		if (resultNew.responseCode == -1) {
			++httpsConnection->currentReconnectTries;
			httpsConnection->doWeConnect = true;
			return this->httpsRequestInternal(httpsConnection, workload, rateLimitData);
		} else {
			httpsConnection->currentReconnectTries = 0;
			return resultNew;
		}
	}

	HttpsResponseData HttpsClient::executeByRateLimitData(HttpsConnection* httpsConnection, const HttpsWorkloadData& workload,
		RateLimitData& rateLimitData) {
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

		returnData = HttpsClient::httpsRequestInternal(httpsConnection, workload, rateLimitData);

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
					 << ": " << returnData.responseData << DiscordCoreAPI::reset() << endl
					 << endl;
			}
		} else {
			if (returnData.responseCode == 429) {
				double doubleVal{};
				if (returnData.responseData.size() > 0) {
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
				returnData = this->executeByRateLimitData(httpsConnection, workload, rateLimitData);
			}
		}
		return returnData;
	}

	HttpsResponseData HttpsClient::getResponse(HttpsConnection* connection, RateLimitData& rateLimitData) {
		connection->resetValues();
		ProcessIOResult result{};
		DiscordCoreAPI::StopWatch stopWatch{ 5000ms };
		while (!connection->areWeDoneTheRequest && result != ProcessIOResult::Error && !stopWatch.hasTimePassed()) {
			result = connection->processIO(1000);
			connection->handleBuffer();
		}
		auto data = connection->finalizeReturnValues(rateLimitData);
		connection->resetValues();
		return data;
	}
}