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
			this->thePtr = std::move(other.thePtr);
			other.thePtr = std::unique_ptr<BIO, BIODeleter>{};
			return *this;
		}

		BIOWrapper(BIOWrapper&& other) noexcept {
			*this = std::move(other);
		}

		BIOWrapper& operator=(BIOWrapper& other) = delete;

		BIOWrapper(BIOWrapper& other) = delete;

		BIOWrapper& operator=(BIO* other) {
			this->thePtr.reset(other);
			if (BIO_up_ref(other) != 1) {
				std::cout << "BIO_up_ref() Error: " << ERR_get_error() << std::endl;
			};
			return *this;
		}

		operator BIO*() {
			return this->thePtr.get();
		}

		BIOWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<BIO, BIODeleter> thePtr{ nullptr, BIODeleter{} };
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

		addrinfoWrapper& operator=(addrinfoWrapper& other) {
			this->thePtr.reset(other.thePtr.release());
			return *this;
		}

		addrinfoWrapper(addrinfoWrapper& other) {
			*this = other;
		}

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
		std::unique_ptr<addrinfo, addrinfoDeleter> thePtr{ new addrinfo, addrinfoDeleter{} };
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
			this->thePtr = std::move(other.thePtr);
			other.thePtr = std::unique_ptr<SSL_CTX, SSL_CTXDeleter>{};
			return *this;
		}

		SSL_CTXWrapper(SSL_CTXWrapper&& other) noexcept {
			*this = std::move(other);
		}

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->thePtr.reset(other);
			if (SSL_CTX_up_ref(other) != 1) {
				std::cout << "SSL_CTX_up_ref() Error: " << ERR_get_error() << std::endl;
			}
			return *this;
		}

		operator SSL_CTX*() {
			return this->thePtr.get();
		}

		SSL_CTXWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr , SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {

		struct SSLDeleter {
			void operator()(SSL* other) {
				if (other != nullptr) {
					SSL_shutdown(other);
					SSL_free(other);
					other = nullptr;
				}
			}
		};

		SSLWrapper& operator=(SSLWrapper&& other) noexcept {
			this->thePtr = std::move(other.thePtr);
			other.thePtr = std::unique_ptr<SSL, SSLDeleter>{};
			return *this;
		}

		SSLWrapper(SSLWrapper&& other) noexcept {
			*this = std::move(other);
		}

		SSLWrapper& operator=(SSL* other) {
			this->thePtr.reset(other);
			if (SSL_up_ref(other) != 1) {
				std::cout << "SSL_up_ref() Error: " << ERR_get_error() << std::endl;
			}
			return *this;
		}

		operator SSL*() {
			return this->thePtr.get();
		}

		SSLWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<SSL, SSLDeleter> thePtr{ nullptr , SSLDeleter{} };
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
		std::unique_ptr<SOCKET, SOCKETDeleter> thePtr{ new SOCKET{}, SOCKETDeleter{} };
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
				std::cout << "WSAStartup Error: " << errorCode << std::endl;
			};
		}

	protected:
		std::unique_ptr<WSADATA, WSADATADeleter> thePtr{ new WSADATA{}, WSADATADeleter{} };
	};

	class HttpSSLClient {
	public:

		HttpSSLClient() = default;

		HttpSSLClient(std::string* theVector);

		bool writeData(std::string theData);

		bool connect(std::string baseUrl);

		bool readData();

		virtual ~HttpSSLClient() = default;

	protected:

		std::string soundcloudCertPath{ "C:/SSL/certs/SoundCloudCert.pem" };
		std::string defaultCertPath{ "C:/SSL/certs/DiscordCert.pem" };
		std::string googleCertPath{ "C:/SSL/certs/GoogleCert.pem" };
		const int32_t maxRecursionDepth{ 25 };
		BIOWrapper connectionBio{ nullptr };
		int64_t maxBufferSize{ 16 * 1024 };
		int32_t currentRecursionDepth{ 0 };
		std::string* theInputVector{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		SSLWrapper ssl{ nullptr };
	};

	template <typename ObjectType>
	concept StringOrVector = requires(ObjectType v)
	{
		{v.data() }->std::convertible_to<char*>;
	} || requires(ObjectType v)
	{
		{v.data()}->std::convertible_to<uint8_t*>;
	};

	class DiscordCoreAPI_Dll WebSocketSSLClient {
	public:

		WebSocketSSLClient(std::string, std::string, std::vector<uint8_t>*);

		WebSocketSSLClient(nullptr_t);

		template<StringOrVector typeName>
		void writeData(typeName& data) {
			int32_t result = SSL_write(this->ssl, data.data(), static_cast<uint32_t>(data.size()));
			if (result <= 0) {
				std::cout << "SSL_write() Error: " << SSL_get_error(this->ssl, result) << std::endl;
				return;
			};
			data.clear();
		}

		std::vector<uint8_t> getData();

		bool readData();

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		std::vector<uint8_t>* inputBuffer{};
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		SSLWrapper ssl{ nullptr };
		std::string baseUrl{ "" };
		std::string port{ "" };
		fd_set readSet{};
	};
	
	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	public:

		DatagramSocketSSLClient(std::string hostName, std::string post, std::vector<char>*);

		DatagramSocketSSLClient(nullptr_t);

		void writeData(std::string& dataToWrite);

		bool readData(bool doWeClear);

		std::vector<uint8_t> getData();

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		std::vector<char>* inputBuffer{};
		BIOWrapper dgramBio{ nullptr };
		std::string baseUrl{ "" };
		std::string port{ "" };
		fd_set readSet{};
	};

	class DiscordCoreAPI_Dll StreamSocketSSLClient {
	public:

		StreamSocketSSLClient(std::string hostName, std::string post, int32_t bufferSize);

		StreamSocketSSLClient(nullptr_t);

		void writeData(std::vector<uint8_t>& dataToWrite);

		std::vector<uint8_t> getData();

		int64_t getBytesRead();

		bool readData();

	protected:

		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper  context{ nullptr };
		const int32_t maxBufferSize{ 0 };
		std::vector<char> inputBuffer{};
		std::string baseUrl{ "" };
		SSLWrapper ssl{ nullptr };
		std::string port{ "" };
		int64_t bytesRead{ 0 };
		fd_set readSet{};
	};

}