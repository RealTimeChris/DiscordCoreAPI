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

#pragma once

#ifndef DATA_PARSING_FUNCTIONS
	#define DATA_PARSING_FUNCTIONS

	#include <simdjson.h>
	#include <map>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll ObjectReturnData {
		simdjson::ondemand::value object{};
		bool didItSucceed{ false };
	};

	struct DiscordCoreAPI_Dll ArrayReturnData {
		simdjson::ondemand::array arrayValue{};
		bool didItSucceed{ false };
	};

	Snowflake getId(simdjson::ondemand::value jsonObjectData, const char* key);

	Snowflake getId(simdjson::ondemand::value jsonObjectData);

	int64_t getInt64(simdjson::ondemand::value jsonData, const char* key);

	int32_t getInt32(simdjson::ondemand::value jsonData, const char* key);

	int16_t getInt16(simdjson::ondemand::value jsonData, const char* key);

	int8_t getInt8(simdjson::ondemand::value jsonData, const char* key);

	uint64_t getUint64(simdjson::ondemand::value jsonData, const char* key);

	uint32_t getUint32(simdjson::ondemand::value jsonData, const char* key);

	uint16_t getUint16(simdjson::ondemand::value jsonData, const char* key);

	uint8_t getUint8(simdjson::ondemand::value jsonData, const char* key);

	float getFloat(simdjson::ondemand::value jsonData, const char* key);

	bool getBool(simdjson::ondemand::value jsonData, const char* key);

	std::string getString(simdjson::ondemand::value jsonData, const char* key);

	std::string getString(ObjectReturnData jsonData, const char* key);

	std::string getString(simdjson::ondemand::value jsonData);

	ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName);

	ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName);

	ObjectReturnData getObject(ArrayReturnData jsonObjectData, uint64_t objectIndex);

	ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName);

	ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName);

};
#endif