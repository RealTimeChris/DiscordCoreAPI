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

	HttpError::HttpError(std::string theMessage) : std::runtime_error(theMessage){};

	HttpsRnRBuilder::HttpsRnRBuilder(bool doWePrintErrorMessagesNew) {
		this->doWePrintErrorMessages = doWePrintErrorMessagesNew;
	}

	void HttpsRnRBuilder::updateRateLimitData(RateLimitData& rateLimitData, std::unordered_map<std::string, std::string>& headersNew) {
		if (headersNew.contains("x-ratelimit-bucket")) {
			rateLimitData.bucket = headersNew["x-ratelimit-bucket"];
		}
		if (headersNew.contains("x-ratelimit-reset-after")) {
			rateLimitData.msRemain.store(static_cast<int64_t>(ceil(stod(headersNew["x-ratelimit-reset-after"])) * 1000.0f));
		}
		if (headersNew.contains("x-ratelimit-remaining")) {
			rateLimitData.getsRemaining.store(static_cast<int64_t>(stoi(headersNew["x-ratelimit-remaining"])));
		}
		if (rateLimitData.getsRemaining.load() <= 1 || rateLimitData.areWeASpecialBucket.load()) {
			rateLimitData.doWeWait.store(true);
		}
	};

	HttpsResponseData HttpsRnRBuilder::finalizeReturnValues(HttpsResponseData& theData, RateLimitData& rateLimitData) {
		try {
			if (theData.responseMessage.size() >= theData.contentSize && theData.contentSize > 0) {
				if ((theData.responseMessage[0] == '{' && theData.responseMessage[theData.contentSize - 1] == '}') ||
					(theData.responseMessage[0] == '[' && theData.responseMessage[theData.contentSize - 1] == ']')) {
					theData.responseData = theData.responseMessage.substr(0, theData.contentSize);
				}
			}
		} catch (...) {
			if (this->doWePrintErrorMessages) {
				DiscordCoreAPI::reportException("HttpsRnRBuilder::finalizeReturnValues()");
			}
		}
		this->updateRateLimitData(rateLimitData, theData.responseHeaders);
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

	size_t HttpsRnRBuilder::parseHeaders(HttpsResponseData& theData, std::string& other) {
		try {
			if (other.find("\r\n\r\n") != std::string::npos) {
				DiscordCoreAPI::StopWatch theStopWatch{ 1500ms };
				std::string newString{};
				newString.insert(newString.begin(), other.begin(), other.begin() + other.find("\r\n\r\n") + std::string("\r\n\r\n").size());
				other.erase(other.begin(), other.begin() + newString.size());
				while (newString.size() > 0 && newString.find(":") != std::string::npos && newString.find("\r\n") != std::string::npos) {
					std::cout << "HTTPS CLIENT LOOP 000" << std::endl;
					if (theStopWatch.hasTimePassed()) {
						break;
					}
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

				if (theData.responseHeaders.contains("Transfer-Encoding") && theData.responseHeaders["Transfer-Encoding"] == "chunked" ||
					theData.responseHeaders.contains("transfer-encoding") && theData.responseHeaders["transfer-encoding"] == "chunked") {
					this->isItChunked = true;
				}
				this->doWeHaveHeaders = true;
				theData.theCurrentState = HttpsState::Collecting_Size;
				return newString.size();
			}
		} catch (...) {
			theData.contentSize = -5;
			return 0;
		}
		return 0;
	}

	bool HttpsRnRBuilder::parseChunk(HttpsResponseData& theData, std::string& other) {
		if (this->isItChunked) {
			if (other.find("\r\n0\r\n\r\n") != std::string::npos) {
				DiscordCoreAPI::StopWatch theStopWatch{ 1500ms };
				while (other.find("\r\n") != other.find("\r\n0\r\n\r\n")) {
					std::cout << "HTTPS CLIENT LOOP 001" << std::endl;
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					this->clearCRLF(other);
					this->parseSize(theData, other);
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
				this->parseSize(theData, other);
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
	}

	size_t HttpsRnRBuilder::parseSize(HttpsResponseData& theData, std::string& other) {
		try {
			if (theData.responseHeaders.contains("Content-Length")) {
				theData.contentSize = stoll(theData.responseHeaders["Content-Length"]);
				this->doWeHaveContentSize = true;
				theData.theCurrentState = HttpsState::Collecting_Contents;
				return 0;
			}
			std::string theValueString{};
			uint64_t hexIndex{ 0 };
			bool isThereHexValues{ false };
			for (uint64_t x = 0; x < other.size(); ++x) {
				if (isxdigit(other[x]) != 0 && static_cast<int32_t>(other[x]) != EOF) {
					isThereHexValues = true;
					theValueString.push_back(other[x]);
				} else {
					hexIndex = x;
					break;
				}
			}
			if (!isThereHexValues) {
				theData.contentSize += 0;
				return 0;
			} else {
				theData.contentSize += stoll(theValueString, nullptr, 16);
				other.erase(other.begin(), other.begin() + hexIndex);
				this->doWeHaveContentSize = true;
				theData.theCurrentState = HttpsState::Collecting_Contents;
				return hexIndex;
			}
		} catch (...) {
			theData.contentSize = -5;
			return 0;
		}
	}

	size_t HttpsRnRBuilder::parseCode(HttpsResponseData& theData, std::string& otherNew) {
		if (otherNew.find("HTTP/1.") != std::string::npos) {
			uint64_t firstNumberIndex{ 0 };
			uint64_t lastNumberIndex{ 0 };
			bool haveWeStarted{ false };
			for (size_t x = otherNew.find("HTTP/1.") + std::string("HTTP/1.").size() + 1; x < otherNew.size(); ++x) {
				if (!haveWeStarted && (isalnum(static_cast<uint8_t>(otherNew[x])) != 0)) {
					firstNumberIndex = x;
					haveWeStarted = true;
				} else if (haveWeStarted && (isalnum(static_cast<uint8_t>(otherNew[x])) == 0)) {
					lastNumberIndex = x;
					break;
				}
			}
			theData.responseCode = stoll(otherNew.substr(firstNumberIndex, lastNumberIndex - firstNumberIndex));
			otherNew.erase(otherNew.begin(), otherNew.begin() + otherNew.find("\r\n"));
			theData.theCurrentState = HttpsState::Collecting_Headers;
			return otherNew.find("\r\n");
		} else if (otherNew.size() > 200 && otherNew.find("HTTP/1.") == std::string::npos) {
			theData.responseCode = 200;
			theData.theCurrentState = HttpsState::Collecting_Contents;
			return 0;
		}
		return 0;
	}

	void HttpsRnRBuilder::clearCRLF(std::string& other) {
		uint64_t theCount{ 0 };
		for (uint64_t x = 0; x < other.size(); ++x) {
			if (isspace(static_cast<uint8_t>(other[x])) != 0) {
				theCount++;
			} else {
				break;
			}
		}
		other.erase(other.begin(), other.begin() + theCount);
	}

	HttpsConnection::HttpsConnection(bool doWePrintErrorMessages) : HttpsRnRBuilder(doWePrintErrorMessages){};

	bool HttpsConnection::handleBuffer(SSLClient* theClient) noexcept {
		auto theConnection = static_cast<HttpsConnection*>(theClient);
		this->theData.theStopWatch.resetTimer();
		while (true) {
			std::cout << "HTTPS CLIENT LOOP 002" << std::endl;
			switch (this->theData.theCurrentState) {
				case HttpsState::Collecting_Code: {
					if (this->theData.theStopWatch.hasTimePassed()) {
						this->areWeDoneTheRequest = true;
						return false;
					}
					theConnection->theInputBufferReal += static_cast<std::string>(theConnection->getInputBufferRemove());
					theConnection->parseCode(this->theData, theConnection->theInputBufferReal);
					this->theData.theStopWatch.resetTimer();
					if (this->theData.responseCode == 204) {
						this->areWeDoneTheRequest = true;
						return false;
					}
					return false;
				}
				case HttpsState::Collecting_Headers: {
					if (this->theData.theStopWatch.hasTimePassed()) {
						this->areWeDoneTheRequest = true;
						return false;
					}
					if (!theConnection->doWeHaveHeaders) {
						theConnection->theInputBufferReal += static_cast<std::string>(theConnection->getInputBufferRemove());
						theConnection->parseHeaders(this->theData, theConnection->theInputBufferReal);
						this->theData.theStopWatch.resetTimer();
					}
					return false;
				}
				case HttpsState::Collecting_Size: {
					if (this->theData.theStopWatch.hasTimePassed()) {
						this->areWeDoneTheRequest = true;
						return false;
					}
					if (!theConnection->doWeHaveContentSize) {
						theConnection->theInputBufferReal += static_cast<std::string>(theConnection->getInputBufferRemove());
						theConnection->clearCRLF(theConnection->theInputBufferReal);
						theConnection->parseSize(this->theData, theConnection->theInputBufferReal);
						theConnection->clearCRLF(theConnection->theInputBufferReal);
						this->theData.theStopWatch.resetTimer();
					}
					return false;
				}
				case HttpsState::Collecting_Contents: {
					theConnection->theInputBufferReal += static_cast<std::string>(theConnection->getInputBufferRemove());
					auto theResult = theConnection->parseChunk(this->theData, theConnection->theInputBufferReal);
					if ((this->theData.responseMessage.size() >= this->theData.contentSize && !theResult) || this->theData.theStopWatch.hasTimePassed() || !theResult ||
						(this->theData.responseCode == -5 && this->theData.contentSize == -5)) {
						this->areWeDoneTheRequest = true;
						return false;
					} else {
						this->theData.theStopWatch.resetTimer();
					}
					return false;
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		return false;
	}

	void HttpsConnection::disconnect(bool) noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			this->theSocket = SOCKET_ERROR;
		}
		this->resetValues();
	}

	void HttpsConnection::resetValues() {
		SSLDataInterface::maxBufferSize = (1024 * 16) - 1;
		SSLDataInterface::outputBuffers.clear();
		this->theInputBufferReal.clear();
		this->inputBuffer.clear();
		this->theData = HttpsResponseData{};
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
		std::unique_lock theLock{ this->theMutex };
		for (auto& [key, value]: this->httpsConnections) {
			if (!value->areWeCheckedOut.load()) {
				value->areWeCheckedOut.store(true);
				value->resetValues();
				return value.get();
			}
		}
		this->currentIndex++;
		this->httpsConnections[this->currentIndex] = std::make_unique<HttpsConnection>(this->configManager->doWePrintHttpsErrorMessages());
		return this->httpsConnections[this->currentIndex].get();
	}

	void HttpsConnectionManager::initialize() {
		for (int64_t enumOne = static_cast<int64_t>(HttpsWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpsWorkloadType::LAST); enumOne++) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			rateLimitData->tempBucket = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
			this->getRateLimitValueBuckets()[static_cast<HttpsWorkloadType>(enumOne)] = rateLimitData->tempBucket;
			this->getRateLimitValues()[rateLimitData->tempBucket] = std::move(rateLimitData);
			std::this_thread::sleep_for(1ms);
		}
	}

	HttpsClient::HttpsClient(DiscordCoreAPI::ConfigManager* configManagerNew) : configManager(configManagerNew), connectionManager(configManagerNew) {
		this->connectionManager.initialize();
	};

	template<> void HttpsClient::submitWorkloadAndGetResult<void>(const HttpsWorkloadData& workload, void* theReturnValue) {
		workload.headersToInsert["Authorization"] = "Bot " + this->configManager->getBotToken();
		workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
		if (workload.payloadType == PayloadType::Application_Json) {
			workload.headersToInsert["Content-Type"] = "application/json";
		} else if (workload.payloadType == PayloadType::Multipart_Form) {
			workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
		}
		auto returnData = this->httpRequest(workload);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workload.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
				", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
			HttpError theError{ theErrorMessage };
			theError.errorCode = returnData.responseCode;
			throw theError;
		}
		return;
	}

	HttpsResponseData HttpsClient::submitWorkloadAndGetResult(const HttpsWorkloadData& workloadNew) {
		RateLimitData rateLimitData{};
		auto theConnection = this->connectionManager.getConnection();
		auto returnData = this->httpRequestInternal(*theConnection, workloadNew, rateLimitData);
		theConnection->areWeCheckedOut.store(false);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			std::string theErrorMessage{ DiscordCoreAPI::shiftToBrightRed() + workloadNew.callStack + " Https Error; Code: " + std::to_string(returnData.responseCode) +
				", Message: " + returnData.responseMessage + DiscordCoreAPI::reset() + "\n\n" };
			HttpError theError{ theErrorMessage };
			theError.errorCode = returnData.responseCode;
			throw theError;
		}
		return returnData;
	}

	HttpsResponseData HttpsClient::httpRequest(const HttpsWorkloadData& workload) {
		if (workload.baseUrl == "") {
			workload.baseUrl = "https://discord.com/api/v10";
		}

		RateLimitData& rateLimitData = *this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]].get();
		if (!rateLimitData.haveWeGoneYet.load()) {
			std::this_thread::sleep_for(100ms);
			rateLimitData.haveWeGoneYet.store(true);
		}
		while (HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load() < workload.thisWorkerId.load() && workload.thisWorkerId.load() != 0) {
			std::cout << "HTTPS CLIENT LOOP 003" << std::endl;
			std::this_thread::sleep_for(1ms);
		}

		while (!rateLimitData.theSemaphore.try_acquire()) {
			std::cout << "HTTPS CLIENT LOOP 004" << std::endl;
			std::this_thread::sleep_for(1ms);
		}

		HttpsResponseData resultData = this->executeByRateLimitData(workload, rateLimitData);
		auto theValue = HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->load();
		HttpsWorkloadData::workloadIdsInternal[workload.workloadType]->store(theValue + 1);
		rateLimitData.theSemaphore.release();
		return resultData;
	}

	HttpsResponseData HttpsClient::httpRequestInternal(HttpsConnection& httpsConnection, const HttpsWorkloadData& workload, RateLimitData& rateLimitData) {
		try {
			httpsConnection.resetValues();
			if (httpsConnection.currentReconnectTries >= httpsConnection.maxReconnectTries) {
				httpsConnection.currentReconnectTries = 0;
				httpsConnection.disconnect(true);
				return HttpsResponseData{};
			}
			if (workload.baseUrl != httpsConnection.currentBaseUrl || !httpsConnection.areWeStillConnected() || httpsConnection.doWeConnect) {
				httpsConnection.currentBaseUrl = workload.baseUrl;
				if (!httpsConnection.connect(workload.baseUrl, "443", this->configManager->doWePrintHttpsErrorMessages(), false)) {
					httpsConnection.currentReconnectTries++;
					httpsConnection.doWeConnect = true;
					return this->httpRequestInternal(httpsConnection, workload, rateLimitData);
				}
				httpsConnection.doWeConnect = false;
			}
			auto theRequest = httpsConnection.buildRequest(workload);
			DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
			ProcessIOResult theResult{};
			do {
				if (theStopWatch.hasTimePassed()) {
					break;
				}
				theResult = httpsConnection.writeData(theRequest, true);
				std::cout << "HTTPS CLIENT LOOP 005" << std::endl;
			} while (theResult == ProcessIOResult::Error);
			if (theResult != ProcessIOResult::No_Error) {
				httpsConnection.currentReconnectTries++;
				httpsConnection.doWeConnect = true;
				return this->httpRequestInternal(httpsConnection, workload, rateLimitData);
			};
			auto result = this->getResponse(httpsConnection, rateLimitData);
			if (result.responseCode == -1) {
				httpsConnection.currentReconnectTries++;
				httpsConnection.doWeConnect = true;
				return this->httpRequestInternal(httpsConnection, workload, rateLimitData);
			} else {
				httpsConnection.currentReconnectTries = 0;
				return result;
			}
		} catch (...) {
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpsClient::httpRequestInternal()");
			}
			httpsConnection.currentReconnectTries++;
			httpsConnection.doWeConnect = true;
			return this->httpRequestInternal(httpsConnection, workload, rateLimitData);
		}
	}

	HttpsResponseData HttpsClient::executeByRateLimitData(const HttpsWorkloadData& workload, RateLimitData& rateLimitData) {
		HttpsResponseData returnData{};
		try {
			int64_t timeRemaining{};
			int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
			if (workload.workloadType == HttpsWorkloadType::Delete_Message_Old) {
				rateLimitData.msRemain.store(4000);
			} else if (workload.workloadType == HttpsWorkloadType::Delete_Message || workload.workloadType == HttpsWorkloadType::Patch_Message) {
				rateLimitData.areWeASpecialBucket.store(true);
			}
			if (rateLimitData.areWeASpecialBucket.load()) {
				rateLimitData.msRemain.store(static_cast<int64_t>(ceil(5000.0f / 4.0f)));
				int64_t targetTime = rateLimitData.sampledTimeInMs.load() + rateLimitData.msRemain.load();
				timeRemaining = targetTime - currentTime;
			} else if (rateLimitData.didWeHitRateLimit.load()) {
				int64_t targetTime = rateLimitData.sampledTimeInMs.load() + rateLimitData.msRemain.load();
				timeRemaining = targetTime - currentTime;
				rateLimitData.didWeHitRateLimit.store(false);
			} else if (rateLimitData.doWeWait.load()) {
				int64_t targetTime = rateLimitData.sampledTimeInMs.load() + rateLimitData.msRemain.load();
				timeRemaining = targetTime - currentTime;
				rateLimitData.doWeWait.store(false);
			}
			if (timeRemaining > 0) {
				if (this->configManager->doWePrintHttpsSuccessMessages()) {
					cout << DiscordCoreAPI::shiftToBrightBlue() << "We're waiting on rate-limit: " << timeRemaining << DiscordCoreAPI::reset() << endl << endl;
				}
				int64_t targetTime = currentTime + timeRemaining;
				while (targetTime > currentTime) {
					std::cout << "HTTPS CLIENT LOOP 006" << std::endl;
					currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
					timeRemaining = targetTime - currentTime;
					if (timeRemaining <= 20) {
					} else {
						std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int64_t>(ceil(static_cast<float>(timeRemaining) * 80.0f / 100.0f)) });
					}
				}
			}
			auto httpsConnection = this->connectionManager.getConnection();
			returnData = HttpsClient::httpRequestInternal(*httpsConnection, workload, rateLimitData);

			httpsConnection->areWeCheckedOut.store(false);
			rateLimitData.sampledTimeInMs.store(
				static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()));

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
					cout << DiscordCoreAPI::shiftToBrightGreen() << workload.callStack + " Success: " << returnData.responseCode << ", " << returnData.responseMessage
						 << DiscordCoreAPI::reset() << endl
						 << endl;
				}
			} else {
				if (returnData.responseCode == 429) {
					simdjson::ondemand::parser theParser{};
					returnData.responseMessage.reserve(returnData.responseMessage.size() + simdjson::SIMDJSON_PADDING);
					auto theDocument = theParser.iterate(returnData.responseMessage.data(), returnData.responseMessage.length(), returnData.responseMessage.capacity());
					double theDouble{};
					if (returnData.responseMessage.size() > 0 && theDocument["retry_after"].get(theDouble) == simdjson::error_code::SUCCESS) {
						rateLimitData.msRemain.store(static_cast<int64_t>(ceil(theDouble)) * 1000);
					}
					
					rateLimitData.didWeHitRateLimit.store(true);
					rateLimitData.sampledTimeInMs.store(
						static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()));
					if (this->configManager->doWePrintHttpsErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << workload.callStack + "::httpRequest(), We've hit rate limit! Time Remaining: "
							 << std::to_string(this->connectionManager.getRateLimitValues()[this->connectionManager.getRateLimitValueBuckets()[workload.workloadType]]->msRemain)
							 << DiscordCoreAPI::reset() << endl
							 << endl;
					}
					returnData = this->executeByRateLimitData(workload, rateLimitData);
				}
			}
			return returnData;
		} catch (...) {
			if (this->configManager->doWePrintHttpsErrorMessages()) {
				DiscordCoreAPI::reportException("HttpsClient::executeByRateLimitData()");
			}
			return returnData;
		}
	}

	HttpsResponseData HttpsClient::getResponse(HttpsConnection& theConnection, RateLimitData& rateLimitData) {
		try {
			theConnection.resetValues();
			ProcessIOResult theResult{};
			DiscordCoreAPI::StopWatch theStopWatch{ 5000ms };
			while (!theConnection.areWeDoneTheRequest && theResult != ProcessIOResult::Error) {
				std::cout << "HTTPS CLIENT LOOP 007" << std::endl;
				theResult = theConnection.processIO(1);
				if (theStopWatch.hasTimePassed()) {
					break;
				}
			}
			return theConnection.finalizeReturnValues(theConnection.theData, rateLimitData);
		} catch (...) {
			DiscordCoreAPI::reportException("httpsclient::getReturnData()");
			theConnection.theData.responseCode = -1;
			return theConnection.finalizeReturnValues(theConnection.theData, rateLimitData);
		}
	}
}