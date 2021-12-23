// Http.hpp - Header for the Http class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"

namespace DiscordCoreInternal {

	vector<string> splitHeaders(const string& text);

	struct HttpResponseData {
		map<string, string> headers{};
		int32_t responseCode{ 0 };
		string content{ "" };
	};

	class HttpClient {
	public:

		HttpClient(string);

		HttpResponseData constructHttpRequest(string baseUrl, string relativePath, map<string, string> headers, string content);

		void sendGetRequest(string, string, string);

		string receiveHttpMessage();

		string get(string, string);

		string receiveSomeData();

		~HttpClient();

	protected:

		unique_ptr<BIO> bioPtr{ make_unique<BIO>(nullptr ,[&]() {BIO_free_all(bioPtr.get()); delete bioPtr.get(); }) };
		unique_ptr<SSL> ssl{ make_unique<SSL>(nullptr ,[&]() {SSL_free(ssl.get()); delete ssl.get(); }) };
		unique_ptr<CURL> curlHandle{ nullptr };
		unique_ptr<SSL_CTX> ctx{ make_unique<SSL_CTX>(nullptr ,[&]() {SSL_CTX_free(ctx.get());  delete ctx.get(); }) };
		map<string, string> headers{};
		uint32_t fileDescriptor{ 0 };
		string botToken{ "" };
		string baseUrl{ "" };
		string port{ "443" };
		WSAData wsaData{};
	};

	class DiscordCoreAPI_Dll HttpRequestAgent {
	public:
		static void initialize(string);
		template<typename returnType>
		static returnType submitWorkloadAndGetResult(HttpWorkloadData workload) {
			try {
				unique_ptr<RateLimitData> rateLimitDataNew = make_unique<RateLimitData>();
				RateLimitData* rateLimitDataRaw = rateLimitDataNew.get();
				rateLimitDataNew->workloadType = workload.workloadType;
				if (HttpRequestAgent::rateLimitDataBucketValues.contains(workload.workloadType)) {
					rateLimitDataNew->bucket = to_hstring(HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType));
					if (HttpRequestAgent::rateLimitData.contains(to_string(rateLimitDataNew->bucket))) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(to_string(rateLimitDataNew->bucket)).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(to_string(rateLimitDataRaw->bucket), move(rateLimitDataNew));
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
					rateLimitDataNew->bucket = to_hstring(HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType));
					if (HttpRequestAgent::rateLimitData.contains(to_string(rateLimitDataNew->bucket))) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(to_string(rateLimitDataNew->bucket)).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(to_string(rateLimitDataRaw->bucket), move(rateLimitDataNew));
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
					rateLimitDataNew->bucket = to_hstring(HttpRequestAgent::rateLimitDataBucketValues.at(workload.workloadType));
					if (HttpRequestAgent::rateLimitData.contains(to_string(rateLimitDataNew->bucket))) {
						rateLimitDataRaw = HttpRequestAgent::rateLimitData.at(to_string(rateLimitDataNew->bucket)).get();
					}
					else {
						HttpRequestAgent::rateLimitData.insert_or_assign(to_string(rateLimitDataRaw->bucket), move(rateLimitDataNew));
					}
				}
				else {
					rateLimitDataNew->tempBucket = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
					HttpRequestAgent::rateLimitDataBucketValues.insert_or_assign(workload.workloadType, rateLimitDataNew->tempBucket);
					HttpRequestAgent::rateLimitData.insert_or_assign(rateLimitDataRaw->tempBucket, move(rateLimitDataNew));
				}
				HttpData returnData = HttpRequestAgent::executeByRateLimitData(workload, rateLimitDataRaw, false);
				return returnData;
			}
			catch (...) {
				DiscordCoreAPI::rethrowException(workload.callStack + "::HttpRequestAgent::submitWorkloadAndGetResult Error: ");
			}
			return HttpData();
		}

	protected:

		static DiscordCoreAPI::map<HttpWorkloadType, string> rateLimitDataBucketValues;
		static DiscordCoreAPI::map<string, unique_ptr<RateLimitData>> rateLimitData;
		static string botToken;
		static string baseURL;

		static HttpRequestHeaderCollection deleteHeaders;
		static HttpRequestHeaderCollection patchHeaders;
		static HttpRequestHeaderCollection postHeaders;
		static HttpRequestHeaderCollection putHeaders;
		static HttpRequestHeaderCollection getHeaders;
		static winrt::Windows::Web::Http::HttpClient deleteHttpClient;
		static winrt::Windows::Web::Http::HttpClient patchHttpClient;
		static winrt::Windows::Web::Http::HttpClient postHttpClient;
		static winrt::Windows::Web::Http::HttpClient putHttpClient;
		static winrt::Windows::Web::Http::HttpClient getHttpClient;

		static HttpData executeByRateLimitData(HttpWorkloadData workload, RateLimitData* rateLimitDataNew, bool printResult);

		static HttpData httpGETObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPUTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPOSTObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpPATCHObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
		static HttpData httpDELETEObjectData(HttpWorkloadData workloadData, RateLimitData* pRateLimitData);
	};
}