/*
*
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
/// Http.cpp - Source file for the Http class.
/// May 12, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file Http.cpp

#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreInternal {

	namespace Globals {
		std::unordered_map<std::string, std::unique_ptr<RateLimitData>> rateLimitValues{};
		std::unordered_map<HttpWorkloadType, std::string> rateLimitValueBuckets{};
	}

	void HttpRnRBuilder::collectHeaderValues(std::unordered_map<std::string, std::string>& headersNew, RateLimitData* rateLimitData) {
		if (headersNew.contains("x-ratelimit-bucket")) {
			rateLimitData->bucket = headersNew["x-ratelimit-bucket"];
		}
		if (headersNew.contains("x-ratelimit-reset-after")) {
			rateLimitData->msRemain = static_cast<int64_t>(ceil(stod(headersNew["x-ratelimit-reset-after"]))) * 1000;
		}
		if (headersNew.contains("x-ratelimit-remaining")) {
			rateLimitData->getsRemaining = static_cast<int64_t>(stoi(headersNew["x-ratelimit-remaining"])) * 1000;
			if (rateLimitData->getsRemaining <= 1) {
				rateLimitData->doWeWait = true;
			}
		}
	};

	HttpData HttpRnRBuilder::handleHeaders(RateLimitData* rateLimitDataPtr) {
		try {
			HttpData httpData{ .responseMessage = this->contentFinal, .responseCode = this->responseCode };
			if (this->contentFinal.size() > 0) {
				if ((this->contentFinal[0] == '{' && this->contentFinal[this->contentFinal.size() - 1] == '}') ||
					(this->contentFinal[0] == '[' && this->contentFinal[this->contentFinal.size() - 1] == ']')) {
					httpData.responseData = nlohmann::json::parse(this->contentFinal);
				}
			}
			this->collectHeaderValues(this->headers, rateLimitDataPtr);
			httpData.responseHeaders = this->headers;
			return httpData;
		} catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::handleHeaders()");
		}
		return HttpData{};
	}

	std::string HttpRnRBuilder::buildRequest(HttpWorkloadData& workload) {
		try {
			std::string baseUrlNew{};
			if (workload.baseUrl.find(".com") != std::string::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(),
					workload.baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
			} else if (workload.baseUrl.find(".org") != std::string::npos) {
				baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(),
					workload.baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
			}
			std::string returnString{};
			if (workload.workloadClass == HttpWorkloadClass::Get || workload.workloadClass == HttpWorkloadClass::Delete) {
				if (workload.workloadClass == HttpWorkloadClass::Get) {
					returnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == HttpWorkloadClass::Delete) {
					returnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				}
				for (auto& [key, value]: workload.headersToInsert) {
					returnString += key + ": " + value + "\r\n";
				}
				returnString += "Connection: Keep-Alive\r\n";
				returnString += "Host: " + baseUrlNew + "\r\n\r\n";
			} else {
				if (workload.workloadClass == HttpWorkloadClass::Patch) {
					returnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == HttpWorkloadClass::Post) {
					returnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
				} else if (workload.workloadClass == HttpWorkloadClass::Put) {
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
		} catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::buildRequest()");
		}
		return std::string{};
	}

	void HttpRnRBuilder::resetValues() {
		this->headers = std::unordered_map<std::string, std::string>{};
		this->doWeHaveContentSize = false;
		this->doWeHaveHeaders = false;
		this->isItChunked = false;
		this->contentFinal = "";
		this->responseCode = -1;
		this->contentSize = 0;
	}

	bool HttpRnRBuilder::checkForHeadersToParse( const std::string& other) {
		if (other.find("HTTP/1.") != std::string::npos) {
			return true;
		} else {
			return false;
		}
	}

	void HttpRnRBuilder::parseHeaders(std::string& other) {
		try {
			if (other.find("\r\n\r\n") != std::string::npos) {
				if (this->responseCode == -1) {
					this->parseCode(other);
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
					this->headers.insert_or_assign(key, value);
					std::string tempString{};
					tempString.insert(tempString.begin(), newString.begin() + currentOffset, newString.end());
					newString = tempString;
				}
				if (this->headers.contains("Transfer-Encoding") && this->headers["Transfer-Encoding"] == "chunked") {
					this->isItChunked = true;
				}
				this->doWeHaveHeaders = true;
			} else {
				return;
			}
			return;
		} catch (...) {
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
					} else {
						return true;
					}
				} else {
					if (!this->doWeHaveContentSize) {
						this->parseSize(other);
					}
					if (this->contentSize == 0) {
						return false;
					}
					if (other.size() >= static_cast<size_t>(this->contentSize)) {
						this->contentFinal.insert(this->contentFinal.end(), other.begin(), other.begin() + this->contentSize);
						return false;
					} else {
						return true;
					}
				}
			} else {
				return true;
			}
		} catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseChunk()");
			return true;
		}
	}

	void HttpRnRBuilder::parseSize(std::string& other) {
		try {
			if (this->headers.contains("Content-Length")) {
				this->contentSize = stoll(this->headers["Content-Length"]);
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
				} else {
					hexIndex = x;
					break;
				}
			}
			if (!isThereHexValues) {
				this->contentSize = 0;
				return;
			} else {
				this->contentSize += stoll(theValueString, nullptr, 16);
				other.erase(other.begin(), other.begin() + hexIndex);
				this->doWeHaveContentSize = true;
			}
		} catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseSize()");
			this->contentSize = -5;
		}
	}

	void HttpRnRBuilder::clearCRLF(std::string& other) {
		uint64_t theCount{ 0 };
		for (uint64_t x = 0; x < other.size(); x += 1) {
			if (isspace(static_cast<unsigned char>(other[x])) != 0) {
				theCount += 1;
			} else {
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
				for (size_t x = other.find("HTTP/1.") + std::string("HTTP/1.").size() + 1; x < other.size(); x += 1) {
					if (!haveWeStarted && (isalnum(static_cast<unsigned char>(other[x])) != 0)) {
						firstNumberIndex = x;
						haveWeStarted = true;
					} else if (haveWeStarted && (isalnum(static_cast<unsigned char>(other[x])) == 0)) {
						lastNumberIndex = x;
						break;
					}
				}
				this->responseCode = stoll(other.substr(firstNumberIndex, lastNumberIndex - firstNumberIndex));
				std::string tempString = other.substr(0, other.find("\r\n", lastNumberIndex) + 2);
				other.erase(other.begin(), other.begin() + tempString.size());
			} else if (other.size() <= 5) {
			} else {
				this->responseCode = -5;
			}
		} catch (...) {
			DiscordCoreAPI::reportException("HttpRnRBuilder::parseCode()");
		}
	}

	HttpConnection* HttpConnectionManager::getConnection() {
		std::lock_guard<std::mutex> theLock{ this->theMutex };
		this->currentIndex += 1;
		if (!Globals::httpConnections.contains(std::this_thread::get_id())) {
			DiscordCoreInternal::Globals::httpConnections.insert(std::make_pair(std::this_thread::get_id(), std::make_unique<DiscordCoreInternal::HttpConnection>()));
		}
		return Globals::httpConnections[std::this_thread::get_id()].get();
	}

	void HttpConnectionManager::initialize() {
		for (int64_t enumOne = static_cast<int64_t>(HttpWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpWorkloadType::LAST); enumOne++) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			rateLimitData->tempBucket = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			Globals::rateLimitValueBuckets[static_cast<HttpWorkloadType>(enumOne)] = rateLimitData->tempBucket;
			Globals::rateLimitValues[rateLimitData->tempBucket] = std::move(rateLimitData);
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
		}
	}

	HttpClient::HttpClient( const std::string& botTokenNew, bool doWePrintHttpNew, bool doWePrintFFMPEGNew)
		: botToken(botTokenNew), doWePrintFFMPEG(doWePrintFFMPEGNew), doWePrintHttp(doWePrintHttpNew) {
		this->connectionManager.initialize();
	};

	HttpData HttpClient::executeByRateLimitData(HttpWorkloadData& workload, HttpConnection& theConnection) {
		HttpData returnData{};
		try {
			theConnection.resetValues();
			RateLimitData* rateLimitDataPtr = Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]].get();
			int64_t timeRemaining{};
			int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			if (workload.workloadType == HttpWorkloadType::Delete_Message_Old) {
				rateLimitDataPtr->msRemain = 4000;
			} else if (workload.workloadType == HttpWorkloadType::Delete_Message || workload.workloadType == HttpWorkloadType::Patch_Message) {
				rateLimitDataPtr->areWeASpecialBucket = true;
			}
			if (rateLimitDataPtr->areWeASpecialBucket) {
				rateLimitDataPtr->msRemain = static_cast<int64_t>(ceil(static_cast<float>(5000) / static_cast<float>(4)));
				int64_t targetTime = rateLimitDataPtr->msRemain + rateLimitDataPtr->sampledTimeInMs;
				timeRemaining = targetTime - currentTime;
			} else if (rateLimitDataPtr->didWeHitRateLimit) {
				int64_t targetTime = rateLimitDataPtr->sampledTimeInMs + rateLimitDataPtr->msRemain;
				timeRemaining = targetTime - currentTime;
				rateLimitDataPtr->didWeHitRateLimit = false;
			} else if (rateLimitDataPtr->doWeWait) {
				int64_t targetTime = rateLimitDataPtr->sampledTimeInMs + rateLimitDataPtr->msRemain;
				timeRemaining = targetTime - currentTime;
				rateLimitDataPtr->doWeWait = false;
			}
			if (timeRemaining > 0) {
				if (this->doWePrintHttp) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "We're waiting on rate-limit: " << timeRemaining << std::endl << DiscordCoreAPI::reset() << std::endl;
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
			rateLimitDataPtr->sampledTimeInMs =
				static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			returnData = HttpClient::executeHttpRequest(workload, theConnection, rateLimitDataPtr);

			if (rateLimitDataPtr->tempBucket != "") {
				rateLimitDataPtr->tempBucket = "";
			}
			std::string currentBucket = rateLimitDataPtr->bucket;
			if (!Globals::rateLimitValues.contains(rateLimitDataPtr->bucket)) {
				std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
				Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]].swap(rateLimitData);
				Globals::rateLimitValueBuckets[workload.workloadType] = currentBucket;
				Globals::rateLimitValues[currentBucket] = std::move(rateLimitData);
			}
			if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
				if (this->doWePrintHttp) {
					std::cout << DiscordCoreAPI::shiftToBrightGreen() << workload.callStack + " Success: " << returnData.responseCode << ", " << returnData.responseMessage
							  << std::endl
							  << DiscordCoreAPI::reset() << std::endl;
				}
			} else {
				if (returnData.responseCode == 429) {
					rateLimitDataPtr->msRemain = static_cast<int64_t>(ceil(nlohmann::json::parse(returnData.responseMessage)["retry_after"].get<double>())) * 1000;
					rateLimitDataPtr->didWeHitRateLimit = true;
					rateLimitDataPtr->sampledTimeInMs =
						static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					std::cout << DiscordCoreAPI::shiftToBrightRed() << workload.callStack + "::httpRequest(), We've hit rate limit! Time Remaining: "
							  << std::to_string(Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]]->msRemain) << std::endl
							  << DiscordCoreAPI::reset() << std::endl;
					returnData = this->executeByRateLimitData(workload, theConnection);
				} else {
					throw HttpError(std::to_string(returnData.responseCode) + ", " + returnData.responseMessage);
				}
			}
			return returnData;
		} catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::executeByRateLimitData()");
		}
		return returnData;
	}

	HttpData HttpClient::executeHttpRequest(HttpWorkloadData& workload, HttpConnection& theConnection, RateLimitData* rateLimitDataPtr) {
		try {
			theConnection.resetValues();
			int64_t currentTimeDistance =
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - theConnection.lastTimeUsed;
			if (theConnection.doWeConnect || (theConnection.lastTimeUsed != 0 && currentTimeDistance >= 30000)) {
				if (!theConnection.connect(workload.baseUrl)) {
					return HttpData{};
				};
				theConnection.doWeConnect = false;
			}
			theConnection.lastTimeUsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto theRequest = theConnection.buildRequest(workload);
			theConnection.writeData(theRequest);
			if (theConnection.currentRecursionDepth >= theConnection.maxRecursion) {
				return HttpData{};
			}
			auto result = this->getResponse(theConnection, rateLimitDataPtr);
			if (result.responseCode == -1) {
				if (theConnection.currentRecursionDepth >= 10) {
					return HttpData{};
				}
				theConnection.currentRecursionDepth += 1;
				theConnection.doWeConnect = true;
				return this->executeHttpRequest(workload, theConnection, rateLimitDataPtr);
			} else {
				theConnection.currentRecursionDepth = 0;
				return result;
			}
		} catch (...) {
			theConnection.doWeConnect = true;
			theConnection.currentRecursionDepth += 1;
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::executeHttpRequest()");
			return this->executeHttpRequest(workload, theConnection, rateLimitDataPtr);
		}
	}

	std::vector<HttpData> HttpClient::executeHttpRequest(std::vector<HttpWorkloadData>& workload) {
		try {
			std::vector<HttpData> returnVector{};
			std::string currentBaseUrl{};
			HttpConnection theConnection{};
			auto rateLimitDataPtr = std::make_unique<RateLimitData>();
			for (auto& value: workload) {
				if (currentBaseUrl != value.baseUrl) {
					if (!theConnection.connect(value.baseUrl)) {
						continue;
					};
				}
				try {
					auto theRequest = theConnection.buildRequest(value);
					theConnection.writeData(theRequest);
				} catch (...) {
					return std::vector<HttpData>{};
				}
				HttpData returnData = this->getResponse(theConnection, rateLimitDataPtr.get());
				returnVector.push_back(returnData);
				currentBaseUrl = value.baseUrl;
			}
			return returnVector;
		} catch (...) {
			DiscordCoreAPI::reportException("HttpClient::executehttpRequest()");
		}
		return std::vector<HttpData>{};
	}

	HttpData HttpClient::getResponse(HttpConnection& theConnection, RateLimitData* rateLimitDataPtr) {
		DiscordCoreAPI::StopWatch stopWatch{ std::chrono::milliseconds{ 3500 } };
		theConnection.getInputBuffer().resize(0);
		theConnection.resetValues();
		while (true) {
			if (!theConnection.processIO()) {
				theConnection.doWeConnect = true;
				break;
			}
			if (theConnection.checkForHeadersToParse(theConnection.getInputBuffer()) && !theConnection.doWeHaveHeaders && !stopWatch.hasTimePassed()) {
				theConnection.parseHeaders(theConnection.getInputBuffer());
			}
			if (static_cast<int64_t>(theConnection.getInputBuffer().size()) >= theConnection.contentSize && !theConnection.parseChunk(theConnection.getInputBuffer())) {
				break;
			}
			if (stopWatch.hasTimePassed() || (theConnection.responseCode == -5 && theConnection.contentSize == -5) || !theConnection.parseChunk(theConnection.getInputBuffer())) {
				break;
			}
		};
		return theConnection.handleHeaders(rateLimitDataPtr);
	}

	std::vector<HttpData> HttpClient::httpRequest(std::vector<HttpWorkloadData>& workload) {
		try {
			return this->executeHttpRequest(workload);
		} catch (...) {
			DiscordCoreAPI::reportException("HttpClient::httpRequest()", nullptr, true);
		}
		return std::vector<HttpData>{};
	}

	HttpData HttpClient::httpRequest(HttpWorkloadData& workload) {
		try {
			if (workload.baseUrl == "") {
				workload.baseUrl = "https://discord.com/api/v10";
			}
			RateLimitData* rateLimitDataPtr = Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]].get();
			if (!rateLimitDataPtr->haveWeGoneYet) {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
				rateLimitDataPtr->haveWeGoneYet = true;
			}
			while (HttpWorkloadData::workloadIdsInternal[workload.workloadType].load() < workload.thisWorkerId.load() && workload.thisWorkerId.load() != 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}

			HttpConnection* theConnectionNew = this->connectionManager.getConnection();

			while (!rateLimitDataPtr->theSemaphore.try_acquire()) {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}

			HttpData resultData = this->executeByRateLimitData(workload, *theConnectionNew);
			auto theValue = HttpWorkloadData::workloadIdsInternal[workload.workloadType].load();
			HttpWorkloadData::workloadIdsInternal[workload.workloadType].store(theValue + 1);
			rateLimitDataPtr->theSemaphore.release();
			return resultData;
		} catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::httpRequest()", nullptr, true);
		}
		return HttpData{};
	}

	const bool HttpClient::getDoWePrintFFMPEG() {
		return this->doWePrintFFMPEG;
	}

	const std::string HttpClient::getBotToken() {
		return this->botToken;
	}

	template<> void submitWorkloadAndGetResult<void>(HttpClient& httpClient, HttpWorkloadData& workloadNew) {
		HttpWorkloadData workload = workloadNew;
		try {
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + httpClient.getBotToken()));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://discordcoreapi.com/ 1.0)"));
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert.insert(std::make_pair("Content-Type", "multipart/form-data; boundary=boundary25"));
			}
			httpClient.httpRequest(workload);
			return;
		} catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()", nullptr, true);
		}
		return;
	}

	std::vector<HttpData> submitWorkloadAndGetResult(HttpClient& httpClient, std::vector<HttpWorkloadData>& workloadNew) {
		std::vector<HttpWorkloadData> workload = workloadNew;
		try {
			return httpClient.httpRequest(workload);
		} catch (...) {
			DiscordCoreAPI::reportException(workload[0].callStack + "::HttpClient::submitWorkloadAndGetResult()");
		}
		return std::vector<HttpData>();
	}

	HttpData submitWorkloadAndGetResult(HttpClient& httpClient, HttpWorkloadData& workloadNew) {
		HttpWorkloadData workload = workloadNew;
		try {
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + httpClient.getBotToken()));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://discordcoreapi.com/ 1.0)"));
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert.insert(std::make_pair("Content-Type", "multipart/form-data; boundary=boundary25"));
			}
			return httpClient.httpRequest(workload);
		} catch (...) {
			DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::submitWorkloadAndGetResult()");
		}
		return HttpData();
	}
}