// IndexInitial.hpp - Main header for the dynamic-link-library version of my bot library.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifdef DISCORDCOREAPIDLL_EXPORTS
#define DiscordCoreAPI_Dll __declspec(dllexport)
#else
#define DiscordCoreAPI_Dll __declspec(dllimport)
#endif

#ifndef _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
#endif

#ifdef _WIN32
#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif
#ifndef _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#define _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"
#endif

#pragma comment(lib, "libcurl")
#pragma comment(lib, "windowsapp")
#pragma comment(lib, "openssl")
#pragma comment(lib, "libssl")
#pragma comment(lib, "libcrypto_static")

#pragma warning(push)
#pragma warning(disable : 4251 4275)

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "User32")
#pragma comment(lib, "Kernel32")
#endif

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Storage.Streams.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>

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
#include <cpp-base64/base64.h>
#include <nlohmann/json.hpp>
#include <openssl/crypto.h>
#include <glib-2.0/glib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <winrt/base.h>
#include <nanobench.h>
#include <opus/opus.h>
#include <coroutine>
#include <iostream>
#include <sodium.h>
#include <iostream>
#include <string>
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

#pragma intrinsic(__ll_lshift)

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
