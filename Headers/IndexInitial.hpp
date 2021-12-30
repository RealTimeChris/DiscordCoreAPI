// IndexInitial.hpp - Main header for the dynamic-link-library version of my bot library.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
#endif

#ifndef OPENSSL_NO_DEPRECATED
#define OPENSSL_NO_DEPRECATED
#endif

#ifdef _WIN32
#ifdef DISCORDCOREAPIDLL_EXPORTS
#define DiscordCoreAPI_Dll __declspec(dllexport)
#else
#define DiscordCoreAPI_Dll __declspec(dllimport)
#endif
#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif
#ifndef _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#define _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>
#pragma comment(lib, "windowsapp")
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "User32")
#pragma comment(lib, "Kernel32")
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"
#endif

#pragma comment(lib, "libcurl")
#pragma comment(lib, "libcrypto")
#pragma comment(lib, "libssl")

#pragma warning(push)
#pragma warning(disable : 4251 4275)

extern "C"
{
#include <libavutil/mastering_display_metadata.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/spherical.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/avstring.h>
#include <libavutil/avassert.h>
#include <libavutil/intfloat.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/version.h>
#include <libavutil/macros.h>
#include <libavutil/avutil.h>
#include <libavutil/base64.h>
#include <libavformat/avio.h>
#include <libavutil/frame.h>
#include <libavutil/dict.h>
#include <libavutil/opt.h>
#include <libavutil/lzo.h>
#include <libavutil/opt.h>
}
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <cpp-base64/base64.h>
#include <openssl/ossl_typ.h>
#include <openssl/x509_vfy.h>
#include <openssl/symhacks.h>
#include <nlohmann/json.hpp>
#include <openssl/x509v3.h>
#include <openssl/bioerr.h>
#include <openssl/crypto.h>
#include <openssl/sslerr.h>
#include <openssl/evperr.h>
#include <glib-2.0/glib.h>
#include <openssl/e_os2.h>
#include <openssl/store.h>
#include <openssl/async.h>
#include <openssl/ocsp.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/kdf.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/ct.h>
#include <nanobench.h>
#include <opus/opus.h>
#include <filesystem>
#include <coroutine>
#include <iostream>
#include <sodium.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <ctime>
#include <regex>
#include <cmath>
#include <mutex>
#include <queue>
#define CURL_STATICLIB
#include <curl\curl.h>

#ifdef _DEBUG
#define DBG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifdef DELETE
#undef DELETE
#endif

/**
* \defgroup main_endpoints Main Endpoints
* \brief For all of the Discord API's endpoints.
*/

/**
* \defgroup voice_connection Voice Connection
* \brief For all of the voice connection related stuff.
*/

/**
* \defgroup discord_events Discord Events
* \brief For all of the events that could be sent by the Discord API.
*/

/**
* \defgroup utilities Utilities
* \brief For utility classes/functions.
*/

/**
* \defgroup foundation_entities Foundation Entities
* \brief For all of the building blocks of the main endpoints.
*/
