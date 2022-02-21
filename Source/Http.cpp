/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// Http.cpp - Source file for the Http class DiscordCoreAPI_Dll.
/// May 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <Http.hpp>
#include <CoRoutine.hpp>
#include <SSLClients.hpp>

namespace DiscordCoreInternal {

	void HttpRnRBuilder::constructHeaderValues(std::multimap<std::string, std::string>& headersNew, RateLimitData* theConnection) {
		if (headersNew.contains("x-ratelimit-remaining")) {
			theConnection->getsRemaining = stol(headersNew.find("x-ratelimit-remaining")->second);
		}
		if (headersNew.contains("x-ratelimit-reset-after")) {
			theConnection->sampledTimeInMs = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
			theConnection->msRemain = static_cast<int64_t>(stod(headersNew.find("x-ratelimit-reset-after")->second) * 1000.0f);
		}
		if (headersNew.contains("x-ratelimit-bucket")) {
			theConnection->bucket = headersNew.find("x-ratelimit-bucket")->second;
		}
	};

	HttpData HttpRnRBuilder::handleHeaders(HttpConnection& theConnection) {
		try {
			HttpData httpData{ .responseMessage = this->contentFinal,.responseCode = this->responseCode };
			if (this->contentFinal.size() > 0) {
				if ((this->contentFinal[0] == '{' && this->contentFinal[this->contentFinal.size() - 1] == '}') || (this->contentFinal[0] == '[' && this->contentFinal[this->contentFinal.size() - 1] == ']')) {
					httpData.responseData = nlohmann::json::parse(this->contentFinal);
				}
			}
			this->constructHeaderValues(this->headers, theConnection.rateLimitDataPtr);
			if (theConnection.rateLimitDataPtr->bucket != "") {
				theConnection.bucket = theConnection.rateLimitDataPtr->bucket;
			}
			httpData.responseHeaders = this->headers;
			return httpData;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::handleHeaders()");
		}
		return HttpData{};
	}

