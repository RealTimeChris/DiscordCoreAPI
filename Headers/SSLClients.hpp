/// SSLClents.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

#ifndef OPENSSL_NO_DEPRECATED
#define OPENSSL_NO_DEPRECATED
#endif
#ifdef _WIN32
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")
#endif

#include "FoundationEntities.hpp"
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll BIOWrapper {

		struct DiscordCoreAPI_Dll BIODeleter {
			void operator()(BIO* other) {
				if (other != nullptr) {
					BIO_free(other);
					other = nullptr;
				}
			}
		};

		BIOWrapper& operator=(BIO* other) {
			this->bioPtr.reset(other);
			if (BIO_up_ref(other) != 1) {
				std::cout << "BIO_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
			};
			return *this;
		}

		operator BIO* () {
			return this->bioPtr.get();
		}

		BIOWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<BIO, BIODeleter> bioPtr{ nullptr, BIODeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {

		struct DiscordCoreAPI_Dll addrinfoDeleter {
			void operator()(addrinfo* other) {
				if (other != nullptr) {
					freeaddrinfo(other);
					other = nullptr;
				}
			}
		};

		addrinfo* operator->() {
			return this->addrinfoPtrTwo;
		}

		operator addrinfo** () {
			return &this->addrinfoPtrTwo;
		}

		operator addrinfo* () {
			return this->addrinfoPtrTwo;
		}

		addrinfoWrapper(nullptr_t) {
			this->addrinfoPtrTwo = this->addrinfoPtr.get();
		};

	protected:
		std::unique_ptr<addrinfo, addrinfoDeleter> addrinfoPtr{ new addrinfo{}, addrinfoDeleter{} };
		addrinfo* addrinfoPtrTwo{ nullptr };
	};

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {

		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			void operator()(SSL_CTX* other) {
				if (other != nullptr) {
					SSL_CTX_free(other);
					other = nullptr;
				}
			}
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->sslCTXPtr.reset(other);
			if (SSL_CTX_up_ref(other) != 1) {
				std::cout << "SSL_CTX_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
			}
			return *this;
		}

		operator SSL_CTX* () {
			return this->sslCTXPtr.get();
		}

		SSL_CTXWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> sslCTXPtr{ nullptr , SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {

		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other) {
				if (other != nullptr) {
					SSL_shutdown(other);
					SSL_free(other);
					other = nullptr;
				}
			}
		};

		SSLWrapper& operator=(SSL* other) {
			this->sslPtr.reset(other);
			if (SSL_up_ref(other) != 1) {
				std::cout << "SSL_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
			}
			return *this;
		}

		operator SSL* () {
			return this->sslPtr.get();
		}

		SSLWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<SSL, SSLDeleter> sslPtr{ nullptr , SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {

		struct DiscordCoreAPI_Dll SOCKETDeleter {
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
			*this->socketPtr = other;
			return *this;
		}

		operator int32_t() {
			return static_cast<int32_t>(*this->socketPtr);
		}

		operator SOCKET() {
			return *this->socketPtr;
		}

		SOCKETWrapper(nullptr_t) {}

	protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> socketPtr{ new SOCKET{}, SOCKETDeleter{} };
	};

#ifdef _WIN32
	struct DiscordCoreAPI_Dll WSADATAWrapper {

		struct DiscordCoreAPI_Dll WSADATADeleter {
			void operator()(WSADATA*) {
				WSACleanup();
			}
		};

		WSADATAWrapper() {
			int32_t errorCode = WSAStartup(MAKEWORD(2, 2), this->wsaDataPtr.get());
			if (errorCode != 0) {
				std::cout << "WSAStartup Error: " << errorCode << std::endl;
			};
		}

	protected:
		std::unique_ptr<WSADATA, WSADATADeleter> wsaDataPtr{ new WSADATA{}, WSADATADeleter{} };
	};
#endif

	class DiscordCoreAPI_Dll HttpSSLClient {
	public:

		HttpSSLClient(std::string* theVector);

		HttpSSLClient(nullptr_t);

		bool connect(std::string baseUrl, std::string portNew = "443");

		bool writeData(std::string& theData);

		bool readData();

		virtual ~HttpSSLClient() = default;

	protected:

		std::string soundcloudCertPath{ "C:/SSL/certs/SoundCloudCert.pem" };
		std::string defaultCertPath{ "C:/SSL/certs/DiscordCert.pem" };
		std::string googleCertPath{ "C:/SSL/certs/GoogleCert.pem" };
		std::string* inputBufferPtr{ nullptr };
		BIOWrapper connectionBio{ nullptr };
		int32_t maxBufferSize{ 16 * 1024 };
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

		WebSocketSSLClient(std::string baseUrl, std::string port, std::vector<uint8_t>* inputBuffer, int64_t maxBufferSize = 16 * 1024);

		WebSocketSSLClient(nullptr_t);

		template<StringOrVector TypeName>
		bool writeData(TypeName& data) {
			size_t writtenBytes{ 0 };
			if (this->ssl == nullptr) {
				return false;
			}
			int32_t returnValue = SSL_write_ex(this->ssl, data.data(), static_cast<uint32_t>(data.size()), &writtenBytes);
			if (returnValue != 1) {
				std::cout << "SSL_write_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			};
			data.clear();
			return true;
		}

		std::vector<uint8_t> getData();

		int64_t getBytesRead();

		bool readData();

		void shutdown();

	protected:

		const int64_t maxBufferSize{ 1024 * 16 };
		std::vector<uint8_t>* inputBufferPtr{};
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		SSLWrapper ssl{ nullptr };
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	public:

		DatagramSocketSSLClient(std::string hostName, std::string port, std::vector<uint8_t>*);

		DatagramSocketSSLClient(nullptr_t);

		bool writeData(std::string& dataToWrite);

		std::vector<uint8_t> getData();

		void readData(bool doWeClear);

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		std::vector<uint8_t>* inputBufferPtr{};
		BIOWrapper connectionBio{ nullptr };
		SOCKETWrapper theSocket{ nullptr };
	};
}