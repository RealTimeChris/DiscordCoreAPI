// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	const string randomCertPath{ "C:/Program Files/Common Files/SSL/certs/DigiCertHighAssuranceEVRootCA.crt.pem" };
	const string soundcloudCertPath{ "C:/Program Files/Common Files/SSL/certs/Root-R3.pem" };
	const string youtubeCertPath{ "C:/Program Files/Common Files/SSL/certs/gtsr1.pem" };
	const string googleCertPath{ "C:/Program Files/Common Files/SSL/certs/gtsr1.pem" };

	DiscordCoreAPI_Dll int32_t parseCode(string rawString);

	struct DiscordCoreAPI_Dll HttpInputData {
		int64_t bytesWritten{ 0 };
		string theData{};
	};

	struct BIODeleter {
		void operator()(BIO* other) {
			BIO_free(other);
		}
	};

	struct DiscordCoreAPI_Dll HttpResponseData {
		friend size_t writeDataCallBack(char* ptr, size_t size, size_t nmemb, void* userData);
		friend size_t readDataCallBack(char* ptr, size_t size, size_t nmemb, void* userData);
		friend class HttpClient;
		bool doWeHaveHeaders{ false };
		map<string, string> headers{};
		int64_t currentOffset{ -1 };
		int64_t contentOffset{ 0 };
		int32_t responseCode{ -1 };
		int64_t contentSize{ -1 };
		string contentReal{ "" };
		string rawInput{ "" };
	};

	class CURLWrapper {
	public:

		CURLWrapper& operator=(CURLWrapper&& other) noexcept {
			this->theCounter = other.theCounter;
			this->thePtr = other.thePtr;
			other.thePtr = nullptr;
			return *this;
		}

		CURLWrapper(CURLWrapper&& other) noexcept {
			*this = move(other);
		}

		CURLWrapper(CURL* other) {
			this->thePtr = other;
		}

		CURLWrapper& operator=(CURL* other) = delete;

		CURLWrapper(CURLWrapper& other) = delete;

		operator CURL* () {
			return this->thePtr;
		}

		CURLWrapper(nullptr_t) {
			this->thePtr = nullptr;
			this->theCounter = new Counter{};
			this->theCounter->incrementCount();
		}

		~CURLWrapper() {
			this->theCounter->decrementCount();
			if (this->theCounter->getCount() == 0) {
				curl_easy_cleanup(this->thePtr);
				delete this->theCounter;
			}
		}

		Counter* theCounter{};
		CURL* thePtr;
	};

	class CURLUWrapper {
	public:

		CURLUWrapper& operator=(CURLUWrapper&& other) noexcept {
			this->theCounter = other.theCounter;
			this->thePtr = other.thePtr;
			other.thePtr = nullptr;
			return *this;
		}

		CURLUWrapper(CURLUWrapper&& other) noexcept {
			*this = move(other);
		}

		CURLUWrapper(CURLU* other) {
			this->thePtr = other;
		}

		CURLUWrapper& operator=(CURLU* other) = delete;

		CURLUWrapper(CURLUWrapper& other) = delete;

		operator CURLU* () {
			return this->thePtr;
		}

		CURLUWrapper(nullptr_t) {
			this->thePtr = nullptr;
			this->theCounter = new Counter{};
			this->theCounter->incrementCount();
		}

		~CURLUWrapper() {
			this->theCounter->decrementCount();
			if (this->theCounter->getCount() == 0) {
				curl_free(this->thePtr);
				delete this->theCounter;
			}
		}

		Counter* theCounter{};
		CURLU* thePtr{};
	};

	class DiscordCoreAPI_Dll HttpClientNew {
	public:
		HttpClientNew();

		static HttpResponseData executeHttpRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		void addHeader(string, string);

		void removeHeader(string);

	protected:

		static string constructRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		static bool connect(string baseUrl, string relativePath, HttpClientNew& clientNew);

		static bool sendRequest(string request, HttpClientNew& clientNew);

		static HttpResponseData getResponse(HttpClientNew& clientNew);

		static void loadToStore(std::string file, X509_STORE*& store);

		static void parseHeaders(HttpResponseData& inputValue);

		static void parseCodeTwo(HttpResponseData& inputValue);

		static bool parseChunk(HttpResponseData& dataPackage);

		static void parseSize(HttpResponseData& dataPackage);

		static void verifyCertificate(string certPath);

		static X509* loadCert(std::string file);
		
		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr, SSL_CTXDeleter{} };
		
		unique_ptr<BIO, BIODeleter> connectionBio{ nullptr, BIODeleter{} };
		unique_ptr<Socket, SocketDeleter> fileDescriptor{ 0, SocketDeleter{} };
		unique_ptr<SSL, SSLDeleter> ssl{ nullptr, SSLDeleter{} };
		map<string, string> headers{};
		vector<char> inputBuffer{};
		
		fd_set readfds{};
	};

	class DiscordCoreAPI_Dll HttpClient {
	public:

		HttpClient& operator=(HttpClient&& other) noexcept;

		HttpClient(HttpClient&& other) noexcept;

		HttpClient& operator=(HttpClient& other) = delete;

		HttpClient(HttpClient& other) = delete;

		HttpClient() = default;

		HttpClient(string, HttpWorkloadClass workloadType);

		static HttpResponseData executeHttpRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		static void addHeader(string, string);

		static void removeHeader(string);

		static map<string, string> getHeaders();

		~HttpClient();

	protected:
		static unique_ptr<curl_slist> headerList;
		static map<string, string> headers;
		static mutex accessMutex;
	};

	class DiscordCoreAPI_Dll HttpRequestAgent {
	public:

		friend class HttpClientNew;

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
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
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
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
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
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
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
		static HttpClient deleteClient;
		static HttpClient patchClient;
		static HttpClient postClient;
		static HttpClient putClient;
		static HttpClient getClient;
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