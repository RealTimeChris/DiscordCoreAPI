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
/// Etf.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file Etf.hpp

#pragma once

#include <discordcoreapi/Utilities/UniquePtr.hpp>
#include <discordcoreapi/Utilities/LightString.hpp>

namespace DiscordCoreAPI {

	struct DCAException : public std::runtime_error {
		inline DCAException(const std::string& error, std::source_location location = std::source_location::current())
			: std::runtime_error("Thrown From: " + std::string{ location.file_name() } + std::string{ " (" } + std::to_string(location.line()) + ":" +
				  std::to_string(location.column()) + ")\n" + error){};
	};

	namespace DiscordCoreInternal {

		template<typename ReturnType> inline ReturnType reverseByteOrder(ReturnType& net) {
			if constexpr (std::endian::native == std::endian::little) {
				switch (sizeof(ReturnType)) {
					case 2: {
						net = _mm_extract_epi16(_mm_shuffle_epi8(_mm_insert_epi16(__m128i{}, net, 0), _mm_insert_epi16(__m128i{}, 0x01, 0)), 0);
						return net;
					}
					case 4: {
						net = _mm_extract_epi32(_mm_shuffle_epi8(_mm_insert_epi32(__m128i{}, net, 0), _mm_insert_epi32(__m128i{}, 0x10203, 0)), 0);
						return net;
					}
					case 8: {
						net = _mm_extract_epi64(
							_mm_shuffle_epi8(_mm_insert_epi64(__m128i{}, net, 0), _mm_insert_epi64(__m128i{}, 0x102030405060708, 0)), 0);
						return net;
					}
					default: {
						return net;
					}
				}
			} else {
				return net;
			}
		}

		template<typename ReturnType> inline void storeBits(char* to, ReturnType num) {
			const uint8_t byteSize{ 8 };
			num = reverseByteOrder<ReturnType>(num);
			for (uint64_t x = 0; x < sizeof(ReturnType); ++x) {
				to[x] = static_cast<uint8_t>(num >> (byteSize * x));
			}
		}

		struct EtfParseError : public DCAException {
		  public:
			inline explicit EtfParseError(const std::string& message, std::source_location location = std::source_location::current())
				: DCAException(message, location){};
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
			LightString<char> finalString{};
			const char* dataBuffer{};
			uint64_t currentSize{};
			uint64_t dataSize{};
			uint64_t offSet{};

			template<typename ReturnType> inline ReturnType readBitsFromBuffer() {
				if (offSet + sizeof(ReturnType) > dataSize) {
					throw EtfParseError{ "EtfParser::readBitsFromBuffer() Error: readBitsFromBuffer() past end of the buffer." };
				}
				ReturnType newValue = *reinterpret_cast<const ReturnType*>(dataBuffer + offSet);
				offSet += sizeof(ReturnType);
				newValue = reverseByteOrder<ReturnType>(newValue);
				return newValue;
			}

