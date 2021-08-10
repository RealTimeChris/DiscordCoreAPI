```cpp
// pch.h - Precomiled header.
// https://github.com/RealTimeChris

#pragma once

#ifndef _PCH_
#define _PCH_

#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/write_concern.hpp>
#include <bsoncxx/builder/core.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>

#include <memorybuffer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Devices.h>
#include <winrt/Windows.Media.Render.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Networking.h>
#include <winrt/Windows.Foundation.h>
#include <concurrent_unordered_map.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>
#include <experimental/generator>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Web.h>
#include <concurrent_vector.h>
#include <ppltaskscheduler.h>
extern "C"
{
#include <libavutil/mastering_display_metadata.h>
#include <libswresample/swresample.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include "libavutil/audio_fifo.h"
#include <libavutil/spherical.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/avstring.h>
#include <libavutil/intfloat.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/version.h>
#include <libavutil/macros.h>
#include <libavutil/avutil.h>
#include <libavutil/base64.h>
#include <libavformat/avio.h>
#include <libavutil/dict.h>
#include <libavutil/opt.h>
#include <libavutil/lzo.h>
#include <libavutil/opt.h>
}
#include <ogg\config_types.h>
#include <nlohmann/json.hpp>
#include <dispatcherqueue.h>
#include <opus/opusfile.h>
#include <pplinterface.h>
#include <ogg\os_types.h>
#include <winrt/base.h>
#include <winrt/base.h>
#include <curl/curl.h>
#include <opus/opus.h>
#include <inttypes.h>
#include <pplawait.h>
#include <concrtrm.h>
#include <ppltasks.h>
#include <atltime.h>
#include <coroutine>
#include <iostream>
#include <agents.h>
#include <sodium.h>
#include <intrin.h>
#include <concrt.h>
#include <unknwn.h>
#include <xiosbase>
#include <stdio.h>
#include <fstream>
#include <fstream>
#include <bitset>
#include <ppl.h>
#include <regex>
#include <ios>

using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Web::Http::Headers;
using namespace winrt::Windows::System::Threading;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace bsoncxx::builder::stream;
using namespace winrt::Windows::Storage;
using namespace bsoncxx::builder::basic;
using namespace winrt::Windows::System;
using namespace concurrency::details;
using namespace Concurrency::details;
using namespace winrt::Windows::Web;
using namespace std::experimental;
using namespace concurrency;
using namespace nlohmann;
using namespace winrt;
using namespace std;

#pragma intrinsic(__ll_lshift)

#endif
```
