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

#include <stdlib.h>
#include <stddef.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#elif defined(_MSC_VER)  // && _MSC_VER >= 1600
#include <stdint.h>
#else
#endif

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __LITTLE_ENDIAN__
#endif
#endif

#ifdef __LITTLE_ENDIAN__

#ifdef _WIN32
#  if defined(_byteswap_ushort) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#    define etfByteOrder16(x) ((unsigned __int16)_byteswap_ushort((unsigned short)x))
#  endif
#endif

#ifdef _WIN32
#  if defined(_byteswap_ulong) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#    define etfByteOrder32(x) ((unsigned __int32)_byteswap_ulong((unsigned long)x))
#  endif
#endif

#if defined(_byteswap_uint64) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#  define etfByteOrder64(x) (_byteswap_uint64(x))
#endif
#endif

#define store16Bits(to, num) \
    do { unsigned __int16 val = etfByteOrder16(num); memcpy(to, &val, 2); } while(0)
#define store32Bits(to, num) \
    do { unsigned __int32 val = etfByteOrder32(num); memcpy(to, &val, 4); } while(0)
#define store64Bits(to, num) \
    do { unsigned __int64 val = etfByteOrder64(num); memcpy(to, &val, 8); } while(0)

constexpr unsigned char FORMAT_VERSION{ 131 };

enum class ETFTokenType :unsigned __int8 {
	NEW_FLOAT = 'F',
	BIT_BINARY = 'M',
	COMPRESSED = 'P',
	SMALL_INTEGER = 'a',
	INTEGER = 'b',
	FLOAT = 'c',
	ATOM = 'd',
	REFERENCE = 'e',
	PORT = 'f',
	PID = 'g',
	SMALL_TUPLE = 'h',
	LARGE_TUPLE = 'i',
	NIL = 'j',
	STRING = 'k',
	LIST = 'l',
	BINARY = 'm',
	SMALL_BIGINT = 'n',
	LARGE_BIGINT = 'o',
	NEW_FUNCTION = 'p',
	EXPORT = 'q',
	NEW_REFERENCE = 'r',
	SMALL_ATOM = 's',
	MAP = 't',
	FUNCTION = 'u',
	UTF8_ATOM = 'v',
	SMALL_UTF8_ATOM = 'w'
};

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ErlpackBuffer {

		vector<unsigned char> buffer{};
		
		size_t length{};

		ErlpackBuffer();
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:
		unsigned __int8* data{ nullptr };
		size_t offset{};
		size_t size{};

		json parseEtfToJson(const vector<unsigned __int8>& dataToParse);

		vector<unsigned __int8> parseJsonToEtf(json dataToParse);

	protected:

		void singleValueJsonToETF(const json* jsonData, ErlpackBuffer* buffer);

		void bufferWrite(ErlpackBuffer* buffer, const char* bytes, size_t length);

		void appendVersion(ErlpackBuffer* buffer);

		void appendNil(ErlpackBuffer* buffer);

		void appendFalse(ErlpackBuffer* buffer);

		void appendTrue(ErlpackBuffer* buffer);

		void appendSmallInteger(ErlpackBuffer* buffer, char value);

		void appendInteger(ErlpackBuffer* buffer, __int32 value);

		void appendUnsignedLongLong(ErlpackBuffer* buffer, unsigned long long value);

		void appendLongLong(ErlpackBuffer* buffer, long long value);

		void appendDouble(ErlpackBuffer* buffer, double value);

		void appendAtom(ErlpackBuffer* buffer, const char* bytes, size_t size);

		void appendAtomUf8(ErlpackBuffer* buffer, const char* bytes, size_t size);

		void appendBinary(ErlpackBuffer* buffer, const char* bytes, size_t size);

		void appendString(ErlpackBuffer* buffer, const char* bytes, size_t size);

		void appendTupleHeader(ErlpackBuffer* buffer, size_t size);

		void appendNilExt(ErlpackBuffer* buffer);

		void appendListHeader(ErlpackBuffer* buffer, size_t size);

		void appendMapHeader(ErlpackBuffer* buffer, size_t size);

		unsigned __int8 read8Bits();
		
		unsigned __int16 read16Bits();

		unsigned __int32 read32Bits();

		unsigned __int64 read64Bits();

		const char* readString(unsigned __int32 length);

		json singleValueETFToJson();

		json processAtom(const char* atom, unsigned __int16 length);

		json decodeAtom();

		json decodeSmallAtom();

		json decodeSmallInteger();

		json decodeInteger();

		json decodeArray(unsigned __int32 length);

		json decodeList();

		json decodeTuple(unsigned __int32 length);

		json decodeNil();

		json decodeMap();

		json decodeFloat();

		json decodeNewFloat();

		json decodeBigint(unsigned __int32 digits);

		json decodeSmallBigint();

		json decodeLargeBigint();

		json decodeBinary();

		json decodeString();

		json decodeStringAsList();

		json decodeSmallTuple();

		json decodeLargeTuple();

	};

}