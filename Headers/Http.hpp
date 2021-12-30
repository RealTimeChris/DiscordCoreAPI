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

	struct DiscordCoreAPI_Dll HttpInputData {
		int64_t bytesWritten{ 0 };
		string theData{};
	};

	class StringWrapper {
	public:

		StringWrapper() {};

		StringWrapper(string newString) {
			this->theStrings.push_back(newString);
			this->currentStringCount += 1;
		}

		uint64_t size() {
			return this->getCurrentTotalSize();
		}

		template<typename _Iter>
		void insert(const string::const_iterator where, _Iter first, _Iter last) {
			string newString{ where };
			uint64_t offSet = newString.size();
			uint64_t offsetIndex = offSet / 16384;
			uint64_t remainder = offSet % 16384;
			uint64_t currentOffset{ 0 };
			uint64_t length = last - first;
			uint64_t currentIndex{ offsetIndex };
			while (length > 0) {
				uint64_t currentRemainder = 16384 - this->theStrings[currentIndex].size();
				if (length > (16384 - this->theStrings[currentIndex].size())) {
					this->theStrings[currentIndex].insert(this->theStrings[currentIndex].begin() + currentOffset, first + currentOffset, first + currentOffset + currentRemainder);
				}
				currentIndex += 1;
				currentOffset += currentRemainder;
				length -= currentRemainder;
			}
		}

		auto begin() {
			return this->theStrings[0].begin();
		}

		auto end() {
			return this->theStrings[this->currentStringCount].end();
		}

		string substr(uint64_t offset, uint64_t count) {
			string theReturnString{};
			uint64_t theIndexOffset = offset / 16384;
			uint64_t remainderOffset = offset % 16384;
			uint64_t remainder = count % 16384;
			if (offset + count >= this->getCurrentTotalSize()) {
				uint64_t currentIndex{ theIndexOffset };
				while (currentIndex < this->theStrings.size()) {
					theReturnString.insert(theReturnString.begin(), this->theStrings[currentIndex].begin() + remainderOffset, this->theStrings[currentIndex].begin() + remainder);
					currentIndex += 1;
				}
			}
			else {
				uint64_t currentIndex{ theIndexOffset };
				while (currentIndex < this->theStrings.size()) {
					if (currentIndex < this->theStrings.size() - 1) {
						theReturnString.insert(theReturnString.begin(), this->theStrings[currentIndex].begin() + remainderOffset, this->theStrings[currentIndex].end());
					}
					else {
						theReturnString.insert(theReturnString.begin(), this->theStrings[currentIndex].begin(), this->theStrings[currentIndex].begin() + remainder);
					}
					currentIndex += 1;
				}
			}
		}

		uint64_t getCurrentTotalSize() {
			uint64_t theValue{ 0 };
			if (this->currentStringCount > 0) {
				theValue = (16384 * this->currentStringCount - 1) + this->theStrings[this->currentStringCount].size();
			}
			return theValue;
		}

		uint64_t find(string theString) {
			uint64_t theIndex{ 0 };
			uint64_t theInnerIndex{ 0 };
			for (uint32_t x = 0; x < this->theStrings.size(); x += 1) {
				if (this->theStrings[x].find(theString) != string::npos) {
					theInnerIndex = this->theStrings[x].find(theString);
					theIndex = x;
					break;
				}
			}
			uint64_t theFinalValue = theIndex * 16384 + theInnerIndex;
			return theFinalValue;
		}

		void addStringContents(string newString) {
			if (this->theStrings[this->currentStringCount].size() + newString.size() > 16384) {
				uint64_t offSetFromMax = 16384 - this->theStrings[this->currentStringCount].size();
				this->theStrings[this->currentStringCount].insert(this->theStrings[this->currentStringCount].end(), newString.begin(), newString.begin() + offSetFromMax);
				string newerString{};
				newerString.insert(newerString.begin(), newString.begin() + offSetFromMax, newString.end());
				this->theStrings.push_back(newerString);
			}
			else {
				this->theStrings[this->currentStringCount].insert(this->theStrings[this->currentStringCount].begin(), newString.begin(), newString.end());
			}
		}

		vector<string> theStrings{};
		uint64_t currentStringCount{ 0 };
	};

	struct BIODeleter {
		void operator()(BIO* other) {
			BIO_free(other);
		}
	};

	struct DiscordCoreAPI_Dll HttpResponseData {
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

	class DiscordCoreAPI_Dll HttpClient {
	public:
		HttpClient();

		static HttpResponseData executeHttpRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		void addHeader(string, string);

		void removeHeader(string);

	protected:

		static string constructRequest(string baseUrl, string relativePath, string content, map<string, string> headers, HttpWorkloadClass workloadClass);

		static bool connect(string baseUrl, string relativePath, HttpClient& clientNew);

		static bool sendRequest(string request, HttpClient& clientNew);

		static HttpResponseData getResponse(HttpClient& clientNew);

		static void parseHeaders(HttpResponseData& inputValue);

		static bool parseChunk(HttpResponseData& dataPackage);

		static void parseSize(HttpResponseData& dataPackage);

		static void parseCode(HttpResponseData& inputValue);

		static shared_ptr<map<string, string>> headersDefault;

		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr, SSL_CTXDeleter{} };
		unique_ptr<Socket, SocketDeleter> fileDescriptor{ 0, SocketDeleter{} };
		unique_ptr<BIO, BIODeleter> connectionBio{ nullptr, BIODeleter{} };
		unique_ptr<SSL, SSLDeleter> ssl{ nullptr, SSLDeleter{} };
		map<string, string> headers{};
		vector<char> inputBuffer{};
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