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
/// Etf.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file Etf.hpp

#pragma once

#include <discordcoreapi/LightString.hpp>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll DCAException : public std::runtime_error, std::string {
		DCAException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

}

namespace DiscordCoreInternal {

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

	struct DiscordCoreAPI_Dll EtfParseError : public DiscordCoreAPI::DCAException {
	  public:
		explicit EtfParseError(const std::string& message);
	};

	constexpr uint8_t formatVersion{ 131 };

	class DiscordCoreAPI_Dll EtfParser {
	  public:
		friend class WebSocketClient;

		std::string_view parseEtfToJson(std::string_view dataToParse);

	  protected:
		DiscordCoreAPI::String finalString{};
		const char* dataBuffer{};
		uint64_t currentSize{};
		uint64_t dataSize{};
		uint64_t offSet{};

		template<typename RTy> RTy readBitsFromBuffer() {
			if (offSet + sizeof(RTy) > dataSize) {
				throw EtfParseError{ "EtfParser::readBitsFromBuffer() Error: readBitsFromBuffer() past end of the buffer." };
			}
			RTy newValue = *reinterpret_cast<const RTy*>(dataBuffer + offSet);
			offSet += sizeof(RTy);
			reverseByteOrder<RTy>(newValue);
			return newValue;
		}

		uint8_t hex2dec(char hex);

		char32_t hex4ToChar32(const char* hex);

		template<typename ValueType> void readEscapedUnicode(ValueType& value, auto&& it);

		bool isThreeByteSequence(uint8_t byte);

		bool isFourByteSequence(uint8_t byte);

		bool isTwoByteSequence(uint8_t byte);

		void writeCharacters(const char* data, uint64_t length);

		void writeCharactersFromBuffer(uint32_t length);

		void writeCharacter(const char value);

		void singleValueETFToJson();

		void parseSmallIntegerExt();

		void parseSmallAtomExt();

		void parseNewFloatExt();

		void parseSmallBigExt();

		void parseIntegerExt();

		void parseStringExt();

		void parseBinaryExt();

		void parseListExt();

		void parseAtomExt();

		void parseNilExt();

		void parseMapExt();
	};

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

	struct DiscordCoreAPI_Dll EtfSerializeError : public DiscordCoreAPI::DCAException {
	  public:
		explicit EtfSerializeError(const std::string& message, std::source_location = std::source_location::current());
	};

	enum class JsonType : uint8_t { Null = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7 };

	class EtfSerializer;

	template<typename ValueType>
	concept IsEtfSerializer = std::same_as<ValueType, EtfSerializeError>;

	template<typename ValueType>
	concept IsEnumT = std::is_enum<std::decay_t<ValueType>>::value && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept IsBoolT = std::same_as<std::decay_t<ValueType>, bool> && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept IsFloatingPointT = std::floating_point<std::decay_t<ValueType>> && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept IsSignedT = std::signed_integral<std::decay_t<ValueType>> && !IsBoolT<std::decay_t<ValueType>> && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept IsUnsignedT =
		std::unsigned_integral<std::decay_t<ValueType>> && !IsBoolT<std::decay_t<ValueType>> && !IsSignedT<ValueType> && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept IsStringT = std::same_as<std::decay_t<ValueType>, std::string> ||
		std::convertible_to<ValueType, std::string> && !std::same_as<std::decay_t<ValueType>, char> && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept HasEmplaceBack = requires(ValueType data) {
		{ data.emplace_back(std::declval<typename ValueType::value_type>) } -> std::same_as<typename ValueType::value_type&>;
	};

	template<typename ValueType>
	concept HasRange = requires(ValueType data) {
		{ data.begin() } -> std::same_as<typename ValueType::iterator>;
		{ data.end() } -> std::same_as<typename ValueType::iterator>;
	};

	template<typename ValueType>
	concept HasResize = requires(ValueType data) {
		{ data.resize(size_t{}) };
	};

	template<typename ValueType>
	concept IsNullT = std::same_as<std::decay_t<ValueType>, std::nullptr_t> && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept VectorSubscriptable = requires(ValueType data) {
		{ data[size_t{}] } -> std::same_as<typename ValueType::value_type&>;
	};