			inline void writeCharacters(const char* data, uint64_t length) {
				if (finalString.size() < currentSize + length) {
					finalString.resize((finalString.size() + length) * 2);
				}
				std::memcpy(finalString.data() + currentSize, data, length);
				currentSize += length;
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
				writeCharacter('"');
				for (uint64_t x = 0; x < length; ++x) {
					bool doWeBreak{};

					switch (stringNew[x]) {
						case '\\': {
							switch (stringNew[++x]) {
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
									writeCharacter(stringNew[x]);
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
							writeCharacter(stringNew[x]);
							break;
						}
					}
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
				double* newDouble = reinterpret_cast<double*>(&value);
				std::string valueNew = std::to_string(*newDouble);
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

		struct EtfSerializeError : public DCAException {
		  public:
			inline EtfSerializeError(const std::string& message, std::source_location location = std::source_location::current())
				: DCAException(message, location){};
		};

		enum class JsonType : uint8_t { Null = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint = 5, Int = 6, Bool = 7 };

		class DiscordCoreAPI_Dll EtfSerializer;

		template<typename ValueType>
		concept EtfSerializerT = std::same_as<ValueType, EtfSerializeError>;

		template<typename ValueType>
		concept EnumT = std::is_enum<std::decay_t<ValueType>>::value && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept BoolT = std::same_as<std::decay_t<ValueType>, bool> && !EtfSerializerT<ValueType> && !std::integral<ValueType>;

		template<typename ValueType>
		concept FloatT = std::floating_point<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept SignedT = std::signed_integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept IntegerT = std::integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept UnsignedT =
			std::unsigned_integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !SignedT<ValueType> && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept StringT = std::same_as<std::decay_t<ValueType>, std::string> || std::same_as<std::decay_t<ValueType>, std::string_view> ||
			std::convertible_to<ValueType, std::string> && !std::same_as<std::decay_t<ValueType>, char> && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept HasEmplaceBack = requires(ValueType data, typename ValueType::reference& valueNew) {
			{ data.emplace_back(valueNew) } -> std::same_as<typename ValueType::reference>;
		};

		template<typename ValueType>
		concept HasRange = requires(ValueType data) {
			{ data.begin() } -> std::same_as<typename ValueType::iterator>;
			{ data.end() } -> std::same_as<typename ValueType::iterator>;
		};

		template<typename ValueType>
		concept HasResize = requires(ValueType data, uint64_t valueNew) {
			{ data.resize(valueNew) };
		};

		template<typename ValueType>
		concept NullT = std::same_as<std::decay_t<ValueType>, std::nullptr_t> && !EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept VectorSubscriptable = requires(ValueType data, uint64_t valueNew) {
			{ data[valueNew] } -> std::same_as<typename ValueType::reference>;
		};

		template<typename ValueType>
		concept ArrayT = HasRange<ValueType> && HasResize<std::decay_t<ValueType>> && HasEmplaceBack<std::decay_t<ValueType>> &&
			VectorSubscriptable<std::decay_t<ValueType>> && requires(ValueType data) { typename ValueType::value_type; } &&
			!EtfSerializerT<ValueType>;

		template<typename ValueType>
		concept ObjectT = requires(ValueType data) {
			typename ValueType::mapped_type;
			typename ValueType::key_type;
		} && HasRange<ValueType> && !EtfSerializerT<ValueType>;

		class DiscordCoreAPI_Dll EtfSerializer {
		  public:
			template<typename ValueType> using allocator = std::allocator<ValueType>;
			using map_allocator = allocator<std::pair<const std::string, EtfSerializer>>;
			using object_type = std::map<std::string, EtfSerializer, std::less<>, map_allocator>;
			using array_type = std::vector<EtfSerializer, allocator<EtfSerializer>>;
			using string_type = std::string;
			using float_type = double;
			using uint_type = uint64_t;
			using int_type = int64_t;
			using bool_type = bool;

			inline EtfSerializer() = default;

			inline EtfSerializer& operator=(EtfSerializer&& data) noexcept {
				std::swap(stringReal, data.stringReal);
				std::swap(value, data.value);
				std::swap(type, data.type);
				return *this;
			}

			inline EtfSerializer(EtfSerializer&& data) noexcept {
				*this = std::move(data);
			}

			inline EtfSerializer& operator=(const EtfSerializer& data) {
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
				return *this;
			}

			inline EtfSerializer(const EtfSerializer& data) {
				*this = data;
			}

			template<ObjectT ValueType> inline EtfSerializer& operator=(ValueType&& data) {
				setValue(JsonType::Object);
				for (auto& [key, value]: data) {
					getObject().at(key) = std::move(value);
				}
				return *this;
			}

			template<ObjectT ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::move(data);
			};

			template<ObjectT ValueType> inline EtfSerializer& operator=(const ValueType& data) {
				setValue(JsonType::Object);
				for (auto& [key, value]: data) {
					getObject().at(key) = value;
				}
				return *this;
			}

			template<ObjectT ValueType> inline EtfSerializer(const ValueType& data) {
				*this = data;
			}

			template<ArrayT ValueType> inline EtfSerializer& operator=(ValueType&& data) {
				setValue(JsonType::Array);
				for (auto& value: data) {
					getArray().emplace_back(std::move(value));
				}
				return *this;
			}

			template<ArrayT ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::move(data);
			}

			template<ArrayT ValueType> inline EtfSerializer& operator=(const ValueType& data) {
				setValue(JsonType::Array);
				for (auto& value: data) {
					getArray().emplace_back(value);
				}
				return *this;
			}

			template<ArrayT ValueType> inline EtfSerializer(const ValueType& data) {
				*this = data;
			}

			template<StringT ValueType> inline EtfSerializer& operator=(ValueType&& data) {
				setValue(JsonType::String);
				getString() = std::move(data);
				return *this;
			}

			template<StringT ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::move(data);
			}

			template<StringT ValueType> inline EtfSerializer& operator=(const ValueType& data) {
				setValue(JsonType::String);
				getString() = data;
				return *this;
			}

			template<StringT ValueType> inline EtfSerializer(const ValueType& data) {
				*this = data;
			}

			template<uint64_t StrLength> inline EtfSerializer& operator=(const char (&str)[StrLength]) {
				setValue(JsonType::String);
				getString().resize(StrLength);
				std::memcpy(getString().data(), str, StrLength);
				return *this;
			}

			template<uint64_t StrLength> inline EtfSerializer(const char (&str)[StrLength]) {
				*this = str;
			}

			template<FloatT ValueType> inline EtfSerializer& operator=(ValueType data) {
				setValue(JsonType::Float);
				getFloat() = data;
				return *this;
			}

			template<FloatT ValueType> inline EtfSerializer(ValueType data) {
				*this = data;
			}

			template<IntegerT ValueType> inline EtfSerializer& operator=(ValueType data) {
				if constexpr (SignedT<ValueType>) {
					setValue(JsonType::Int);
					getInt() = data;
				} else if constexpr (UnsignedT<ValueType>) {
					setValue(JsonType::Uint);
					getUint() = data;
				}
				return *this;
			}

			template<IntegerT ValueType> inline EtfSerializer(ValueType data) {
				*this = data;
			}

			template<BoolT ValueType> inline EtfSerializer& operator=(ValueType data) {
				setValue(JsonType::Bool);
				getBool() = data;
				return *this;
			}

			template<BoolT ValueType> inline EtfSerializer(ValueType data) {
				*this = data;
			}

			template<EnumT ValueType> inline EtfSerializer& operator=(ValueType data) {
				setValue(JsonType::Int);
				getInt() = static_cast<int64_t>(data);
				return *this;
			}

			template<EnumT ValueType> inline EtfSerializer(ValueType data) {
				*this = data;
			}

			inline EtfSerializer& operator=(JsonType data) {
				setValue(data);
				return *this;
			}

			inline EtfSerializer(JsonType data) {
				*this = data;
			}

			inline JsonType getType() {
				return type;
			}

			inline std::string refreshString() {
				stringReal.clear();
				appendVersion();
				serializeJsonToEtfString(*this);
				return std::move(stringReal);
			}

			inline EtfSerializer& operator[](typename object_type::key_type&& key) {
				if (type == JsonType::Null) {
					setValue(JsonType::Object);
				}

				if (type == JsonType::Object) {
					auto result = getObject().emplace(std::move(key), EtfSerializer{});
					return result.first->second;
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
			}

			inline EtfSerializer& operator[](const typename object_type::key_type& key) {
				if (type == JsonType::Null) {
					setValue(JsonType::Object);
				}

				if (type == JsonType::Object) {
					auto result = getObject().emplace(key, EtfSerializer{});
					return result.first->second;
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
			}

			inline EtfSerializer& operator[](uint64_t index) {
				if (type == JsonType::Null) {
					setValue(JsonType::Array);
				}

				if (type == JsonType::Array) {
					if (index >= getArray().size()) {
						getArray().resize(index + 1);
					}

					return getArray().at(index);
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
			}

			inline void emplaceBack(EtfSerializer&& other) {
				if (type == JsonType::Null) {
					setValue(JsonType::Array);
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
				}

				if (type == JsonType::Array) {
					getArray().emplace_back(other);
					return;
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
			}

			inline bool operator==(const EtfSerializer& lhs) const {
				if (lhs.type != type) {
					return false;
				}
				switch (type) {
					case JsonType::Object: {
						if (getObject() != getObject()) {
							return false;
						}
						break;
					}
					case JsonType::Array: {
						if (getArray() != getArray()) {
							return false;
						}
						break;
					}
					case JsonType::String: {
						if (getString() != getString()) {
							return false;
						}
						break;
					}
					case JsonType::Float: {
						if (getFloat() != getFloat()) {
							return false;
						}
						break;
					}
					case JsonType::Uint: {
						if (getUint() != getUint()) {
							return false;
						}
						break;
					}
					case JsonType::Int: {
						if (getInt() != getInt()) {
							return false;
						}
						break;
					}
					case JsonType::Bool: {
						if (getBool() != getBool()) {
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

			inline ~EtfSerializer() {
				destroy();
			}

		  protected:
			JsonType type{ JsonType::Null };
			std::string stringReal{};
			void* value{};

			inline object_type& getObject() {
				return *static_cast<object_type*>(value);
			}

			inline array_type& getArray() {
				return *static_cast<array_type*>(value);
			}

			inline string_type& getString() {
				return *static_cast<string_type*>(value);
			}

			inline float_type& getFloat() {
				return *static_cast<float_type*>(value);
			}

			inline uint_type& getUint() {
				return *static_cast<uint_type*>(value);
			}

			inline int_type& getInt() {
				return *static_cast<int_type*>(value);
			}

			inline bool_type& getBool() {
				return *static_cast<bool_type*>(value);
			}

			inline const object_type& getObject() const {
				return *static_cast<object_type*>(value);
			}

			inline const array_type& getArray() const {
				return *static_cast<array_type*>(value);
			}

			inline const string_type& getString() const {
				return *static_cast<string_type*>(value);
			}

			inline const float_type& getFloat() const {
				return *static_cast<float_type*>(value);
			}

			inline const uint_type& getUint() const {
				return *static_cast<uint_type*>(value);
			}

			inline const int_type& getInt() const {
				return *static_cast<int_type*>(value);
			}

			inline const bool_type& getBool() const {
				return *static_cast<bool_type*>(value);
			}

			inline void serializeJsonToEtfString(const EtfSerializer& dataToParse) {
				switch (dataToParse.type) {
					case JsonType::Object: {
						return writeEtfObject(dataToParse.getObject());
					}
					case JsonType::Array: {
						return writeEtfArray(dataToParse.getArray());
					}
					case JsonType::String: {
						return writeEtfString(dataToParse.getString());
					}
					case JsonType::Float: {
						return writeEtfFloat(dataToParse.getFloat());
					}
					case JsonType::Uint: {
						return writeEtfUint(dataToParse.getUint());
					}
					case JsonType::Int: {
						return writeEtfInt(dataToParse.getInt());
					}
					case JsonType::Bool: {
						return writeEtfBool(dataToParse.getBool());
					}
					case JsonType::Null: {
						return writeEtfNull();
					}
				}
			}

			inline void writeEtfObject(const object_type& jsonData) {
				appendMapHeader(static_cast<uint32_t>(jsonData.size()));
				for (auto& [key, valueNew]: jsonData) {
					appendBinaryExt(key, static_cast<uint32_t>(key.size()));
					serializeJsonToEtfString(valueNew);
				}
			}

			inline void writeEtfArray(const array_type& jsonData) {
				appendListHeader(static_cast<uint32_t>(jsonData.size()));
				for (auto& valueNew: jsonData) {
					serializeJsonToEtfString(valueNew);
				}
				appendNilExt();
			}

			inline void writeEtfString(const string_type& jsonData) {
				appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
			}

			inline void writeEtfUint(const uint_type jsonData) {
				if (jsonData <= std::numeric_limits<uint8_t>::max() && jsonData >= std::numeric_limits<uint8_t>::min()) {
					appendUint8(static_cast<uint8_t>(jsonData));
				} else if (jsonData <= std::numeric_limits<uint32_t>::max() && jsonData >= std::numeric_limits<uint32_t>::min()) {
					appendUint32(static_cast<uint32_t>(jsonData));
				} else {
					appendUint64(jsonData);
				}
			}

			inline void writeEtfInt(const int_type jsonData) {
				if (jsonData <= std::numeric_limits<int8_t>::max() && jsonData >= std::numeric_limits<int8_t>::min()) {
					appendInt8(static_cast<int8_t>(jsonData));
				} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
					appendInt32(static_cast<int32_t>(jsonData));
				} else {
					appendInt64(jsonData);
				}
			}

			inline void writeEtfFloat(const float_type jsonData) {
				appendNewFloatExt(jsonData);
			}

			inline void writeEtfBool(const bool_type jsonData) {
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

			inline void appendUint64(uint64_t valueNew) {
				char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
				char encodedBytes{};
				while (valueNew > 0) {
					newBuffer[3 + encodedBytes] = valueNew & 0xFF;
					valueNew >>= 8;
					++encodedBytes;
				}
				newBuffer[1] = encodedBytes;
				newBuffer[2] = 0;
				writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
			}

			inline void appendInt64(int64_t valueNew) {
				char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
				char encodedBytes{};
				while (valueNew > 0) {
					newBuffer[3 + encodedBytes] = valueNew & 0xFF;
					valueNew >>= 8;
					++encodedBytes;
				}
				newBuffer[1] = encodedBytes;
				if (valueNew >= 0) {
					newBuffer[2] = 0;
				} else {
					newBuffer[2] = 1;
				}
				writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
			}

			inline void appendUint32(const uint32_t valueNew) {
				char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
				storeBits(newBuffer + 1, valueNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendInt32(const int32_t valueNew) {
				char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
				storeBits(newBuffer + 1, valueNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendUint8(const uint8_t valueNew) {
				char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(valueNew) };
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendInt8(const int8_t valueNew) {
				char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(valueNew) };
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
						AllocWrapper<object_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getObject());
						break;
					}
					case JsonType::Array: {
						AllocWrapper<array_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getArray());
						break;
					}
					case JsonType::String: {
						AllocWrapper<string_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getString());
						break;
					}
					case JsonType::Float: {
						AllocWrapper<float_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getFloat());
						break;
					}
					case JsonType::Uint: {
						AllocWrapper<uint_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getUint());
						break;
					}
					case JsonType::Int: {
						AllocWrapper<int_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getInt());
						break;
					}
					case JsonType::Bool: {
						AllocWrapper<bool_type> alloc{};
						value = alloc.allocate(1);
						alloc.construct(&getBool());
						break;
					}
					case JsonType::Null: {
						break;
					}
				}
			}

			inline void destroy() {
				if (value) {
					switch (type) {
						case JsonType::Object: {
							AllocWrapper<object_type> alloc{};
							alloc.destroy(&getObject());
							alloc.deallocate(static_cast<object_type*>(value), 1);
							break;
						}
						case JsonType::Array: {
							AllocWrapper<array_type> alloc{};
							alloc.destroy(&getArray());
							alloc.deallocate(static_cast<array_type*>(value), 1);
							break;
						}
						case JsonType::String: {
							AllocWrapper<string_type> alloc{};
							alloc.destroy(&getString());
							alloc.deallocate(static_cast<string_type*>(value), 1);
							break;
						}
						case JsonType::Float: {
							AllocWrapper<float_type> alloc{};
							alloc.destroy(&getFloat());
							alloc.deallocate(static_cast<float_type*>(value), 1);
							break;
						}
						case JsonType::Uint: {
							AllocWrapper<uint_type> alloc{};
							alloc.destroy(&getUint());
							alloc.deallocate(static_cast<uint_type*>(value), 1);
							break;
						}
						case JsonType::Int: {
							AllocWrapper<int_type> alloc{};
							alloc.destroy(&getInt());
							alloc.deallocate(static_cast<int_type*>(value), 1);
							break;
						}
						case JsonType::Bool: {
							AllocWrapper<bool_type> alloc{};
							alloc.destroy(&getBool());
							alloc.deallocate(static_cast<bool_type*>(value), 1);
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

	};
}// namespace DiscordCoreInternal
