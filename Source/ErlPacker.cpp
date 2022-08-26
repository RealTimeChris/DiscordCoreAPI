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
#include <xmmintrin.h>

namespace DiscordCoreInternal {

	ErlPackError::ErlPackError(const std::string& message) : std::runtime_error(message.c_str()){};

	ErlPacker::ErlPacker(){};

	ErlPacker& ErlPacker::operator=(const std::string& theBuffer) {
		return *this;
	}

	ErlPacker::ErlPacker(const std::string& theBuffer) : buffer(( char* )theBuffer.data()){};

	std::string ErlPacker::parseJsonToEtf(nlohmann::json& dataToParse) {
		this->bufferString.clear();
		this->offSet = 0;
		ErlPacker::appendVersion();
		ErlPacker::singleValueJsonToETF(dataToParse);
		return this->bufferString;
	}

	nlohmann::json ErlPacker::parseEtfToJson(std::string& dataToParse) {
		this->buffer = dataToParse.data();
		this->size = dataToParse.size();
		this->offSet = 0;
		if (ErlPacker::readBits<uint8_t>() != formatVersion) {
			throw ErlPackError{ "ErlPacker::parseEtfToJson() Error: Incorrect format version specified." };
		}
		return ErlPacker::singleValueETFToJson();
	}

	void ErlPacker::singleValueJsonToETF(nlohmann::json& jsonData) {
		switch (jsonData.type()) {
			case nlohmann::json::value_t::array: {
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
				break;
			}
			case nlohmann::json::value_t::object: {
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
				break;
			}
			case nlohmann::json::value_t::number_integer: {
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
				break;
			}
			case nlohmann::json::value_t::boolean: {
				if (jsonData.get<bool>()) {
					ErlPacker::appendTrue();
				}
				else {
					ErlPacker::appendFalse();
				}
				break;
			}
			case nlohmann::json::value_t::string: {
				std::string newString = jsonData.get<std::string>();
				std::string newVector{};
				newVector.insert(newVector.begin(), newString.begin(), newString.end());
				uint32_t newValue = static_cast<uint32_t>(newVector.size());
				ErlPacker::appendBinaryExt(newVector, newValue);
				break;
			}
			case nlohmann::json::value_t::number_float: {
				double newValue = jsonData.get<double>();
				ErlPacker::appendFloatExt(newValue);
				break;
			}
			case nlohmann::json::value_t::null: {
				ErlPacker::appendNil();
				break;
			}
			default: {
			}
		}
	}

