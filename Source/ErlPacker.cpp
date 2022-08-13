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
#include<discordcoreapi/SSLClients.hpp>

#define etfReverseByteOrder16(x) ntohs(x)
#define etfReverseByteOrder32(x) ntohl(x)
#define etfReverseByteOrder64(x) \
	((((( uint64_t )x) << 56)) | (((( uint64_t )x) << 40) & 0x00ff000000000000ULL) | (((( uint64_t )x) << 24) & 0x0000ff0000000000ULL) | \
		(((( uint64_t )x) << 8) & 0x000000ff00000000ULL) | (((( uint64_t )x) >> 8) & 0x00000000ff000000ULL) | (((( uint64_t )x) >> 24) & 0x0000000000ff0000ULL) | \
		(((( uint64_t )x) >> 40) & 0x000000000000ff00ULL) | (((( uint64_t )x) >> 56)))

namespace DiscordCoreInternal {


	ErlPackError::ErlPackError(const std::string& message) : std::runtime_error(message.c_str()){};

	std::string ErlPacker::parseJsonToEtf(const nlohmann::json& dataToParse) {
		this->offSet = 0;
		this->bufferOut.clear();
		this->appendVersion();
		this->singleValueJsonToETF(dataToParse);
		return this->bufferOut;
	}

	nlohmann::json ErlPacker::parseEtfToJson(std::string& dataToParse) {
		this->theLength = dataToParse.size();
		this->offSet = 0;
		this->theIncrement = 0;
		this->buffer = ( uint8_t* )dataToParse.data();
		uint8_t version = this->read8Bits();
		//std::cout << "THE RESULT TIME: " << this->theStopWatch.totalTimePassed() << std::endl;
		this->theStopWatch.resetTimer();
		return std::move(this->singleValueETFToJson());
	}

