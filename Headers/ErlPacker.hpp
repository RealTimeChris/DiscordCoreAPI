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

constexpr unsigned __int8 FORMAT_VERSION{ 131 };

enum class ETFTokenType : unsigned __int8 {
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

	class DiscordCoreAPI_Dll ErlPackBuffer {
	public:

		unsigned __int8* buffer{ nullptr };

		unsigned __int32 currentSize{};

		bool builtFromBuffer{ false };
		
		unsigned __int64 offSet{};

		void growBuffer();

		~ErlPackBuffer();
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:
		static vector<unsigned __int8> parseJsonToEtf(json dataToParse);

		static json parseEtfToJson(const vector<unsigned __int8>& dataToParse);

	protected:
		static unsigned __int16 etfByteOrder16(unsigned __int16 x);

		static unsigned __int32 etfByteOrder32(unsigned __int32 x);

		static unsigned __int64 etfByteOrder64(unsigned __int64 x);

		static void store16Bits(void* to, unsigned __int16 num);

		static void store32Bits(void* to, unsigned __int32 num);

		static void store64Bits(void* to, unsigned __int64 num);

		static void singleValueJsonToETF(const json* jsonData, ErlPackBuffer* buffer);

		static void writeToBuffer(ErlPackBuffer* buffer, const char* bytes, unsigned __int64 length);

		static void appendVersion(ErlPackBuffer* buffer);

		static void appendNil(ErlPackBuffer* buffer);

		static void appendFalse(ErlPackBuffer* buffer);

		static void appendTrue(ErlPackBuffer* buffer);

		static void appendSmallInteger(ErlPackBuffer* buffer, char value);

		static void appendInteger(ErlPackBuffer* buffer, __int32 value);

		static void appendUnsignedLongLong(ErlPackBuffer* buffer, unsigned long long value);

		static void appendLongLong(ErlPackBuffer* buffer, long long value);

		static void appendDouble(ErlPackBuffer* buffer, double value);

		static void appendAtom(ErlPackBuffer* buffer, const char* bytes, unsigned __int64 size);

		static void appendAtomUf8(ErlPackBuffer* buffer, const char* bytes, unsigned __int64 size);

		static void appendBinary(ErlPackBuffer* buffer, const char* bytes, unsigned __int64 size);

		static void appendString(ErlPackBuffer* buffer, const char* bytes, unsigned __int64 size);

		static void appendTupleHeader(ErlPackBuffer* buffer, unsigned __int64 size);

		static void appendNilExt(ErlPackBuffer* buffer);

		static void appendListHeader(ErlPackBuffer* buffer, unsigned __int64 size);

		static void appendMapHeader(ErlPackBuffer* buffer, unsigned __int64 size);

		static unsigned __int8 read8Bits(ErlPackBuffer* buffer);
		
		static unsigned __int16 read16Bits(ErlPackBuffer* buffer);

		static unsigned __int32 read32Bits(ErlPackBuffer* buffer);

		static unsigned __int64 read64Bits(ErlPackBuffer* buffer);

		static const char* readString(ErlPackBuffer* buffer, unsigned __int32 length);

		static json singleValueETFToJson(ErlPackBuffer* buffer);

		static json processAtom(ErlPackBuffer* buffer, const char* atom, unsigned __int16 length);

		static json parseAtom(ErlPackBuffer* buffer);

		static json parseSmallAtom(ErlPackBuffer* buffer);

		static json parseSmallInteger(ErlPackBuffer* buffer);

		static json parseInteger(ErlPackBuffer* buffer);

		static json parseArray(ErlPackBuffer* buffer, unsigned __int32 length);

		static json parseList(ErlPackBuffer* buffer);

		static json parseTuple(ErlPackBuffer* buffer, unsigned __int32 length);

		static json parseNil(ErlPackBuffer* buffer);

		static json parseMap(ErlPackBuffer* buffer);

		static json parseFloat(ErlPackBuffer* buffer);

		static json parseNewFloat(ErlPackBuffer* buffer);

		static json parseBigint(ErlPackBuffer* buffer, unsigned __int32 digits);

		static json parseSmallBigint(ErlPackBuffer* buffer);

		static json parseLargeBigint(ErlPackBuffer* buffer);

		static json parseBinary(ErlPackBuffer* buffer);

		static json parseString(ErlPackBuffer* buffer);

		static json parseStringAsList(ErlPackBuffer* buffer);

		static json parseSmallTuple(ErlPackBuffer* buffer);

		static json parseLargeTuple(ErlPackBuffer* buffer);

	};

}