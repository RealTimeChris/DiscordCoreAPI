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

	Snowflake getId(simdjson::ondemand::value jsonData, const char* key) {
		return Snowflake{ getString(jsonData, key) };
	}

	Snowflake getId(simdjson::ondemand::value jsonObjectData) {
		return Snowflake{ getString(jsonObjectData) };
	}

	int64_t getInt64(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return int64_t{ value };
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	int32_t getInt32(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return static_cast<int32_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	int16_t getInt16(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return static_cast<int16_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	int8_t getInt8(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return static_cast<int8_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint64_t getUint64(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return uint64_t{ value };
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint32_t getUint32(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return static_cast<uint32_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint16_t getUint16(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return static_cast<uint16_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint8_t getUint8(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return static_cast<uint8_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	double getFloat(simdjson::ondemand::value jsonData, const char* key) {
		double value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return double{ value };
			} else {
				return 0.0f;
			}
		} else {
			return 0.0f;
		}
	}

	bool getBool(simdjson::ondemand::value jsonData, const char* key) {
		bool value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return bool{ value };
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	std::string getString(simdjson::ondemand::value jsonData, const char* key) {
		std::string_view value{};
		if (jsonData.type() != simdjson::ondemand::json_type::null) {
			if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
				return std::string{ value.data(), value.size() };
			} else {
				return "";
			}
		} else {
			return "";
		}
	}

	bool getObject(simdjson::ondemand::value& object, const char* key, simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			if (jsonObjectData[key].get(object) == simdjson::error_code::SUCCESS) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool getArray(simdjson::ondemand::array& array, const char* key, simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			if (jsonObjectData[key].get(array) == simdjson::error_code::SUCCESS) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool getArray(simdjson::ondemand::array& array, simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			if (jsonObjectData.get(array) == simdjson::error_code::SUCCESS) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	std::string getString(ObjectReturnData jsonData, const char* key) {
		std::string_view value{};
		if (jsonData.didItSucceed && jsonData.object[key].get(value) == simdjson::error_code::SUCCESS) {
			return static_cast<std::string>(value);
		}
		return static_cast<std::string>(value);
	}

	std::string getString(simdjson::ondemand::value jsonObjectData) {
		std::string_view value{};
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			if (jsonObjectData.get(value) == simdjson::error_code::SUCCESS) {
				return std::string{ value.data(), value.size() };
			} else {
				return "";
			}
		} else {
			return "";
		}
	}

	ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName) {
		ObjectReturnData value{};
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			if (jsonObjectData[objectName].get(value.object) == simdjson::error_code::SUCCESS) {
				value.didItSucceed = true;
			}
		}
		return value;
	}

	ObjectReturnData getObject(ObjectReturnData jsonData, const char* objectName) {
		ObjectReturnData value{};
		if (jsonData.didItSucceed && jsonData.object[objectName].get(value.object) == simdjson::error_code::SUCCESS) {
			value.didItSucceed = true;
		}
		return value;
	}

	ObjectReturnData getObject(ArrayReturnData jsonData, uint64_t objectIndex) {
		ObjectReturnData value{};
		if (jsonData.didItSucceed && jsonData.arrayValue.at(objectIndex).get(value.object) == simdjson::error_code::SUCCESS) {
			value.didItSucceed = true;
		}
		return value;
	}

	ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName) {
		ArrayReturnData value{};
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			if (jsonObjectData[arrayName].get(value.arrayValue) == simdjson::error_code::SUCCESS) {
				value.didItSucceed = true;
			}
		}
		return value;
	}

	ArrayReturnData getArray(ObjectReturnData jsonData, const char* arrayName) {
		ArrayReturnData value{};
		if (jsonData.didItSucceed && jsonData.object[arrayName].get(value.arrayValue) == simdjson::error_code::SUCCESS) {
			value.didItSucceed = true;
		}
		return value;
	}

	void parseObject(simdjson::ondemand::value jsonObjectData, PresenceUpdateFlags& data) {
		uint8_t dataNew = static_cast<uint8_t>(data);

		auto stringNew = getString(jsonObjectData, "dekstop");

		if (stringNew == "online") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Desktop_Online, true);
		} else if (stringNew == "idle") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Desktop_Idle, true);
		} else if (stringNew == "dnd") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Desktop_Dnd, true);
		}

		stringNew = getString(jsonObjectData, "mobile");

		if (stringNew == "online") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Mobile_Online, true);
		} else if (stringNew == "idle") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Mobile_Idle, true);
		} else if (stringNew == "dnd") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Mobile_Dnd, true);
		}

		stringNew = getString(jsonObjectData, "web");

		if (stringNew == "online") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Web_Online, true);
		} else if (stringNew == "idle") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Web_Idle, true);
		} else if (stringNew == "dnd") {
			dataNew += setBool(dataNew, PresenceUpdateFlags::Web_Dnd, true);
		}

		data = static_cast<PresenceUpdateFlags>(dataNew);
	}
};
