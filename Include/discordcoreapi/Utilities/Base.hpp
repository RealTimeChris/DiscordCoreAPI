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
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <chrono>
inline std::tm getCurrentTimeVal(time_t& result) {
	std::tm resultTwo{};
	localtime_s(&resultTwo, &result);
	return resultTwo;
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
inline std::tm getCurrentTimeVal(time_t& result) {
	std::tm resultTwo{ *localtime(&result) };
	return resultTwo;
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
/// @brief For all of the Discord API's endpoints.
 */

/**
 * \defgroup voice_connection Voice Connection
/// @brief For all of the voice connection related stuff.
 */

/**
 * \defgroup discord_events Discord Events
/// @brief For all of the events that could be sent by the Discord API.
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
/// @brief For all of events that could be sent by Discord's Websockets.
 */

/**
 * \defgroup discord_core_internal DiscordCore Internal
/// @brief For all of the internal api stuff.
 */

namespace DiscordCoreAPI {

	inline thread_local jsonifier::jsonifier_core parser{};

	template<typename ValueType> using StopWatch = jsonifier_internal::stop_watch<ValueType>;
	using SysClock								 = std::chrono::system_clock;
	using HRClock								 = std::chrono::high_resolution_clock;
	using Milliseconds							 = std::chrono::milliseconds;
	using Microseconds							 = std::chrono::microseconds;
	using Nanoseconds							 = std::chrono::nanoseconds;
	using Seconds								 = std::chrono::seconds;

	enum class PrintMessageType {
		General	  = 0,
		Https	  = 1,
		WebSocket = 2,
	};

	inline constexpr jsonifier::string_view shiftToBrightGreen() {
		return "\033[1;40;92m";
	}

	inline constexpr jsonifier::string_view shiftToBrightBlue() {
		return "\033[1;40;96m";
	}

	inline constexpr jsonifier::string_view shiftToBrightRed() {
		return "\033[1;40;91m";
	}

	inline constexpr jsonifier::string_view reset() {
		return "\033[0m";
	}

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief Class for printing different types of messages to output and error streams.
	class MessagePrinter {
	  public:
		inline MessagePrinter() = default;

		/// @brief Initialize the MessagePrinter with configuration settings and output/error streams.
		/// @tparam ValueType The type containing configuration settings.
		/// @param other An instance of ValueType with configuration settings.
		/// @param outputStreamNew The output stream to print messages to (default: std::cout).
		/// @param errorStreamNew The error stream to print messages to (default: std::cerr).
		template<typename ValueType> inline static void initialize(const ValueType& other, std::ostream& outputStreamNew = std::cout, std::ostream& errorStreamNew = std::cerr) {
			doWePrintGeneralErrors.store(other.doWePrintGeneralErrorMessages(), std::memory_order_release);
			doWePrintGeneralSuccesses.store(other.doWePrintGeneralSuccessMessages(), std::memory_order_release);
			doWePrintHttpsErrors.store(other.doWePrintHttpsErrorMessages(), std::memory_order_release);
			doWePrintHttpsSuccesses.store(other.doWePrintHttpsSuccessMessages(), std::memory_order_release);
			doWePrintWebSocketErrors.store(other.doWePrintWebSocketErrorMessages(), std::memory_order_release);
			doWePrintWebSocketSuccesses.store(other.doWePrintWebSocketSuccessMessages(), std::memory_order_release);
			outputStream = &outputStreamNew;
			errorStream	 = &errorStreamNew;
		}

		/// @brief Print an error message of the specified type.
		/// @tparam messageType The type of message to print.
		/// @param what The error message.
		/// @param where The source location where the error occurred (default: current source location).
		template<PrintMessageType messageType, typename StringType>
		inline static void printError(const StringType& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case PrintMessageType::General: {
					if (doWePrintGeneralErrors.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "General Error, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									 << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::WebSocket: {
					if (doWePrintWebSocketErrors.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "WebSocket Error, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									 << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::Https: {
					if (doWePrintHttpsErrors.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "Https Error, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									 << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
			}
		}

		/// @brief Print a success message of the specified type.
		/// @tparam messageType The type of message to print.
		/// @param what The success message.
		/// @param where The source location where the success occurred (default: current source location).
		template<PrintMessageType messageType, typename StringType>
		inline static void printSuccess(const StringType& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case PrintMessageType::General: {
					if (doWePrintGeneralSuccesses.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightBlue() << "General Success, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									  << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::WebSocket: {
					if (doWePrintWebSocketSuccesses.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "WebSocket Success, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									  << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::Https: {
					if (doWePrintHttpsSuccesses.load(std::memory_order_acquire)) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "Https Success, caught at: " << where.file_name() << ", " << where.line() << ":" << where.column()
									  << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
			}
		}

	  protected:
		inline static std::atomic_bool doWePrintHttpsSuccesses{};///< Flag to control printing of HTTPS success messages.
		inline static std::atomic_bool doWePrintHttpsErrors{};///< Flag to control printing of HTTPS error messages.
		inline static std::atomic_bool doWePrintWebSocketSuccesses{};///< Flag to control printing of WebSocket success messages.
		inline static std::atomic_bool doWePrintWebSocketErrors{};///< Flag to control printing of WebSocket error messages.
		inline static std::atomic_bool doWePrintGeneralSuccesses{};///< Flag to control printing of general success messages.
		inline static std::atomic_bool doWePrintGeneralErrors{};///< Flag to control printing of general error messages.
		inline static std::ostream* outputStream{};///< Pointer to the output stream for message printing.
		inline static std::ostream* errorStream{};///< Pointer to the error stream for message printing.
		inline static std::mutex accessMutex{};///< Mutex for thread-safe access to shared resources.
	};

	/// @brief Time formatting methods.
	enum class TimeFormat : char {
		LongDate	  = 'D',///< "20 April 2021" - Long Date
		LongDateTime  = 'F',///< "Tuesday, 20 April 2021 16:20" - Long Date/Time
		LongTime	  = 'T',///< "16:20:30" - Long Time
		ShortDate	  = 'd',///< "20/04/2021" - Short Date
		ShortDateTime = 'f',///< "20 April 2021 16:20" - Short Date/Time
		ShortTime	  = 't',///< "16:20" - Short Time
	};

	class TimeStampParse;

	/// @brief A base class for handling time stamps and conversions.
	/// @tparam ValueType The value type to be used with the time stamp.
	template<typename ValueType> class TimeStampBase {
	  public:
		/// @brief Checks if the time stamp is equal to a string representation.
		/// @param other The string to compare with.
		/// @return True if equal, false otherwise.
		inline bool operator==(jsonifier::string_view other) const {
			return *static_cast<const ValueType*>(this) == other;
		}

		/// @brief Converts given time values into a future ISO8601 time stamp.
		/// @param minutesToAdd Number of minutes to add.
		/// @param hoursToAdd Number of hours to add.
		/// @param daysToAdd Number of days to add.
		/// @param monthsToAdd Number of months to add.
		/// @param yearsToAdd Number of years to add.
		/// @param timeFormat Format for the resulting time stamp.
		/// @return ISO8601 time stamp string.
		inline static jsonifier::string convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd, int32_t yearsToAdd,
			TimeFormat timeFormat) {
			std::time_t result = std::time(nullptr);
			static constexpr int32_t secondsPerMinute{ 60 };
			static constexpr int32_t minutesPerHour{ 60 };
			static constexpr int32_t secondsPerHour{ minutesPerHour * secondsPerMinute };
			static constexpr int32_t hoursPerDay{ 24 };
			static constexpr int32_t secondsPerDay{ secondsPerHour * hoursPerDay };
			static constexpr int32_t daysPerMonth{ 30 };
			static constexpr int32_t secondsPerMonth{ secondsPerDay * daysPerMonth };
			static constexpr int32_t daysPerYear{ 365 };
			static constexpr int32_t secondsPerYear{ secondsPerDay * daysPerYear };
			int32_t secondsToAdd = (yearsToAdd * secondsPerYear) + (monthsToAdd * secondsPerMonth) + (daysToAdd * secondsPerDay) + ((hoursToAdd + 8) * secondsPerHour) +
				(minutesToAdd * secondsPerMinute);
			result += secondsToAdd;
			std::tm resultTwo{ getCurrentTimeVal(result) };
			jsonifier::string returnString{};
			if (resultTwo.tm_isdst) {
				if (resultTwo.tm_hour + 4 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getTimeSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 4ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			} else {
				if (resultTwo.tm_hour + 5 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getTimeSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 5ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			}
			return returnString;
		}

		/// @brief Converts the current time into an ISO8601 time stamp.
		/// @param timeFormat Format for the resulting time stamp.
		/// @return ISO8601 time stamp string.
		inline static jsonifier::string convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
			std::time_t result = std::time(nullptr);
			std::tm resultTwo{ getCurrentTimeVal(result) };
			jsonifier::string returnString{};
			if (resultTwo.tm_isdst) {
				if (resultTwo.tm_hour + 4 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getTimeSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900ULL, static_cast<uint64_t>(resultTwo.tm_mon) + 1ULL, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 4ULL, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			} else {
				if (resultTwo.tm_hour + 5 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				auto newValue =
					getTimeSinceEpoch(static_cast<uint64_t>(resultTwo.tm_year) + 1900, static_cast<uint64_t>(resultTwo.tm_mon) + 1, static_cast<uint64_t>(resultTwo.tm_mday),
						static_cast<uint64_t>(resultTwo.tm_hour) + 5, static_cast<uint64_t>(resultTwo.tm_min), static_cast<uint64_t>(resultTwo.tm_sec));
				returnString = getISO8601TimeStamp(timeFormat, newValue);
			}
			return returnString;
		}

		/// @brief Checks if a certain time duration has elapsed.
		/// @param days Number of days for elapsed time.
		/// @param hours Number of hours for elapsed time.
		/// @param minutes Number of minutes for elapsed time.
		/// @return True if the specified time has elapsed, otherwise false.
		inline bool hasTimeElapsed(uint64_t days, uint64_t hours, uint64_t minutes) const {
			uint64_t startTimeRaw{ convertTimeStampToTimeUnits(*static_cast<const ValueType*>(this)) };
			auto currentTime						   = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			static constexpr uint64_t secondsPerMinute = 60;
			static constexpr uint64_t secondsPerHour   = secondsPerMinute * 60;
			static constexpr uint64_t secondsPerDay	   = secondsPerHour * 24;
			auto targetElapsedTime =
				((static_cast<uint64_t>(days) * secondsPerDay) + (static_cast<uint64_t>(hours) * secondsPerHour) + (static_cast<uint64_t>(minutes) * secondsPerMinute)) * 1000;
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
		/// @param durationInMs Duration in milliseconds to convert.
		/// @return Human-readable duration string.
		inline static jsonifier::string convertMsToDurationString(uint64_t durationInMs) {
			jsonifier::string newString{};
			static constexpr uint64_t msPerSecond{ 1000 };
			static constexpr uint64_t secondsPerMinute{ 60 };
			static constexpr uint64_t minutesPerHour{ 60 };
			static constexpr uint64_t msPerMinute{ msPerSecond * secondsPerMinute };
			static constexpr uint64_t msPerHour{ msPerMinute * minutesPerHour };
			uint64_t hoursLeft	 = static_cast<uint64_t>(trunc(durationInMs / msPerHour));
			uint64_t minutesLeft = static_cast<uint64_t>(trunc((durationInMs % msPerHour) / msPerMinute));
			uint64_t secondsLeft = static_cast<uint64_t>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
			if (hoursLeft >= 1) {
				newString += jsonifier::toString(hoursLeft) + " Hours, ";
				newString += jsonifier::toString(minutesLeft) + " Minutes, ";
				newString += jsonifier::toString(secondsLeft) + " Seconds.";
			} else if (minutesLeft >= 1) {
				newString += jsonifier::toString(minutesLeft) + " Minutes, ";
				newString += jsonifier::toString(secondsLeft) + " Seconds.";
			} else {
				newString += jsonifier::toString(secondsLeft) + " Seconds.";
			}
			return newString;
		}

		/// @brief Gets an ISO8601 time stamp string based on the provided time format.
		/// @param timeFormat Format for the resulting time stamp.
		/// @param inputTime Input time value.
		/// @return ISO8601 time stamp string.
		inline static jsonifier::string getISO8601TimeStamp(TimeFormat timeFormat, uint64_t inputTime) {
			uint64_t timeValue = static_cast<uint64_t>(inputTime) / 1000;
			time_t rawTime(static_cast<time_t>(timeValue));
			std::tm resultTwo{ getCurrentTimeVal(rawTime) };
			jsonifier::string timeStamp{};
			timeStamp.resize(48);
			switch (timeFormat) {
				case TimeFormat::LongDate: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongDateTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%FT%T", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%T", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDate: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDateTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &resultTwo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortTime: {
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

		/// @brief Gets the time since the Unix epoch for the specified date and time.
		/// @param year Year.
		/// @param month Month.
		/// @param day Day.
		/// @param hour Hour.
		/// @param minute Minute.
		/// @param second Second.
		/// @return Time since Unix epoch in milliseconds.
		inline static uint64_t getTimeSinceEpoch(uint64_t year, uint64_t month, uint64_t day, uint64_t hour, uint64_t minute, uint64_t second) {
			static constexpr uint64_t secondsInJan{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInFeb{ 28 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInMar{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInApr{ 30 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInMay{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInJun{ 30 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInJul{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInAug{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInSep{ 30 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInOct{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInNov{ 30 * 24 * 60 * 60 };
			static constexpr uint64_t secondsInDec{ 31 * 24 * 60 * 60 };
			static constexpr uint64_t secondsPerMinute{ 60 };
			static constexpr uint64_t secondsPerHour{ 60 * 60 };
			static constexpr uint64_t secondsPerDay{ 60 * 60 * 24 };
			Seconds value{};
			for (uint64_t x = 1970; x < year; ++x) {
				value += Seconds{ secondsInJan };
				value += Seconds{ secondsInFeb };
				value += Seconds{ secondsInMar };
				value += Seconds{ secondsInApr };
				value += Seconds{ secondsInMay };
				value += Seconds{ secondsInJun };
				value += Seconds{ secondsInJul };
				value += Seconds{ secondsInAug };
				value += Seconds{ secondsInSep };
				value += Seconds{ secondsInOct };
				value += Seconds{ secondsInNov };
				value += Seconds{ secondsInDec };
				if (x % 4 == 0) {
					value += Seconds{ secondsPerDay };
				}
			}
			if (month > 0) {
				value += Seconds{ static_cast<uint64_t>((day - 1) * secondsPerDay) };
				value += Seconds{ static_cast<uint64_t>(hour * secondsPerHour) };
				value += Seconds{ static_cast<uint64_t>(minute * secondsPerMinute) };
				value += Seconds{ second };
			}
			if (month > 1) {
				value += Seconds{ secondsInJan };
			}
			if (month > 2) {
				value += Seconds{ secondsInFeb };
			}
			if (month > 3) {
				value += Seconds{ secondsInMar };
			}
			if (month > 4) {
				value += Seconds{ secondsInApr };
			}
			if (month > 5) {
				value += Seconds{ secondsInMay };
			}
			if (month > 6) {
				value += Seconds{ secondsInJun };
			}
			if (month > 7) {
				value += Seconds{ secondsInJul };
			}
			if (month > 8) {
				value += Seconds{ secondsInAug };
			}
			if (month > 9) {
				value += Seconds{ secondsInSep };
			}
			if (month > 10) {
				value += Seconds{ secondsInOct };
			}
			if (month > 11) {
				value += Seconds{ secondsInNov };
			}
			return static_cast<uint64_t>(std::chrono::duration_cast<Milliseconds>(value).count());
		}

	  protected:
		/// @brief Converts a string time stamp into a uint64_t time value.
		/// @param originalTimeStamp Original time stamp string.
		/// @return Converted time value in milliseconds.
		inline static uint64_t convertTimeStampToTimeUnits(jsonifier::string_view originalTimeStamp) {
			if (originalTimeStamp != "" && originalTimeStamp != "0") {
				auto newValue = getTimeSinceEpoch(std::stoull(originalTimeStamp.substr(0ull, 4ull).data()), std::stoull(originalTimeStamp.substr(5ull, 6ull).data()),
					std::stoull(originalTimeStamp.substr(8ull, 9ull).data()), std::stoull(originalTimeStamp.substr(11ull, 12ull).data()),
					std::stoull(originalTimeStamp.substr(14ull, 15ull).data()), std::stoull(originalTimeStamp.substr(17ull, 18ull).data()));
				return newValue;
			} else {
				return static_cast<uint64_t>(std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count());
			}
		}

		/// @brief Converts a string time stamp into a uint64_t time value.
		/// @param stringTimeStamp String time stamp to convert.
		/// @return Converted time value in milliseconds.
		inline uint64_t convertstring_toUintTimeStamp(jsonifier::string_view stringTimeStamp) const {
			return convertTimeStampToTimeUnits(stringTimeStamp);
		}

		/// @brief Converts a uint64_t time value into a string time stamp.
		/// @param uintTimeStamp Time value to convert.
		/// @return String time stamp.
		inline jsonifier::string convertUintTostring_timeStamp(uint64_t uintTimeStamp) const {
			return getISO8601TimeStamp(TimeFormat::LongDateTime, uintTimeStamp);
		}
	};

	/// @brief A class that extends TimeStampBase and jsonifier::string to provide additional functionality.
	class TimeStampParse : public TimeStampBase<TimeStampParse>, public jsonifier::string {
	  public:
		template<typename ValueType> friend class TimeStampBase;

		template<uint64_t size, typename value_type> bool operator==(value_type* lhs) {
			return jsonifier::string::operator==(lhs);
		}

		/// @brief Default constructor for TimeStampParse.
		inline TimeStampParse() = default;

		/// @brief Assignment operator to assign a string value to TimeStampParse.
		/// @param valueNew The new string value to assign.
		/// @return Reference to the modified TimeStampParse instance.
		inline TimeStampParse& operator=(jsonifier::string_view valueNew) {
			resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		/// @brief Constructor to create a TimeStampParse instance from a string value.
		/// @param valueNew The string value to create the instance from.
		inline TimeStampParse(jsonifier::string_view valueNew) {
			*this = valueNew;
		}

		/// @brief Move assignment operator to move a string value into TimeStampParse.
		/// @param valueNew The string value to move.
		/// @return Reference to the modified TimeStampParse instance.
		inline TimeStampParse& operator=(jsonifier::string&& valueNew) {
			resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		/// @brief Move constructor to create a TimeStampParse instance by moving a string value.
		/// @param valueNew The string value to move.
		inline TimeStampParse(jsonifier::string&& valueNew) {
			*this = std::move(valueNew);
		}

		/// @brief Assignment operator to assign a uint64_t value to TimeStampParse.
		/// @param valueNew The new uint64_t value to assign.
		/// @return Reference to the modified TimeStampParse instance.
		inline TimeStampParse& operator=(uint64_t valueNew) {
			*this = jsonifier::toString(valueNew);
			return *this;
		}

		/// @brief Constructor to create a TimeStampParse instance from a uint64_t value.
		/// @param valueNew The uint64_t value to create the instance from.
		inline TimeStampParse(uint64_t valueNew) {
			*this = valueNew;
		}

		/// @brief Returns a substring of the TimeStampParse instance.
		/// @param offset The starting position of the substring.
		/// @param count The length of the substring.
		/// @return A new TimeStampParse instance representing the substring.
		inline TimeStampParse substr(uint64_t offset, uint64_t count) const {
			return substr(offset, count);
		}

		/// @brief Returns the size of the TimeStampParse instance.
		/// @return The size of the TimeStampParse instance.
		inline uint64_t size() const {
			return jsonifier::string::size();
		}

		/// @brief Returns a pointer to the character data of the TimeStampParse instance.
		/// @return Pointer to the character data.
		inline char* data() const {
			return jsonifier::string::data();
		}

		/// @brief Conversion operator to convert TimeStampParse to uint64_t.
		/// @return The uint64_t value represented by the TimeStampParse instance.
		inline operator uint64_t() const {
			if (empty()) {
				return 0;
			} else {
				return convertstring_toUintTimeStamp(*this);
			}
		}
	};

	/// @brief A class that extends TimeStampBase to provide additional functionality.
	class TimeStamp : public TimeStampBase<TimeStamp> {
	  public:
		template<typename ValueType> friend class TimeStampBase;

		/// @brief Default constructor for TimeStamp.
		inline TimeStamp() = default;

		/// @brief Assignment operator to assign a TimeStampParse value to TimeStamp.
		/// @param other The TimeStampParse instance to assign.
		/// @return Reference to the modified TimeStamp instance.
		inline TimeStamp& operator=(const TimeStampParse& other) {
			value = other.operator uint64_t();
			return *this;
		}

		/// @brief Constructor to create a TimeStamp instance from a TimeStampParse value.
		/// @param other The TimeStampParse instance to create the instance from.
		inline TimeStamp(const TimeStampParse& other) {
			*this = other;
		}

		/// @brief Assignment operator to assign a string value to TimeStamp.
		/// @param valueNew The new string value to assign.
		/// @return Reference to the modified TimeStamp instance.
		inline TimeStamp& operator=(jsonifier::string_view valueNew) {
			value = std::stoull(valueNew.data());
			return *this;
		}

		/// @brief Constructor to create a TimeStamp instance from a string value.
		/// @param valueNew The string value to create the instance from.
		inline TimeStamp(jsonifier::string_view valueNew) {
			*this = valueNew;
		}

		/// @brief Move assignment operator to move a string value into TimeStamp.
		/// @param valueNew The string value to move.
		/// @return Reference to the modified TimeStamp instance.
		inline TimeStamp& operator=(jsonifier::string&& valueNew) {
			value = std::stoull(valueNew.data());
			return *this;
		}

		/// @brief Move constructor to create a TimeStamp instance by moving a string value.
		/// @param valueNew The string value to move.
		inline TimeStamp(jsonifier::string&& valueNew) {
			*this = std::move(valueNew);
		}

		/// @brief Assignment operator to assign a uint64_t value to TimeStamp.
		/// @param valueNew The new uint64_t value to assign.
		/// @return Reference to the modified TimeStamp instance.
		inline TimeStamp& operator=(uint64_t valueNew) {
			value = valueNew;
			return *this;
		}

		/// @brief Constructor to create a TimeStamp instance from a uint64_t value.
		/// @param valueNew The uint64_t value to create the instance from.
		inline TimeStamp(uint64_t valueNew) {
			*this = valueNew;
		}

		/// @brief Conversion operator to convert TimeStamp to int64_t.
		/// @return The int64_t value represented by the TimeStamp instance.
		inline operator uint64_t() const {
			return value;
		}

		/// @brief Conversion operator to convert TimeStamp to jsonifier::string.
		/// @return The jsonifier::string value represented by the TimeStamp instance.
		inline operator jsonifier::string() const {
			return convertUintTostring_timeStamp(value);
		}

	  protected:
		uint64_t value{};///< The value stored in the TimeStamp instance.
	};

	class Snowflake;

	template<typename ValueType = void> class ToEntity {
		inline ValueType toEntity(Snowflake initialId, Snowflake additionalId);

		inline ValueType toEntity(Snowflake initialId);
	};

	/// @brief A class representing a Snowflake identifier with various operations.
	class Snowflake {
	  public:
		/// @brief Default constructor for Snowflake.
		inline Snowflake() = default;

		template<typename ValueType> inline ValueType toEntity() {
			return ToEntity<ValueType>{}.toEntity(*this);
		}

		template<typename ValueType> inline ValueType toEntity(Snowflake additionalId) {
			return ToEntity<ValueType>{}.toEntity(*this, additionalId);
		}

		/// @brief Assignment operator to assign a string value to Snowflake.
		/// @param other The string value to assign.
		/// @return Reference to the modified Snowflake instance.
		inline Snowflake& operator=(jsonifier::string_view other) {
			if (other.size() > 0) {
				for (auto& value: other) {
					if (!std::isdigit(static_cast<uint8_t>(value))) {
						return *this;
					}
				}
				id = std::stoull(other.data());
			}
			return *this;
		}

		/// @brief Constructor to create a Snowflake instance from a string value.
		/// @param other The string value to create the instance from.
		inline Snowflake(jsonifier::string_view other) {
			*this = other;
		}

		/// @brief Assignment operator to assign a uint64_t value to Snowflake.
		/// @param other The uint64_t value to assign.
		/// @return Reference to the modified Snowflake instance.
		inline Snowflake& operator=(uint64_t other) {
			id = other;
			return *this;
		}

		/// @brief Constructor to create a Snowflake instance from a uint64_t value.
		/// @param other The uint64_t value to create the instance from.
		inline Snowflake(uint64_t other) {
			*this = other;
		}

		/// @brief Conversion operator to convert Snowflake to jsonifier::string.
		/// @return The jsonifier::string value represented by the Snowflake instance.
		inline explicit operator jsonifier::string() const {
			return jsonifier::toString(id);
		}

		/// @brief Explicit conversion operator to convert Snowflake to uint64_t.
		/// @return The uint64_t value represented by the Snowflake instance.
		inline explicit operator const uint64_t&() const {
			return id;
		}

		inline bool operator==(const Snowflake& other) const {
			return this->id == other.id;
		}

		inline bool operator==(jsonifier::string_view other) const {
			return this->operator jsonifier::string() == other;
		}

		inline bool operator==(uint64_t other) const {
			return this->id == other;
		}

		/// @brief Concatenation operator to concatenate Snowflake and a string value.
		/// @tparam ValueType The type of the string value.
		/// @param rhs The string value to concatenate.
		/// @return The concatenated string.
		template<jsonifier_internal::string_t ValueType> inline jsonifier::string operator+(jsonifier::string_view rhs) const {
			jsonifier::string newString{ operator jsonifier::string() };
			newString += rhs;
			return newString;
		}

		/// @brief Friend function to concatenate Snowflake and a string value.
		/// @param lhs The Snowflake instance.
		/// @param other The string value to concatenate.
		/// @return The concatenated string.
		inline friend jsonifier::string operator+(const Snowflake& lhs, jsonifier::string_view other) {
			jsonifier::string lhsNew{ static_cast<jsonifier::string>(lhs) };
			lhsNew += other;
			return lhsNew;
		}

		/// @brief Friend function to concatenate two values.
		/// @tparam ValueType01 The type of the first value.
		/// @tparam ValueType02 The type of the second value.
		/// @param lhs The first value.
		/// @param rhs The second value.
		/// @return The concatenated string.
		template<jsonifier_internal::string_t ValueType01, typename ValueType02> friend inline jsonifier::string operator+(const ValueType01& lhs, const ValueType02& rhs) {
			jsonifier::string newString{ lhs };
			newString += rhs.operator jsonifier::string();
			return newString;
		}

		/// @brief Friend function to concatenate two values.
		/// @tparam ValueType01 The type of the first value.
		/// @tparam ValueType02 The type of the second value.
		/// @param lhs The first value.
		/// @param rhs The second value.
		/// @return The concatenated string.
		template<uint64_t size> friend inline jsonifier::string operator+(const char (&lhs)[size], const Snowflake& rhs) {
			jsonifier::string newString{ lhs };
			newString += rhs.operator jsonifier::string();
			return newString;
		}

		/// @brief Converts the Snowflake ID into a time and date stamp.
		/// @return A jsonifier::string containing the timestamp.
		inline jsonifier::string getCreatedAtTimeStamp() {
			uint64_t timeStamp{ static_cast<uint64_t>((id >> 22) + 1420070400000) };
			return TimeStamp::getISO8601TimeStamp(TimeFormat::LongDateTime, timeStamp);
		}

	  protected:
		uint64_t id{};///< The Snowflake ID.
	};

	inline std::ostream& operator<<(std::ostream& os, Snowflake sf) {
		os << sf.operator jsonifier::string();
		return os;
	}

	/// @brief An exception class derived from std::runtime_error for DCA-related exceptions.
	struct DCAException : public std::runtime_error {
		/// @brief Constructor to create a DCAException with an error message and optional source location.
		/// @param error The error message.
		/// @param location The source location of the exception (default: current location).
		inline DCAException(jsonifier::string_view error, std::source_location location = std::source_location::current())
			: std::runtime_error(std::string{ "Thrown From: " + jsonifier::string{ location.file_name() } + jsonifier::string{ " (" } + jsonifier::toString(location.line()) + ":" +
				  jsonifier::toString(location.column()) + ")\n" + error }){};
	};

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// @brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class StreamType { None = 0, Client = 1, Server = 2 };

	/**@}*/
}
