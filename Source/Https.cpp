/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
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

	HttpError::HttpError(std::string theMessage) : std::runtime_error(theMessage){};

	void HttpsRnRBuilder::updateRateLimitData(std::unordered_map<std::string, std::string>& headersNew, RateLimitData& rateLimitData) {
		if (headersNew.contains("x-ratelimit-bucket")) {
			rateLimitData.bucket = headersNew["x-ratelimit-bucket"];
		}
		if (headersNew.contains("x-ratelimit-reset-after")) {
			rateLimitData.msRemain = static_cast<int64_t>(ceil(stod(headersNew["x-ratelimit-reset-after"]))) * 1000;
		}
		if (headersNew.contains("x-ratelimit-remaining")) {
			rateLimitData.getsRemaining = static_cast<int64_t>(stoi(headersNew["x-ratelimit-remaining"])) * 1000;
			if (rateLimitData.getsRemaining <= 1) {
				rateLimitData.doWeWait = true;
			}
		}
	};

	HttpsResponseData HttpsRnRBuilder::finalizeReturnValues(RateLimitData& rateLimitData, HttpsResponseData& theData) {
		if (theData.responseMessage.size() > 0) {
			if ((theData.responseMessage[0] == '{' && theData.responseMessage[theData.responseMessage.size() - 1] == '}') ||
				(theData.responseMessage[0] == '[' && theData.responseMessage[theData.responseMessage.size() - 1] == ']')) {
				theData.responseData = nlohmann::json::parse(theData.responseMessage);
			}
		}
		this->updateRateLimitData(theData.responseHeaders, rateLimitData);
		return theData;
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
		std::string theReturnString{};
		if (workload.workloadClass == HttpsWorkloadClass::Get || workload.workloadClass == HttpsWorkloadClass::Delete) {
			if (workload.workloadClass == HttpsWorkloadClass::Get) {
				theReturnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			} else if (workload.workloadClass == HttpsWorkloadClass::Delete) {
				theReturnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			}
			for (auto& [key, value]: workload.headersToInsert) {
				theReturnString += key + ": " + value + "\r\n";
			}
			theReturnString += "Connection: Keep-Alive\r\n";
			theReturnString += "Host: " + baseUrlNew + "\r\n\r\n";
		} else {
			if (workload.workloadClass == HttpsWorkloadClass::Patch) {
				theReturnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			} else if (workload.workloadClass == HttpsWorkloadClass::Post) {
				theReturnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			} else if (workload.workloadClass == HttpsWorkloadClass::Put) {
				theReturnString = "PUT " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			}
			for (auto& [key, value]: workload.headersToInsert) {
				theReturnString += key + ": " + value + "\r\n";
			}
			theReturnString += "Connection: Keep-Alive\r\n";
			theReturnString += "Host: " + baseUrlNew + "\r\n";
			theReturnString += "Content-Length: " + std::to_string(workload.content.size()) + "\r\n\r\n";
			theReturnString += workload.content;
		}
		return theReturnString;
	}

	void HttpsRnRBuilder::parseHeaders(std::string& other, HttpsResponseData& theData) {
		try {
			if (other.find("\r\n\r\n") != std::string::npos) {
				if (theData.responseCode == -1) {
					this->parseCode(other, theData);
				}
				std::string newString{};
				newString.insert(newString.begin(), other.begin(), other.begin() + other.find("\r\n\r\n") + std::string("\r\n\r\n").size());
				other.erase(other.begin(), other.begin() + newString.size());
				while (newString.size() > 0 && newString.find(":") != std::string::npos && newString.find("\r\n") != std::string::npos) {
					int64_t currentOffset{ 0 };
					std::string lineString = newString.substr(0, newString.find("\r\n") + 2);
					currentOffset = lineString.size();
					std::string key = lineString.substr(0, lineString.find(":"));
					std::string value = lineString.substr(lineString.find(":") + 2, lineString.size() - key.size() - 2);
					value = value.substr(0, value.find("\r\n"));
					theData.responseHeaders[key] = value;
					std::string tempString{};
					tempString.insert(tempString.begin(), newString.begin() + currentOffset, newString.end());
					newString = tempString;
				}
				if (theData.responseHeaders.contains("Transfer-Encoding") && theData.responseHeaders["Transfer-Encoding"] == "chunked") {
					this->isItChunked = true;
				}
				this->doWeHaveHeaders = true;
				theData.theCurrentState = HttpsState::Collecting_Contents;
			} else {
			}
		} catch (...) {
			theData.contentSize = -5;
		}
	}

	bool HttpsRnRBuilder::parseChunk(std::string& other, HttpsResponseData& theData) {
		if (this->doWeHaveHeaders) {
			if (this->isItChunked) {
				if (other.find("\r\n0\r\n\r\n") != std::string::npos) {
					while (other.find("\r\n") != other.find("\r\n0\r\n\r\n")) {
						this->clearCRLF(other);
						this->parseSize(other, theData);
						this->clearCRLF(other);
						if (other.find("\r\n") != other.find("\r\n0\r\n\r\n")) {
							theData.responseMessage.insert(theData.responseMessage.end(), other.begin(), other.begin() + other.find("\r\n"));
							other.erase(other.begin(), other.begin() + other.find("\r\n") + 2);
						}
					}
					theData.responseMessage.insert(theData.responseMessage.end(), other.begin(), other.begin() + other.find("\r\n0\r\n\r\n"));
					return false;
				} else {
					return true;
				}
			} else {
				if (!this->doWeHaveContentSize) {
					this->parseSize(other, theData);
				}
				if (theData.contentSize == 0) {
					return false;
				}
				if (other.size() >= static_cast<size_t>(theData.contentSize)) {
					theData.responseMessage.insert(theData.responseMessage.end(), other.begin(), other.begin() + theData.contentSize);
					return false;
				} else {
					return true;
				}
			}
		} else {
			return true;
		}
	}

	bool HttpsRnRBuilder::checkForHeadersToParse(const std::string& other) {
		if (other.find("HTTP/1.") != std::string::npos) {
			return true;
		} else {
			return false;
		}
	}

	void HttpsRnRBuilder::resetValues() {
		this->doWeHaveContentSize = false;
		this->inputBufferReal.clear();
		this->doWeHaveHeaders = false;
		this->isItChunked = false;
	}

	void HttpsRnRBuilder::parseSize(std::string& other, HttpsResponseData& theData) {
		try {
			if (theData.responseHeaders.contains("Content-Length")) {
				theData.contentSize = stoll(theData.responseHeaders["Content-Length"]);
				this->doWeHaveContentSize = true;
				theData.theCurrentState = HttpsState::Collecting_Contents;
				return;
			}
			std::string theValueString{};
			uint64_t hexIndex{ 0 };
			bool isThereHexValues{ false };
			for (uint64_t x = 0; x < other.size(); x++) {
				if (isxdigit(other[x]) != 0 && static_cast<int32_t>(other[x]) != EOF) {
					isThereHexValues = true;
					theValueString.push_back(other[x]);
				} else {
					hexIndex = x;
					break;
				}
			}
			if (!isThereHexValues) {
				theData.contentSize = 0;
				return;
			} else {
				theData.contentSize += stoll(theValueString, nullptr, 16);
				other.erase(other.begin(), other.begin() + hexIndex);
				this->doWeHaveContentSize = true;
				theData.theCurrentState = HttpsState::Collecting_Contents;
			}
		} catch (...) {
			theData.contentSize = -5;
		}
	}

	void HttpsRnRBuilder::parseCode(std::string& otherNew, HttpsResponseData& theData) {
		std::string other = otherNew;
		if (other.find("HTTP/1.") != std::string::npos) {
			uint64_t firstNumberIndex{ 0 };
			uint64_t lastNumberIndex{ 0 };
			bool haveWeStarted{ false };
			for (size_t x = other.find("HTTP/1.") + std::string("HTTP/1.").size() + 1; x < other.size(); x++) {
				if (!haveWeStarted && (isalnum(static_cast<unsigned char>(other[x])) != 0)) {
					firstNumberIndex = x;
					haveWeStarted = true;
				} else if (haveWeStarted && (isalnum(static_cast<unsigned char>(other[x])) == 0)) {
					lastNumberIndex = x;
					break;
				}
			}
			theData.responseCode = stoll(other.substr(firstNumberIndex, lastNumberIndex - firstNumberIndex));
			theData.theCurrentState = HttpsState::Collecting_Headers;
		} else if (other.size() <= 5) {
		} else {
			theData.responseCode = -5;
		}
	}

	void HttpsRnRBuilder::clearCRLF(std::string& other) {
		uint64_t theCount{ 0 };
		for (uint64_t x = 0; x < other.size(); x++) {
			if (isspace(static_cast<unsigned char>(other[x])) != 0) {
				theCount++;
			} else {
				break;
			}
		}
		other.erase(other.begin(), other.begin() + theCount);
	}

	std::unordered_map<std::string, std::unique_ptr<RateLimitData>>& HttpsConnectionManager::getRateLimitValues() {
		return this->rateLimitValues;
	}

	std::unordered_map<HttpsWorkloadType, std::string>& HttpsConnectionManager::getRateLimitValueBuckets() {
		return this->rateLimitValueBuckets;
	}

	HttpsConnection* HttpsConnectionManager::getConnection() {
		std::lock_guard theLock{ this->theMutex };
		for (auto& [key, value]: this->httpsConnections) {
			if (!value->areWeCheckedOut.load()) {
				value->areWeCheckedOut.store(true);
				return value.get();
			}
		}
		this->currentIndex++;
		this->httpsConnections[this->currentIndex] = std::make_unique<HttpsConnection>();
		return this->httpsConnections[this->currentIndex].get();
	}

	void HttpsConnectionManager::initialize() {
		for (int64_t enumOne = static_cast<int64_t>(HttpsWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpsWorkloadType::LAST); enumOne++) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			rateLimitData->tempBucket = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			this->getRateLimitValueBuckets()[static_cast<HttpsWorkloadType>(enumOne)] = rateLimitData->tempBucket;
			this->getRateLimitValues()[rateLimitData->tempBucket] = std::move(rateLimitData);
			std::this_thread::sleep_for(1ms);
		}
	}

	HttpsClient::HttpsClient(DiscordCoreAPI::ConfigManager* configManagerNew) : configManager(configManagerNew) {
		this->connectionManager.initialize();
	};

	template<> void HttpsClient::submitWorkloadAndGetResult<void>(HttpsWorkloadData& workloadNew) {
		HttpsWorkloadData workload = workloadNew;
		workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
		workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
		if (workload.payloadType == PayloadType::Application_Json) {
			workload.headersToInsert["Content-Type"] = "application/json";
		} else if (workload.payloadType == PayloadType::Multipart_Form) {
			workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
		}
		auto returnData = this->httpRequest(workload);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workloadNew.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
				", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
			HttpError theError{ theErrorMessage };
			theError.errorCode = returnData.responseCode;
			throw theError;
		}
		return;
	}

	HttpsResponseData HttpsClient::submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew) {
		RateLimitData rateLimitData{};
		auto returnData = this->httpRequestInternal(workloadNew, rateLimitData);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workloadNew.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
				", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
			HttpError theError{ theErrorMessage };
			theError.errorCode = returnData.responseCode;
			throw theError;
		}
		return returnData;
	}

	HttpsResponseData HttpsClient::httpRequest(HttpsWorkloadData& workload) {
		if (workload.baseUrl == "") {
			workload.baseUrl = "https://discord.com/api/v10";
		}
		RateLimitData& rateLimitData = *this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]].get();
		if (!rateLimitData.haveWeGoneYet) {
			std::this_thread::sleep_for(100ms);
			rateLimitData.haveWeGoneYet = true;
		}
		while (HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load() < workload.thisWorkerId.load() && workload.thisWorkerId.load() != 0) {
			std::this_thread::sleep_for(1ms);
		}

		while (!rateLimitData.theSemaphore.try_acquire()) {
			std::this_thread::sleep_for(1ms);
		}

		HttpsResponseData resultData = this->executeByRateLimitData(workload);
		auto theValue = HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load();
		HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->store(theValue + 1);
		rateLimitData.theSemaphore.release();
		return resultData;
	}

	void HttpsConnection::disconnect(bool) noexcept {
		if (this->theSSLState.load() == SSLConnectionState::Connected) {
			std::unique_lock theLock{ this->connectionMutex };
			std::unique_lock theLock02{ this->readMutex };
			std::unique_lock theLock03{ this->writeMutex };
			this->theSSLState.store(SSLConnectionState::Disconnected);
			this->theSocket = SOCKET_ERROR;
			this->inputBuffer.clear();
			this->outputBuffers.clear();
		}
	}

	HttpsResponseData HttpsClient::httpRequestInternal(const HttpsWorkloadData& workload, RateLimitData& rateLimitData) {
		auto httpsConnection = this->connectionManager.getConnection();
		try {
			httpsConnection->resetValues();
			if (httpsConnection->currentReconnectTries >= httpsConnection->maxReconnectTries) {
				httpsConnection->currentReconnectTries = 0;
				httpsConnection->areWeCheckedOut.store(false);
				return HttpsResponseData{};
			}
			if (workload.baseUrl != httpsConnection->currentBaseUrl || !httpsConnection->areWeStillConnected() || httpsConnection->doWeConnect) {
				httpsConnection->currentBaseUrl = workload.baseUrl;
				if (!httpsConnection->connect(workload.baseUrl, "443")) {
					httpsConnection->currentReconnectTries++;
					httpsConnection->doWeConnect = true;
					httpsConnection->areWeCheckedOut.store(false);
					return this->httpRequestInternal(workload, rateLimitData);
				}
				httpsConnection->doWeConnect = false;
			}
			auto theRequest = httpsConnection->buildRequest(workload);
			if (!this->sendData(theRequest, true, *httpsConnection)) {
				httpsConnection->currentReconnectTries++;
				httpsConnection->doWeConnect = true;
				httpsConnection->areWeCheckedOut.store(false);
				return this->httpRequestInternal(workload, rateLimitData);
			};
			auto result = this->getResponse(rateLimitData, *httpsConnection);
			if (result.responseCode == -1) {
				httpsConnection->currentReconnectTries++;
				httpsConnection->doWeConnect = true;
				httpsConnection->areWeCheckedOut.store(false);
				return this->httpRequestInternal(workload, rateLimitData);
			} else {
				httpsConnection->currentReconnectTries = 0;
				httpsConnection->areWeCheckedOut.store(false);
				return result;
			}
		} catch (...) {
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpsClient::httpRequestInternal()");
			}
			httpsConnection->currentReconnectTries++;
			httpsConnection->doWeConnect = true;
			httpsConnection->areWeCheckedOut.store(false);
			return this->httpRequestInternal(workload, rateLimitData);
		}
	}

	HttpsResponseData HttpsClient::getResponse(RateLimitData& rateLimitData, HttpsConnection& theConnection) {
		DiscordCoreAPI::StopWatch stopWatch{ 5500ms };
		theConnection.getInputBuffer().clear();
		theConnection.resetValues();
		HttpsResponseData theData{};
		while (true) {
			theConnection.processIO(10000);
			std::string theString = theConnection.getInputBuffer();
			if (theString.size() > 0) {
				theConnection.inputBufferReal.insert(theConnection.inputBufferReal.end(), theString.begin(), theString.end());
			}
			bool doWeBreak{ false };
			switch (theData.theCurrentState) {
				case HttpsState::Collecting_Code: {
					if (stopWatch.hasTimePassed()) {
						doWeBreak = true;
						break;
					}
					theConnection.parseCode(theConnection.inputBufferReal, theData);
					break;
				}
				case HttpsState::Collecting_Headers: {
					if (stopWatch.hasTimePassed()) {
						doWeBreak = true;
						break;
					}
					if (theConnection.checkForHeadersToParse(theConnection.inputBufferReal) && !theConnection.doWeHaveHeaders) {
						theConnection.parseHeaders(theConnection.inputBufferReal, theData);
						stopWatch.resetTimer();
					}
					break;
				}
				case HttpsState::Collecting_Size: {
					if (stopWatch.hasTimePassed()) {
						doWeBreak = true;
						break;
					}
					if (!theConnection.doWeHaveContentSize) {
						theConnection.clearCRLF(theConnection.inputBufferReal);
						theConnection.parseSize(theConnection.inputBufferReal, theData);
						theConnection.clearCRLF(theConnection.inputBufferReal);
						stopWatch.resetTimer();
					}
					break;
				}
				case HttpsState::Collecting_Contents: {
					if (static_cast<int64_t>(theConnection.inputBufferReal.size()) >= theData.contentSize && !theConnection.parseChunk(theConnection.inputBufferReal, theData) ||
						stopWatch.hasTimePassed() || (theData.responseCode == -5 && theData.contentSize == -5)) {
						doWeBreak = true;
						break;
					}
				}
			}
			if (doWeBreak) {
				break;
			}
		};
		return theConnection.finalizeReturnValues(rateLimitData, theData);
	}

	bool HttpsClient::sendData(const std::string& dataToSend, bool priority, HttpsConnection& theConnection) {
		try {
			if (theConnection.areWeStillConnected()) {
				DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
				bool didWeWrite{ false };
				do {
					if (theStopWatch.hasTimePassed()) {
						return false;
					}
					didWeWrite = theConnection.writeData(dataToSend, priority);
				} while (!didWeWrite);
				return true;
			} else {
				return false;
			}
		} catch (...) {
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendMessage()");
			}
			return false;
		}
	}

	HttpsResponseData HttpsClient::executeByRateLimitData(const HttpsWorkloadData& workload) {
		HttpsResponseData returnData{};
		RateLimitData& rateLimitData = *this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]].get();
		int64_t timeRemaining{};
		int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		if (workload.workloadType == HttpsWorkloadType::Delete_Message_Old) {
			rateLimitData.msRemain = 4000;
		} else if (workload.workloadType == HttpsWorkloadType::Delete_Message || workload.workloadType == HttpsWorkloadType::Patch_Message) {
			rateLimitData.areWeASpecialBucket = true;
		}
		if (rateLimitData.areWeASpecialBucket) {
			rateLimitData.msRemain = static_cast<int64_t>(ceil(static_cast<float>(5000) / static_cast<float>(4)));
			int64_t targetTime = rateLimitData.msRemain + rateLimitData.sampledTimeInMs;
			timeRemaining = targetTime - currentTime;
		} else if (rateLimitData.didWeHitRateLimit) {
			int64_t targetTime = rateLimitData.sampledTimeInMs + rateLimitData.msRemain;
			timeRemaining = targetTime - currentTime;
			rateLimitData.didWeHitRateLimit = false;
		} else if (rateLimitData.doWeWait) {
			int64_t targetTime = rateLimitData.sampledTimeInMs + rateLimitData.msRemain;
			timeRemaining = targetTime - currentTime;
			rateLimitData.doWeWait = false;
		}
		if (timeRemaining > 0) {
			if (this->configManager->doWePrintHttpsSuccessMessages()) {
				std::cout << DiscordCoreAPI::shiftToBrightBlue() << "We're waiting on rate-limit: " << timeRemaining << DiscordCoreAPI::reset() << std::endl << std::endl;
			}
			int64_t targetTime = currentTime + timeRemaining;
			while (targetTime > currentTime) {
				currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				timeRemaining = targetTime - currentTime;
				if (timeRemaining <= 20) {
				} else {
					std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int64_t>(ceil(timeRemaining * 80.0f / 100.0f)) });
				}
			}
		}
		rateLimitData.sampledTimeInMs = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		returnData = HttpsClient::httpRequestInternal(workload, rateLimitData);

		if (rateLimitData.tempBucket != "") {
			rateLimitData.tempBucket = "";
		}
		std::string currentBucket = rateLimitData.bucket;
		if (!this->connectionManager.getRateLimitValues().contains(rateLimitData.bucket)) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]].swap(rateLimitData);
			this->connectionManager.getRateLimitValueBuckets()[workload.workloadType] = currentBucket;
			this->connectionManager.getRateLimitValues()[currentBucket] = std::move(rateLimitData);
		}
		if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
			if (this->configManager->doWePrintHttpsSuccessMessages()) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << workload.callStack + " Success: " << returnData.responseCode << ", " << returnData.responseMessage
						  << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
		} else {
			if (returnData.responseCode == 429) {
				rateLimitData.msRemain = static_cast<int64_t>(ceil(nlohmann::json::parse(returnData.responseMessage)["retry_after"].get<double>())) * 1000;
				rateLimitData.didWeHitRateLimit = true;
				rateLimitData.sampledTimeInMs =
					static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				if (this->configManager->doWePrintHttpsErrorMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << workload.callStack + "::httpRequest(), We've hit rate limit! Time Remaining: "
							  << std::to_string(this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]]->msRemain)
							  << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				returnData = this->executeByRateLimitData(workload);
			}
		}
		return returnData;
	}

}