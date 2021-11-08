// ErlPacker.hpp - Header for the erlpacking class.
// Nov 8, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#define etfByteOrder16(x) ((unsigned __int16)_byteswap_ushort((unsigned short)x))

#define etfByteOrder32(x) ((unsigned __int32)_byteswap_ulong((unsigned long)x))

#define etfByteOrder64(x) (_byteswap_uint64(x))

#define store16Bits(to, num) \
    do { unsigned __int16 val = etfByteOrder16(num); memcpy(to, &val, 2); } while(0)
#define store32Bits(to, num) \
    do { unsigned __int32 val = etfByteOrder32(num); memcpy(to, &val, 4); } while(0)
#define store64Bits(to, num) \
    do { unsigned __int64 val = etfByteOrder64(num); memcpy(to, &val, 8); } while(0)

constexpr unsigned char FORMAT_VERSION{ 131 };

enum class ETFTokenType :unsigned __int8 {
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

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ErlpackBuffer {

		vector<unsigned __int8> buffer{};

		size_t offSet{};
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:
		static vector<unsigned __int8> parseJsonToEtf(json dataToParse);

		static json parseEtfToJson(const vector<unsigned __int8>& dataToParse);

	protected:
		static void singleValueJsonToETF(const json* jsonData, ErlpackBuffer* buffer);

		static void writeToBuffer(ErlpackBuffer* buffer, const char* bytes, size_t length);

		static void appendVersion(ErlpackBuffer* buffer);

		static void appendNil(ErlpackBuffer* buffer);

		static void appendFalse(ErlpackBuffer* buffer);

		static void appendTrue(ErlpackBuffer* buffer);

		static void appendSmallInteger(ErlpackBuffer* buffer, char value);

		static void appendInteger(ErlpackBuffer* buffer, __int32 value);

		static void appendUnsignedLongLong(ErlpackBuffer* buffer, unsigned long long value);

		static void appendLongLong(ErlpackBuffer* buffer, long long value);

		static void appendDouble(ErlpackBuffer* buffer, double value);

		static void appendAtom(ErlpackBuffer* buffer, const char* bytes, size_t size);

		static void appendAtomUf8(ErlpackBuffer* buffer, const char* bytes, size_t size);

		static void appendBinary(ErlpackBuffer* buffer, const char* bytes, size_t size);

		static void appendString(ErlpackBuffer* buffer, const char* bytes, size_t size);

		static void appendTupleHeader(ErlpackBuffer* buffer, size_t size);

		static void appendNilExt(ErlpackBuffer* buffer);

		static void appendListHeader(ErlpackBuffer* buffer, size_t size);

		static void appendMapHeader(ErlpackBuffer* buffer, size_t size);

		static unsigned __int8 read8Bits(ErlpackBuffer* buffer);
		
		static unsigned __int16 read16Bits(ErlpackBuffer* buffer);

		static unsigned __int32 read32Bits(ErlpackBuffer* buffer);

		static unsigned __int64 read64Bits(ErlpackBuffer* buffer);

		static const char* readString(ErlpackBuffer* buffer, unsigned __int32 length);

		static json singleValueETFToJson(ErlpackBuffer* buffer);

		static json processAtom(ErlpackBuffer* buffer, const char* atom, unsigned __int16 length);

		static json decodeAtom(ErlpackBuffer* buffer);

		static json decodeSmallAtom(ErlpackBuffer* buffer);

		static json decodeSmallInteger(ErlpackBuffer* buffer);

		static json decodeInteger(ErlpackBuffer* buffer);

		static json decodeArray(ErlpackBuffer* buffer, unsigned __int32 length);

		static json decodeList(ErlpackBuffer* buffer);

		static json decodeTuple(ErlpackBuffer* buffer, unsigned __int32 length);

		static json decodeNil(ErlpackBuffer* buffer);

		static json decodeMap(ErlpackBuffer* buffer);

		static json decodeFloat(ErlpackBuffer* buffer);

		static json decodeNewFloat(ErlpackBuffer* buffer);

		static json decodeBigint(ErlpackBuffer* buffer, unsigned __int32 digits);

		static json decodeSmallBigint(ErlpackBuffer* buffer);

		static json decodeLargeBigint(ErlpackBuffer* buffer);

		static json decodeBinary(ErlpackBuffer* buffer);

		static json decodeString(ErlpackBuffer* buffer);

		static json decodeStringAsList(ErlpackBuffer* buffer);

		static json decodeSmallTuple(ErlpackBuffer* buffer);

		static json decodeLargeTuple(ErlpackBuffer* buffer);

	};

}