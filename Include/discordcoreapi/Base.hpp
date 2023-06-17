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

#ifndef NO_UNIQUE_ADDRESS
	#ifdef __has_cpp_attribute
		#if __has_cpp_attribute(no_unique_address)
			#ifdef _MSC_VER
				#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
			#else
				#define NO_UNIQUE_ADDRESS [[no_unique_address]]
			#endif
		#else
			#define NO_UNIQUE_ADDRESS
		#endif
	#else
		#define NO_UNIQUE_ADDRESS
	#endif
#endif

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

namespace Globals {

	template<typename ValueType> class AllocWrapper {
	  public:
		using value_type = ValueType;
		using pointer = value_type*;
		using allocator = std::pmr::polymorphic_allocator<value_type>;
		using allocator_traits = std::allocator_traits<allocator>;

		inline constexpr AllocWrapper() noexcept = default;

		inline constexpr AllocWrapper& operator=(AllocWrapper&& other) noexcept {
			return *this;
		};

		inline constexpr AllocWrapper(AllocWrapper&& other) noexcept {};

		inline constexpr AllocWrapper& operator=(const AllocWrapper& other) noexcept {
			return *this;
		};

		inline constexpr AllocWrapper(const AllocWrapper& other) noexcept {};

		inline constexpr pointer allocate(size_t count) noexcept {
			return allocTraits.allocate(alloc, count);
		}

		inline constexpr void deallocate(pointer ptr, size_t count) noexcept {
			allocTraits.deallocate(alloc, ptr, count);
		}

		template<typename... Args> inline constexpr void construct(pointer ptr, Args... args) noexcept {
			allocTraits.construct(alloc, ptr, args...);
		}

		inline constexpr void destroy(pointer ptr) noexcept {
			allocTraits.destroy(alloc, ptr);
		}

	  protected:
		NO_UNIQUE_ADDRESS allocator_traits allocTraits{};
		NO_UNIQUE_ADDRESS allocator alloc{};
	};

	enum class MessageType {
		General = 0,
		Https = 1,
		WebSocket = 2,
	};

	DiscordCoreAPI_Dll inline std::string shiftToBrightGreen() {
		return std::string("\033[1;40;92m");
	}

	DiscordCoreAPI_Dll inline std::string shiftToBrightBlue() {
		return std::string("\033[1;40;96m");
	}

	DiscordCoreAPI_Dll inline std::string shiftToBrightRed() {
		return std::string("\033[1;40;91m");
	}

	DiscordCoreAPI_Dll inline std::string reset() {
		return std::string("\033[0m");
	}

	class MessagePrinter {
	  public:
		inline MessagePrinter() noexcept = default;

		template<typename ValueType> inline static void initialize(const ValueType& other, std::ostream& outputStreamNew = std::cout,
			std::ostream& errorStreamNew = std::cerr) noexcept {
			doWePrintGeneralErrors.store(other.doWePrintHttpsSuccessMessages());
			doWePrintGeneralSuccesses.store(other.doWePrintGeneralSuccessMessages());
			doWePrintHttpsErrors.store(other.doWePrintHttpsErrorMessages());
			doWePrintHttpsSuccesses.store(other.doWePrintHttpsSuccessMessages());
			doWePrintWebSocketErrors.store(other.doWePrintWebSocketErrorMessages());
			doWePrintWebSocketSuccesses.store(other.doWePrintWebSocketSuccessMessages());
			outputStream = &outputStreamNew;
			errorStream = &errorStreamNew;
		}

		template<MessageType messageType>
		inline static void printError(const std::string& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case MessageType::General: {
					if (doWePrintGeneralErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "General Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case MessageType::Https: {
					if (doWePrintHttpsErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "Https Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case MessageType::WebSocket: {
					if (doWePrintWebSocketErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "WebSocket Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
			}
		}

		template<MessageType messageType>
		inline static void printSuccess(const std::string& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case MessageType::General: {
					if (doWePrintGeneralSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightBlue() << "General Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case MessageType::Https: {
					if (doWePrintHttpsSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "Https Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case MessageType::WebSocket: {
					if (doWePrintWebSocketSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "WebSocket Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
			}
		}

	  protected:
		inline static std::atomic_bool doWePrintHttpsSuccesses{};
		inline static std::atomic_bool doWePrintHttpsErrors{};
		inline static std::atomic_bool doWePrintWebSocketSuccesses{};
		inline static std::atomic_bool doWePrintWebSocketErrors{};
		inline static std::atomic_bool doWePrintGeneralSuccesses{};
		inline static std::atomic_bool doWePrintGeneralErrors{};
		inline static std::ostream* outputStream{};
		inline static std::ostream* errorStream{};
		inline static std::mutex accessMutex{};
	};
}
