/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// ErlPacker.cpp - Source file for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.cpp

#include <discordcoreapi/ErlPacker.hpp>

namespace DiscordCoreInternal {

	uint64_t ntohostlong(uint64_t const net) {
		uint8_t data[8] = {};
		memcpy(&data, &net, sizeof(net));

		return (( uint64_t )data[7] << 0) | (( uint64_t )data[6] << 8) | (( uint64_t )data[5] << 16) | (( uint64_t )data[4] << 24) | (( uint64_t )data[3] << 32) |
			(( uint64_t )data[2] << 40) | (( uint64_t )data[1] << 48) | (( uint64_t )data[0] << 56);
	}

	uint32_t ntohostint(uint32_t const net) {
		uint8_t data[4] = {};
		memcpy(&data, &net, sizeof(net));

		return (( uint32_t )data[3] << 0) | (( uint32_t )data[2] << 8) | (( uint32_t )data[1] << 16) | (( uint32_t )data[0] << 24);
	}

	uint16_t ntohostshort(uint16_t const net) {
		uint8_t data[4] = {};
		memcpy(&data, &net, sizeof(net));

		return (( uint16_t )data[1] << 0) | (( uint16_t )data[0] << 8);
	}

	ErlPackError::ErlPackError(const std::string& message) : std::runtime_error(message.c_str()){};

	ErlPacker::ErlPacker() : buffer(this->theBufferRef) {}

	ErlPacker& ErlPacker::operator=(std::string&theBuffer) {
		this->buffer = theBuffer;
		return *this;
	}

	ErlPacker::ErlPacker(std::string& theBuffer) : buffer(theBuffer) {}

	std::string ErlPacker::parseJsonToEtf(const nlohmann::json& dataToParse) {
		std::string theString{};
		this->buffer = theString;
		this->offSet = 0;
		ErlPacker::appendVersion();
		ErlPacker::singleValueJsonToETF(dataToParse);
		return buffer;
	}

	nlohmann::json ErlPacker::parseEtfToJson(std::string& dataToParse) {
		this->offSet = 0;
		this->buffer = dataToParse;
		uint8_t version = ErlPacker::read8Bits();
		return ErlPacker::singleValueETFToJson();
	}

