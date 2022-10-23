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
/// ErlParser.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlParser.hpp

#pragma once

#ifndef ERL_PACKER
	#define ERL_PACKER

	#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	struct ErlParseError : public std::runtime_error {
	  public:
		explicit ErlParseError(const std::string& message);
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class EtfType : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Atom_Ext = 100,
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Map_Ext = 116,
	};

	class DiscordCoreAPI_Dll ErlParser {
	  public:
		ErlParser() noexcept {};

		std::string& parseEtfToJson(std::string_view dataToParse);

		~ErlParser(){};

	  protected:
		std::array<char, 1024 * 16> stringBuffer{};
		std::string_view dataBuffer{};
		std::string finalString{};
		uint64_t offSet{};

		template<typename RTy> RTy readBits() {
			if (this->offSet + sizeof(RTy) > this->dataBuffer.size()) {
				throw ErlParseError{ "ErlParser::readBits() Error: readBits() past end of the buffer.\n\n" };
			}
			const RTy newValue = *reinterpret_cast<const RTy*>(this->dataBuffer.data() + this->offSet);
			this->offSet += sizeof(RTy);
			return DiscordCoreAPI::reverseByteOrder<const RTy>(newValue);
		}

		void writeCharactersFromBuffer(uint32_t length);

		void writeCharacters(const char* data, std::size_t length);

		void writeCharacter(const char theChar);

		void singleValueETFToJson();

		void parseBigInt();

		void parseIntegerExt();

		void parseSmallIntegerExt();

		void parseNewFloatExt();

		void parseSmallBigExt();

		void parseStringExt();

		void parseBinaryExt();

		void parseListExt();

		void parseAtomExt();

		void parseNilExt();

		void parseMapExt();
	};

}// namespace DiscordCoreInternal
#endif