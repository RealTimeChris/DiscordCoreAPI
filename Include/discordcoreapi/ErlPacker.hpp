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
		ErlPacker() = default;

		std::string parseJsonToEtf(const nlohmann::json&);

		nlohmann::json parseEtfToJson(std::string& dataToParse);

	  protected:
		std::string bufferOut{};
		uint8_t* buffer{ nullptr };
		mutable uint64_t offSet{};
		int64_t theLength{ 0 };

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

		/**
	 * @brief Process an 'atom' value.
	 * An atom is a "label" or constant value within the data,
	 * such as a key name, nullptr, or false.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		//nlohmann::json process_atom(const char* atom, uint16_t length);

		/**
	 * @brief Decode an 'atom' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_atom();

		/**
	 * @brief Decode a small 'atom' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_small_atom();

		/**
	 * @brief Decode a small integer value (0-255).
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_small_integer();

		/**
	 * @brief Decode an integer value (-MAXINT -> MAXINT-1).
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_integer();

		/**
	 * @brief Decode an array of values.
	 * 
	 * @return nlohmann::json values converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_array(uint32_t length);

		/**
	 * @brief Decode a list of values.
	 * 
	 * @return nlohmann::json values converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_list();

		/**
	 * @brief Decode a 'tuple' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_tuple(uint32_t length);

		/**
	 * @brief Decode a nil 'atom' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_nil();

		/**
	 * @brief Decode a map (object) value.
	 * Will recurse to evaluate each member variable.
	 * 
	 * @return nlohmann::json values converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_map();

		/**
	 * @brief Decode a floating point numeric value.
	 * (depreciated in erlang but still expected to be supported)
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_float();

		/**
	 * @brief Decode a floating type numeric value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_new_float();

		/**
	 * @brief Decode a 'bigint' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_bigint(uint32_t digits);

		/**
	 * @brief Decode a small 'bigint' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_bigint_small();

		/**
	 * @brief Decode a large 'bigint' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_bigint_large();

		/**
	 * @brief Decode a binary value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_binary();

		/**
	 * @brief Decode a string value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_string();

		/**
	 * @brief Decode a string list value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_string_as_list();

		/**
	 * @brief Decode a 'small tuple' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_tuple_small();

		/**
	 * @brief Decode a 'large tuple' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_tuple_large();

		/**
	 * @brief Decode a compressed value.
	 * This is a zlib-compressed binary blob which contains another
	 * ETF object.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		//nlohmann::json decode_compressed();

		/**
	 * @brief Decode a 'reference' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_reference();

		/**
	 * @brief Decode a 'new reference' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_new_reference();

		/**
	 * @brief Decode a 'port' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_port();

		/**
	 * @brief Decode a 'PID' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_pid();

		/**
	 * @brief Decode an 'export' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
		nlohmann::json decode_export();
	};
}// namespace DiscordCoreInternal
