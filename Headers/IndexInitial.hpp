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
#ifdef _DEBUG
#define _CRTDBG_unordered_map_ALLOC
#include "crtdbg.h"
#endif
#endif

#pragma comment(lib, "libcurl")
#pragma comment(lib, "libcrypto")
#pragma comment(lib, "libssl")

#pragma warning(push)
#pragma warning(disable : 4251 4275)

extern "C"
{
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
}
#include <cpp-base64/base64.h>
#include <nlohmann/json.hpp>
#include <openssl/x509v3.h>
#include <glib-2.0/glib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <nanobench.h>
#include <opus/opus.h>
#define CURL_STATICLIB
#include <curl/curl.h>
#include <type_traits>
#include <coroutine>
#include <sodium.h>
#include <iostream>
#include <concepts>
#include <memory>
#include <regex>
#include <mutex>
#include <queue>

#ifdef _WIN32
#ifdef _DEBUG
#define DBG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif
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
