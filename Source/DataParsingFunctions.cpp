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
/// DataParsingFunctions.cpp - Source file for the data-parsing functions.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file DataParsingFunctions.cpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreAPI {

	JsonParseError::JsonParseError(Int32 theCode) : std::runtime_error(theErrors[theCode]){};

	Snowflake getId(simdjson::ondemand::value jsonData, const char* theKey) {
		return Snowflake{ DiscordCoreAPI::strtoull(getString(jsonData, theKey)) };
	}

	Snowflake getId(simdjson::ondemand::value jsonObjectData) {
		return Snowflake{ DiscordCoreAPI::strtoull(getString(jsonObjectData)) };
	}

	Float getFloat(simdjson::ondemand::value jsonData, const char* theKey) {
		Double theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return Double{ theValue };
		} else {
			return 0.0f;
		}
	}

	Bool getBool(simdjson::ondemand::value jsonData, const char* theKey) {
		Bool theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return Bool{ theValue };
		} else {
			return false;
		}
	}

	Uint8 getUint8(simdjson::ondemand::value jsonData, const char* theKey) {
		Uint64 theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return static_cast<Uint8>(theValue);
		} else {
			return 0;
		}
	}

	Uint16 getUint16(simdjson::ondemand::value jsonData, const char* theKey) {
		Uint64 theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return static_cast<Uint16>(theValue);
		} else {
			return 0;
		}
	}

	Uint32 getUint32(simdjson::ondemand::value jsonData, const char* theKey) {
		Uint64 theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return static_cast<Uint32>(theValue);
		} else {
			return 0;
		}
	}

	Uint64 getUint64(simdjson::ondemand::value jsonData, const char* theKey) {
		Uint64 theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return Uint64{ theValue };
		} else {
			return 0;
		}
	}

	String getString(simdjson::ondemand::value jsonData, const char* theKey) {
		StringView theValue{};
		if (jsonData[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return String{ theValue.data(), theValue.size() };
		} else {
			return "";
		}
	}

	String getString(simdjson::ondemand::value jsonData) {
		StringView theValue{};
		if (jsonData.get(theValue) == simdjson::error_code::SUCCESS) {
			return String{ theValue.data(), theValue.size() };
		} else {
			return "";
		}
	}

	ObjectReturnData getObject(ArrayReturnData jsonData, Uint64 objectIndex) {
		ObjectReturnData theValue{};
		if (jsonData.didItSucceed && jsonData.theArray.at(objectIndex).get(theValue.theObject) == simdjson::error_code::SUCCESS) {
			theValue.didItSucceed = true;
		}
		return theValue;
	}

	ObjectReturnData getObject(ObjectReturnData jsonData, const char* objectName) {
		ObjectReturnData theValue{};
		if (jsonData.didItSucceed && jsonData.theObject[objectName].get(theValue.theObject) == simdjson::error_code::SUCCESS) {
			theValue.didItSucceed = true;
		}
		return theValue;
	}

	ArrayReturnData getArray(ObjectReturnData jsonData, const char* arrayName) {
		ArrayReturnData theValue{};
		if (jsonData.didItSucceed && jsonData.theObject[arrayName].get(theValue.theArray) == simdjson::error_code::SUCCESS) {
			theValue.didItSucceed = true;
		}
		return theValue;
	}

	ObjectReturnData getObject(simdjson::ondemand::array jsonData, Uint64 objectIndex) {
		ObjectReturnData theValue{};
		if (jsonData.at(objectIndex).get(theValue.theObject) == simdjson::error_code::SUCCESS) {
			theValue.didItSucceed = true;
		}
		return theValue;
	}

	ObjectReturnData getObject(simdjson::ondemand::value jsonData, const char* objectName) {
		ObjectReturnData theValue{};
		if (jsonData[objectName].get(theValue.theObject) == simdjson::error_code::SUCCESS) {
			theValue.didItSucceed = true;
		}
		return theValue;
	}

	ArrayReturnData getArray(simdjson::ondemand::value jsonData, const char* arrayName) {
		ArrayReturnData theValue{};
		if (jsonData[arrayName].get(theValue.theArray) == simdjson::error_code::SUCCESS) {
			theValue.didItSucceed = true;
		}
		return theValue;
	}

	String getString(ObjectReturnData jsonData, const char* theKey) {
		StringView theValue{};
		if (jsonData.didItSucceed && jsonData.theObject[theKey].get(theValue) == simdjson::error_code::SUCCESS) {
			return static_cast<String>(theValue);
		}
		return static_cast<String>(theValue);
	}

	void parseObject(simdjson::ondemand::value jsonData, PresenceUpdateFlags& theData) {
		Uint8 theDataNew = static_cast<Uint8>(theData);

		auto theStringNew = getString(jsonData, "dekstop");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Dnd, true));
		}

		theStringNew = getString(jsonData, "mobile");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Dnd, true));
		}

		theStringNew = getString(jsonData, "web");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Dnd, true));
		}
	}
};