	void ErlPacker::singleValueJsonToETF(const nlohmann::json& jsonData) {
		if (jsonData.is_array()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length == 0) {
				ErlPacker::appendNilExt();
			} else {
				if (length > std::numeric_limits<uint32_t>::max() - 1) {
					throw ErlPackError{ "ErlPacker::singleValueJsonToETF() Error: List too large for ETF.\n\n" };
				}
			}
			ErlPacker::appendListHeader(length);
			for (uint64_t index = 0; index < length; ++index) {
				ErlPacker::singleValueJsonToETF(jsonData[index]);
			}
			ErlPacker::appendNilExt();
		} else if (jsonData.is_object()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length > std::numeric_limits<uint32_t>::max() - 1) {
				throw ErlPackError{ "ErlPacker::singleValueJsonToETF() Error: Map too large for ETF.\n\n" };
			}
			ErlPacker::appendMapHeader(length);
			for (auto n = jsonData.begin(); n != jsonData.end(); ++n) {
				nlohmann::json jstr = n.key();
				ErlPacker::singleValueJsonToETF(jstr);
				ErlPacker::singleValueJsonToETF(n.value());
			}
		} else if (jsonData.is_number_integer()) {
			uint64_t numberOld = jsonData.get<uint64_t>();
			if (numberOld <= 127) {
				uint8_t number = jsonData.get<uint8_t>();
				ErlPacker::appendSmallIntegerExt(number);
			} else if (jsonData.is_number_unsigned() && (numberOld >= std::numeric_limits<uint32_t>::max() - static_cast<size_t>(1))) {
				uint64_t number = jsonData.get<uint64_t>();
				ErlPacker::appendUnsignedLongLong(number);
			} else {
				uint32_t number = jsonData.get<uint32_t>();
				ErlPacker::appendIntegerExt(number);
			}
		} else if (jsonData.is_boolean()) {
			if (jsonData.get<bool>()) {
				ErlPacker::appendTrue();
			} else {
				ErlPacker::appendFalse();
			}
		} else if (jsonData.is_string()) {
			std::string newString = jsonData.get<std::string>();
			std::string newVector{};
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			uint32_t newValue = static_cast<uint32_t>(newVector.size());
			ErlPacker::appendBinaryExt(newVector, newValue);
		} else if (jsonData.is_number_float()) {
			double newValue = jsonData.get<double>();
			ErlPacker::appendFloatExt(newValue);
		} else if (jsonData.is_null()) {
			ErlPacker::appendNil();
		}
	}

	void ErlPacker::writeToBuffer(const std::string& bytes) {
		this->buffer.insert(this->buffer.begin() + this->offSet, bytes.begin(), bytes.end());
		this->offSet += static_cast<uint32_t>(bytes.size());
	}

	void ErlPacker::appendVersion() {
		std::string bufferNew{ static_cast<char>(formatVersion) };
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendSmallIntegerExt(uint8_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendIntegerExt(uint32_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Integer_Ext) };
		uint32_t newValue{ 1 };
		DiscordCoreAPI::storeBits(bufferNew, value);
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFloatExt(double& value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Float_Ext) };
		void* doubleValue{ &value };
		DiscordCoreAPI::storeBits(bufferNew, *static_cast<uint64_t*>(doubleValue));
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNilExt() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Nil_Ext) };
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNil() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 3, static_cast<uint8_t>('n'), static_cast<uint8_t>('i'), static_cast<uint8_t>('l') };
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFalse() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 5, static_cast<uint8_t>('f'), static_cast<uint8_t>('a'), static_cast<uint8_t>('l'),
			static_cast<uint8_t>('s'), static_cast<uint8_t>('e') };
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendTrue() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 4, static_cast<uint8_t>('t'), static_cast<uint8_t>('r'), static_cast<uint8_t>('u'),
			static_cast<uint8_t>('e') };
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendUnsignedLongLong(uint64_t value) {
		std::string bufferNew{};
		bufferNew.resize(static_cast<uint64_t>(1) + 2 + sizeof(uint64_t));
		bufferNew[0] = static_cast<uint8_t>(ETFTokenType::Small_Big_Ext);
		uint8_t bytesToEncode = 0;
		while (value > 0) {
			bufferNew[static_cast<size_t>(3) + bytesToEncode] = value & 0xF;
			value >>= 8;
			bytesToEncode++;
		}
		bufferNew[1] = bytesToEncode;
		bufferNew[2] = 0;
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendBinaryExt(const std::string& bytes, uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Binary_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, size);
		ErlPacker::writeToBuffer(bufferNew);
		ErlPacker::writeToBuffer(bytes);
	}

	void ErlPacker::appendListHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, size);
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendMapHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, size);
		ErlPacker::writeToBuffer(bufferNew);
	}

	uint8_t ErlPacker::read8Bits() {
		if (this->offSet + sizeof(uint8_t) > this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of buffer.\n\n" };
		}
		uint8_t newValue = *reinterpret_cast<uint8_t*>(this->buffer.data() + this->offSet);
		this->offSet += sizeof(uint8_t);
		return newValue;
	}

	uint16_t ErlPacker::read16Bits() {
		if (this->offSet + sizeof(uint16_t) > this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of buffer.\n\n" };
		}
		uint16_t newValue = *reinterpret_cast<uint16_t*>(this->buffer.data() + this->offSet);
		newValue = ntohostshort(newValue);
		this->offSet += sizeof(uint16_t);
		return newValue;
	}

	uint32_t ErlPacker::read32Bits() {
		if (this->offSet + sizeof(uint32_t) > this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of buffer.\n\n" };
		}
		uint32_t newValue = *reinterpret_cast<uint32_t*>(this->buffer.data() + this->offSet);
		newValue = ntohostint(newValue);
		this->offSet += sizeof(uint32_t);
		return newValue;
	}

	uint64_t ErlPacker::read64Bits() {
		if (this->offSet + sizeof(uint64_t) > this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of buffer.\n\n" };
		}
		uint64_t newValue = *reinterpret_cast<uint64_t*>(this->buffer.data() + this->offSet);
		newValue = ntohostlong(newValue);
		this->offSet += sizeof(uint64_t);
		return newValue;
	}

	void ErlPacker::readString(uint32_t length, std::string& theString) {
		if (this->offSet + static_cast<uint64_t>(length) > this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::readString() Error: readString() past end of buffer.\n\n" };
		}
		std::string string{};
		string.insert(string.begin(), this->buffer.begin() + this->offSet, this->buffer.begin() + this->offSet + length);
		this->offSet += length;
		theString = std::move(string);
	}

	nlohmann::json ErlPacker::singleValueETFToJson() {
		if (this->offSet >= this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		uint8_t type = ErlPacker::read8Bits();
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return ErlPacker::parseSmallIntegerExt();
			}
			case ETFTokenType::Integer_Ext: {
				return ErlPacker::parseIntegerExt();
			}
			case ETFTokenType::Float_Ext: {
				return ErlPacker::parseFloatExt();
			}
			case ETFTokenType::Atom_Ext: {
				return ErlPacker::parseAtomUtf8Ext();
			}
			case ETFTokenType::Small_Tuple_Ext: {
				return ErlPacker::parseSmallTupleExt();
			}
			case ETFTokenType::Large_Tuple_Ext: {
				return ErlPacker::parseLargeTupleExt();
			}
			case ETFTokenType::Nil_Ext: {
				return ErlPacker::parseNilExt();
			}
			case ETFTokenType::String_Ext: {
				return ErlPacker::parseStringAsList();
			}
			case ETFTokenType::List_Ext: {
				return ErlPacker::parseListExt();
			}
			case ETFTokenType::Binary_Ext: {
				return ErlPacker::parseBinaryExt();
			}
			case ETFTokenType::Small_Big_Ext: {
				return ErlPacker::parseSmallBigExt();
			}
			case ETFTokenType::Large_Big_Ext: {
				return ErlPacker::parseLargeBigExt();
			}
			case ETFTokenType::Map_Ext: {
				return ErlPacker::parseMapExt();
			}
			case ETFTokenType::Atom_Utf8_Ext: {
				return ErlPacker::parseAtomUtf8Ext();
			}
			case ETFTokenType::Small_Atom_Utf8_Ext: {
				return ErlPacker::parseSmallAtomUtf8Ext();
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	nlohmann::json ErlPacker::parseSmallIntegerExt() {
		uint8_t newValue = ErlPacker::read8Bits();
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseBigint(uint32_t digits) {
		uint8_t sign = ErlPacker::read8Bits();
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t x = 0; x < digits; ++x) {
			uint8_t digit = ErlPacker::read8Bits();
			uint64_t digitNew = digit;
			value += digitNew * b;
			b <<= 8;
		}
		if (digits <= 4) {
			if (sign == 0) {
				nlohmann::json jsonData = std::to_string(value);
				return jsonData;
			}
			const bool isSignBitAvailable = (value & 1ull << 31ull) == 0;
			if (isSignBitAvailable) {
				nlohmann::json jsonData = std::to_string(-static_cast<int32_t>(value));
				return jsonData;
			}
		}
		char outBuffer[32] = { 0 };
		const char* formatString = sign == 0 ? "%llu" : "-%ll";
		const int32_t res = sprintf(outBuffer, formatString, value);
		if (res < 0) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Parse big integer failed.\n\n" };
		}
		const uint8_t length = static_cast<uint8_t>(res);
		nlohmann::json jsonData = std::string(outBuffer, length);
		return jsonData;
	}

	nlohmann::json ErlPacker::parseIntegerExt() {
		uint32_t newValue = ErlPacker::read32Bits();
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseFloatExt() {
		uint32_t floatLength = 31;
		std::string floatStr{};
		nlohmann::json jsonData{};
		ErlPacker::readString(floatLength, floatStr);
		if (floatStr.size() == 0) {
			return jsonData;
		}
		double number{};
		std::vector<char> nullTerminated{};
		nullTerminated.insert(nullTerminated.begin(), floatStr.begin(), floatStr.begin() + floatLength);
		auto count = sscanf(nullTerminated.data(), "%lf", &number);
		if (!count) {
			return jsonData;
		}
		jsonData = number;
		return jsonData;
	}

	nlohmann::json ErlPacker::processAtom(const std::string& atom, uint32_t length) {
		nlohmann::json jsonData{};
		if (atom.size() == 0) {
			return jsonData;
		}
		if (length >= 3 && length <= 5) {
			if (length == 3 && strncmp(atom.data(), "nil", 3) == 0) {
				return jsonData;
			} else if (length == 4 && strncmp(atom.data(), "null", 4) == 0) {
				return jsonData;
			} else if (length == 4 && strncmp(atom.data(), "true", 4) == 0) {
				jsonData = true;
				return jsonData;
			} else if (length == 5 && strncmp(atom.data(), "false", 5) == 0) {
				jsonData = false;
				return jsonData;
			}
		}
		jsonData = std::string(atom.data(), length);
		return jsonData;
	}

	nlohmann::json ErlPacker::parseTuple(const uint32_t length) {
		return ErlPacker::parseArray(length);
	}

	nlohmann::json ErlPacker::parseSmallTupleExt() {
		uint8_t theValue = ErlPacker::read8Bits();
		uint32_t theValueNew = theValue;
		return ErlPacker::parseTuple(theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeTupleExt() {
		uint32_t theValue = ErlPacker::read32Bits();
		return ErlPacker::parseTuple(theValue);
	}

	nlohmann::json ErlPacker::parseNilExt() {
		return nlohmann::json();
	}

	nlohmann::json ErlPacker::parseStringAsList() {
		uint16_t length = ErlPacker::read16Bits();
		nlohmann::json theArray = nlohmann::json::array();
		if (static_cast<uint64_t>(this->offSet) + length > this->buffer.size()) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.push_back(ErlPacker::parseSmallIntegerExt());
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseListExt() {
		uint32_t length = ErlPacker::read32Bits();
		nlohmann::json::array_t theArray = ErlPacker::parseArray(length);
		uint8_t theValue = ErlPacker::read8Bits();
		const ETFTokenType tailMarker = static_cast<ETFTokenType>(theValue);
		if (tailMarker != ETFTokenType::Nil_Ext) {
			return nlohmann::json::array_t();
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseBinaryExt() {
		uint32_t length = ErlPacker::read32Bits();
		std::string stringNew{};
		ErlPacker::readString(length, stringNew);
		if (stringNew.size() == 0) {
			nlohmann::json jsonData{};
			return jsonData;
		}
		nlohmann::json jsonData = std::string(stringNew.data(), length);
		return jsonData;
	}

	nlohmann::json ErlPacker::parseSmallBigExt() {
		uint8_t theValue = ErlPacker::read8Bits();
		uint32_t theValueNew = theValue;
		return ErlPacker::parseBigint(theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeBigExt() {
		uint32_t theValue = ErlPacker::read32Bits();
		return ErlPacker::parseBigint(theValue);
	}

	nlohmann::json ErlPacker::parseArray(const uint32_t length) {
		nlohmann::json array = nlohmann::json::array();
		for (uint32_t x = 0; x < length; ++x) {
			array.emplace_back(ErlPacker::singleValueETFToJson());
		}
		return array;
	}

	nlohmann::json ErlPacker::parseMapExt() {
		uint32_t length = ErlPacker::read32Bits();
		std::unordered_map<nlohmann::json, nlohmann::json> theMap{};
		for (uint32_t x = 0; x < length; ++x) {
			const nlohmann::json key = ErlPacker::singleValueETFToJson();
			const nlohmann::json value = ErlPacker::singleValueETFToJson();
			theMap[key] = value;
		}
		return theMap;
	}

	nlohmann::json ErlPacker::parseAtomUtf8Ext() {
		uint16_t length = ErlPacker::read16Bits();
		uint32_t lengthNew = length;
		std::string atom{};
		ErlPacker::readString(lengthNew, atom);
		return ErlPacker::processAtom(atom, lengthNew);
	}

	nlohmann::json ErlPacker::parseSmallAtomUtf8Ext() {
		uint8_t length = ErlPacker::read8Bits();
		uint32_t lengthNew = length;
		std::string atom{};
		ErlPacker::readString(lengthNew, atom);
		return ErlPacker::processAtom(atom, lengthNew);
	}

}
