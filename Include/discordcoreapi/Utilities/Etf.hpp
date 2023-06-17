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

#include <discordcoreapi/Utilities/LightString.hpp>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll DCAException : public std::runtime_error {
		DCAException(const std::string& error, std::source_location location = std::source_location::current()) noexcept;
	};

}

namespace DiscordCoreInternal {

	template<typename RTy> inline void reverseByteOrder(RTy& net) {
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

	template<typename ReturnType> inline void storeBits(char* to, ReturnType num) {
		const uint8_t byteSize{ 8 };
		reverseByteOrder<ReturnType>(num);
		for (int32_t x = 0; x < sizeof(ReturnType); ++x) {
			to[x] = static_cast<uint8_t>(num >> (byteSize * x));
		}
	}
	
	struct DiscordCoreAPI_Dll EtfParseError : public DiscordCoreAPI::DCAException {
	  public:
		inline explicit EtfParseError(const std::string& message) : DCAException(message){};
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

	constexpr uint8_t formatVersion{ 131 };

	class DiscordCoreAPI_Dll EtfParser {
	  public:
		friend class WebSocketClient;

		inline std::string_view parseEtfToJson(std::string_view dataToParse) {
			dataBuffer = dataToParse.data();
			dataSize = dataToParse.size();
			finalString.clear();
			currentSize = 0;
			offSet = 0;
			if (readBitsFromBuffer<uint8_t>() != formatVersion) {
				throw EtfParseError{ "EtfParser::parseEtfToJson() Error: Incorrect format version specified." };
			}
			singleValueETFToJson();
			return std::string_view{ finalString.data(), currentSize };
		}

	  protected:
		DiscordCoreAPI::String finalString{};
		const char* dataBuffer{};
		uint64_t currentSize{};
		uint64_t dataSize{};
		uint64_t offSet{};

		template<typename RTy> inline RTy readBitsFromBuffer() {
			if (offSet + sizeof(RTy) > dataSize) {
				throw EtfParseError{ "EtfParser::readBitsFromBuffer() Error: readBitsFromBuffer() past end of the buffer." };
			}
			RTy newValue = *reinterpret_cast<const RTy*>(dataBuffer + offSet);
			offSet += sizeof(RTy);
			reverseByteOrder<RTy>(newValue);
			return newValue;
		}

		inline void writeCharacters(const char* data, uint64_t length) {
			if (finalString.size() < currentSize + length) {
				finalString.resize((finalString.size() + length) * 2);
			}
			std::memcpy(finalString.data() + currentSize, data, length);
			currentSize += length;
		}

		inline uint8_t hex2dec(char hex) {
			return ((hex & 0xf) + (hex >> 6) * 9);
		}

		inline char32_t hex4ToChar32(const char* hex) {
			uint32_t value = hex2dec(hex[3]);
			value |= hex2dec(hex[2]) << 8;
			value |= hex2dec(hex[1]) << 16;
			value |= hex2dec(hex[0]) << 24;
			return value;
		}

		template<typename ValueType> inline void readEscapedUnicode(ValueType& value, auto& it) {
			char8_t buffer[4];
			char32_t codepoint = hex4ToChar32(it);
			auto& facet = std::use_facet<std::codecvt<char32_t, char8_t, mbstate_t>>(std::locale());
			std::mbstate_t mbstate{};
			const char32_t* fromNext;
			char8_t* toNext;
			const auto result = facet.out(mbstate, &codepoint, &codepoint + 1, fromNext, buffer, buffer + 4, toNext);
			if (result != std::codecvt_base::ok) {
				return;
			}

			if ((toNext - buffer) != 1) [[unlikely]] {
				return;
			}
			*value = static_cast<char>(buffer[0]);
			it += 4;
		}

		inline void writeCharactersFromBuffer(uint32_t length) {
			if (!length) {
				writeCharacters("\"\"", 2);
				return;
			}
			if (offSet + static_cast<uint64_t>(length) > dataSize) {
				throw EtfParseError{ "ErlPacker::writeCharactersFromBuffer() Error: Read past end of buffer." };
			}
			if (finalString.size() < currentSize + length) {
				finalString.resize((finalString.size() + length) * 2);
			}
			const char* stringNew = dataBuffer + offSet;
			offSet += length;
			if (length >= 3 && length <= 5) {
				if (length == 3 && strncmp(stringNew, "nil", 3) == 0) {
					writeCharacters("null", 4);
					return;
				} else if (length == 4 && strncmp(stringNew, "null", 4) == 0) {
					writeCharacters("null", 4);
					return;
				} else if (length == 4 && strncmp(stringNew, "true", 4) == 0) {
					writeCharacters("true", 4);
					return;
				} else if (length == 5 && strncmp(stringNew, "false", 5) == 0) {
					writeCharacters("false", 5);
					return;
				}
			}
			auto handleEscaped = [&, this](const char*& iter) {
				switch (*iter) {
					case '"':
					case '\\':
					case '/':
						writeCharacter(*iter);
						++iter;
						return;
					case 'b':
						writeCharacter('\b');
						++iter;
						return;
					case 'f':
						writeCharacter('\f');
						++iter;
						return;
					case 'n':
						writeCharacter('\n');
						++iter;
						return;
					case 'r':
						writeCharacter('\r');
						++iter;
						return;
					case 't':
						writeCharacter('\t');
						++iter;
						return;
					case 'u': {
						auto newVal = finalString.data() + offSet;
						readEscapedUnicode(newVal, iter);
						writeCharacter(*newVal);
						return;
					}
					default: {
						++iter;
						return;
					}
				}
			};
			writeCharacter('"');
			for (auto iter = stringNew; iter != stringNew + length;) {
				bool doWeBreak{};

				switch (*iter) {
					case '\\': {
						switch (*++iter) {
							case '\"':
								writeCharacter('\\');
								writeCharacter('\\');
								writeCharacter('\"');
								break;
							case '\\':
								break;
							case '/':
								writeCharacter('/');
								break;
							case 'b':
								writeCharacter('\b');
								break;
							case 'f':
								writeCharacter('\f');
								break;
							case 'n':
								writeCharacter('\n');
								break;
							case 'r':
								writeCharacter('\r');
								break;
							case 't':
								writeCharacter('\t');
								break;

							default: {
								writeCharacter(*iter);
								break;
							}
						}
						break;
					}
					case '"': {
						writeCharacter('\\');
						writeCharacter('\"');
						break;
					}
					default: {
						writeCharacter(*iter);
						break;
					}
				}
				++iter;
				if (doWeBreak) {
					break;
				}
			}
			writeCharacter('"');
		}

		inline void writeCharacter(const char value) {
			if (finalString.size() < currentSize + 1) {
				finalString.resize((finalString.size() + 1) * 2);
			}
			finalString[currentSize++] = value;
		}

		inline void singleValueETFToJson() {
			if (offSet > dataSize) {
				throw EtfParseError{ "ErlPacker::singleValueETFToJson() Error: Read past end of buffer." };
			}
			uint8_t type = readBitsFromBuffer<uint8_t>();
			switch (static_cast<EtfType>(type)) {
				case EtfType::New_Float_Ext: {
					return parseNewFloatExt();
				}
				case EtfType::Small_Integer_Ext: {
					return parseSmallIntegerExt();
				}
				case EtfType::Integer_Ext: {
					return parseIntegerExt();
				}
				case EtfType::Atom_Ext: {
					return parseAtomExt();
				}
				case EtfType::Nil_Ext: {
					return parseNilExt();
				}
				case EtfType::String_Ext: {
					return parseStringExt();
				}
				case EtfType::List_Ext: {
					return parseListExt();
				}
				case EtfType::Binary_Ext: {
					return parseBinaryExt();
				}
				case EtfType::Small_Big_Ext: {
					return parseSmallBigExt();
				}
				case EtfType::Small_Atom_Ext: {
					return parseSmallAtomExt();
				}
				case EtfType::Map_Ext: {
					return parseMapExt();
				}
				default: {
					throw EtfParseError{ "EtfParser::singleValueETFToJson() Error: Unknown data type in ETF, the type: " + std::to_string(type) };
				}
			}
		}

		inline void parseListExt() {
			uint32_t length = readBitsFromBuffer<uint32_t>();
			writeCharacter('[');
			if (static_cast<uint64_t>(offSet) + length > dataSize) {
				throw EtfParseError{ "ErlPacker::parseListExt() Error: Read past end of buffer." };
			}
			for (uint16_t x = 0; x < length; ++x) {
				singleValueETFToJson();
				if (x < length - 1) {
					writeCharacter(',');
				}
			}
			readBitsFromBuffer<uint8_t>();
			writeCharacter(']');
		}

		inline void parseSmallIntegerExt() {
			auto string = std::to_string(readBitsFromBuffer<uint8_t>());
			writeCharacters(string.data(), string.size());
		}

		inline void parseIntegerExt() {
			auto string = std::to_string(readBitsFromBuffer<uint32_t>());
			writeCharacters(string.data(), string.size());
		}

		inline void parseStringExt() {
			writeCharacter('"');
			uint16_t length = readBitsFromBuffer<uint16_t>();
			if (static_cast<uint64_t>(offSet) + length > dataSize) {
				throw EtfParseError{ "ErlPacker::parseStringExt() Error: Read past end of buffer." };
			}
			for (uint16_t x = 0; x < length; ++x) {
				parseSmallIntegerExt();
			}
			writeCharacter('"');
		}

		inline void parseNewFloatExt() {
			uint64_t value = readBitsFromBuffer<uint64_t>();
			auto newDouble = *reinterpret_cast<double*>(&value);
			std::string valueNew = std::to_string(newDouble);
			writeCharacters(valueNew.data(), valueNew.size());
		}

		inline void parseSmallBigExt() {
			writeCharacter('"');
			auto digits = readBitsFromBuffer<uint8_t>();
			uint8_t sign = readBitsFromBuffer<uint8_t>();


			if (digits > 8) {
				throw EtfParseError{ "EtfParser::parseSmallBigExt() Error: Big integers larger than 8 bytes not supported." };
			}

			uint64_t value = 0;
			uint64_t bits = 1;
			for (uint8_t i = 0; i < digits; ++i) {
				uint64_t digit = readBitsFromBuffer<uint8_t>();
				value += digit * bits;
				bits <<= 8;
			}

			if (sign == 0) {
				auto string = std::to_string(value);
				writeCharacters(string.data(), string.size());
				writeCharacter('"');
			} else {
				auto string = std::to_string(-(static_cast<int64_t>(value)));
				writeCharacters(string.data(), string.size());
				writeCharacter('"');
			}
		}

		inline void parseAtomExt() {
			writeCharactersFromBuffer(readBitsFromBuffer<uint16_t>());
		}

		inline void parseBinaryExt() {
			writeCharactersFromBuffer(readBitsFromBuffer<uint32_t>());
		}

		inline void parseNilExt() {
			writeCharacters("[]", 2);
		}

		inline void parseSmallAtomExt() {
			writeCharactersFromBuffer(readBitsFromBuffer<uint8_t>());
		}

		inline void parseMapExt() {
			uint32_t length = readBitsFromBuffer<uint32_t>();
			writeCharacter('{');
			for (uint32_t x = 0; x < length; ++x) {
				singleValueETFToJson();
				writeCharacter(':');
				singleValueETFToJson();
				if (x < length - 1) {
					writeCharacter(',');
				}
			}
			writeCharacter('}');
		}
	};

	struct DiscordCoreAPI_Dll EtfSerializeError : public DiscordCoreAPI::DCAException {
	  public:
		inline EtfSerializeError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException(message, location){};
	};

	enum class JsonType : uint8_t { Null = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint = 5, Int = 6, Bool = 7 };

	class EtfSerializer;

	template<typename ValueType>
	concept EtfSerializerT = std::same_as<ValueType, EtfSerializeError>;

	template<typename ValueType>
	concept EnumT = std::is_enum<std::decay_t<ValueType>>::value && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept BoolT = std::same_as<std::decay_t<ValueType>, bool> && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept FloatT = std::floating_point<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept SignedT = std::signed_integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept UnsignedT =
		std::unsigned_integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !SignedT<ValueType> && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept StringT = std::same_as<std::decay_t<ValueType>, std::string> ||
		std::convertible_to<ValueType, std::string> && !std::same_as<std::decay_t<ValueType>, char> && !EtfSerializerT<ValueType>;

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
	concept NullT = std::same_as<std::decay_t<ValueType>, std::nullptr_t> && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept VectorSubscriptable = requires(ValueType data) {
		{ data[size_t{}] } -> std::same_as<typename ValueType::value_type&>;
	};

	template<typename ValueType>
	concept ArrayT = HasRange<ValueType> && HasResize<std::decay_t<ValueType>> && HasEmplaceBack<std::decay_t<ValueType>> &&
		VectorSubscriptable<std::decay_t<ValueType>> && requires(ValueType data) { typename ValueType::value_type; } && !EtfSerializerT<ValueType>;

	template<typename ValueType>
	concept ObjectT = requires(ValueType data) {
		typename ValueType::mapped_type;
		typename ValueType::key_type;
	} && HasRange<ValueType> && !EtfSerializerT<ValueType>;

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

		inline EtfSerializer() noexcept = default;

#ifdef _WIN32
		template<typename ValueType> inline EtfSerializer& operator=(ValueType&&) noexcept;
		template<typename ValueType> inline EtfSerializer(ValueType&&) noexcept;
		template<typename ValueType> inline EtfSerializer& operator=(const ValueType&) noexcept;
		template<typename ValueType> inline EtfSerializer(const ValueType&) noexcept;
#endif

		inline EtfSerializer& operator=(EtfSerializer&& data) noexcept {
			type = data.type;
			value = data.value;
			data.value = nullptr;
			stringReal = std::move(data.stringReal);
			return *this;
		}

		inline EtfSerializer(EtfSerializer&& data) noexcept {
			*this = std::move(data);
		}

		inline EtfSerializer& operator=(const EtfSerializer& data) noexcept {
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
				case JsonType::Uint: {
					setValue(JsonType::Uint);
					getUint() = data.getUint();
					break;
				}
				case JsonType::Int: {
					setValue(JsonType::Int);
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

		inline EtfSerializer(const EtfSerializer& data) noexcept {
			*this = data;
		}

		inline EtfSerializer& operator=(EtfSerializer& data) noexcept {
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
				case JsonType::Uint: {
					setValue(JsonType::Uint);
					getUint() = data.getUint();
					break;
				}
				case JsonType::Int: {
					setValue(JsonType::Int);
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

		inline EtfSerializer(EtfSerializer& data) noexcept {
			*this = data;
		}

		template<ObjectT ValueType> inline EtfSerializer& operator=(ValueType&& data) noexcept {
			setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				getObject()[key] = std::move(value);
			}
			return *this;
		}

		template<ObjectT ValueType> inline EtfSerializer(ValueType&& data) noexcept {
			*this = std::move(data);
		};

		template<ObjectT ValueType> inline EtfSerializer& operator=(const ValueType& data) noexcept {
			setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				getObject()[key] = value;
			}
			return *this;
		}

		template<ObjectT ValueType> inline EtfSerializer(const ValueType& data) noexcept {
			*this = data;
		}

		template<ArrayT ValueType> inline EtfSerializer& operator=(ValueType&& data) noexcept {
			setValue(JsonType::Array);
			for (auto& value: data) {
				getArray().emplace_back(std::move(value));
			}
			return *this;
		}

		template<ArrayT ValueType> inline EtfSerializer(ValueType&& data) noexcept {
			*this = std::move(data);
		}

		template<ArrayT ValueType> inline EtfSerializer& operator=(const ValueType& data) noexcept {
			setValue(JsonType::Array);
			for (auto& value: data) {
				getArray().emplace_back(value);
			}
			return *this;
		}

		template<ArrayT ValueType> inline EtfSerializer(const ValueType& data) noexcept {
			*this = data;
		}

		template<StringT ValueType> inline EtfSerializer& operator=(ValueType&& data) noexcept {
			setValue(JsonType::String);
			getString() = std::move(data);
			return *this;
		}

		template<StringT ValueType> inline EtfSerializer(ValueType&& data) noexcept {
			*this = std::move(data);
		}

		template<StringT ValueType> inline EtfSerializer& operator=(const ValueType& data) noexcept {
			setValue(JsonType::String);
			getString() = data;
			return *this;
		}

		template<StringT ValueType> inline EtfSerializer(const ValueType& data) noexcept {
			*this = data;
		}	

		template<size_t StrLength> inline EtfSerializer& operator=(const char (&str)[StrLength]) {
			setValue(JsonType::String);
			this->getString().resize(StrLength);
			std::memcpy(this->getString().data(), str, StrLength);
			return *this;
		}

		template<size_t StrLength> inline EtfSerializer(const char (&str)[StrLength]) {
			*this = str;
		}

		template<FloatT ValueType> inline EtfSerializer& operator=(ValueType data) noexcept {
			setValue(JsonType::Float);
			getFloat() = data;
			return *this;
		}

		template<FloatT ValueType> inline EtfSerializer(ValueType data) noexcept {
			*this = data;
		}

		template<SignedT ValueType> inline EtfSerializer& operator=(ValueType data) noexcept {
			setValue(JsonType::Int);
			getInt() = data;
			return *this;
		}

		template<SignedT ValueType> inline EtfSerializer(ValueType data) noexcept {
			*this = data;
		}

		template<UnsignedT ValueType> inline EtfSerializer& operator=(ValueType data) noexcept {
			setValue(JsonType::Uint);
			getUint() = static_cast<uint64_t>(data);
			return *this;
		}

		template<UnsignedT ValueType> inline EtfSerializer(ValueType data) noexcept {
			*this = data;
		}

		template<BoolT ValueType> inline EtfSerializer& operator=(ValueType data) noexcept {
			setValue(JsonType::Bool);
			getBool() = data;
			return *this;
		}

		template<BoolT ValueType> inline EtfSerializer(ValueType data) noexcept {
			*this = data;
		}

		template<EnumT ValueType> inline EtfSerializer& operator=(ValueType data) noexcept {
			setValue(JsonType::Int);
			getInt() = static_cast<int64_t>(data);
			return *this;
		}

		template<EnumT ValueType> inline EtfSerializer(ValueType data) noexcept {
			*this = data;
		}

		inline EtfSerializer& operator=(JsonType data) noexcept {
			setValue(data);
			return *this;
		} 

		inline EtfSerializer(JsonType data) noexcept {
			*this = data;
		}

		inline operator std::string&&() noexcept {
			return std::move(stringReal);
		}

		inline operator std::string() noexcept {
			return stringReal;
		}

		inline JsonType getType() noexcept {
			return type;
		}

		inline void refreshString() {
			stringReal.clear();
			appendVersion();
			serializeJsonToEtfString(this);
		}

		inline EtfSerializer& operator[](typename ObjectType::key_type key) {
			if (type == JsonType::Null) {
				setValue(JsonType::Object);
				type = JsonType::Object;
			}

			if (type == JsonType::Object) {
				auto result = getObject().emplace(std::move(key), EtfSerializer{});
				return result.first->second;
			}
			throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
		}

		inline EtfSerializer& operator[](uint64_t index) {
			if (type == JsonType::Null) {
				setValue(JsonType::Array);
				type = JsonType::Array;
			}

			if (type == JsonType::Array) {
				if (index >= getArray().size()) {
					getArray().resize(index + 1);
				}

				return getArray().operator[](index);
			}
			throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
		}

		inline void emplaceBack(EtfSerializer&& other) {
			if (type == JsonType::Null) {
				setValue(JsonType::Array);
				type = JsonType::Array;
			}

			if (type == JsonType::Array) {
				getArray().emplace_back(std::move(other));
				return;
			}
			throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
		}

		inline void emplaceBack(EtfSerializer& other) {
			if (type == JsonType::Null) {
				setValue(JsonType::Array);
				type = JsonType::Array;
			}

			if (type == JsonType::Array) {
				getArray().emplace_back(other);
				return;
			}
			throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
		}		

		inline ~EtfSerializer() noexcept {
			destroy();
		}

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

		inline void serializeJsonToEtfString(const EtfSerializer* dataToParse) {
			switch (dataToParse->type) {
				case JsonType::Object: {
					return writeEtfObject(dataToParse->getObject());
				}
				case JsonType::Array: {
					return writeEtfArray(dataToParse->getArray());
				}
				case JsonType::String: {
					return writeEtfString(dataToParse->getString());
				}
				case JsonType::Float: {
					return writeEtfFloat(dataToParse->getFloat());
				}
				case JsonType::Uint: {
					return writeEtfUint(dataToParse->getUint());
				}
				case JsonType::Int: {
					return writeEtfInt(dataToParse->getInt());
				}
				case JsonType::Bool: {
					return writeEtfBool(dataToParse->getBool());
				}
				case JsonType::Null: {
					return writeEtfNull();
				}
			}
		}

		inline void writeEtfObject(const ObjectType& jsonData) {
			appendMapHeader(static_cast<uint32_t>(jsonData.size()));
			for (auto& [key, value]: jsonData) {
				appendBinaryExt(key, static_cast<uint32_t>(key.size()));
				serializeJsonToEtfString(&value);
			}
		}

		inline void writeEtfArray(const ArrayType& jsonData) {
			appendListHeader(static_cast<uint32_t>(jsonData.size()));
			for (auto& value: jsonData) {
				serializeJsonToEtfString(&value);
			}
			appendNilExt();
		}

		inline void writeEtfString(const StringType& jsonData) {
			appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
		}

		inline void writeEtfUint(const UintType jsonData) {
			if (jsonData <= std::numeric_limits<uint8_t>::max() && jsonData >= std::numeric_limits<uint8_t>::min()) {
				appendUint8(static_cast<uint8_t>(jsonData));
			} else if (jsonData <= std::numeric_limits<uint32_t>::max() && jsonData >= std::numeric_limits<uint32_t>::min()) {
				appendUint32(static_cast<uint32_t>(jsonData));
			} else {
				appendUint64(jsonData);
			}
		}

		inline void writeEtfInt(const IntType jsonData) {
			if (jsonData <= std::numeric_limits<int8_t>::max() && jsonData >= std::numeric_limits<int8_t>::min()) {
				appendInt8(static_cast<int8_t>(jsonData));
			} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
				appendInt32(static_cast<int32_t>(jsonData));
			} else {
				appendInt64(jsonData);
			}
		}

		inline void writeEtfFloat(const FloatType jsonData) {
			appendNewFloatExt(jsonData);
		}

		inline void writeEtfBool(const BoolType jsonData) {
			appendBool(jsonData);
		}

		inline void writeEtfNull() {
			appendNil();
		}

		inline void writeString(const char* data, uint64_t length) {
			stringReal.append(data, length);
		}

		inline void writeCharacter(const char charValue) {
			stringReal.push_back(charValue);
		}

		inline friend bool operator==(const EtfSerializer& lhs, const EtfSerializer& rhs) {
			if (lhs.type != rhs.type) {
				return false;
			}
			switch (rhs.type) {
				case JsonType::Object: {
					if (lhs.getObject() != rhs.getObject()) {
						return false;
					}
					break;
				}
				case JsonType::Array: {
					if (lhs.getArray() != rhs.getArray()) {
						return false;
					}
					break;
				}
				case JsonType::String: {
					if (lhs.getString() != rhs.getString()) {
						return false;
					}
					break;
				}
				case JsonType::Float: {
					if (lhs.getFloat() != rhs.getFloat()) {
						return false;
					}
					break;
				}
				case JsonType::Uint: {
					if (lhs.getUint() != rhs.getUint()) {
						return false;
					}
					break;
				}
				case JsonType::Int: {
					if (lhs.getInt() != rhs.getInt()) {
						return false;
					}
					break;
				}
				case JsonType::Bool: {
					if (lhs.getBool() != rhs.getBool()) {
						return false;
					}
					break;
				}
				case JsonType::Null: {
					break;
				}
			}
			return true;
		}

		inline void appendBinaryExt(const std::string& bytes, uint32_t sizeNew) {
			char newBuffer[5]{ static_cast<int8_t>(EtfType::Binary_Ext) };
			storeBits(newBuffer + 1, sizeNew);
			writeString(newBuffer, std::size(newBuffer));
			writeString(bytes.data(), bytes.size());
		}

		inline void appendNewFloatExt(const double FloatValue) {
			char newBuffer[9]{ static_cast<uint8_t>(EtfType::New_Float_Ext) };
			const void* punner{ &FloatValue };
			storeBits(newBuffer + 1, *static_cast<const uint64_t*>(punner));
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendListHeader(const uint32_t sizeNew) {
			char newBuffer[5]{ static_cast<uint8_t>(EtfType::List_Ext) };
			storeBits(newBuffer + 1, sizeNew);
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendMapHeader(const uint32_t sizeNew) {
			char newBuffer[5]{ static_cast<uint8_t>(EtfType::Map_Ext) };
			storeBits(newBuffer + 1, sizeNew);
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendUint64(uint64_t value) {
			char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
			char encodedBytes{};
			while (value > 0) {
				newBuffer[3 + encodedBytes] = value & 0xFF;
				value >>= 8;
				++encodedBytes;
			}
			newBuffer[1] = encodedBytes;
			newBuffer[2] = 0;
			writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
		}

		inline void appendInt64(int64_t value) {
			char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
			char encodedBytes{};
			while (value > 0) {
				newBuffer[3 + encodedBytes] = value & 0xFF;
				value >>= 8;
				++encodedBytes;
			}
			newBuffer[1] = encodedBytes;
			if (value >= 0) {
				newBuffer[2] = 0;
			} else {
				newBuffer[2] = 1;
			}
			writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
		}

		inline void appendUint32(const uint32_t value) {
			char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
			storeBits(newBuffer + 1, value);
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendInt32(const int32_t value) {
			char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
			storeBits(newBuffer + 1, value);
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendUint8(const uint8_t value) {
			char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendInt8(const int8_t value) {
			char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendBool(bool data) {
			if (data) {
				char newBuffer[6]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
				writeString(newBuffer, std::size(newBuffer));

			} else {
				char newBuffer[7]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
				writeString(newBuffer, std::size(newBuffer));
			}
		}

		inline void appendVersion() {
			char newBuffer[1]{ static_cast<int8_t>(formatVersion) };
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void appendNilExt() {
			writeCharacter(static_cast<uint8_t>(EtfType::Nil_Ext));
		}

		inline void appendNil() {
			char newBuffer[5]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
			writeString(newBuffer, std::size(newBuffer));
		}

		inline void setValue(JsonType typeNew) {
			destroy();
			type = typeNew;
			switch (type) {
				case JsonType::Object: {
					allocator<ObjectType> alloc{};
					AllocatorType<ObjectType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getObject());
					break;
				}
				case JsonType::Array: {
					allocator<ArrayType> alloc{};
					AllocatorType<ArrayType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getArray());
					break;
				}
				case JsonType::String: {
					allocator<StringType> alloc{};
					AllocatorType<StringType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getString());
					break;
				}
				case JsonType::Uint: {
					allocator<UintType> alloc{};
					AllocatorType<UintType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getUint());
					break;
				}
				case JsonType::Int: {
					allocator<IntType> alloc{};
					AllocatorType<IntType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getInt());
					break;
				}
				case JsonType::Bool: {
					allocator<BoolType> alloc{};
					AllocatorType<BoolType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getBool());
					break;
				}
				case JsonType::Float: {
					allocator<FloatType> alloc{};
					AllocatorType<FloatType> allocator{};
					value = allocator.allocate(alloc, 1);
					allocator.construct(alloc, &getFloat());
					break;
				}
				case JsonType::Null: {
					break;
				}
			}
		}

		inline void destroy() noexcept {
			if (value) {
				switch (type) {
					case JsonType::Object: {
						allocator<ObjectType> alloc{};
						AllocatorType<ObjectType> allocator{};
						allocator.destroy(alloc, &getObject());
						allocator.deallocate(alloc, static_cast<ObjectType*>(value), 1);
						break;
					}
					case JsonType::Array: {
						allocator<ArrayType> alloc{};
						AllocatorType<ArrayType> allocator{};
						allocator.destroy(alloc, &getArray());
						allocator.deallocate(alloc, static_cast<ArrayType*>(value), 1);
						break;
					}
					case JsonType::String: {
						allocator<StringType> alloc{};
						AllocatorType<StringType> allocator{};
						allocator.destroy(alloc, &getString());
						allocator.deallocate(alloc, static_cast<StringType*>(value), 1);
						break;
					}
					case JsonType::Uint: {
						allocator<UintType> alloc{};
						AllocatorType<UintType> allocator{};
						allocator.destroy(alloc, &getUint());
						allocator.deallocate(alloc, static_cast<UintType*>(value), 1);
						break;
					}
					case JsonType::Int: {
						allocator<IntType> alloc{};
						AllocatorType<IntType> allocator{};
						allocator.destroy(alloc, &getInt());
						allocator.deallocate(alloc, static_cast<IntType*>(value), 1);
						break;
					}
					case JsonType::Float: {
						allocator<FloatType> alloc{};
						AllocatorType<FloatType> allocator{};
						allocator.destroy(alloc, &getFloat());
						allocator.deallocate(alloc, static_cast<FloatType*>(value), 1);
						break;
					}
					case JsonType::Bool: {
						allocator<BoolType> alloc{};
						AllocatorType<BoolType> allocator{};
						allocator.destroy(alloc, &getBool());
						allocator.deallocate(alloc, static_cast<BoolType*>(value), 1);
						break;
					}
					case JsonType::Null: {
						break;
					}
				}
				type = JsonType::Null;
				value = nullptr;
			}
		}
	};

}// namespace DiscordCoreInternal
