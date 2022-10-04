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

#pragma once

#ifndef ERL_PACKER
	#define ERL_PACKER

	#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	struct ErlPackError : public std::runtime_error {
	  public:
		explicit ErlPackError(const String& message);
	};

	constexpr Uint8 formatVersion{ 131 };

	enum class ETFTokenType : Uint8 {
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

		String& parseJsonToEtf(DiscordCoreAPI::JsonObject&& dataToParse);

		String& parseEtfToJson(StringView dataToParse);

		~ErlPacker(){};

	  protected:
		String comparisongStringFalse{ "false" };
		String comparisongStringNil{ "nil" };
		String falseString{ "false" };
		String nilString{ "nil" };
		String bufferString{};
		StringView buffer{};
		Uint64 offSet{};
		Uint64 size{};

		void singleValueJsonToETF(DiscordCoreAPI::JsonObject&& dataToParse);

		void writeObject(DiscordCoreAPI::JsonObject::ObjectType&& jsonData);

		void writeString(DiscordCoreAPI::JsonObject::StringType&& jsonData);

		void writeInt(DiscordCoreAPI::JsonObject::IntType jsonData);

		void writeUint(DiscordCoreAPI::JsonObject::UintType jsonData);

		void writeFloat(DiscordCoreAPI::JsonObject::FloatType jsonData);

		void writeArray(DiscordCoreAPI::JsonObject::ArrayType&& jsonData);

		void writeBool(DiscordCoreAPI::JsonObject::BoolType jsonData);

		void writeNullExt();

		void writeNull();

		void writeToBuffer(String&&);

		void appendBinaryExt(String&&, Uint32);

		void appendUnsignedLongLong(Uint64);

		void appendSmallIntegerExt(Uint8);

		void appendIntegerExt(Uint32);

		void appendListHeader(Uint32);

		void appendMapHeader(Uint32);

		void appendNewFloatExt(Double);

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

		Uint64 readString(Uint32 length);

		String processAtom(const char* atom, Uint32 length);

		String singleValueETFToJson();

		String parseSmallIntegerExt();

		String parseBigint(Uint32);

		String parseArray(Uint32);

		String parseTuple(Uint32);

		String parseSmallTupleExt();

		String parseLargeTupleExt();

		String parseSmallAtomExt();

		String parseStringAsList();

		String parseNewFloatExt();

		String parseSmallBigExt();

		String parseLargeBigExt();

		String parseAtomUtf8Ext();

		String parseIntegerExt();

		String parseBinaryExt();

		String parseFloatExt();

		String parseListExt();

		String parseNilExt();

		String parseMapExt();
	};

}// namespace DiscordCoreInternal
#endif