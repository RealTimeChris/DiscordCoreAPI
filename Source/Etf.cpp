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
/// Etf.cpp - Source file for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file Etf.cpp

#include <discordcoreapi/Etf.hpp>
#include <codecvt>

namespace DiscordCoreInternal {

	EtfParseError::EtfParseError(const std::string& message) : DCAException(message){};

	std::string_view EtfParser::parseEtfToJson(std::string_view dataToParse) {
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

	void EtfParser::writeCharacters(const char* data, uint64_t length) {
		if (finalString.size() < currentSize + length) {
			finalString.resize((finalString.size() + length) * 2);
		}
		std::memcpy(finalString.data() + currentSize, data, length);
		currentSize += length;
	}

	uint8_t EtfParser::hex2dec(char hex) {
		return ((hex & 0xf) + (hex >> 6) * 9);
	}

	char32_t EtfParser::hex4ToChar32(const char* hex) {
		uint32_t value = hex2dec(hex[3]);
		value |= hex2dec(hex[2]) << 8;
		value |= hex2dec(hex[1]) << 16;
		value |= hex2dec(hex[0]) << 24;
		return value;
	}

	template<typename ValueType> void EtfParser::readEscapedUnicode(ValueType& value, auto& it) {
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

	void EtfParser::writeCharactersFromBuffer(uint32_t length) {
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

	void EtfParser::writeCharacter(const char value) {
		if (finalString.size() < currentSize + 1) {
			finalString.resize((finalString.size() + 1) * 2);
		}
		finalString[currentSize++] = value;
	}

	void EtfParser::singleValueETFToJson() {
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

	void EtfParser::parseListExt() {
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

	void EtfParser::parseSmallIntegerExt() {
		auto string = std::to_string(readBitsFromBuffer<uint8_t>());
		writeCharacters(string.data(), string.size());
	}

	void EtfParser::parseIntegerExt() {
		auto string = std::to_string(readBitsFromBuffer<uint32_t>());
		writeCharacters(string.data(), string.size());
	}

	void EtfParser::parseStringExt() {
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

	void EtfParser::parseNewFloatExt() {
		uint64_t value = readBitsFromBuffer<uint64_t>();
		auto newDouble = *reinterpret_cast<double*>(&value);
		std::string valueNew = std::to_string(newDouble);
		writeCharacters(valueNew.data(), valueNew.size());
	}

	void EtfParser::parseSmallBigExt() {
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

	void EtfParser::parseAtomExt() {
		writeCharactersFromBuffer(readBitsFromBuffer<uint16_t>());
	}

	void EtfParser::parseBinaryExt() {
		writeCharactersFromBuffer(readBitsFromBuffer<uint32_t>());
	}

	void EtfParser::parseNilExt() {
		writeCharacters("[]", 2);
	}

	void EtfParser::parseSmallAtomExt() {
		writeCharactersFromBuffer(readBitsFromBuffer<uint8_t>());
	}

	void EtfParser::parseMapExt() {
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

	EtfSerializeError::EtfSerializeError(const std::string& message, std::source_location location) : DCAException(message, location){};

	EtfSerializer::operator std::string&&() noexcept {
		return std::move(stringReal);
	}

	EtfSerializer::operator std::string() noexcept {
		return stringReal;
	}

	JsonType EtfSerializer::getType() noexcept {
		return type;
	}

	void EtfSerializer::refreshString() {
		stringReal.clear();
		appendVersion();
		serializeJsonToEtfString(this);
	}

	EtfSerializer& EtfSerializer::operator[](typename ObjectType::key_type key) {
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

	EtfSerializer& EtfSerializer::operator[](uint64_t index) {
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

	void EtfSerializer::emplaceBack(EtfSerializer&& other) {
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

	void EtfSerializer::emplaceBack(EtfSerializer& other) {
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

	void EtfSerializer::serializeJsonToEtfString(const EtfSerializer* dataToParse) {
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

	void EtfSerializer::writeEtfObject(const ObjectType& jsonData) {
		appendMapHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& [key, value]: jsonData) {
			appendBinaryExt(key, static_cast<uint32_t>(key.size()));
			serializeJsonToEtfString(&value);
		}
	}

	void EtfSerializer::writeEtfArray(const ArrayType& jsonData) {
		appendListHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& value: jsonData) {
			serializeJsonToEtfString(&value);
		}
		appendNilExt();
	}

	void EtfSerializer::writeEtfString(const StringType& jsonData) {
		appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
	}

	void EtfSerializer::writeEtfUint(const UintType jsonData) {
		if (jsonData <= std::numeric_limits<uint8_t>::max() && jsonData >= std::numeric_limits<uint8_t>::min()) {
			appendUint8(static_cast<uint8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<uint32_t>::max() && jsonData >= std::numeric_limits<uint32_t>::min()) {
			appendUint32(static_cast<uint32_t>(jsonData));
		} else {
			appendUint64(jsonData);
		}
	}

	void EtfSerializer::writeEtfInt(const IntType jsonData) {
		if (jsonData <= std::numeric_limits<int8_t>::max() && jsonData >= std::numeric_limits<int8_t>::min()) {
			appendInt8(static_cast<int8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
			appendInt32(static_cast<int32_t>(jsonData));
		} else {
			appendInt64(jsonData);
		}
	}

	void EtfSerializer::writeEtfFloat(const FloatType jsonData) {
		appendNewFloatExt(jsonData);
	}

	void EtfSerializer::writeEtfBool(const BoolType jsonData) {
		appendBool(jsonData);
	}

	void EtfSerializer::writeEtfNull() {
		appendNil();
	}

	void EtfSerializer::writeString(const char* data, uint64_t length) {
		stringReal.append(data, length);
	}

	void EtfSerializer::writeCharacter(const char charValue) {
		stringReal.push_back(charValue);
	}

	bool operator==(const EtfSerializer& lhs, const EtfSerializer& rhs) {
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

	void EtfSerializer::appendBinaryExt(const std::string& bytes, uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Binary_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		writeString(newBuffer, std::size(newBuffer));
		writeString(bytes.data(), bytes.size());
	}

	void EtfSerializer::appendNewFloatExt(const double FloatValue) {
		char newBuffer[9]{ static_cast<uint8_t>(EtfType::New_Float_Ext) };
		const void* punner{ &FloatValue };
		storeBits(newBuffer + 1, *static_cast<const uint64_t*>(punner));
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendListHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::List_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendMapHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Map_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendUint64(uint64_t value) {
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

	void EtfSerializer::appendInt64(int64_t value) {
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

	void EtfSerializer::appendUint32(const uint32_t value) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
		storeBits(newBuffer + 1, value);
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendInt32(const int32_t value) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
		storeBits(newBuffer + 1, value);
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendUint8(const uint8_t value) {
		char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendInt8(const int8_t value) {
		char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendBool(bool data) {
		if (data) {
			char newBuffer[6]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
			writeString(newBuffer, std::size(newBuffer));

		} else {
			char newBuffer[7]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
			writeString(newBuffer, std::size(newBuffer));
		}
	}

	void EtfSerializer::appendVersion() {
		char newBuffer[1]{ static_cast<int8_t>(formatVersion) };
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::appendNilExt() {
		writeCharacter(static_cast<uint8_t>(EtfType::Nil_Ext));
	}

	void EtfSerializer::appendNil() {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
		writeString(newBuffer, std::size(newBuffer));
	}

	void EtfSerializer::setValue(JsonType typeNew) {
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

	void EtfSerializer::destroy() noexcept {
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

	EtfSerializer::~EtfSerializer() noexcept {
		destroy();
	}
}
