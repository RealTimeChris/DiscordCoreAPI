/// SSLClents.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventEntities.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll BIOWrapper {

		struct BIODeleter {
			void operator()(BIO* other) {
				if (other != nullptr) {
					BIO_free(other);
					other = nullptr;
				}
			}
		};

		BIOWrapper& operator=(BIOWrapper&& other) noexcept {
			this->thePtr = move(other.thePtr);
			other.thePtr = unique_ptr<BIO, BIODeleter>{};
			return *this;
		}

		BIOWrapper(BIOWrapper&& other) noexcept {
			*this = move(other);
		}

		BIOWrapper& operator=(BIO* other) {
			this->thePtr.reset(other);
			if (BIO_up_ref(other) != 1) {
				cout << "BIO_up_ref() Error: " << ERR_get_error() << endl;
			};
			return *this;
		}

		operator BIO*() {
			return this->thePtr.get();
		}

		BIOWrapper(nullptr_t) {};

	protected:
		unique_ptr<BIO, BIODeleter> thePtr{ nullptr, BIODeleter{} };
	};

	struct DiscordCoreAPI_Dll SSL_METHODWrapper {

		struct SSL_METHODDeleter {
			void operator()(const SSL_METHOD*) {}
		};

		SSL_METHODWrapper& operator=(SSL_METHODWrapper&& other) noexcept {
			this->thePtr = move(other.thePtr);
			other.thePtr = unique_ptr<SSL_METHOD, SSL_METHODDeleter>{};
			return *this;
		}

		SSL_METHODWrapper(SSL_METHODWrapper&& other) noexcept {
			*this = move(other);
		}

		SSL_METHODWrapper& operator=(const SSL_METHOD* other) {
			this->thePtr.reset(other);
			return *this;
		}

		operator const SSL_METHOD*() {
			return this->thePtr.get();
		}

		SSL_METHODWrapper(nullptr_t) {};

		SSL_METHODWrapper(const SSL_METHOD* other) {
			*this = other;
		}

	protected:
		unique_ptr<const SSL_METHOD, SSL_METHODDeleter> thePtr{ nullptr, SSL_METHODDeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {

		struct addrinfoDeleter {
			void operator()(addrinfo* other) {
				if (other != nullptr) {
					freeaddrinfo(other);
					other = nullptr;
				}
			}
		};

		addrinfoWrapper& operator=(addrinfo* other) {
			this->thePtr.reset(other);
			return *this;
		}

		addrinfo* operator->() {
			return this->thePtr.get();
		}

		operator addrinfo*() {
			return this->thePtr.get();
		}

		addrinfoWrapper(nullptr_t) {
			ZeroMemory(this->thePtr.get(), sizeof(addrinfo));
		};

	protected:
		unique_ptr<addrinfo, addrinfoDeleter> thePtr{ new addrinfo, addrinfoDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {

		struct SSL_CTXDeleter {
			void operator()(SSL_CTX* other) {
				if (other != nullptr) {
					SSL_CTX_free(other);
					other = nullptr;
				}
			}
		};

		SSL_CTXWrapper& operator=(SSL_CTXWrapper&& other) noexcept {
			this->thePtr = move(other.thePtr);
			other.thePtr = unique_ptr<SSL_CTX, SSL_CTXDeleter>{};
			return *this;
		}

		SSL_CTXWrapper(SSL_CTXWrapper&& other) noexcept {
			*this = move(other);
		}

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->thePtr.reset(other);
			if (SSL_CTX_up_ref(other) != 1) {
				cout << "SSL_CTX_up_ref() Error: " << ERR_get_error() << endl;
			}
			return *this;
		}

		operator SSL_CTX*() {
			return this->thePtr.get();
		}

		SSL_CTXWrapper(nullptr_t) {};

	protected:
		unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr , SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {

		struct SSLDeleter {
			void operator()(SSL* other) {
				if (other != nullptr) {
					SSL_free(other);
					other = nullptr;
				}
			}
		};

		SSLWrapper& operator=(SSLWrapper&& other) noexcept {
			this->thePtr = move(other.thePtr);
			other.thePtr = unique_ptr<SSL, SSLDeleter>{};
			return *this;
		}

		SSLWrapper(SSLWrapper&& other) noexcept {
			*this = move(other);
		}

		SSLWrapper& operator=(SSL* other) {
			this->thePtr.reset(other);
			if (SSL_up_ref(other) != 1) {
				cout << "SSL_up_ref() Error: " << ERR_get_error() << endl;
			}
			return *this;
		}

		operator SSL*() {
			return this->thePtr.get();
		}

		SSLWrapper(nullptr_t) {};

	protected:
		unique_ptr<SSL, SSLDeleter> thePtr{ nullptr , SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {

		struct SOCKETDeleter {
			void operator()(SOCKET* other) {
				if (other != nullptr) {
#ifdef _WIN32
					shutdown(*other, 2);
					closesocket(*other);
#else
					close(*other);
#endif
					other = nullptr;
				}
			}

		};

		SOCKETWrapper& operator=(SOCKET other) {
			*this->thePtr = other;
			return *this;
		}

		operator SOCKET() {
			return *this->thePtr;
		}

		SOCKETWrapper(nullptr_t) {}

	protected:
		unique_ptr<SOCKET, SOCKETDeleter> thePtr{ new SOCKET{}, SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll WSADATAWrapper {

		struct WSADATADeleter {
			void operator()(WSADATA*) {
				WSACleanup();
			}
		};

		WSADATAWrapper() {
			int32_t errorCode = WSAStartup(MAKEWORD(2, 2), this->thePtr.get());
			if (errorCode != 0) {
				cout << "WSAStartup Error: " << errorCode << endl;
			};
		}

	protected:
		unique_ptr<WSADATA, WSADATADeleter> thePtr{ new WSADATA{}, WSADATADeleter{} };
	};

	class HttpSSLClient {
	public:

		friend class HttpClient;

		HttpSSLClient() = default;

		HttpSSLClient(string* theVector);

		bool writeData(string theData);

		bool connect(string baseUrl);

		string getInputBuffer();

		bool readData();

	protected:

		string soundcloudCertPath{ "C:/SSL/certs/SoundCloudCert.pem" };
		string defaultCertPath{ "C:/SSL/certs/DiscordCert.pem" };
		string googleCertPath{ "C:/SSL/certs/GoogleCert.pem" };
		const int32_t maxRecursionDepth{ 25 };
		BIOWrapper connectionBio{ nullptr };
		int64_t maxBufferSize{ 16 * 1024 };
		int32_t currentRecursionDepth{ 0 };
		string* theInputVector{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		bool doWeReconnectBool{ true };
		SSLWrapper ssl{ nullptr };		
	};

	template <typename T>
	concept StringOrVector = requires(T v)
	{
		{v.data() }-> convertible_to<char*>;
	} || requires(T v)
	{
		{v.data()}-> convertible_to<uint8_t*>;
	};

	class DiscordCoreAPI_Dll WebSocketSSLClient {
	public:

		WebSocketSSLClient(string, string, vector<uint8_t>*);

		WebSocketSSLClient(nullptr_t);

		vector<uint8_t>& getInputBuffer();

		template<StringOrVector typeName>
		void writeData(typeName& data) {
			int32_t result = SSL_write(this->ssl, data.data(), static_cast<uint32_t>(data.size()));
			if (result <= 0) {
				cout << "SSL_write() Error: " << SSL_get_error(this->ssl, result) << endl;
				return;
			};
			data.clear();
		}

		bool readData();

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		vector<uint8_t>* inputBuffer{};
		SSLWrapper ssl{ nullptr };
		string baseUrl{ "" };
		string port{ "" };
		fd_set readSet{};
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	public:

		DatagramSocketSSLClient(string hostName, string post, vector<char>*);

		DatagramSocketSSLClient(nullptr_t);

		void writeData(string& dataToWrite);

		vector<char>& getInputBuffer();

		bool readData(bool doWeClear);

		vector<uint8_t> getData();

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		vector<char>* inputBuffer{};
		string baseUrl{ "" };
		string port{ "" };
		fd_set readSet{};
	};

	class DiscordCoreAPI_Dll StreamSocketSSLClient {
	public:

		StreamSocketSSLClient(string hostName, string post, int32_t bufferSize);

		StreamSocketSSLClient(nullptr_t);

		void writeData(vector<uint8_t>& dataToWrite);

		vector<uint8_t> getData();

		int64_t getBytesRead();

		bool readData();

	protected:

		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper  context{ nullptr };
		const int32_t maxBufferSize{ 0 };
		vector<char> inputBuffer{};
		SSLWrapper ssl{ nullptr };
		int64_t bytesRead{ 0 };
		string baseUrl{ "" };
		string port{ "" };
		fd_set readSet{};
	};

}