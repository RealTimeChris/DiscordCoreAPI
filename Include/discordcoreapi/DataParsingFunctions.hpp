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
		simdjson::ondemand::value theObject{};
		Bool didItSucceed{ false };
	};

	struct DiscordCoreAPI_Dll ArrayReturnData {
		simdjson::ondemand::array theArray{};
		Bool didItSucceed{ false };
	};

	Snowflake getId(simdjson::ondemand::value jsonObjectData, const char* theKey);

	Snowflake getId(simdjson::ondemand::value jsonObjectData);

	Uint64 getUint64(simdjson::ondemand::value jsonData, const char* theKey);

	Uint32 getUint32(simdjson::ondemand::value jsonData, const char* theKey);

	Uint16 getUint16(simdjson::ondemand::value jsonData, const char* theKey);

	Uint8 getUint8(simdjson::ondemand::value jsonData, const char* theKey);

	Float getFloat(simdjson::ondemand::value jsonData, const char* theKey);

	Bool getBool(simdjson::ondemand::value jsonData, const char* theKey);

	String getString(simdjson::ondemand::value jsonData, const char* theKey);

	String getString(ObjectReturnData jsonData, const char* theKey);

	String getString(simdjson::ondemand::value jsonData);

	ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName);

	ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName);

	ObjectReturnData getObject(ArrayReturnData jsonObjectData, Uint64 objectIndex);

	ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName);

	ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName);

};
#endif