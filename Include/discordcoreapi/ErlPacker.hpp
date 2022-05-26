/*
*
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
/// ErlPacker.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file ErlPacker.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	struct ErlPackError : public std::runtime_error {
	  public:
		ErlPackError(const std::string& message) : std::runtime_error(message.c_str()){};
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
		New_Float_Ext = 70,
		Bit_Binary = 77,
		Comressed = 80,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Atom = 100,
		Reference = 101,
		Port = 102,
		Pid = 103,
		Small_Tuple = 104,
		Large_Tuple = 105,
		Nil_Ext = 106,
		String = 107,
		List = 108,
		Binary = 109,
		Small_BigInt = 110,
		Large_BigInt = 111,
		New_Function = 112,
		Export = 113,
		New_Reference = 114,
		Small_Atom = 115,
		Map = 116,
		Function = 117,
		Utf8_Atom = 118,
		Small_Utf8_Atom = 119
	};

	struct ErlPackBuffer {
	  public:
		std::string buffer{};

		mutable uint64_t offSet{};

		ErlPackBuffer() = default;
		
		ErlPackBuffer& operator=(const std::string*);

		ErlPackBuffer(const std::string*);

		ErlPackBuffer& operator=(const ErlPackBuffer&) = delete;

		ErlPackBuffer(const ErlPackBuffer&) = delete;

		ErlPackBuffer& operator=(ErlPackBuffer&) = delete;

		ErlPackBuffer(ErlPackBuffer&) = delete;
	};

	class ErlPacker {
	  public:
		std::string parseJsonToEtf(const nlohmann::json&);

		nlohmann::json parseEtfToJson(const std::string*);

	  protected:
		template<typename ReturnType> void etfByteOrder(ReturnType, ReturnType&);

		template<typename ReturnType> void storeBits(std::vector<uint8_t>&, ReturnType&, uint32_t&);

		void singleValueJsonToETF(ErlPackBuffer&, const nlohmann::json&);

		void writeToBuffer(ErlPackBuffer&, const std::vector<uint8_t>&);

		void appendVersion(ErlPackBuffer&);

		void appendFalse(ErlPackBuffer&);

		void appendTrue(ErlPackBuffer&);

		void appendSmallIntegerExt(ErlPackBuffer&, uint8_t&);

		void appendIntegerExt(ErlPackBuffer&, int32_t&);

		void appendNil(ErlPackBuffer& buffer);

		void appendUnsignedLongLong(ErlPackBuffer&, uint64_t&);

		void appendNewFloatExt(ErlPackBuffer&, double&);

		void appendBinary(ErlPackBuffer&, const std::vector<uint8_t>&, uint32_t&);

		void appendNilExt(ErlPackBuffer&);

		void appendListHeader(ErlPackBuffer&, uint32_t);

		void appendMapHeader(ErlPackBuffer&, uint32_t);

		template<typename ReturnType> void readBits(const ErlPackBuffer&, ReturnType&);

		void readString(const ErlPackBuffer&, uint32_t&, std::vector<char>&);

		nlohmann::json singleValueETFToJson(const ErlPackBuffer&);

		nlohmann::json processAtom(const std::vector<char>&, uint32_t&);

		nlohmann::json parseAtom(const ErlPackBuffer&);

		nlohmann::json parseSmallAtom(const ErlPackBuffer&);

		nlohmann::json parseSmallIntegerExt(const ErlPackBuffer&);

		nlohmann::json parseIntegerExt(const ErlPackBuffer&);

		nlohmann::json parseArray(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseList(const ErlPackBuffer&);

		nlohmann::json parseTuple(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseNilExt();

		nlohmann::json parseMap(const ErlPackBuffer&);

		nlohmann::json parseNewFloatExt(const ErlPackBuffer&);

		nlohmann::json parseBigInt(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseSmallBigInt(const ErlPackBuffer&);

		nlohmann::json parseLargeBigInt(const ErlPackBuffer&);

		nlohmann::json parseBinary(const ErlPackBuffer&);

		nlohmann::json parseString(const ErlPackBuffer&);

		nlohmann::json parseStringAsList(const ErlPackBuffer&);

		nlohmann::json parseSmallTuple(const ErlPackBuffer&);

		nlohmann::json parseLargeTuple(const ErlPackBuffer&);
	};
}// namespace DiscordCoreInternal
