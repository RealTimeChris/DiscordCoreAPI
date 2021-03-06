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

	ErlPackBuffer& ErlPackBuffer::operator=(const std::string* theBuffer) {
		this->buffer.insert(this->buffer.begin(), theBuffer->begin(), theBuffer->end());
		return *this;
	};

	ErlPackBuffer::ErlPackBuffer(const std::string* theBuffer) {
		*this = theBuffer;
	};

	std::string ErlPacker::parseJsonToEtf(const nlohmann::json& dataToParse) {
		ErlPackBuffer buffer{};
		ErlPacker::appendVersion(buffer);
		ErlPacker::singleValueJsonToETF(buffer, dataToParse);
		return buffer.buffer;
	}

	nlohmann::json ErlPacker::parseEtfToJson(const std::string* dataToParse) {
		ErlPackBuffer buffer{ dataToParse };
		uint8_t version{};
		ErlPacker::readBits(buffer, version);
		return ErlPacker::singleValueETFToJson(buffer);
	}

	void ErlPacker::singleValueJsonToETF(ErlPackBuffer& buffer, const nlohmann::json& jsonData) {
		if (jsonData.is_array()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length == 0) {
				ErlPacker::appendNilExt(buffer);
			} else {
				if (length > std::numeric_limits<uint32_t>::max() - 1) {
					throw ErlPackError{ "ErlPacker::singleValueJsonToETF() Error: List too large for ETF.\n\n" };
				}
			}
			ErlPacker::appendListHeader(buffer, length);
			for (uint64_t index = 0; index < length; ++index) {
				ErlPacker::singleValueJsonToETF(buffer, jsonData[index]);
			}
			ErlPacker::appendNilExt(buffer);
		} else if (jsonData.is_object()) {
			uint32_t length = static_cast<uint32_t>(jsonData.size());
			if (length > std::numeric_limits<uint32_t>::max() - 1) {
				throw ErlPackError{ "ErlPacker::singleValueJsonToETF() Error: Map too large for ETF.\n\n" };
			}
			ErlPacker::appendMapHeader(buffer, length);
			for (auto n = jsonData.begin(); n != jsonData.end(); ++n) {
				nlohmann::json jstr = n.key();
				ErlPacker::singleValueJsonToETF(buffer, jstr);
				ErlPacker::singleValueJsonToETF(buffer, n.value());
			}
		} else if (jsonData.is_number_integer()) {
			uint64_t numberOld = jsonData.get<uint64_t>();
			if (numberOld <= 127) {
				uint8_t number = jsonData.get<uint8_t>();
				ErlPacker::appendSmallIntegerExt(buffer, number);
			} else if (jsonData.is_number_unsigned() && (numberOld >= std::numeric_limits<uint32_t>::max() - static_cast<size_t>(1))) {
				uint64_t number = jsonData.get<uint64_t>();
				ErlPacker::appendUnsignedLongLong(buffer, number);
			} else {
				uint32_t number = jsonData.get<uint32_t>();
				ErlPacker::appendIntegerExt(buffer, number);
			}
		} else if (jsonData.is_boolean()) {
			if (jsonData.get<bool>()) {
				ErlPacker::appendTrue(buffer);
			} else {
				ErlPacker::appendFalse(buffer);
			}
		} else if (jsonData.is_string()) {
			std::string newString = jsonData.get<std::string>();
			std::string newVector{};
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			uint32_t newValue = static_cast<uint32_t>(newVector.size());
			ErlPacker::appendBinaryExt(buffer, newVector, newValue);
		} else if (jsonData.is_number_float()) {
			double newValue = jsonData.get<double>();
			ErlPacker::appendFloatExt(buffer, newValue);
		} else if (jsonData.is_null()) {
			ErlPacker::appendNil(buffer);
		}
	}

	void ErlPacker::writeToBuffer(ErlPackBuffer& buffer, const std::string& bytes) {
		buffer.buffer.insert(buffer.buffer.begin() + buffer.offSet, bytes.begin(), bytes.end());
		buffer.offSet += static_cast<uint32_t>(bytes.size());
	}

	void ErlPacker::appendVersion(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<char>(formatVersion) };
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendSmallIntegerExt(ErlPackBuffer& buffer, uint8_t& value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendIntegerExt(ErlPackBuffer& buffer, uint32_t& value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Integer_Ext) };
		uint32_t newValue{ 1 };
		DiscordCoreAPI::storeBits(bufferNew, value);
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendFloatExt(ErlPackBuffer& buffer, double& value) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Float_Ext) };
		void* doubleValue{ &value };
		DiscordCoreAPI::storeBits(bufferNew, *static_cast<uint64_t*>(doubleValue));
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendNilExt(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Nil_Ext) };
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendNil(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 3, static_cast<uint8_t>('n'), static_cast<uint8_t>('i'), static_cast<uint8_t>('l') };
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendFalse(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 5, static_cast<uint8_t>('f'), static_cast<uint8_t>('a'), static_cast<uint8_t>('l'),
			static_cast<uint8_t>('s'), static_cast<uint8_t>('e') };
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendTrue(ErlPackBuffer& buffer) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Small_Atom_Utf8_Ext), 4, static_cast<uint8_t>('t'), static_cast<uint8_t>('r'), static_cast<uint8_t>('u'),
			static_cast<uint8_t>('e') };
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendUnsignedLongLong(ErlPackBuffer& buffer, uint64_t& value) {
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
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendBinaryExt(ErlPackBuffer& buffer, const std::string& bytes, uint32_t& size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Binary_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, size);
		ErlPacker::writeToBuffer(buffer, bufferNew);
		ErlPacker::writeToBuffer(buffer, bytes);
	}

	void ErlPacker::appendListHeader(ErlPackBuffer& buffer, uint32_t& size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, size);
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	void ErlPacker::appendMapHeader(ErlPackBuffer& buffer, uint32_t& size) {
		std::string bufferNew{ static_cast<uint8_t>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, size);
		ErlPacker::writeToBuffer(buffer, bufferNew);
	}

	template<typename ReturnType> void ErlPacker::readBits(const ErlPackBuffer& buffer, ReturnType& theValue) {
		const uint8_t byteSize{ 8 };
		if (buffer.offSet + sizeof(ReturnType) > buffer.buffer.size()) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of buffer.\n\n" };
		}
		ReturnType newValue{ 0 };
		for (uint64_t x = 0; x < sizeof(ReturnType); x++) {
			newValue |= static_cast<ReturnType>(static_cast<uint64_t>(buffer.buffer.data()[buffer.offSet + x]) << (x * static_cast<uint64_t>(byteSize)));
		}
		buffer.offSet += sizeof(ReturnType);
		theValue = DiscordCoreAPI::reverseByteOrder(newValue);
	}

	void ErlPacker::readString(const ErlPackBuffer& buffer, uint32_t& length, std::string& theString) {
		if (buffer.offSet + static_cast<uint64_t>(length) > buffer.buffer.size()) {
			throw ErlPackError{ "ErlPacker::readString() Error: readString() past end of buffer.\n\n" };
		}
		std::string str{};
		str.insert(str.begin(), buffer.buffer.begin() + buffer.offSet, buffer.buffer.begin() + buffer.offSet + length);
		buffer.offSet += length;
		theString = std::move(str);
	}

	nlohmann::json ErlPacker::singleValueETFToJson(const ErlPackBuffer& buffer) {
		if (buffer.offSet >= buffer.buffer.size()) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		uint8_t type{};
		ErlPacker::readBits(buffer, type);
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return ErlPacker::parseSmallIntegerExt(buffer);
			}
			case ETFTokenType::Integer_Ext: {
				return ErlPacker::parseIntegerExt(buffer);
			}
			case ETFTokenType::Float_Ext: {
				return ErlPacker::parseFloatExt(buffer);
			}
			case ETFTokenType::Atom_Ext: {
				return ErlPacker::parseAtomUtf8Ext(buffer);
			}
			case ETFTokenType::Small_Tuple_Ext: {
				return ErlPacker::parseSmallTupleExt(buffer);
			}
			case ETFTokenType::Large_Tuple_Ext: {
				return ErlPacker::parseLargeTupleExt(buffer);
			}
			case ETFTokenType::Nil_Ext: {
				return ErlPacker::parseNilExt();
			}
			case ETFTokenType::String_Ext: {
				return ErlPacker::parseStringAsList(buffer);
			}
			case ETFTokenType::List_Ext: {
				return ErlPacker::parseListExt(buffer);
			}
			case ETFTokenType::Binary_Ext: {
				return ErlPacker::parseBinaryExt(buffer);
			}
			case ETFTokenType::Small_Big_Ext: {
				return ErlPacker::parseSmallBigExt(buffer);
			}
			case ETFTokenType::Large_Big_Ext: {
				return ErlPacker::parseLargeBigExt(buffer);
			}
			case ETFTokenType::Map_Ext: {
				return ErlPacker::parseMapExt(buffer);
			}
			case ETFTokenType::Atom_Utf8_Ext: {
				return ErlPacker::parseAtomUtf8Ext(buffer);
			}
			case ETFTokenType::Small_Atom_Utf8_Ext: {
				return ErlPacker::parseSmallAtomUtf8Ext(buffer);
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	nlohmann::json ErlPacker::parseSmallIntegerExt(const ErlPackBuffer& buffer) {
		uint8_t newValue{};
		readBits(buffer, newValue);
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseBigint(const ErlPackBuffer& buffer, uint32_t& digits) {
		uint8_t sign{};
		readBits(buffer, sign);
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		uint64_t value = 0;
		uint64_t b = 1;
		for (uint32_t x = 0; x < digits; ++x) {
			uint8_t digit{};
			readBits(buffer, digit);
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

	nlohmann::json ErlPacker::parseIntegerExt(const ErlPackBuffer& buffer) {
		uint32_t newValue{};
		readBits(buffer, newValue);
		nlohmann::json jsonData = newValue;
		return jsonData;
	}

	nlohmann::json ErlPacker::parseFloatExt(const ErlPackBuffer& buffer) {
		uint32_t floatLength = 31;
		std::string floatStr{};
		nlohmann::json jsonData{};
		readString(buffer, floatLength, floatStr);
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

	nlohmann::json ErlPacker::processAtom(const std::string& atom, uint32_t& length) {
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

	nlohmann::json ErlPacker::parseTuple(const ErlPackBuffer& buffer, uint32_t& length) {
		return parseArray(buffer, length);
	}

	nlohmann::json ErlPacker::parseSmallTupleExt(const ErlPackBuffer& buffer) {
		uint8_t theValue{};
		readBits(buffer, theValue);
		uint32_t theValueNew = theValue;
		return parseTuple(buffer, theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeTupleExt(const ErlPackBuffer& buffer) {
		uint32_t theValue{};
		readBits(buffer, theValue);
		return parseTuple(buffer, theValue);
	}

	nlohmann::json ErlPacker::parseNilExt() {
		return nlohmann::json();
	}

	nlohmann::json ErlPacker::parseStringAsList(const ErlPackBuffer& buffer) {
		uint16_t length{};
		readBits(buffer, length);
		nlohmann::json theArray = nlohmann::json::array();
		if (static_cast<uint64_t>(buffer.offSet) + length > buffer.buffer.size()) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			theArray.push_back(parseSmallIntegerExt(buffer));
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseListExt(const ErlPackBuffer& buffer) {
		uint32_t length{};
		readBits(buffer, length);
		nlohmann::json::array_t theArray = parseArray(buffer, length);
		uint8_t theValue{};
		readBits(buffer, theValue);
		const ETFTokenType tailMarker = static_cast<ETFTokenType>(theValue);
		if (tailMarker != ETFTokenType::Nil_Ext) {
			return nlohmann::json::array_t();
		}
		return theArray;
	}

	nlohmann::json ErlPacker::parseBinaryExt(const ErlPackBuffer& buffer) {
		uint32_t length{};
		readBits(buffer, length);
		std::string stringNew{};
		readString(buffer, length, stringNew);
		if (stringNew.size() == 0) {
			nlohmann::json jsonData{};
			return jsonData;
		}
		nlohmann::json jsonData = std::string(stringNew.data(), length);
		return jsonData;
	}

	nlohmann::json ErlPacker::parseSmallBigExt(const ErlPackBuffer& buffer) {
		uint8_t theValue{};
		readBits(buffer, theValue);
		uint32_t theValueNew = theValue;
		return parseBigint(buffer, theValueNew);
	}

	nlohmann::json ErlPacker::parseLargeBigExt(const ErlPackBuffer& buffer) {
		uint32_t theValue{};
		readBits(buffer, theValue);
		return parseBigint(buffer, theValue);
	}

	nlohmann::json ErlPacker::parseArray(const ErlPackBuffer& buffer, uint32_t& length) {
		nlohmann::json array = nlohmann::json::array();
		for (uint32_t x = 0; x < length; ++x) {
			array.emplace_back(singleValueETFToJson(buffer));
		}
		return array;
	}

	nlohmann::json ErlPacker::parseMapExt(const ErlPackBuffer& buffer) {
		uint32_t length{};
		readBits(buffer, length);
		std::unordered_map<nlohmann::json, nlohmann::json> theMap{};
		for (uint32_t x = 0; x < length; ++x) {
			const nlohmann::json key = singleValueETFToJson(buffer);
			const nlohmann::json value = singleValueETFToJson(buffer);
			theMap[key] = value;
		}
		return theMap;
	}

	nlohmann::json ErlPacker::parseAtomUtf8Ext(const ErlPackBuffer& buffer) {
		uint16_t length{};
		readBits(buffer, length);
		uint32_t lengthNew = length;
		std::string atom{};
		readString(buffer, lengthNew, atom);
		return processAtom(atom, lengthNew);
	}

	nlohmann::json ErlPacker::parseSmallAtomUtf8Ext(const ErlPackBuffer& buffer) {
		uint8_t length{};
		readBits(buffer, length);
		uint32_t lengthNew = length;
		std::string atom{};
		readString(buffer, lengthNew, atom);
		return processAtom(atom, lengthNew);
	}

}
