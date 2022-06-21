/*
*
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
/// Http.cpp - Source file for the Http class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Http.cpp

#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/ThreadPool.hpp>

namespace DiscordCoreInternal {

	namespace Globals {
		std::unordered_map<std::thread::id, std::unique_ptr<HttpConnection>> httpConnections{};
		std::unordered_map<std::string, std::unique_ptr<RateLimitData>> rateLimitValues{};
		std::unordered_map<HttpWorkloadType, std::string> rateLimitValueBuckets{};
	}

	void HttpRnRBuilder::updateRateLimitData(std::unordered_map<std::string, std::string>& headersNew, RateLimitData& rateLimitData) {
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

	HttpResponseData HttpRnRBuilder::finalizeReturnValues(RateLimitData& rateLimitData, HttpResponseData& theData) {
		if (theData.responseMessage.size() > 0) {
			if ((theData.responseMessage[0] == '{' && theData.responseMessage[theData.responseMessage.size() - 1] == '}') ||
				(theData.responseMessage[0] == '[' && theData.responseMessage[theData.responseMessage.size() - 1] == ']')) {
				theData.responseData = nlohmann::json::parse(theData.responseMessage);
			}
		}
		this->updateRateLimitData(theData.responseHeaders, rateLimitData);
		return theData;
	}

	std::string HttpRnRBuilder::buildRequest(const HttpWorkloadData& workload) {
		std::string baseUrlNew{};
		if (workload.baseUrl.find(".com") != std::string::npos) {
			baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(),
				workload.baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
		} else if (workload.baseUrl.find(".org") != std::string::npos) {
			baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + std::string("https://").size(),
				workload.baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
		}
		std::string theReturnString{};
		if (workload.workloadClass == HttpWorkloadClass::Get || workload.workloadClass == HttpWorkloadClass::Delete) {
			if (workload.workloadClass == HttpWorkloadClass::Get) {
				theReturnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			} else if (workload.workloadClass == HttpWorkloadClass::Delete) {
				theReturnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			}
			for (auto& [key, value]: workload.headersToInsert) {
				theReturnString += key + ": " + value + "\r\n";
			}
			theReturnString += "Connection: Keep-Alive\r\n";
			theReturnString += "Host: " + baseUrlNew + "\r\n\r\n";
		} else {
			if (workload.workloadClass == HttpWorkloadClass::Patch) {
				theReturnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			} else if (workload.workloadClass == HttpWorkloadClass::Post) {
				theReturnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
			} else if (workload.workloadClass == HttpWorkloadClass::Put) {
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

	void HttpRnRBuilder::resetValues() {
		this->doWeHaveContentSize = false;
		this->doWeHaveHeaders = false;
		this->isItChunked = false;
	}

	bool HttpRnRBuilder::checkForHeadersToParse(const std::string& other) {
		if (other.find("HTTP/1.") != std::string::npos) {
			return true;
		} else {
			return false;
		}
	}

	void HttpRnRBuilder::parseHeaders(std::string& other, HttpResponseData& theData) {
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
				theData.theCurrentState = HttpState::Collecting_Contents;
			} else {
			}
		} catch (...) {
			theData.contentSize = -5;
		}
	}

	bool HttpRnRBuilder::parseChunk(std::string& other, HttpResponseData& theData) {
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

	void HttpRnRBuilder::parseSize(std::string& other, HttpResponseData& theData) {
		try {
			if (theData.responseHeaders.contains("Content-Length")) {
				theData.contentSize = stoll(theData.responseHeaders["Content-Length"]);
				this->doWeHaveContentSize = true;
				theData.theCurrentState = HttpState::Collecting_Contents;
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
				theData.contentSize = 0;
				return;
			} else {
				theData.contentSize += stoll(theValueString, nullptr, 16);
				other.erase(other.begin(), other.begin() + hexIndex);
				this->doWeHaveContentSize = true;
				theData.theCurrentState = HttpState::Collecting_Contents;
			}
		} catch (...) {
			theData.contentSize = -5;
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

	void HttpRnRBuilder::parseCode(std::string& other, HttpResponseData& theData) {
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
			theData.responseCode = stoll(other.substr(firstNumberIndex, lastNumberIndex - firstNumberIndex));
			std::string tempString = other.substr(0, other.find("\r\n", lastNumberIndex) + 2);
			other.erase(other.begin(), other.begin() + tempString.size());
			theData.theCurrentState = HttpState::Collecting_Headers;
		} else if (other.size() <= 5) {
		} else {
			theData.responseCode = -5;
		}
	}

	HttpConnection* HttpConnectionManager::getConnection() {
		std::lock_guard<std::mutex> theLock{ this->theMutex };
		if (!Globals::httpConnections.contains(std::this_thread::get_id())) {
			Globals::httpConnections[std::this_thread::get_id()] = std::make_unique<HttpConnection>();
		}
		return Globals::httpConnections[std::this_thread::get_id()].get();
	}

	void HttpConnectionManager::initialize() {
		for (int64_t enumOne = static_cast<int64_t>(HttpWorkloadType::Unset); enumOne != static_cast<int64_t>(HttpWorkloadType::LAST); enumOne++) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			rateLimitData->tempBucket = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			Globals::rateLimitValueBuckets[static_cast<HttpWorkloadType>(enumOne)] = rateLimitData->tempBucket;
			Globals::rateLimitValues[rateLimitData->tempBucket] = std::move(rateLimitData);
			std::this_thread::sleep_for(1ms);
		}
	}

	HttpClient::HttpClient(const std::string& botTokenNew, bool doWePrintHttpSuccessMessagesNew, bool doWePrintHttpErrorMessagesNew, bool doWePrintFFMPEGSuccessMessagesNew,
		bool doWePrintFFMPEGErrorMessagesNew, bool doWePrintWebSocketErrorMessagesNew)
		: botToken(botTokenNew), doWePrintFFMPEGErrorMessages(doWePrintFFMPEGErrorMessagesNew), doWePrintHttpSuccessMessages(doWePrintHttpSuccessMessagesNew),
		  doWePrintHttpErrorMessages(doWePrintHttpErrorMessagesNew), doWePrintFFMPEGSuccessMessages(doWePrintFFMPEGSuccessMessagesNew),
		  doWePrintWebSocketErrorMessages(doWePrintWebSocketErrorMessagesNew) {
		this->connectionManager.initialize();
	};

	HttpResponseData HttpClient::executeByRateLimitData(const HttpWorkloadData& workload, HttpConnection& theConnection) {
		HttpResponseData returnData{};
		theConnection.resetValues();
		RateLimitData& rateLimitData = *Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]].get();
		int64_t timeRemaining{};
		int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		if (workload.workloadType == HttpWorkloadType::Delete_Message_Old) {
			rateLimitData.msRemain = 4000;
		} else if (workload.workloadType == HttpWorkloadType::Delete_Message || workload.workloadType == HttpWorkloadType::Patch_Message) {
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
			if (this->doWePrintHttpSuccessMessages) {
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
		rateLimitData.sampledTimeInMs =
			static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		returnData = HttpClient::httpRequestInternal(workload, theConnection, rateLimitData);

		if (rateLimitData.tempBucket != "") {
			rateLimitData.tempBucket = "";
		}
		std::string currentBucket = rateLimitData.bucket;
		if (!Globals::rateLimitValues.contains(rateLimitData.bucket)) {
			std::unique_ptr<RateLimitData> rateLimitData{ std::make_unique<RateLimitData>() };
			Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]].swap(rateLimitData);
			Globals::rateLimitValueBuckets[workload.workloadType] = currentBucket;
			Globals::rateLimitValues[currentBucket] = std::move(rateLimitData);
		}
		if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
			if (this->doWePrintHttpSuccessMessages) {
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
				if (this->doWePrintHttpErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << workload.callStack + "::httpRequest(), We've hit rate limit! Time Remaining: "
							  << std::to_string(Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]]->msRemain) << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				returnData = this->executeByRateLimitData(workload, theConnection);
			} else if (this->doWePrintHttpErrorMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightRed()
						  << workload.callStack + " Error: Code = " << std::to_string(returnData.responseCode) + ", Message = " + returnData.responseMessage
						  << DiscordCoreAPI::reset() << std::endl
						  << std::endl;
			}
		}
		return returnData;
	}

	HttpResponseData HttpClient::httpRequestInternal(const HttpWorkloadData& workload, HttpConnection& theConnection, RateLimitData& rateLimitData) {
		try {
			theConnection.resetValues();
			int64_t currentTimeDistance =
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - theConnection.lastTimeUsed;
			if (theConnection.currentRecursionDepth >= theConnection.maxRecursion) {
				return HttpResponseData{};
			}
			if (theConnection.doWeConnect || (theConnection.lastTimeUsed != 0 && currentTimeDistance >= 30000)) {
				theConnection.connect(workload.baseUrl);
				theConnection.doWeConnect = false;
			}
			theConnection.lastTimeUsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto theRequest = theConnection.buildRequest(workload);
			theConnection.writeData(theRequest);
			auto result = this->getResponse(theConnection, rateLimitData);
			if (result.responseCode == -1) {
				theConnection.currentRecursionDepth += 1;
				theConnection.doWeConnect = true;
				return this->httpRequestInternal(workload, theConnection, rateLimitData);
			} else {
				theConnection.currentRecursionDepth = 0;
				return result;
			}
		} catch (...) {
			if (this->doWePrintHttpErrorMessages) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpClient::executeHttpRequest()");
			}
			theConnection.currentRecursionDepth += 1;
			theConnection.doWeConnect = true;
			return this->httpRequestInternal(workload, theConnection, rateLimitData);
		}
	}

	HttpResponseData HttpClient::getResponse(HttpConnection& theConnection, RateLimitData& rateLimitData) {
		DiscordCoreAPI::StopWatch stopWatch{ 3500ms };
		theConnection.getInputBuffer().clear();
		theConnection.resetValues();
		HttpResponseData theData{};
		while (true) {
			try {
				theConnection.processIO();
			} catch (ProcessingError&) {
				if (this->doWePrintHttpErrorMessages) {
					DiscordCoreAPI::reportException("HttpClient::getResponse()");
					theConnection.doWeConnect = true;
					break;
				}
			}
			bool doWeBreak{ false };
			switch (theData.theCurrentState) {
				case HttpState::Collecting_Code: {
					theConnection.parseCode(theConnection.getInputBuffer(), theData);
					break;
				}
				case HttpState::Collecting_Headers: {
					if (theConnection.checkForHeadersToParse(theConnection.getInputBuffer()) && !theConnection.doWeHaveHeaders && !stopWatch.hasTimePassed()) {
						theConnection.parseHeaders(theConnection.getInputBuffer(), theData);
					}
					break;
				} 
				case HttpState::Collecting_Size: {
					if (!theConnection.doWeHaveContentSize) {
						theConnection.clearCRLF(theConnection.getInputBuffer());
						theConnection.parseSize(theConnection.getInputBuffer(), theData);
						theConnection.clearCRLF(theConnection.getInputBuffer());
					}
					break;
				}
				case HttpState::Collecting_Contents: {
					if (static_cast<int64_t>(theConnection.getInputBuffer().size()) >= theData.contentSize && !theConnection.parseChunk(theConnection.getInputBuffer(), theData) ||
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

	std::vector<HttpResponseData> HttpClient::httpRequest(const std::vector<HttpWorkloadData>& workload) {
		std::vector<HttpResponseData> returnVector{};
		std::string currentBaseUrl{};
		HttpConnection theConnection{};
		auto rateLimitData = std::make_unique<RateLimitData>();
		for (auto& value: workload) {
			if (currentBaseUrl != value.baseUrl) {
				try {
					theConnection.connect(value.baseUrl);
				} catch (ProcessingError&) {
					if (this->doWePrintHttpErrorMessages) {
						DiscordCoreAPI::reportException("HttpClient::httpRequest()");
					}
					continue;
				}
			}
			auto theRequest = theConnection.buildRequest(value);
			theConnection.writeData(theRequest);
			HttpResponseData returnData = this->getResponse(theConnection, *rateLimitData);
			returnVector.push_back(returnData);
			currentBaseUrl = value.baseUrl;
		}
		return returnVector;
	}

	HttpResponseData HttpClient::httpRequest(HttpWorkloadData& workload) {
		if (workload.baseUrl == "") {
			workload.baseUrl = "https://discord.com/api/v10";
		}
		RateLimitData& rateLimitData = *Globals::rateLimitValues[Globals::rateLimitValueBuckets[workload.workloadType]].get();
		if (!rateLimitData.haveWeGoneYet) {
			std::this_thread::sleep_for(500ms);
			rateLimitData.haveWeGoneYet = true;
		}
		while (HttpWorkloadData::workloadIdsInternal[workload.workloadType].load() < workload.thisWorkerId.load() && workload.thisWorkerId.load() != 0) {
			std::this_thread::sleep_for(1ms);
		}

		HttpConnection* theConnectionNew = this->connectionManager.getConnection();

		while (!rateLimitData.theSemaphore.try_acquire()) {
			std::this_thread::sleep_for(1ms);
		}

		HttpResponseData resultData = this->executeByRateLimitData(workload, *theConnectionNew);
		auto theValue = HttpWorkloadData::workloadIdsInternal[workload.workloadType].load();
		HttpWorkloadData::workloadIdsInternal[workload.workloadType].store(theValue + 1);
		rateLimitData.theSemaphore.release();
		return resultData;
	}

	const bool HttpClient::getDoWePrintWebSocketErrorMessages() {
		return this->doWePrintWebSocketErrorMessages;
	}

	const bool HttpClient::getDoWePrintFFMPEGSuccessMessages() {
		return this->doWePrintFFMPEGSuccessMessages;
	}

	const bool HttpClient::getDoWePrintFFMPEGErrorMessages() {
		return this->doWePrintFFMPEGErrorMessages;
	}

	const bool HttpClient::getDoWePrintHttpSuccessMessages() {
		return this->doWePrintHttpSuccessMessages;
	}

	const bool HttpClient::getDoWePrintHttpErrorMessages() {
		return this->doWePrintHttpErrorMessages;
	}

	template<> void HttpClient::submitWorkloadAndGetResult<void>(HttpWorkloadData& workloadNew) {
		HttpWorkloadData workload = workloadNew;
		workload.headersToInsert["Authorization"] = "Bot " + this->botToken;
		workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
		if (workload.payloadType == PayloadType::Application_Json) {
			workload.headersToInsert["Content-Type"] = "application/json";
		} else if (workload.payloadType == PayloadType::Multipart_Form) {
			workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
		}
		this->httpRequest(workload);
		return;
	}

	template<> std::vector<HttpResponseData> HttpClient::submitWorkloadAndGetResult(const std::vector<HttpWorkloadData>& workloadNew) {
		std::vector<HttpWorkloadData> workload = workloadNew;
		return this->httpRequest(workload);
	}

	template<> HttpResponseData HttpClient::submitWorkloadAndGetResult(HttpWorkloadData& workloadNew) {
		HttpWorkloadData workload = workloadNew;
		workload.headersToInsert["Authorization"] = "Bot " + this->botToken;
		workload.headersToInsert["User-Agent"] = "DiscordBot (https://discordcoreapi.com/ 1.0)";
		if (workload.payloadType == PayloadType::Application_Json) {
			workload.headersToInsert["Content-Type"] = "application/json";
		} else if (workload.payloadType == PayloadType::Multipart_Form) {
			workload.headersToInsert["Content-Type"] = "multipart/form-data; boundary=boundary25";
		}
		return this->httpRequest(workload);
	}
}