	template<typename ValueType>
	concept IsArrayT = HasRange<ValueType> && HasResize<std::decay_t<ValueType>> && HasEmplaceBack<std::decay_t<ValueType>> &&
		VectorSubscriptable<std::decay_t<ValueType>> && requires(ValueType data) { typename ValueType::value_type; } && !IsEtfSerializer<ValueType>;

	template<typename ValueType>
	concept IsObjectT = requires(ValueType data) {
		typename ValueType::mapped_type;
		typename ValueType::key_type;
	} && HasRange<ValueType> && !IsEtfSerializer<ValueType>;

	class DiscordCoreAPI_Dll EtfSerializer {
	  public:
		using MapAllocatorType = std::allocator<std::pair<const std::string, EtfSerializer>>;
		template<typename ValueType> using allocator = std::allocator<std::decay_t<ValueType>>;
		template<typename ValueType> using AllocatorType = std::allocator_traits<allocator<ValueType>>;
		using ObjectType = std::map<std::string, EtfSerializer, std::less<>, MapAllocatorType>;
		using ArrayType = std::vector<EtfSerializer, allocator<EtfSerializer>>;
		using StringType = std::string;
		using FloatType = double;
		using UintType = uint64_t;
		using IntType = int64_t;
		using BoolType = bool;

		EtfSerializer() noexcept = default;

#ifdef _WIN32
		template<typename ValueType> EtfSerializer& operator=(ValueType&&) noexcept;
		template<typename ValueType> EtfSerializer(ValueType&&) noexcept;
		template<typename ValueType> EtfSerializer& operator=(const ValueType&) noexcept;
		template<typename ValueType> EtfSerializer(const ValueType&) noexcept;
#endif

		EtfSerializer& operator=(EtfSerializer&& data) noexcept {
			type = data.type;
			value = data.value;
			data.value = nullptr;
			stringReal = std::move(data.stringReal);
			return *this;
		}

		EtfSerializer(EtfSerializer&& data) noexcept {
			*this = std::move(data);
		}

		EtfSerializer& operator=(const EtfSerializer& data) noexcept {
			switch (data.type) {
				case JsonType::Object: {
					setValue(JsonType::Object);
					getObject() = data.getObject();
					break;
				}
				case JsonType::Array: {
					setValue(JsonType::Array);
					getArray() = data.getArray();
					break;
				}
				case JsonType::String: {
					setValue(JsonType::String);
					getString() = data.getString();
					break;
				}
				case JsonType::Float: {
					setValue(JsonType::Float);
					getFloat() = data.getFloat();
					break;
				}
				case JsonType::Uint64: {
					setValue(JsonType::Uint64);
					getUint() = data.getUint();
					break;
				}
				case JsonType::Int64: {
					setValue(JsonType::Int64);
					getInt() = data.getInt();
					break;
				}
				case JsonType::Bool: {
					setValue(JsonType::Bool);
					getBool() = data.getBool();
					break;
				}
				case JsonType::Null: {
					break;
				}
			}
			stringReal = data.stringReal;
			type = data.type;
			return *this;
		}

		EtfSerializer(const EtfSerializer& data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(EtfSerializer& data) noexcept {
			switch (data.type) {
				case JsonType::Object: {
					setValue(JsonType::Object);
					getObject() = data.getObject();
					break;
				}
				case JsonType::Array: {
					setValue(JsonType::Array);
					getArray() = data.getArray();
					break;
				}
				case JsonType::String: {
					setValue(JsonType::String);
					getString() = data.getString();
					break;
				}
				case JsonType::Float: {
					setValue(JsonType::Float);
					getFloat() = data.getFloat();
					break;
				}
				case JsonType::Uint64: {
					setValue(JsonType::Uint64);
					getUint() = data.getUint();
					break;
				}
				case JsonType::Int64: {
					setValue(JsonType::Int64);
					getInt() = data.getInt();
					break;
				}
				case JsonType::Bool: {
					setValue(JsonType::Bool);
					getBool() = data.getBool();
					break;
				}
				case JsonType::Null: {
					break;
				}
			}
			stringReal = data.stringReal;
			type = data.type;
			return *this;
		}

		EtfSerializer(EtfSerializer& data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsObjectT auto&& data) noexcept {
			setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				getObject()[key] = std::move(value);
			}
			return *this;
		}

