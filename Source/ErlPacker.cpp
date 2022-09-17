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

	std::string ErlPacker::parseJsonToEtf(std::string& dataToParse) {
		this->bufferString.clear();
		this->offSet = 0;
		this->size = 0;
		this->appendVersion();
		dataToParse.reserve(dataToParse.size() + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser theParser{};
		auto theDocument = theParser.iterate(dataToParse.data(), dataToParse.length(), dataToParse.capacity()).value().get_value().value();
		this->singleValueJsonToETF(theDocument);
		return this->bufferString;
	}

	std::string& ErlPacker::parseEtfToJson(std::string_view dataToParse) {
		this->bufferString.clear();
		this->offSet = 0;
		this->buffer = dataToParse;
		this->size = dataToParse.size();
		if (this->readBits<uint8_t>() != formatVersion) {
			throw ErlPackError{ "ErlPacker::parseEtfToJson() Error: Incorrect format version specified." };
		}
		this->bufferString = this->singleValueETFToJson();
		return this->bufferString;
	}

	void ErlPacker::writeString(simdjson::ondemand::value jsonData) {
		std::stringstream theStream{};
		theStream << jsonData.get_string().take_value();
		auto theSize = theStream.str().size();
		this->appendBinaryExt(theStream.str(), theSize);
	}

	void ErlPacker::writeNumber(simdjson::ondemand::value jsonData) {
		if (jsonData.get_number_type() == simdjson::ondemand::number_type::floating_point_number) {
			auto theFloat = jsonData.get_double().take_value();
			this->appendNewFloatExt(theFloat);
		} else if (jsonData.get_number_type() == simdjson::ondemand::number_type::signed_integer ||
			jsonData.get_number_type() == simdjson::ondemand::number_type::unsigned_integer) {
			auto theInt = jsonData.get_uint64().take_value();
			if (theInt <= 255) {
				this->appendSmallIntegerExt(static_cast<uint8_t>(theInt));
			} else if (theInt <= UINT32_MAX) {
				this->appendIntegerExt(static_cast<uint32_t>(theInt));
			} else {
				this->appendUnsignedLongLong(theInt);
			}
		}
	}

	void ErlPacker::writeBool(simdjson::ondemand::value jsonData) {
		auto theBool = jsonData.get_bool().take_value();
		if (theBool) {
			this->appendTrue();
		} else {
			this->appendFalse();
		}
	}

	void ErlPacker::singleValueJsonToETF(simdjson::ondemand::value jsonData) {
		switch (jsonData.type()) {
			case simdjson::ondemand::json_type::array:
				this->appendListHeader(jsonData.count_elements().take_value());
				for (auto child: jsonData.get_array()) {
					this->singleValueJsonToETF(child.value());
				}
				this->appendNilExt();
				break;
			case simdjson::ondemand::json_type::object:
				this->appendMapHeader(jsonData.count_fields().take_value());
				for (auto field: jsonData.get_object()) {
					std::stringstream theStream{};
					theStream << field.key();
					std::string theKey = theStream.str();
					auto theSize = theKey.size();
					this->appendBinaryExt(theKey, theSize);
					this->singleValueJsonToETF(field.value());
				}
				break;
			case simdjson::ondemand::json_type::number:
				this->writeNumber(jsonData);
				break;
			case simdjson::ondemand::json_type::string:
				this->writeString(jsonData);
				break;
			case simdjson::ondemand::json_type::boolean:
				this->writeBool(jsonData);
				break;
			case simdjson::ondemand::json_type::null:
				this->appendNilExt();
				break;
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
		char* theStringNew = ( char* )this->buffer.data() + this->offSet;
		for (int32_t x = 0; x < length; ++x) {
			switch (static_cast<char>(theStringNew[x])) {
				case '\b': {
					theStringNew[x] = static_cast<char>('b');
					break;
				}
				case '\t': {
					theStringNew[x] = static_cast<char>('t');
					break;
				}
				case '\n': {
					theStringNew[x] = static_cast<char>('n');
					break;
				}
				case '\v': {
					theStringNew[x] = static_cast<char>('v');
					break;
				}
				case '\f': {
					theStringNew[x] = static_cast<char>('f');
					break;
				}
				case '\r': {
					theStringNew[x] = static_cast<char>('r');
					break;
				}
				case '\"': {
					theStringNew[x] = static_cast<char>('r');
					break;
				}
				case '\'': {
					theStringNew[x] = static_cast<char>('b');
					break;
				}
				case '\\': {
					theStringNew[x] = static_cast<char>('b');
					break;
				}
				default: {}
			}
		}
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
				return this->parseAtomUtf8Ext();
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
			case ETFTokenType::String_Ext: {
				std::string theString{ "\"" };
				theString += std::move(this->parseStringAsList());
				theString += "\"";
				return theString;
			}
			case ETFTokenType::List_Ext: {
				return this->parseListExt();
			}
			case ETFTokenType::Binary_Ext: {
				std::string theString{ "\"" };
				theString += std::move(this->parseBinaryExt());
				theString += "\"";
				if (theString == "\"\"") {
					theString = "nullptr";
				}
				return theString;
			}
			case ETFTokenType::Small_Big_Ext: {
				std::string theString{ "\"" };
				theString += std::move(this->parseSmallBigExt());
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
			case ETFTokenType::Atom_Utf8_Ext: {
				return this->parseAtomUtf8Ext();
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	std::string ErlPacker::parseSmallIntegerExt() {
		return std::to_string(this->readBits<uint8_t>());
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
		return std::to_string(this->readBits<uint32_t>());
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
				return std::string{ "null" };
			} else if (length == 4 && strncmp(atom, "true", 4) == 0) {
				return std::string{ "true" };
			} else if (length == 5 && strncmp(atom, "false", 5) == 0) {
				return std::string{ "false" };
			}
		}
		std::string theValue{ "\"" };
		theValue += std::string{ atom, length };
		theValue += "\"";
		return theValue;
	}

	std::string ErlPacker::parseTuple(const uint32_t length) {
		return this->parseArray(length);
	}

	std::string ErlPacker::parseSmallTupleExt() {
		return this->parseTuple(this->readBits<uint8_t>());
	}

	std::string ErlPacker::parseLargeTupleExt() {
		return this->parseTuple(this->readBits<uint32_t>());
	}

	std::string ErlPacker::parseNilExt() {
		return R"([])";
	}

	std::string ErlPacker::parseStringAsList() {
		uint16_t length = this->readBits<uint16_t>();
		std::string theArray{};
		if (static_cast<uint64_t>(this->offSet) + length > this->size) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		theArray.reserve(length);
		for (uint16_t x = 0; x < length; ++x) {
			theArray.push_back(this->parseSmallIntegerExt()[0]);
		}
		return theArray;
	}

	std::string ErlPacker::parseListExt() {
		uint32_t length = this->readBits<uint32_t>();
		std::string theArray{};
		theArray += R"([)";
		theArray += std::move(this->parseArray(length));
		theArray += R"(])";
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

		return std::string{ stringNew, length };
	}

	std::string ErlPacker::parseSmallBigExt() {
		return this->parseBigint(this->readBits<uint8_t>());
	}

	std::string ErlPacker::parseLargeBigExt() {
		return this->parseBigint(this->readBits<uint32_t>());
	}

	std::string ErlPacker::parseArray(const uint32_t length) {
		std::string array{};
		for (uint32_t x = 0; x < length; x++) {
			array += std::move(this->singleValueETFToJson());
			if (x < length - 1) {
				array += R"(,)";
			}
		}

		return array;
	}

	std::string ErlPacker::parseMapExt() {
		uint32_t length = readBits<uint32_t>();
		std::string map{};
		map += R"({)";
		for (uint32_t i = 0; i < length; ++i) {
			map += std::move(this->singleValueETFToJson());
			map += ":";
			map += std::move(this->singleValueETFToJson());
			if (i < length - 1) {
				map += ",";
			}
		}
		map += R"(})";
		return map;
	}

	std::string ErlPacker::parseAtomUtf8Ext() {
		uint32_t lengthNew = this->readBits<uint16_t>();
		auto atom = this->readString(lengthNew);
		return this->processAtom(atom, lengthNew);
	}

	std::string ErlPacker::parseSmallAtomExt() {
		uint8_t length = this->readBits<uint8_t>();
		auto atom = this->readString(length);
		return this->processAtom(atom, length);
	};
}