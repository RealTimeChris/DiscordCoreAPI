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

	constexpr uint8_t FORMAT_VERSION{ 131 };

	enum class ETFTokenType : uint8_t {
		NEW_FLOAT = 70,
		BIT_BINARY = 77,
		COMPRESSED = 80,
		SMALL_INTEGER = 97,
		INTEGER = 98,
		FLOAT = 99,
		ATOM = 100,
		REFERENCE = 101,
		PORT = 102,
		PID = 103,
		SMALL_TUPLE = 104,
		LARGE_TUPLE = 105,
		NIL = 106,
		STRING = 107,
		LIST = 108,
		BINARY = 109,
		SMALL_BIGINT = 110,
		LARGE_BIGINT = 111,
		NEW_FUNCTION = 112,
		EXPORT = 113,
		NEW_REFERENCE = 114,
		SMALL_ATOM = 115,
		MAP = 116,
		FUNCTION = 117,
		UTF8_ATOM = 118,
		SMALL_UTF8_ATOM = 119
	};

	struct DiscordCoreAPI_Dll ErlPackBuffer {
	public:

		unique_ptr<vector<uint8_t>> buffer{ make_unique<vector<uint8_t>>() };

		uint32_t offSet{};

		ErlPackBuffer();

		ErlPackBuffer(unique_ptr<vector<uint8_t>>);

		ErlPackBuffer& operator=(ErlPackBuffer&&) noexcept;

		ErlPackBuffer(ErlPackBuffer&&) noexcept;

		ErlPackBuffer& operator=(ErlPackBuffer&) = delete;

		ErlPackBuffer(ErlPackBuffer&) = delete;

		~ErlPackBuffer();
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:

		static vector<uint8_t> parseJsonToEtf(json&);

		static json parseEtfToJson(unique_ptr<vector<uint8_t>>);

	protected:

		static uint16_t etfByteOrder16(uint16_t&);

		static uint32_t etfByteOrder32(uint32_t&);

		static uint64_t etfByteOrder64(uint64_t&);

		static void store16Bits(vector<uint8_t>&, uint16_t&, uint32_t&);

		static void store32Bits(vector<uint8_t>&, uint32_t&, uint32_t&);

		static void store64Bits(vector<uint8_t>&, uint64_t&, uint32_t&);

		static void singleValueJsonToETF(ErlPackBuffer& ,json&);

		static void writeToBuffer(ErlPackBuffer&, vector<uint8_t>&);

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

		static uint8_t read8Bits(ErlPackBuffer&);

		static uint16_t read16Bits(ErlPackBuffer&);

		static uint32_t read32Bits(ErlPackBuffer&);

		static uint64_t read64Bits(ErlPackBuffer&);

		static vector<char> readString(ErlPackBuffer&, uint32_t&);

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