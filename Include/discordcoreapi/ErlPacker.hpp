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
/// ErlPacker.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.hpp

#ifndef ERL_PACKER
#define ERL_PACKER

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	struct ErlPackError : public std::runtime_error {
	  public:
		explicit ErlPackError(const std::string& message);
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Float_Ext = 99,
		Atom_Ext = 100,
		Small_Tuple_Ext = 104,
		Large_Tuple_Ext = 105,
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Large_Big_Ext = 111,
		Small_Atom_Ext = 115,
		Map_Ext = 116,
		Atom_Utf8_Ext = 118
	};

	class DiscordCoreAPI_Dll ErlPacker {
	  public:
		ErlPacker() noexcept {};

		std::string_view parseJsonToEtf(std::string&& dataToParse);

		std::string_view parseEtfToJson(std::string_view dataToParse);

		~ErlPacker(){};

	  protected:
		std::string bufferString{};
		std::string_view buffer{};
		uint64_t stringSize{};
		uint64_t offSet{};
		uint64_t size{};

		void writeToString(const char*, size_t length);

		void singleValueJsonToETF(simdjson::ondemand::value jsonData);

		void writeObject(simdjson::ondemand::value jsonData);

		void writeString(simdjson::ondemand::value jsonData);

		void writeNumber(simdjson::ondemand::value jsonData);

		void writeArray(simdjson::ondemand::value jsonData);

		void writeBool(simdjson::ondemand::value jsonData);

		void writeToBuffer(const std::string&);

		void appendBinaryExt(const std::string&, uint32_t);

		void appendUnsignedLongLong(uint64_t);

		void appendSmallIntegerExt(uint8_t);

		void appendIntegerExt(uint32_t);

		void appendListHeader(uint32_t);

		void appendMapHeader(uint32_t);

		void appendNewFloatExt(double);

		void appendVersion();

		void appendNilExt();

		void appendFalse();

		void appendTrue();

		void appendNil();

		template<typename ReturnType> ReturnType readBits() {
			if (this->offSet + sizeof(ReturnType) > this->size) {
				throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer.\n\n" };
			}
			const ReturnType newValue = *reinterpret_cast<const ReturnType*>(this->buffer.data() + this->offSet);
			this->offSet += sizeof(ReturnType);
			return DiscordCoreAPI::reverseByteOrder<const ReturnType>(newValue);
		}

		size_t readString(uint32_t length);

		void processAtom(const char* atom, uint32_t length);

		void singleValueETFToJson();

		void parseSmallIntegerExt();

		void parseBigint(uint32_t);

		void parseArray(uint32_t);

		void parseTuple(uint32_t);

		void parseSmallTupleExt();

		void parseLargeTupleExt();

		void parseSmallAtomExt();

		void parseStringAsList();

		void parseNewFloatExt();

		void parseSmallBigExt();

		void parseLargeBigExt();

		void parseAtomUtf8Ext();

		void parseIntegerExt();

		void parseBinaryExt();

		void parseFloatExt();

		void parseListExt();

		void parseNilExt();

		void parseMapExt();
	};
}// namespace DiscordCoreInternal
#endif