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

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/
		template<typename ReturnType> inline ReturnType ntohsNew(ReturnType value) {
			return static_cast<ReturnType>(((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));
		}

		template<typename ReturnType> inline ReturnType ntohlNew(ReturnType value) {
			return static_cast<ReturnType>(((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24));
		}

		template<typename ReturnType> inline ReturnType ntohllNew(ReturnType value) {
			return static_cast<ReturnType>(((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) | ((value & 0x0000000000FF0000ULL) << 24) |
				((value & 0x00000000FF000000ULL) << 8) | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
				((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56));
		}

		/// @brief Reverses the byte order of a value if needed, based on the endianness.
		/// @tparam ReturnType The type of the value to reverse.
		/// @param net The value to reverse.
		/// @return The reversed value.
		template<typename ReturnType> inline ReturnType reverseByteOrder(ReturnType net) {
			if constexpr (std::endian::native == std::endian::little) {
				switch (sizeof(ReturnType)) {
					case 2: {
						return ntohsNew(net);
					}
					case 4: {
						return ntohlNew(net);
					}
					case 8: {
						return ntohllNew(net);
					}
					default: {
						return net;
					}
				}
			} else {
				return net;
			}
		}

		/// @brief Stores the bits of a number into a character array.
		/// @tparam ReturnType The type of the number.
		/// @param to The character array to store the bits.
		/// @param num The number whose bits are to be stored.
		template<typename ReturnType, typename ValueType> inline void storeBits(ValueType* to, ReturnType num) {
			const uint8_t byteSize{ 8 };
			num = reverseByteOrder(num);

			// Store the bits of the number in the character array
			for (uint64_t x = 0; x < sizeof(ReturnType); ++x) {
				to[x] = static_cast<ValueType>(num >> (byteSize * x));
			}
		}

		/// @brief Exception class for ETF parsing errors.
		struct EtfParseError : public DCAException {
			/// @brief Constructs an EtfParseError instance with a message and source location.
			/// @param message The error message.
			/// @param location The source location where the error occurred.
			inline explicit EtfParseError(const std::string& message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
		};

		enum class EtfType : int8_t {
			New_Float_Ext	  = 70,
			Small_Integer_Ext = 97,
			Integer_Ext		  = 98,
			Atom_Ext		  = 100,
			Nil_Ext			  = 106,
			String_Ext		  = 107,
			List_Ext		  = 108,
			Binary_Ext		  = 109,
			Small_Big_Ext	  = 110,
			Small_Atom_Ext	  = 115,
			Map_Ext			  = 116,
		};

		constexpr uint8_t formatVersion{ 131 };

		/// @brief Class for parsing ETF data into JSON format.
		class DiscordCoreAPI_Dll EtfParser {
		  public:
			friend class WebSocketClient;

			/// @brief Parse ETF data to JSON format.
			/// @param dataToParse The ETF data to be parsed.
			/// @return The JSON representation of the parsed data.
			inline std::basic_string_view<uint8_t> parseEtfToJson(std::basic_string_view<uint8_t> dataToParse) {
				dataBuffer = dataToParse.data();
				dataSize   = dataToParse.size();
				finalString.clear();
				currentSize = 0;
				offSet		= 0;
				if (readBitsFromBuffer<uint8_t>() != formatVersion) {
					throw EtfParseError{ "EtfParser::parseEtfToJson() Error: Incorrect format version specified." };
				}
				singleValueETFToJson();
				return { finalString.data(), currentSize };
			}

		  protected:
			jsonifier::string_base<uint8_t> finalString{};///< The final JSON string.
			const uint8_t* dataBuffer{};///< Pointer to ETF data buffer.
			uint64_t currentSize{};///< Current size of the JSON string.
			uint64_t dataSize{};///< Size of the ETF data.
			uint64_t offSet{};///< Current offset in the ETF data.

			/// @brief Read bits from the data buffer and convert to ReturnType.
			/// @tparam ReturnType The type to convert the read data to.
			/// @return The converted value.
			template<typename ReturnType> inline ReturnType readBitsFromBuffer() {
				if (offSet + sizeof(ReturnType) > dataSize) {
					throw EtfParseError{ "EtfParser::readBitsFromBuffer() Error: readBitsFromBuffer() past end of the buffer." };
				}
				ReturnType newValue{};
				std::memcpy(&newValue, dataBuffer + offSet, sizeof(ReturnType));
				offSet += sizeof(ReturnType);
				newValue = reverseByteOrder(newValue);
				return newValue;
			}

			/// @brief Write characters to the final JSON string.
			/// @param data Pointer to the data to be written.
			/// @param length Number of characters to write.
			inline void writeCharacters(const char* data, uint64_t length) {
				if (finalString.size() < currentSize + length) {
					finalString.resize((finalString.size() + length) * 2);
				}
				std::memcpy(finalString.data() + currentSize, data, length);
				currentSize += length;
			}

			/// @brief Write characters from the buffer to the final JSON string.
			/// @param length Number of characters to write from the buffer.
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
				const uint8_t* stringNew = dataBuffer + offSet;
				offSet += length;
				if (length >= 3 && length <= 5) {
					if (length == 3 && stringNew[0] == 'n' && stringNew[1] == 'i' && stringNew[2] == 'l') {
						writeCharacters("null", 4);
						return;
					} else if (length == 4 && stringNew[0] == 'n' && stringNew[1] == 'u' && stringNew[2] == 'l' && stringNew[3] == 'l') {
						writeCharacters("null", 4);
						return;
					} else if (length == 4 && stringNew[0] == 't' && stringNew[1] == 'r' && stringNew[2] == 'u' && stringNew[3] == 'e') {
						writeCharacters("true", 4);
						return;
					} else if (length == 5 && stringNew[0] == 'f' && stringNew[1] == 'a' && stringNew[2] == 'l' && stringNew[3] == 's' && stringNew[4] == 'e') {
						writeCharacters("false", 5);
						return;
					}
				}
				writeCharacter<'"'>();
				for (uint64_t x = 0; x < length; ++x) {
					switch (stringNew[x]) {
						case '\\': {
							switch (stringNew[++x]) {
								case '\"':
									writeCharacter<'\"'>();
									break;
								case '\\':
									writeCharacter<'\\'>();
									break;
								case 'b':
									writeCharacter<'\b'>();
									break;
								case 'f':
									writeCharacter<'\f'>();
									break;
								case 'n':
									writeCharacter<'\n'>();
									break;
								case 'r':
									writeCharacter<'\r'>();
									break;
								case 't':
									writeCharacter<'\t'>();
									break;
								default: {
									writeCharacter(stringNew[x]);
									break;
								}
							}
							break;
						}
						case '"': {
							writeCharacter<'\\'>();
							writeCharacter<'\"'>();
							break;
						}
						default: {
							writeCharacter(stringNew[x]);
							break;
						}
					}
				}
				writeCharacter<'"'>();
			}

			/// @brief Write a character to the final JSON string.
			/// @param value The character to write.
			inline void writeCharacter(const char value) {
				if (finalString.size() < currentSize + 1) {
					finalString.resize((finalString.size() + 1) * 2);
				}
				finalString[currentSize++] = value;
			}

			/// @brief Write a character to the final JSON string.
			/// @tparam value The character to write.
			template<const char charToWrite> inline void writeCharacter() {
				if (finalString.size() < currentSize + 1) {
					finalString.resize((finalString.size() + 1) * 2);
				}
				finalString[currentSize++] = charToWrite;
			}

			/// @brief Parse a single ETF value and convert to JSON.
			void singleValueETFToJson() {
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

			/// @brief Parse ETF data representing a list and convert to JSON array.
			inline void parseListExt() {
				uint32_t length = readBitsFromBuffer<uint32_t>();
				writeCharacter<'['>();
				if (static_cast<uint64_t>(offSet) + length > dataSize) {
					throw EtfParseError{ "ErlPacker::parseListExt() Error: Read past end of buffer." };
				}
				for (uint16_t x = 0; x < length; ++x) {
					singleValueETFToJson();
					if (x < length - 1) {
						writeCharacter<','>();
					}
				}
				readBitsFromBuffer<uint8_t>();
				writeCharacter<']'>();
			}

			/// @brief Parse ETF data representing a small integer and convert to JSON number.
			inline void parseSmallIntegerExt() {
				auto string = std::to_string(readBitsFromBuffer<uint8_t>());
				writeCharacters(string.data(), string.size());
			}

			/// @brief Parse ETF data representing an integer and convert to JSON number.
			inline void parseIntegerExt() {
				auto string = std::to_string(readBitsFromBuffer<uint32_t>());
				writeCharacters(string.data(), string.size());
			}

			/// @brief Parse ETF data representing a string and convert to JSON string.
			inline void parseStringExt() {
				writeCharacter<'"'>();
				uint16_t length = readBitsFromBuffer<uint16_t>();
				if (static_cast<uint64_t>(offSet) + length > dataSize) {
					throw EtfParseError{ "ErlPacker::parseStringExt() Error: Read past end of buffer." };
				}
				for (uint16_t x = 0; x < length; ++x) {
					parseSmallIntegerExt();
				}
				writeCharacter<'"'>();
			}

			/// @brief Parse ETF data representing a new float and convert to JSON number.
			inline void parseNewFloatExt() {
				uint64_t value = readBitsFromBuffer<uint64_t>();
				double newDouble{};
				std::memcpy(&newDouble, &value, sizeof(double));
				std::string valueNew = std::to_string(newDouble);
				writeCharacters(valueNew.data(), valueNew.size());
			}

			/// @brief Parse ETF data representing a small big integer and convert to JSON number.
			inline void parseSmallBigExt() {
				writeCharacter<'"'>();
				auto digits	 = readBitsFromBuffer<uint8_t>();
				uint8_t sign = readBitsFromBuffer<uint8_t>();


				if (digits > 8) {
					throw EtfParseError{ "EtfParser::parseSmallBigExt() Error: Big integers larger than 8 bytes not supported." };
				}

				uint64_t value = 0;
				uint64_t bits  = 1;
				for (uint8_t x = 0; x < digits; ++x) {
					uint64_t digit = readBitsFromBuffer<uint8_t>();
					value += digit * bits;
					bits <<= 8;
				}

				if (sign == 0) {
					auto string = std::to_string(value);
					writeCharacters(string.data(), string.size());
					writeCharacter<'"'>();
				} else {
					auto string = std::to_string(-(static_cast<int64_t>(value)));
					writeCharacters(string.data(), string.size());
					writeCharacter<'"'>();
				}
			}

			/// @brief Parse ETF data representing an atom and convert to JSON string.
			inline void parseAtomExt() {
				writeCharactersFromBuffer(readBitsFromBuffer<uint16_t>());
			}

			/// @brief Parse ETF data representing a binary and convert to JSON string.
			inline void parseBinaryExt() {
				writeCharactersFromBuffer(readBitsFromBuffer<uint32_t>());
			}

			/// @brief Parse ETF data representing a nil value and convert to JSON null.
			inline void parseNilExt() {
				writeCharacters("[]", 2);
			}

			/// @brief Parse ETF data representing a small atom and convert to JSON string.
			inline void parseSmallAtomExt() {
				writeCharactersFromBuffer(readBitsFromBuffer<uint8_t>());
			}

			/// @brief Parse ETF data representing a map and convert to JSON object.
			inline void parseMapExt() {
				uint32_t length = readBitsFromBuffer<uint32_t>();
				writeCharacter<'{'>();
				for (uint32_t x = 0; x < length; ++x) {
					singleValueETFToJson();
					writeCharacter<':'>();
					singleValueETFToJson();
					if (x < length - 1) {
						writeCharacter<','>();
					}
				}
				writeCharacter<'}'>();
			}
		};

		/// @brief Custom exception class for ETF serialization errors.
		struct EtfSerializeError : public DCAException {
		  public:
			/// @brief Constructor for EtfSerializeError.
			/// @param message The error message.
			/// @param location Source location where the error occurred.
			inline EtfSerializeError(const std::string& message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
		};

		/// @brief Enumeration for different JSON value types.
		enum class JsonType : uint8_t { Null = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint = 5, Int = 6, Bool = 7 };

		/// @brief Forward declaration of the EtfSerializer class.
		class EtfSerializer;

		/// @brief Concept for floating-point types excluding EtfSerializer types.
		template<typename ValueType>
		concept FloatT = std::floating_point<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

		/// @brief Concept for signed integral types excluding EtfSerializer and boolean types.
		template<typename ValueType>
		concept SignedT = std::signed_integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !EtfSerializerT<ValueType>;

		/// @brief Concept for unsigned integral types excluding EtfSerializer, boolean, and signed types.
		template<typename ValueType>
		concept UnsignedT = std::unsigned_integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>> && !SignedT<ValueType> && !EtfSerializerT<ValueType>;

		/// @brief Concept for string types excluding EtfSerializer.
		template<typename ValueType>
		concept string_t = std::same_as<std::decay_t<ValueType>, std::string> || std::same_as<std::decay_t<ValueType>, std::string_view> ||
			std::convertible_to<ValueType, std::string> && !std::same_as<std::decay_t<ValueType>, char> && !EtfSerializerT<ValueType>;

		/// @brief Concept for types that have emplace_back method.
		template<typename ValueType>
		concept HasEmplaceBack = requires(ValueType data, typename ValueType::reference& valueNew) {
			{ data.emplace_back(valueNew) } -> std::same_as<typename ValueType::reference>;
		};

		/// @brief Concept for types that have begin and end methods.
		template<typename ValueType>
		concept HasRange = requires(ValueType data) {
			{ data.begin() } -> std::same_as<typename ValueType::iterator>;
			{ data.end() } -> std::same_as<typename ValueType::iterator>;
		};

		/// @brief Concept for types that have resize method.
		template<typename ValueType>
		concept HasResize = requires(ValueType data, uint64_t valueNew) {
			{ data.resize(valueNew) };
		};

		/// @brief Concept for null types excluding EtfSerializer.
		template<typename ValueType>
		concept NullT = std::same_as<std::decay_t<ValueType>, std::nullptr_t> && !EtfSerializerT<ValueType>;

		/// @brief Concept for types that are subscriptable with [] operator.
		template<typename ValueType>
		concept VectorSubscriptable = requires(ValueType data, uint64_t valueNew) {
			{ data[valueNew] } -> std::same_as<typename ValueType::reference>;
		};

		/// @brief Concept for array types excluding EtfSerializer.
		template<typename ValueType>
		concept ArrayT = HasRange<ValueType> && HasResize<std::decay_t<ValueType>> && HasEmplaceBack<std::decay_t<ValueType>> && VectorSubscriptable<std::decay_t<ValueType>> &&
			requires(ValueType data) { typename ValueType::value_type; } && !EtfSerializerT<ValueType>;

		/// @brief Concept for object (associative container) types excluding EtfSerializer.
		template<typename ValueType>
		concept ObjectT = requires(ValueType data) {
			typename ValueType::mapped_type;
			typename ValueType::key_type;
		} && HasRange<ValueType> && !EtfSerializerT<ValueType>;

		class EtfSerializer {
		  public:
			template<typename ValueType> using allocator = jsonifier_internal::alloc_wrapper<ValueType>;
			using object_type							 = UnorderedMap<std::string, EtfSerializer>;
			using array_type							 = jsonifier::vector<EtfSerializer>;
			using string_type							 = std::string;
			using float_type							 = double;
			using uint_type								 = uint64_t;
			using int_type								 = int64_t;
			using bool_type								 = bool;

			inline EtfSerializer() = default;

			inline EtfSerializer& operator=(EtfSerializer&& data) noexcept {
				destroyImpl();
				stringReal = std::move(data.stringReal);
				type	   = data.type;
				data.type  = JsonType::Null;
				switch (type) {
					case JsonType::Object: {
						objectValue		 = data.objectValue;
						data.objectValue = nullptr;
						break;
					}
					case JsonType::Array: {
						arrayValue		= data.arrayValue;
						data.arrayValue = nullptr;
						break;
					}
					case JsonType::String: {
						stringValue		 = data.stringValue;
						data.stringValue = nullptr;
						break;
					}
					case JsonType::Float: {
						floatValue		= data.floatValue;
						data.floatValue = nullptr;
						break;
					}
					case JsonType::Int: {
						intValue	  = data.intValue;
						data.intValue = nullptr;
						break;
					}
					case JsonType::Uint: {
						uintValue	   = data.uintValue;
						data.uintValue = nullptr;
						break;
					}
					case JsonType::Bool: {
						boolValue	   = data.boolValue;
						data.boolValue = nullptr;
						break;
					}
					case JsonType::Null: {
						break;
					}
				}
				return *this;
			}

			inline EtfSerializer(EtfSerializer&& data) noexcept {
				*this = std::move(data);
			}

			inline EtfSerializer& operator=(const EtfSerializer& data) {
				destroyImpl();
				switch (data.type) {
					case JsonType::Object: {
						setValue<JsonType::Object>(data.getObject());
						break;
					}
					case JsonType::Array: {
						setValue<JsonType::Array>(data.getArray());
						break;
					}
					case JsonType::String: {
						setValue<JsonType::String>(data.getString());
						break;
					}
					case JsonType::Float: {
						setValue<JsonType::Float>(data.getFloat());
						break;
					}
					case JsonType::Uint: {
						setValue<JsonType::Uint>(data.getUint());
						break;
					}
					case JsonType::Int: {
						setValue<JsonType::Int>(data.getInt());
						break;
					}
					case JsonType::Bool: {
						setValue<JsonType::Bool>(data.getBool());
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
				setValue<JsonType::Object>(std::forward<ValueType>(data));
				return *this;
			}

			template<ObjectT ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::forward<ValueType>(data);
			}

			template<ArrayT ValueType> inline EtfSerializer& operator=(ValueType&& data) {
				setValue<JsonType::Array>(std::forward<ValueType>(data));
				return *this;
			}

			template<ArrayT ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::forward<ValueType>(data);
			}

			template<string_t ValueType> inline EtfSerializer& operator=(ValueType&& data) {
				setValue<JsonType::String>(std::forward<ValueType>(data));
				return *this;
			}

			template<string_t ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::forward<ValueType>(data);
			}

			template<uint64_t StrLength> inline EtfSerializer& operator=(const char (&str)[StrLength]) {
				setValue<JsonType::String>(str);
				return *this;
			}

			template<uint64_t StrLength> inline EtfSerializer(const char (&str)[StrLength]) {
				*this = str;
			}

			template<FloatT ValueType> inline EtfSerializer& operator=(ValueType data) {
				setValue<JsonType::Float>(std::forward<ValueType>(data));
				return *this;
			}

			template<FloatT ValueType> inline EtfSerializer(ValueType data) {
				*this = std::forward<ValueType>(data);
			}

			template<IntegerT ValueType> inline EtfSerializer& operator=(ValueType data) {
				if constexpr (SignedT<ValueType>) {
					setValue<JsonType::Int>(std::forward<ValueType>(data));
				} else if constexpr (UnsignedT<ValueType>) {
					setValue<JsonType::Uint>(std::forward<ValueType>(data));
				}
				return *this;
			}

			template<IntegerT ValueType> inline EtfSerializer(ValueType data) {
				*this = std::forward<ValueType>(data);
			}

			template<BoolT ValueType> inline EtfSerializer& operator=(ValueType data) {
				setValue<JsonType::Bool>(std::forward<ValueType>(data));
				return *this;
			}

			template<BoolT ValueType> inline EtfSerializer(ValueType data) {
				*this = std::forward<ValueType>(data);
			}

			template<EnumT ValueType> inline EtfSerializer& operator=(ValueType&& data) {
				setValue<JsonType::Int>(static_cast<int64_t>(std::forward<ValueType>(data)));
				return *this;
			}

			template<EnumT ValueType> inline EtfSerializer(ValueType&& data) {
				*this = std::forward<ValueType>(data);
			}

			inline EtfSerializer& operator=(JsonType data) {
				switch (data) {
					case JsonType::Object: {
						setValue<JsonType::Object>();
						break;
					}
					case JsonType::Array: {
						setValue<JsonType::Array>();
						break;
					}
					case JsonType::String: {
						setValue<JsonType::String>();
						break;
					}
					case JsonType::Float: {
						setValue<JsonType::Float>();
						break;
					}
					case JsonType::Uint: {
						setValue<JsonType::Uint>();
						break;
					}
					case JsonType::Int: {
						setValue<JsonType::Int>();
						break;
					}
					case JsonType::Bool: {
						setValue<JsonType::Bool>();
						break;
					}
					case JsonType::Null: {
						setValue<JsonType::Null>();
						break;
					}
				}
				return *this;
			}

			inline EtfSerializer(JsonType data) {
				*this = data;
			}

			inline JsonType getType() const {
				return type;
			}

			inline operator std::basic_string<uint8_t>() {
				stringReal.clear();
				appendVersion();
				serializeJsonToEtfString(*this);
				return stringReal;
			}

			inline EtfSerializer& operator[](const typename object_type::key_type& key) {
				if (type == JsonType::Null) {
					setValue<JsonType::Object>();
				}

				if (type == JsonType::Object) {
					return getObject().operator[](key);
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
			}

			template<typename object_type_new> inline EtfSerializer& operator[](typename object_type::key_type&& key) {
				if (type == JsonType::Null) {
					setValue<JsonType::Object>();
				}

				if (type == JsonType::Object) {
					return getObject().operator[](std::forward<typename object_type_new::key_type>(key));
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Object." };
			}

			inline EtfSerializer& operator[](uint64_t index) {
				if (type == JsonType::Null) {
					setValue<JsonType::Array>();
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
					setValue<JsonType::Array>();
				}

				if (type == JsonType::Array) {
					getArray().emplace_back(std::move(other));
					return;
				}
				throw EtfSerializeError{ "Sorry, but this value's type is not Array." };
			}

			inline void emplaceBack(const EtfSerializer& other) {
				if (type == JsonType::Null) {
					setValue<JsonType::Array>();
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
						if (!compareValues<JsonType::Object>(lhs)) {
							return false;
						}
						break;
					}
					case JsonType::Array: {
						if (!compareValues<JsonType::Array>(lhs)) {
							return false;
						}
						break;
					}
					case JsonType::String: {
						if (!compareValues<JsonType::String>(lhs)) {
							return false;
						}
						break;
					}
					case JsonType::Float: {
						if (!compareValues<JsonType::Float>(lhs)) {
							return false;
						}
						break;
					}
					case JsonType::Uint: {
						if (!compareValues<JsonType::Uint>(lhs)) {
							return false;
						}
						break;
					}
					case JsonType::Int: {
						if (!compareValues<JsonType::Int>(lhs)) {
							return false;
						}
						break;
					}
					case JsonType::Bool: {
						if (!compareValues<JsonType::Bool>(lhs)) {
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

			inline object_type& getObject() const {
				if (this->type != JsonType::Object) {
					throw EtfSerializeError{ "Sorry, but this value's type is not object!" };
				}
				return *objectValue;
			}

			inline array_type& getArray() const {
				if (this->type != JsonType::Array) {
					throw EtfSerializeError{ "Sorry, but this value's type is not array!" };
				}
				return *arrayValue;
			}

			inline string_type& getString() const {
				if (this->type != JsonType::String) {
					throw EtfSerializeError{ "Sorry, but this value's type is not string!" };
				}
				return *stringValue;
			}

			inline float_type& getFloat() const {
				if (this->type != JsonType::Float) {
					throw EtfSerializeError{ "Sorry, but this value's type is not float!" };
				}
				return *floatValue;
			}

			inline uint_type& getUint() const {
				if (this->type != JsonType::Uint) {
					throw EtfSerializeError{ "Sorry, but this value's type is not uint!" };
				}
				return *uintValue;
			}

			inline int_type& getInt() const {
				if (this->type != JsonType::Int) {
					throw EtfSerializeError{ "Sorry, but this value's type is not int!" };
				}
				return *intValue;
			}

			inline bool_type& getBool() const {
				if (this->type != JsonType::Bool) {
					throw EtfSerializeError{ "Sorry, but this value's type is not bool!" };
				}
				return *boolValue;
			}

			inline ~EtfSerializer() {
				destroyImpl();
			}

		  protected:
			std::basic_string<uint8_t> stringReal{};
			JsonType type{ JsonType::Null };
			union {
				UnorderedMap<std::string, EtfSerializer>* objectValue;
				jsonifier::vector<EtfSerializer>* arrayValue;
				std::string* stringValue;
				double* floatValue;
				uint64_t* uintValue;
				int64_t* intValue;
				bool* boolValue;
			};

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

			template<typename ValueType> inline void writeString(const ValueType* data, uint64_t length) {
				auto oldSize = stringReal.size();
				stringReal.resize(oldSize + length);
				std::memcpy(stringReal.data() + oldSize, data, length);
			}

			inline void appendBinaryExt(const std::string& bytes, uint32_t sizeNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(EtfType::Binary_Ext) };
				storeBits(newBuffer + 1, sizeNew);
				writeString(newBuffer, std::size(newBuffer));
				writeString(bytes.data(), bytes.size());
			}

			inline void appendNewFloatExt(const double newFloat) {
				uint8_t newBuffer[9]{ static_cast<uint8_t>(EtfType::New_Float_Ext) };
				uint64_t newValue{};
				std::memcpy(&newValue, &newFloat, sizeof(newFloat));
				storeBits(newBuffer + 1, newValue);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendListHeader(const uint32_t sizeNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(EtfType::List_Ext) };
				storeBits(newBuffer + 1, sizeNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendMapHeader(const uint32_t sizeNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(EtfType::Map_Ext) };
				storeBits(newBuffer + 1, sizeNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendUint64(uint64_t valueNew) {
				uint8_t newBuffer[11]{ static_cast<uint8_t>(EtfType::Small_Big_Ext) };
				uint8_t encodedBytes{};
				while (valueNew > 0) {
					newBuffer[3 + encodedBytes] = static_cast<uint8_t>(valueNew & 0xFF);
					valueNew >>= 8;
					++encodedBytes;
				}
				newBuffer[1] = encodedBytes;
				newBuffer[2] = 0;
				writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
			}

			inline void appendInt64(int64_t valueNew) {
				uint8_t newBuffer[11]{ static_cast<uint8_t>(EtfType::Small_Big_Ext) };
				uint8_t encodedBytes{};
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
				uint8_t newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
				storeBits(newBuffer + 1, valueNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendInt32(const int32_t valueNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
				storeBits(newBuffer + 1, valueNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendUint8(const uint8_t valueNew) {
				uint8_t newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<uint8_t>(valueNew) };
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendInt8(const int8_t valueNew) {
				uint8_t newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<uint8_t>(valueNew) };
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendBool(bool data) {
				if (data) {
					uint8_t newBuffer[6]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
					writeString(newBuffer, std::size(newBuffer));

				} else {
					uint8_t newBuffer[7]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
					writeString(newBuffer, std::size(newBuffer));
				}
			}

			inline void appendVersion() {
				uint8_t newBuffer[1]{ static_cast<uint8_t>(formatVersion) };
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendNilExt() {
				uint8_t newBuffer[1]{ static_cast<uint8_t>(EtfType::Nil_Ext) };
				writeString(newBuffer, std::size(newBuffer));
			}

			inline void appendNil() {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
				writeString(newBuffer, std::size(newBuffer));
			}

			template<JsonType typeNew, typename... ValueTypes> inline void setValue(ValueTypes&&... args) {
				destroyImpl();
				type = typeNew;
				if constexpr (typeNew == JsonType::Object) {
					allocator<object_type> alloc{};
					objectValue = alloc.allocate(1);
					alloc.construct(objectValue, std::forward<ValueTypes>(args)...);
				} else if constexpr (typeNew == JsonType::Array) {
					allocator<array_type> alloc{};
					arrayValue = alloc.allocate(1);
					alloc.construct(arrayValue, std::forward<ValueTypes>(args)...);
				} else if constexpr (typeNew == JsonType::String) {
					allocator<string_type> alloc{};
					stringValue = alloc.allocate(1);
					alloc.construct(stringValue, std::forward<ValueTypes>(args)...);
				} else if constexpr (typeNew == JsonType::Float) {
					allocator<float_type> alloc{};
					floatValue = alloc.allocate(1);
					alloc.construct(floatValue, std::forward<ValueTypes>(args)...);
				} else if constexpr (typeNew == JsonType::Uint) {
					allocator<uint_type> alloc{};
					uintValue = alloc.allocate(1);
					alloc.construct(uintValue, std::forward<ValueTypes>(args)...);
				} else if constexpr (typeNew == JsonType::Int) {
					allocator<int_type> alloc{};
					intValue = alloc.allocate(1);
					alloc.construct(intValue, std::forward<ValueTypes>(args)...);
				} else if constexpr (typeNew == JsonType::Bool) {
					allocator<bool_type> alloc{};
					boolValue = alloc.allocate(1);
					alloc.construct(boolValue, std::forward<ValueTypes>(args)...);
				}
			}

			template<JsonType typeNew> inline void destroy() {
				if constexpr (typeNew == JsonType::Object) {
					allocator<object_type> alloc{};
					alloc.destroy(objectValue);
					alloc.deallocate(static_cast<object_type*>(objectValue), 1);
					objectValue = nullptr;
				} else if constexpr (typeNew == JsonType::Array) {
					allocator<array_type> alloc{};
					alloc.destroy(arrayValue);
					alloc.deallocate(static_cast<array_type*>(arrayValue), 1);
					arrayValue = nullptr;
				} else if constexpr (typeNew == JsonType::String) {
					allocator<string_type> alloc{};
					alloc.destroy(stringValue);
					alloc.deallocate(static_cast<string_type*>(stringValue), 1);
					stringValue = nullptr;
				} else if constexpr (typeNew == JsonType::Float) {
					allocator<float_type> alloc{};
					alloc.destroy(floatValue);
					alloc.deallocate(static_cast<float_type*>(floatValue), 1);
					floatValue = nullptr;
				} else if constexpr (typeNew == JsonType::Uint) {
					allocator<uint_type> alloc{};
					alloc.destroy(uintValue);
					alloc.deallocate(static_cast<uint_type*>(uintValue), 1);
					uintValue = nullptr;
				} else if constexpr (typeNew == JsonType::Int) {
					allocator<int_type> alloc{};
					alloc.destroy(intValue);
					alloc.deallocate(static_cast<int_type*>(intValue), 1);
					intValue = nullptr;
				} else if constexpr (typeNew == JsonType::Bool) {
					allocator<bool_type> alloc{};
					alloc.destroy(boolValue);
					alloc.deallocate(static_cast<bool_type*>(boolValue), 1);
					boolValue = nullptr;
				}
			}

			template<JsonType typeNew> inline bool compareValues(const EtfSerializer& other) const {
				if constexpr (typeNew == JsonType::Object) {
					return *objectValue == *other.objectValue;
				} else if constexpr (typeNew == JsonType::Array) {
					return *arrayValue == *other.arrayValue;
				} else if constexpr (typeNew == JsonType::String) {
					return *stringValue == *other.stringValue;
				} else if constexpr (typeNew == JsonType::Float) {
					return *floatValue == *other.floatValue;
				} else if constexpr (typeNew == JsonType::Uint) {
					return *uintValue == *other.uintValue;
				} else if constexpr (typeNew == JsonType::Int) {
					return *intValue == *other.intValue;
				} else if constexpr (typeNew == JsonType::Bool) {
					return *boolValue == *other.boolValue;
				} else {
					return true;
				}
			}

			inline void destroyImpl() {
				switch (type) {
					case JsonType::Object: {
						destroy<JsonType::Object>();
						break;
					}
					case JsonType::Array: {
						destroy<JsonType::Array>();
						break;
					}
					case JsonType::String: {
						destroy<JsonType::String>();
						break;
					}
					case JsonType::Float: {
						destroy<JsonType::Float>();
						break;
					}
					case JsonType::Uint: {
						destroy<JsonType::Uint>();
						break;
					}
					case JsonType::Int: {
						destroy<JsonType::Int>();
						break;
					}
					case JsonType::Bool: {
						destroy<JsonType::Bool>();
						break;
					}
					case JsonType::Null: {
						break;
					}
					default: {
						break;
					}
				}
				type = JsonType::Null;
			}
		};

		/**@}*/

	};
}// namespace DiscordCoreInternal