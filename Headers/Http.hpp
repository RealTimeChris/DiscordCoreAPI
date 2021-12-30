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

	class DiscordCoreAPI_Dll HttpResponseData {
	public:
		HttpResponseData() {
			this->doWeHaveHeaders = false;
			this->currentOffset = -1;
			this->contentOffset = -1;
			this->responseCode = -1;
			this->contentSize = -1;
		}

		void incrementStack() {
			this->currentOffset = 0;
			this->contentOffset = 0;
			this->contentSize = 0;
		}
		
		map<string, string> headers{};
		bool doWeHaveHeaders{ false };
		string contentFinalReal{ "" };
		int64_t currentOffset{};
		int64_t contentOffset{};
		int32_t responseCode{};
		int64_t contentSize{};
		string rawInput{};
	};

	struct CURLDeleter {
		void operator()(CURL* other) {
			if (other != nullptr) {
				curl_free(other);
				other = nullptr;
			}
		}
	};

	struct CURLWrapper {

		CURLWrapper& operator=(CURL* other) {
			this->thePtr = unique_ptr<CURL, CURLDeleter>(other, CURLDeleter{});
			return *this;
		}

		operator CURL* () {
			return this->thePtr.get();
		}

		~CURLWrapper() {}

	protected:
		unique_ptr<CURL, CURLDeleter> thePtr{ nullptr , CURLDeleter{} };
	};

	struct CURLUDeleter {
		void operator()(CURL* other) {
			if (other != nullptr) {
				curl_free(other);
				other = nullptr;
			}
		}
	};

	struct CURLUWrapper {

		CURLUWrapper& operator=(CURLU* other) {
			this->thePtr = unique_ptr<CURLU, CURLUDeleter>(other, CURLUDeleter{});
			return *this;
		}

		operator CURLU* () {
			return this->thePtr.get();
		}

		~CURLUWrapper() {}

	protected:
		unique_ptr<CURLU, CURLUDeleter> thePtr{ nullptr , CURLUDeleter{} };
	};

	struct BIODeleter {
		void operator()(BIO* other) {
			if (other != nullptr) {
				BIO_free(other);
				other = nullptr;
			}
		}
	};

	struct BIOWrapper {

		BIOWrapper(nullptr_t) {};

		BIOWrapper& operator=(BIO* other) {
			this->thePtr = unique_ptr<BIO, BIODeleter>(other, BIODeleter{});
			if (BIO_up_ref(other) != 1) {
				cout << "BIO_up_ref() Error: " << ERR_get_error() << endl;
			};
			return *this;
		}

		operator BIO* () {
			return this->thePtr.get();
		}

		~BIOWrapper() {}

	protected:
		unique_ptr<BIO, BIODeleter> thePtr{ nullptr , BIODeleter{} };
	};


	class DiscordCoreAPI_Dll HttpClient {
	public:
		HttpClient();

		static HttpResponseData executeHttpRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

	protected:

		static string constructRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		static bool connect(string baseUrl, string relativePath, HttpClient& clientNew);

		static bool sendRequest(string request, HttpClient& clientNew);

		static HttpResponseData getResponse(HttpClient& clientNew);

		static void parseHeaders(HttpResponseData& inputValue);

		static bool parseChunk(HttpResponseData& dataPackage);

		static void parseSize(HttpResponseData& dataPackage);

		static void parseCode(HttpResponseData& inputValue);

		SOCKETWrapper fileDescriptor{ nullptr };
		BIOWrapper connectionBio{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		vector<char> inputBuffer{};
		SSLWrapper ssl{ nullptr };
		fd_set readfds{};
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
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
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
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
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
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return HttpData();
		}

	protected:
		static map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static map<string, unique_ptr<RateLimitData>> rateLimitData;
		static string botToken;
		static string baseURL;
		static HttpData executeByRateLimitData(HttpWorkloadData workload, RateLimitData* rateLimitDataNew, bool printResult);
		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
	};
}