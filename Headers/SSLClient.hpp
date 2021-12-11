/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

#pragma comment(lib, "mbedtls")
#pragma comment(lib, "mbedx509")
#pragma comment(lib, "mbedcrypto")

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
        uint32_t exit_code = MBEDTLS_EXIT_FAILURE;
        mbedtls_net_context serverFd{};
        uint32_t flags{ 0 };
        unsigned char buffer[1024];
        const char* pers = "ssl_client1";

        mbedtls_entropy_context entropy{};
        mbedtls_ctr_drbg_context ctrDebug{};
        mbedtls_ssl_context ssl{};
        mbedtls_ssl_config config{};
        mbedtls_x509_crt cacert{};
	};

}