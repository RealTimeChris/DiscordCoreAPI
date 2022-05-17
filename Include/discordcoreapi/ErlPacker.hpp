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
		ErlPackError( const std::string& message) : std::runtime_error(message.c_str()){};
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
		New_Float = 70,
		Bit_Binary = 77,
		Comressed = 80,
		Small_Integer = 97,
		Integer = 98,
		Float = 99,
		Atom = 100,
		Reference = 101,
		Port = 102,
		Pid = 103,
		Small_Tuple = 104,
		Large_Tuple = 105,
		Nil = 106,
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

	struct DiscordCoreAPI_Dll ErlPackBuffer {
	  public:
		std::string buffer{};

		mutable uint64_t offSet{};

		ErlPackBuffer() = default;

		ErlPackBuffer(const std::string*);

		ErlPackBuffer& operator=(ErlPackBuffer&&) noexcept;

		ErlPackBuffer(ErlPackBuffer&&) noexcept;

		ErlPackBuffer& operator=(const ErlPackBuffer&) = delete;

		ErlPackBuffer(const ErlPackBuffer&) = delete;

		ErlPackBuffer& operator=(ErlPackBuffer&) = delete;

		ErlPackBuffer(ErlPackBuffer&) = delete;
	};

	class DiscordCoreAPI_Dll ErlPacker {
	  public:
		std::string parseJsonToEtf(const nlohmann::json&);

		nlohmann::json parseEtfToJson(const std::string*);

	  protected:
		template<typename ReturnType> void etfByteOrder(ReturnType, ReturnType&);

		template<typename ReturnType> void storeBits(std::vector<uint8_t>&, ReturnType&, uint32_t&);

		void singleValueJsonToETF(ErlPackBuffer&, const nlohmann::json&);

		void writeToBuffer(ErlPackBuffer&, const std::vector<uint8_t>&);

		void appendVersion(ErlPackBuffer&);

		void appendNil(ErlPackBuffer&);

		void appendFalse(ErlPackBuffer&);

		void appendTrue(ErlPackBuffer&);

		void appendSmallInteger(ErlPackBuffer&, uint8_t&);

		void appendInteger(ErlPackBuffer&, uint32_t&);

		void appendUnsignedLongLong(ErlPackBuffer&, uint64_t&);

		void appendDouble(ErlPackBuffer&, double&);

		void appendBinary(ErlPackBuffer&, const std::vector<uint8_t>&, uint32_t&);

		void appendNilExt(ErlPackBuffer&);

		void appendListHeader(ErlPackBuffer&, uint32_t&);

		void appendMapHeader(ErlPackBuffer&, uint32_t&);

		template<typename ReturnType> void readBits(const ErlPackBuffer&, ReturnType&);

		void readString(const ErlPackBuffer&, uint32_t&, std::vector<char>&);

		nlohmann::json singleValueETFToJson(const ErlPackBuffer&);

		nlohmann::json processAtom(std::vector<char>&, uint32_t&);

		nlohmann::json parseAtom(const ErlPackBuffer&);

		nlohmann::json parseSmallAtom(const ErlPackBuffer&);

		nlohmann::json parseSmallInteger(const ErlPackBuffer&);

		nlohmann::json parseInteger(const ErlPackBuffer&);

		nlohmann::json parseArray(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseList(const ErlPackBuffer&);

		nlohmann::json parseTuple(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseNil();

		nlohmann::json parseMap(const ErlPackBuffer&);

		nlohmann::json parseFloat(const ErlPackBuffer&);

		nlohmann::json parseNewFloat(const ErlPackBuffer&);

		nlohmann::json parseBigint(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseSmallBigint(const ErlPackBuffer&);

		nlohmann::json parseLargeBigint(const ErlPackBuffer&);

		nlohmann::json parseBinary(const ErlPackBuffer&);

		nlohmann::json parseString(const ErlPackBuffer&);

		nlohmann::json parseStringAsList(const ErlPackBuffer&);

		nlohmann::json parseSmallTuple(const ErlPackBuffer&);

		nlohmann::json parseLargeTuple(const ErlPackBuffer&);
	};
}// namespace DiscordCoreInternal
