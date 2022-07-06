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
/// ErlPacker.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	struct ErlPackError : public std::runtime_error {
	  public:
		ErlPackError(const std::string& message);
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
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
		Map_Ext = 116,
		Atom_Utf8_Ext = 118,
		Small_Atom_Utf8_Ext = 119
	};

	struct DiscordCoreAPI_Dll ErlPackBuffer {
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

	class DiscordCoreAPI_Dll ErlPacker {
	  public:
		std::string parseJsonToEtf(const nlohmann::json&);

		nlohmann::json parseEtfToJson(const std::string*);

	  protected:
		void singleValueJsonToETF(ErlPackBuffer&, const nlohmann::json&);

		void writeToBuffer(ErlPackBuffer&, const std::string&);

		void appendVersion(ErlPackBuffer&);

		void appendSmallIntegerExt(ErlPackBuffer&, uint8_t&);

		void appendIntegerExt(ErlPackBuffer&, uint32_t&);

		void appendFloatExt(ErlPackBuffer&, double&);

		void appendNilExt(ErlPackBuffer&);

		void appendNil(ErlPackBuffer&);

		void appendFalse(ErlPackBuffer&);

		void appendTrue(ErlPackBuffer&);

		void appendUnsignedLongLong(ErlPackBuffer&, uint64_t&);

		void appendBinaryExt(ErlPackBuffer&, const std::string&, uint32_t&);

		void appendListHeader(ErlPackBuffer&, uint32_t&);

		void appendMapHeader(ErlPackBuffer&, uint32_t&);

		template<typename ReturnType> void readBits(const ErlPackBuffer&, ReturnType&);

		void readString(const ErlPackBuffer&, uint32_t&, std::string&);

		nlohmann::json singleValueETFToJson(const ErlPackBuffer&);

		nlohmann::json parseSmallIntegerExt(const ErlPackBuffer&);

		nlohmann::json parseBigint(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseIntegerExt(const ErlPackBuffer&);

		nlohmann::json parseFloatExt(const ErlPackBuffer&);

		nlohmann::json processAtom(const std::string&, uint32_t&);

		nlohmann::json parseTuple(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseSmallTupleExt(const ErlPackBuffer&);

		nlohmann::json parseLargeTupleExt(const ErlPackBuffer&);

		nlohmann::json parseNilExt();

		nlohmann::json parseStringAsList(const ErlPackBuffer&);

		nlohmann::json parseListExt(const ErlPackBuffer&);

		nlohmann::json parseBinaryExt(const ErlPackBuffer&);

		nlohmann::json parseSmallBigExt(const ErlPackBuffer&);

		nlohmann::json parseLargeBigExt(const ErlPackBuffer&);

		nlohmann::json parseArray(const ErlPackBuffer&, uint32_t&);

		nlohmann::json parseMapExt(const ErlPackBuffer&);

		nlohmann::json parseAtomUtf8Ext(const ErlPackBuffer&);

		nlohmann::json parseSmallAtomUtf8Ext(const ErlPackBuffer&);
	};
}// namespace DiscordCoreInternal
