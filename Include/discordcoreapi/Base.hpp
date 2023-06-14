/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// Base.hpp - Header for the some of the base definitions and includes.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file Etf.hpp

#pragma once

#pragma once
#pragma warning(push)
#pragma warning(disable : 4275)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#pragma warning(disable : 4996)

#ifdef _WIN32
#ifndef DiscordCoreAPI_EXPORTS_NOPE
	#ifdef DiscordCoreAPI_EXPORTS
		#ifndef DiscordCoreAPI_Dll
			#define DiscordCoreAPI_Dll __declspec(dllexport)
		#endif
	#else
		#ifndef DiscordCoreAPI_Dll
			#define DiscordCoreAPI_Dll __declspec(dllimport)
		#endif
		#endif 
#else
		#define DiscordCoreAPI_Dll
#endif
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef WINRT_LEAN_AND_MEAN
		#define WINRT_LEAN_AND_MEAN
	#endif
	#include <WinSock2.h>
#elif __linux__
	#ifndef DiscordCoreAPI_Dll
		#define DiscordCoreAPI_Dll
	#endif
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <pthread.h>
	#include <cstdint>
	#include <cstring>
	#include <time.h>
	#include <ctime>
#endif

#include <source_location>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <immintrin.h>
#include <functional>
#include <concepts>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <atomic>
#include <random>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <array>
#include <map>
#include <set>

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min

#endif