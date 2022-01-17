// ErlPacker.hpp - Header for the erlpacking class.
// Nov 8, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ErlPackError : public exception {
	public:
		explicit ErlPackError(const string& message) : exception(message.c_str()) {}
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
		New_Float = 70,
		Bit_Binary= 77,
		Comressed= 80,
		Small_Integer= 97,
		Integer = 98,
		Float= 99,
		Atom= 100,
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

		unique_ptr<vector<uint8_t>> buffer{ make_unique<vector<uint8_t>>() };

		uint32_t offSet{};

		ErlPackBuffer() = default;

		ErlPackBuffer(unique_ptr<vector<uint8_t>>);

		ErlPackBuffer& operator=(ErlPackBuffer&&) noexcept;

		ErlPackBuffer(ErlPackBuffer&&) noexcept;

		ErlPackBuffer& operator=(ErlPackBuffer&) = delete;

		ErlPackBuffer(ErlPackBuffer&) = delete;
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:

		static vector<uint8_t> parseJsonToEtf(json&);

		static json parseEtfToJson(unique_ptr<vector<uint8_t>>);

	protected:

		template<typename returnType>
		static void etfByteOrder(returnType, returnType&);

		template<typename returnType>
		static void storeBits(vector<uint8_t>&, returnType&, uint32_t&);

		static void singleValueJsonToETF(ErlPackBuffer&, json&);

		static void writeToBuffer(ErlPackBuffer&, vector<uint8_t>);

		static void appendVersion(ErlPackBuffer&);

		static void appendNil(ErlPackBuffer&);

		static void appendFalse(ErlPackBuffer&);

		static void appendTrue(ErlPackBuffer&);

		static void appendSmallInteger(ErlPackBuffer&, uint8_t&);

		static void appendInteger(ErlPackBuffer&, uint32_t&);

		static void appendUnsignedLongLong(ErlPackBuffer&, uint64_t&);

		static void appendLongLong(ErlPackBuffer&, int64_t&);

		static void appendDouble(ErlPackBuffer&, double&);

		static void appendBinary(ErlPackBuffer&, vector<uint8_t>&, uint32_t&);

		static void appendTupleHeader(ErlPackBuffer&, uint32_t&);

		static void appendNilExt(ErlPackBuffer&);

		static void appendListHeader(ErlPackBuffer&, uint32_t&);

		static void appendMapHeader(ErlPackBuffer&, uint32_t&);

		template<typename returnType>
		static void readBits(ErlPackBuffer&, returnType&);

		static void readString(ErlPackBuffer&, uint32_t&, vector<char>&);

		static json singleValueETFToJson(ErlPackBuffer&);

		static json processAtom(ErlPackBuffer&, vector<char>&, uint32_t&);

		static json parseAtom(ErlPackBuffer&);

		static json parseSmallAtom(ErlPackBuffer&);

		static json parseSmallInteger(ErlPackBuffer&);

		static json parseInteger(ErlPackBuffer&);

		static json parseArray(ErlPackBuffer&, uint32_t&);

		static json parseList(ErlPackBuffer&);

		static json parseTuple(ErlPackBuffer&, uint32_t&);

		static json parseNil(ErlPackBuffer&);

		static json parseMap(ErlPackBuffer&);

		static json parseFloat(ErlPackBuffer&);

		static json parseNewFloat(ErlPackBuffer&);

		static json parseBigint(ErlPackBuffer&, uint32_t&);

		static json parseSmallBigint(ErlPackBuffer&);

		static json parseLargeBigint(ErlPackBuffer&);

		static json parseBinary(ErlPackBuffer&);

		static json parseString(ErlPackBuffer&);

		static json parseStringAsList(ErlPackBuffer&);

		static json parseSmallTuple(ErlPackBuffer&);

		static json parseLargeTuple(ErlPackBuffer&);

	}; 
}