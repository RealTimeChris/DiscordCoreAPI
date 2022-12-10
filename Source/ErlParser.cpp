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

	ErlParseError::ErlParseError(const std::string& message) : DCAException(message){};

	std::string_view ErlParser::parseEtfToJson(std::string_view dataToParse) {
		this->dataBuffer = dataToParse;
		this->currentSize = 0;
		this->offSet = 0;
		if (this->readBitsFromBuffer<uint8_t>() != formatVersion) {
			throw ErlParseError{ "ErlParser::parseEtfToJson() Error: Incorrect format version specified." };
		}
		this->singleValueETFToJson();
		return std::string_view{ this->finalString.data(), this->currentSize };
	}

	void ErlParser::writeCharacters(const char* data, size_t length) {
		if (this->finalString.size() < this->currentSize + length) {
			this->finalString.resize((this->finalString.size() + length) * 2);
		}
		std::copy(data, data + length, this->finalString.data() + this->currentSize);
		this->currentSize += length;
	}

	void ErlParser::writeCharactersFromBuffer(uint32_t length) {
		if (!length) {
			this->writeCharacters("\"\"", 2);
			return;
		}
		if (this->offSet + static_cast<uint64_t>(length) > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlParser::writeCharactersFromBuffer() Error: readString() past end of buffer." };
		}
		if (this->finalString.size() < this->currentSize + length) {
			this->finalString.resize((this->finalString.size() + length) * 2);
		}
		const char* stringNew = this->dataBuffer.data() + this->offSet;
		this->offSet += length;
		if (length >= 3 && length <= 5) {
			if (length == 3 && strncmp(stringNew, "nil", 3) == 0) {
				this->writeCharacters("null", 4);
				return;
			} else if (length == 4 && strncmp(stringNew, "null", 4) == 0) {
				this->writeCharacters("null", 4);
				return;
			} else if (length == 4 && strncmp(stringNew, "true", 4) == 0) {
				this->writeCharacters("true", 4);
				return;
			} else if (length == 5 && strncmp(stringNew, "false", 5) == 0) {
				this->writeCharacters("false", 5);
				return;
			}
		}
		this->writeCharacter('"');
		for (size_t x = 0; x < length; ++x) {
			switch (stringNew[x]) {
				case 0x00: {
					break;
				}
				case 0x27: {
					this->writeCharacter('\\');
					this->writeCharacter('\'');
					break;
				}
				case 0x22: {
					this->writeCharacter('\\');
					this->writeCharacter('"');
					break;
				}
				case 0x5c: {
					this->writeCharacter('\\');
					this->writeCharacter('\\');
					break;
				}
				case 0x07: {
					this->writeCharacter('\\');
					this->writeCharacter('a');
					break;
				}
				case 0x08: {
					this->writeCharacter('\\');
					this->writeCharacter('b');
					break;
				}
				case 0x0C: {
					this->writeCharacter('\\');
					this->writeCharacter('f');
					break;
				}
				case 0x0A: {
					this->writeCharacter('\\');
					this->writeCharacter('n');
					break;
				}
				case 0x0D: {
					this->writeCharacter('\\');
					this->writeCharacter('r');
					break;
				}
				case 0x0B: {
					this->writeCharacter('\\');
					this->writeCharacter('v');
					break;
				}
				case 0x09: {
					this->writeCharacter('\\');
					this->writeCharacter('t');
					break;
				}
				default: {
					this->writeCharacter(stringNew[x]);
					break;
				}
			}
		}
		this->writeCharacter('"');
	}

	void ErlParser::writeCharacter(const char value) {
		if (this->finalString.size() < this->currentSize + 1) {
			this->finalString.resize((this->finalString.size() + 1) * 2);
		}
		this->finalString[this->currentSize++] = value;
	}

	void ErlParser::singleValueETFToJson() {
		if (this->offSet > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlParser::singleValueETFToJson() Error: Read past end of ETF buffer." };
		}
		uint8_t type = this->readBitsFromBuffer<uint8_t>();
		switch (static_cast<DiscordCoreAPI::EtfType>(type)) {
			case DiscordCoreAPI::EtfType::New_Float_Ext: {
				return this->parseNewFloatExt();
			}
			case DiscordCoreAPI::EtfType::Small_Integer_Ext: {
				return this->parseSmallIntegerExt();
			}
			case DiscordCoreAPI::EtfType::Integer_Ext: {
				return this->parseIntegerExt();
			}
			case DiscordCoreAPI::EtfType::Atom_Ext: {
				return this->parseAtomExt();
			}
			case DiscordCoreAPI::EtfType::Nil_Ext: {
				return this->parseNilExt();
			}
			case DiscordCoreAPI::EtfType::String_Ext: {
				return this->parseStringExt();
			}
			case DiscordCoreAPI::EtfType::List_Ext: {
				return this->parseListExt();
			}
			case DiscordCoreAPI::EtfType::Binary_Ext: {
				return this->parseBinaryExt();
			}
			case DiscordCoreAPI::EtfType::Small_Big_Ext: {
				return this->parseSmallBigExt();
			}
			case DiscordCoreAPI::EtfType::Small_Atom_Ext: {
				return this->parseSmallAtomExt();
			}
			case DiscordCoreAPI::EtfType::Map_Ext: {
				return this->parseMapExt();
			}
			default: {
				throw ErlParseError{ "ErlParser::singleValueETFToJson() Error: Unknown data type in ETF, the type: " + std::to_string(type) +
					"" };
			}
		}
	}

	void ErlParser::parseListExt() {
		uint32_t length = this->readBitsFromBuffer<uint32_t>();
		this->writeCharacter('[');
		if (static_cast<uint64_t>(this->offSet) + length > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlPacker::parseListExt() Error: List reading past end of buffer." };
		}
		for (uint16_t x = 0; x < length; ++x) {
			this->singleValueETFToJson();
			if (x < length - 1) {
				this->writeCharacter(',');
			}
		}
		this->readBitsFromBuffer<uint8_t>();
		this->writeCharacter(']');
	}

	void ErlParser::parseSmallIntegerExt() {
		auto string = std::to_string(this->readBitsFromBuffer<uint8_t>());
		this->writeCharacters(string.data(), string.size());
	}

	void ErlParser::parseIntegerExt() {
		auto string = std::to_string(this->readBitsFromBuffer<uint32_t>());
		this->writeCharacters(string.data(), string.size());
	}

	void ErlParser::parseStringExt() {
		this->writeCharacter('"');
		uint16_t length = this->readBitsFromBuffer<uint16_t>();
		if (static_cast<uint64_t>(this->offSet) + length > this->dataBuffer.size()) {
			throw ErlParseError{ "ErlParser::parseStringExt() Error: std::string reading past end of buffer." };
		}
		for (uint16_t x = 0; x < length; ++x) {
			this->parseSmallIntegerExt();
		}
		this->writeCharacter('"');
	}

	void ErlParser::parseNewFloatExt() {
		uint64_t value = readBitsFromBuffer<uint64_t>();
		void* ptr{ &value };
		std::string valueNew = std::to_string(*static_cast<double*>(ptr));
		this->writeCharacters(valueNew.data(), valueNew.size());
	}

	void ErlParser::parseSmallBigExt() {
		this->writeCharacter('"');
		auto digits = readBitsFromBuffer<uint8_t>();
		uint8_t sign = readBitsFromBuffer<uint8_t>();


		if (digits > 8) {
			throw ErlParseError{ "ErlParser::parseSmallBigExt() Error: Big integers larger than 8 bytes not supported." };
		}

		uint64_t value = 0;
		uint64_t bits = 1;
		for (uint8_t i = 0; i < digits; ++i) {
			uint64_t digit = readBitsFromBuffer<uint8_t>();
			value += digit * bits;
			bits <<= 8;
		}

		if (sign == 0) {
			auto string = std::to_string(value);
			this->writeCharacters(string.data(), string.size());
			this->writeCharacter('"');
		} else {
			auto string = std::to_string(-(static_cast<int64_t>(value)));
			this->writeCharacters(string.data(), string.size());
			this->writeCharacter('"');
		}
	}

	void ErlParser::parseAtomExt() {
		this->writeCharactersFromBuffer(this->readBitsFromBuffer<uint16_t>());
	}

	void ErlParser::parseBinaryExt() {
		this->writeCharactersFromBuffer(this->readBitsFromBuffer<uint32_t>());
	}

	void ErlParser::parseNilExt() {
		this->writeCharacters("null", 4);
	}

	void ErlParser::parseSmallAtomExt() {
		this->writeCharactersFromBuffer(this->readBitsFromBuffer<uint8_t>());
	}

	void ErlParser::parseMapExt() {
		uint32_t length = readBitsFromBuffer<uint32_t>();
		this->writeCharacter('{');
		for (uint32_t x = 0; x < length; ++x) {
			this->singleValueETFToJson();
			this->writeCharacter(':');
			this->singleValueETFToJson();
			if (x < length - 1) {
				this->writeCharacter(',');
			}
		}
		this->writeCharacter('}');
	}
}