		EtfSerializer(IsObjectT auto&& data) noexcept {
			*this = std::move(data);
		};

		EtfSerializer& operator=(const IsObjectT auto& data) noexcept {
			setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				getObject()[key] = value;
			}
			return *this;
		}

		EtfSerializer(const IsObjectT auto& data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsArrayT auto&& data) noexcept {
			setValue(JsonType::Array);
			for (auto& value: data) {
				getArray().emplace_back(std::move(value));
			}
			return *this;
		}

		EtfSerializer(IsArrayT auto&& data) noexcept {
			*this = std::move(data);
		}

		EtfSerializer& operator=(const IsArrayT auto& data) noexcept {
			setValue(JsonType::Array);
			for (auto& value: data) {
				getArray().emplace_back(value);
			}
			return *this;
		}

		EtfSerializer(const IsArrayT auto& data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsStringT auto&& data) noexcept {
			setValue(JsonType::String);
			getString() = std::move(data);
			return *this;
		}

		EtfSerializer(IsStringT auto&& data) noexcept {
			*this = std::move(data);
		}

		EtfSerializer& operator=(const IsStringT auto& data) noexcept {
			setValue(JsonType::String);
			getString() = data;
			return *this;
		}

		EtfSerializer(const IsStringT auto& data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsFloatingPointT auto data) noexcept {
			setValue(JsonType::Float);
			getFloat() = data;
			return *this;
		}

		EtfSerializer(IsFloatingPointT auto data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsUnsignedT auto data) noexcept {
			setValue(JsonType::Uint64);
			getUint() = data;
			return *this;
		}

		EtfSerializer(IsUnsignedT auto data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsSignedT auto data) noexcept {
			setValue(JsonType::Int64);
			getUint() = data;
			return *this;
		}

		EtfSerializer(IsSignedT auto data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsEnumT auto data) noexcept {
			setValue(JsonType::Uint64);
			getUint() = static_cast<uint64_t>(data);
			return *this;
		}

		EtfSerializer(IsEnumT auto data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(IsBoolT auto data) noexcept {
			setValue(JsonType::Bool);
			getBool() = data;
			return *this;
		}

		EtfSerializer(IsBoolT auto data) noexcept {
			*this = data;
		}

		EtfSerializer& operator=(JsonType data) noexcept {
			setValue(data);
			return *this;
		}

		EtfSerializer(JsonType data) noexcept {
			*this = data;
		}

		operator std::string&&() noexcept;

		operator std::string() noexcept;

		void refreshString();

		EtfSerializer& operator[](typename ObjectType::key_type key);

		EtfSerializer& operator[](uint64_t index);

		void emplaceBack(EtfSerializer&& data);
		void emplaceBack(EtfSerializer& data);

		JsonType getType() noexcept;

		template<typename ValueType> const ValueType& getValue() const;
		template<typename ValueType> ValueType& getValue();

		~EtfSerializer() noexcept;

	  protected:
		JsonType type{ JsonType::Null };
		std::string stringReal{};
		void* value{};

		inline ObjectType& getObject() {
			return *static_cast<ObjectType*>(value);
		}

		inline ArrayType& getArray() {
			return *static_cast<ArrayType*>(value);
		}

		inline StringType& getString() {
			return *static_cast<StringType*>(value);
		}

		inline FloatType& getFloat() {
			return *static_cast<FloatType*>(value);
		}

		inline UintType& getUint() {
			return *static_cast<UintType*>(value);
		}

		inline IntType& getInt() {
			return *static_cast<IntType*>(value);
		}

		inline BoolType& getBool() {
			return *static_cast<BoolType*>(value);
		}

		inline const ObjectType& getObject() const {
			return *static_cast<ObjectType*>(value);
		}

		inline const ArrayType& getArray() const {
			return *static_cast<ArrayType*>(value);
		}

		inline const StringType& getString() const {
			return *static_cast<StringType*>(value);
		}

		inline const FloatType& getFloat() const {
			return *static_cast<FloatType*>(value);
		}

		inline const UintType& getUint() const {
			return *static_cast<UintType*>(value);
		}

		inline const IntType& getInt() const {
			return *static_cast<IntType*>(value);
		}

		inline const BoolType& getBool() const {
			return *static_cast<BoolType*>(value);
		}

		void serializeJsonToEtfString(const EtfSerializer* dataToParse);

		void writeEtfObject(const ObjectType& jsonData);

		void writeEtfArray(const ArrayType& jsonData);

		void writeEtfString(const StringType& jsonData);

		void writeEtfUint(const UintType jsonData);

		void writeEtfInt(const IntType jsonData);

		void writeEtfFloat(const FloatType jsonData);

		void writeEtfBool(const BoolType jsonData);

		void writeEtfNull();

		void writeString(const char* data, uint64_t length);

		void writeCharacter(const char Char);

		void appendBinaryExt(const std::string& bytes, const uint32_t sizeNew);

		void appendNewFloatExt(const double FloatValue);

		void appendListHeader(const uint32_t sizeNew);

		void appendMapHeader(const uint32_t sizeNew);

		void appendUint64(const uint64_t value);

		void appendInt64(const int64_t value);

		void appendUint32(const uint32_t value);

		void appendInt32(const int32_t value);

		void appendUint8(const uint8_t value);

		void appendInt8(const int8_t value);

		void appendBool(bool data);

		void appendVersion();

		void appendNilExt();

		void appendNil();

		void setValue(JsonType TypeNew);

		void destroy() noexcept;

		friend bool operator==(const EtfSerializer& lhs, const EtfSerializer& rhs);
	};