	std::string HttpRnRBuilder::buildRequest(HttpWorkloadData& workload) {
		try {
			std::string baseUrlNew{};
			if (workload.baseUrl.find(".com") != std::string::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(), workload.baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
			}
			else if (workload.baseUrl.find(".org") != std::string::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(), workload.baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
			}
			std::string returnString{};
			if (workload.workloadClass == HttpWorkloadClass::Get || workload.workloadClass == HttpWorkloadClass::Delete) {
				if (workload.workloadClass == HttpWorkloadClass::Get) {
					returnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				else if (workload.workloadClass == HttpWorkloadClass::Delete) {
					returnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value] : workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "Connection: Keep-Alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n\r\n";
			}
			else {
				if (workload.workloadClass == HttpWorkloadClass::Patch) {
					returnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				else if (workload.workloadClass == HttpWorkloadClass::Post) {
					returnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				else if (workload.workloadClass == HttpWorkloadClass::Put) {
					returnString = "PUT " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value] : workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "Connection: Keep-Alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n";
				returnString += "Content-Length: " + std::to_string(workload.content.size()) + "\r\n\r\n";
				returnString += workload.content;
			}
			return returnString;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::buildRequest()");
		}
		return std::string{};
	}

	void HttpRnRBuilder::resetValues(std::string&) {
		this->headers = std::multimap<std::string, std::string>{};
		this->doWeHaveContentSize = false;
		this->doWeHaveHeaders = false;
		this->isItChunked = false;
		this->contentFinal = "";
		this->responseCode = -1;
		this->contentSize = 0;
	}

	bool HttpRnRBuilder::checkForHeadersToParse(std::string& other) {
		if (other.find("HTTP/1.") != std::string::npos) {
			return true;
		}
		else {
			return false;
		}
	}

	void HttpRnRBuilder::parseHeaders(std::string& other) {
		try {
			if (other.find("\r\n\r\n") != std::string::npos) {
				if (this->responseCode == -1) {
					this->parseCode(other);
				}
				std::string newString{ "" };
				newString.insert(newString.begin(), other.begin(), other.begin() + other.find("\r\n\r\n") + std::string("\r\n\r\n").size());
				other.erase(other.begin(), other.begin() + newString.size());
				while (newString.size() > 0 && newString.find(":") != std::string::npos && newString.find("\r\n") != std::string::npos) {
					int64_t currentOffset{ 0 };
					std::string lineString = newString.substr(0, newString.find("\r\n") + 2);
					currentOffset = lineString.size();
					std::string key = lineString.substr(0, lineString.find(":"));
					std::string value = lineString.substr(lineString.find(":") + 2, lineString.size() - key.size() - 2);
					value = value.substr(0, value.find("\r\n"));
					this->headers.insert(std::make_pair(key, value));
					std::string tempString{};
					tempString.insert(tempString.begin(), newString.begin() + currentOffset, newString.end());
					newString = tempString;
				}
				if (this->headers.contains("Transfer-Encoding") && this->headers.find("Transfer-Encoding")->second == "chunked") {
					this->isItChunked = true;
				}
				this->doWeHaveHeaders = true;
			}
			else {
				return;
			}
			return;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseHeaders()");
			this->contentSize = -5;
		}
		return;
	}

	bool HttpRnRBuilder::parseChunk(std::string& other) {
		try {
			if (this->doWeHaveHeaders) {
				if (this->isItChunked) {
					if (!this->doWeHaveContentSize) {
						this->clearCRLF(other);
						this->parseSize(other);
						this->clearCRLF(other);
					}
					if (other.find("\r\n0\r\n\r\n") != std::string::npos) {
						while (other.find("\r\n") != other.find("\r\n0\r\n\r\n")) {
							this->clearCRLF(other);
							this->parseSize(other);
							this->clearCRLF(other);
							if (other.find("\r\n") != other.find("\r\n0\r\n\r\n")) {
								this->contentFinal.insert(this->contentFinal.end(), other.begin(), other.begin() + other.find("\r\n"));
								other.erase(other.begin(), other.begin() + other.find("\r\n") + 2);
							}
						}
						this->contentFinal.insert(this->contentFinal.end(), other.begin(), other.begin() + other.find("\r\n0\r\n\r\n"));
						return false;
					}
					else {
						return true;
					}
				}
				else {

					if (!this->doWeHaveContentSize) {
						this->parseSize(other);
					}

					if (this->contentSize == 0) {
						return false;
					}
					if (other.size() >= static_cast<size_t>(this->contentSize)) {
						this->contentFinal.insert(this->contentFinal.end(), other.begin(), other.begin() + this->contentSize);
						return false;
					}
					else {
						return true;
					}
				}
			}
			else {
				return true;
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseChunk()");
			return true;
		}
	}

	void HttpRnRBuilder::parseSize(std::string& other) {
		try {
			if (this->headers.contains("Content-Length")) {
				this->contentSize = stoll(this->headers.find("Content-Length")->second);
				this->doWeHaveContentSize = true;
				return;
			}
			std::string theValueString{};
			uint64_t hexIndex{ 0 };
			bool isThereHexValues{ false };
			for (uint64_t x = 0; x < other.size(); x += 1) {
				if (isxdigit(other[x]) != 0 && static_cast<int32_t>(other[x]) != EOF) {
					isThereHexValues = true;
					theValueString.push_back(other[x]);
				}
				else {
					hexIndex = x;
					break;
				}
			}
			if (!isThereHexValues) {
				this->contentSize = 0;
				return;

			}
			else {
				this->contentSize += stoll(theValueString, nullptr, 16);
				other.erase(other.begin(), other.begin() + hexIndex);
				this->doWeHaveContentSize = true;
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseSize()");
			this->contentSize = -5;
		}
	}

	void HttpRnRBuilder::clearCRLF(std::string&other) {
		uint64_t theCount{ 0 };
		for (uint64_t x = 0; x < other.size(); x += 1) {
			if (isspace(static_cast<unsigned char>(other[x])) != 0) {
				theCount += 1;
			}
			else {
				break;
			}
		}
		other.erase(other.begin(), other.begin() + theCount);
	}
 
	void HttpRnRBuilder::parseCode(std::string& other) {
		try {
			if (other.find("HTTP/1.") != std::string::npos) {
				uint64_t firstNumberIndex{ 0 };
				uint64_t lastNumberIndex{ 0 };
				bool haveWeStarted{ false };
				for (uint64_t x = other.find("HTTP/1.") + std::string("HTTP/1.").size() + 1; x < other.size(); x += 1) {
					if (!haveWeStarted && (isalnum(static_cast<unsigned char>(other[x])) != 0)) {
						firstNumberIndex = x;
						haveWeStarted = true;

					}
					else if (haveWeStarted && (isalnum(static_cast<unsigned char>(other[x])) == 0)) {
						lastNumberIndex = x;
						break;
					}
				}
				this->responseCode = stoll(other.substr(firstNumberIndex, lastNumberIndex - firstNumberIndex));
				std::string tempString = other.substr(0, other.find("\r\n", lastNumberIndex) + 2);
				other.erase(other.begin(), other.begin() + tempString.size());
			}
			else if (other.size() <= 5) {

			}
			else {
				this->responseCode = -5;
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseCode()");
		}
	}

	HttpConnection& HttpConnectionManager::getConnection(HttpWorkloadType type) {
		return *this->httpConnections.at(type).get();
	}

	void HttpConnectionManager::storeConnection(HttpWorkloadType type) {
		this->httpConnections.insert(std::make_pair(type, std::make_unique<HttpConnection>()));
	}

	void HttpConnectionManager::initialize() {
		for (int32_t enumOne = static_cast<int32_t>(HttpWorkloadType::Unset); enumOne != static_cast<int32_t>(HttpWorkloadType::LAST); enumOne++) {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			this->storeConnection(static_cast<HttpWorkloadType>(enumOne));
		}
	}

	HttpClient::HttpClient(std::string botTokenNew) :botToken(botTokenNew) {
		this->connectionManager.initialize();
	};

	HttpData HttpClient::executeByRateLimitData(HttpWorkloadData& workload, bool printResult, HttpConnection& theConnection) {
		HttpData returnData{};
		try {
			std::lock_guard<std::mutex> workloadLock{ theConnection.accessMutex };
			int64_t timeRemaining{};
			int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
			if (theConnection.bucket != "" && this->connectionManager.rateLimitValues.contains(theConnection.bucket)) {
				theConnection.rateLimitDataPtr = this->connectionManager.rateLimitValues.at(theConnection.bucket).get();
				auto semaphorePtr = &theConnection.rateLimitDataPtr->semaphore;
				semaphorePtr->acquire();
				if (this->connectionManager.rateLimitValues.at(theConnection.bucket)->getsRemaining <= 0) {
					int64_t targetTime = this->connectionManager.rateLimitValues.at(theConnection.bucket)->msRemain +
						this->connectionManager.rateLimitValues.at(theConnection.bucket)->sampledTimeInMs;
					timeRemaining = targetTime - currentTime;
				}

				if (timeRemaining > 0) {
					std::cout << "We're waiting on rate-limit: " << timeRemaining << std::endl << std::endl;
					int64_t targetTime = currentTime + timeRemaining;
					while (targetTime > currentTime) {
						currentTime = std::chrono::duration_cast<std::chrono::milliseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count();
						timeRemaining = targetTime - currentTime;

						if (timeRemaining <= 20) {

						}
						else {
							std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int64_t>(ceil(timeRemaining * 80.0f / 100.0f)) });
						}
					}
				}
				returnData = HttpClient::executeHttpRequest(workload, theConnection);
				if (workload.workloadType == HttpWorkloadType::Delete_Message_Old) {
					theConnection.rateLimitDataPtr->getsRemaining = 0;
				}
				if (!this->connectionManager.rateLimitValues.contains(theConnection.bucket)) {
					std::unique_ptr<RateLimitData> tempRateLimitData{ std::make_unique<RateLimitData>() };
					*tempRateLimitData = *theConnection.rateLimitDataPtr;
					this->connectionManager.rateLimitValues.insert_or_assign(theConnection.bucket, std::move(tempRateLimitData));
				}
				semaphorePtr->release();
			}
			else {
				std::unique_ptr<RateLimitData> tempRateLimitData{ std::make_unique<RateLimitData>() };
				theConnection.rateLimitDataPtr = tempRateLimitData.get();
				returnData = HttpClient::executeHttpRequest(workload, theConnection);
				this->connectionManager.rateLimitValues.insert_or_assign(theConnection.bucket, std::move(tempRateLimitData));
			}

			if (printResult) {
				if (returnData.responseCode != 204 && returnData.responseCode != 201 && returnData.responseCode != 200) {
					std::cout << workload.callStack + " Error: " << returnData.responseCode << ", " << returnData.responseMessage << std::endl << std::endl;
				}
				else {
					std::cout << workload.callStack + " Success: " << returnData.responseCode << ", " << returnData.responseMessage << std::endl << std::endl;
				}
			}
			return returnData;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpClient::executeByRateLimitData()");
			return returnData;
		}
		
	}

	HttpData HttpClient::executeHttpRequest(HttpWorkloadData& workload, HttpConnection& theConnection) {
		try {
			theConnection.resetValues(theConnection.getInputBuffer());
			if (theConnection.doWeConnect) {
				std::string stringNew{};
				if (workload.baseUrl.find(".com") != std::string::npos) {
					stringNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(), workload.baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
				}
				else if (workload.baseUrl.find(".org") != std::string::npos) {
					stringNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(), workload.baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
				}
				if (!theConnection.connect(stringNew)) {
					return HttpData{};
				};
				theConnection.doWeConnect = false;
			}
			auto theRequest = theConnection.buildRequest(workload);
			theConnection.writeData(theRequest);
			auto result = this->getResponse(theConnection);
			if (result.responseCode == -1) {
				theConnection.doWeConnect = true;
				return this->executeHttpRequest(workload, theConnection);
			}
			else {
				return result;
			}
		}
		catch (...) {
			theConnection.doWeConnect = true;
			return this->executeHttpRequest(workload, theConnection);
		}
	}

	std::vector<HttpData> HttpClient::executeHttpRequest(std::vector<HttpWorkloadData>& workload) {
		try {
			std::vector<HttpData> returnVector{};
			std::string currentBaseUrl{ "" };

			HttpConnection theConnection{};
			auto rateLimitDataPtr = std::make_unique<RateLimitData>();
			theConnection.rateLimitDataPtr = rateLimitDataPtr.get();
			for (auto& value : workload) {
				if (currentBaseUrl != value.baseUrl) {
					std::string stringNew{};
					if (value.baseUrl.find(".com") != std::string::npos) {
						stringNew = value.baseUrl.substr(value.baseUrl.find("https://") + std::string("https://").size(), value.baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
					}
					else if (value.baseUrl.find(".org") != std::string::npos) {
						stringNew = value.baseUrl.substr(value.baseUrl.find("https://") + std::string("https://").size(), value.baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
					}
					if (!theConnection.connect(stringNew)) {
						continue;
					};
				}
				try {
					auto theRequest = theConnection.buildRequest(value);
					theConnection.writeData(theRequest);
				}
				catch (...) {
					return std::vector<HttpData>{};
				}
				HttpData returnData = this->getResponse(theConnection);
				returnVector.push_back(returnData);
				currentBaseUrl = value.baseUrl;
			}

			return returnVector;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpClient::executehttpRequest()");
		}
		return std::vector<HttpData>{};
	}

	HttpData HttpClient::getResponse(HttpConnection& theConnection) {
		DiscordCoreAPI::StopWatch stopWatch{ std::chrono::milliseconds{3500} };
		theConnection.getInputBuffer().resize(0);
		while (true) {
			if (!theConnection.processIO()) {
				break;
			}
			if (theConnection.checkForHeadersToParse(theConnection.getInputBuffer()) && !theConnection.doWeHaveHeaders && !stopWatch.hasTimePassed()) {
				theConnection.parseHeaders(theConnection.getInputBuffer());
			}
			if (stopWatch.hasTimePassed() || (theConnection.responseCode == -5 && theConnection.contentSize == -5) || !theConnection.parseChunk(theConnection.getInputBuffer())) {
				break;
			}
		};
		return theConnection.handleHeaders(theConnection);
	}

	std::vector<HttpData> HttpClient::httpRequest(std::vector<HttpWorkloadData>& workload) {
		try {
			std::vector<HttpData> resultData = this->executeHttpRequest(workload);
			
			return resultData;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpClient::httpRequest()");
		}
		return std::vector<HttpData>{};
	}

	HttpData HttpClient::httpRequest(HttpWorkloadData& workload, bool printResult) {
		try {
			if (workload.baseUrl == "") {
				workload.baseUrl = "https://discord.com/api/v9";
			}
			HttpConnection& theConnectionNew = this->connectionManager.getConnection(workload.workloadType);
			HttpData resultData = this->executeByRateLimitData(workload, printResult, theConnectionNew);
			if (resultData.responseCode == 429) {
				resultData.responseData = nlohmann::json::parse(resultData.responseMessage);
				std::cout << workload.callStack + "::httpRequest(), We've hit rate limit! Time Remaining: " << std::to_string(this->connectionManager.rateLimitValues.at(theConnectionNew.bucket)->msRemain) << std::endl << std::endl;
				this->connectionManager.rateLimitValues.at(theConnectionNew.bucket)->msRemain = static_cast<int64_t>(1000.0f * resultData.responseData.at("retry_after").get<float>());
				theConnectionNew.resetValues(theConnectionNew.getInputBuffer());
				resultData = this->httpRequest(workload, printResult);
			}
			return resultData;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpClient::httpRequest()");
		}
		return HttpData{};
	}

	template<>
	void submitWorkloadAndGetResult<void>(HttpClient& httpClient, HttpWorkloadData& workload) {
		try {
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + httpClient.botToken));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://gi;thub.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
			workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			httpClient.httpRequest(workload, true);
			return;
		}
		catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
		}
		return;
	}

	std::vector<HttpData> submitWorkloadAndGetResult(HttpClient& httpClient, std::vector<HttpWorkloadData>& workload) {
		try {
			auto returnData = httpClient.httpRequest(workload);
			return returnData;
		}
		catch (...) {
			DiscordCoreAPI::reportException(workload[0].callStack + "::HttpClient::submitWorkloadAndGetResult()");
		}
		return std::vector<HttpData>();
	}

	HttpData submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workload)	{
		try {
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + httpClient.botToken));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://github.com/RealTimeChris/DiscordCoreAPI, 1.0)"));
			workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			return httpClient.httpRequest(workload, false);
		}
		catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
		}
		return HttpData();
	}

};