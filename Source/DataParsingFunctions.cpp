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

	JsonParseError::JsonParseError(int32_t theCode) : std::runtime_error(theErrors[theCode]){};

	Snowflake getId(simdjson::ondemand::value jsonObjectData, const char* theKey) {
		return Snowflake{ DiscordCoreAPI::strtoull(getString(jsonObjectData, theKey)) };
	}

	float getFloat(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			double theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<float>(theValue);
			} else {
				return 0.0f;
			}
		} catch (...) {
			reportException("getFloat()");
			return 0.0f;
		}
	}

	bool getBool(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			bool theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return theValue;
			} else {
				return false;
			}
		} catch (...) {
			reportException("getBool()");
			return 0;
		}
	}

	uint8_t getUint8(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<uint8_t>(theValue);
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint8()");
			return 0;
		}
	}

	uint16_t getUint16(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<uint16_t>(theValue);
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint16()");
			return 0;
		}
	}

	uint32_t getUint32(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return static_cast<uint32_t>(theValue);
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint32()");
			return 0;
		}
	}

	uint64_t getUint64(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			uint64_t theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return theValue;
			} else {
				return 0;
			}
		} catch (...) {
			reportException("getUint64()");
			return 0;
		}
	}

	std::string getString(simdjson::ondemand::value jsonData, const char* theKey) {
		try {
			std::string_view theValue{};
			if (jsonData.type() != simdjson::ondemand::json_type::null) {
				jsonData[theKey].get(theValue);
				return std::string{ theValue.data(), theValue.size() };
			} else {
				return "";
			}
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	ObjectReturnData getObject(ArrayReturnData jsonObjectData, size_t objectIndex, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (jsonObjectData.didItSucceed) {
				auto theResult = jsonObjectData.theArray.at(objectIndex).get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (jsonObjectData.didItSucceed) {
				auto theResult = jsonObjectData.theObject[objectName].get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName) {
		ArrayReturnData theValue{};
		try {
			if (jsonObjectData.didItSucceed) {
				auto theResult = jsonObjectData.theObject[arrayName].get(theValue.theArray);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getArray()");
			return theValue;
		}
		return theValue;
	}

	ObjectReturnData getObject(simdjson::ondemand::array jsonObjectData, size_t objectIndex, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (!jsonObjectData.at(objectIndex).is_null()) {
				auto theResult = jsonObjectData.at(objectIndex).get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName, std::source_location theLocation) {
		ObjectReturnData theValue{};
		try {
			if (!jsonObjectData.is_null()) {
				auto theResult = jsonObjectData[objectName].get(theValue.theObject);
				if (theResult == simdjson::error_code::SUCCESS) {
					theValue.didItSucceed = true;
					return theValue;
				} else {
					return theValue;
				}
			}
		} catch (...) {
			reportException("getObject()", theLocation);
			return theValue;
		}
		return theValue;
	}

	ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName) {
		ArrayReturnData theValue{};
		try {
			auto theResult = jsonObjectData[arrayName].get(theValue.theArray);
			if (theResult == simdjson::error_code::SUCCESS) {
				theValue.didItSucceed = true;
				return theValue;
			} else {
				return theValue;
			}
		} catch (...) {
			reportException("getArray()");
			return theValue;
		}
	}

	std::string getString(ObjectReturnData jsonData, const char* theKey) {
		try {
			std::string_view theValue{};
			if (jsonData.didItSucceed) {
				jsonData.theObject[theKey].get(theValue);
				return std::string{ theValue.data(), theValue.size() };
			} else {
				return "";
			}
		} catch (...) {
			reportException("getString()");
			return "";
		}
	}

	template<> void parseObject(simdjson::ondemand::value jsonObjectData, PresenceUpdateFlags& theData) {
		uint8_t theDataNew = static_cast<uint8_t>(theData);

		auto theStringNew = getString(jsonObjectData, "dekstop");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Desktop_Dnd, true));
		}

		theStringNew = getString(jsonObjectData, "mobile");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Mobile_Dnd, true));
		}

		theStringNew = getString(jsonObjectData, "web");

		if (theStringNew == "online") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Online, true));
		} else if (theStringNew == "idle") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Idle, true));
		} else if (theStringNew == "dnd") {
			theData = static_cast<PresenceUpdateFlags>(setBool(theDataNew, PresenceUpdateFlags::Web_Dnd, true));
		}
	}
};
