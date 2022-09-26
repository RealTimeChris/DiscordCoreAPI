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
/// DataParsingFunctions.hpp - Header file for the data-parsing functions.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file DataParsingFunctions.hpp

#ifndef DATA_PARSING_FUNCTIONS
#define DATA_PARSING_FUNCTIONS

#include <simdjson.h>

namespace DiscordCoreAPI {

	static std::map<int32_t, std::string> theErrors{ { 0, "SUCCESS" }, { 1, "CAPACITY" }, { 2, "MEMALLOC" }, { 3, "TAPE_ERRPR" }, { 4, "DEPTH_ERROR" }, { 5, "STRING_ERROR" },
		{ 6, "T_ATOM_ERROR" }, { 7, "F_ATOM_ERROR" }, { 8, "N_ATOM_ERROR" }, { 9, "NUMBER_ERROR" }, { 10, "UTF8_ERROR" }, { 11, "UNINITIALIZED" }, { 12, "EMPTY" },
		{ 13, "UNESCAPED_CHARS" }, { 14, "UNCLOSED_STRING" }, { 15, "UNSUPPORTED_ARCHITECTURE" }, { 16, "INCORRECT_TYPE" }, { 17, "NUMBER_OUT_OF_RANGE" },
		{ 18, "INDEX_OUT_OF_BOUNDS" }, { 19, "NO_SUCH_FIELD" }, { 20, "IO_ERROR" }, { 21, "INVALID_JSON_POINTER" }, { 22, "INVALID_URI_FRAGMENT" }, { 23, "UNEXPECTED_ERROR" },
		{ 24, "PARSER_IN_USE" }, { 25, "OUT_OF_ORDER_ITERATION" }, { 26, "INSUFFICIENT_PADDING" }, { 27, "INCOMPLETE_ARRAY_OR_OBJECT" }, { 28, "SCALAR_DOCUMENT_AS_VALUE" },
		{ 29, "OUT_OF_BOUNDS" }, { 30, "NUM_ERROR_CODES " } };

	struct DiscordCoreAPI_Dll JsonParseError : public std::runtime_error {
		explicit JsonParseError(int32_t theCode);
	};

	struct DiscordCoreAPI_Dll ObjectReturnData {
		simdjson::ondemand::value theObject{};
		bool didItSucceed{ false };
	};

	struct DiscordCoreAPI_Dll ArrayReturnData {
		simdjson::ondemand::array theArray{};
		bool didItSucceed{ false };
	};

	class DiscordCoreAPI_Dll SimdJsonConverter {
	  public:
		SimdJsonConverter() noexcept = default;
		
		void prepForParsing(std::string& theString);

		template<typename ObjectType> void parseObject(ObjectType&, const char* theParseEntryPoint = nullptr);

	  protected:
		simdjson::ondemand::parser theParser{};
		simdjson::ondemand::value theObject{};

		Snowflake getId(simdjson::ondemand::value jsonObjectData, const char* theKey);

		bool getBool(simdjson::ondemand::value jsonData, const char* theKey);

		uint8_t getUint8(simdjson::ondemand::value jsonData, const char* theKey);

		uint16_t getUint16(simdjson::ondemand::value jsonData, const char* theKey);

		uint32_t getUint32(simdjson::ondemand::value jsonData, const char* theKey);

		float getFloat(simdjson::ondemand::value jsonData, const char* theKey);

		uint64_t getUint64(simdjson::ondemand::value jsonData, const char* theKey);

		std::string getString(simdjson::ondemand::value jsonData, const char* theKey);

		std::string getString(ObjectReturnData jsonData, const char* theKey);

		ObjectReturnData getObject(ArrayReturnData jsonObjectData, size_t objectIndex, std::source_location theLocation = std::source_location::current());

		ObjectReturnData getObject(simdjson::ondemand::array jsonObjectData, size_t objectIndex, std::source_location theLocation);

		ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName);

		ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName, std::source_location theLocation = std::source_location::current());

		ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName, std::source_location theLocation = std::source_location::current());

		ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName);
	};

	

};
#endif