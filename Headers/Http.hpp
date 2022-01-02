// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	const string soundcloudCertPath{ "C:/Program Files/Common Files/SSL/certs/Root-R3.pem" };
	const string youtubeCertPath{ "C:/Program Files/Common Files/SSL/certs/gtsr1.pem" };
	const string googleCertPath{ "C:/Program Files/Common Files/SSL/certs/gtsr1.pem" };

	struct DiscordCoreAPI_Dll CURLDeleter {
		void operator()(CURL* other) {
			if (other != nullptr) {
				curl_easy_cleanup(other);
				other = nullptr;
			}
		}
	};

	struct DiscordCoreAPI_Dll CURLWrapper {

		CURLWrapper(nullptr_t) {};

		CURLWrapper& operator=(CURL* other) {
			this->thePtr = unique_ptr<CURL, CURLDeleter>(other, CURLDeleter{});
			return *this;
		}

		CURLWrapper(CURL* other) {
			this->thePtr = unique_ptr<CURL, CURLDeleter>(other, CURLDeleter{});
		}

		operator CURL* () {
			return this->thePtr.get();
		}

	protected:
		unique_ptr<CURL, CURLDeleter> thePtr{ nullptr , CURLDeleter{} };
	};

	class DiscordCoreAPI_Dll HttpResponseData {
	public:

		bool doWeHaveContentSize{ false };
		bool doWeHaveHeaders{ false };
		map<string, string> headers{};
		string contentFinalReal{ "" };
		int64_t responseCode{ -1 };
		int64_t contentSize{ -1 };
		bool isItChunked{ false };
		string rawInput{ "" };
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:
		HttpClient();

		static HttpResponseData executeHttpRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

	protected:

		static string constructRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		static bool connect(string baseUrl, string relativePath, HttpClient& clientNew);

		static bool sendRequest(string request, HttpClient& clientNew);

		static bool checkForHeadersToParse(HttpResponseData theData);

		static HttpResponseData getResponse(HttpClient& clientNew);

		static void parseHeaders(HttpResponseData& inputValue);

		static bool parseChunk(HttpResponseData& dataPackage);

		static void parseSize(HttpResponseData& dataPackage);
		
		static void parseCode(HttpResponseData& inputValue);

		BIOWrapper connectionBio{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		vector<char> inputBuffer{};
		SSLWrapper ssl{ nullptr };
	};

	class DiscordCoreAPI_Dll HttpRequestAgent {
	public:

		friend class HttpClient;

		static void initialize(string);

		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, true);
				returnType returnObject{};
				DataParser::parseObject(returnData.data, &returnObject);
				return returnObject;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult");
			}
			returnType returnObject{};
			return returnObject;
		}

		template<>
		static void submitWorkloadAndGetResult<void>(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, true);
				return;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult");
			}
			return;
		}

		template<>
		static HttpData submitWorkloadAndGetResult<HttpData>(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType);
					if (HttpRequestAgent::rateLimitData.contains(rateLimitDataNew->bucket)) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(rateLimitDataNew->bucket).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->bucket, move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, true);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::reportException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult");
			}
			return HttpData();
		}

	protected:
		static map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static map<string, unique_ptr<RateLimitData>> rateLimitData;
		static shared_ptr<string> botToken;
		static shared_ptr<string> baseURL;
		static HttpData executeByRateLimitData(HttpWorkloadData workload, RateLimitData* rateLimitDataNew, bool printResult);
		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
	};
}