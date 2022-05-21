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
/// Http.hpp - Header file for the "Http Stuff".
/// May 12, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file Http.hpp

#pragma once

#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/SSLClients.hpp>
#include <semaphore>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll HttpConnectionManager;
	struct DiscordCoreAPI_Dll RateLimitData;

	struct DiscordCoreAPI_Dll HttpResponseData {
		friend class HttpRnRBuilder;
		friend class HttpClient;
		std::unordered_map<std::string, std::string> responseHeaders{};
		std::string responseMessage{};
		nlohmann::json responseData{};
		int64_t responseCode{ -1 };

	  protected:
		int64_t contentSize{ 0 };
	};

	class DiscordCoreAPI_Dll HttpRnRBuilder {
	  public:
		friend HttpClient;

		void updateRateLimitData(std::unordered_map<std::string, std::string>& headers, RateLimitData* theConnection);

		HttpResponseData finalizeReturnValues(RateLimitData* rateLimitDataPtr, HttpResponseData& theData);

		std::string buildRequest(const HttpWorkloadData& workload);

		bool checkForHeadersToParse(const std::string&);

		void parseHeaders(std::string&, HttpResponseData& theData);

		bool parseChunk(std::string&, HttpResponseData& theData);

		void resetValues();

		virtual ~HttpRnRBuilder() = default;

	  protected:
		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		bool isItChunked{ false };

		void parseSize(std::string&, HttpResponseData& theData);

		void parseCode(std::string&, HttpResponseData& theData);

		void clearCRLF(std::string&);
	};

	struct DiscordCoreAPI_Dll RateLimitData {
	  public:
		friend HttpConnectionManager;
		friend HttpRnRBuilder;
		friend HttpClient;

		RateLimitData() = default;

	  protected:
		std::counting_semaphore<1> theSemaphore{ 1 };
		bool areWeASpecialBucket{ false };
		bool didWeHitRateLimit{ false };
		int64_t sampledTimeInMs{ 0 };
		bool haveWeGoneYet{ false };
		int64_t getsRemaining{ 0 };
		std::string tempBucket{};
		bool doWeWait{ false };
		int64_t msRemain{ 0 };
		std::string bucket{};
	};

	struct DiscordCoreAPI_Dll HttpConnection : public HttpSSLClient, public HttpRnRBuilder {
	  public:
		int32_t currentRecursionDepth{ 0 };
		const int32_t maxRecursion{ 10 };
		int64_t lastTimeUsed{ 0 };
		bool doWeConnect{ true };

		virtual ~HttpConnection() = default;
	};

	namespace Globals {
		static std::unordered_map<std::thread::id, std::unique_ptr<HttpConnection>> httpConnections{};
	}

	class DiscordCoreAPI_Dll HttpConnectionManager {
	  public:
		std::mutex theMutex{};

		HttpConnection* getConnection();

		void initialize();
	};

	class DiscordCoreAPI_Dll HttpClient {
	  public:
		HttpClient() = default;

		HttpClient(const std::string& botTokenNew, bool doWePrintHttpSuccessNew, bool doWePrintHttpErrorNew, bool doWePrintFFMPEGSuccessNew, bool doWePrintFFMPEGErrorNew);

		std::vector<HttpResponseData> httpRequest(const std::vector<HttpWorkloadData>&);

		HttpResponseData httpRequest(HttpWorkloadData&);

		const bool getDoWePrintFFMPEGSuccess();

		const bool getDoWePrintFFMPEGError();

		const bool getDoWePrintHttpSuccess();

		const bool getDoWePrintHttpError();

		template<typename ReturnType> ReturnType submitWorkloadAndGetResult(HttpWorkloadData& workload) {
			ReturnType returnObject{};
			workload.headersToInsert.insert(std::make_pair("Authorization", "Bot " + this->botToken));
			workload.headersToInsert.insert(std::make_pair("User-Agent", "DiscordBot (https://discordcoreapi.com 1.0)"));
			if (workload.payloadType == PayloadType::Application_Json) {
				workload.headersToInsert.insert(std::make_pair("Content-Type", "application/json"));
			} else if (workload.payloadType == PayloadType::Multipart_Form) {
				workload.headersToInsert.insert(std::make_pair("Content-Type", "multipart/form-data; boundary=boundary25"));
			}
			HttpResponseData returnData = this->httpRequest(workload);
			DataParser::parseObject(returnData.responseData, returnObject);
			return returnObject;
		}

		template<std::same_as<std::vector<HttpResponseData>> Type> Type submitWorkloadAndGetResult(const std::vector<HttpWorkloadData>& workload);

		template<std::same_as<void> Type> Type submitWorkloadAndGetResult(HttpWorkloadData& workload);

		template<std::same_as<HttpResponseData> Type> Type submitWorkloadAndGetResult(HttpWorkloadData& workload);

	  protected:
		HttpConnectionManager connectionManager{};
		const bool doWePrintFFMPEGSuccess{};
		const bool doWePrintHttpSuccess{};
		const bool doWePrintFFMPEGError{};
		const bool doWePrintHttpError{};
		const std::string botToken{};

		HttpResponseData executeHttpRequest(const HttpWorkloadData& workload, HttpConnection& theConnection, RateLimitData* rateLimitDataPtr);

		HttpResponseData executeByRateLimitData(const HttpWorkloadData& workload, HttpConnection& theConnection);

		HttpResponseData getResponse(HttpConnection& theConnection, RateLimitData* rateLimitDataPtr);

		std::vector<HttpResponseData> executeHttpRequest(const std::vector<HttpWorkloadData>&);
	};

	

}// namespace DiscordCoreInternal