	void ErlPacker::writeToBuffer(const std::string& bytes) {
		if (this->offSet > this->bufferString.size()) {
			this->bufferString.resize(this->offSet);
		}
		this->bufferString.insert(this->bufferString.begin() + this->offSet, bytes.begin(), bytes.end());
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

	void ErlPacker::appendFloatExt(double value) {
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

	const char* ErlPacker::readString(uint32_t length) {
		if (this->offSet + static_cast<uint64_t>(length) > this->size) {
			throw ErlPackError{ "ErlPacker::readString() Error: readString() past end of buffer.\n\n" };
		}
		const char* theStringNew = this->buffer + this->offSet;
		this->offSet += length;
		return theStringNew;
	}

	nlohmann::json ErlPacker::singleValueETFToJson() {
		if (this->offSet >= this->size) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		uint8_t type = ErlPacker::readBits<uint8_t>();
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return ErlPacker::parseSmallIntegerExt();
			}
			case ETFTokenType::Integer_Ext: {
				return ErlPacker::parseIntegerExt();
			}
			case ETFTokenType::New_Float_Ext: {
				[[fallthrough]];
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
		nlohmann::json theValue = ErlPacker::readBits<uint8_t>();
		return theValue;
	}

	nlohmann::json ErlPacker::parseBigint(uint32_t digits) {
		uint8_t sign = ErlPacker::readBits<uint8_t>();
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t x = 0; x < digits; ++x) {
			uint8_t digit = ErlPacker::readBits<uint8_t>();
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
		auto theValue = sign == 0 ? static_cast<uint64_t>(value) : static_cast<int64_t>(value);
		const int32_t res = sprintf(outBuffer, formatString, theValue);
		if (res < 0) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Parse big integer failed.\n\n" };
		}
		const uint8_t length = static_cast<uint8_t>(res);
		nlohmann::json theReturnValue = std::string{ outBuffer, length };
		return theReturnValue;
	}

	nlohmann::json ErlPacker::parseIntegerExt() {
		nlohmann::json theValue = ErlPacker::readBits<uint32_t>();
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

	bool compareString(const char* theString,const char*theString02, size_t theSize) {
		for (size_t x = 0; x < theSize; ++x) {
			if (!(theString[x] & theString02[x])) {
				return false;
			}
		}
		return true;
	}

	nlohmann::json ErlPacker::processAtom(const char* atom, uint32_t length) {
		if (atom == nullptr) {
			return nlohmann::json{};
		}
		static const char* atom_null{ "null" };
		static const char* atom_true{ "true" };
		if (length == 3) {
			for (size_t x = 0; x < length; ++x) {
				this->comparisongStringNil[x] = atom[x];
			}
			if (this->comparisongStringNil == this->nilString) {
				return nlohmann::json{};
			}
		} else if (length == 5) {
			for (size_t x = 0; x < length; ++x) {
				this->comparisongStringFalse[x] = atom[x];
			}
			if (this->comparisongStringFalse == this->falseString) {
				return false;
			}
		}
		else if (length == 4 && reinterpret_cast<const uint32_t*>(atom) == reinterpret_cast<const uint32_t*>(atom_null)) {// "null"
			return nlohmann::json{};
		} else if (length == 4 && reinterpret_cast<const uint32_t*>(atom) == reinterpret_cast<const uint32_t*>(atom_true)) {// "true"
			return true;
		}

		nlohmann::json j = std::string(atom, length);
		return j;
	}

	nlohmann::json ErlPacker::parseTuple(const uint32_t length) {
		nlohmann::json theValue = ErlPacker::parseArray(length);
		return theValue;
	}

	nlohmann::json ErlPacker::parseSmallTupleExt() {
		nlohmann::json theValue = ErlPacker::parseTuple(ErlPacker::readBits<uint8_t>());
		return theValue;
	}

	nlohmann::json ErlPacker::parseLargeTupleExt() {
		nlohmann::json theValue = ErlPacker::parseTuple(ErlPacker::readBits<uint32_t>());
		return theValue;
	}

	nlohmann::json ErlPacker::parseNilExt() {
		return nlohmann::json();
	}

	nlohmann::json ErlPacker::parseStringAsList() {
		uint16_t length = ErlPacker::readBits<uint16_t>();
		nlohmann::json theArray = nlohmann::json::array();
		if (static_cast<uint64_t>(this->offSet) + length > this->size) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.emplace_back(ErlPacker::parseSmallIntegerExt());
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseListExt() {
		uint32_t length = ErlPacker::readBits<uint32_t>();
		nlohmann::json::array_t theArray = ErlPacker::parseArray(length);
		uint8_t theValue = ErlPacker::readBits<uint8_t>();
		const ETFTokenType tailMarker = static_cast<ETFTokenType>(theValue);
		if (tailMarker != ETFTokenType::Nil_Ext) {
			return nlohmann::json::array_t();
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseBinaryExt() {
		uint32_t length = ErlPacker::readBits<uint32_t>();
		auto stringNew = ErlPacker::readString(length);
		if (stringNew == nullptr) {
			return nlohmann::json{};
		}
		nlohmann::json theValue = std::string{ stringNew, length };
		return theValue;
	}

	nlohmann::json ErlPacker::parseSmallBigExt() {
		nlohmann::json theValue = ErlPacker::parseBigint(ErlPacker::readBits<uint8_t>());
		return theValue;
	}

	nlohmann::json ErlPacker::parseLargeBigExt() {
		nlohmann::json theValue = ErlPacker::parseBigint(ErlPacker::readBits<uint32_t>());
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
		uint32_t length = readBits<uint32_t>();
		nlohmann::json map{};
		for (uint32_t i = 0; i < length; ++i) {
			auto key = singleValueETFToJson();
			if (key.is_number()) {
				map.emplace(std::to_string(key.get<uint64_t>()), singleValueETFToJson());
				
			} else {
				map.emplace(key.get<std::string>(), singleValueETFToJson());
			}
		}
		return map;
	}

	nlohmann::json ErlPacker::parseAtomUtf8Ext() {
		uint32_t lengthNew = ErlPacker::readBits<uint16_t>();
		auto atom = ErlPacker::readString(lengthNew);
		nlohmann::json theValue = ErlPacker::processAtom(atom, lengthNew);
		return theValue;
	}

	nlohmann::json ErlPacker::parseSmallAtomUtf8Ext() {
		uint8_t length = ErlPacker::readBits<uint8_t>();
		auto atom = ErlPacker::readString(length);
		nlohmann::json theValue = ErlPacker::processAtom(atom, length);
		return theValue;
	}

}
