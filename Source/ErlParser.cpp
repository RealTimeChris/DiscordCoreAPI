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
/// ErlParser.cpp - Source file for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlParser.cpp

#include <discordcoreapi/ErlParser.hpp>

namespace DiscordCoreInternal {

	ErlParseError::ErlParseError(const std::string& message) : std::runtime_error(message.c_str()){};

	std::string& ErlParser::parseEtfToJson(std::string_view dataToParse) {
		this->offSet = 0;
		this->dataBuffer = dataToParse;
		this->finalString.clear();
		if (this->readBits<uint8_t>() != formatVersion) {
			throw ErlParseError{ "ErlParser::parseEtfToJson() Error: Incorrect format version specified." };
		}
		this->singleValueETFToJson();
		this->finalString.reserve(this->finalString.size() + simdjson::SIMDJSON_PADDING);
		std::cout << "FINAL STRING: " << this->finalString << std::endl;
		return this->finalString;
	}

	void ErlParser::writeCharactersFromBuffer(uint32_t length) {
		if (this->offSet + static_cast<uint64_t>(length) > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlParser::readString() Error: readString() past end of buffer.\n\n" };
		}
		uint64_t finalSize{};
		char* stringNew = ( char* )this->dataBuffer.data() + this->offSet;
		for (uint32_t x = 0; x < length; ++x) {
			switch (stringNew[x]) {
				case 0x00: {
					break;
				}
				case 0x27: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = '\'';
					break;
				}
				case 0x22: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = '"';
					break;
				}
				case 0x5c: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = '\\';
					break;
				}
				case 0x07: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 'a';
					break;
				}
				case 0x08: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 'b';
					break;
				}
				case 0x0C: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 'f';
					break;
				}
				case 0x0A: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 'n';
					break;
				}
				case 0x0D: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 'r';
					break;
				}
				case 0x0B: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 'v';
					break;
				}
				case 0x09: {
					this->stringBuffer[finalSize++] = '\\';
					this->stringBuffer[finalSize++] = 't';
					break;
				}
				default: {
					this->stringBuffer[finalSize++] = stringNew[x];
					break;
				}
			}
		}
		this->offSet += length;
		std::string_view string{ this->stringBuffer.data(), finalSize };
		std::cout << "THE REAL STRING: " << string << std::endl;
		if (this->stringBuffer.empty()) {
			this->writeCharacters("\"\"", 2);
			return;
		}
		if (finalSize >= 3 && finalSize <= 5) {
			if (finalSize == 3 && strncmp(string.data(), "nil", 3) == 0) {
				this->writeCharacters("null", 4);
				return;
			} else if (finalSize == 4 && strncmp(string.data(), "null", 4) == 0) {
				this->writeCharacters("null", 4);
				return;
			} else if (finalSize == 4 && strncmp(string.data(), "true", 4) == 0) {
				this->writeCharacters("true", 4);
				return;
			} else if (finalSize == 5 && strncmp(string.data(), "false", 5) == 0) {
				this->writeCharacters("false", 5);
				return;
			}
		}
		this->writeCharacter('\"');
		this->writeCharacters(string.data(), finalSize);
		this->writeCharacter('\"');
	}

	void ErlParser::writeCharacters(const char* data, std::size_t length) {
		this->finalString.append(data, length);
	}

	void ErlParser::writeCharacter(const char theChar) {
		this->finalString.push_back(theChar);
	}

	void ErlParser::singleValueETFToJson() {
		if (this->offSet > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlParser::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		uint8_t type = this->readBits<uint8_t>();
		std::cout << "THE TYPE: " << type << std::endl;
		switch (static_cast<EtfType>(type)) {
			case EtfType::Small_Integer_Ext: {
				return this->parseSmallIntegerExt();
			}
			case EtfType::Integer_Ext: {
				return this->parseIntegerExt();
			}
			case EtfType::Atom_Ext: {
				return this->parseAtomExt();
			}
			case EtfType::String_Ext: {
				return this->parseStringExt();
			}
			case EtfType::New_Float_Ext: {
				return this->parseNewFloatExt();
			}
			case EtfType::Nil_Ext: {
				return this->parseNilExt();
			}
			case EtfType::List_Ext: {
				return this->parseListExt();
			}
			case EtfType::Binary_Ext: {
				return this->parseBinaryExt();
			}
			case EtfType::Small_Big_Ext: {
				return this->parseSmallBigExt();
			}
			case EtfType::Map_Ext: {
				return this->parseMapExt();
			}
			default: {
				throw ErlParseError{ "ErlParser::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	void ErlParser::parseListExt() {
		std::cout << "WERE HERE PARSELISTEXT" << std::endl;
		uint32_t length = this->readBits<uint32_t>();
		this->writeCharacter('[');
		if (static_cast<uint64_t>(this->offSet) + length > this->dataBuffer.size()){
			throw ErlParseError{ "ErlPacker::parseStringAsList() Error: List reading past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			std::cout << "THE LENGTH: " << length << std::endl;
			this->singleValueETFToJson();
			std::cout << "FINAL STRING: " << this->finalString << std::endl;
		}
		this->writeCharacter(']');
		return;
	}

	void ErlParser::parseSmallIntegerExt() {
		std::cout << "WERE HERE PARSESMALLINTEGEREXT" << std::endl;
		auto string = std::to_string(this->readBits<uint8_t>());
		this->writeCharacters(string.data(), string.size());
	}

	void ErlParser::parseIntegerExt() {
		std::cout << "WERE HERE PARSESINTEGEREXT" << std::endl;
		auto string = std::to_string(this->readBits<uint32_t>());
		this->writeCharacters(string.data(), string.size());
	}

	void ErlParser::parseStringExt() {
		std::cout << "WERE HERE PARSESTRINGEXT" << std::endl;
		this->writeCharacter('\"');
		uint16_t length = this->readBits<uint16_t>();
		if (static_cast<uint64_t>(this->offSet) + length > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlParser::parseStringAsList() Error: std::string reading past end of buffer.\n\n" };
		}
		for (uint16_t x = 0; x < length; ++x) {
			this->parseSmallIntegerExt();
		}
		this->writeCharacter('\"');
	}

	void ErlParser::parseNewFloatExt() {
		std::cout << "WERE HERE PARSENEWFLOAT" << std::endl;
		uint64_t value = readBits<uint64_t>();
		void* ptr{ &value };
		std::string valueNew = std::to_string(*static_cast<double*>(ptr));
		this->writeCharacters(valueNew.data(), valueNew.size());
	}

	void ErlParser::parseBigInt() {
		auto digits = this->readBits<uint8_t>();
		uint8_t sign = this->readBits<uint8_t>();
		if (digits > 128) {
			throw ErlParseError{ "ErlParser::parseBigInt() Error: Integers larger than 8 bytes are not supported.\n\n" };
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
				auto theString = std::to_string(value);
				this->writeCharacters(theString.data(), theString.size());
				return;
			}
			const bool isSignBitAvailable = (value & 1ull << 31ull) == 0;
			if (isSignBitAvailable) {
				auto theString = std::to_string(-static_cast<int32_t>(value));
				this->writeCharacters(theString.data(), theString.size());
				return;
			}
		}
		char outBuffer[32] = { 0 };
		const char* formatString = sign == 0 ? "%llu" : "-%ll";
		auto theValue = sign == 0 ? static_cast<uint64_t>(value) : static_cast<int64_t>(value);
		const int32_t res = sprintf(outBuffer, formatString, theValue);
		if (res < 0) {
			throw ErlParseError{ "ErlParser::parseBigInt() Error: Parse big integer failed.\n\n" };
		}
		const uint8_t length = static_cast<uint8_t>(res);
		this->writeCharacters(outBuffer, length);
	}

	void ErlParser::parseSmallBigExt() {
		std::cout << "WERE HERE PARSESMALLBIGEXT" << std::endl;
		this->writeCharacter('\"');
		this->parseBigInt();
		this->writeCharacter('\"');
	}

	void ErlParser::parseAtomExt() {
		uint32_t length = this->readBits<uint16_t>();
		std::cout << "WERE HERE PARSEATOMEXT: LENGTH: " << length << std::endl;
		this->writeCharactersFromBuffer(length);
	}

	void ErlParser::parseBinaryExt() {
		std::cout << "WERE HERE PARSEBINARYEXT" << std::endl;
		uint32_t length = this->readBits<uint32_t>();
		this->writeCharactersFromBuffer(length);
	}

	void ErlParser::parseNilExt() {
		std::cout << "WERE HERE PARSENILEXT" << std::endl;
		this->writeCharacter('}');
	}

	void ErlParser::parseMapExt() {
		std::cout << "WERE HERE PARSEMAPEXT" << std::endl;
		uint32_t length = readBits<uint32_t>();
		this->writeCharacter('{');
		for (uint32_t i = 0; i < length; ++i) {
			std::cout << "WERE HERE PARSEMAPEXT LENGTH" << length << std::endl;
			this->singleValueETFToJson();
			this->writeCharacter(':');
			this->singleValueETFToJson();
			if (i < length - 1) {
				this->writeCharacter(',');
			}
		}
		this->writeCharacter('}');
	}
}
