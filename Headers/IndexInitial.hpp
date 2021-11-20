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

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"
#endif

#pragma comment(lib, "windowsapp")

#pragma warning(push)
#pragma warning(disable : 4251 4275)

#include <memorybuffer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Storage.Streams.h>
#include <concurrent_unordered_map.h>
#include <winrt/Windows.Devices.h>
#include <concurrent_vector.h>
#include <ppltaskscheduler.h>

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
#include <dispatcherqueue.h>
#include <glib-2.0/glib.h>
#include <pplinterface.h>
#include <winrt/base.h>
#include <opus/opus.h>
#include <inttypes.h>
#include <pplawait.h>
#include <concrtrm.h>
#include <ppltasks.h>
#include <coroutine>
#include <winbase.h>
#include <semaphore>
#include <iostream>
#include <sodium.h>
#include <intrin.h>
#include <agents.h>
#include <concrt.h>
#include <unknwn.h>
#include <xiosbase>
#include <utility>
#include <stdio.h>
#include <fstream>
#include <bitset>
#include <future>
#include <memory>
#include <atomic>
#include <queue>
#include <ppl.h>
#include <deque>
#include <regex>
#include <ios>
#define CURL_STATICLIB
#include <curl\curl.h>
#include <string>
#include <atltime.h>
#include <limits>

using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Web::Http::Headers;
using namespace winrt::Windows::System::Threading;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;
using namespace concurrency::details;
using namespace winrt::Windows::Web;
using namespace concurrency;
using namespace nlohmann;
using namespace winrt;
using namespace std;

#ifdef _DEBUG
#define DBG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif

#pragma intrinsic(__ll_lshift)

/**
* \defgroup discord_core_client DiscordCoreClient
* \brief For all of the main endpoints.
*/

/**
* \defgroup voice_connection Voice Connection
* \brief For all of the voice connection related stuff.
*/

/**
* \defgroup discord_events Discord Events
* \brief For all of the possible events that could be sent by the Discord API.
*/

/**
* \defgroup utilities Utilities
* \brief For utility classes/functions.
*/

/**
* \defgroup foundation_entities FoundationEntities
* \brief For all of the building blocks of the main endpoints.
*/

