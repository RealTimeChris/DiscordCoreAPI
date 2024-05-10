/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// Base.hpp - Header for the some of the base definitions and includes.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file Base.hpp
#pragma once

#if !defined(__GNUC__)
	#pragma warning(disable : 4710)
	#pragma warning(disable : 4711)
	#pragma warning(disable : 4251)
	#pragma warning(disable : 4371)
	#pragma warning(disable : 4514)
	#pragma warning(disable : 4623)
	#pragma warning(disable : 4625)
	#pragma warning(disable : 4626)
	#pragma warning(disable : 4820)
	#pragma warning(disable : 5267)
	#pragma warning(disable : 5026)
	#pragma warning(disable : 5027)
	#pragma warning(disable : 5045)
	#pragma warning(disable : 5246)
#endif

#if defined(__clang__) && defined(NDEBUG) && !defined(DCA_INLINE)
	#define DCA_INLINE inline __attribute__((always_inline))
#elif !defined(DCA_INLINE)
	#define DCA_INLINE inline
#endif

#if !defined(DCA_CPU_INSTRUCTIONS)
	#define DCA_CPU_INSTRUCTIONS 0
#endif

#if !defined(DCA_CHECK_FOR_INSTRUCTION)
	#define DCA_CHECK_FOR_INSTRUCTION(x) (DCA_CPU_INSTRUCTIONS & x)
#endif

#if !defined(DCA_CHECK_FOR_AVX)
	#define DCA_CHECK_FOR_AVX(x) (DCA_CPU_INSTRUCTIONS >= x)
#endif

#if !defined(DCA_NEON)
	#define DCA_NEON (1 << 4)
#endif
#if !defined(DCA_AVX)
	#define DCA_AVX (1 << 5)
#endif
#if !defined(DCA_AVX2)
	#define DCA_AVX2 (1 << 6)
#endif
#if !defined(DCA_AVX512)
	#define DCA_AVX512 (1 << 7)
#endif

#if defined _WIN32
	#if !defined DiscordCoreAPI_EXPORTS_NOPE
		#if defined DiscordCoreAPI_EXPORTS
			#if !defined DiscordCoreAPI_Dll
				#define DiscordCoreAPI_Dll __declspec(dllexport)
			#endif
		#else
			#if !defined DiscordCoreAPI_Dll
				#define DiscordCoreAPI_Dll __declspec(dllimport)
			#endif
		#endif
	#else
		#define DiscordCoreAPI_Dll
	#endif
	#if !defined WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#if !defined WINRT_LEAN_AND_MEAN
		#define WINRT_LEAN_AND_MEAN
	#endif
	#if !defined(NOMINMAX)
		#define NOMINMAX
	#endif
	#include <chrono>
DCA_INLINE tm getTime(time_t time) {
	tm timeNew{};
	gmtime_s(&timeNew, &time);
	return timeNew;
}
	#include <WinSock2.h>
#else
	#if !defined DiscordCoreAPI_Dll
		#define DiscordCoreAPI_Dll
	#endif
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <pthread.h>
	#include <cstdint>
	#include <cstring>
	#include <time.h>
	#include <ctime>
DCA_INLINE tm getTime(time_t time) {
	return *gmtime(&time);
}
#endif

#include <jsonifier/Index.hpp>

#include <source_location>
#include <shared_mutex>
#include <immintrin.h>
#include <functional>
#include <semaphore>
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

using namespace std::literals;

/**
 * \defgroup main_endpoints Main Endpoints
/// @brief For all of the discord api's endpoints.
 */

/**
 * \defgroup voice_connection Voice Connection
/// @brief For all of the voice connection related stuff.
 */

/**
 * \defgroup discord_events Discord Events
/// @brief For all of the events that could be sent by the discord api.
 */

/**
 * \defgroup utilities Utilities
/// @brief For utility classes/functions.
 */

/**
 * \defgroup foundation_entities Foundation Entities
/// @brief For all of the building blocks of the main endpoints.
 */

/**
 * \defgroup discord_events Discord Events
/// @brief For all of events that could be sent by discord's websockets.
 */

/**
 * \defgroup discord_core_internal DiscordCoreInternal
/// @brief For all of the internal api stuff.
 */

/**
 * @brief The main namespace for the forward-facing interfaces.
 *
 * \ingroup discord_core_api
 */

