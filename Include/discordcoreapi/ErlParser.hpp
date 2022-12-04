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

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ErlParseError : public DiscordCoreAPI::DCAException {
	  public:
		explicit ErlParseError(const std::string& message);
	};

	const uint8_t formatVersion{ 131 };

	class DiscordCoreAPI_Dll ErlParser {
	  public:
		std::string_view parseEtfToJson(std::string_view dataToParse);

	  protected:
		std::array<char, 1024 * 16> stringBuffer{};
		std::string_view dataBuffer{};
		std::string finalString{};
		size_t currentSize{};
		uint64_t offSet{};

		template<typename RTy> RTy readBitsFromBuffer() {
			if (this->offSet + sizeof(RTy) > this->dataBuffer.size()) {
				throw ErlParseError{ "ErlParser::readBitsFromBuffer() Error: readBitsFromBuffer() past end of the buffer.\n\n" };
			}
			RTy newValue = *reinterpret_cast<const RTy*>(this->dataBuffer.data() + this->offSet);
			this->offSet += sizeof(RTy);
			DiscordCoreAPI::reverseByteOrder<RTy>(newValue);
			return newValue;
		}

		void writeCharacters(const char* data, size_t length);

		void writeCharactersFromBuffer(uint32_t length);

		void writeCharacter(const char value);

		void singleValueETFToJson();

		void parseSmallIntegerExt();

		void parseSmallAtomExt();

		void parseNewFloatExt();

		void parseSmallBigExt();

		void parseIntegerExt();

		void parseStringExt();

		void parseBinaryExt();

		void parseListExt();

		void parseAtomExt();

		void parseNilExt();

		void parseMapExt();
	};

}// namespace DiscordCoreInternal
