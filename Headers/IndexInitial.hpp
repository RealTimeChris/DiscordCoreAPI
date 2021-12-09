// IndexInitial.hpp - Main header for the dynamic-link-library version of my bot library.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifdef DISCORDCOREAPIDLL_EXPORTS
#define DiscordCoreAPI_Dll __declspec(dllexport)
#pragma comment(lib, "libcurl")
#else
#define DiscordCoreAPI_Dll __declspec(dllimport)
#endif

#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif

#ifndef _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
#endif

#ifndef _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#define _CRT_USE_WINAPI_FAMILY_DESKTOP_APP
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"
#endif

#pragma comment(lib, "windowsapp")

#pragma warning(push)
#pragma warning(disable : 4251 4275)

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Storage.Streams.h>
#include <concurrent_unordered_map.h>
#include <winrt/Windows.Devices.h>
#include <concurrent_queue.h>
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
#include <nlohmann/json.hpp>
#include <glib-2.0/glib.h>
#include <winrt/base.h>
#include <nanobench.h>
#include <opus/opus.h>
#include <coroutine>
#include <atltime.h>
#include <iostream>
#include <sodium.h>
#include <stdio.h>
#include <string>
#include <regex>
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
