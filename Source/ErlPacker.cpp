/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
/// ErlPacker.cpp - Source file for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.cpp

#include <discordcoreapi/ErlPacker.hpp>

namespace DiscordCoreInternal {

	ErlPackError::ErlPackError(const std::string& message) : std::runtime_error(message.c_str()){};

	std::string ErlPacker::parseJsonToEtf(nlohmann::json& dataToParse) {
		this->bufferString.clear();
		this->offSet = 0;
		this->size = 0;
		this->appendVersion();
		this->singleValueJsonToETF(dataToParse);
		return this->bufferString;
	}

	std::string ErlPacker::parseEtfToJson(std::string& dataToParse) {
		this->bufferString.clear();
		this->offSet = 0;
		this->buffer = dataToParse.data();
		this->size = dataToParse.size();
		if (this->readBits<uint8_t>() != formatVersion) {
			throw ErlPackError{ "ErlPacker::parseEtfToJson() Error: Incorrect format version specified." };
		}
		this->bufferString += this->singleValueETFToJson();
		return this->bufferString;
	}

	void ErlPacker::singleValueJsonToETF(nlohmann::json& jsonData) {
		if (jsonData.is_array()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length == 0) {
				this->appendNilExt();
			} else {
				if (length > std::numeric_limits<uint32_t>::max() - 1) {
					throw ErlPackError{ "ErlPacker::singleValueJsonToETF() Error: List too large for ETF.\n\n" };
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
				throw ErlPackError{ "ErlPacker::singleValueJsonToETF() Error: Map too large for ETF.\n\n" };
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
			this->appendNewFloatExt(newValue);
		} else if (jsonData.is_null()) {
			this->appendNil();
		}
	}

	void ErlPacker::writeToBuffer(const std::string& bytes) {
		this->bufferString.insert(this->bufferString.end(), bytes.begin(), bytes.end());
		this->offSet += bytes.size();
	}

	void ErlPacker::appendVersion() {
		std::string bufferNew{};
		bufferNew.push_back(static_cast<char>(formatVersion));
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendSmallIntegerExt(uint8_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendIntegerExt(uint32_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Integer_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, value);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNewFloatExt(double floatValue) {
		std::string bufferNew{};
		bufferNew.push_back(static_cast<unsigned char>(ETFTokenType::New_Float_Ext));

		void* punner{ &floatValue };
		DiscordCoreAPI::storeBits(bufferNew, *static_cast<uint64_t*>(punner));
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNilExt() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Nil_Ext) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNil() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Ext), 3, static_cast<uint8_t>('n'), static_cast<uint8_t>('i'), static_cast<uint8_t>('l') };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFalse() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Ext), 5, static_cast<uint8_t>('f'), static_cast<uint8_t>('a'), static_cast<uint8_t>('l'),
			static_cast<uint8_t>('s'), static_cast<uint8_t>('e') };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendTrue() {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Ext), 4, static_cast<uint8_t>('t'), static_cast<uint8_t>('r'), static_cast<uint8_t>('u'),
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

	void ErlPacker::appendBinaryExt(const std::string& bytes, uint32_t sizeNew) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Binary_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
		this->writeToBuffer(bytes);
	}

	void ErlPacker::appendListHeader(uint32_t sizeNew) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendMapHeader(uint32_t sizeNew) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
	}

	const char* ErlPacker::readString(uint32_t length) {
		if (this->offSet + static_cast<uint64_t>(length) > this->size) {
			throw ErlPackError{ "this->readString() Error: readString() past end of buffer.\n\n" };
		}
		const char* theStringNew = this->buffer + this->offSet;
		this->offSet += length;
		return theStringNew;
	}

	std::string ErlPacker::singleValueETFToJson() {
		if (this->offSet >= this->size) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		uint8_t type = this->readBits<uint8_t>();
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return this->parseSmallIntegerExt();
			}
			case ETFTokenType::Integer_Ext: {
				return this->parseIntegerExt();
			}
			case ETFTokenType::New_Float_Ext: {
				return this->parseNewFloatExt();
			}
			case ETFTokenType::Float_Ext: {
				return this->parseFloatExt();
			}
			case ETFTokenType::Atom_Ext: {
				auto theStringNew = this->parseAtomUtf8Ext();
				std::string theString{ "\"" };
				if (theStringNew != "null") {
					theStringNew.insert(theStringNew.begin(), theString.begin(), theString.end());
					theStringNew += "\"";
				} else {
					theStringNew = "nullptr";
				}
				return theStringNew;
			}
			case ETFTokenType::Small_Tuple_Ext: {
				return this->parseSmallTupleExt();
			}
			case ETFTokenType::Large_Tuple_Ext: {
				return this->parseLargeTupleExt();
			}
			case ETFTokenType::Nil_Ext: {
				return this->parseNilExt();
			}
			case ETFTokenType::String_Ext: {\
				std::string theString{};
				theString += "\"";
				theString += this->parseStringAsList();
				theString += "\"";
				return theString;
			}
			case ETFTokenType::List_Ext: {
				return this->parseListExt();
			}
			case ETFTokenType::Binary_Ext: {
				std::string theString{};
				theString += "\"";
				theString += this->parseBinaryExt();
				theString += "\"";
				return theString;
			}
			case ETFTokenType::Small_Big_Ext: {
				std::string theString{};
				theString += "\"";
				theString += this->parseSmallBigExt();
				theString += "\"";
				return theString;
			}
			case ETFTokenType::Large_Big_Ext: {
				return this->parseLargeBigExt();
			}
			case ETFTokenType::Small_Atom_Ext: {
				return this->parseSmallAtomExt();
			}
			case ETFTokenType::Map_Ext: {
				return this->parseMapExt();
			}
			case ETFTokenType::Atom_Utf8_Ext: {\
				return this->parseAtomUtf8Ext();
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	std::string ErlPacker::parseSmallIntegerExt() {
		std::string theValue = std::to_string(this->readBits<uint8_t>());
		return theValue;
	}

	std::string ErlPacker::parseBigint(uint32_t digits) {
		uint8_t sign = this->readBits<uint8_t>();
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t x = 0; x < digits; ++x) {
			uint8_t digit = this->readBits<uint8_t>();
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
		std::string theReturnValue = std::string{ outBuffer, length };
		return theReturnValue;
	}

	std::string ErlPacker::parseIntegerExt() {
		std::string theValue = std::to_string(this->readBits<uint32_t>());
		return theValue;
	}

	std::string ErlPacker::parseFloatExt() {
		const uint8_t floatLength = 31;
		const char* floatString = readString(floatLength);

		if (floatString == NULL) {
			return nlohmann::json{};
		}

		double number{};
		std::string nullTerminated{};
		nullTerminated.insert(nullTerminated.begin(), floatString, floatString + floatLength);

		auto count = sscanf(nullTerminated.data(), "%lf", &number);

		if (count != 1) {
			return std::string{};
		}

		std::string returnValue = std::to_string(number);
		return returnValue;
	}

	std::string ErlPacker::parseNewFloatExt() {
		uint64_t theValue = readBits<uint64_t>();
		void* thePtr{ &theValue };
		std::string theValueNew = std::to_string(*static_cast<double*>(thePtr));
		return theValueNew;
	}

	std::string ErlPacker::processAtom(const char* atom, uint32_t length) {
		if (atom == nullptr) {
			return nlohmann::json{};
		}
		if (length >= 3 && length <= 5) {
			if (length == 3 && strncmp(atom, "nil", 3) == 0) {
				return std::string{ "null" };
			} else if (length == 4 && strncmp(atom, "null", 4) == 0) {
				return std::string{ "nil" };
			} else if (length == 4 && strncmp(atom, "true", 4) == 0) {
				return std::string{ "true" };
			} else if (length == 5 && strncmp(atom, "false", 5) == 0) {
				return std::string{ "false" };
			}
		}
		nlohmann::json theValue = std::string{ atom, length };
		return theValue;
	}

	std::string ErlPacker::parseTuple(const uint32_t length) {
		std::string theValue = this->parseArray(length);
		return theValue;
	}

	std::string ErlPacker::parseSmallTupleExt() {
		std::string theValue = this->parseTuple(this->readBits<uint8_t>());
		return theValue;
	}

	std::string ErlPacker::parseLargeTupleExt() {
		std::string theValue = this->parseTuple(this->readBits<uint32_t>());
		return theValue;
	}

	std::string ErlPacker::parseNilExt() {
		std::string theString{};
		theString.append(R"([])");
		return theString;
	}

	std::string ErlPacker::parseStringAsList() {
		uint16_t length = this->readBits<uint16_t>();
		std::string theArray{};
		if (static_cast<uint64_t>(this->offSet) + length > this->size) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.append(this->parseSmallIntegerExt());
		}
		return theArray;
	}

	std::string ErlPacker::parseListExt() {
		uint32_t length = this->readBits<uint32_t>();
		std::string theArray{};
		theArray.append(R"([)");
		theArray += this->parseArray(length);
		theArray.append(R"(])");
		uint8_t theValue = this->readBits<uint8_t>();
		if (static_cast<ETFTokenType>(theValue) != ETFTokenType::Nil_Ext) {
			return std::string{};
		}
		return theArray;
	}

	std::string ErlPacker::parseBinaryExt() {
		uint32_t length = this->readBits<uint32_t>();
		auto stringNew = this->readString(length);
		if (stringNew == nullptr) {
			return std::string{};
		}

		std::string theValue{};

		for (uint32_t x = 0; x < length; ++x) {
			theValue.push_back(stringNew[x]);
		}

		return theValue;
	}

	std::string ErlPacker::parseSmallBigExt() {
		std::string theValue = this->parseBigint(this->readBits<uint8_t>());
		return theValue;
	}

	std::string ErlPacker::parseLargeBigExt() {
		std::string theValue = this->parseBigint(this->readBits<uint32_t>());
		return theValue;
	}

	std::string ErlPacker::parseArray(const uint32_t length) {
		std::string array{};
		for (uint32_t x = 0; x < length; x++) {
			std::string theString{ this->singleValueETFToJson() };
			DiscordCoreAPI::spinLock(100000);
			array.append(theString);
			if (x < length - 1) {
				array.append(R"(,)");
			}
		}
	
		return array;
	}

	std::string ErlPacker::parseMapExt() {
		uint32_t length = readBits<uint32_t>();
		std::string map{};
		map.append(R"({)");
		for (uint32_t i = 0; i < length; ++i) {
			std::string theKey{};
			theKey = singleValueETFToJson();
			theKey += ":";
			auto value = singleValueETFToJson();
			if (i < length -1) {
				value += ",";
			}
			map.append(theKey);
			map.append(value);
		}
		map.append(R"(})");
		return map;
	}

	std::string ErlPacker::parseAtomUtf8Ext() {
		uint32_t lengthNew = this->readBits<uint16_t>();
		auto atom = this->readString(lengthNew);
		std::string theValue = this->processAtom(atom, lengthNew);
		return theValue;
	}

	std::string ErlPacker::parseSmallAtomExt() {
		uint8_t length = this->readBits<uint8_t>();
		auto atom = this->readString(length);
		std::string theValue = this->processAtom(atom, length);
		return theValue;
	}
}