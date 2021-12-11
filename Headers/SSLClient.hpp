/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "mbedtls")
#pragma comment(lib, "mbedx509")
#pragma comment(lib, "mbedcrypto")

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>
#pragma comment(lib,"ws2_32")
#endif

#include <mbedtls/debug.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/debug.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll SSLClient {
	public:
        SSLClient(string, string);

        ~SSLClient();

	protected:
        uint32_t returnCode{ 0 };
        uint32_t exitCode = MBEDTLS_EXIT_FAILURE;
        mbedtls_net_context serverFd{};
        uint32_t flags{ 0 };
        string path{ "/?v=9&encoding=etf" };
        unsigned char buffer[1024];
        const char* pers = "ssl_client1";

        mbedtls_entropy_context entropy{};
        mbedtls_ctr_drbg_context ctrDebug{};
        mbedtls_ssl_context ssl{};
        mbedtls_ssl_config config{};
        mbedtls_x509_crt cacert{};
	};

}