/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
/// Utilities.cpp - Header file for the Utilities.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file Utilities.hpp

#pragma once

#pragma warning(push)
#pragma warning(disable : 4275)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#pragma warning(disable : 4996)

#ifdef _WIN32
	#ifdef DiscordCoreAPI_EXPORTS
		#define DiscordCoreAPI_Dll __declspec(dllexport)
	#else
		#define DiscordCoreAPI_Dll __declspec(dllimport)
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

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
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

/**
 * \defgroup discord_events Discord Events
 * \brief For all of events that could be sent by Discord's Websockets.
 */

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	class DiscordCoreAPI_Dll Snowflake {
	  public:
		Snowflake() noexcept = default;

		Snowflake& operator=(const std::string&) noexcept;
		Snowflake(const std::string&) noexcept;

		Snowflake& operator=(const uint64_t) noexcept;
		Snowflake(const uint64_t) noexcept;

		operator std::string() noexcept;

		explicit operator uint64_t();

		operator std::string() const noexcept;

		explicit operator uint64_t() const;

		friend std::string operator+(const std::string&, const Snowflake&);

		friend std::string operator+(const char*, const Snowflake&);

		friend bool operator==(const Snowflake&, const Snowflake&);

	  protected:
		uint64_t id{};
	};

	inline std::string operator+(const char* lhs, const Snowflake& rhs) {
		std::string string{};
		string += lhs;
		string += std::to_string(rhs.id);
		return string;
	}

	inline std::string operator+(const std::string& lhs, const Snowflake& rhs) {
		std::string string{};
		string += lhs;
		string += std::to_string(rhs.id);
		return string;
	}

	inline bool operator==(const Snowflake& lhs, const Snowflake& rhs) {
		return lhs.id == rhs.id;
	}

	template<typename RTy> void reverseByteOrder(RTy& net) {
		if constexpr (std::endian::native == std::endian::little) {
			switch (sizeof(RTy)) {
				case 1: {
					return;
				}
				case 2: {
					net =
						_mm256_extract_epi16(_mm256_shuffle_epi8(_mm256_insert_epi16(__m256i{}, net, 0), _mm256_insert_epi16(__m256i{}, 0x01, 0)), 0);
					return;
				}
				case 4: {
					net = _mm256_extract_epi32(
						_mm256_shuffle_epi8(_mm256_insert_epi32(__m256i{}, net, 0), _mm256_insert_epi32(__m256i{}, 0x10203, 0)), 0);
					return;
				}
				case 8: {
					net = _mm256_extract_epi64(
						_mm256_shuffle_epi8(_mm256_insert_epi64(__m256i{}, net, 0), _mm256_insert_epi64(__m256i{}, 0x102030405060708, 0)), 0);
					return;
				}
			}
		}
	}

	template<typename ReturnType> void storeBits(char* to, ReturnType num) {
		const uint8_t byteSize{ 8 };
		reverseByteOrder<ReturnType>(num);
		for (int32_t x = 0; x < sizeof(ReturnType); ++x) {
			to[x] = static_cast<uint8_t>(num >> (byteSize * x));
		}
	}

	template<typename TTy> class StopWatch {
	  public:
		using HRClock = std::chrono::high_resolution_clock;

		StopWatch() = delete;

		StopWatch<TTy>& operator=(const StopWatch<TTy>& data) {
			this->maxNumberOfTimeUnits.store(data.maxNumberOfTimeUnits.load());
			this->startTime.store(data.startTime.load());
			return *this;
		}

		StopWatch(const StopWatch<TTy>& data) {
			*this = data;
		}

		StopWatch(TTy maxNumberOfMsNew) {
			this->maxNumberOfTimeUnits.store(maxNumberOfMsNew);
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

		TTy totalTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			return elapsedTime;
		}

		TTy getTotalWaitTime() {
			return this->maxNumberOfTimeUnits.load();
		}

		bool hasTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			return elapsedTime >= this->maxNumberOfTimeUnits.load();
		}

		void resetTimer() {
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::atomic<TTy> maxNumberOfTimeUnits{ TTy{ 0 } };
		std::atomic<TTy> startTime{ TTy{ 0 } };
	};

	const uint8_t formatVersion{ 131 };

	enum class EtfType : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Atom_Ext = 100,
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Small_Atom_Ext = 115,
		Map_Ext = 116,
	};

	template<typename Ty>
	concept IsEnum = std::is_enum<Ty>::value;

	struct DiscordCoreAPI_Dll EnumConverter {
		template<IsEnum EnumType> EnumConverter& operator=(const std::vector<EnumType>& data) {
			for (auto& value: data) {
				this->vector.emplace_back(std::move(static_cast<uint64_t>(value)));
			}
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(const std::vector<EnumType>& data) {
			*this = data;
		};

		template<IsEnum EnumType> EnumConverter& operator=(EnumType data) {
			this->integer = static_cast<uint64_t>(data);
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(EnumType data) {
			*this = data;
		};

		operator std::vector<uint64_t>() const noexcept;

		operator uint64_t() const noexcept;

		bool isItAVector() const noexcept;

	  protected:
		std::vector<uint64_t> vector{};
		bool vectorType{ false };
		uint64_t integer{};
	};

	enum class JsonType : uint8_t { Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

	enum class JsonifierSerializeType { Etf = 0, Json = 1 };

	class DiscordCoreAPI_Dll Jsonifier;

	template<typename Ty>
	concept IsConvertibleToJsonifier = std::convertible_to<Ty, Jsonifier>;

	class DiscordCoreAPI_Dll Jsonifier {
	  public:
		using MapAllocatorType = std::allocator<std::pair<const std::string, Jsonifier>>;
		template<typename OTy> using AllocatorType = std::allocator<OTy>;
		template<typename OTy> using AllocatorTraits = std::allocator_traits<AllocatorType<OTy>>;
		using ObjectType = std::map<std::string, Jsonifier, std::less<>, MapAllocatorType>;
		using ArrayType = std::vector<Jsonifier, AllocatorType<Jsonifier>>;
		using StringType = std::string;
		using FloatType = double;
		using UintType = uint64_t;
		using IntType = int64_t;
		using BoolType = bool;

		union JsonValue {
			JsonValue() noexcept = default;
			JsonValue& operator=(JsonValue&&) noexcept = delete;
			JsonValue(JsonValue&&) noexcept = delete;
			JsonValue& operator=(const JsonValue&) noexcept = delete;
			JsonValue(const JsonValue&) noexcept = delete;
			ObjectType* object;
			StringType* string;
			ArrayType* array;
			FloatType numberDouble;
			UintType numberUint;
			IntType numberInt;
			BoolType boolean;
		};

		Jsonifier() noexcept = default;

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>&& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->push_back(std::move(value));
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->push_back(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>& data) noexcept {
			*this = data;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsEnum Ty> Jsonifier& operator=(Ty data) noexcept {
			this->jsonValue.numberUint = static_cast<uint64_t>(data);
			this->type = JsonType::Uint64;
			return *this;
		}

		template<IsEnum Ty> Jsonifier(Ty data) noexcept {
			*this = data;
		}

		Jsonifier& operator=(Jsonifier&& data) noexcept;

		Jsonifier(Jsonifier&& data) noexcept;

		Jsonifier& operator=(const Jsonifier& data) noexcept;

		Jsonifier(const Jsonifier& data) noexcept;

		operator std::string&&() noexcept;

		operator std::string() noexcept;

		void refreshString(JsonifierSerializeType OpCode);

		Jsonifier& operator=(EnumConverter&& data) noexcept;
		Jsonifier(EnumConverter&& data) noexcept;

		Jsonifier& operator=(const EnumConverter& data) noexcept;
		Jsonifier(const EnumConverter& data) noexcept;

		Jsonifier& operator=(std::string&& data) noexcept;
		Jsonifier(std::string&& data) noexcept;

		Jsonifier& operator=(const std::string& data) noexcept;
		Jsonifier(const std::string& data) noexcept;

		Jsonifier& operator=(const char* data) noexcept;
		Jsonifier(const char* data) noexcept;

		Jsonifier& operator=(double data) noexcept;
		Jsonifier(double data) noexcept;

		Jsonifier& operator=(float data) noexcept;
		Jsonifier(float data) noexcept;

		Jsonifier& operator=(uint64_t data) noexcept;
		Jsonifier(uint64_t data) noexcept;

		Jsonifier& operator=(uint32_t data) noexcept;
		Jsonifier(uint32_t data) noexcept;

		Jsonifier& operator=(uint16_t data) noexcept;
		Jsonifier(uint16_t data) noexcept;

		Jsonifier& operator=(uint8_t data) noexcept;
		Jsonifier(uint8_t data) noexcept;

		Jsonifier& operator=(int64_t data) noexcept;
		Jsonifier(int64_t data) noexcept;

		Jsonifier& operator=(int32_t data) noexcept;
		Jsonifier(int32_t data) noexcept;

		Jsonifier& operator=(int16_t data) noexcept;
		Jsonifier(int16_t data) noexcept;

		Jsonifier& operator=(int8_t data) noexcept;
		Jsonifier(int8_t data) noexcept;

		Jsonifier& operator=(bool data) noexcept;
		Jsonifier(bool data) noexcept;

		Jsonifier& operator=(JsonType TypeNew) noexcept;
		Jsonifier(JsonType type) noexcept;

		Jsonifier& operator=(std::nullptr_t) noexcept;
		Jsonifier(std::nullptr_t data) noexcept;

		Jsonifier& operator[](typename ObjectType::key_type key);

		Jsonifier& operator[](uint64_t index);

		template<typename Ty> const Ty& getValue() const {
			return Ty{};
		}

		template<typename Ty> Ty& getValue() {
			return Ty{};
		}

		JsonType getType() noexcept;

		void emplaceBack(Jsonifier&& data) noexcept;
		void emplaceBack(Jsonifier& data) noexcept;

		~Jsonifier() noexcept;

	  protected:
		JsonType type{ JsonType::Null };
		JsonValue jsonValue{};
		std::string string{};

		void serializeJsonToEtfString(const Jsonifier* dataToParse);

		void serializeJsonToJsonString(const Jsonifier* dataToParse);

		void writeJsonObject(const ObjectType& ObjectNew);

		void writeJsonArray(const ArrayType& Array);

		void writeJsonString(const StringType& StringNew);

		void writeJsonFloat(const FloatType x);

		template<typename NumberType,
			std::enable_if_t<
				std::is_integral<NumberType>::value || std::is_same<NumberType, uint64_t>::value || std::is_same<NumberType, int64_t>::value, int> =
				0>
		void writeJsonInt(NumberType Int) {
			auto IntNew = std::to_string(Int);
			this->writeString(IntNew.data(), IntNew.size());
		}

		void writeJsonBool(const BoolType ValueNew);

		void writeJsonNull();

		void writeEtfObject(const ObjectType& jsonData);

		void writeEtfArray(const ArrayType& jsonData);

		void writeEtfString(const StringType& jsonData);

		void writeEtfUint(const UintType jsonData);

		void writeEtfInt(const IntType jsonData);

		void writeEtfFloat(const FloatType jsonData);

		void writeEtfBool(const BoolType jsonData);

		void writeEtfNull();

		void writeString(const char* data, size_t length);

		void writeCharacter(const char Char);

		void appendBinaryExt(const std::string& bytes, const uint32_t sizeNew);

		void appendUnsignedLongLong(const uint64_t value);

		void appendNewFloatExt(const double FloatValue);

		void appendSmallIntegerExt(const uint8_t value);

		void appendListHeader(const uint32_t sizeNew);

		void appendMapHeader(const uint32_t sizeNew);

		void appendIntegerExt(const uint32_t value);

		void appendBool(bool data);

		void appendVersion();

		void appendNilExt();

		void appendNil();

		void setValue(JsonType TypeNew);

		void destroy() noexcept;

		friend bool operator==(const Jsonifier& lhs, const Jsonifier& rhs);
	};

	template<> inline const Jsonifier::ObjectType& Jsonifier::getValue() const {
		return *this->jsonValue.object;
	}

	template<> inline const Jsonifier::ArrayType& Jsonifier::getValue() const {
		return *this->jsonValue.array;
	}

	template<> inline const Jsonifier::StringType& Jsonifier::getValue() const {
		return *this->jsonValue.string;
	}

	template<> inline const Jsonifier::FloatType& Jsonifier::getValue() const {
		return this->jsonValue.numberDouble;
	}

	template<> inline const Jsonifier::UintType& Jsonifier::getValue() const {
		return this->jsonValue.numberUint;
	}

	template<> inline const Jsonifier::IntType& Jsonifier::getValue() const {
		return this->jsonValue.numberInt;
	}

	template<> inline const Jsonifier::BoolType& Jsonifier::getValue() const {
		return this->jsonValue.boolean;
	}

	template<> inline Jsonifier::ObjectType& Jsonifier::getValue() {
		return *this->jsonValue.object;
	}

	template<> inline Jsonifier::ArrayType& Jsonifier::getValue() {
		return *this->jsonValue.array;
	}

	template<> inline Jsonifier::StringType& Jsonifier::getValue() {
		return *this->jsonValue.string;
	}

	template<> inline Jsonifier::FloatType& Jsonifier::getValue() {
		return this->jsonValue.numberDouble;
	}

	template<> inline Jsonifier::UintType& Jsonifier::getValue() {
		return this->jsonValue.numberUint;
	}

	template<> inline Jsonifier::IntType& Jsonifier::getValue() {
		return this->jsonValue.numberInt;
	}

	template<> inline Jsonifier::BoolType& Jsonifier::getValue() {
		return this->jsonValue.boolean;
	}

	struct DiscordCoreAPI_Dll ActivityData;

	/// \brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class StreamType { None = 0, Client = 1, Server = 2 };

	/// \brief For connecting two bots to stream the VC contents between the two.
	struct DiscordCoreAPI_Dll StreamInfo {
		bool streamBotAudio{ false };///< Do we stream the audio coming from other bots?
		std::string address{};///< The address to connect to.
		StreamType type{};///< The type of streamer that this is. Set one to client and one to server.
		uint16_t port{};///< The port to connect to.
	};

};

namespace DiscordCoreInternal {

	using HRClock = std::chrono::high_resolution_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	using SysClock = std::chrono::system_clock;
	using Seconds = std::chrono::seconds;
	using namespace std::literals;
	using std::cout;
	using std::endl;

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder;
	class DiscordCoreAPI_Dll YouTubeRequestBuilder;
	class DiscordCoreAPI_Dll WebSocketSSLShard;
	class DiscordCoreAPI_Dll BaseSocketAgent;
	class DiscordCoreAPI_Dll SoundCloudAPI;
	class DiscordCoreAPI_Dll YouTubeAPI;

	enum class WebSocketOpCode : uint8_t {
		Op_Continuation = 0x00,
		Op_Text = 0x01,
		Op_Binary = 0x02,
		Op_Close = 0x08,
		Op_Ping = 0x09,
		Op_Pong = 0x0a
	};

	/// \brief Websocket close codes.
	class DiscordCoreAPI_Dll WebSocketClose {
	  public:
		/// \brief Websocket close codes.
		enum class WebSocketCloseCode : uint16_t {
			Unset = 1 << 0,///< Unset.
			Normal_Close = 1 << 1,///< Normal close.
			Unknown_Error = 1 << 2,///< We're not sure what went wrong. Try reconnecting?
			Unknown_Opcode = 1 << 3,///< You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
			Decode_Error = 1 << 4,///< You sent an invalid payload to us. Don't do that!
			Not_Authenticated = 1 << 5,///< You sent us a payload prior to identifying.
			Authentication_Failed = 1 << 6,///< The account token sent with your identify payload is incorrect.
			Already_Authenticated = 1 << 7,///< You sent more than one identify payload. Don't do that!
			Invalid_Seq = 1 << 8,///<	The sequence sent when resuming the session was invalid. Reconnect and start a new session.
			Rate_Limited =
				1 << 9,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
			Session_Timed = 1 << 10,///< Your session timed out. Reconnect and start a new one.
			Invalid_Shard = 1 << 11,///< You sent us an invalid shard when identifying.
			Sharding_Required =
				1 << 12,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
			Invalid_API_Version = 1 << 13,///< You sent an invalid version for the gateway.
			Invalid_Intent = 1 << 14,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
			Disallowed_Intent = 1
				<< 15,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
			We_Do_Reconnect = Normal_Close | Unknown_Error | Unknown_Opcode | Decode_Error | Not_Authenticated | Already_Authenticated | Invalid_Seq |
				Rate_Limited | Session_Timed,
			We_Do_Not_Reconnect = Authentication_Failed | Invalid_Shard | Sharding_Required | Invalid_API_Version | Invalid_Intent | Disallowed_Intent
		};

		std::unordered_map<uint16_t, WebSocketCloseCode> mappingValues{ { 0, WebSocketCloseCode::Unset }, { 1000, WebSocketCloseCode::Normal_Close },
			{ 4000, WebSocketCloseCode::Unknown_Error }, { 4001, WebSocketCloseCode::Unknown_Opcode }, { 4002, WebSocketCloseCode::Decode_Error },
			{ 4003, WebSocketCloseCode::Not_Authenticated }, { 4004, WebSocketCloseCode::Authentication_Failed },
			{ 4005, WebSocketCloseCode::Already_Authenticated }, { 4007, WebSocketCloseCode::Invalid_Seq },
			{ 4008, WebSocketCloseCode::Rate_Limited }, { 4009, WebSocketCloseCode::Session_Timed }, { 4010, WebSocketCloseCode::Invalid_Shard },
			{ 4011, WebSocketCloseCode::Sharding_Required }, { 4012, WebSocketCloseCode::Invalid_API_Version },
			{ 4013, WebSocketCloseCode::Invalid_Intent }, { 4014, WebSocketCloseCode::Disallowed_Intent } };

		std::unordered_map<WebSocketCloseCode, std::string> outputErrorValues{ {
																				   WebSocketCloseCode::Unknown_Error,
																				   "4000; We're not sure what went wrong.",
																			   },
			{ WebSocketCloseCode::Unknown_Opcode, "4001; You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!" },
			{ WebSocketCloseCode::Decode_Error, "4002; You sent an invalid payload to Discord. Don't do that!" },
			{ WebSocketCloseCode::Not_Authenticated, "4003; You sent us a payload prior to identifying." },
			{ WebSocketCloseCode::Authentication_Failed, "4004; The account token sent with your identify payload is incorrect." },
			{ WebSocketCloseCode::Already_Authenticated, "4005; You sent more than one identify payload. Don't do that!" },
			{ WebSocketCloseCode::Invalid_Seq, "4006; The sequence sent when resuming the session was invalid. Reconnect and start a new session." },
			{ WebSocketCloseCode::Rate_Limited,
				"4008; Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this." },
			{ WebSocketCloseCode::Session_Timed, "4009; Your session timed out. Reconnect and start a new one." },
			{ WebSocketCloseCode::Invalid_Shard, "4010; You sent us an invalid shard when identifying." },
			{ WebSocketCloseCode::Sharding_Required,
				"4011; The session would have handled too many guilds - you are required to shard your connection in order to connect." },
			{ WebSocketCloseCode::Invalid_API_Version, "4012; You sent an invalid version for the gateway." },
			{ WebSocketCloseCode::Invalid_Intent,
				"4013; You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value." },
			{ WebSocketCloseCode::Disallowed_Intent,
				"4014; You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are "
				"not "
				"approved for." } };

		WebSocketCloseCode value{};

		WebSocketClose& operator=(uint16_t valueNew);

		explicit WebSocketClose(uint16_t valueNew);

		operator std::string();

		operator bool();
	};

	/// \brief Voice Websocket close codes.
	class DiscordCoreAPI_Dll VoiceWebSocketClose {
	  public:
		/// \brief Voice Websocket close codes.
		enum class VoiceWebSocketCloseCode : uint16_t {
			Unset = 1 << 0,///< Unset.
			Normal_Close = 1 << 1,///< Normal close.
			Unknown_Opcode = 1 << 2,///< You sent an invalid opcode.
			Failed_To_Decode = 1 << 3,///< You sent an invalid payload in your identifying to the Gateway.
			Not_Authenticated = 1 << 4,///< You sent a payload before identifying with the Gateway.
			Authentication_Failed = 1 << 5,///<	The token you sent in your identify payload is incorrect.
			Already_Authenticated = 1 << 6,///<	You sent more than one identify payload. Stahp.
			Session_No_Longer_Valid = 1 << 7,///< Your session is no longer valid.
			Session_Timeout = 1 << 8,///< Your session has timed out.
			Server_Not_Found = 1 << 9,///< We can't find the server you're trying to connect to.
			Unknown_Protocol = 1 << 10,///< We didn't recognize the protocol you sent.
			Disconnected = 1
				<< 11,///< Channel was deleted, you were kicked, voice server changed, or the main gateway session was dropped. Should not reconnect.
			Voice_Server_Crashed = 1 << 12,///< The server crashed. Our bad! Try resuming.
			Unknown_Encryption_Mode = 1 << 13///< We didn't recognize your encryption.
		};

		std::unordered_map<uint16_t, VoiceWebSocketCloseCode> mappingValues{ { 0, VoiceWebSocketCloseCode::Unset },
			{ 1000, VoiceWebSocketCloseCode::Normal_Close }, { 4001, VoiceWebSocketCloseCode::Unknown_Opcode },
			{ 4002, VoiceWebSocketCloseCode::Failed_To_Decode }, { 4003, VoiceWebSocketCloseCode::Not_Authenticated },
			{ 4004, VoiceWebSocketCloseCode ::Authentication_Failed }, { 4005, VoiceWebSocketCloseCode::Already_Authenticated },
			{ 4006, VoiceWebSocketCloseCode ::Session_No_Longer_Valid }, { 4009, VoiceWebSocketCloseCode::Session_Timeout },
			{ 4011, VoiceWebSocketCloseCode::Server_Not_Found }, { 4012, VoiceWebSocketCloseCode::Unknown_Protocol },
			{ 4014, VoiceWebSocketCloseCode ::Disconnected }, { 4015, VoiceWebSocketCloseCode ::Voice_Server_Crashed },
			{ 4016, VoiceWebSocketCloseCode ::Unknown_Encryption_Mode } };

		std::unordered_map<VoiceWebSocketCloseCode, std::string> outputErrorValues{ { VoiceWebSocketCloseCode::Unset, "0; Unset." },
			{ VoiceWebSocketCloseCode::Normal_Close, "1000; Normal close." },
			{ VoiceWebSocketCloseCode::Unknown_Opcode, "4001; You sent an invalid opcode." },
			{ VoiceWebSocketCloseCode::Failed_To_Decode, "4002; You sent an invalid payload in your identifying to the Gateway." },
			{ VoiceWebSocketCloseCode::Not_Authenticated, "4003; You sent a payload before identifying with the Gateway." },
			{ VoiceWebSocketCloseCode::Authentication_Failed, "4004; The token you sent in your identify payload is incorrect." },
			{ VoiceWebSocketCloseCode::Already_Authenticated, "4005; You sent more than one identify payload. Stahp." },
			{ VoiceWebSocketCloseCode::Session_No_Longer_Valid, "4006; Your session is no longer valid." },
			{ VoiceWebSocketCloseCode::Session_Timeout, "4009; Your session has timed out." },
			{ VoiceWebSocketCloseCode::Server_Not_Found, "4011; We can't find the server you're trying to connect to." },
			{ VoiceWebSocketCloseCode::Unknown_Protocol, "4012; We didn't recognize the protocol you sent." },
			{ VoiceWebSocketCloseCode::Disconnected,
				"4014; Channel was deleted, you were kicked, voice server changed, or the main gateway session was dropped. Should not reconnect." },
			{ VoiceWebSocketCloseCode::Voice_Server_Crashed, "4015; The server crashed. Our bad! Try resuming." },
			{ VoiceWebSocketCloseCode::Unknown_Encryption_Mode, "4016; We didn't recognize your encryption." } };

		VoiceWebSocketCloseCode value{};

		VoiceWebSocketClose& operator=(uint16_t valueNew);

		VoiceWebSocketClose(uint16_t valueNew);

		operator std::string();

		operator bool();
	};

	/// \brief Voice Websocket close codes.
	class DiscordCoreAPI_Dll HttpsResponseCode {
	  public:
		/// \brief Voice Websocket close codes.
		enum class HttpsResponseCodes : uint32_t {
			Ok = 200,///< The request completed successfully.
			Created = 201,///< The entity was created successfully.
			No_Content = 204,///< The request completed successfully but returned no content.
			Not_Modifies = 304,///< The entity was not modified (no action was taken).
			Bad_Request = 400,///< The request was improperly formatted, or the server couldn't understand it.
			Unauthorized = 401,///< The Authorization header was missing or invalid.
			Forbidden = 403,///< The Authorization token you passed did not have permission to the resource.
			Not_Found = 404,///< The resource at the location specified doesn't exist.
			Method_Not_Allowed = 405,///< The HTTP method used is not valid for the location specified.
			Too_Many_Requests = 429,///< You are being rate limited, see Rate Limits.
			Gatewat_Unavailable = 502,///< There was not a gateway available to process your request. Wait a bit and retry.
		};

		std::unordered_map<HttpsResponseCodes, std::string> outputErrorValues{ { static_cast<HttpsResponseCodes>(200),
																				   "200; The request completed successfully" },
			{ static_cast<HttpsResponseCodes>(201), "201; The entity was created successfully" },
			{ static_cast<HttpsResponseCodes>(204), "204; The request completed successfully but returned no content" },
			{ static_cast<HttpsResponseCodes>(304), "304; The entity was not modified (no action was taken)" },
			{ static_cast<HttpsResponseCodes>(400), "400; The request was improperly formatted, or the server couldn't understand it" },
			{ static_cast<HttpsResponseCodes>(401), "401; The Authorization header was missing or invalid" },
			{ static_cast<HttpsResponseCodes>(403), "403; The Authorization token you passed did not have permission to the resource" },
			{ static_cast<HttpsResponseCodes>(404), "404; The resource at the location specified doesn't exist" },
			{ static_cast<HttpsResponseCodes>(405), "405; The HTTP method used is not valid for the location specified" },
			{ static_cast<HttpsResponseCodes>(429), "429; You are being rate limited, see Rate Limits" },
			{ static_cast<HttpsResponseCodes>(502), "502; There was not a gateway available to process your request.Wait a bit and retry" } };

		HttpsResponseCodes value{};

		HttpsResponseCode& operator=(uint32_t valueNew);

		HttpsResponseCode(uint32_t valueNew);

		operator uint32_t();

		operator std::string();
	};

}// namespace DiscordCoreInternal


namespace DiscordCoreAPI {

	using HRClock = std::chrono::high_resolution_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	using SysClock = std::chrono::system_clock;
	using Seconds = std::chrono::seconds;
	using namespace std::literals;
	using std::cout;
	using std::endl;

	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData;
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData;
	struct DiscordCoreAPI_Dll File;

	class DiscordCoreAPI_Dll DiscordCoreClient;
	class DiscordCoreAPI_Dll VoiceConnection;
	class DiscordCoreAPI_Dll GuildMember;
	class DiscordCoreAPI_Dll ChannelData;
	class DiscordCoreAPI_Dll Reactions;
	class DiscordCoreAPI_Dll BotUser;

	struct DiscordCoreAPI_Dll DCAException : public std::runtime_error, std::string {
		DCAException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	template<typename RTy> class CoRoutine;

	/// \brief Update-presence status types.
	enum class UpdatePresenceStatusTypes {
		online = 0,///< Online.
		dnd = 1,///< Do Not Disturb.
		idle = 2,///<	AFK.
		invisible = 3,///< Invisible and shown as offline.
		offline = 4,///< Offline
	};

	/// \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		std::vector<ActivityData> activities{};///< A vector of activities.
		UpdatePresenceStatusTypes status{};///< Current status.
		int64_t since{ 0 };///< When was the activity started?
		bool afk{ false };///< Are we afk.

		operator Jsonifier();
	};

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*function)( void ));

	/// \brief Input event response types.
	enum class InputEventResponseType : uint8_t {
		Unset = 0,///< Unset.
		Deferred_Response = 1,
		Ephemeral_Deferred_Response = 2,///< Deferred ephemeral response.
		Interaction_Response = 3,///< Interaction response.
		Ephemeral_Interaction_Response = 4,///< Ephemeral Interaction response.
		Edit_Interaction_Response = 5,///< Interaction response edit.
		Follow_Up_Message = 6,///< Follow-up Message.
		Ephemeral_Follow_Up_Message = 7,///< Ephemeral follow-up Message.
		Edit_Follow_Up_Message = 8,///< Follow-up Message edit.
		Application_Command_AutoComplete_Result = 9,///< Respond to an autocomplete interaction with suggested choices.
		Modal_Interaction_Response = 10,///< Respond to an interaction with a popup modal.
	};

	/// \brief Gateway intents.
	enum class GatewayIntents : uint32_t {
		Guilds = 1 << 0,///< Intent for receipt of Guild information.
		Guild_Members = 1 << 1,///< Intent for receipt of Guild members.
		Guild_Bans = 1 << 2,///< Intent for receipt of Guild bans.
		Guild_Emojis = 1 << 3,///< Intent for receipt of Guild emojis.
		Guild_Integrations = 1 << 4,///< Intent for receipt of Guild integrations.
		Guild_Webhooks = 1 << 5,///< Intent for receipt of Guild webhooks.
		Guild_Invites = 1 << 6,///< Intent for receipt of Guild invites.
		Guild_VoiceStates = 1 << 7,///< Intent for receipt of Guild voice states.
		Guild_Presences = 1 << 8,///< Intent for receipt of Guild presences.
		Guild_Messages = 1 << 9,///< Intent for receipt of Guild messages.
		Guild_Message_Reactions = 1 << 10,///< Intent for receipt of Guild message reactions.
		Guild_Message_Typing = 1 << 11,///< Intent for receipt of Guild message typing notifications.
		Direct_Messages = 1 << 12,///< Intent for receipt of direct messages (DMs).
		Direct_Message_Reactions = 1 << 13,///< Intent for receipt of direct message reactions.
		Direct_Message_Typing = 1 << 14,///< Intent for receipt of direct message typing notifications.
		Message_Content = 1 << 15,///< Intent for receipt of message content.
		Guild_Scheduled_Events = 1 << 16,///< Scheduled events.
		Default_Intents = Guilds | Guild_Bans | Guild_Emojis | Guild_Integrations | Guild_Webhooks | Guild_Invites | Guild_VoiceStates |
			Guild_Messages | Guild_Message_Reactions | Guild_Message_Typing | Direct_Messages | Direct_Message_Reactions | Direct_Message_Typing |
			Guild_Scheduled_Events,///< Default intents (all non-privileged intents).
		Privileged_Intents = Guild_Members | Guild_Presences | Message_Content,///< Privileged intents requiring ID.
		All_Intents = Default_Intents | Privileged_Intents///< Every single intent.
	};

	/// \brief Function data for repeated functions to be loaded.
	struct DiscordCoreAPI_Dll RepeatedFunctionData {
		std::function<void(DiscordCoreClient*)> function{ nullptr };///< The std::function pointer to be loaded.
		uint32_t intervalInMs{ 0 };///< The time interval at which to call the std::function.
		bool repeated{ false };///< Whether or not the std::function is repeating.
		int64_t dummyArg{ 0 };
	};

	/// \brief Represents which text format to use for websocket transfer.
	enum class TextFormat : uint8_t {
		Etf = 0x00,///< Etf format.
		Json = 0x01///< Json format.
	};

	/// \brief Sharding options for the library.
	struct DiscordCoreAPI_Dll ShardingOptions {
		uint32_t numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		uint32_t totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		uint32_t startingShard{ 0 };///< The first shard to start on this process.
	};

	/**@}*/

	/// \brief Loggin options for the library.
	struct DiscordCoreAPI_Dll LoggingOptions {
		bool logWebSocketSuccessMessages{ false };///< Do we log the websocket success messages to cout?
		bool logWebSocketErrorMessages{ false };///< Do we log the websocket error messages to cout?
		bool logGeneralSuccessMessages{ false };///< Do we log general success messages to cout?
		bool logFFMPEGSuccessMessages{ false };///< Do we log FFMPEG success messages to cout?
		bool logGeneralErrorMessages{ false };///< Do we log general error messages to cout?
		bool logHttpsSuccessMessages{ false };///< Do we log Http response success messages to cout?
		bool logFFMPEGErrorMessages{ false };///< Do we log FFMPEG error messages to cout?
		bool logHttpsErrorMessages{ false };///< Do we log Http response error messages to cout?
	};

	/// \brief For selecting the caching style of the library.
	struct DiscordCoreAPI_Dll CacheOptions {
		bool cacheChannels{ true };///< Do we cache Channels?
		bool cacheGuilds{ true };///< Do we cache Guilds?
		bool cacheRoles{ true };///< Do we cache Roles?
		bool cacheUsers{ true };///< Do we cache Users/GuildMembers?
	};

	/// \brief Configuration data for the library's main class, DiscordCoreClient.
	struct DiscordCoreAPI_Dll DiscordCoreClientConfig {
		std::vector<RepeatedFunctionData> functionsToExecute{};///< Functions to execute after a timer, or on a repetition.
		GatewayIntents intents{ GatewayIntents::All_Intents };///< The gateway intents to be used for this instance.
		TextFormat textFormat{ TextFormat::Etf };///< Use ETF or JSON format for websocket transfer?
		UpdatePresenceData presenceData{};///< Presence data to initialize your bot with.
		std::string connectionAddress{};///< A potentially alternative connection address for the websocket.
		ShardingOptions shardOptions{};///< Options for the sharding of your bot.
		LoggingOptions logOptions{};///< Options for the output/logging of the library.
		CacheOptions cacheOptions{};///< Options for the cache of the library.
		uint16_t connectionPort{};///< A potentially alternative connection port for the websocket.
		std::string botToken{};///< Your bot's token.
	};

	struct DiscordCoreAPI_Dll JsonStringValue {
		std::string value{};
		JsonType type{};
	};

	class DiscordCoreAPI_Dll ConfigManager {
	  public:
		ConfigManager() noexcept = default;

		explicit ConfigManager(const DiscordCoreClientConfig&);

		const bool doWePrintWebSocketSuccessMessages() const;

		const bool doWePrintWebSocketErrorMessages() const;

		const bool doWePrintHttpsSuccessMessages() const;

		const bool doWePrintHttpsErrorMessages() const;

		const bool doWePrintFFMPEGSuccessMessages() const;

		const bool doWePrintFFMPEGErrorMessages() const;

		const bool doWePrintGeneralSuccessMessages() const;

		const bool doWePrintGeneralErrorMessages() const;

		const bool doWeCacheChannels() const;

		const bool doWeCacheUsers() const;

		const bool doWeCacheGuilds() const;

		const bool doWeCacheRoles() const;

		const UpdatePresenceData getPresenceData() const;

		const std::string getBotToken() const;

		const uint32_t getTotalShardCount() const;

		const uint32_t getStartingShard() const;

		const uint32_t getShardCountForThisProcess() const;

		const std::string getConnectionAddress() const;

		void setConnectionAddress(const std::string& connectionAddressNew);

		const uint16_t getConnectionPort() const;

		void setConnectionPort(const uint16_t connectionPortNew);

		const std::vector<RepeatedFunctionData> getFunctionsToExecute() const;

		const TextFormat getTextFormat() const;

		const GatewayIntents getGatewayIntents();

	  protected:
		DiscordCoreClientConfig config{};
	};

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// \brief Color constants for use in the EmbedData color values.
	namespace Colors {
		const std::string White = "FFFFFF",///< White.
			DiscordWhite = "FFFFFE",///< Discord white.
			LightGray = "C0C0C0",///< Light gray.
			Gray = "808080",///< Gray.
			DarkGray = "404040",///< Dark gray.
			Black = "000000",///< Black.
			DiscordBlack = "000001",///< Discord black.
			Red = "FF0000",///< Red.
			Pink = "FFAFAF",///< Pink.
			Orange = "FFC800",///< Orange.
			Yellow = "FFFF00",///< Yellow.
			Green = "00FF00",///< Green.
			Magenta = "FF00FF",///< Magenta.
			Cyan = "00FFFF",///< Cyan.
			Blue = "0000FF",///< Blue.
			LightSeaGreen = "1ABC9C",///< Light sea green.
			MediumSeaGreen = "2ECC71",///< Medium sea green.
			SummerSky = "3498DB",///< Summer skye.
			DeepLilac = "9B59B6",///< Deep lilac.
			Ruby = "E91E63",///< Ruby.
			MoonYellow = "F1C40F",///< Moon yellow.
			TahitiGold = "E67E22",///< Tahiti gold.
			Cinnabar = "E74C3C",///< Cinnabar.
			Submarine = "95A5A6",///< Submarine.
			BlueAquamarine = "607D8B",///< Blue aquamarine.
			DeepSea = "11806A",///< Deep sea.
			SeaGreen = "1F8B4C",///< Sea green.
			Endeavour = "206694",///< Endeavor.
			VividViolet = "71368A",///< Vivid violet.
			JazzberryJam = "AD1457",///< Jazzberry jam.
			DarkGoldenrod = "C27C0E",///< Dark goldenrod.
			Rust = "A84300",///< Rust.
			Brown = "992D22",///< Brown.
			GrayChateau = "979C9F",///< Gray chateau.
			Bismark = "546E7A",///< Bismark.
			StiBlue = "0E4BEF",///< Sti blue.
			WrxBlue = "00247D",///< Wrx blue.
			RalliArtCrimson = "E60012",///< Ralliart crimson.
			Lime = "00FF00",///< Lime.
			ForestGreen = "228B22",///< Forest green.
			CadmiumGreen = "097969",///< Cadmium green.
			Aquamarine = "7FFFD4",///< Aquamarine.
			BlueGreen = "088F8F",///< Blue green.
			Raspberry = "E30B5C",///< Raspberry.
			ScarletRed = "FF2400";///< Scarlet red.
	};

	template<typename OTy> class ObjectCache {
	  public:
		ObjectCache() noexcept {};

		void emplace(OTy&& data) noexcept {
			std::unique_lock lock{ this->accessMutex };
			this->set.emplace(std::move(data));
		}

		void emplace(OTy& data) noexcept {
			std::unique_lock lock{ this->accessMutex };
			this->set.emplace(data);
		}

		const OTy& readOnly(OTy& key) noexcept {
			std::shared_lock lock{ this->accessMutex };
			return *this->set.find(key);
		}

		OTy& at(OTy&& key) noexcept {
			std::shared_lock lock{ this->accessMutex };
			return ( OTy& )*this->set.find(key);
		}

		OTy& at(OTy& key) noexcept {
			std::shared_lock lock{ this->accessMutex };
			return ( OTy& )*this->set.find(key);
		}

		auto begin() {
			std::unique_lock lock{ this->accessMutex };
			return this->set.begin();
		}

		auto end() {
			std::unique_lock lock{ this->accessMutex };
			return this->set.end();
		}

		const bool contains(OTy& key) noexcept {
			std::shared_lock lock{ this->accessMutex };
			return this->set.contains(key);
		}

		void erase(OTy& key) {
			if (this->set.contains(key)) {
				std::unique_lock lock{ this->accessMutex };
				this->set.erase(key);
			}
		}

		OTy& operator[](OTy& key) {
			if (!this->contains(key)) {
				std::shared_lock lock{ this->accessMutex };
				this->set.emplace(key);
			}
			return ( OTy& )*this->set.find(key);
		}

		OTy& operator[](OTy&& key) {
			if (!this->contains(key)) {
				std::shared_lock lock{ this->accessMutex };
				this->set.emplace(std::move(key));
			}
			return ( OTy& )*this->set.find(key);
		}

		uint64_t size() noexcept {
			std::unique_lock lock{ this->accessMutex };
			return this->set.size();
		}

	  protected:
		std::unordered_set<OTy> set{};
		std::shared_mutex accessMutex{};
	};

	class DiscordCoreAPI_Dll StringWrapper {
	  public:
		StringWrapper() noexcept = default;

		StringWrapper& operator=(StringWrapper&& other) noexcept;

		StringWrapper(StringWrapper&& other) noexcept;

		StringWrapper& operator=(const StringWrapper& other);

		StringWrapper(const StringWrapper& other);

		StringWrapper& operator=(const std::string& string);

		explicit StringWrapper(const std::string& string);

		StringWrapper& operator=(const char* string);

		StringWrapper(const char* string);

		operator std::string();

		void emplace_back(char value);

		uint64_t size();

		const char* data();

	  protected:
		std::unique_ptr<char[]> ptr{};
	};

	inline std::basic_ostream<char>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, const StringWrapper& rhs) {
		for (auto& value: static_cast<std::string>(static_cast<StringWrapper>(rhs))) {
			lhs.put(value);
		}
		return lhs;
	}

	inline std::basic_string<char> operator+(const std::basic_string<char, std::char_traits<char>, std::allocator<char>>& lhs, StringWrapper rhs) {
		std::stringstream stream{};
		stream << lhs << rhs;
		return stream.str();
	}

	inline std::basic_string<char> operator+(const char* lhs, StringWrapper rhs) {
		std::stringstream stream{};
		stream << lhs << rhs;
		return stream.str();
	}

	inline bool operator==(StringWrapper lhs, const char* rhs) {
		return static_cast<std::string>(lhs) == static_cast<std::string>(rhs);
	}

	inline bool operator!=(StringWrapper lhs, const char* rhs) {
		return static_cast<std::string>(lhs) != rhs;
	}

	inline bool operator==(std::string& lhs, StringWrapper& rhs) {
		return lhs == rhs;
	}

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// \brief Audio frame types.
	enum class AudioFrameType : uint8_t {
		Unset = 0,///< Unset.
		Encoded = 1,///< Encoded.
		RawPCM = 2,///< Raw PCM.
		Skip = 3///< Skip.
	};

	/// \brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		int64_t sampleCount{ -1ll };///< The number of samples per this frame.
		uint64_t guildMemberId{ 0 };///< GuildMemberId for the sending GuildMember.
		size_t currentSize{};///< The current size of the allocated memory.
		std::string data{};///< The audio data.

		AudioFrameData() noexcept = default;

		AudioFrameData& operator=(AudioFrameData&&) noexcept;

		AudioFrameData(AudioFrameData&&) noexcept;

		AudioFrameData& operator=(const AudioFrameData&) noexcept = delete;

		AudioFrameData(const AudioFrameData&) noexcept = delete;

		AudioFrameData& operator+=(std::string_view) noexcept;

		AudioFrameData& operator+=(char);

		friend bool operator==(const AudioFrameData& lhs, const AudioFrameData& rhs);

		void clearData() noexcept;
	};

	/// For connecting to a voice-channel. "streamInfo" is used when a socket is created to connect this bot to another bot, for transmitting audio back and forth.
	/// \brief For connecting to a voice-channel. "streamInfo" is used when a socket is created to connect this bot to another bot, for transmitting audio back and forth.
	struct DiscordCoreAPI_Dll VoiceConnectInitData {
		DiscordCoreAPI::StreamInfo streamInfo{};///< The info for the stream-socekt, if applicable.
		bool selfDeaf{ false };///< Self-deafen the bot?
		bool selfMute{ false };///< Self-mute the bot?
		int32_t currentShard{};///< The current websocket shard, if applicable.
		Snowflake channelId{};///< The channel id to connect to.
		Snowflake guildId{};///< The guild id to connect to.
		Snowflake userId{};///< This bot's user id.
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// \brief Time formatting methods.
	enum class TimeFormat : char {
		LongDate = 'D',///< "20 April 2021" - Long Date
		LongDateTime = 'F',///< "Tuesday, 20 April 2021 16:20" - Long Date/Time
		LongTime = 'T',///< "16:20:30" - Long Time
		ShortDate = 'd',///< "20/04/2021" - Short Date
		ShortDateTime = 'f',///< "20 April 2021 16:20" - Short Date/Time
		ShortTime = 't',///< "16:20" - Short Time
	};

	DiscordCoreAPI_Dll uint64_t strtoull(std::string_view string);

	template<typename RTy> RTy fromString(const std::string& string, std::ios_base& (*type)( std::ios_base& )) {
		RTy value{};
		std::istringstream stream(string);
		stream >> type, stream >> value;
		return value;
	}

	template<typename RTy> std::string toHex(RTy inputValue) {
		std::stringstream stream{};
		stream << std::setfill('0') << std::setw(sizeof(RTy) * 2) << std::hex << inputValue;
		return stream.str();
	}

	class DiscordCoreAPI_Dll RGBColorValue {
	  public:
		uint8_t green{};
		uint8_t blue{};
		uint8_t red{};
	};

	using HexColorValue = std::string;

	class DiscordCoreAPI_Dll ColorValue {
	  public:
		ColorValue(std::string hexColorValue);

		ColorValue(uint32_t colorValue);

		RGBColorValue getRgbColorValue();

		HexColorValue getHexColorValue();

		uint32_t getIntColorValue();

	  protected:
		uint32_t color{};
	};

	enum class HashType {
		User_Avatar = 0,
		Channel_Icon = 1,
		GuildMember_Avatar = 2,
		Guild_Icon = 3,
		Guild_Splash = 4,
		Guild_Banner = 5,
		Guild_Discovery = 6
	};

	class DiscordCoreAPI_Dll IconHash {
	  public:
		IconHash() noexcept = default;

		IconHash& operator=(const std::string& string);

		IconHash(const std::string& string) noexcept;

		bool operator==(const IconHash& other);

		std::string getIconHash() noexcept;

		~IconHash() noexcept = default;

	  protected:
		uint64_t highBits{};
		uint64_t lowBits{};
	};

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief Permission values, for a given Channel, by Role or GuildMember.
	enum class Permission : uint64_t {
		Create_Instant_Invite = 1ull << 0,///< Create Instant Invite.
		Kick_Members = 1ull << 1,///< Kick Members.
		Ban_Members = 1ull << 2,///< Ban Members.
		Administrator = 1ull << 3,///< Administrator.
		Manage_Channels = 1ull << 4,///< Manage Channels.
		Manage_Guild = 1ull << 5,///< Manage Guild.
		Add_Reactions = 1ull << 6,///< Add Reactions.
		View_Audit_Log = 1ull << 7,///< View Audit Log.
		Priority_Speaker = 1ull << 8,///< Priority Speaker.
		Stream = 1ull << 9,///< Stream.
		View_Channel = 1ull << 10,///< View Channel.
		Send_Messages = 1ull << 11,///< Send Messages.
		Send_Tts_Messages = 1ull << 12,///< Send TTS Messages.
		Manage_Messages = 1ull << 13,///< Manage Messages.
		Embed_Links = 1ull << 14,///< Embed Links.
		Attach_Files = 1ull << 15,///< Attach Files.
		Read_Message_History = 1ull << 16,///< Read Message History.
		Mention_Everyone = 1ull << 17,///< Mention Everyone.
		Use_External_Emojis = 1ull << 18,///< Use External Emoji.
		View_Guild_Insights = 1ull << 19,///< View Guild Insights.
		Connect = 1ull << 20,///< Connect.
		Speak = 1ull << 21,///< Speak.
		Mute_Members = 1ull << 22,///< Mute Members.
		Deafen_Members = 1ull << 23,///< Deafen Members.
		Move_Members = 1ull << 24,///< Move Members.
		Use_Vad = 1ull << 25,///< Use VAD.
		Change_Nickname = 1ull << 26,///< Change Nickname.
		Manage_Nicknames = 1ull << 27,///< Manage Nicknames.
		Manage_Roles = 1ull << 28,///< Manage Roles.
		Manage_Webhooks = 1ull << 29,///< Manage Webhooks.
		Manage_Emojis_And_Stickers = 1ull << 30,///< Manage Emojis And Stickers.
		Use_Application_Commands = 1ull << 31,///< Use Application Commands.
		Request_To_Speak = 1ull << 32,///< Request To Speak.
		Manage_Events = 1ull << 33,///< Manage Events.
		Manage_Threads = 1ull << 34,///< Manage Threads.
		Create_Public_Threads = 1ull << 35,///< Create Public Threads.
		Create_Private_Threads = 1ull << 36,///< Create Private Threads.
		Use_External_Stickers = 1ull << 37,///< Use External Stickers.
		Send_Messages_In_Threads = 1ull << 38,///< Send Messages In Threads.
		Start_Embedded_Activities = 1ull << 39,///< Start Embedded Activities.
		Moderate_Members = 1ull << 40///< Moderate Members.
	};

	/// \brief Permissions class, for representing and manipulating Permission values.
	class DiscordCoreAPI_Dll Permissions {
	  public:
		Permissions() noexcept = default;

		Permissions& operator=(Permission&& other);

		Permissions(Permission&& permsNew);

		Permissions& operator=(const Permission& other);

		explicit Permissions(const Permission& permsNew);

		Permissions& operator=(std::string&& other);

		explicit Permissions(std::string&& permsNew);

		Permissions& operator=(const std::string& other);

		explicit Permissions(const std::string& permsNew);

		Permissions& operator=(const uint64_t other);

		explicit Permissions(const uint64_t permsNew);

		operator uint64_t();

		operator std::string();

		/// \brief Returns a string containing all of a given User's Permissions for a given Channel.
		/// \param guildMember The GuildMember who's Permissions to analyze.
		/// \param channel The Channel withint which to check for Permissions.
		/// \returns std::string A string containing the final Permission's value for a given Channel.
		static std::string getCurrentChannelPermissions(const GuildMember& guildMember, const ChannelData& channel);

		/// \brief Checks for a given Permission in a chosen Channel, for a specific User.
		/// \param guildMember The GuildMember who to check the Permissions of.
		/// \param channel The Channel within which to check for the Permission's presence.
		/// \param permission A Permission to check the current Channel for.
		/// \returns bool A bool suggesting the presence of the chosen Permission.
		bool checkForPermission(const GuildMember& guildMember, const ChannelData& channel, Permission permission);

		/// \brief Returns a string containing the currently held Permissions in a given Guild.
		/// \param guildMember The GuildMember who's Permissions are to be evaluated.
		/// \returns std::string A string containing the current Permissions.
		static std::string getCurrentGuildPermissions(const GuildMember& guildMember);

		/// \brief Removes one or more Permissions from the current Permissions value.
		/// \param permissionsToRemove A vector containing the Permissions you wish to remove.
		void removePermissions(const std::vector<Permission>& permissionsToRemove);

		/// \brief Adds one or more Permissions to the current Permissions value.
		/// \param permissionsToAdd A vector containing the Permissions you wish to add.
		void addPermissions(const std::vector<Permission>& permissionsToAdd);

		/// \brief Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format.
		/// \returns std::vector A vector full of strings of the Permissions that are in the input std::string's value.
		std::vector<std::string> displayPermissions();

		/// \brief Returns a string containing the currently held Permissions.
		/// \returns std::string A string containing the current Permissions.
		std::string getCurrentPermissionString();

		/// \brief Returns a string containing ALL of the possible Permissions.
		/// \returns std::string A string containing all of the possible Permissions.
		static std::string getAllPermissions();

	  protected:
		uint64_t permissions{};

		static std::string computeOverwrites(const std::string& basePermissions, const GuildMember& guildMember, const ChannelData& channel);

		static std::string computePermissions(const GuildMember& guildMember, const ChannelData& channel);

		static std::string computeBasePermissions(const GuildMember& guildMember);
	};

	/// \brief Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block.
	/// \param currentFunctionName A string to display the current function's name.
	/// \param location For deriving the current file, line, and column - do not set this value.
	DiscordCoreAPI_Dll void reportException(const std::string& currentFunctionName, std::source_location location = std::source_location::current());

	DiscordCoreAPI_Dll void rethrowException(const std::string& currentFunctionName, std::source_location location = std::source_location::current());

	DiscordCoreAPI_Dll std::string constructMultiPartData(const std::string& data, const std::vector<File>& files);

	DiscordCoreAPI_Dll std::string convertToLowerCase(const std::string& stringToConvert);

	DiscordCoreAPI_Dll std::string base64Encode(const std::string&, bool = false);

	DiscordCoreAPI_Dll std::string loadFileContents(const std::string& filePath);

	DiscordCoreAPI_Dll std::string utf8MakeValid(const std::string& inputString);

	DiscordCoreAPI_Dll std::string urlEncode(const std::string& inputString);

	DiscordCoreAPI_Dll std::string escapeCharacters(std::string_view string);

	DiscordCoreAPI_Dll void spinLock(uint64_t timeInNsToSpinLockFor);

	DiscordCoreAPI_Dll std::string generateBase64EncodedKey();

	DiscordCoreAPI_Dll std::string shiftToBrightGreen();

	DiscordCoreAPI_Dll std::string shiftToBrightBlue();

	DiscordCoreAPI_Dll std::string shiftToBrightRed();

	DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);

	DiscordCoreAPI_Dll std::string reset();

	/// \brief Acquires a timeStamp with the current time and date - suitable for use in message-embeds.
	/// \returns std::string A string containing the current date-time stamp.
	DiscordCoreAPI_Dll std::string getTimeAndDate();


	/// \brief Class for representing a timeStamp, as well as working with time-related values.
	class TimeStamp {
	  public:
		explicit TimeStamp(TimeFormat formatNew = TimeFormat::LongDateTime) {
			this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
		}

		TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second,
			TimeFormat formatNew) {
			this->getTimeSinceEpoch(stoull(year), stoull(month), stoull(day), stoull(hour), stoull(minute), stoull(second));
		}

		operator std::string() {
			return getISO8601TimeStamp(TimeFormat::LongDateTime);
		}

		operator uint64_t() {
			if (this->timeStampInTimeUnits == 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
			return this->timeStampInTimeUnits;
		}

		TimeStamp& operator=(std::string&& originalTimeStampNew) {
			this->convertTimeStampToTimeUnits(TimeFormat::LongDateTime, originalTimeStampNew);
			return *this;
		}

		explicit TimeStamp(std::string&& originalTimeStampNew) {
			*this = std::move(originalTimeStampNew);
			if (this->timeStampInTimeUnits == 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
		}

		TimeStamp& operator=(std::string& originalTimeStampNew) {
			this->convertTimeStampToTimeUnits(TimeFormat::LongDateTime, originalTimeStampNew);
			return *this;
		}

		explicit TimeStamp(std::string& originalTimeStampNew) {
			*this = originalTimeStampNew;
		}

		TimeStamp& operator=(const TimeStamp& other) {
			this->timeStampInTimeUnits = other.timeStampInTimeUnits;
			return *this;
		}

		TimeStamp(const TimeStamp& other) {
			*this = other;
		}

		TimeStamp(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, TimeFormat formatNew) {
			this->getTimeSinceEpoch(year, month, day, hour, minute, second);
		};

		TimeStamp(uint64_t timeInTimeUnits, TimeFormat formatNew) {
			this->timeStampInTimeUnits = timeInTimeUnits;
		}

		static std::string convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd,
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

		static std::string convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
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
			if (this->timeStampInTimeUnits <= 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
			uint64_t startTimeRaw = this->timeStampInTimeUnits;
			auto currentTime = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			uint64_t secondsPerMinute = 60;
			uint64_t secondsPerHour = secondsPerMinute * 60;
			uint64_t secondsPerDay = secondsPerHour * 24;
			auto targetElapsedTime = ((days * secondsPerDay) + (hours * secondsPerHour) + (minutes * secondsPerMinute)) * 1000;
			auto actualElapsedTime = currentTime - startTimeRaw;
			if (actualElapsedTime >= targetElapsedTime) {
				return true;
			} else {
				return false;
			}
		}

		static std::string convertMsToDurationString(uint64_t durationInMs) {
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

		void getTimeSinceEpoch(int64_t year, int64_t month, int64_t day, int64_t hour, int64_t minute, int64_t second) {
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
			this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(value).count();
		}

		void convertTimeStampToTimeUnits(TimeFormat formatNew, std::string originalTimeStamp) {
			try {
				if (originalTimeStamp != "" && originalTimeStamp != "0") {
					TimeStamp timeValue = TimeStamp{ stoi(originalTimeStamp.substr(0, 4)), stoi(originalTimeStamp.substr(5, 6)),
						stoi(originalTimeStamp.substr(8, 9)), stoi(originalTimeStamp.substr(11, 12)), stoi(originalTimeStamp.substr(14, 15)),
						stoi(originalTimeStamp.substr(17, 18)), formatNew };
					this->timeStampInTimeUnits = static_cast<uint64_t>(timeValue);
				} else {
					this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
				}
			} catch (...) {
				reportException("TimeStamp::convertTimeStampToTimeUnits()");
			}
		}

		std::string getISO8601TimeStamp(TimeFormat timeFormat) {
			if (this->timeStampInTimeUnits <= 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
			uint64_t timeValue = this->timeStampInTimeUnits / 1000;
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

	template<typename ITy>
	concept IsInteger = std::is_integral<ITy>::value;

	template<IsInteger StoredAs, IsEnum OTy> auto setBool(StoredAs theFlags, OTy theFlagToSet, bool enabled) {
		auto theValue{ theFlags };
		if (enabled) {
			theValue |= std::to_underlying(theFlagToSet);
		} else {
			theValue &= ~std::to_underlying(theFlagToSet);
		}
		return theValue;
	}

	template<IsEnum OTy> bool getBool(OTy theFlags, OTy theFlagToCheckFor) {
		return std::to_underlying(theFlags) & std::to_underlying(theFlagToCheckFor);
	}

	template<typename OTy>
	concept CopyableOrMovable = std::copyable<OTy> || std::movable<OTy>;

	/// \brief A thread-safe messaging block for data-structures.
	/// \tparam OTy The type of object that will be sent over
	///  message block.
	template<CopyableOrMovable OTy> class UnboundedMessageBlock {
	  public:
		UnboundedMessageBlock<OTy>& operator=(UnboundedMessageBlock<OTy>&& other) noexcept {
			if (this != &other) {
				this->queue = std::move(other.queue);
				other.queue = std::deque<OTy>{};
			}
			return *this;
		}

		UnboundedMessageBlock(UnboundedMessageBlock<OTy>&& other) noexcept {
			*this = std::move(other);
		}

		UnboundedMessageBlock<OTy>& operator=(const UnboundedMessageBlock<OTy>&) = delete;

		UnboundedMessageBlock(const UnboundedMessageBlock<OTy>&) = delete;

		UnboundedMessageBlock() noexcept {};

		/// \brief Sends an object of type OTy to the "recipient", ahead of other already queued items.
		/// \param object An object of OTy.
		void sendFront(OTy&& object) {
			std::unique_lock lock{ this->accessMutex };
			this->queue.emplace_front(std::move(object));
		}

		/// \brief Sends an object of type OTy to the "recipient", ahead of other already queued items.
		/// \param object An object of OTy.
		void sendFront(OTy& object) {
			std::unique_lock lock{ this->accessMutex };
			this->queue.emplace_front(object);
		}

		/// \brief Sends an object of type OTy to the "recipient".
		/// \param object An object of OTy.
		void send(OTy&& object) {
			std::unique_lock lock{ this->accessMutex };
			this->queue.emplace_back(std::move(object));
		}

		/// \brief Sends an object of type OTy to the "recipient".
		/// \param object An object of OTy.
		void send(OTy& object) {
			std::unique_lock lock{ this->accessMutex };
			this->queue.emplace_back(std::move(object));
		}

		/// \brief Sends an object of type OTy to the "recipient".
		/// \param object An object of OTy.
		void send(const OTy&& object) {
			std::unique_lock lock{ this->accessMutex };
			this->queue.emplace_back(object);
		}

		/// \brief Sends an object of type OTy to the "recipient".
		/// \param object An object of OTy.
		void send(const OTy& object) {
			std::unique_lock lock{ this->accessMutex };
			this->queue.emplace_back(std::move(object));
		}

		/// \brief Clears the contents of the messaging block.
		void clearContents() {
			std::unique_lock lock{ this->accessMutex };
			this->queue.clear();
			this->queue = std::deque<OTy>{};
		}

		/// \brief Tries to receive an object of type OTy to be placed into a reference.
		/// \param object A reference of type OTy for placing the potentially received object.
		/// \returns bool A bool, denoting whether or not we received an object.
		bool tryReceive(OTy& object) {
			std::unique_lock lock{ this->accessMutex };
			if (this->queue.size() > 0) {
				object = std::move(this->queue.front());
				this->queue.pop_front();
				return true;
			} else {
				return false;
			}
		}

	  protected:
		std::deque<OTy> queue{};
		std::mutex accessMutex{};
	};

	template<typename RTy> class NewThreadAwaiter;

	/// \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam RTy The type of value returned by the containing CoRoutine.
	template<typename RTy> auto NewThreadAwaitable() {
		return NewThreadAwaiter<RTy>{};
	}

	/// \brief Typedef for the message filter.
	template<typename OTy> using ObjectFilter = std::function<bool(OTy)>;

	/// \brief ObjectCollectorReturnData responseData.
	template<typename OTy> struct DiscordCoreAPI_Dll ObjectCollectorReturnData {
		std::vector<OTy> objects{};///< A vector of collected Objects.
	};

	/// \brief OTy collector, for collecting Objects from a Channel.
	template<typename OTy> class DiscordCoreAPI_Dll ObjectCollector {
	  public:
		static std::unordered_map<std::string, UnboundedMessageBlock<OTy>*> objectsBuffersMap;

		ObjectCollector() noexcept = default;

		/// \brief Begin waiting for Objects.
		/// \param quantityToCollect Maximum quantity of Objects to collect before returning the results.
		/// \param msToCollectForNew Maximum number of Milliseconds to wait for Objects before returning the results.
		/// \param filteringFunctionNew A filter function to apply to new Objects, where returning "true" from the function results in a Object being stored.
		/// \returns A ObjectCollectorReturnData structure.
		CoRoutine<ObjectCollectorReturnData<OTy>> collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew,
			ObjectFilter<OTy> filteringFunctionNew) {
			co_await NewThreadAwaitable<ObjectCollectorReturnData<OTy>>();
			this->quantityOfObjectToCollect = quantityToCollect;
			this->filteringFunction = filteringFunctionNew;
			this->msToCollectFor = msToCollectForNew;
			this->collectorId = std::to_string(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
			ObjectCollector::objectsBuffersMap[this->collectorId] = &this->objectsBuffer;
			this->run();
			co_return std::move(this->objectReturnData);
		}

		void run() {
			int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
			int64_t elapsedTime{ 0 };
			while (elapsedTime < this->msToCollectFor) {
				OTy message{};
				waitForTimeToPass<OTy>(this->objectsBuffer, message, static_cast<int32_t>(this->msToCollectFor - elapsedTime));
				if (this->filteringFunction(message)) {
					this->objectReturnData.objects.emplace_back(message);
				}
				if (static_cast<int32_t>(this->objectReturnData.objects.size()) >= this->quantityOfObjectToCollect) {
					break;
				}

				elapsedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count() - startingTime;
			}
		}

		~ObjectCollector() {
			if (ObjectCollector::objectsBuffersMap.contains(this->collectorId)) {
				ObjectCollector::objectsBuffersMap.erase(this->collectorId);
			}
		}

	  protected:
		ObjectCollectorReturnData<OTy> objectReturnData{};
		ObjectFilter<OTy> filteringFunction{ nullptr };
		UnboundedMessageBlock<OTy> objectsBuffer{};
		int32_t quantityOfObjectToCollect{ 0 };
		int32_t msToCollectFor{ 0 };
		std::string collectorId{};
	};

	template<typename OTy> bool waitForTimeToPass(UnboundedMessageBlock<OTy>& outBuffer, OTy& argOne, int32_t timeInMsNew) {
		StopWatch stopWatch{ Milliseconds{ timeInMsNew } };
		while (!outBuffer.tryReceive(argOne)) {
			std::this_thread::sleep_for(1ms);
			if (stopWatch.hasTimePassed()) {
				return true;
			}
		};
		return false;
	}

	/**@}*/
};

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll LengthData {
		uint64_t offSet{};
		uint64_t length{};
	};

	class DiscordCoreAPI_Dll StringBuffer {
	  public:
		StringBuffer() noexcept;

		std::string_view operator[](LengthData);

		char operator[](uint64_t);

		void writeData(const char* ptr, uint64_t size);

		std::string::iterator begin();

		std::string::iterator end();

		void erase(uint64_t);

		uint64_t size();

		char* data();

		void clear();

	  protected:
		std::string string01{};
		uint64_t sizeValue{};
	};

	enum class RingBufferAccessType { Read = 0, Write = 1 };

	template<typename OTy, uint64_t Size> class RingBufferInterface {
	  public:
		void modifyReadOrWritePosition(RingBufferAccessType type, uint64_t size) noexcept {
			if (type == RingBufferAccessType::Read) {
				this->tail += size;
				this->areWeFull = false;
			} else {
				this->head += size;
				if (this->head == this->tail) {
					this->areWeFull = true;
				}
				if (this->head != this->tail) {
					this->areWeFull = false;
				}
			}
		}

		uint64_t getUsedSpace() noexcept {
			if (this->areWeFull) {
				return this->arrayValue.size();
			}
			if ((this->head % this->arrayValue.size()) >= (this->tail % this->arrayValue.size())) {
				uint64_t freeSpace = this->arrayValue.size() - ((this->head % this->arrayValue.size()) - (this->tail % this->arrayValue.size()));
				return this->arrayValue.size() - freeSpace;
			} else {
				uint64_t freeSpace = (this->tail % this->arrayValue.size()) - (this->head % this->arrayValue.size());
				return this->arrayValue.size() - freeSpace;
			}
		}

		uint64_t getFreeSpace() noexcept {
			return this->arrayValue.size() - this->getUsedSpace();
		}

		OTy* getCurrentTail() noexcept {
			return (this->arrayValue.data() + (this->tail % (this->arrayValue.size())));
		}

		OTy* getCurrentHead() noexcept {
			return (this->arrayValue.data() + (this->head % (this->arrayValue.size())));
		}

		bool isItFull() noexcept {
			return this->areWeFull;
		}

		virtual void clear() noexcept {
			this->areWeFull = false;
			this->tail = 0;
			this->head = 0;
		}

	  protected:
		std::array<OTy, Size> arrayValue{};
		bool areWeFull{ false };
		uint64_t tail{};
		uint64_t head{};
	};

	template<typename OTy> class RingBufferSlice : public RingBufferInterface<OTy, 1024 * 16> {};

	template<typename OTy, uint64_t SliceCount> class RingBuffer : public RingBufferInterface<RingBufferSlice<OTy>, SliceCount> {
	  public:
		void clear() noexcept {
			for (uint64_t x = 0; x < this->arrayValue.size(); ++x) {
				this->arrayValue[x].clear();
			}
			this->areWeFull = false;
			this->tail = 0;
			this->head = 0;
		}
	};
}

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::Snowflake> {
	uint64_t operator()(DiscordCoreAPI::Snowflake const& object) const noexcept {
		return object.operator size_t();
	}
};