	template<> inline const EtfSerializer::ObjectType& EtfSerializer::getValue() const {
		if (type != JsonType::Object) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
		}
		return *static_cast<ObjectType*>(value);
	}

	template<> inline const EtfSerializer::ArrayType& EtfSerializer::getValue() const {
		if (type != JsonType::Array) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
		}
		return *static_cast<ArrayType*>(value);
	}

	template<> inline const EtfSerializer::StringType& EtfSerializer::getValue() const {
		if (type != JsonType::String) {
			throw EtfSerializeError{ "Sorry, but this value's type is not String." };
		}
		return *static_cast<StringType*>(value);
	}

	template<> inline const EtfSerializer::FloatType& EtfSerializer::getValue() const {
		if (type != JsonType::Float) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Float." };
		}
		return *static_cast<FloatType*>(value);
	}

	template<> inline const EtfSerializer::UintType& EtfSerializer::getValue() const {
		if (type != JsonType::Uint64) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Uint." };
		}
		return *static_cast<UintType*>(value);
	}

	template<> inline const EtfSerializer::IntType& EtfSerializer::getValue() const {
		if (type != JsonType::Int64) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Int." };
		}
		return *static_cast<IntType*>(value);
	}

	template<> inline const EtfSerializer::BoolType& EtfSerializer::getValue() const {
		if (type != JsonType::Bool) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Bool." };
		}
		return *static_cast<BoolType*>(value);
	}

	template<> inline EtfSerializer::ObjectType& EtfSerializer::getValue() {
		if (type != JsonType::Object) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
		}
		return *static_cast<ObjectType*>(value);
	}

	template<> inline EtfSerializer::ArrayType& EtfSerializer::getValue() {
		if (type != JsonType::Array) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
		}
		return *static_cast<ArrayType*>(value);
	}

	template<> inline EtfSerializer::StringType& EtfSerializer::getValue() {
		if (type != JsonType::String) {
			throw EtfSerializeError{ "Sorry, but this value's type is not String." };
		}
		return *static_cast<StringType*>(value);
	}

	template<> inline EtfSerializer::FloatType& EtfSerializer::getValue() {
		if (type != JsonType::Float) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Float." };
		}
		return *static_cast<FloatType*>(value);
	}

	template<> inline EtfSerializer::UintType& EtfSerializer::getValue() {
		if (type != JsonType::Uint64) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Uint." };
		}
		return *static_cast<UintType*>(value);
	}

	template<> inline EtfSerializer::IntType& EtfSerializer::getValue() {
		if (type != JsonType::Int64) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Int." };
		}
		return *static_cast<IntType*>(value);
	}

	template<> inline EtfSerializer::BoolType& EtfSerializer::getValue() {
		if (type != JsonType::Bool) {
			throw EtfSerializeError{ "Sorry, but this value's type is not Bool." };
		}
		return *static_cast<BoolType*>(value);
	}

}// namespace DiscordCoreInternal
