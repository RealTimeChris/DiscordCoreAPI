/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
#include <codecvt>

namespace DiscordCoreInternal {

	ErlParseError::ErlParseError(const std::string& message) : DCAException(message){};

	std::string_view ErlParser::parseEtfToJson(std::string_view dataToParse) {
		this->dataBuffer = dataToParse.data();
		this->dataSize = dataToParse.size();
		this->currentSize = 0;
		this->offSet = 0;
		if (this->readBitsFromBuffer<uint8_t>() != formatVersion) {
			throw ErlParseError{ "ErlParser::parseEtfToJson() Error: Incorrect format version specified." };
		}
		this->singleValueETFToJson();
		return std::string_view{ this->finalString.data(), this->currentSize };
	}

	void ErlParser::writeCharacters(const char* data, uint64_t length) {
		if (this->finalString.size() < this->currentSize + length) {
			this->finalString.resize((this->finalString.size() + length) * 2);
		}
		std::memcpy(this->finalString.data() + this->currentSize, data, length);
		this->currentSize += length;
	}
	
	unsigned char ErlParser::hex2dec(char hex) {
		return ((hex & 0xf) + (hex >> 6) * 9);
	}

	char32_t ErlParser::hex4ToChar32(const char* hex) {
		uint32_t value = hex2dec(hex[3]);
		value |= hex2dec(hex[2]) << 4;
		value |= hex2dec(hex[1]) << 8;
		value |= hex2dec(hex[0]) << 12;
		return value;
	}

	template<typename OTy> void ErlParser::readEscapedUnicode(OTy& value, auto&& it) {
		char8_t buffer[4];
		char32_t codepoint = hex4ToChar32(it);
		auto& facet = std::use_facet<std::codecvt<char32_t, char8_t, mbstate_t>>(std::locale());
		std::mbstate_t mbstate{};
		const char32_t* fromNext;
		char8_t* toNext;
		const auto result = facet.out(mbstate, &codepoint, &codepoint + 1, fromNext, buffer, buffer + 4, toNext);
		if (result != std::codecvt_base::ok) {
			return;
		}

		if ((toNext - buffer) != 1) [[unlikely]] {
			return;
		}
		*value = static_cast<char>(buffer[0]);

		std::advance(it, 4);
	}

	void ErlParser::writeCharactersFromBuffer(uint32_t length) {
		if (!length) {
			this->writeCharacters("\"\"", 2);
			return;
		}
		if (this->offSet + static_cast<uint64_t>(length) > this->dataSize) {
			throw ErlParseError{ "ErlPacker::writeCharactersFromBuffer() Error: Read past end of buffer." };
		}
		if (this->finalString.size() < this->currentSize + length) {
			this->finalString.resize((this->finalString.size() + length) * 2);
		}
		const char* stringNew = this->dataBuffer + this->offSet;
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
		auto handleEscaped = [&,this]() {
			switch (*stringNew) {
				case '"':
				case '\\':
				case '/':
					writeCharacter(*stringNew);
					++stringNew;
					break;
				case 'b':
					writeCharacter('\b');
					++stringNew;
					break;
				case 'f':
					writeCharacter('\f');
					++stringNew;
					break;
				case 'n':
					writeCharacter('\n');
					++stringNew;
					break;
				case 'r':
					writeCharacter('\r');
					++stringNew;
					break;
				case 't':
					writeCharacter('\t');
					++stringNew;
					break;
				case 'u': {
					auto newPtr = this->finalString.data() + offSet;
					readEscapedUnicode(newPtr, stringNew);
					++stringNew;
					return;
				}
				default: {
					++stringNew;
					return;
				}
			}
		};
		this->writeCharacter('"');
		for (auto iter = stringNew; iter != stringNew + length;) {
			bool doWeBreak{};
			switch (*iter) {
				case 0x00: {
					doWeBreak = true;
					break;
				}
				case 0x22: {
					this->writeCharacter('\\');
					this->writeCharacter('"');
					++iter;
					break;
				}
				case 0x5c: {
					handleEscaped();
					++iter;
					break;
				}
				case 0x07: {
					this->writeCharacter('\\');
					this->writeCharacter('a');
					++iter;
					break;
				}
				case 0x08: {
					this->writeCharacter('\\');
					this->writeCharacter('b');
					++iter;
					break;
				}
				case 0x0C: {
					this->writeCharacter('\\');
					this->writeCharacter('f');
					++iter;
					break;
				}
				case 0x0A: {
					this->writeCharacter('\\');
					this->writeCharacter('n');
					++iter;
					break;
				}
				case 0x0D: {
					this->writeCharacter('\\');
					this->writeCharacter('r');
					++iter;
					break;
				}
				case 0x0B: {
					this->writeCharacter('\\');
					this->writeCharacter('v');
					++iter;
					break;
				}
				case 0x09: {
					this->writeCharacter('\\');
					this->writeCharacter('t');
					++iter;
					break;
				}
				default: {
					this->writeCharacter(*iter);
					++iter;
				}
					
			}
			if (doWeBreak) {
				break;
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
		if (this->offSet > this->dataSize) {
			throw ErlParseError{ "ErlPacker::singleValueETFToJson() Error: Read past end of buffer." };
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
				throw ErlParseError{ "ErlParser::singleValueETFToJson() Error: Unknown data type in ETF, the type: " + std::to_string(type) };
			}
		}
	}

	void ErlParser::parseListExt() {
		uint32_t length = this->readBitsFromBuffer<uint32_t>();
		this->writeCharacter('[');
		if (static_cast<uint64_t>(this->offSet) + length > this->dataSize) {
			throw ErlParseError{ "ErlPacker::parseListExt() Error: Read past end of buffer." };
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
		if (static_cast<uint64_t>(this->offSet) + length > this->dataSize) {
			throw ErlParseError{ "ErlPacker::parseStringExt() Error: Read past end of buffer." };
		}
		for (uint16_t x = 0; x < length; ++x) {
			this->parseSmallIntegerExt();
		}
		this->writeCharacter('"');
	}

	void ErlParser::parseNewFloatExt() {
		uint64_t value = readBitsFromBuffer<uint64_t>();
		auto newDouble = *reinterpret_cast<double*>(value);
		std::string valueNew = std::to_string(newDouble);
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
		this->writeCharacters("[]", 2);
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
