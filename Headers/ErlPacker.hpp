// ErlPacker.hpp - Header for the erlpacking class.
// Nov 8, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ErlPackError : public std::exception {
	public:
		explicit ErlPackError(const std::string& message) : std::exception(message.c_str()) {}
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
		New_Float = 70,
		Bit_Binary = 77,
		Comressed = 80,
		Small_Integer = 97,
		Integer = 98,
		Float = 99,
		Atom = 100,
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

		ErlPackBuffer(std::vector<uint8_t>*);

		ErlPackBuffer& operator=(ErlPackBuffer&&) noexcept;

		ErlPackBuffer(ErlPackBuffer&&) noexcept;

		ErlPackBuffer& operator=(const ErlPackBuffer&) = delete;

		ErlPackBuffer(const ErlPackBuffer&) = delete;

		ErlPackBuffer& operator=(ErlPackBuffer&) = delete;

		ErlPackBuffer(ErlPackBuffer&) = delete;
	};

	class DiscordCoreAPI_Dll ErlPacker {
	public:

		std::vector<uint8_t> parseJsonToEtf(nlohmann::json&);

		nlohmann::json parseEtfToJson(std::vector<uint8_t>*);

	protected:

		template<typename ReturnType>
		void etfByteOrder(ReturnType, ReturnType&);

		template<typename ReturnType>
		void storeBits(std::vector<uint8_t>&, ReturnType&, uint32_t&);

		void singleValueJsonToETF(ErlPackBuffer&, nlohmann::json&);

		void writeToBuffer(ErlPackBuffer&, std::vector<uint8_t>);

		void appendVersion(ErlPackBuffer&);

		void appendNil(ErlPackBuffer&);

		void appendFalse(ErlPackBuffer&);

		void appendTrue(ErlPackBuffer&);

		void appendSmallInteger(ErlPackBuffer&, uint8_t&);

		void appendInteger(ErlPackBuffer&, uint32_t&);

		void appendUnsignedLongLong(ErlPackBuffer&, uint64_t&);

		void appendLongLong(ErlPackBuffer&, int64_t&);

		void appendDouble(ErlPackBuffer&, double&);

		void appendBinary(ErlPackBuffer&, std::vector<uint8_t>&, uint32_t&);

		void appendTupleHeader(ErlPackBuffer&, uint32_t&);

		void appendNilExt(ErlPackBuffer&);

		void appendListHeader(ErlPackBuffer&, uint32_t&);

		void appendMapHeader(ErlPackBuffer&, uint32_t&);

		template<typename ReturnType>
		void readBits(ErlPackBuffer&, ReturnType&);

		void readString(ErlPackBuffer&, uint32_t&, std::vector<char>&);

		nlohmann::json singleValueETFToJson(ErlPackBuffer&);

		nlohmann::json processAtom(ErlPackBuffer&, std::vector<char>&, uint32_t&);

		nlohmann::json parseAtom(ErlPackBuffer&);

		nlohmann::json parseSmallAtom(ErlPackBuffer&);

		nlohmann::json parseSmallInteger(ErlPackBuffer&);

		nlohmann::json parseInteger(ErlPackBuffer&);

		nlohmann::json parseArray(ErlPackBuffer&, uint32_t&);

		nlohmann::json parseList(ErlPackBuffer&);

		nlohmann::json parseTuple(ErlPackBuffer&, uint32_t&);

		nlohmann::json parseNil(ErlPackBuffer&);

		nlohmann::json parseMap(ErlPackBuffer&);

		nlohmann::json parseFloat(ErlPackBuffer&);

		nlohmann::json parseNewFloat(ErlPackBuffer&);

		nlohmann::json parseBigint(ErlPackBuffer&, uint32_t&);

		nlohmann::json parseSmallBigint(ErlPackBuffer&);

		nlohmann::json parseLargeBigint(ErlPackBuffer&);

		nlohmann::json parseBinary(ErlPackBuffer&);

		nlohmann::json parseString(ErlPackBuffer&);

		nlohmann::json parseStringAsList(ErlPackBuffer&);

		nlohmann::json parseSmallTuple(ErlPackBuffer&);

		nlohmann::json parseLargeTuple(ErlPackBuffer&);

	}; 
}