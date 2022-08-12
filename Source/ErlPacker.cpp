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

	BufferPack::BufferPack(std::string* bufferNew) {
		this->buffer = bufferNew;
		
	};

	ErlPackBuffer& ErlPackBuffer::operator=(BufferPack& theBuffer) noexcept{
		this->buffer = theBuffer.buffer;
		return *this;
	};

	ErlPackBuffer::ErlPackBuffer(BufferPack& theBuffer) {
		*this = theBuffer;
	};

	ErlPackBuffer::~ErlPackBuffer() {}

	std::string ErlPacker::parseJsonToEtf(const nlohmann::json& dataToParse) {
		std::string theString{};
		BufferPack theBuffer{ &theString };
		ErlPackBuffer buffer{ theBuffer };
		this->appendVersion(buffer);
		this->singleValueJsonToETF(buffer, dataToParse);
		return std::move(*buffer.buffer);
	}

	nlohmann::json ErlPacker::parseEtfToJson(BufferPack& dataToParse) {
		ErlPackBuffer buffer{ dataToParse };
		uint8_t version = this->readBits<uint8_t>(buffer);
		return this->singleValueETFToJson(buffer);
	}

	void ErlPacker::singleValueJsonToETF(ErlPackBuffer& buffer, const nlohmann::json& jsonData) {
		if (jsonData.is_array()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length == 0) {
				this->appendNilExt(buffer);
			} else {
				if (length > std::numeric_limits<uint32_t>::max() - 1) {
					throw ErlPackError{ "this->singleValueJsonToETF() Error: List too large for ETF.\n\n" };
				}
			}
			this->appendListHeader(buffer, length);
			for (uint64_t index = 0; index < length; ++index) {
				this->singleValueJsonToETF(buffer, jsonData[index]);
			}
			this->appendNilExt(buffer);
		} else if (jsonData.is_object()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length > std::numeric_limits<uint32_t>::max() - 1) {
				throw ErlPackError{ "this->singleValueJsonToETF() Error: Map too large for ETF.\n\n" };
			}
			this->appendMapHeader(buffer, length);
			for (auto n = jsonData.begin(); n != jsonData.end(); ++n) {
				nlohmann::json jstr = n.key();
				this->singleValueJsonToETF(buffer, jstr);
				this->singleValueJsonToETF(buffer, n.value());
			}
		} else if (jsonData.is_number_integer()) {
			uint64_t numberOld = jsonData.get<uint64_t>();
			if (numberOld <= 127) {
				uint8_t number = jsonData.get<uint8_t>();
				this->appendSmallIntegerExt(buffer, number);
			} else if (jsonData.is_number_unsigned() && (numberOld >= std::numeric_limits<uint32_t>::max() - static_cast<size_t>(1))) {
				uint64_t number = jsonData.get<uint64_t>();
				this->appendUnsignedLongLong(buffer, number);
			} else {
				uint32_t number = jsonData.get<uint32_t>();
				this->appendIntegerExt(buffer, number);
			}
		} else if (jsonData.is_boolean()) {
			if (jsonData.get<bool>()) {
				this->appendTrue(buffer);
			} else {
				this->appendFalse(buffer);
			}
		} else if (jsonData.is_string()) {
			std::string newString = jsonData.get<std::string>();
			std::string newVector{};
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			uint32_t newValue = static_cast<uint32_t>(newVector.size());
			this->appendBinaryExt(buffer, newVector, newValue);
		} else if (jsonData.is_number_float()) {
			double newValue = jsonData.get<double>();
			this->appendFloatExt(buffer, newValue);
		} else if (jsonData.is_null()) {
			this->appendNil(buffer);
		}
	}

	void ErlPacker::writeToBuffer(ErlPackBuffer& buffer, const std::string& bytes) {
		buffer.buffer->insert(buffer.buffer->begin() + buffer.offSet, bytes.begin(), bytes.end());
		buffer.offSet += static_cast<uint32_t>(bytes.size());
	}

	void ErlPacker::appendVersion(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<char>(formatVersion) };
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendSmallIntegerExt(ErlPackBuffer& buffer, uint8_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendIntegerExt(ErlPackBuffer& buffer, uint32_t value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Integer_Ext) };
		uint32_t newValue{ 1 };
		DiscordCoreAPI::store32Bits(&bufferNew, value);
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendFloatExt(ErlPackBuffer& buffer, double value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Float_Ext) };
		void* doubleValue{ &value };
		DiscordCoreAPI::store64Bits(&bufferNew, *static_cast<uint64_t*>(doubleValue));
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendNilExt(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Nil_Ext) };
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendNil(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 3, static_cast<uint8_t>('n'), static_cast<uint8_t>('i'), static_cast<uint8_t>('l') };
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendFalse(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 5, static_cast<uint8_t>('f'), static_cast<uint8_t>('a'), static_cast<uint8_t>('l'),
			static_cast<uint8_t>('s'), static_cast<uint8_t>('e') };
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendTrue(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 4, static_cast<uint8_t>('t'), static_cast<uint8_t>('r'), static_cast<uint8_t>('u'),
			static_cast<uint8_t>('e') };
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendUnsignedLongLong(ErlPackBuffer& buffer, uint64_t value) {
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
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendBinaryExt(ErlPackBuffer& buffer, const std::string& bytes, uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Binary_Ext) };
		DiscordCoreAPI::store32Bits(&bufferNew, size);
		this->writeToBuffer(buffer, bufferNew);
		this->writeToBuffer(buffer, bytes);
	}

	void ErlPacker::appendListHeader(ErlPackBuffer& buffer, uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::store32Bits(&bufferNew, size);
		this->writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendMapHeader(ErlPackBuffer& buffer, uint32_t size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::store32Bits(&bufferNew, size);
		this->writeToBuffer(buffer, bufferNew);
	}

	template<typename ReturnType> ReturnType ErlPacker::readBits(const ErlPackBuffer& buffer) {
		if (buffer.offSet + sizeof(ReturnType) > buffer.buffer->size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of buffer.\n\n" };
		}
		auto theValue = *reinterpret_cast<ReturnType*>(buffer.buffer->data() + buffer.offSet);
		buffer.offSet += sizeof(ReturnType);
		std::cout << "THE TYPEID: " << typeid(ReturnType).name() << "THE ID:" << std::endl;
		if (typeid(ReturnType).name() != "unsigned char") {
			std::cout << "WERE READING 8 BITS! FAKELY" << std::endl;
			return theValue;
		} else if (typeid(ReturnType).name() == "uint16_t") {
			return DiscordCoreAPI::reverseByteOrder16(theValue);
		} else if (typeid(ReturnType).name() == "uint32_t") {
			return DiscordCoreAPI::reverseByteOrder32(theValue);
		} else if (typeid(ReturnType).name() == "uint64_t") {
			return DiscordCoreAPI::reverseByteOrder64(theValue);
		}
		return ReturnType{};
	}

	const char* ErlPacker::readString(const ErlPackBuffer& buffer, uint32_t length) {
		if (buffer.offSet + static_cast<uint64_t>(length) > buffer.buffer->size()) {
			throw ErlPackError{ "ErlPacker::readString() Error: readString() past end of buffer.\n\n" };
		}
		const char* theString{ buffer.buffer->data() + buffer.offSet };
		buffer.offSet += length;
		return theString;
	}

	nlohmann::json ErlPacker::singleValueETFToJson(const ErlPackBuffer& buffer) {
		if (buffer.offSet >= buffer.buffer->size()) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		uint8_t type = this->readBits<uint8_t>(buffer);
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return this->parseSmallIntegerExt(buffer);
			}
			case ETFTokenType::Integer_Ext: {
				return this->parseIntegerExt(buffer);
			}
			case ETFTokenType::Float_Ext: {
				return this->parseFloatExt(buffer);
			}
			case ETFTokenType::Atom_Ext: {
				return this->parseAtomUtf8Ext(buffer);
			}
			case ETFTokenType::Small_Tuple_Ext: {
				return this->parseSmallTupleExt(buffer);
			}
			case ETFTokenType::Large_Tuple_Ext: {
				return this->parseLargeTupleExt(buffer);
			}
			case ETFTokenType::Nil_Ext: {
				return this->parseNilExt();
			}
			case ETFTokenType::String_Ext: {
				return this->parseStringAsList(buffer);
			}
			case ETFTokenType::List_Ext: {
				return this->parseListExt(buffer);
			}
			case ETFTokenType::Binary_Ext: {
				return this->parseBinaryExt(buffer);
			}
			case ETFTokenType::Small_Big_Ext: {
				return this->parseSmallBigExt(buffer);
			}
			case ETFTokenType::Large_Big_Ext: {
				return this->parseLargeBigExt(buffer);
			}
			case ETFTokenType::Map_Ext: {
				return this->parseMapExt(buffer);
			}
			case ETFTokenType::Atom_Utf8_Ext: {
				return this->parseAtomUtf8Ext(buffer);
			}
			case ETFTokenType::Small_Atom_Utf8_Ext: {
				return this->parseSmallAtomUtf8Ext(buffer);
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	nlohmann::json ErlPacker::parseSmallIntegerExt(const ErlPackBuffer& buffer) {
		uint8_t newValue = this->readBits<uint8_t>(buffer);
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseBigint(const ErlPackBuffer& buffer, uint32_t digits) {
		nlohmann::json jsonData{};
		uint8_t sign = this->readBits<uint8_t>(buffer);
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t x = 0; x < digits; ++x) {
			uint8_t digit = this->readBits<uint8_t>(buffer);
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

	nlohmann::json ErlPacker::parseIntegerExt(const ErlPackBuffer& buffer) {
		uint32_t newValue = this->readBits<uint32_t>(buffer);
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseFloatExt(const ErlPackBuffer& buffer) {
		uint32_t floatLength = 31;
		const char* floatStr = this->readString(buffer, floatLength);
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

	nlohmann::json ErlPacker::parseTuple(const ErlPackBuffer& buffer, uint32_t length) {
		return this->parseArray(buffer, length);
	}

	nlohmann::json ErlPacker::parseSmallTupleExt(const ErlPackBuffer& buffer) {
		uint8_t theValue = this->readBits<uint8_t>(buffer);
		uint32_t theValueNew = theValue;
		return this->parseTuple(buffer, theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeTupleExt(const ErlPackBuffer& buffer) {
		uint32_t theValue = this->readBits<uint32_t>(buffer);
		return this->parseTuple(buffer, theValue);
	}

	nlohmann::json ErlPacker::parseNilExt() {
		return nlohmann::json();
	}

	nlohmann::json ErlPacker::parseStringAsList(const ErlPackBuffer& buffer) {
		uint16_t length = this->readBits<uint16_t>(buffer);
		nlohmann::json theArray = nlohmann::json::array();
		if (static_cast<uint64_t>(buffer.offSet) + length > buffer.buffer->size()) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.push_back(this->parseSmallIntegerExt(buffer));
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseListExt(const ErlPackBuffer& buffer) {
		uint32_t length = this->readBits<uint32_t>(buffer);
		nlohmann::json::array_t theArray = this->parseArray(buffer, length);
		uint8_t theValue = this->readBits<uint8_t>(buffer);
		const ETFTokenType tailMarker = static_cast<ETFTokenType>(theValue);
		if (tailMarker != ETFTokenType::Nil_Ext) {
			return nlohmann::json::array_t();
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseBinaryExt(const ErlPackBuffer& buffer) {
		uint32_t length = this->readBits<uint32_t>(buffer);
		const char* stringNew = this->readString(buffer, length);
		if (length == 0) {
			nlohmann::json jsonData{};
			return jsonData;
		}
		nlohmann::json jsonData = std::string{ stringNew, length };
		return jsonData;
	}

	nlohmann::json ErlPacker::parseSmallBigExt(const ErlPackBuffer& buffer) {
		uint8_t theValue = this->readBits<uint8_t>(buffer);
		uint32_t theValueNew = theValue;
		return this->parseBigint(buffer, theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeBigExt(const ErlPackBuffer& buffer) {
		uint32_t theValue = this->readBits<uint32_t>(buffer);
		return this->parseBigint(buffer, theValue);
	}

	nlohmann::json ErlPacker::parseArray(const ErlPackBuffer& buffer, uint32_t length) {
		nlohmann::json theArray = nlohmann::json::array();
		for (uint32_t x = 0; x < length; ++x) {
			theArray.emplace_back(this->singleValueETFToJson(buffer));
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseMapExt(const ErlPackBuffer& buffer) {
		const uint32_t length = readBits<uint32_t>(buffer);
		auto theMap = nlohmann::json::object();
		for (uint32_t i = 0; i < length; ++i) {
			auto key = singleValueETFToJson(buffer);
			auto value = singleValueETFToJson(buffer);
			if (key.is_number()) {
				theMap[std::to_string(key.get<uint64_t>())] = std::move(value);
			} else {
				theMap[key.get<std::string>()] = std::move(value);
			}
		}
		return theMap;
	}

	nlohmann::json ErlPacker::parseAtomUtf8Ext(const ErlPackBuffer& buffer) {
		uint16_t length = this->readBits<uint16_t>(buffer);
		uint32_t lengthNew = length;
		const char* atom = this->readString(buffer, lengthNew);
		return this->processAtom(atom, lengthNew);
	}

	nlohmann::json ErlPacker::parseSmallAtomUtf8Ext(const ErlPackBuffer& buffer) {
		uint8_t length = this->readBits<uint8_t>(buffer);
		uint32_t lengthNew = length;
		const char* atom = this->readString(buffer, lengthNew);
		return this->processAtom(atom, lengthNew);
	}

}
