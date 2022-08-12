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

	DiscordCoreAPI::StopWatch theStopWatch{ 50us };

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
		this->buffer = ( uint8_t* )dataToParse.data();
		std::cout << "THE STRING: " << dataToParse << std::endl;
		uint8_t version = this->read8Bits();
		theStopWatch.resetTimer();
		auto theResult = this->singleValueETFToJson();
		std::cout << "THE RESULT TIME: " << theStopWatch.totalTimePassed() << std::endl;
		for (auto& value: theResult) {
			//std::cout << "THE ID: " << value.dump() << std::endl;
		}
		return theResult;
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
		DiscordCoreAPI::store32Bits(bufferNew, value);
		ErlPacker::writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFloatExt(double value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Float_Ext) };
		void* doubleValue{ &value };
		DiscordCoreAPI::store64Bits(bufferNew, *static_cast<uint64_t*>(doubleValue));
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
		DiscordCoreAPI::store32Bits(bufferNew, size);
		this->writeToBuffer(bufferNew);
		this->writeToBuffer(bytes);
	}

	void ErlPacker::appendListHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::store32Bits(bufferNew, size);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendMapHeader(uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::store32Bits(bufferNew, size);
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
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of this->\n\n" };
		}
		uint16_t theValue = _byteswap_ushort(*reinterpret_cast<uint16_t*>(this->buffer + this->offSet));
		this->offSet += sizeof(uint16_t);
		return theValue;
	}

	uint32_t ErlPacker::read32Bits() {
		if (this->offSet + sizeof(uint32_t) > this->theLength) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of this->\n\n" };
		}
		uint32_t theValue = _byteswap_ulong(*reinterpret_cast<uint32_t*>(this->buffer + this->offSet));
		this->offSet += sizeof(uint32_t);
		return theValue;
	}

	uint64_t ErlPacker::read64Bits() {
		if (this->offSet + sizeof(uint64_t) > this->theLength) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of this->\n\n" };
		}
		uint64_t theValue = _byteswap_uint64(*reinterpret_cast<uint64_t*>(this->buffer + this->offSet));
		this->offSet += sizeof(uint64_t);
		return theValue;
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
		uint8_t type = this->read8Bits();
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return this->decode_small_integer();
			}
			case ETFTokenType::Integer_Ext: {
				return this->decode_integer();
			}
			case ETFTokenType::Float_Ext: {
				return this->decode_float();
			}
			case ETFTokenType::Atom_Ext: {
				return this->decode_atom();
			}
			case ETFTokenType::Small_Tuple_Ext: {
				return this->decode_tuple_small();
			}
			case ETFTokenType::Large_Tuple_Ext: {
				return this->decode_tuple_large();
			}
			case ETFTokenType::Nil_Ext: {
				return this->decode_nil();
			}
			case ETFTokenType::String_Ext: {
				return this->decode_string_as_list();
			}
			case ETFTokenType::List_Ext: {
				return this->decode_list();
			}
			case ETFTokenType::Binary_Ext: {
				return this->decode_binary();
			}
			case ETFTokenType::Small_Big_Ext: {
				return this->decode_bigint_small();
			}
			case ETFTokenType::Large_Big_Ext: {
				return this->decode_bigint_large();
			}
			case ETFTokenType::Map_Ext: {
				return this->decode_map();
			}
			case ETFTokenType::Atom_Utf8_Ext: {
				//return this->parseAtomUtf8Ext();
			}
			case ETFTokenType::Small_Atom_Utf8_Ext: {
				//return this->parseSmallAtomUtf8Ext();
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}
	nlohmann::json ErlPacker::decode_atom() {
		auto length = this->read16Bits();
		const char* atom = this->readString(length);
		return nlohmann::json{};
	}

	nlohmann::json ErlPacker::decode_small_atom() {
		auto length = this->read8Bits();
		const char* atom = this->readString(length);
		return nlohmann::json{};
	}

	nlohmann::json ErlPacker::decode_small_integer() {
		nlohmann::json j;
		j = ( uint8_t )this->read8Bits();
		return j;
	}

	nlohmann::json ErlPacker::decode_integer() {
		nlohmann::json j;
		j = ( int32_t )this->read32Bits();
		return j;
	}

	nlohmann::json ErlPacker::decode_array(uint32_t length) {
		nlohmann::json array = nlohmann::json::array();
		for (uint32_t i = 0; i < length; ++i) {
			array.emplace_back(singleValueETFToJson());
		}
		return array;
	}

	nlohmann::json ErlPacker::decode_list() {
		const uint32_t length = this->read32Bits();
		auto array = decode_array(length);

		const auto tailMarker = this->read8Bits();
		if (static_cast<ETFTokenType>(tailMarker) !=ETFTokenType::Nil_Ext) {
			return nlohmann::json();
		}

		return array;
	}

	nlohmann::json ErlPacker::decode_tuple(uint32_t length) {
		return decode_array(length);
	}

	nlohmann::json ErlPacker::decode_nil() {
		return nlohmann::json::array();
	}

	nlohmann::json ErlPacker::decode_map() {
		const uint32_t length = this->read32Bits();
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

	nlohmann::json ErlPacker::decode_float() {
		const uint8_t FLOAT_LENGTH = 31;
		const char* floatStr = this->readString(FLOAT_LENGTH);

		if (floatStr == NULL) {
			return nlohmann::json();
		}

		double number;
		char nullTerimated[FLOAT_LENGTH + 1] = { 0 };

		memcpy(nullTerimated, floatStr, FLOAT_LENGTH);

		auto count = sscanf(nullTerimated, "%lf", &number);

		if (count != 1) {
			return nlohmann::json();
		}

		nlohmann::json j = number;
		return j;
	}

	nlohmann::json ErlPacker::decode_new_float() {
		union {
			uint64_t ui64;
			double df;
		} val;
		val.ui64 = this->read64Bits();
		nlohmann::json j = val.df;
		return j;
	}

	nlohmann::json ErlPacker::decode_bigint(uint32_t digits) {
		const uint8_t sign = this->read8Bits();

		if (digits > 8) {
			throw std::runtime_error("ETF: big integer larger than 8 bytes unsupported");
		}

		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t i = 0; i < digits; ++i) {
			uint64_t digit = this->read8Bits();
			value += digit * b;
			b <<= 8;
		}

		if (digits <= 4) {
			if (sign == 0) {
				nlohmann::json j = std::to_string(static_cast<uint32_t>(value));
				return j;
			}

			const bool isSignBitAvailable = (value & (1 << 31)) == 0;
			if (isSignBitAvailable) {
				int32_t negativeValue = -static_cast<int32_t>(value);
				nlohmann::json j = std::to_string(negativeValue);
				return j;
			}
		}

		if (sign == 0) {
			nlohmann::json j = std::to_string(value);
			return j;
		} else {
			nlohmann::json j = std::to_string(-(( int64_t )value));
			return j;
		}
	}

	nlohmann::json ErlPacker::decode_bigint_small() {
		const auto bytes = this->read8Bits();
		return decode_bigint(bytes);
	}

	nlohmann::json ErlPacker::decode_bigint_large() {
		const auto bytes = this->read32Bits();
		return decode_bigint(bytes);
	}

	nlohmann::json ErlPacker::decode_binary() {
		const auto length = this->read32Bits();
		const char* str = this->readString(length);
		if (str == NULL) {
			return nlohmann::json();
		}
		nlohmann::json j = std::string(str, length);
		return j;
	}

	nlohmann::json ErlPacker::decode_string() {
		const auto length = this->read16Bits();
		const char* str = this->readString(length);
		if (str == NULL) {
			return nlohmann::json();
		}
		nlohmann::json j = std::string(str, length);
		return j;
	}

	nlohmann::json ErlPacker::decode_string_as_list() {
		const auto length = this->read16Bits();
		nlohmann::json array = nlohmann::json::array();
		if (offSet + length > this->theLength) {
			throw std::runtime_error("String list past end of buffer");
		}
		for (uint16_t i = 0; i < length; ++i) {
			array.push_back(decode_small_integer());
		}
		return array;
	}

	nlohmann::json ErlPacker::decode_tuple_small() {
		return decode_tuple(this->read8Bits());
	}

	nlohmann::json ErlPacker::decode_tuple_large() {
		return decode_tuple(this->read32Bits());
	}

	nlohmann::json ErlPacker::decode_reference() {
		nlohmann::json reference;

		reference["node"] = singleValueETFToJson();

		std::vector<int32_t> ids;
		ids.push_back(this->read32Bits());
		reference["id"] = ids;

		reference["creation"] = this->read8Bits();

		return reference;
	}

	nlohmann::json ErlPacker::decode_new_reference() {
		nlohmann::json reference;

		uint16_t len = this->read16Bits();
		reference["node"] = singleValueETFToJson();
		reference["creation"] = this->read8Bits();

		std::vector<int32_t> ids;
		for (uint16_t i = 0; i < len; ++i) {
			ids.push_back(this->read32Bits());
		}
		reference["id"] = ids;

		return reference;
	}

	nlohmann::json ErlPacker::decode_port() {
		nlohmann::json port;
		port["node"] = singleValueETFToJson();
		port["id"] = this->read32Bits();
		port["creation"] = this->read8Bits();
		return port;
	}

	nlohmann::json ErlPacker::decode_pid() {
		nlohmann::json pid;
		pid["node"] = singleValueETFToJson();
		pid["id"] = this->read32Bits();
		pid["serial"] = this->read32Bits();
		pid["creation"] = this->read8Bits();
		return pid;
	}

	nlohmann::json ErlPacker::decode_export() {
		nlohmann::json exp;
		exp["mod"] = singleValueETFToJson();
		exp["fun"] = singleValueETFToJson();
		exp["arity"] = singleValueETFToJson();
		return exp;
	}
}