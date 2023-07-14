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

#ifndef __GNUC__
	#pragma warning(push)
	#pragma warning(disable : 4251)
#endif

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
	#include <chrono>
inline std::tm getCurrentTimeVal(time_t& result) {
	std::tm resultTwo{};
	localtime_s(&resultTwo, &result);
	return resultTwo;
}
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
#include <map>
#include <set>

#ifdef __linux__
inline std::tm getCurrentTimeVal(time_t& result) {
	std::tm resultTwo{ *localtime(&result) };
	return resultTwo;
}
#endif

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

using namespace std::literals;

namespace DiscordCoreAPI {

	inline thread_local Jsonifier::JsonifierCore jsonifierCore{};

	using SysClock = std::chrono::system_clock;
	using HRClock = std::chrono::high_resolution_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	using Seconds = std::chrono::seconds;

	template<typename ValueType> class AllocWrapper {
	  public:
		using value_type = ValueType;
		using pointer = value_type*;
		using allocator = std::pmr::polymorphic_allocator<value_type>;
		using allocator_traits = std::allocator_traits<allocator>;

		inline constexpr AllocWrapper() = default;

		inline constexpr AllocWrapper& operator=(AllocWrapper&& other) {
			return *this;
		};

		inline constexpr AllocWrapper(AllocWrapper&& other){};

		inline constexpr AllocWrapper& operator=(const AllocWrapper& other) {
			return *this;
		};

		inline constexpr AllocWrapper(const AllocWrapper& other){};

		inline constexpr pointer allocate(uint64_t count) {
			return allocTraits.allocate(alloc, count);
		}

		inline constexpr void deallocate(pointer ptr, uint64_t count) {
			allocTraits.deallocate(alloc, ptr, count);
		}

		template<typename... Args> inline constexpr void construct(pointer ptr, Args... args) {
			allocTraits.construct(alloc, ptr, args...);
		}

		inline constexpr void destroy(pointer ptr) {
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

	class DiscordCoreAPI_Dll MessagePrinter {
	  public:
		inline MessagePrinter() = default;

		template<typename ValueType>
		inline static void initialize(const ValueType& other, std::ostream& outputStreamNew = std::cout, std::ostream& errorStreamNew = std::cerr) {
			doWePrintGeneralErrors.store(other.doWePrintGeneralErrorMessages());
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
									 << reset() << std::endl
									 << std::endl;
					}
					break;
				}
				case PrintMessageType::Https: {
					if (doWePrintHttpsErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "Https Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl
									 << std::endl;
					}
					break;
				}
				case PrintMessageType::WebSocket: {
					if (doWePrintWebSocketErrors.load()) {
						std::unique_lock lock{ accessMutex };
						*errorStream << shiftToBrightRed() << "WebSocket Error, caught at: " << where.file_name() << ", " << where.line() << ":"
									 << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									 << reset() << std::endl
									 << std::endl;
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
									  << reset() << std::endl
									  << std::endl;
					}
					break;
				}
				case PrintMessageType::Https: {
					if (doWePrintHttpsSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "Https Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl
									  << std::endl;
					}
					break;
				}
				case PrintMessageType::WebSocket: {
					if (doWePrintWebSocketSuccesses.load()) {
						std::unique_lock lock{ accessMutex };
						*outputStream << shiftToBrightGreen() << "WebSocket Success, caught at: " << where.file_name() << ", " << where.line() << ":"
									  << where.column() << ", in: " << where.function_name() << ", it is: " << what << std::endl
									  << reset() << std::endl
									  << std::endl;
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
		inline StopWatch() = default;

		inline StopWatch& operator=(StopWatch<TimeType>&& data) {
			maxNumberOfTimeUnits.store(data.maxNumberOfTimeUnits.load());
			startTime.store(data.startTime.load());
			return *this;
		}

		inline StopWatch(StopWatch<TimeType>&& other) {
			*this = std::move(other);
		}

		inline StopWatch<TimeType>& operator=(const StopWatch<TimeType>& data) = delete;
		inline StopWatch(const StopWatch<TimeType>& other) = delete;

		inline StopWatch(TimeType maxNumberOfMsNew) {
			maxNumberOfTimeUnits.store(maxNumberOfMsNew);
			resetTimer();
		}

		inline StopWatch(int64_t maxNumberOfMsNew) {
			maxNumberOfTimeUnits.store(TimeType{ maxNumberOfMsNew });
			resetTimer();
		}

		inline TimeType totalTimePassed() {
			return std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()) - startTime.load();
		}

		inline TimeType getTotalWaitTime() {
			return maxNumberOfTimeUnits.load();
		}

		inline bool hasTimePassed() {
			if (std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()) - startTime.load() >= maxNumberOfTimeUnits.load()) {
				return true;
			} else {
				return false;
			}
		}

		inline void resetTimer() {
			startTime.store(std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::atomic<TimeType> maxNumberOfTimeUnits{ TimeType{ 0 } };
		std::atomic<TimeType> startTime{ TimeType{ 0 } };
	};

	template<class ValueType> class TimeStampBase {
	  public:
		inline bool operator==(const std::string& other) const {
			return *static_cast<const ValueType*>(this) == other;
		}

		inline std::string convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd,
			int32_t yearsToAdd, TimeFormat timeFormat) {
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
			std::tm resultTwo{ getCurrentTimeVal(result) };
			std::string returnString{};
			if (resultTwo.tm_isdst) {
				if (resultTwo.tm_hour + 4 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				TimeStampBase timeStamp{};
				timeStamp.getTimeSinceEpoch(static_cast<int64_t>(resultTwo.tm_year) + 1900, static_cast<int64_t>(resultTwo.tm_mon) + 1,
					resultTwo.tm_mday, static_cast<int64_t>(resultTwo.tm_hour) + 4, resultTwo.tm_min, resultTwo.tm_sec);
				timeStamp.getISO8601TimeStamp(timeFormat);
				returnString = static_cast<std::string>(*static_cast<ValueType*>(&timeStamp));
			} else {
				if (resultTwo.tm_hour + 5 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				TimeStampBase timeStamp{};
				timeStamp.getTimeSinceEpoch(resultTwo.tm_year + 1900, resultTwo.tm_mon + 1, resultTwo.tm_mday, resultTwo.tm_hour + 5,
					resultTwo.tm_min, resultTwo.tm_sec);
				timeStamp.getISO8601TimeStamp(timeFormat);
				returnString = static_cast<std::string>(*static_cast<ValueType*>(&timeStamp));
			}
			return returnString;
		}

		inline std::string convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
			std::time_t result = std::time(nullptr);
			std::tm resultTwo{ getCurrentTimeVal(result) };
			std::string returnString{};
			if (resultTwo.tm_isdst) {
				if (resultTwo.tm_hour + 4 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				TimeStampBase timeStamp{};
				timeStamp.getTimeSinceEpoch(static_cast<int64_t>(resultTwo.tm_year) + 1900, static_cast<int64_t>(resultTwo.tm_mon) + 1,
					resultTwo.tm_mday, static_cast<int64_t>(resultTwo.tm_hour) + 4, resultTwo.tm_min, resultTwo.tm_sec);
				timeStamp.getISO8601TimeStamp(timeFormat);
				returnString = static_cast<std::string>(*static_cast<ValueType*>(&timeStamp));
			} else {
				if (resultTwo.tm_hour + 5 >= 24) {
					resultTwo.tm_hour = resultTwo.tm_hour - 24;
					++resultTwo.tm_mday;
				}
				TimeStampBase timeStamp{};
				timeStamp.getTimeSinceEpoch(resultTwo.tm_year + 1900, resultTwo.tm_mon + 1, resultTwo.tm_mday, resultTwo.tm_hour + 5,
					resultTwo.tm_min, resultTwo.tm_sec);
				timeStamp.getISO8601TimeStamp(timeFormat);
				returnString = static_cast<std::string>(*static_cast<ValueType*>(&timeStamp));
			}
			return returnString;
		}

		bool hasTimeElapsed(uint64_t days, uint64_t hours, uint64_t minutes) {
			if (static_cast<int64_t>(*static_cast<ValueType*>(this)) <= 0) {
				*static_cast<ValueType*>(this) = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
			int64_t startTimeRaw = *static_cast<ValueType*>(this);
			auto currentTime = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
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
			*static_cast<ValueType*>(this) = std::chrono::duration_cast<Milliseconds>(value).count();
		}

		inline void convertTimeStampToTimeUnits(std::string originalTimeStamp) {
			if (originalTimeStamp != "" && originalTimeStamp != "0") {
				TimeStampBase timeValue{};
				timeValue.getTimeSinceEpoch(stoi(originalTimeStamp.substr(0, 4)), stoi(originalTimeStamp.substr(5, 6)),
					stoi(originalTimeStamp.substr(8, 9)), stoi(originalTimeStamp.substr(11, 12)), stoi(originalTimeStamp.substr(14, 15)),
					stoi(originalTimeStamp.substr(17, 18)));
				*static_cast<ValueType*>(this) = static_cast<uint64_t>(timeValue);
			} else {
				*static_cast<ValueType*>(this) = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
		}

		inline std::string getISO8601TimeStamp(TimeFormat timeFormat) {
			if (static_cast<int64_t>(*static_cast<ValueType*>(this)) <= 0) {
				*static_cast<ValueType*>(this) = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
			uint64_t timeValue = static_cast<int64_t>(*static_cast<ValueType*>(this)) / 1000;
			time_t rawTime(timeValue);
			std::tm resultTwo{ getCurrentTimeVal(rawTime) };
			std::string timeStamp{};
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

	  protected:
		inline ~TimeStampBase() = default;
		inline TimeStampBase() = default;
	};

	class TimeStampParse : public TimeStampBase<TimeStampParse>, public std::string {
	  public:
		template<typename ValueType> friend class TimeStampBase;

		inline TimeStampParse() = default;

		inline TimeStampParse& operator=(const std::string& valueNew) {
			this->resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		inline TimeStampParse(const std::string& valueNew) {
			*this = valueNew;
		}

		inline TimeStampParse& operator=(std::string&& valueNew) {
			this->resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		inline TimeStampParse(std::string&& valueNew) {
			*this = std::move(valueNew);
		}

		inline TimeStampParse& operator=(int64_t valueNew) {
			*this = std::to_string(valueNew);
			return *this;
		}

		inline TimeStampParse(int64_t valueNew) {
			*this = valueNew;
		}

		inline TimeStampParse substr(size_t offset, size_t count) {
			return this->substr(offset, count);
		}

		inline size_t size() {
			return std::string::size();
		}

		inline char* data() {
			return std::string::data();
		}

		inline operator int64_t() const {
			return std::stoll(*this);
		}
	};

	class TimeStamp : public TimeStampBase<TimeStamp> {
	  public:
		template<typename ValueType> friend class TimeStampBase;

		inline TimeStamp() = default;

		inline TimeStamp& operator=(const TimeStampParse& other) {
			value = other.operator int64_t();
			return *this;
		}

		inline TimeStamp(const TimeStampParse& other) {
			*this = other;
		}

		inline TimeStamp& operator=(const std::string& valueNew) {
			value = stoull(valueNew);
			return *this;
		}

		inline TimeStamp(const std::string& valueNew) {
			*this = valueNew;
		}

		inline TimeStamp& operator=(std::string&& valueNew) {
			value = stoull(valueNew);
			return *this;
		}

		inline TimeStamp(std::string&& valueNew) {
			*this = std::move(valueNew);
		}

		inline TimeStamp& operator=(int64_t valueNew) {
			value = valueNew;
			return *this;
		}

		inline TimeStamp(int64_t valueNew) {
			*this = valueNew;
		}

		inline operator int64_t() const {
			return value;
		}

		inline operator std::string() const {
			return std::to_string(value);
		}

	  protected:
		int64_t value{};
	};

	template<typename ValueType>
	concept StringT = std::convertible_to<ValueType, std::string>;

	class DiscordCoreAPI_Dll Snowflake {
	  public:
		inline Snowflake() = default;

		inline Snowflake& operator=(const std::string& other) {
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

		inline Snowflake(const std::string& other) {
			*this = other;
		}

		inline Snowflake& operator=(uint64_t other) {
			id = other;
			return *this;
		}

		inline Snowflake(uint64_t other) {
			*this = other;
		}

		inline operator std::string() const {
			return std::to_string(id);
		}

		inline explicit operator uint64_t() const {
			return id;
		}

		inline bool operator==(const Snowflake& rhs) const {
			return id == rhs.id;
		}

		template<StringT ValueType> inline std::string operator+(const std::string& rhs) const {
			std::string newString{ this->operator std::string() };
			newString += rhs;
			return newString;
		}

		inline friend std::string operator+(const Snowflake& lhs, const std::string& other) {
			std::string lhsNew{ lhs };
			lhsNew += other;
			return lhsNew;
		}

		template<StringT ValueType01, typename ValueType02> friend inline std::string operator+(const ValueType01& lhs, const ValueType02& rhs) {
			std::string newString{ lhs };
			newString += rhs;
			return newString;
		}

	  protected:
		uint64_t id{};

		/// \brief Converts the snowflake-id into a time and date stamp.
		/// \returns std::string A std::string containing the timeStamp.
		inline std::string getCreatedAtTimeStamp() {
			TimeStamp timeStamp{ static_cast<int64_t>((id >> 22) + 1420070400000) };
			return timeStamp.getISO8601TimeStamp(TimeFormat::LongDateTime);
		}
	};

	inline std::ostream& operator<<(std::ostream& os, Snowflake sf) {
		os << sf.operator std::string();
		return os;
	}

	class SemaphoreLock {
	  public:
		inline SemaphoreLock(std::counting_semaphore<1>& semaphoreNew) : semaphore{ semaphoreNew } {
			semaphore.acquire();
		};
		inline ~SemaphoreLock() {
			semaphore.release();
		}

	  protected:
		std::counting_semaphore<1>& semaphore;
	};

	/// \brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class StreamType { None = 0, Client = 1, Server = 2 };
}
