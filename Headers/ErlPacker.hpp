// ErlPacker.hpp - Header for the erlpacking class.
// Nov 8, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ErlPackError : public std::exception {
	public:
		explicit ErlPackError(const std::string& message) : std::exception(message.c_str()) {}
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

		std::vector<uint8_t> buffer{};

		uint32_t offSet{};

		ErlPackBuffer() = default;

		ErlPackBuffer(std::unique_ptr<std::vector<uint8_t>>);

		ErlPackBuffer& operator=(ErlPackBuffer&&) noexcept;

		ErlPackBuffer(ErlPackBuffer&&) noexcept;

		ErlPackBuffer& operator=(const ErlPackBuffer&) = delete;

		ErlPackBuffer(const ErlPackBuffer&) = delete;

		ErlPackBuffer& operator=(ErlPackBuffer&) = delete;

		ErlPackBuffer(ErlPackBuffer&) = delete;
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:

		static std::vector<uint8_t> parseJsonToEtf(nlohmann::json&);

		static nlohmann::json parseEtfToJson(std::unique_ptr<std::vector<uint8_t>>);

	protected:

		template<typename returnType>
		static void etfByteOrder(returnType, returnType&);

		template<typename returnType>
		static void storeBits(std::vector<uint8_t>&, returnType&, uint32_t&);

		static void singleValueJsonToEtf(ErlPackBuffer&, nlohmann::json&);

		static void writeToBuffer(ErlPackBuffer&, std::vector<uint8_t>);

		static void appendVersion(ErlPackBuffer&);

		static void appendNil(ErlPackBuffer&);

		static void appendFalse(ErlPackBuffer&);

		static void appendTrue(ErlPackBuffer&);

		static void appendSmallInteger(ErlPackBuffer&, uint8_t&);

		static void appendInteger(ErlPackBuffer&, uint32_t&);

		static void appendUnsignedLongLong(ErlPackBuffer&, uint64_t&);

		static void appendLongLong(ErlPackBuffer&, int64_t&);

		static void appendDouble(ErlPackBuffer&, double&);

		static void appendBinary(ErlPackBuffer&, std::vector<uint8_t>&, uint32_t&);

		static void appendTupleHeader(ErlPackBuffer&, uint32_t&);

		static void appendNilExt(ErlPackBuffer&);

		static void appendListHeader(ErlPackBuffer&, uint32_t&);

		static void appendMapHeader(ErlPackBuffer&, uint32_t&);

		template<typename returnType>
		static void readBits(ErlPackBuffer&, returnType&);

		static void readString(ErlPackBuffer&, uint32_t&, std::vector<char>&);

		static nlohmann::json singleValueETFToJson(ErlPackBuffer&);

		static nlohmann::json processAtom(ErlPackBuffer&, std::vector<char>&, uint32_t&);

		static nlohmann::json parseAtom(ErlPackBuffer&);

		static nlohmann::json parseSmallAtom(ErlPackBuffer&);

		static nlohmann::json parseSmallInteger(ErlPackBuffer&);

		static nlohmann::json parseInteger(ErlPackBuffer&);

		static nlohmann::json parseArray(ErlPackBuffer&, uint32_t&);

		static nlohmann::json parseList(ErlPackBuffer&);

		static nlohmann::json parseTuple(ErlPackBuffer&, uint32_t&);

		static nlohmann::json parseNil(ErlPackBuffer&);

		static nlohmann::json parseMap(ErlPackBuffer&);

		static nlohmann::json parseFloat(ErlPackBuffer&);

		static nlohmann::json parseNewFloat(ErlPackBuffer&);

		static nlohmann::json parseBigint(ErlPackBuffer&, uint32_t&);

		static nlohmann::json parseSmallBigint(ErlPackBuffer&);

		static nlohmann::json parseLargeBigint(ErlPackBuffer&);

		static nlohmann::json parseBinary(ErlPackBuffer&);

		static nlohmann::json parseString(ErlPackBuffer&);

		static nlohmann::json parseStringAsList(ErlPackBuffer&);

		static nlohmann::json parseSmallTuple(ErlPackBuffer&);

		static nlohmann::json parseLargeTuple(ErlPackBuffer&);

	}; 
}