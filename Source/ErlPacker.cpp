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

	ErlPackError::ErlPackError(const std::string& message) : std::runtime_error(message.c_str()){};

	ErlPacker::ErlPacker() : buffer(&bufferRef){};

	ErlPacker& ErlPacker::operator=(std::string& theBuffer) {
		this->buffer.release();
		this->buffer.reset(&theBuffer);
		return *this;
	}

	ErlPacker::ErlPacker(std::string& theBuffer) : buffer(&theBuffer){};

	std::string ErlPacker::parseJsonToEtf(nlohmann::json& dataToParse) {
		std::string theString{};
		this->buffer.release();
		this->buffer.reset(&theString);
		this->offSet = 0;
		ErlPacker::appendVersion();
		ErlPacker::singleValueJsonToETF(dataToParse);
		return *this->buffer;
	}

	nlohmann::json ErlPacker::parseEtfToJson(std::string&& dataToParse) {
		this->offSet = 0;
		this->buffer.release();
		this->buffer.reset(&dataToParse);
		uint8_t version = ErlPacker::read8Bits();
		return ErlPacker::singleValueETFToJson();
	}

	ErlPacker::~ErlPacker() {
		this->buffer.release();
	}

	void ErlPacker::singleValueJsonToETF(nlohmann::json& jsonData) {
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
		if (this->offSet > this->buffer->size()) {
			this->buffer->resize(this->offSet);
		}
		this->buffer->insert(this->buffer->begin() + this->offSet, bytes.begin(), bytes.end());
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
		DiscordCoreAPI::store32Bits(bufferNew, value);
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFloatExt(double value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Float_Ext) };
		void* doubleValue{ &value };
		DiscordCoreAPI::store64Bits(bufferNew, *static_cast<uint64_t*>(doubleValue));
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
		DiscordCoreAPI::store32Bits(bufferNew, size);
		ErlPacker::writeToBuffer(bufferNew);
		ErlPacker::writeToBuffer(bytes);
	}

	void ErlPacker::appendListHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::store32Bits(bufferNew, size);
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendMapHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::store32Bits(bufferNew, size);
		ErlPacker::writeToBuffer(bufferNew);
	}

	uint8_t ErlPacker::read8Bits() {
		if (this->offSet + sizeof(uint8_t) > this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer->\n\n" };
		}
		uint8_t newValue = *reinterpret_cast<uint8_t*>(this->buffer->data() + this->offSet);
		this->offSet += sizeof(uint8_t);
		return newValue;
	}

	uint16_t ErlPacker::read16Bits() {
		if (this->offSet + sizeof(uint16_t) > this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer->\n\n" };
		}
		uint16_t newValue = *reinterpret_cast<uint16_t*>(this->buffer->data() + this->offSet);
		this->offSet += sizeof(uint16_t);
		return DiscordCoreAPI::ntohostshort(newValue);
	}

	uint32_t ErlPacker::read32Bits() {
		if (this->offSet + sizeof(uint32_t) > this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer->\n\n" };
		}
		uint32_t newValue = *reinterpret_cast<uint32_t*>(this->buffer->data() + this->offSet);
		this->offSet += sizeof(uint32_t);
		return DiscordCoreAPI::ntohostint(newValue);
	}

	uint64_t ErlPacker::read64Bits() {
		if (this->offSet + sizeof(uint64_t) > this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer->\n\n" };
		}
		uint64_t newValue = *reinterpret_cast<uint64_t*>(this->buffer->data() + this->offSet);
		this->offSet += sizeof(uint64_t);
		return DiscordCoreAPI::ntohostlong(newValue);
	}

	const char* ErlPacker::readString(uint32_t length) {
		if (this->offSet + static_cast<uint64_t>(length) > this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::readString() Error: readString() past end of buffer->\n\n" };
		}
		const char* theStringNew = this->buffer->data() + this->offSet;
		this->offSet += length;
		return theStringNew;
	}

	nlohmann::json ErlPacker::singleValueETFToJson() {
		if (this->offSet >= this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer->\n\n" };
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
		nlohmann::json theValue = ErlPacker::read8Bits();
		return theValue;
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
				return std::to_string(value);
			}
			const bool isSignBitAvailable = (value & 1ull << 31ull) == 0;
			if (isSignBitAvailable) {
				return std::to_string(-static_cast<int32_t>(value));
			}
		}
		char outBuffer[32] = { 0 };
		const char* formatString = sign == 0 ? "%llu" : "-%ll";
		const int32_t res = sprintf(outBuffer, formatString, value);
		if (res < 0) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Parse big integer failed.\n\n" };
		}
		const uint8_t length = static_cast<uint8_t>(res);
		nlohmann::json theValue = std::string{ outBuffer, length };
		return theValue;
	}

	nlohmann::json ErlPacker::parseIntegerExt() {
		nlohmann::json theValue = ErlPacker::read32Bits();
		return theValue;
	}

	nlohmann::json ErlPacker::parseFloatExt() {
		uint32_t floatLength = 31;
		auto theStringNew = ErlPacker::readString(floatLength);
		if (theStringNew == nullptr) {
			return nlohmann::json{};
		}
		double number{};
		std::string nullTerminated{ theStringNew, floatLength };
		auto count = sscanf(nullTerminated.data(), "%lf", &number);
		if (!count) {
			return nlohmann::json{};
		}
		nlohmann::json theValue = number;
		return theValue;
	}

	nlohmann::json ErlPacker::processAtom(const char* atom, uint32_t length) {
		if (atom == nullptr) {
			return nlohmann::json{};
		}
		if (length >= 3 && length <= 5) {
			if (length == 3 && strncmp(atom, "nil", 3) == 0) {
				return nlohmann::json{};
			} else if (length == 4 && strncmp(atom, "null", 4) == 0) {
				return nlohmann::json{};
			} else if (length == 4 && strncmp(atom, "true", 4) == 0) {
				return true;
			} else if (length == 5 && strncmp(atom, "false", 5) == 0) {
				return false;
			}
		}
		nlohmann::json theValue = std::string{ atom, length };
		return theValue;
	}

	nlohmann::json ErlPacker::parseTuple(const uint32_t length) {
		nlohmann::json theValue = ErlPacker::parseArray(length);
		return theValue;
	}

	nlohmann::json ErlPacker::parseSmallTupleExt() {
		nlohmann::json theValue = ErlPacker::parseTuple(ErlPacker::read8Bits());
		return theValue;
	}

	nlohmann::json ErlPacker::parseLargeTupleExt() {
		nlohmann::json theValue = ErlPacker::parseTuple(ErlPacker::read32Bits());
		return theValue;
	}

	nlohmann::json ErlPacker::parseNilExt() {
		return nlohmann::json();
	}

	nlohmann::json ErlPacker::parseStringAsList() {
		uint16_t length = ErlPacker::read16Bits();
		nlohmann::json theArray = nlohmann::json::array();
		if (static_cast<uint64_t>(this->offSet) + length > this->buffer->size()) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer->\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.emplace_back(ErlPacker::parseSmallIntegerExt());
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
		auto stringNew = ErlPacker::readString(length);
		if (stringNew == nullptr) {
			return nlohmann::json{};
		}
		nlohmann::json theValue = std::string{ stringNew, length };
		return theValue;
	}

	nlohmann::json ErlPacker::parseSmallBigExt() {
		nlohmann::json theValue = ErlPacker::parseBigint(ErlPacker::read8Bits());
		return theValue;
	}

	nlohmann::json ErlPacker::parseLargeBigExt() {
		nlohmann::json theValue = ErlPacker::parseBigint(ErlPacker::read32Bits());
		return theValue;
	}

	nlohmann::json ErlPacker::parseArray(const uint32_t length) {
		nlohmann::json array = nlohmann::json::array();
		for (uint32_t x = 0; x < length; ++x) {
			array.emplace_back(ErlPacker::singleValueETFToJson());
		}
		return array;
	}

	nlohmann::json ErlPacker::parseMapExt() {
		uint32_t length = read32Bits();
		auto map = nlohmann::json::object();
		for (uint32_t i = 0; i < length; ++i) {
			auto key = singleValueETFToJson();
			auto value = singleValueETFToJson();
			if (key.is_number()) {
				map[std::to_string(key.get<uint64_t>())] = std::move(value);
			} else {
				map[key.get<std::string>()] = std::move(value);
			}
		}
		return map;
	}

	nlohmann::json ErlPacker::parseAtomUtf8Ext() {
		uint32_t lengthNew = ErlPacker::read16Bits();
		auto atom = ErlPacker::readString(lengthNew);
		nlohmann::json theValue = ErlPacker::processAtom(atom, lengthNew);
		return theValue;
	}

	nlohmann::json ErlPacker::parseSmallAtomUtf8Ext() {
		uint8_t length = ErlPacker::read8Bits();
		auto atom = ErlPacker::readString(length);
		nlohmann::json theValue = ErlPacker::processAtom(atom, length);
		return theValue;
	}

}
