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
		explicit ErlPackError(const std::string& message);
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

	class DiscordCoreAPI_Dll ErlPacker {
	  public:
		std::string parseJsonToEtf(const nlohmann::json&);

		nlohmann::json parseEtfToJson(std::string&);

		ErlPacker();

		ErlPacker& operator=(std::string&);

		ErlPacker(std::string&);

	  protected:
		std::string theBufferRef{};
		std::string& buffer;

		mutable uint64_t offSet{};

		void singleValueJsonToETF(const nlohmann::json&);

		void writeToBuffer(const std::string&);

		void appendVersion();

		void appendSmallIntegerExt(uint8_t);

		void appendIntegerExt(uint32_t);

		void appendFloatExt(double);

		void appendNilExt();

		void appendNil();

		void appendFalse();

		void appendTrue();

		void appendUnsignedLongLong(uint64_t);

		void appendBinaryExt(const std::string&, uint32_t);

		void appendListHeader(uint32_t);

		void appendMapHeader(uint32_t);

		uint8_t read8Bits();

		uint16_t read16Bits();

		uint32_t read32Bits();

		uint64_t read64Bits();

		const char* readString(uint32_t length);

		nlohmann::json singleValueETFToJson();

		nlohmann::json parseSmallIntegerExt();

		nlohmann::json parseBigint(uint32_t);

		nlohmann::json parseIntegerExt();

		nlohmann::json parseFloatExt();

		nlohmann::json processAtom(const char* atom, uint32_t length);

		nlohmann::json parseTuple(uint32_t);

		nlohmann::json parseSmallTupleExt();

		nlohmann::json parseLargeTupleExt();

		nlohmann::json parseNilExt();

		nlohmann::json parseStringAsList();

		nlohmann::json parseListExt();

		nlohmann::json parseBinaryExt();

		nlohmann::json parseSmallBigExt();

		nlohmann::json parseLargeBigExt();

		nlohmann::json parseArray(uint32_t);

		nlohmann::json parseMapExt();

		nlohmann::json parseAtomUtf8Ext();

		nlohmann::json parseSmallAtomUtf8Ext();
	};
}// namespace DiscordCoreInternal
