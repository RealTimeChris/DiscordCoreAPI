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

#include <jsonifier/Index.hpp>

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

using namespace std::literals;

namespace DiscordCoreAPI {

	inline thread_local Jsonifier::JsonifierCore parser{};

	using HRClock = std::chrono::high_resolution_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	using Seconds = std::chrono::seconds;

	template<typename Derived> struct Relational {
		inline friend bool operator>(const Derived& data1, const Derived& data2) {
			return data2 < data1;
		}
		inline friend bool operator==(const Derived& data1, const Derived& data2) {
			return !(data1 < data2) && !(data2 < data1);
		}
		inline friend bool operator!=(const Derived& data1, const Derived& data2) {
			return !(data1 == data2);
		}
		inline friend bool operator<=(const Derived& data1, const Derived& data2) {
			return (data1 < data2) || (data1 == data2);
		}
		inline friend bool operator>=(const Derived& data1, const Derived& data2) {
			return (data1 > data2) || (data1 == data2);
		}

	  protected:
		inline Relational() noexcept = default;
	};

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

	enum class PrintMessageType {
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

		template<PrintMessageType messageType>
		inline static void printError(const std::string& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case PrintMessageType::General: {
					if (doWePrintGeneralErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "General Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::Https: {
					if (doWePrintHttpsErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "Https Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::WebSocket: {
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

		template<PrintMessageType messageType>
		inline static void printSuccess(const std::string& what, std::source_location where = std::source_location::current()) {
			switch (messageType) {
				case PrintMessageType::General: {
					if (doWePrintGeneralSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightBlue() << "General Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::Https: {
					if (doWePrintHttpsSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "Https Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl;
					}
					break;
				}
				case PrintMessageType::WebSocket: {
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

	template<typename ValueType>
	concept CopyableOrMovable = std::copyable<std::decay_t<ValueType>> || std::movable<std::decay_t<ValueType>>;

	/// \brief Time formatting methods.
	enum class TimeFormat : char {
		LongDate = 'D',///< "20 April 2021" - Long Date
		LongDateTime = 'F',///< "Tuesday, 20 April 2021 16:20" - Long Date/Time
		LongTime = 'T',///< "16:20:30" - Long Time
		ShortDate = 'd',///< "20/04/2021" - Short Date
		ShortDateTime = 'f',///< "20 April 2021 16:20" - Short Date/Time
		ShortTime = 't',///< "16:20" - Short Time
	};

	template<typename TimeType> class StopWatch {
	  public:
		inline StopWatch() noexcept = default;

		inline StopWatch& operator=(StopWatch<TimeType>&& data) noexcept {
			maxNumberOfTimeUnits.store(data.maxNumberOfTimeUnits.load());
			startTime.store(data.startTime.load());
			return *this;
		}

		inline StopWatch(StopWatch<TimeType>&& other) noexcept {
			*this = std::move(other);
		}

		inline StopWatch<TimeType>& operator=(const StopWatch<TimeType>& data) noexcept = delete;
		inline StopWatch(const StopWatch<TimeType>& other) noexcept = delete;

		inline StopWatch(TimeType maxNumberOfMsNew) noexcept {
			maxNumberOfTimeUnits.store(maxNumberOfMsNew);
			resetTimer();
		}

		inline StopWatch(int64_t maxNumberOfMsNew) noexcept {
			maxNumberOfTimeUnits.store(TimeType{ maxNumberOfMsNew });
			resetTimer();
		}

		inline TimeType totalTimePassed() noexcept {
			return std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()) - startTime.load();
		}

		inline TimeType getTotalWaitTime() noexcept {
			return maxNumberOfTimeUnits.load();
		}

		inline bool hasTimePassed() noexcept {
			if (std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()) - startTime.load() >= maxNumberOfTimeUnits.load()) {
				return true;
			} else {
				return false;
			}
		}

		inline void resetTimer() noexcept {
			startTime.store(std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::atomic<TimeType> maxNumberOfTimeUnits{ TimeType{ 0 } };
		std::atomic<TimeType> startTime{ TimeType{ 0 } };
	};

	/// \brief Class for representing a timeStamp, as well as working with time-related values.
	class DiscordCoreAPI_Dll TimeStamp {
	  public:
		inline TimeStamp() noexcept = default;

		inline TimeStamp& operator=(const TimeStamp& other) {
			timeStampInTimeUnits = other.timeStampInTimeUnits;
			return *this;
		}

		inline TimeStamp(const TimeStamp& other) {
			*this = other;
		}

		inline TimeStamp(TimeFormat formatNew) {
			timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
		}

		inline TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second,
			TimeFormat formatNew) {
			getTimeSinceEpoch(stoull(year), stoull(month), stoull(day), stoull(hour), stoull(minute), stoull(second));
		}

		inline operator std::string() {
			return getISO8601TimeStamp(TimeFormat::LongDateTime);
		}

		inline operator uint64_t() {
			if (timeStampInTimeUnits == 0) {
				timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
			}
			return timeStampInTimeUnits;
		}

		inline TimeStamp& operator=(std::string&& originalTimeStampNew) {
			convertTimeStampToTimeUnits(TimeFormat::LongDateTime, originalTimeStampNew);
			return *this;
		}

		inline TimeStamp(std::string&& originalTimeStampNew) {
			*this = std::move(originalTimeStampNew);
			if (timeStampInTimeUnits == 0) {
				timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
			}
		}

		inline TimeStamp& operator=(std::string& originalTimeStampNew) {
			convertTimeStampToTimeUnits(TimeFormat::LongDateTime, originalTimeStampNew);
			return *this;
		}

		inline TimeStamp(std::string& originalTimeStampNew) {
			*this = originalTimeStampNew;
		}

		inline TimeStamp(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, TimeFormat formatNew) {
			getTimeSinceEpoch(year, month, day, hour, minute, second);
		};

		inline TimeStamp(uint64_t timeInTimeUnits, TimeFormat formatNew) {
			timeStampInTimeUnits = timeInTimeUnits;
		}

		inline std::string convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd,
			int32_t yearsToAdd, TimeFormat formatNew) {
			std::time_t result = std::time(nullptr);
			int32_t secondsPerMinute{ 60 };
			int32_t minutesPerHour{ 60 };
			int32_t secondsPerHour{ minutesPerHour * secondsPerMinute };
			int32_t hoursPerDay{ 24 };
			int32_t secondsPerDay{ secondsPerHour * hoursPerDay };
			int32_t daysPerMonth{ 30 };
			int32_t secondsPerMonth{ secondsPerDay * daysPerMonth };
			int32_t daysPerYear{ 365 };
			int32_t secondsPerYear{ secondsPerDay * daysPerYear };
			int32_t secondsToAdd = (yearsToAdd * secondsPerYear) + (monthsToAdd * secondsPerMonth) + (daysToAdd * secondsPerDay) +
				((hoursToAdd + 8) * secondsPerHour) + (minutesToAdd * secondsPerMinute);
			result += secondsToAdd;
			auto resultTwo = std::localtime(&result);
			std::string returnString{};
			if (resultTwo->tm_isdst) {
				if (resultTwo->tm_hour + 4 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					++resultTwo->tm_mday;
				}
				TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1),
					std::to_string(resultTwo->tm_mday), std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min),
					std::to_string(resultTwo->tm_sec), formatNew };
				timeStamp.getISO8601TimeStamp(formatNew);
				returnString = static_cast<std::string>(timeStamp);
			} else {
				if (resultTwo->tm_hour + 5 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					++resultTwo->tm_mday;
				}
				TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1),
					std::to_string(resultTwo->tm_mday), std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min),
					std::to_string(resultTwo->tm_sec), formatNew };
				timeStamp.getISO8601TimeStamp(formatNew);
				returnString = static_cast<std::string>(timeStamp);
			}
			return returnString;
		}

		inline std::string convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
			std::time_t result = std::time(nullptr);
			auto resultTwo = std::localtime(&result);
			std::string returnString{};
			if (resultTwo->tm_isdst) {
				if (resultTwo->tm_hour + 4 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					++resultTwo->tm_mday;
				}
				TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1),
					std::to_string(resultTwo->tm_mday), std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min),
					std::to_string(resultTwo->tm_sec), timeFormat };
				returnString = timeStamp.getISO8601TimeStamp(timeFormat);
			} else {
				if (resultTwo->tm_hour + 5 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					++resultTwo->tm_mday;
				}
				TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1),
					std::to_string(resultTwo->tm_mday), std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min),
					std::to_string(resultTwo->tm_sec), timeFormat };
				returnString = timeStamp.getISO8601TimeStamp(timeFormat);
			}
			return returnString;
		}

		bool hasTimeElapsed(uint64_t days, uint64_t hours, uint64_t minutes) {
			if (timeStampInTimeUnits <= 0) {
				timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
			}
			int64_t startTimeRaw = timeStampInTimeUnits;
			auto currentTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
			int64_t secondsPerMinute = 60;
			int64_t secondsPerHour = secondsPerMinute * 60;
			int64_t secondsPerDay = secondsPerHour * 24;
			auto targetElapsedTime = ((static_cast<int64_t>(days) * secondsPerDay) + (static_cast<int64_t>(hours) * secondsPerHour) +
										 (static_cast<int64_t>(minutes) * secondsPerMinute)) *
				1000;
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

		inline static std::string convertMsToDurationString(uint64_t durationInMs) {
			std::string newString{};
			uint64_t msPerSecond{ 1000 };
			uint64_t secondsPerMinute{ 60 };
			uint64_t minutesPerHour{ 60 };
			uint64_t msPerMinute{ msPerSecond * secondsPerMinute };
			uint64_t msPerHour{ msPerMinute * minutesPerHour };
			uint64_t hoursLeft = static_cast<uint64_t>(trunc(durationInMs / msPerHour));
			uint64_t minutesLeft = static_cast<uint64_t>(trunc((durationInMs % msPerHour) / msPerMinute));
			uint64_t secondsLeft = static_cast<uint64_t>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
			if (hoursLeft >= 1) {
				newString += std::to_string(hoursLeft) + " Hours, ";
				newString += std::to_string(minutesLeft) + " Minutes, ";
				newString += std::to_string(secondsLeft) + " Seconds.";
			} else if (minutesLeft >= 1) {
				newString += std::to_string(minutesLeft) + " Minutes, ";
				newString += std::to_string(secondsLeft) + " Seconds.";
			} else {
				newString += std::to_string(secondsLeft) + " Seconds.";
			}
			return newString;
		}

	  protected:
		uint64_t timeStampInTimeUnits{};

		inline void getTimeSinceEpoch(int64_t year, int64_t month, int64_t day, int64_t hour, int64_t minute, int64_t second) {
			const uint32_t secondsInJan{ 31 * 24 * 60 * 60 };
			const uint32_t secondsInFeb{ 28 * 24 * 60 * 60 };
			const uint32_t secondsInMar{ 31 * 24 * 60 * 60 };
			const uint32_t secondsInApr{ 30 * 24 * 60 * 60 };
			const uint32_t secondsInMay{ 31 * 24 * 60 * 60 };
			const uint32_t secondsInJun{ 30 * 24 * 60 * 60 };
			const uint32_t secondsInJul{ 31 * 24 * 60 * 60 };
			const uint32_t secondsInAug{ 31 * 24 * 60 * 60 };
			const uint32_t secondsInSep{ 30 * 24 * 60 * 60 };
			const uint32_t secondsInOct{ 31 * 24 * 60 * 60 };
			const uint32_t secondsInNov{ 30 * 24 * 60 * 60 };
			const uint32_t secondsInDec{ 31 * 24 * 60 * 60 };
			const uint32_t secondsPerMinute{ 60 };
			const uint32_t secondsPerHour{ 60 * 60 };
			const uint32_t secondsPerDay{ 60 * 60 * 24 };
			Seconds value{};
			for (int32_t x = 1970; x < year; ++x) {
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
			timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(value).count();
		}

		inline void convertTimeStampToTimeUnits(TimeFormat formatNew, std::string originalTimeStamp) {
			if (originalTimeStamp != "" && originalTimeStamp != "0") {
				TimeStamp timeValue = TimeStamp{ stoi(originalTimeStamp.substr(0, 4)), stoi(originalTimeStamp.substr(5, 6)),
					stoi(originalTimeStamp.substr(8, 9)), stoi(originalTimeStamp.substr(11, 12)), stoi(originalTimeStamp.substr(14, 15)),
					stoi(originalTimeStamp.substr(17, 18)), formatNew };
				timeStampInTimeUnits = static_cast<uint64_t>(timeValue);
			} else {
				timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
			}
		}

		inline std::string getISO8601TimeStamp(TimeFormat timeFormat) {
			if (timeStampInTimeUnits <= 0) {
				timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count();
			}
			uint64_t timeValue = timeStampInTimeUnits / 1000;
			time_t rawTime(timeValue);
			tm timeInfo = *localtime(&rawTime);
			std::string timeStamp{};
			timeStamp.resize(48);
			switch (timeFormat) {
				case TimeFormat::LongDate: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongDateTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%FT%T", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%T", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDate: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDateTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortTime: {
					uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%R", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				default: {
					break;
				}
			}
			return timeStamp;
		}
	};

	class DiscordCoreAPI_Dll Snowflake : public Relational<Snowflake> {
	  public:
		inline Snowflake() noexcept = default;

		inline Snowflake& operator=(const std::string& other) noexcept {
			for (auto& value: other) {
				if (!std::isdigit(static_cast<uint8_t>(value))) {
					return *this;
				}
			}
			if (other.size() == 0) {
				return *this;
			}
			id = stoull(other);
			return *this;
		}

		inline Snowflake(const std::string& other) noexcept {
			*this = other;
		}

		inline Snowflake& operator=(uint64_t other) noexcept {
			id = other;
			return *this;
		}

		inline Snowflake(uint64_t other) noexcept {
			*this = other;
		}

		inline operator std::string() const noexcept {
			return std::to_string(id);
		}

		inline explicit operator uint64_t() const noexcept {
			return id;
		}

		/// \brief Converts the
		/// snowflake-id into a time and date stamp. \returns std::string A
		/// std::string containing the timeStamp.
		std::string getCreatedAtTimeStamp(TimeFormat timeFormat) {
			TimeStamp timeStamp{ (id >> 22) + 1420070400000, timeFormat };
			return timeStamp.operator std::string();
		}

		inline friend bool operator<(const Snowflake& lhs, const Snowflake& rhs) noexcept {
			return lhs.id < rhs.id;
		}

		friend inline std::string operator+(const std::string&, const Snowflake&) noexcept;

	  protected:
		uint64_t id{};
	};

	inline std::ostream& operator<<(std::ostream& os, Snowflake sf) {
		os << sf.operator std::string();
		return os;
	}

	inline std::string operator+(const std::string& lhs, const Snowflake& rhs) noexcept {
		std::string string{};
		string += lhs;
		string += std::to_string(rhs.id);
		return string;
	}
}