	void ErlPacker::singleValueJsonToETF(const nlohmann::json& jsonData) {
		if (jsonData.is_array()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length == 0) {
				this->appendNilExt();
			} else {
				if (length > std::numeric_limits<uint32_t>::max() - 1) {
					throw ErlPackError{ "this->singleValueJsonToETF() Error: List too large for ETF.\n\n" };
				}
			}
			this->appendListHeader(length);
			for (uint64_t index = 0; index < length; ++index) {
				this->singleValueJsonToETF(jsonData[index]);
			}
			this->appendNilExt();
		} else if (jsonData.is_object()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length > std::numeric_limits<uint32_t>::max() - 1) {
				throw ErlPackError{ "this->singleValueJsonToETF() Error: Map too large for ETF.\n\n" };
			}
			this->appendMapHeader(length);
			for (auto n = jsonData.begin(); n != jsonData.end(); ++n) {
				nlohmann::json jstr = n.key();
				this->singleValueJsonToETF(jstr);
				this->singleValueJsonToETF(n.value());
			}
		} else if (jsonData.is_number_integer()) {
			uint64_t numberOld = jsonData.get<uint64_t>();
			if (numberOld <= 127) {
				uint8_t number = jsonData.get<uint8_t>();
				this->appendSmallIntegerExt(number);
			} else if (jsonData.is_number_unsigned() && (numberOld >= std::numeric_limits<uint32_t>::max() - static_cast<size_t>(1))) {
				uint64_t number = jsonData.get<uint64_t>();
				this->appendUnsignedLongLong(number);
			} else {
				uint32_t number = jsonData.get<uint32_t>();
				this->appendIntegerExt(number);
			}
		} else if (jsonData.is_boolean()) {
			if (jsonData.get<bool>()) {
				this->appendTrue();
			} else {
				this->appendFalse();
			}
		} else if (jsonData.is_string()) {
			std::string newString = jsonData.get<std::string>();
			std::string newVector{};
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			uint32_t newValue = static_cast<uint32_t>(newVector.size());
			this->appendBinaryExt(newVector, newValue);
		} else if (jsonData.is_number_float()) {
			double newValue = jsonData.get<double>();
			this->appendFloatExt(newValue);
		} else if (jsonData.is_null()) {
			this->appendNil();
		}
	}

	void ErlPacker::writeToBuffer(const std::string& bytes) {
		this->bufferOut.insert(this->bufferOut.begin() + this->offSet, bytes.begin(), bytes.end());
		this->offSet += static_cast<uint32_t>(bytes.size());
	}

	void ErlPacker::appendVersion() {
		std::string bufferNew{ static_cast<char>(formatVersion) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendSmallIntegerExt(uint8_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendIntegerExt(uint32_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Integer_Ext) };
		uint32_t newValue{ 1 };
		DiscordCoreAPI::store32Bits(&bufferNew, value);
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFloatExt(double value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Float_Ext) };
		void* doubleValue{ &value };
		DiscordCoreAPI::store64Bits(&bufferNew, *static_cast<uint64_t*>(doubleValue));
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNilExt() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Nil_Ext) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNil() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 3, static_cast<uint8_t>('n'), static_cast<uint8_t>('i'), static_cast<uint8_t>('l') };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFalse() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 5, static_cast<uint8_t>('f'), static_cast<uint8_t>('a'), static_cast<uint8_t>('l'),
			static_cast<uint8_t>('s'), static_cast<uint8_t>('e') };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendTrue() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 4, static_cast<uint8_t>('t'), static_cast<uint8_t>('r'), static_cast<uint8_t>('u'),
			static_cast<uint8_t>('e') };
		this->writeToBuffer(bufferNew);
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
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendBinaryExt(const std::string& bytes, uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Binary_Ext) };
		DiscordCoreAPI::store32Bits(&bufferNew, size);
		this->writeToBuffer(bufferNew);
		this->writeToBuffer(bytes);
	}

	void ErlPacker::appendListHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::store32Bits(&bufferNew, size);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendMapHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::store32Bits(&bufferNew, size);
		this->writeToBuffer(bufferNew);
	}

	uint8_t ErlPacker::read8Bits() {
		if (this->offSet + sizeof(uint8_t) > this->theLength) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of this->\n\n" };
		}
		uint8_t theValue = *reinterpret_cast<uint8_t*>(this->buffer + this->offSet);
		this->offSet += sizeof(uint8_t);
		return theValue;
	}

	uint16_t ErlPacker::read16Bits() {
		if (this->offSet + sizeof(uint16_t) > this->theLength) {
			throw std::runtime_error("ETF: read_16_bits() past end of buffer");
		}
		uint16_t val = etfReverseByteOrder16(*reinterpret_cast<const uint16_t*>(this->buffer + offSet));
		offSet += sizeof(uint16_t);
		return val;
	}

	uint32_t ErlPacker::read32Bits() {
		if (this->offSet+ sizeof(uint32_t) > this->theLength) {
			throw std::runtime_error("ETF: read_32_bits() past end of buffer");
		}
		uint32_t val = etfReverseByteOrder32(*reinterpret_cast<const uint32_t*>(this->buffer + offSet));
		offSet += sizeof(uint32_t);
		return val;
	}

	uint64_t ErlPacker::read64Bits() {
		if (this->offSet + sizeof(uint64_t) > this->theLength) {
			throw std::runtime_error("ETF: read_64_bits() past end of buffer");
		}
		uint64_t val = etfReverseByteOrder64(*reinterpret_cast<const uint64_t*>(this->buffer + offSet));
		offSet += sizeof(uint64_t);
		return val;
	}

	const char* ErlPacker::readString(uint32_t length) {
		if (this->offSet + length > this->theLength) {
			throw ErlPackError{ "ErlPacker::readString() Error: readString() past end of this->\n\n" };
		}
		const char* theString{ ( char* )(this->buffer + this->offSet) };
		this->offSet += length;
		return theString;
	}

	nlohmann::json ErlPacker::singleValueETFToJson() {
		if (this->offSet >= this->theLength) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF this->\n\n" };
		}
		//this->theIncrement++;
		//std::cout << "THE INCREMENT: " << this->theIncrement << std::endl;
		uint8_t type = this->read8Bits();
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseSmallIntegerExt();
				//std::cout << "THE TIME PASSED FOR SMALL INTEGER EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Integer_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseIntegerExt();
				//std::cout << "THE TIME PASSED FOR INTEGER EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Float_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseFloatExt();
				//std::cout << "THE TIME PASSED FOR FLOAT EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Atom_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseAtomUtf8Ext();
				//std::cout << "THE TIME PASSED FOR ATOM EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Small_Tuple_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseSmallTupleExt();
				//std::cout << "THE TIME PASSED FOR SMALL TUPLE EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Large_Tuple_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseLargeTupleExt();
				//std::cout << "THE TIME PASSED FOR LARGE TUPLE EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Nil_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseNilExt();
				//std::cout << "THE TIME PASSED FOR NIL EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::String_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseStringAsList();
				//std::cout << "THE TIME PASSED FOR STRINGEXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::List_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseListExt();
				//std::cout << "THE TIME PASSED FOR LIST EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Binary_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseBinaryExt();
				//std::cout << "THE TIME PASSED FOR BINARY EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Small_Big_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseSmallBigExt();
				//std::cout << "THE TIME PASSED FOR SMALL BIG EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Large_Big_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseLargeBigExt();
				//std::cout << "THE TIME PASSED FOR LARGE BIG EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Map_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseMapExt();
				//std::cout << "THE TIME PASSED FOR MAP EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Atom_Utf8_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseAtomUtf8Ext();
				//std::cout << "THE TIME PASSED FOR ATOM EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			case ETFTokenType::Small_Atom_Utf8_Ext: {
				//this->theStopWatch.resetTimer();
				auto theResult = this->parseSmallAtomUtf8Ext();
				//std::cout << "THE TIME PASSED FOR SMALL ATOM EXT: " << this->theStopWatch.totalTimePassed() << std::endl;
				return theResult;
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	nlohmann::json ErlPacker::parseSmallIntegerExt() {
		nlohmann::json j;
		j = ( uint8_t )this->read8Bits();
		return j;
	}

	nlohmann::json ErlPacker::parseBigint(uint32_t digits) {
		nlohmann::json jsonData{};
		uint8_t sign = this->read8Bits();
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t x = 0; x < digits; ++x) {
			uint8_t digit = this->read8Bits();
			uint64_t digitNew = digit;
			value += digitNew * b;
			b <<= 8;
		}
		if (digits <= 4) {
			if (sign == 0) {
				jsonData = std::to_string(value);
				return jsonData;
			}
			const bool isSignBitAvailable = (value & 1ull << 31ull) == 0;
			if (isSignBitAvailable) {
				jsonData = std::to_string(-static_cast<int32_t>(value));
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
		jsonData = std::string(outBuffer, length);
		return jsonData;
	}

	nlohmann::json ErlPacker::parseIntegerExt() {
		uint32_t newValue = this->read32Bits();
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseFloatExt() {
		uint32_t floatLength = 31;
		const char* floatStr = this->readString(floatLength);
		nlohmann::json jsonData{};
		double number{};
		std::vector<char> nullTerminated{};
		nullTerminated.insert(nullTerminated.begin(), floatStr, floatStr + floatLength);
		auto count = sscanf(nullTerminated.data(), "%lf", &number);
		if (!count) {
			return jsonData;
		}
		jsonData = number;
		return jsonData;
	}

	nlohmann::json ErlPacker::processAtom(const char* atom, uint32_t length) {
		nlohmann::json jsonData{};
		if (length == 0) {
			return jsonData;
		}
		if (length >= 3 && length <= 5) {
			if (length == 3 && strncmp(atom, "nil", 3) == 0) {
				return jsonData;
			} else if (length == 4 && strncmp(atom, "null", 4) == 0) {
				return jsonData;
			} else if (length == 4 && strncmp(atom, "true", 4) == 0) {
				jsonData = true;
				return jsonData;
			} else if (length == 5 && strncmp(atom, "false", 5) == 0) {
				jsonData = false;
				return jsonData;
			}
		}
		jsonData = std::string{ atom, length };
		return jsonData;
	}

	nlohmann::json ErlPacker::parseTuple(uint32_t length) {
		return this->parseArray(length);
	}

	nlohmann::json ErlPacker::parseSmallTupleExt() {
		uint8_t theValue = this->read8Bits();
		uint32_t theValueNew = theValue;
		return this->parseTuple(theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeTupleExt() {
		uint32_t theValue = this->read32Bits();
		return this->parseTuple(theValue);
	}

	nlohmann::json ErlPacker::parseNilExt() {
		return nlohmann::json();
	}

	nlohmann::json ErlPacker::parseStringAsList() {
		uint16_t length = this->read16Bits();
		nlohmann::json theArray = nlohmann::json::array();
		if (static_cast<uint64_t>(this->offSet) + length > this->theLength) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of this->\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.push_back(this->parseSmallIntegerExt());
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseListExt() {
		const uint32_t length = this->read32Bits();
		auto theArray = parseArray(length);
		const ETFTokenType tailMarker = static_cast<ETFTokenType>(this->read8Bits());
		if (tailMarker != ETFTokenType::Nil_Ext) {
			return nlohmann::json{};
		}
		return theArray;
	}
	
	nlohmann::json ErlPacker::parseBinaryExt() {
		//this->theStopWatch.resetTimer();
		const auto length = this->read32Bits();
		//std::cout << "THE TIME TO READ 32 BITS: " << this->theStopWatch.totalTimePassed() << std::endl;
		//this->theStopWatch.resetTimer();
		const char* string = this->readString(length);
		//std::cout << "THE TIME TO READ STRING: " << this->theStopWatch.totalTimePassed() << std::endl;
		if (string == NULL) {
			nlohmann::json theData{};
			return std::move(theData);
		}
		nlohmann::json theData = std::string{ string, length };
		return std::move(theData);
	}

	nlohmann::json ErlPacker::parseSmallBigExt() {
		uint8_t theValue = this->read8Bits();
		uint32_t theValueNew = theValue;
		return this->parseBigint(theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeBigExt() {
		uint32_t theValue = this->read32Bits();
		return this->parseBigint(theValue);
	}

	nlohmann::json ErlPacker::parseArray(uint32_t length) {
		nlohmann::json theArray = nlohmann::json::array();
		for (uint32_t i = 0; i < length; ++i) {
			theArray.emplace_back(this->singleValueETFToJson());
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseMapExt() {
		const uint32_t length = this->read32Bits();
		auto map = nlohmann::json::object();
		for (uint32_t i = 0; i < length; ++i) {
			auto key = this->singleValueETFToJson();
			auto value = this->singleValueETFToJson();
			if (key.is_number()) {
				map[std::to_string(key.get<uint64_t>())] = std::move(value);
			} else {
				map[key.get<std::string>()] = std::move(value);
			}
		}
		return map;
	}

	nlohmann::json ErlPacker::parseAtomUtf8Ext() {
		uint16_t length = this->read16Bits();
		uint32_t lengthNew = length;
		const char* atom = this->readString(lengthNew);
		return this->processAtom(atom, lengthNew);
	}

	nlohmann::json ErlPacker::parseSmallAtomUtf8Ext() {
		uint8_t length = this->read8Bits();
		uint32_t lengthNew = length;
		const char* atom = this->readString(lengthNew);
		return this->processAtom(atom, lengthNew);
	}

}