namespace discord_core_api {

	DCA_INLINE thread_local jsonifier::jsonifier_core<false> parser{};

	template<typename value_type> using stop_watch = jsonifier_internal::stop_watch<value_type>;
	using sys_clock								   = std::chrono::system_clock;
	using hrclock								   = std::chrono::high_resolution_clock;
	using milliseconds							   = std::chrono::duration<int64_t, std::milli>;
	using microseconds							   = std::chrono::duration<int64_t, std::micro>;
	using nanoseconds							   = std::chrono::duration<int64_t, std::nano>;
	using seconds								   = std::chrono::duration<int64_t, std::ratio<1, 1>>;

	enum class print_message_type {
		general	  = 0,
		https	  = 1,
		websocket = 2,
	};

	constexpr jsonifier::string_view shiftToBrightGreen() {
		return "\033[1;40;92m";
	}

	constexpr jsonifier::string_view shiftToBrightBlue() {
		return "\033[1;40;96m";
	}

	constexpr jsonifier::string_view shiftToBrightRed() {
		return "\033[1;40;91m";
	}

	constexpr jsonifier::string_view reset() {
		return "\033[0m";
	}

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief Class for printing different types of messages to output and error streams.
	class message_printer {
	  public:
		DCA_INLINE message_printer() = default;

		/// @brief Initialize the message_printer with configuration settings and output/error streams.
		/// @tparam value_type the type containing configuration settings.
		/// @param other an instance of value_type with configuration settings.
		template<typename value_type> DCA_INLINE static void initialize(const value_type& other) {
			doWePrintGeneralErrors.store(other.doWePrintGeneralErrorMessages(), std::memory_order_release);
			doWePrintGeneralSuccesses.store(other.doWePrintGeneralSuccessMessages(), std::memory_order_release);
			doWePrintHttpsErrors.store(other.doWePrintHttpsErrorMessages(), std::memory_order_release);
			doWePrintHttpsSuccesses.store(other.doWePrintHttpsSuccessMessages(), std::memory_order_release);
			doWePrintWebSocketErrors.store(other.doWePrintWebSocketErrorMessages(), std::memory_order_release);
			doWePrintWebSocketSuccesses.store(other.doWePrintWebSocketSuccessMessages(), std::memory_order_release);
			outputStream = other.getOutputStream();
			errorStream	 = other.getErrorStream();
		}

