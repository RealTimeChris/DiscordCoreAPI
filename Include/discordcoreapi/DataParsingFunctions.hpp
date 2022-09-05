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

#include <discordcoreapi/FoundationEntities.hpp>
#include <simdjson.h>

namespace DiscordCoreInternal {

	static std::map<int32_t, std::string> theErrors{ { 0, "SUCCESS" }, { 1, "CAPACITY" }, { 2, "MEMALLOC" }, { 3, "TAPE_ERRPR" }, { 4, "DEPTH_ERROR" }, { 5, "STRING_ERROR" },
		{ 6, "T_ATOM_ERROR" }, { 7, "F_ATOM_ERROR" }, { 8, "N_ATOM_ERROR" }, { 9, "NUMBER_ERROR" }, { 10, "UTF8_ERROR" }, { 11, "UNINITIALIZED" }, { 12, "EMPTY" },
		{ 13, "UNESCAPED_CHARS" }, { 14, "UNCLOSED_STRING" }, { 15, "UNSUPPORTED_ARCHITECTURE" }, { 16, "INCORRECT_TYPE" }, { 17, "NUMBER_OUT_OF_RANGE" },
		{ 18, "INDEX_OUT_OF_BOUNDS" }, { 19, "NO_SUCH_FIELD" }, { 20, "IO_ERROR" }, { 21, "INVALID_JSON_POINTER" }, { 22, "INVALID_URI_FRAGMENT" }, { 23, "UNEXPECTED_ERROR" },
		{ 24, "PARSER_IN_USE" }, { 25, "OUT_OF_ORDER_ITERATION" }, { 26, "INSUFFICIENT_PADDING" }, { 27, "INCOMPLETE_ARRAY_OR_OBJECT" }, { 28, "SCALAR_DOCUMENT_AS_VALUE" },
		{ 29, "OUT_OF_BOUNDS" }, { 30, "NUM_ERROR_CODES " } };

	struct JsonParseError :public std::runtime_error {
		explicit JsonParseError(int32_t theCode);
	};
	

	DiscordCoreAPI_Dll uint8_t getUint8(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll uint16_t getUint16(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll uint32_t getUint32(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll uint32_t getUint32(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll uint64_t getUint64(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll double getFloat(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll bool getBool(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll std::string getString(simdjson::simdjson_result<simdjson::fallback::ondemand::object>&& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll std::string getString(simdjson::simdjson_result<simdjson::fallback::ondemand::object>& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());
	
	DiscordCoreAPI_Dll std::string getString(simdjson::fallback::ondemand::document_stream::iterator::value_type& theParser, const char* keyName, bool throwIfError = false,
		std::source_location theLocation = std::source_location::current());

}
#endif