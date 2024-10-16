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

namespace discord_core_api {

	namespace discord_core_internal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/
		template<typename return_type> DCA_INLINE return_type ntohsNew(return_type value) {
			return static_cast<return_type>(((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));
		}

		template<typename return_type> DCA_INLINE return_type ntohlNew(return_type value) {
			return static_cast<return_type>(((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24));
		}

		template<typename return_type> DCA_INLINE return_type ntohllNew(return_type value) {
			return static_cast<return_type>(((value & 0x00000000000000FFull) << 56) | ((value & 0x000000000000FF00ULL) << 40) | ((value & 0x0000000000FF0000ULL) << 24) |
				((value & 0x00000000FF000000ULL) << 8) | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
				((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56));
		}

		/// @brief Reverses the byte order of a value if needed, based on the endianness.
		/// @tparam return_type the type of the value to reverse.
		/// @param net the value to reverse.
		/// @return the reversed value.
		template<typename return_type> DCA_INLINE void reverseByteOrder(return_type& net) {
			if constexpr (std::endian::native == std::endian::little) {
				switch (sizeof(return_type)) {
					case 2: {
						net = ntohsNew(net);
						return;
					}
					case 4: {
						net = ntohlNew(net);
						return;
					}
					case 8: {
						net = ntohllNew(net);
						return;
					}
					default: {
						return;
					}
				}
			} else {
				return;
			}
		}

		/// @brief Stores the bits of a number into a character array.
		/// @tparam return_type the type of the number.
		/// @param to the character array to store the bits.
		/// @param num the number whose bits are to be stored.
		template<typename return_type, typename value_type> DCA_INLINE void storeBits(value_type* to, return_type num) {
			static constexpr uint8_t byteSize{ 8 };
			reverseByteOrder(num);

			// store the bits of the number in the character array
			for (uint64_t x = 0; x < sizeof(return_type); ++x) {
				to[x] = static_cast<value_type>(num >> (byteSize * x));
			}
		}

		/// @brief Exception class for etf parsing errors.
		struct etf_parse_error : public dca_exception {
			/// @brief Constructs an etf_parse_error instance with a message and source location.
			/// @param message the error message.
			/// @param location the source location where the error occurred.
			DCA_INLINE explicit etf_parse_error(const jsonifier::string_view& message, std::source_location location = std::source_location::current())
				: dca_exception{ message, location } {};
		};

		enum class etf_type : int8_t {
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

		/// @brief Class for parsing etf data into json format.
		class DiscordCoreAPI_Dll etf_parser : public jsonifier_internal::alloc_wrapper<uint8_t> {
		  public:
			friend class websocket_client;
			using allocator = jsonifier_internal::alloc_wrapper<uint8_t>;

			/// @brief Parse etf data to json format.
			/// @param dataToParse the etf data to be parsed.
			/// @return the json representation of the parsed data.
			DCA_INLINE jsonifier::string_view_base<uint8_t> parseEtfToJson(jsonifier::string_view_base<uint8_t> dataToParse) {
				dataBuffer = dataToParse.data();
				dataSize   = dataToParse.size();
				finalString.clear();
				currentSize = 0;
				offSet		= 0;
				if (readBitsFromBuffer<uint8_t>() != formatVersion) {
					throw etf_parse_error{ "etf_parser::parseEtfToJson() error: incorrect format version specified." };
				}
				singleValueETFToJson();
				return { finalString.data(), currentSize };
			}

		  protected:
			jsonifier::string_base<uint8_t> finalString{};///< The final json string.
			const uint8_t* dataBuffer{};///< Pointer to etf data buffer.
			uint64_t currentSize{};///< c///< current size of the json string.
			uint64_t dataSize{};///< Size of the etf data.
			uint64_t offSet{};///< current offset in the etf data.

			/// @brief Read bits from the data buffer and convert to return_type.
			/// @tparam return_type the type to convert the read data to.
			/// @return the converted value.
			template<typename return_type> DCA_INLINE return_type readBitsFromBuffer() {
				if (offSet + sizeof(return_type) > dataSize) {
					throw etf_parse_error{ "etf_parser::readBitsFromBuffer() error: readBitsFromBuffer() past end of the buffer." };
				}
				return_type newValue{};
				std::memcpy(&newValue, dataBuffer + offSet, sizeof(return_type));
				offSet += sizeof(return_type);
				reverseByteOrder(newValue);
				return newValue;
			}

			/// @brief Write characters to the final json string.
			/// @param data pointer to the data to be written.
			/// @param length number of characters to write.
			DCA_INLINE void writeCharacters(const char* data, uint64_t length) {
				if (finalString.size() < currentSize + length) {
					finalString.resize((finalString.size() + length) * 2);
				}
				std::memcpy(finalString.data() + currentSize, data, length);
				currentSize += length;
			}

			/// @brief Write characters to the final json string.
			/// @param data pointer to the data to be written.
			/// @param length number of characters to write.
			template<uint64_t size> DCA_INLINE void writeCharacters(const char (&data)[size]) {
				if (finalString.size() < currentSize + size - 1) {
					finalString.resize((finalString.size() + size - 1) * 2);
				}
				std::memcpy(finalString.data() + currentSize, data, size - 1);
				currentSize += size - 1;
			}

			/// @brief Write characters from the buffer to the final json string.
			/// @param length number of characters to write from the buffer.
			DCA_INLINE void writeCharactersFromBuffer(uint32_t length) {
				if (!length) {
					writeCharacters("\"\"");
					return;
				}
				if (offSet + static_cast<uint64_t>(length) > dataSize) {
					throw etf_parse_error{ "erl_packer::writeCharactersFromBuffer() error: read past end of buffer." };
				}
				if (finalString.size() < currentSize + length) {
					finalString.resize((finalString.size() + length) * 2);
				}
				const uint8_t* stringNew = dataBuffer + offSet;
				offSet += length;
				if (length >= 3 && length <= 5) {
					if (length == 3 && stringNew[0] == 'n' && stringNew[1] == 'i' && stringNew[2] == 'l') {
						writeCharacters("null");
						return;
					} else if (length == 4 && stringNew[0] == 'n' && stringNew[1] == 'u' && stringNew[2] == 'l' && stringNew[3] == 'l') {
						writeCharacters("null");
						return;
					} else if (length == 4 && stringNew[0] == 't' && stringNew[1] == 'r' && stringNew[2] == 'u' && stringNew[3] == 'e') {
						writeCharacters("true");
						return;
					} else if (length == 5 && stringNew[0] == 'f' && stringNew[1] == 'a' && stringNew[2] == 'l' && stringNew[3] == 's' && stringNew[4] == 'e') {
						writeCharacters("false");
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

			/// @brief Write a character to the final json string.
			/// @param value the character to write.
			template<typename value_type> DCA_INLINE void writeCharacter(const value_type value) {
				if (finalString.size() < currentSize + 1) {
					finalString.resize((finalString.size() + 1) * 2);
				}
				allocator::construct(&finalString[currentSize++], static_cast<uint8_t>(value));
			}

			/// @brief Write a character to the final json string.
			/// @tparam value the character to write.
			template<const char charToWrite> DCA_INLINE void writeCharacter() {
				if (finalString.size() < currentSize + 1) {
					finalString.resize((finalString.size() + 1) * 2);
				}
				allocator::construct(&finalString[currentSize++], static_cast<uint8_t>(charToWrite));
			}

			/// @brief Parse a single etf value and convert to json.
			DCA_INLINE void singleValueETFToJson() {
				if (offSet > dataSize) {
					throw etf_parse_error{ "erl_packer::singleValueETFToJson() error: read past end of buffer." };
				}
				uint8_t type = readBitsFromBuffer<uint8_t>();
				switch (static_cast<etf_type>(type)) {
					case etf_type::New_Float_Ext: {
						return parseNewFloatExt();
					}
					case etf_type::Small_Integer_Ext: {
						return parseSmallIntegerExt();
					}
					case etf_type::Integer_Ext: {
						return parseIntegerExt();
					}
					case etf_type::Atom_Ext: {
						return parseAtomExt();
					}
					case etf_type::Nil_Ext: {
						return parseNilExt();
					}
					case etf_type::String_Ext: {
						return parseStringExt();
					}
					case etf_type::List_Ext: {
						return parseListExt();
					}
					case etf_type::Binary_Ext: {
						return parseBinaryExt();
					}
					case etf_type::Small_Big_Ext: {
						return parseSmallBigExt();
					}
					case etf_type::Small_Atom_Ext: {
						return parseSmallAtomExt();
					}
					case etf_type::Map_Ext: {
						return parseMapExt();
					}
					default: {
						throw etf_parse_error{ "etf_parser::singleValueETFToJson() error: unknown data type in etf, the type: " + jsonifier::toString(type) };
					}
				}
			}

			/// @brief Parse etf data representing a list and convert to json array.
			DCA_INLINE void parseListExt() {
				uint32_t length = readBitsFromBuffer<uint32_t>();
				writeCharacter<'['>();
				if (static_cast<uint64_t>(offSet) + length > dataSize) {
					throw etf_parse_error{ "erl_packer::parseListExt() error: read past end of buffer." };
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

			/// @brief Parse etf data representing a small integer and convert to json number.
			DCA_INLINE void parseSmallIntegerExt() {
				auto string = jsonifier::toString(readBitsFromBuffer<uint8_t>());
				writeCharacters(string.data(), string.size());
			}

			/// @brief Parse etf data representing an integer and convert to json number.
			DCA_INLINE void parseIntegerExt() {
				auto string = jsonifier::toString(readBitsFromBuffer<uint32_t>());
				writeCharacters(string.data(), string.size());
			}

			/// @brief Parse etf data representing a string and convert to json string.
			DCA_INLINE void parseStringExt() {
				writeCharacter<'"'>();
				uint16_t length = readBitsFromBuffer<uint16_t>();
				if (static_cast<uint64_t>(offSet) + length > dataSize) {
					throw etf_parse_error{ "erl_packer::parseStringExt() error: read past end of buffer." };
				}
				for (uint16_t x = 0; x < length; ++x) {
					parseSmallIntegerExt();
				}
				writeCharacter<'"'>();
			}

			/// @brief Parse etf data representing a new float and convert to json number.
			DCA_INLINE void parseNewFloatExt() {
				uint64_t value = readBitsFromBuffer<uint64_t>();
				double newDouble{};
				std::memcpy(&newDouble, &value, sizeof(double));
				jsonifier::string valueNew = jsonifier::toString(newDouble);
				writeCharacters(valueNew.data(), valueNew.size());
			}

			/// @brief Parse etf data representing a small big integer and convert to json number.
			DCA_INLINE void parseSmallBigExt() {
				auto digits	 = readBitsFromBuffer<uint8_t>();
				uint8_t sign = readBitsFromBuffer<uint8_t>();

				if (digits > 8) {
					throw etf_parse_error{ "etf_parser::parseSmallBigExt() error: big integers larger than 8 bytes not supported." };
				}

				uint64_t value = 0;
				uint64_t bits  = 1;
				for (uint8_t x = 0; x < digits; ++x) {
					uint64_t digit = readBitsFromBuffer<uint8_t>();
					value += digit * bits;
					bits <<= 8;
				}

				if (sign == 0) {
					auto string = jsonifier::toString(value);
					writeCharacters(string.data(), string.size());
				} else {
					auto string = jsonifier::toString(-(static_cast<int64_t>(value)));
					writeCharacters(string.data(), string.size());
				}
			}

			/// @brief Parse etf data representing an atom and convert to json string.
			DCA_INLINE void parseAtomExt() {
				writeCharactersFromBuffer(readBitsFromBuffer<uint16_t>());
			}

			/// @brief Parse etf data representing a binary and convert to json string.
			DCA_INLINE void parseBinaryExt() {
				writeCharactersFromBuffer(readBitsFromBuffer<uint32_t>());
			}

			/// @brief Parse etf data representing a nil value and convert to json null.
			DCA_INLINE void parseNilExt() {
				writeCharacters("[]");
			}

			/// @brief Parse etf data representing a small atom and convert to json string.
			DCA_INLINE void parseSmallAtomExt() {
				writeCharactersFromBuffer(readBitsFromBuffer<uint8_t>());
			}

			/// @brief Parse etf data representing a map and convert to json object.
			DCA_INLINE void parseMapExt() {
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

		/// @brief Custom exception class for etf serialization errors.
		struct etf_serialize_error : public dca_exception {
		  public:
			/// @brief Constructor for etf_serialize_error.
			/// @param message the error message.
			/// @param location source location where the error occurred.
			DCA_INLINE etf_serialize_error(const jsonifier::string_view& message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
		};

		/// @brief Enumeration for different json value types.
		enum class json_type : uint8_t { null_t = 0, object_t = 1, array_t = 2, string_t = 3, float_t = 4, uint_t = 5, int_t = 6, bool_t = 7 };

		/// @brief Concept for array types excluding etf_serializer.
		template<typename value_type>
		concept array_t = jsonifier::concepts::range<value_type> && jsonifier::concepts::has_resize<std::remove_cvref_t<value_type>> &&
			jsonifier::concepts::has_emplace_back<std::remove_cvref_t<value_type>> && jsonifier::concepts::vector_subscriptable<std::remove_cvref_t<value_type>> &&
			requires(value_type&& data) { typename value_type::value_type; };

		/// @brief Concept for object (associative container) types excluding etf_serializer.
		template<typename value_type>
		concept object_t = requires(value_type data) {
			typename value_type::mapped_type;
			typename value_type::key_type;
		} && jsonifier::concepts::range<value_type>;

		class etf_serializer {
		  public:
			template<typename value_type> using allocator = jsonifier_internal::alloc_wrapper<value_type>;
			using object_type							  = unordered_map<jsonifier::string, etf_serializer>;
			using array_type							  = jsonifier::vector<etf_serializer>;
			using string_type							  = jsonifier::string;
			using float_type							  = double;
			using uint_type								  = uint64_t;
			using int_type								  = int64_t;
			using bool_type								  = bool;

			DCA_INLINE etf_serializer() = default;

			DCA_INLINE etf_serializer& operator=(etf_serializer&& data) noexcept {
				destroy();
				stringReal = std::move(data.stringReal);
				type	   = data.type;
				data.type  = json_type::null_t;
				switch (type) {
					case json_type::object_t: {
						objectValue		 = data.objectValue;
						data.objectValue = nullptr;
						break;
					}
					case json_type::array_t: {
						arrayValue		= data.arrayValue;
						data.arrayValue = nullptr;
						break;
					}
					case json_type::string_t: {
						stringValue		 = data.stringValue;
						data.stringValue = nullptr;
						break;
					}
					case json_type::float_t: {
						floatValue		= data.floatValue;
						data.floatValue = nullptr;
						break;
					}
					case json_type::int_t: {
						intValue	  = data.intValue;
						data.intValue = nullptr;
						break;
					}
					case json_type::uint_t: {
						uintValue	   = data.uintValue;
						data.uintValue = nullptr;
						break;
					}
					case json_type::bool_t: {
						boolValue	   = data.boolValue;
						data.boolValue = nullptr;
						break;
					}
					case json_type::null_t: {
						break;
					}
				}
				return *this;
			}

			DCA_INLINE etf_serializer(etf_serializer&& data) noexcept {
				*this = std::move(data);
			}

			DCA_INLINE etf_serializer& operator=(const etf_serializer& data) {
				destroy();
				switch (data.type) {
					case json_type::object_t: {
						setValue<json_type::object_t>(data.getObject());
						break;
					}
					case json_type::array_t: {
						setValue<json_type::array_t>(data.getArray());
						break;
					}
					case json_type::string_t: {
						setValue<json_type::string_t>(data.getString());
						break;
					}
					case json_type::float_t: {
						setValue<json_type::float_t>(data.getFloat());
						break;
					}
					case json_type::uint_t: {
						setValue<json_type::uint_t>(data.getUint());
						break;
					}
					case json_type::int_t: {
						setValue<json_type::int_t>(data.getInt());
						break;
					}
					case json_type::bool_t: {
						setValue<json_type::bool_t>(data.getBool());
						break;
					}
					case json_type::null_t: {
						break;
					}
				}
				stringReal = data.stringReal;
				return *this;
			}

			DCA_INLINE etf_serializer(const etf_serializer& data) {
				*this = data;
			}

			template<object_t value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) noexcept {
				setValue<json_type::object_t>(std::forward<value_type>(data));
				return *this;
			}

			template<object_t value_type> DCA_INLINE etf_serializer(value_type&& data) noexcept {
				*this = std::forward<value_type>(data);
			}

			template<array_t value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) noexcept {
				setValue<json_type::array_t>(std::forward<value_type>(data));
				return *this;
			}

			template<array_t value_type> DCA_INLINE etf_serializer(value_type&& data) noexcept {
				*this = std::forward<value_type>(data);
			}

			template<jsonifier::concepts::string_t value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) noexcept {
				setValue<json_type::string_t>(std::forward<value_type>(data));
				return *this;
			}

			template<jsonifier::concepts::string_t value_type> DCA_INLINE etf_serializer(value_type&& data) noexcept {
				*this = std::forward<value_type>(data);
			}

			template<uint_type str_length> DCA_INLINE etf_serializer& operator=(const char (&str)[str_length]) {
				setValue<json_type::string_t>(str);
				return *this;
			}

			template<uint_type str_length> DCA_INLINE etf_serializer(const char (&str)[str_length]) {
				*this = str;
			}

			template<jsonifier::concepts::float_type value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) {
				setValue<json_type::float_t>(std::forward<value_type>(data));
				return *this;
			}

			template<jsonifier::concepts::float_type value_type> DCA_INLINE etf_serializer(value_type&& data) {
				*this = std::forward<value_type>(data);
			}

			template<jsonifier::concepts::integer_t value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) {
				if constexpr (jsonifier::concepts::signed_type<value_type>) {
					setValue<json_type::int_t>(std::forward<value_type>(data));
				} else if constexpr (jsonifier::concepts::unsigned_type<value_type>) {
					setValue<json_type::uint_t>(std::forward<value_type>(data));
				}
				return *this;
			}

			template<jsonifier::concepts::integer_t value_type> DCA_INLINE etf_serializer(value_type&& data) {
				*this = std::forward<value_type>(data);
			}

			template<jsonifier::concepts::bool_t value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) {
				setValue<json_type::bool_t>(std::forward<value_type>(data));
				return *this;
			}

			template<jsonifier::concepts::bool_t value_type> DCA_INLINE etf_serializer(value_type&& data) {
				*this = std::forward<value_type>(data);
			}

			template<jsonifier::concepts::enum_t value_type> DCA_INLINE etf_serializer& operator=(value_type&& data) noexcept {
				setValue<json_type::int_t>(static_cast<int_type>(std::forward<value_type>(data)));
				return *this;
			}

			template<jsonifier::concepts::enum_t value_type> DCA_INLINE etf_serializer(value_type&& data) noexcept {
				*this = std::forward<value_type>(data);
			}

			DCA_INLINE etf_serializer& operator=(json_type data) {
				destroy();
				switch (data) {
					case json_type::object_t: {
						setValue<json_type::object_t>();
						break;
					}
					case json_type::array_t: {
						setValue<json_type::array_t>();
						break;
					}
					case json_type::string_t: {
						setValue<json_type::string_t>();
						break;
					}
					case json_type::float_t: {
						setValue<json_type::float_t>();
						break;
					}
					case json_type::uint_t: {
						setValue<json_type::uint_t>();
						break;
					}
					case json_type::int_t: {
						setValue<json_type::int_t>();
						break;
					}
					case json_type::bool_t: {
						setValue<json_type::bool_t>();
						break;
					}
					case json_type::null_t: {
						setValue<json_type::null_t>();
						break;
					}
				}
				return *this;
			}

			DCA_INLINE etf_serializer(json_type data) {
				*this = data;
			}

			DCA_INLINE json_type getType() const {
				return type;
			}

			DCA_INLINE operator jsonifier::string_base<uint8_t>() {
				stringReal.clear();
				appendVersion();
				serializeJsonToEtfString(*this);
				return stringReal;
			}

			DCA_INLINE etf_serializer& operator[](typename object_type::key_type&& key) {
				if (type == json_type::null_t) {
					setValue<json_type::object_t>();
				}

				if (type == json_type::object_t) {
					return getObject().operator[](std::forward<typename object_type::key_type>(key));
				}
				throw etf_serialize_error{ "Sorry, but this value's type is not object." };
			}

			DCA_INLINE etf_serializer& operator[](uint_type index) {
				if (type == json_type::null_t) {
					setValue<json_type::array_t>();
				}

				if (type == json_type::array_t) {
					if (index >= getArray().size()) {
						getArray().resize(index + 1);
					}

					return getArray().at(index);
				}
				throw etf_serialize_error{ "Sorry, but this value's type is not array." };
			}

			DCA_INLINE void emplaceBack(etf_serializer&& data) {
				if (type == json_type::null_t) {
					setValue<json_type::array_t>();
				}

				if (type == json_type::array_t) {
					getArray().emplace_back(std::move(data));
					return;
				}
				throw etf_serialize_error{ "Sorry, but this value's type is not array." };
			}

			DCA_INLINE void emplaceBack(const etf_serializer& rhs) {
				if (type == json_type::null_t) {
					setValue<json_type::array_t>();
				}

				if (type == json_type::array_t) {
					getArray().emplace_back(rhs);
					return;
				}
				throw etf_serialize_error{ "Sorry, but this value's type is not array." };
			}

			DCA_INLINE bool_type operator==(const etf_serializer& rhs) const {
				if (rhs.type != type) {
					return false;
				}
				switch (type) {
					case json_type::object_t: {
						return *objectValue == *rhs.objectValue;
					}
					case json_type::array_t: {
						return *arrayValue == *rhs.arrayValue;
					}
					case json_type::string_t: {
						return *stringValue == *rhs.stringValue;
					}
					case json_type::float_t: {
						return *floatValue == *rhs.floatValue;
					}
					case json_type::uint_t: {
						return *uintValue == *rhs.uintValue;
					}
					case json_type::int_t: {
						return *intValue == *rhs.intValue;
					}
					case json_type::bool_t: {
						return *boolValue == *rhs.boolValue;
					}
					case json_type::null_t: {
						break;
					}
				}
				return true;
			}

			DCA_INLINE object_type& getObject() const {
				if (type != json_type::object_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not object!" };
				}
				return *objectValue;
			}

			DCA_INLINE array_type& getArray() const {
				if (type != json_type::array_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not array!" };
				}
				return *arrayValue;
			}

			DCA_INLINE string_type& getString() const {
				if (type != json_type::string_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not string!" };
				}
				return *stringValue;
			}

			DCA_INLINE float_type& getFloat() const {
				if (type != json_type::float_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not float!" };
				}
				return *floatValue;
			}

			DCA_INLINE uint_type& getUint() const {
				if (type != json_type::uint_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not uint!" };
				}
				return *uintValue;
			}

			DCA_INLINE int_type& getInt() const {
				if (type != json_type::int_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not int!" };
				}
				return *intValue;
			}

			DCA_INLINE bool_type& getBool() const {
				if (type != json_type::bool_t) {
					throw etf_serialize_error{ "Sorry, but this value's type is not bool!" };
				}
				return *boolValue;
			}

			DCA_INLINE ~etf_serializer() {
				destroy();
			}

		  protected:
			jsonifier::string_base<uint8_t> stringReal{};
			json_type type{ json_type::null_t };
			union {
				object_type* objectValue;
				array_type* arrayValue;
				string_type* stringValue;
				float_type* floatValue;
				uint_type* uintValue;
				int_type* intValue;
				bool_type* boolValue;
			};

			DCA_INLINE void serializeJsonToEtfString(const etf_serializer& dataToParse) {
				switch (dataToParse.type) {
					case json_type::object_t: {
						return writeEtfObject(dataToParse.getObject());
					}
					case json_type::array_t: {
						return writeEtfArray(dataToParse.getArray());
					}
					case json_type::string_t: {
						return writeEtfString(dataToParse.getString());
					}
					case json_type::float_t: {
						return writeEtfFloat(dataToParse.getFloat());
					}
					case json_type::uint_t: {
						return writeEtfUint(dataToParse.getUint());
					}
					case json_type::int_t: {
						return writeEtfInt(dataToParse.getInt());
					}
					case json_type::bool_t: {
						return writeEtfBool(dataToParse.getBool());
					}
					case json_type::null_t: {
						return writeEtfNull();
					}
				}
			}

			DCA_INLINE void writeEtfObject(const object_type& jsonData) {
				appendMapHeader(static_cast<uint32_t>(jsonData.size()));
				for (auto& [key, valueNew]: jsonData) {
					appendBinaryExt(key, static_cast<uint32_t>(key.size()));
					serializeJsonToEtfString(valueNew);
				}
			}

			DCA_INLINE void writeEtfArray(const array_type& jsonData) {
				appendListHeader(static_cast<uint32_t>(jsonData.size()));
				for (auto& valueNew: jsonData) {
					serializeJsonToEtfString(valueNew);
				}
				appendNilExt();
			}

			DCA_INLINE void writeEtfString(const string_type& jsonData) {
				appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
			}

			DCA_INLINE void writeEtfUint(const uint_type jsonData) {
				if (jsonData <= std::numeric_limits<uint8_t>::max() && jsonData >= std::numeric_limits<uint8_t>::min()) {
					appendUint8(static_cast<uint8_t>(jsonData));
				} else if (jsonData <= std::numeric_limits<uint32_t>::max() && jsonData >= std::numeric_limits<uint32_t>::min()) {
					appendUint32(static_cast<uint32_t>(jsonData));
				} else {
					appendUint64(jsonData);
				}
			}

			DCA_INLINE void writeEtfInt(const int_type jsonData) {
				if (jsonData <= std::numeric_limits<int8_t>::max() && jsonData >= std::numeric_limits<int8_t>::min()) {
					appendInt8(static_cast<int8_t>(jsonData));
				} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
					appendInt32(static_cast<int32_t>(jsonData));
				} else {
					appendInt64(jsonData);
				}
			}

			DCA_INLINE void writeEtfFloat(const float_type jsonData) {
				appendNewFloatExt(jsonData);
			}

			DCA_INLINE void writeEtfBool(const bool_type jsonData) {
				appendBool(jsonData);
			}

			DCA_INLINE void writeEtfNull() {
				appendNil();
			}

			template<typename value_type> DCA_INLINE void writeString(const value_type* data, uint_type length) {
				auto oldSize = stringReal.size();
				stringReal.resize(oldSize + length);
				std::memcpy(stringReal.data() + oldSize, data, length);
			}

			DCA_INLINE void appendBinaryExt(jsonifier::string_view bytes, uint32_t sizeNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Binary_Ext) };
				storeBits(newBuffer + 1, sizeNew);
				writeString(newBuffer, std::size(newBuffer));
				writeString(bytes.data(), bytes.size());
			}

			DCA_INLINE void appendNewFloatExt(const float_type newFloat) {
				uint8_t newBuffer[9]{ static_cast<uint8_t>(etf_type::New_Float_Ext) };
				uint_type newValue{};
				std::memcpy(&newValue, &newFloat, sizeof(newFloat));
				storeBits(newBuffer + 1, newValue);
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendListHeader(const uint32_t sizeNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::List_Ext) };
				storeBits(newBuffer + 1, sizeNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendMapHeader(const uint32_t sizeNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Map_Ext) };
				storeBits(newBuffer + 1, sizeNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendUint64(uint_type valueNew) {
				uint8_t newBuffer[11]{ static_cast<uint8_t>(etf_type::Small_Big_Ext) };
				uint8_t encodedBytes{};
				while (valueNew > 0) {
					newBuffer[3 + encodedBytes] = static_cast<uint8_t>(valueNew & 0xFF);
					valueNew >>= 8;
					++encodedBytes;
				}
				newBuffer[1] = encodedBytes;
				newBuffer[2] = 0;
				writeString(newBuffer, 1ull + 2ull + static_cast<uint_type>(encodedBytes));
			}

			DCA_INLINE void appendInt64(int_type valueNew) {
				uint8_t newBuffer[11]{ static_cast<uint8_t>(etf_type::Small_Big_Ext) };
				uint8_t encodedBytes{};
				while (valueNew > 0) {
					newBuffer[3 + encodedBytes] = static_cast<uint8_t>(valueNew & 0xFF);
					valueNew >>= 8;
					++encodedBytes;
				}
				newBuffer[1] = encodedBytes;
				if (valueNew >= 0) {
					newBuffer[2] = 0;
				} else {
					newBuffer[2] = 1;
				}
				writeString(newBuffer, 1ull + 2ull + static_cast<uint_type>(encodedBytes));
			}

			DCA_INLINE void appendUint32(const uint32_t valueNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Integer_Ext) };
				storeBits(newBuffer + 1, valueNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendInt32(const int32_t valueNew) {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Integer_Ext) };
				storeBits(newBuffer + 1, valueNew);
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendUint8(const uint8_t valueNew) {
				uint8_t newBuffer[2]{ static_cast<uint8_t>(etf_type::Small_Integer_Ext), static_cast<uint8_t>(valueNew) };
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendInt8(const int8_t valueNew) {
				uint8_t newBuffer[2]{ static_cast<uint8_t>(etf_type::Small_Integer_Ext), static_cast<uint8_t>(valueNew) };
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendBool(bool_type data) {
				if (data) {
					uint8_t newBuffer[6]{ static_cast<uint8_t>(etf_type::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
					writeString(newBuffer, std::size(newBuffer));

				} else {
					uint8_t newBuffer[7]{ static_cast<uint8_t>(etf_type::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
					writeString(newBuffer, std::size(newBuffer));
				}
			}

			DCA_INLINE void appendVersion() {
				uint8_t newBuffer[1]{ static_cast<uint8_t>(formatVersion) };
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendNilExt() {
				uint8_t newBuffer[1]{ static_cast<uint8_t>(etf_type::Nil_Ext) };
				writeString(newBuffer, std::size(newBuffer));
			}

			DCA_INLINE void appendNil() {
				uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
				writeString(newBuffer, std::size(newBuffer));
			}

			template<json_type typeNew, typename... value_types> DCA_INLINE void setValue(value_types&&... args) {
				destroy();
				type = typeNew;
				if constexpr (typeNew == json_type::object_t) {
					allocator<object_type> alloc{};
					objectValue = alloc.allocate(1);
					alloc.construct(objectValue, std::forward<value_types>(args)...);
				} else if constexpr (typeNew == json_type::array_t) {
					allocator<array_type> alloc{};
					arrayValue = alloc.allocate(1);
					alloc.construct(arrayValue, std::forward<value_types>(args)...);
				} else if constexpr (typeNew == json_type::string_t) {
					allocator<string_type> alloc{};
					stringValue = alloc.allocate(1);
					alloc.construct(stringValue, std::forward<value_types>(args)...);
				} else if constexpr (typeNew == json_type::float_t) {
					allocator<float_type> alloc{};
					floatValue = alloc.allocate(1);
					alloc.construct(floatValue, std::forward<value_types>(args)...);
				} else if constexpr (typeNew == json_type::uint_t) {
					allocator<uint_type> alloc{};
					uintValue = alloc.allocate(1);
					alloc.construct(uintValue, std::forward<value_types>(args)...);
				} else if constexpr (typeNew == json_type::int_t) {
					allocator<int_type> alloc{};
					intValue = alloc.allocate(1);
					alloc.construct(intValue, std::forward<value_types>(args)...);
				} else if constexpr (typeNew == json_type::bool_t) {
					allocator<bool_type> alloc{};
					boolValue = alloc.allocate(1);
					alloc.construct(boolValue, std::forward<value_types>(args)...);
				}
			}

			DCA_INLINE void destroy() {
				switch (type) {
					case json_type::object_t: {
						allocator<object_type> alloc{};
						alloc.destroy(objectValue);
						alloc.deallocate(static_cast<object_type*>(objectValue));
						objectValue = nullptr;
						break;
					}
					case json_type::array_t: {
						allocator<array_type> alloc{};
						alloc.destroy(arrayValue);
						alloc.deallocate(static_cast<array_type*>(arrayValue));
						arrayValue = nullptr;
						break;
					}
					case json_type::string_t: {
						allocator<string_type> alloc{};
						alloc.destroy(stringValue);
						alloc.deallocate(static_cast<string_type*>(stringValue));
						stringValue = nullptr;
						break;
					}
					case json_type::float_t: {
						allocator<float_type> alloc{};
						alloc.destroy(floatValue);
						alloc.deallocate(static_cast<float_type*>(floatValue));
						floatValue = nullptr;
						break;
					}
					case json_type::uint_t: {
						allocator<uint_type> alloc{};
						alloc.destroy(uintValue);
						alloc.deallocate(static_cast<uint_type*>(uintValue));
						uintValue = nullptr;
						break;
					}
					case json_type::int_t: {
						allocator<int_type> alloc{};
						alloc.destroy(intValue);
						alloc.deallocate(static_cast<int_type*>(intValue));
						intValue = nullptr;
						break;
					}
					case json_type::bool_t: {
						allocator<bool_type> alloc{};
						alloc.destroy(boolValue);
						alloc.deallocate(static_cast<bool_type*>(boolValue));
						boolValue = nullptr;
						break;
					}
					case json_type::null_t: {
						break;
					}
					default: {
						break;
					}
				}
				type = json_type::null_t;
			}
		};

		/**@}*/

	};

	DCA_INLINE thread_local discord_core_internal::etf_parser etfParser{};
}// namespace discord_core_internal