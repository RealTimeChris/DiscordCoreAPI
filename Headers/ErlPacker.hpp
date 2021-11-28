// ErlPacker.hpp - Header for the erlpacking class.
// Nov 8, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace DiscordCoreInternal {

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

	class DiscordCoreAPI_Dll ErlPackBuffer {
	public:
		vector<uint8_t> buffer{};

		uint32_t offSet{};
	};

	class DiscordCoreAPI_Dll ErlPacker {

	public:
		static vector<uint8_t> parseJsonToEtf(json dataToParse);

		static json parseEtfToJson(const vector<uint8_t>& dataToParse);

	protected:
		static uint16_t etfByteOrder16(uint16_t x);

		static uint32_t etfByteOrder32(uint32_t x);

		static uint64_t etfByteOrder64(uint64_t x);

		static void store16Bits(uint8_t* to, uint16_t num);

		static void store32Bits(uint8_t* to, uint32_t num);

		static void store64Bits(uint8_t* to, uint64_t num);

		static void singleValueJsonToETF(const json* jsonData, ErlPackBuffer* buffer);

		static void writeToBuffer(ErlPackBuffer* buffer, const uint8_t* bytes, uint32_t length);

		static void appendVersion(ErlPackBuffer* buffer);

		static void appendNil(ErlPackBuffer* buffer);

		static void appendFalse(ErlPackBuffer* buffer);

		static void appendTrue(ErlPackBuffer* buffer);

		static void appendSmallInteger(ErlPackBuffer* buffer, uint8_t value);

		static void appendInteger(ErlPackBuffer* buffer, int32_t value);

		static void appendUnsignedLongLong(ErlPackBuffer* buffer, uint64_t value);

		static void appendLongLong(ErlPackBuffer* buffer, int64_t value);

		static void appendDouble(ErlPackBuffer* buffer, double value);

		static void appendAtom(ErlPackBuffer* buffer, const uint8_t* bytes, uint32_t size);

		static void appendAtomUf8(ErlPackBuffer* buffer, const uint8_t* bytes, uint32_t size);

		static void appendBinary(ErlPackBuffer* buffer, const uint8_t* bytes, uint32_t size);

		static void appendString(ErlPackBuffer* buffer, const uint8_t* bytes, uint32_t size);

		static void appendTupleHeader(ErlPackBuffer* buffer, uint64_t size);

		static void appendNilExt(ErlPackBuffer* buffer);

		static void appendListHeader(ErlPackBuffer* buffer, uint64_t size);

		static void appendMapHeader(ErlPackBuffer* buffer, uint64_t size);

		static uint8_t read8Bits(ErlPackBuffer* buffer);

		static uint16_t read16Bits(ErlPackBuffer* buffer);

		static uint32_t read32Bits(ErlPackBuffer* buffer);

		static uint64_t read64Bits(ErlPackBuffer* buffer);

		static vector<char> readString(ErlPackBuffer* buffer, uint32_t length);

		static json singleValueETFToJson(ErlPackBuffer* buffer);

		static json processAtom(ErlPackBuffer* buffer, vector<char> atom, uint16_t length);

		static json parseAtom(ErlPackBuffer* buffer);

		static json parseSmallAtom(ErlPackBuffer* buffer);

		static json parseSmallInteger(ErlPackBuffer* buffer);

		static json parseInteger(ErlPackBuffer* buffer);

		static json parseArray(ErlPackBuffer* buffer, uint32_t length);

		static json parseList(ErlPackBuffer* buffer);

		static json parseTuple(ErlPackBuffer* buffer, uint32_t length);

		static json parseNil(ErlPackBuffer* buffer);

		static json parseMap(ErlPackBuffer* buffer);

		static json parseFloat(ErlPackBuffer* buffer);

		static json parseNewFloat(ErlPackBuffer* buffer);

		static json parseBigint(ErlPackBuffer* buffer, uint32_t digits);

		static json parseSmallBigint(ErlPackBuffer* buffer);

		static json parseLargeBigint(ErlPackBuffer* buffer);

		static json parseBinary(ErlPackBuffer* buffer);

		static json parseString(ErlPackBuffer* buffer);

		static json parseStringAsList(ErlPackBuffer* buffer);

		static json parseSmallTuple(ErlPackBuffer* buffer);

		static json parseLargeTuple(ErlPackBuffer* buffer);

	};

}