		/// @brief Print an error message of the specified type.
		/// @tparam messageType the type of message to print.
		/// @param what the error message.
		/// @param where the source location where the error occurred (default: current source location).
		template<print_message_type messageType, typename string_type>
		DCA_INLINE static void printError(const string_type& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case print_message_type::general: {
					if (doWePrintGeneralErrors.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "General error, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									 << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case print_message_type::websocket: {
					if (doWePrintWebSocketErrors.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "WebSocket error, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									 << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case print_message_type::https: {
					if (doWePrintHttpsErrors.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "Https error, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									 << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
			}
		}

		/// @brief Print a success message of the specified type.
		/// @tparam messageType the type of message to print.
		/// @param what the success message.
		/// @param where the source location where the success occurred (default: current source location).
		template<print_message_type messageType, typename string_type>
		DCA_INLINE static void printSuccess(const string_type& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case print_message_type::general: {
					if (doWePrintGeneralSuccesses.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightBlue() << "General success, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									  << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case print_message_type::websocket: {
					if (doWePrintWebSocketSuccesses.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "WebSocket success, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									  << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case print_message_type::https: {
					if (doWePrintHttpsSuccesses.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "Https success, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									  << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
			}
		}

	  protected:
		DCA_INLINE static std::atomic_bool doWePrintWebSocketSuccesses{};///< Flag to control printing of websocket success messages.
		DCA_INLINE static std::atomic_bool doWePrintGeneralSuccesses{};///< Flag to control printing of general success messages.
		DCA_INLINE static std::atomic_bool doWePrintWebSocketErrors{};///< Flag to control printing of websocket error messages.
		DCA_INLINE static std::atomic_bool doWePrintHttpsSuccesses{};///< Flag to control printing of https success messages.
		DCA_INLINE static std::atomic_bool doWePrintGeneralErrors{};///< Flag to control printing of general error messages.
		DCA_INLINE static std::atomic_bool doWePrintHttpsErrors{};///< Flag to control printing of https error messages.
		DCA_INLINE static std::ostream* outputStream{};///< Pointer to the output stream for message printing.
		DCA_INLINE static std::ostream* errorStream{};///< Pointer to the error stream for message printing.
		DCA_INLINE static std::mutex accessMutex{};///< Mutex for thread-safe access to shared resources.
	};

	/// @brief Time formatting methods.
	enum class time_format : char {
		long_date		= 'D',///< "20 April 2021" - Long Date
		long_date_time	= 'F',///< "Tuesday, 20 April 2021 16:20" - Long Date/Time
		long_time		= 'T',///< "16:20:30" - Long Time
		short_date		= 'd',///< "20/04/2021" - Short Date
		short_date_time = 'f',///< "20 April 2021 16:20" - Short Date/Time
		short_time		= 't',///< "16:20" - Short Time
	};

	class time_stamp;

	/// @brief A base class for handling time stamps and conversions.
	/// @tparam value_type the value type to be used with the time stamp.
	template<typename value_type> class time_stamp_base {
	  public:
		static constexpr uint64_t msPerSecond{ 1000ULL };
		static constexpr uint64_t secondsPerMinute{ 60ULL };
		static constexpr uint64_t minutesPerHour{ 60ULL };
		static constexpr uint64_t msPerMinute{ msPerSecond * secondsPerMinute };
		static constexpr uint64_t msPerHour{ msPerMinute * minutesPerHour };
		static constexpr uint64_t secondsPerHour{ minutesPerHour * secondsPerMinute };
		static constexpr uint64_t hoursPerDay{ 24ULL };
		static constexpr uint64_t secondsPerDay{ secondsPerHour * hoursPerDay };
		static constexpr uint64_t daysPerMonth{ 30ULL };
		static constexpr uint64_t secondsPerMonth{ secondsPerDay * daysPerMonth };
		static constexpr uint64_t daysPerYear{ 365ULL };
		static constexpr uint64_t secondsPerYear{ secondsPerDay * daysPerYear };
		/// @brief Checks if the time stamp is equal to a string representation.
		/// @param other the string to compare with.
		/// @return true if equal, false otherwise.
		DCA_INLINE bool operator==(jsonifier::string_view other) const {
			return static_cast<jsonifier::string>(*static_cast<const value_type*>(this)) == other;
		}

		/// @brief Converts given time values into a future iso8601 time stamp.
		/// @param minutesToAdd number of minutes to add.
		/// @param hoursToAdd number of hours to add.
		/// @param daysToAdd number of days to add.
		/// @param monthsToAdd number of months to add.
		/// @param yearsToAdd number of years to add.
		/// @param timeFormat format for the resulting time stamp.
		/// @return iso8601 time stamp string.
		DCA_INLINE static jsonifier::string convertToFutureISO8601TimeStamp(uint64_t minutesToAdd, uint64_t hoursToAdd, uint64_t daysToAdd, uint64_t monthsToAdd, uint64_t yearsToAdd,
			time_format timeFormat) {
			std::time_t result = std::time(nullptr);
			uint64_t secondsToAdd = (yearsToAdd * secondsPerYear) + (monthsToAdd * secondsPerMonth) + (daysToAdd * secondsPerDay) + ((hoursToAdd + 8) * secondsPerHour) +
				(minutesToAdd * secondsPerMinute);
			result += secondsToAdd;
			std::tm resultTwo{ getTime(result) };
			jsonifier::string returnString{};
			if (resultTwo.tm_isdst) {
				if (resultTwo.tm_hour + 4ULL >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getMsSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 4ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			} else {
				if (resultTwo.tm_hour + 5ULL >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getMsSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 5ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			}
			return returnString;
		}

		/// @brief Converts the current time into an iso8601 time stamp.
		/// @param timeFormat format for the resulting time stamp.
		/// @return iso8601 time stamp string.
		DCA_INLINE static jsonifier::string convertToCurrentISO8601TimeStamp(time_format timeFormat) {
			std::time_t result = std::time(nullptr);
			std::tm resultTwo{ getTime(result) };
			jsonifier::string returnString{};
			if (resultTwo.tm_isdst) {
				if (resultTwo.tm_hour + 4ULL >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getMsSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 4ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			} else {
				if (resultTwo.tm_hour + 5ULL >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getMsSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 5ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			}
			return returnString;
		}

		/// @brief Checks if a certain time duration has elapsed.
		/// @param days number of days for elapsed time.
		/// @param hours number of hours for elapsed time.
		/// @param minutes number of minutes for elapsed time.
		/// @return true if the specified time has elapsed, otherwise false.
		DCA_INLINE bool hasTimeElapsed(int64_t days, int64_t hours, int64_t minutes) const {
			int64_t startTimeRaw{ static_cast<int64_t>(convertTimeStampToTimeUnits(static_cast<jsonifier::string>(*static_cast<const value_type*>(this)))) };
			startTimeRaw	 = std::chrono::local_time<std::chrono::milliseconds>(std::chrono::milliseconds{ startTimeRaw }).time_since_epoch().count();
			auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto targetElapsedTime =
				((static_cast<int64_t>(days) * secondsPerDay) + ((static_cast<int64_t>(hours)) * secondsPerHour) + (static_cast<int64_t>(minutes) * secondsPerMinute)) * 1000LL;
			auto actualElapsedTime = currentTime - startTimeRaw;
			if (actualElapsedTime <= 0) {
				return false;
			}
			if (actualElapsedTime >= targetElapsedTime) {
				return true;
			} else {
				return false;
			}
		}

		/// @brief Converts milliseconds into a human-readable duration string.
		/// @param durationInMs duration in milliseconds to convert.
		/// @return human-readable duration string.
		DCA_INLINE static jsonifier::string convertMsToDurationString(uint64_t durationInMs) {
			jsonifier::string newString{};
			uint64_t hoursLeft	 = static_cast<uint64_t>(trunc(durationInMs / msPerHour));
			uint64_t minutesLeft = static_cast<uint64_t>(trunc((durationInMs % msPerHour) / msPerMinute));
			uint64_t secondsLeft = static_cast<uint64_t>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
			if (hoursLeft >= 1) {
				newString += jsonifier::toString(hoursLeft) + " hours, ";
				newString += jsonifier::toString(minutesLeft) + " minutes, ";
				newString += jsonifier::toString(secondsLeft) + " seconds.";
			} else if (minutesLeft >= 1) {
				newString += jsonifier::toString(minutesLeft) + " minutes, ";
				newString += jsonifier::toString(secondsLeft) + " seconds.";
			} else {
				newString += jsonifier::toString(secondsLeft) + " seconds.";
			}
			return newString;
		}

		/// @brief Gets an iso8601 time stamp string based on the provided time format.
		/// @param timeFormat format for the resulting time stamp.
		/// @param inputTime input time value.
		/// @return iso8601 time stamp string.
		DCA_INLINE static jsonifier::string getISO8601TimeStamp(time_format timeFormat, uint64_t inputTime) {
			uint64_t timeValue = static_cast<uint64_t>(inputTime) / 1000ULL;
			time_t rawTime(static_cast<time_t>(timeValue));
			std::tm resultTwo{ getTime(rawTime) };
			jsonifier::string timeStamp{};
			timeStamp.resize(48);
			switch (timeFormat) {
				case time_format::long_date: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case time_format::long_date_time: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%FT%T", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case time_format::long_time: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%T", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case time_format::short_date: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case time_format::short_date_time: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case time_format::short_time: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%R", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				default: {
					break;
				}
			}
			return timeStamp;
		}

		/// @brief Gets the time since the unix epoch for the specified date and time.
		/// @param year year.
		/// @param month month.
		/// @param day day.
		/// @param hour hour.
		/// @param minute minute.
		/// @param second second.
		/// @return time since unix epoch in milliseconds.
		DCA_INLINE static uint64_t getMsSinceEpoch(uint64_t year, uint64_t month, uint64_t day, uint64_t hour, uint64_t minute, uint64_t second) {
			static constexpr uint64_t secondsInJan{ 31ULL * secondsPerDay };
			static constexpr uint64_t secondsInFeb{ 28ULL * secondsPerDay };
			static constexpr uint64_t secondsInMar{ 31ULL * secondsPerDay };
			static constexpr uint64_t secondsInApr{ 30ULL * secondsPerDay };
			static constexpr uint64_t secondsInMay{ 31ULL * secondsPerDay };
			static constexpr uint64_t secondsInJun{ 30ULL * secondsPerDay };
			static constexpr uint64_t secondsInJul{ 31ULL * secondsPerDay };
			static constexpr uint64_t secondsInAug{ 31ULL * secondsPerDay };
			static constexpr uint64_t secondsInSep{ 30ULL * secondsPerDay };
			static constexpr uint64_t secondsInOct{ 31ULL * secondsPerDay };
			static constexpr uint64_t secondsInNov{ 30ULL * secondsPerDay };
			static constexpr uint64_t secondsInDec{ 31ULL * secondsPerDay };
			seconds value{};
			for (uint64_t x = 1970ULL; x < year; ++x) {
				value += seconds{ secondsInJan };
				value += seconds{ secondsInFeb };
				value += seconds{ secondsInMar };
				value += seconds{ secondsInApr };
				value += seconds{ secondsInMay };
				value += seconds{ secondsInJun };
				value += seconds{ secondsInJul };
				value += seconds{ secondsInAug };
				value += seconds{ secondsInSep };
				value += seconds{ secondsInOct };
				value += seconds{ secondsInNov };
				value += seconds{ secondsInDec };
				if (x % 4ULL == 0ULL) {
					value += seconds{ secondsPerDay };
				}
			}
			if (month > 0) {
				value += seconds{ static_cast<uint64_t>((day - 1ULL) * secondsPerDay) };
				value += seconds{ static_cast<uint64_t>(hour * secondsPerHour) };
				value += seconds{ static_cast<uint64_t>(minute * secondsPerMinute) };
				value += seconds{ second };
			}
			if (month > 1ULL) {
				value += seconds{ secondsInJan };
			}
			if (month > 2ULL) {
				value += seconds{ secondsInFeb };
			}
			if (month > 3ULL) {
				value += seconds{ secondsInMar };
			}
			if (month > 4ULL) {
				value += seconds{ secondsInApr };
			}
			if (month > 5ULL) {
				value += seconds{ secondsInMay };
			}
			if (month > 6ULL) {
				value += seconds{ secondsInJun };
			}
			if (month > 7ULL) {
				value += seconds{ secondsInJul };
			}
			if (month > 8ULL) {
				value += seconds{ secondsInAug };
			}
			if (month > 9ULL) {
				value += seconds{ secondsInSep };
			}
			if (month > 10ULL) {
				value += seconds{ secondsInOct };
			}
			if (month > 11ULL) {
				value += seconds{ secondsInNov };
			}
			return static_cast<uint64_t>(std::chrono::duration_cast<milliseconds>(value).count());
		}

	  protected:
		/// @brief Converts a string time stamp into a uint64_t time value.
		/// @param originalTimeStamp original time stamp string.
		/// @return converted time value in milliseconds.
		DCA_INLINE static uint64_t convertTimeStampToTimeUnits(jsonifier::string_view originalTimeStamp) {
			if (originalTimeStamp != "" && originalTimeStamp.size() >= 19) {
				auto newValue =
					getMsSinceEpoch(jsonifier::strToUint64(originalTimeStamp.substr(0ULL, 4ULL).data()), jsonifier::strToUint64(originalTimeStamp.substr(5ULL, 6ULL).data()),
						jsonifier::strToUint64(originalTimeStamp.substr(8ULL, 9ULL).data()), jsonifier::strToUint64(originalTimeStamp.substr(11ULL, 12ULL).data()),
						jsonifier::strToUint64(originalTimeStamp.substr(14ULL, 15ULL).data()), jsonifier::strToUint64(originalTimeStamp.substr(17ULL, 18ULL).data()));
				return newValue;
			} else {
				return static_cast<uint64_t>(std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()).count());
			}
		}

		/// @brief Converts a string time stamp into a uint64_t time value.
		/// @param stringTimeStamp string time stamp to convert.
		/// @return converted time value in milliseconds.
		DCA_INLINE uint64_t convertStringToUintTimeStamp(jsonifier::string_view stringTimeStamp) const {
			return convertTimeStampToTimeUnits(stringTimeStamp);
		}

		/// @brief Converts a uint64_t time value into a string time stamp.
		/// @param uintTimeStamp time value to convert.
		/// @return string time stamp.
		DCA_INLINE jsonifier::string convertUintToStringTimeStamp(uint64_t uintTimeStamp) const {
			return getISO8601TimeStamp(time_format::long_date_time, uintTimeStamp);
		}
	};

	/// @brief A class that extends time_stamp_base to provide additional functionality.
	class time_stamp : public time_stamp_base<time_stamp> {
	  public:
		template<typename value_type> friend class time_stamp_base;

		/// @brief Default constructor for time_stamp.
		DCA_INLINE time_stamp() = default;

		/// @brief Move assignment operator to move a string value into time_stamp.
		/// @param valueNew the string value to move.
		/// @return reference to the modified time_stamp instance.
		DCA_INLINE time_stamp& operator=(jsonifier::string_view valueNew) {
			value = convertStringToUintTimeStamp(valueNew);
			return *this;
		}

		/// @brief Move constructor to create a time_stamp instance by moving a string value.
		/// @param valueNew the string value to move.
		DCA_INLINE time_stamp(jsonifier::string_view valueNew) {
			*this = valueNew;
		}

		/// @brief Assignment operator to assign a uint64_t value to time_stamp.
		/// @param valueNew the new uint64_t value to assign.
		/// @return reference to the modified time_stamp instance.
		DCA_INLINE time_stamp& operator=(uint64_t valueNew) {
			value = valueNew;
			return *this;
		}

		/// @brief Constructor to create a time_stamp instance from a uint64_t value.
		/// @param valueNew the uint64_t value to create the instance from.
		DCA_INLINE time_stamp(uint64_t valueNew) {
			*this = valueNew;
		}

		/// @brief Conversion operator to convert time_stamp to int64_t.
		/// @return the int64_t value represented by the time_stamp instance.
		DCA_INLINE operator uint64_t() const {
			return value;
		}

		/// @brief Conversion operator to convert time_stamp to jsonifier::string.
		/// @return the jsonifier::string value represented by the time_stamp instance.
		DCA_INLINE operator jsonifier::string() const {
			return convertUintToStringTimeStamp(value);
		}

	  protected:
		uint64_t value{};///< The value stored in the time_stamp instance.
	};

	class snowflake;

	/// @brief A class for converting snowflake id's into the data structures they represent.
	/// @tparam value_type The type of value being acquired.
	template<typename value_type = void> class to_entity {
		DCA_INLINE static value_type toEntity(snowflake initialId, snowflake additionalId);

		DCA_INLINE static value_type toEntity(snowflake initialId);
	};

	/// @brief A class representing a snowflake identifier with various operations.
	class snowflake {
	  public:
		/// @brief Default constructor for snowflake.
		DCA_INLINE snowflake() = default;

		/// @brief A class for converting snowflake id's into the data structures they represent.
		/// @tparam value_type The type of value being acquired.
		/// @return value_type If successful, the intended data structure.
		template<typename value_type> DCA_INLINE value_type toEntity() {
			return to_entity<value_type>{}.toEntity(*this);
		}

		/// @brief A class for converting snowflake id's into the data structures they represent.
		/// @param additionalId For certain classes, a second Id for collecting the data structure.
		/// @tparam value_type The type of value being acquired.
		/// @return value_type If successful, the intended data structure.
		template<typename value_type> DCA_INLINE value_type toEntity(snowflake additionalId) {
			return to_entity<value_type>{}.toEntity(*this, additionalId);
		}

		/// @brief Assignment operator to assign a string value to snowflake.
		/// @param other the string value to assign.
		/// @return reference to the modified snowflake instance.
		DCA_INLINE snowflake& operator=(const jsonifier::string& other) {
			if (other.size() > 0) {
				for (auto& value: other) {
					if (!std::isdigit(static_cast<uint8_t>(value))) {
						return *this;
					}
				}
				id = jsonifier::strToUint64(other);
			}
			return *this;
		}

		/// @brief Constructor to create a snowflake instance from a string value.
		/// @param other the string value to create the instance from.
		DCA_INLINE snowflake(const jsonifier::string& other) {
			*this = other;
		}

		/// @brief Assignment operator to assign a uint64_t value to snowflake.
		/// @param other the uint64_t value to assign.
		/// @return reference to the modified snowflake instance.
		DCA_INLINE snowflake& operator=(uint64_t other) {
			id = other;
			return *this;
		}

		/// @brief Constructor to create a snowflake instance from a uint64_t value.
		/// @param other the uint64_t value to create the instance from.
		DCA_INLINE snowflake(uint64_t other) {
			*this = other;
		}

		/// @brief Conversion operator to convert snowflake to jsonifier::string.
		/// @return the jsonifier::string value represented by the snowflake instance.
		DCA_INLINE explicit operator jsonifier::string() const {
			return jsonifier::toString(id);
		}

		/// @brief Explicit conversion operator to convert snowflake to uint64_t.
		/// @return the uint64_t value represented by the snowflake instance.
		DCA_INLINE explicit operator const uint64_t&() const {
			return id;
		}

		DCA_INLINE bool operator==(const snowflake& other) const {
			return id == other.id;
		}

		DCA_INLINE bool operator==(jsonifier::string_view other) const {
			return operator jsonifier::string() == other;
		}

		DCA_INLINE bool operator==(uint64_t other) const {
			return id == other;
		}

		/// @brief Concatenation operator to concatenate snowflake and a string value.
		/// @tparam value_type the type of the string value.
		/// @param rhs the string value to concatenate.
		/// @return the concatenated string.
		template<jsonifier::concepts::string_t value_type> DCA_INLINE jsonifier::string operator+(jsonifier::string_view rhs) const {
			jsonifier::string newString{ operator jsonifier::string() };
			newString += rhs;
			return newString;
		}

		/// @brief Friend function to concatenate snowflake and a string value.
		/// @param lhs the snowflake instance.
		/// @param other the string value to concatenate.
		/// @return the concatenated string.
		DCA_INLINE friend jsonifier::string operator+(const snowflake& lhs, jsonifier::string_view other) {
			jsonifier::string lhsNew{ static_cast<jsonifier::string>(lhs) };
			lhsNew += other;
			return lhsNew;
		}

		/// @brief Friend function to concatenate two values.
		/// @tparam value_type01 the type of the first value.
		/// @tparam value_type02 the type of the second value.
		/// @param lhs the first value.
		/// @param rhs the second value.
		/// @return the concatenated string.
		template<jsonifier::concepts::string_t value_type01, typename value_type02> friend DCA_INLINE jsonifier::string operator+(const value_type01& lhs, const value_type02& rhs) {
			jsonifier::string newString{ lhs };
			newString += rhs.operator jsonifier::string();
			return newString;
		}

		/// @brief Friend function to concatenate two values.
		/// @tparam value_type01 the type of the first value.
		/// @tparam value_type02 the type of the second value.
		/// @param lhs the first value.
		/// @param rhs the second value.
		/// @return the concatenated string.
		template<uint64_t size> friend DCA_INLINE jsonifier::string operator+(const char (&lhs)[size], const snowflake& rhs) {
			jsonifier::string newString{ lhs };
			newString += rhs.operator jsonifier::string();
			return newString;
		}

		/// @brief Converts the snowflake id into a time and date stamp.
		/// @return a jsonifier::string containing the timestamp.
		DCA_INLINE jsonifier::string getCreatedAtTimeStamp() {
			uint64_t timeStamp{ static_cast<uint64_t>((id >> 22) + 1420070400000) };
			return time_stamp::getISO8601TimeStamp(time_format::long_date_time, timeStamp);
		}

	  protected:
		uint64_t id{};///< The snowflake id.
	};

	DCA_INLINE std::ostream& operator<<(std::ostream& os, snowflake sf) {
		os << sf.operator jsonifier::string();
		return os;
	}

	/// @brief An exception class derived from std::runtime_error for dca-related exceptions.
	struct dca_exception : public std::runtime_error {
		/// @brief Constructor to create a dca_exception with an error message and optional source location.
		/// @param error the error message.
		/// @param location the source location of the exception (default: current location).
		DCA_INLINE dca_exception(jsonifier::string_view error, std::source_location location = std::source_location::current()) : std::runtime_error{ "No error." } {
			auto newError  = jsonifier::string{ "Thrown from: " };
			auto newString = location.file_name();
			if (newString) {
				newError += newString;
			}
			newError += jsonifier::string{ " (" } + jsonifier::toString(location.line()) + jsonifier::string{ ":" };
			newError += jsonifier::toString(location.column()) + jsonifier::string{ ")\n" };
			newError += error;
			if (!newError.empty()) {
				*static_cast<std::runtime_error*>(this) = std::runtime_error{ std::string{ newError } };
			}
		};
	};

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// @brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class stream_type { none = 0, client = 1, server = 2 };

	/**@}*/
}
