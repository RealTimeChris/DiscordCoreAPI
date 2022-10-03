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
/// Utilities.cpp - Source file for the Utilities.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file Utilities.cpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/Utilities.hpp>
#include <fstream>

namespace DiscordCoreInternal {

	WebSocketClose& WebSocketClose::operator=(uint16_t theValueNew) {
		this->theValue = this->mappingValues[theValueNew];
		return *this;
	};

	WebSocketClose::WebSocketClose(uint16_t theValueNew) {
		*this = theValueNew;
	};

	WebSocketClose::operator uint16_t() {
		for (auto& [key, value]: this->mappingValues) {
			if (theValue == value) {
				return key;
			}
		}
		return uint16_t{};
	}

	WebSocketClose::operator bool() {
		return utCast(this->theValue) & utCast(WebSocketCloseCode::We_Do_Reconnect);
	}
}

namespace DiscordCoreAPI {

	Snowflake& Snowflake::operator=(const std::string other) noexcept {
		if (!other.empty()) {
			this->theId = stoull(other);
		}
		return *this;
	}

	Snowflake::Snowflake(const std::string other) noexcept {
		*this = other;
	}

	Snowflake& Snowflake::operator=(const size_t other) noexcept {
		this->theId = other;
		return *this;
	}

	Snowflake::Snowflake(const size_t other) noexcept {
		*this = other;
	}

	Snowflake::operator size_t() noexcept {
		return this->theId;
	}

	EnumConverter& EnumConverter::operator=(EnumConverter&& other) noexcept {
		this->thePtr = other.thePtr;
		other.thePtr = nullptr;
		this->vectorType = other.vectorType;
		return *this;
	}

	EnumConverter::EnumConverter(EnumConverter&& other) noexcept {
		*this = std::move(other);
	}

	EnumConverter::operator std::vector<uint64_t>() {
		std::vector<uint64_t> theObject{};
		for (auto& value: *static_cast<std::vector<uint64_t>*>(this->thePtr)) {
			theObject.emplace_back(value);
		}
		return theObject;
	}

	EnumConverter::operator uint64_t() {
		return uint64_t{ *static_cast<uint64_t*>(this->thePtr) };
	}

	EnumConverter::~EnumConverter() {
		if (this->thePtr) {
			if (this->vectorType) {
				delete static_cast<std::vector<uint64_t>*>(this->thePtr);
			} else {
				delete static_cast<uint64_t*>(this->thePtr);
			}
		}
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(const StringType theData) noexcept {
		*this = ValueType::String;
		*this->string = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(const char* theData) noexcept {
		*this = ValueType::String;
		*this->string = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint64_t theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint32_t theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint16_t theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint8_t theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(int64_t theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(int32_t theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(int16_t theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(int8_t theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(double theData) noexcept {
		this->numberDouble = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(float theData) noexcept {
		this->numberDouble = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(bool theData) noexcept {
		this->boolean = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(ValueType theType) noexcept {
		switch (theType) {
			case ValueType::Object: {
				this->object = new ObjectType{};
				break;
			}
			case ValueType::Array: {
				this->array = new ArrayType{};
				break;
			}
			case ValueType::String: {
				this->string = new StringType{};
				break;
			}
			case ValueType::Bool: {
				this->boolean = static_cast<BoolType>(false);
				break;
			}
			case ValueType::Int64: {
				this->numberInt = static_cast<IntType>(0);
				break;
			}
			case ValueType::Uint64: {
				this->numberUint = static_cast<UintType>(0);
				break;
			}
			case ValueType::Float: {
				this->numberDouble = static_cast<FloatType>(0.0);
				break;
			}
			case ValueType::Null: {
				break;
			}
		}
		return *this;
	}

	JsonObject::JsonValue::JsonValue(ValueType theType) noexcept {
		*this = theType;
	}

	void JsonObject::JsonValue::destroy(ValueType theType) {
		switch (theType) {
			case ValueType::Array: {
				delete this->array;
				break;
			}
			case ValueType::Object: {
				delete this->object;
				break;
			}
			case ValueType::String: {
				delete this->string;
				break;
			}
		}
	}

	JsonObject& JsonObject::operator=(EnumConverter theData) noexcept {
		this->theValue = uint64_t{ theData };
		this->theValue.numberUint = uint64_t{ theData };
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(EnumConverter theData) noexcept {
		*this = std::move(theData);
	}

	JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
		if (this->theType != ValueType::Null) {
			this->theValue.destroy(this->theType);
		}
		switch (theKey.theType) {
			case ValueType::Object: {
				this->theValue = ValueType::Object;
				for (auto& [key, value]: *theKey.theValue.object) {
					this->theValue.object->emplace(key, std::move(value));
				}
				break;
			}
			case ValueType::Array: {
				this->theValue = ValueType::Array;
				for (auto& value: *theKey.theValue.array) {
					this->theValue.array->emplace_back(std::move(value));
				}
				break;
			}
			case ValueType::String: {
				this->theValue = ValueType::String;
				*this->theValue.string = *theKey.theValue.string;
				break;
			}
			case ValueType::Bool: {
				this->theValue.boolean = theKey.theValue.boolean;
				break;
			}
			case ValueType::Int64: {
				this->theValue.numberInt = theKey.theValue.numberInt;
				break;
			}
			case ValueType::Uint64: {
				this->theValue.numberUint = theKey.theValue.numberUint;
				break;
			}
			case ValueType::Float: {
				this->theValue.numberDouble = theKey.theValue.numberDouble;
				break;
			}
			case ValueType::Null: {
				break;
			}
		}
		this->theKey = theKey.theKey;
		this->theString = theKey.theString;
		this->theType = theKey.theType;
		return *this;
	}

	JsonObject::JsonObject(const JsonObject& theKey) noexcept {
		*this = theKey;
	}

	JsonObject& JsonObject::operator=(const std::string theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::String;
		return *this;
	}

	JsonObject::JsonObject(const std::string theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(const char* theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::String;
		return *this;
	}

	JsonObject::JsonObject(const char* theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(uint64_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(uint64_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(uint32_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(uint32_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(uint16_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(uint16_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(uint8_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(uint8_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(int64_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(int64_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(int32_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(int32_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(int16_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(int16_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(int8_t theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(int8_t theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(double theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Float;
		return *this;
	}

	JsonObject::JsonObject(double theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(float theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Float;
		return *this;
	}

	JsonObject::JsonObject(float theData) noexcept {
		this->theValue = theData;
	}

	JsonObject& JsonObject::operator=(bool theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Bool;
		return *this;
	}

	JsonObject::JsonObject(bool theData) noexcept {
		*this = theData;
	}

	JsonObject::JsonObject(const char* theKey, ValueType theType) noexcept {
		this->theKey = theKey;
		this->theType = theType;
		this->theValue = this->theType;
	}

	JsonObject& JsonObject::operator=(ValueType theType) noexcept {
		this->theValue = theType;
		this->theType = theType;
		return *this;
	}

	JsonObject& JsonObject::operator[](size_t index) const {
		return this->theValue.array->operator[](index);
	}

	JsonObject& JsonObject::operator[](size_t index) {
		if (this->theType == ValueType::Null) {
			this->theType = ValueType::Array;
			this->theValue = ValueType::Array;
		}

		if (this->theType == ValueType::Array) {
			if (index >= this->theValue.array->size()) {
				this->theValue.array->resize(index + 1);
			}

			return this->theValue.array->operator[](index);
		}
		throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
	}

	JsonObject& JsonObject::operator[](const typename ObjectType::key_type& key) const {
		if (this->theType == ValueType::Object) {
			auto result = this->theValue.object->emplace(key, nullptr);
			return result.first->second;
		}
		throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
	}

	JsonObject& JsonObject::operator[](typename ObjectType::key_type key) {
		if (this->theType == ValueType::Null) {
			this->theType = ValueType::Object;
			this->theValue = ValueType::Object;
		}

		if (this->theType == ValueType::Object) {
			auto result = this->theValue.object->emplace(std::move(key), JsonObject{});
			return result.first->second;
		}
		throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
	}

	JsonObject::operator std::string() noexcept {
		std::string theString{};
		this->dump(*this, theString);
		return theString;
	}

	void JsonObject::dump(const JsonObject& theData, std::string& theString) {
		switch (theData.theType) {
			case ValueType::Object: {
				if (theData.theValue.object->empty()) {
					theString += "{}";
				}

				theString += '{';

				size_t theIndex{};
				for (auto iterator = theData.theValue.object->cbegin(); iterator != theData.theValue.object->cend(); ++iterator) {
					theString += '\"';
					theString += iterator->first;
					theString += "\":";
					dump(iterator->second, theString);
					if (theIndex < theData.theValue.object->size() - 1) {
						theString += ',';
					}
					theIndex++;
				}
				theString += '}';
				break;
			}
			case ValueType::Array: {
				if (theData.theValue.array->empty()) {
					theString += "[]";
					break;
				}

				theString += '[';

				for (auto iterator = theData.theValue.array->cbegin(); iterator != theData.theValue.array->cend() - 1; ++iterator) {
					dump(*iterator, theString);
					theString += ',';
				}

				dump(theData.theValue.array->back(), theString);

				theString += ']';
				break;
			}

			case ValueType::String: {
				theString += '\"';
				theString += *theData.theValue.string;
				theString += '\"';
				break;
			}
			case ValueType::Bool: {
				std::stringstream theStream{};
				theStream << std::boolalpha << theData.theValue.boolean;
				theString += theStream.str();
				break;
			}
			case ValueType::Float: {
				theString += std::to_string(theData.theValue.numberDouble);
				break;
			}
			case ValueType::Uint64: {
				theString += std::to_string(theData.theValue.numberUint);
				break;
			}
			case ValueType::Int64: {
				theString += std::to_string(theData.theValue.numberInt);
				break;
			}
			case ValueType::Null: {
				theString += "null";
				break;
			}
			case ValueType::Null_Ext: {
				theString += "[]";
				break;
			}
		}
		return;
	}

	void JsonObject::pushBack(JsonObject other) noexcept {
		if (this->theType == ValueType::Null) {
			this->theType = ValueType::Array;
			this->theValue = ValueType::Array;
			this->theKey = theKey;
		} else if (this->theType == ValueType::Object) {
			this->theValue.object->emplace(theKey, ValueType::Array);
			this->theValue.object->at(theKey).theValue.array->emplace_back(other);
		}

		if (this->theType == ValueType::Array) {
			this->theValue.array->emplace_back(other);
		}
	};

	JsonObject::~JsonObject() noexcept {
		this->theValue.destroy(this->theType);
	}

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*theFunction)( void )) {
		outputSttream << theFunction();
		return outputSttream;
	}

	ConfigManager::ConfigManager(const DiscordCoreClientConfig& theConfigNew) {
		this->theConfig = theConfigNew;
	}

	const bool ConfigManager::doWePrintWebSocketSuccessMessages() {
		return this->theConfig.logOptions.logWebSocketSuccessMessages;
	}

	const bool ConfigManager::doWePrintWebSocketErrorMessages() {
		return this->theConfig.logOptions.logWebSocketErrorMessages;
	}

	const bool ConfigManager::doWePrintHttpsSuccessMessages() {
		return this->theConfig.logOptions.logHttpsSuccessMessages;
	}

	const bool ConfigManager::doWePrintHttpsErrorMessages() {
		return this->theConfig.logOptions.logHttpsErrorMessages;
	}

	const bool ConfigManager::doWePrintFFMPEGSuccessMessages() {
		return this->theConfig.logOptions.logFFMPEGSuccessMessages;
	}

	const bool ConfigManager::doWePrintFFMPEGErrorMessages() {
		return this->theConfig.logOptions.logFFMPEGErrorMessages;
	}

	const bool ConfigManager::doWePrintGeneralSuccessMessages() {
		return this->theConfig.logOptions.logGeneralSuccessMessages;
	}

	const bool ConfigManager::doWePrintGeneralErrorMessages() {
		return this->theConfig.logOptions.logGeneralErrorMessages;
	}

	const bool ConfigManager::doWeCacheChannels() {
		return this->theConfig.cacheOptions.cacheChannels;
	}

	const bool ConfigManager::doWeCacheUsers() {
		return this->theConfig.cacheOptions.cacheUsers;
	}

	const bool ConfigManager::doWeCacheGuilds() {
		return this->theConfig.cacheOptions.cacheGuilds;
	}

	const bool ConfigManager::doWeCacheRoles() {
		return this->theConfig.cacheOptions.cacheRoles;
	}

	const DiscordCoreInternal::UpdatePresenceData ConfigManager::getPresenceData() {
		return this->theConfig.presenceData;
	}

	const std::string ConfigManager::getBotToken() {
		return this->theConfig.botToken;
	}

	const uint32_t ConfigManager::getTotalShardCount() {
		return this->theConfig.shardOptions.totalNumberOfShards;
	}

	const uint32_t ConfigManager::getStartingShard() {
		return this->theConfig.shardOptions.startingShard;
	}

	const uint32_t ConfigManager::getShardCountForThisProcess() {
		return this->theConfig.shardOptions.numberOfShardsForThisProcess;
	}

	const std::string ConfigManager::getConnectionAddress() {
		return this->theConfig.connectionAddress;
	}

	void ConfigManager::setConnectionAddress(const std::string& connectionAddressNew) {
		this->theConfig.connectionAddress = connectionAddressNew;
	}

	const std::string ConfigManager::getConnectionPort() {
		return this->theConfig.connectionPort;
	}

	void ConfigManager::setConnectionPort(const std::string& connectionPortNew) {
		this->theConfig.connectionPort = connectionPortNew;
	}

	const std::vector<RepeatedFunctionData> ConfigManager::getFunctionsToExecute() {
		return this->theConfig.functionsToExecute;
	}

	const TextFormat ConfigManager::getTextFormat() {
		return this->theConfig.textFormat;
	}

	const GatewayIntents ConfigManager::getGatewayIntents() {
		return this->theConfig.theIntents;
	}

	StringWrapper& StringWrapper::operator=(StringWrapper&& other) noexcept {
		if (this != &other) {
			this->thePtr.reset(nullptr);
			this->thePtr = std::move(other.thePtr);
			other.thePtr.reset(nullptr);
			other.thePtr = nullptr;
		}
		return *this;
	}

	StringWrapper::StringWrapper(StringWrapper&& other) noexcept {
		*this = std::move(other);
	}

	StringWrapper& StringWrapper::operator=(const StringWrapper& other) {
		if (this != &other) {
			this->thePtr.reset(nullptr);
			std::stringstream theStream{};
			if (other.thePtr) {
				theStream << other.thePtr;
			}
			auto theLength = theStream.str().size();
			this->thePtr = std::make_unique<char[]>(theLength + 1);
			for (uint64_t x = 0; x < theLength; ++x) {
				this->thePtr[x] = other.thePtr[x];
			}
		}
		return *this;
	}

	StringWrapper::StringWrapper(const StringWrapper& other) {
		*this = other;
	}

	StringWrapper& StringWrapper::operator=(const std::string& theString) {
		auto theLength = theString.size();
		this->thePtr.reset(nullptr);
		this->thePtr = std::make_unique<char[]>(theLength + 1);
		for (int32_t x = 0; x < theLength; ++x) {
			this->thePtr[x] = theString[x];
		}
		return *this;
	}

	StringWrapper::StringWrapper(const std::string& theString) {
		*this = theString;
	}

	StringWrapper& StringWrapper::operator=(const char* theString) {
		if (theString) {
			this->thePtr.reset(nullptr);
			std::stringstream theStream{};
			theStream << theString;
			int64_t theLength = theStream.str().size();
			this->thePtr = std::make_unique<char[]>(theLength + 1);
			for (int64_t x = 0; x < theLength; ++x) {
				this->thePtr[x] = theString[x];
			}
		}
		return *this;
	}

	StringWrapper::StringWrapper(const char* theString) {
		*this = theString;
	}

	StringWrapper::operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>() {
		std::stringstream theStream{};
		if (this->thePtr) {
			theStream << this->thePtr;
		}
		std::string theString{};
		for (uint32_t x = 0; x < theStream.str().size(); ++x) {
			theString.push_back(theStream.str()[x]);
		}
		return theString;
	}

	void StringWrapper::emplace_back(char theChar) {
		std::stringstream theStream{};
		if (this->thePtr) {
			theStream << this->thePtr;
		}
		auto theLength = theStream.str().size();
		this->thePtr = std::make_unique<char[]>(theLength + 2);
		for (uint64_t x = 0; x < theLength; ++x) {
			this->thePtr[x] = theStream.str()[x];
		}
		this->thePtr[theLength] = theChar;
	}

	size_t StringWrapper::size() {
		std::stringstream theStream{};
		if (this->thePtr) {
			theStream << this->thePtr;
		}
		auto theLength = theStream.str().size();
		return theLength;
	}

	const char* StringWrapper::data() {
		return this->thePtr.get();
	}

	ColorValue::ColorValue(uint32_t theColorValue) {
		this->theColor = theColorValue;
	}

	ColorValue::ColorValue(std::string theHexColorValue) {
		if (theHexColorValue == "") {
			theHexColorValue = "fefefe";
		}
		this->theColor = stoull(theHexColorValue, nullptr, 16);
	}

	RGBColorValue ColorValue::getRgbColorValue() {
		uint8_t red = static_cast<uint8_t>(this->theColor >> 16);
		uint8_t green = static_cast<uint8_t>(this->theColor >> 8);
		uint8_t blue = static_cast<uint8_t>(this->theColor);
		RGBColorValue theColor{};
		theColor.blue = blue;
		theColor.green = green;
		theColor.red = red;
		return theColor;
	}

	HexColorValue ColorValue::getHexColorValue() {
		std::stringstream theStream{};
		theStream << std::hex << this->theColor;
		return theStream.str();
	}

	uint32_t ColorValue::getIntColorValue() {
		return this->theColor;
	}

	IconHash& IconHash::operator=(const std::string theString) {
		std::string newHash{ theString };
		if (newHash.empty() || newHash == "0") {
			this->highBits = 0;
			this->lowBits = 0;
			return *this;
		}
		if (newHash.length() == 34 && newHash.substr(0, 2) == "a_") {
			newHash = newHash.substr(2);
		}
		if (newHash.length() != 32 && newHash.length() != 33) {
			throw std::length_error("IconHash must be exactly 32 characters in length, passed value is: '" + std::to_string(newHash.size()) + "', in length.");
		}
		this->lowBits = fromString<uint64_t>(newHash.substr(0, 16), std::hex);
		this->highBits = fromString<uint64_t>(newHash.substr(16, 16), std::hex);
		return *this;
	}

	IconHash::IconHash(const std::string theString) noexcept {
		*this = theString;
	}

	std::string IconHash::getIconHash() noexcept {
		if (this->highBits == 0 || this->lowBits == 0) {
			return {};
		} else {
			return std::string{ toHex(this->lowBits) + toHex(this->highBits) };
		}
	}

	bool IconHash::operator==(const IconHash& other) {
		return this->lowBits == other.lowBits && this->highBits == other.highBits;
	}

	uint64_t strtoull(const std::string& theString) {
		for (auto& value: theString) {
			if (!isdigit(value)) {
				return 0;
			}
		}
		if (theString != "") {
			return stoull(theString);
		} else {
			return 0;
		}
	}

	Permissions& Permissions::operator=(Permission&& other) {
		this->thePermissions = static_cast<uint64_t>(other);
		return *this;
	}

	Permissions::Permissions(Permission&& permsNew) {
		*this = std::move(permsNew);
	}

	Permissions& Permissions::operator=(const Permission& other) {
		this->thePermissions = static_cast<uint64_t>(other);
		return *this;
	}

	Permissions::Permissions(const Permission& permsNew) {
		*this = permsNew;
	}

	Permissions& Permissions::operator=(std::string&& other) {
		if (other.size() == 0 || other == "") {
			this->thePermissions = 0;
		} else {
			for (auto& value: other) {
				this->thePermissions = stoull(other);
			}
		}
		other = "";
		return *this;
	}

	Permissions::Permissions(std::string&& permsNew) {
		*this = std::move(permsNew);
	}

	Permissions& Permissions::operator=(const std::string& other) {
		if (other.size() == 0 || other == "") {
			this->thePermissions = 0;
		} else {
			this->thePermissions = stoull(other);
		}
		return *this;
	}

	Permissions::Permissions(const std::string& permsNew) {
		*this = permsNew;
	}


	Permissions& Permissions::operator=(uint64_t other) {
		this->thePermissions = other;
		return *this;
	}

	Permissions::Permissions(uint64_t permsNew) {
		*this = permsNew;
	}

	Permissions::operator uint64_t() {
		return this->thePermissions;
	}

	Permissions::operator std::string() {
		return std::string{ std::to_string(this->thePermissions) };
	}

	std::string Permissions::getCurrentChannelPermissions(const GuildMember& guildMember, ChannelData& channel) {
		std::string permsString = Permissions::computePermissions(guildMember, channel);
		return permsString;
	}

	bool Permissions::checkForPermission(const GuildMember& guildMember, ChannelData& channel, Permission permission) {
		std::string permissionsString = Permissions::computePermissions(guildMember, channel);
		if ((stoull(permissionsString) & static_cast<uint64_t>(permission)) == static_cast<uint64_t>(permission)) {
			return true;
		} else {
			return false;
		}
	}

	std::string Permissions::getCurrentGuildPermissions(const GuildMember& guildMember) {
		std::string permissions = Permissions::computeBasePermissions(guildMember);
		return permissions;
	}

	void Permissions::removePermissions(const std::vector<Permission>& permissionsToRemove) {
		uint64_t permissionsInteger = this->thePermissions;
		for (auto value: permissionsToRemove) {
			permissionsInteger &= ~static_cast<uint64_t>(value);
		}
		std::stringstream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	void Permissions::addPermissions(const std::vector<Permission>& permissionsToAdd) {
		uint64_t permissionsInteger = this->thePermissions;
		for (auto value: permissionsToAdd) {
			permissionsInteger |= static_cast<uint64_t>(value);
		}
		std::stringstream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	std::vector<std::string> Permissions::displayPermissions() {
		std::vector<std::string> returnVector{};
		uint64_t permissionsInteger = this->thePermissions;
		if (permissionsInteger & (1ll << 3)) {
			for (uint64_t x = 0; x < 41; ++x) {
				permissionsInteger |= 1ll << x;
			}
		}
		if (permissionsInteger & (1ll << 0)) {
			returnVector.emplace_back("Create Instant Invite");
		}
		if (permissionsInteger & (1ll << 1)) {
			returnVector.emplace_back("Kick Members");
		}
		if (permissionsInteger & (1ll << 2)) {
			returnVector.emplace_back("Ban Members");
		}
		if (permissionsInteger & (1ll << 3)) {
			returnVector.emplace_back("Administrator");
		}
		if (permissionsInteger & (1ll << 4)) {
			returnVector.emplace_back("Manage Channels");
		}
		if (permissionsInteger & (1ll << 5)) {
			returnVector.emplace_back("Manage Guild");
		}
		if (permissionsInteger & (1ll << 6)) {
			returnVector.emplace_back("Add Reactions");
		}
		if (permissionsInteger & (1ll << 7)) {
			returnVector.emplace_back("View Audit Log");
		}
		if (permissionsInteger & (1ll << 8)) {
			returnVector.emplace_back("Priority Speaker");
		}
		if (permissionsInteger & (1ll << 9)) {
			returnVector.emplace_back("Stream");
		}
		if (permissionsInteger & (1ll << 10)) {
			returnVector.emplace_back("View Channel");
		}
		if (permissionsInteger & (1ll << 11)) {
			returnVector.emplace_back("Send Messages");
		}
		if (permissionsInteger & (1ll << 12)) {
			returnVector.emplace_back("Send TTS Messages");
		}
		if (permissionsInteger & (1ll << 13)) {
			returnVector.emplace_back("Manage Messages");
		}
		if (permissionsInteger & (1ll << 14)) {
			returnVector.emplace_back("Embed Links");
		}
		if (permissionsInteger & (1ll << 15)) {
			returnVector.emplace_back("Attach Files");
		}
		if (permissionsInteger & (1ll << 16)) {
			returnVector.emplace_back("Read Message History");
		}
		if (permissionsInteger & (1ll << 17)) {
			returnVector.emplace_back("Mention Everyone");
		}
		if (permissionsInteger & (1ll << 18)) {
			returnVector.emplace_back("Use External Emoji");
		}
		if (permissionsInteger & (1ll << 19)) {
			returnVector.emplace_back("View Guild Insights");
		}
		if (permissionsInteger & (1ll << 20)) {
			returnVector.emplace_back("Connect");
		}
		if (permissionsInteger & (1ll << 21)) {
			returnVector.emplace_back("Speak");
		}
		if (permissionsInteger & (1ll << 22)) {
			returnVector.emplace_back("Mute Members");
		}
		if (permissionsInteger & (1ll << 23)) {
			returnVector.emplace_back("Deafen Members");
		}
		if (permissionsInteger & (1ll << 24)) {
			returnVector.emplace_back("Move Members");
		}
		if (permissionsInteger & (1ll << 25)) {
			returnVector.emplace_back("Use VAD");
		}
		if (permissionsInteger & (1ll << 26)) {
			returnVector.emplace_back("Change Nickname");
		}
		if (permissionsInteger & (1ll << 27)) {
			returnVector.emplace_back("Manage Nicknames");
		}
		if (permissionsInteger & (1ll << 28)) {
			returnVector.emplace_back("Manage Roles");
		}
		if (permissionsInteger & (1ll << 29)) {
			returnVector.emplace_back("Manage Webhooks");
		}
		if (permissionsInteger & (1ll << 30)) {
			returnVector.emplace_back("Manage Emojis And Stickers");
		}
		if (permissionsInteger & (1ll << 31)) {
			returnVector.emplace_back("Use Application Commands");
		}
		if (permissionsInteger & (1ll << 32)) {
			returnVector.emplace_back("Request To Speak");
		}
		if (permissionsInteger & (1ll << 33)) {
			returnVector.emplace_back("Manage Events");
		}
		if (permissionsInteger & (1ll << 34)) {
			returnVector.emplace_back("Manage Threads");
		}
		if (permissionsInteger & (1ll << 35)) {
			returnVector.emplace_back("Create Public Threads");
		}
		if (permissionsInteger & (1ll << 36)) {
			returnVector.emplace_back("Create Private Threads");
		}
		if (permissionsInteger & (1ll << 37)) {
			returnVector.emplace_back("Use External Stickers");
		}
		if (permissionsInteger & (1ll << 38)) {
			returnVector.emplace_back("Send Messages In Threads");
		}
		if (permissionsInteger & (1ll << 39)) {
			returnVector.emplace_back("Start Embedded Activities");
		}
		if (permissionsInteger & (1ll << 40)) {
			returnVector.emplace_back("Moderate Members");
		}
		return returnVector;
	}

	std::string Permissions::getCurrentPermissionString() {
		std::string theReturnString = std::to_string(this->thePermissions);
		return theReturnString;
	}

	std::string Permissions::getAllPermissions() {
		uint64_t allPerms{ 0 };
		for (uint64_t x = 0; x < 41; ++x) {
			allPerms |= 1ll << x;
		}
		std::stringstream stream{};
		stream << allPerms;
		return stream.str();
	}

	std::string Permissions::computeOverwrites(const std::string& basePermissions, const GuildMember& guildMember, ChannelData& channel) {
		if ((stoull(basePermissions) & static_cast<uint64_t>(Permission::Administrator)) == static_cast<uint64_t>(Permission::Administrator)) {
			return Permissions::getAllPermissions();
		}

		uint64_t permissions = stoull(basePermissions);
		for (int32_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites[x].id == guildMember.guildId) {
				permissions &= ~channel.permissionOverwrites[x].deny;
				permissions |= channel.permissionOverwrites[x].allow;
				break;
			}
		}
		std::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(Roles::getCachedRoleAsync({ .guildId = guildMember.guildId, .roleId = value }).get());
		}
		uint64_t allow{ 0 };
		uint64_t deny{ 0 };
		for (auto& value: guildMemberRoles) {
			for (int32_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
				if (value.id == channel.permissionOverwrites[x].id) {
					allow |= channel.permissionOverwrites[x].allow;
					deny |= channel.permissionOverwrites[x].deny;
				}
			}
		}
		permissions &= ~deny;
		permissions |= allow;
		for (int32_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites[x].id == guildMember.id) {
				permissions &= ~channel.permissionOverwrites[x].deny;
				permissions |= channel.permissionOverwrites[x].allow;
				break;
			}
		}
		return std::to_string(permissions);
	}

	std::string Permissions::computePermissions(const GuildMember& guildMember, ChannelData& channel) {
		std::string permissions = Permissions::computeBasePermissions(guildMember);
		permissions = Permissions::computeOverwrites(permissions, guildMember, channel);
		return permissions;
	}

	std::string Permissions::computeBasePermissions(const GuildMember& guildMember) {
		const GuildData guild = Guilds::getCachedGuildAsync({ .guildId = guildMember.guildId }).get();
		if (guild.ownerId == guildMember.id) {
			return Permissions::getAllPermissions();
		}
		std::vector<RoleData> guildRoles{};
		for (auto& value: guild.roles) {
			guildRoles.emplace_back(Roles::getCachedRoleAsync({ .guildId = guild.id, .roleId = value }).get());
		}
		RoleData roleEveryone{};
		for (auto& value: guildRoles) {
			if (value.id == guild.id) {
				roleEveryone = value;
			}
		}
		uint64_t permissions{};
		if (roleEveryone.permissions != 0) {
			permissions = roleEveryone.permissions;
		}
		GetGuildMemberRolesData getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId = guildMember.guildId;
		std::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(Roles::getCachedRoleAsync({ .guildId = guild.id, .roleId = value }).get());
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions;
		}

		if ((permissions & static_cast<uint64_t>(Permission::Administrator)) == static_cast<uint64_t>(Permission::Administrator)) {
			return Permissions::getAllPermissions();
		}

		return std::to_string(permissions);
	}

	void reportException(const std::string& currentFunctionName, std::source_location theLocation) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			std::stringstream theStream{};
			theStream << shiftToBrightRed() << "Error Report: \n"
					  << "Caught At: " << currentFunctionName << ", in File: " << theLocation.file_name() << " (" << std::to_string(theLocation.line()) << ":"
					  << std::to_string(theLocation.column()) << ")"
					  << "\nThe Error: \n"
					  << e.what() << reset() << std::endl
					  << std::endl;
			auto theReturnString = theStream.str();
			cout << theReturnString;
		}
	}

	void rethrowException(const std::string& currentFunctionName, std::source_location theLocation) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			std::stringstream theStream{};
			theStream << shiftToBrightRed() << "Caught At: " << currentFunctionName << ", in File: " << theLocation.file_name() << " (" << std::to_string(theLocation.line()) << ":"
					  << std::to_string(theLocation.column()) << ")"
					  << "\nThe Error: \n"
					  << e.what() << reset();
			auto theReturnString = theStream.str();
			cout << theReturnString;
			if (std::current_exception()) {
				std::rethrow_exception(std::current_exception());
			}
		}
	}

	std::string constructMultiPartData(std::string theData, const std::vector<File>& files) {
		const std::string boundary("boundary25");
		const std::string partStart("--" + boundary + "\r\nContent-Type: application/octet-stream\r\nContent-Disposition: form-data; ");

		std::string content("--" + boundary);

		content += "\r\nContent-Type: application/json\r\nContent-Disposition: form-data; "
				   "name=\"payload_json\"\r\n\r\n";
		content += theData + "\r\n";
		if (files.size() == 1) {
			content += partStart + "name=\"file\"; filename=\"" + files[0].fileName + "\"" + "\r\n\r\n";
			content += files[0].data;
		} else {
			for (uint8_t x = 0; x < files.size(); ++x) {
				content += partStart + "name=\"files[" + std::to_string(x) + "]\"; filename=\"" + files[x].fileName + "\"\r\n\r\n";
				content += files[x].data;
				content += "\r\n";
			}
		}
		content += "\r\n--" + boundary + "--";
		return content;
	}

	std::string convertToLowerCase(const std::string& stringToConvert) {
		std::string newString;
		for (auto& value: stringToConvert) {
			if (isupper(static_cast<uint8_t>(value))) {
				newString += static_cast<char>(tolower(static_cast<uint8_t>(value)));
			} else {
				newString += value;
			}
		}
		return newString;
	}

	std::string base64Encode(const std::string& theString, bool url) {
		const char* base64CharsArray[2] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"abcdefghijklmnopqrstuvwxyz"
											"0123456789"
											"+/",

			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789"
			"-_" };

		size_t encodedLength = (theString.size() + 2) / 3 * 4;

		uint8_t trailing_char = url ? '.' : '=';

		const char* base64Chars = base64CharsArray[url];

		std::string theReturnString{};
		theReturnString.reserve(encodedLength);
		DiscordCoreAPI::StopWatch theStopWatch{ 1500ms };
		uint64_t pos = 0;
		while (pos < theString.size()) {
			if (theStopWatch.hasTimePassed()) {
				break;
			}
			theReturnString.push_back(base64Chars[(theString[static_cast<uint64_t>(pos + 0)] & 0xfc) >> 2]);

			if (static_cast<uint64_t>(pos + 1) < theString.size()) {
				theReturnString.push_back(base64Chars[((theString[static_cast<int64_t>(pos + 0)] & 0x03) << 4) + ((theString[static_cast<int64_t>(pos + 1)] & 0xf0) >> 4)]);

				if (static_cast<uint64_t>(pos + 2) < theString.size()) {
					theReturnString.push_back(base64Chars[((theString[static_cast<int64_t>(pos + 1)] & 0x0f) << 2) + ((theString[static_cast<int64_t>(pos + 2)] & 0xc0) >> 6)]);
					theReturnString.push_back(base64Chars[theString[static_cast<int64_t>(pos + 2)] & 0x3f]);
				} else {
					theReturnString.push_back(base64Chars[(theString[static_cast<int64_t>(pos + 1)] & 0x0f) << 2]);
					theReturnString.push_back(trailing_char);
				}
			} else {
				theReturnString.push_back(base64Chars[(theString[static_cast<int64_t>(pos + 0)] & 0x03) << 4]);
				theReturnString.push_back(trailing_char);
				theReturnString.push_back(trailing_char);
			}

			pos += 3;
		}

		return theReturnString;
	}

	std::string loadFileContents(const std::string& filePath) {
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		std::ostringstream stream{};
		stream << file.rdbuf();
		return stream.str();
	}

	std::string utf8MakeValid(const std::string& inputString) {
		std::string theReturnString{};
		for (auto& value: inputString) {
			if (value >= 128 || value < 0) {
				int32_t theDifference = 0 - value;
				theReturnString.push_back(value + theDifference);
			} else {
				theReturnString.push_back(value);
			}
		}
		return theReturnString;
	}

	std::string urlEncode(const std::string& inputString) {
		std::ostringstream escaped{};
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = inputString.begin(), n = inputString.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int32_t(static_cast<uint8_t>(c));
			escaped << std::nouppercase;
		}
		return escaped.str();
	}

	void spinLock(uint64_t timeInNsToSpinLockFor) {
		uint64_t startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		uint64_t timePassed{ 0 };
		while (timePassed < timeInNsToSpinLockFor) {
			timePassed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - startTime;
		}
	}

	std::string generateBase64EncodedKey() {
		std::string theReturnString{};
		theReturnString.resize(16);
		std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()) };
		for (uint32_t x = 0; x < 16; ++x) {
			theReturnString[x] = static_cast<uint8_t>((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * 255.0f);
		}
		theReturnString = base64Encode(theReturnString, false);
		return theReturnString;
	}

	std::string shiftToBrightGreen() {
		return std::string("\033[1;40;92m");
	}

	std::string shiftToBrightBlue() {
		return std::string("\033[1;40;96m");
	}

	std::string shiftToBrightRed() {
		return std::string("\033[1;40;91m");
	}

	bool nanoSleep(int64_t ns) {
#ifdef _WIN32
		HANDLE timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
		LARGE_INTEGER largeInt{ .QuadPart = -ns / 100 };
		if (!timer) {
			return false;
		}

		if (!SetWaitableTimerEx(timer, &largeInt, 0, NULL, NULL, NULL, 0)) {
			CloseHandle(timer);
			return false;
		}
		WaitForSingleObjectEx(timer, INFINITE, false);
		CloseHandle(timer);
#else
		std::this_thread::sleep_for(std::chrono::nanoseconds{ ns });
#endif
		return true;
	}

	std::string reset() {
		return std::string("\033[0m");
	}

	std::string getTimeAndDate() {
		const time_t now = std::time(nullptr);
		tm time = *std::localtime(&now);
		std::string timeStamp{};
		timeStamp.resize(48);
		if (time.tm_isdst) {
			if (time.tm_hour + 4 >= 24) {
				time.tm_hour = 0 + time.tm_hour + 4 - 24;

			} else {
				time.tm_hour = time.tm_hour + 4;
			}
		} else {
			if (time.tm_hour + 5 >= 24) {
				time.tm_hour = 0 + time.tm_hour + 5 - 24;
			} else {
				time.tm_hour = time.tm_hour + 5;
			}
		}
		size_t size = strftime(timeStamp.data(), 48, "%F %R", &time);
		timeStamp.resize(size);
		return timeStamp;
	}

	std::string escapeCharacters(std::string_view theString) {
		std::string theStringNew{};
		if (theStringNew.size() <= theString.size() * 2) {
			theStringNew.resize(theString.size() * 2);
		}
		size_t theIndex{};
		for (uint32_t x = 0; x < theString.size(); ++x) {
			switch (static_cast<char>(theString[x])) {
				case 0x00: {
					break;
				}
				case 0x22: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('"');
					theIndex += 2;
					break;
				}
				case 0x5c: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('\\');
					theIndex += 2;
					break;
				}
				case 0x07: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('a');
					theIndex += 2;
					break;
				}
				case 0x08: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('b');
					theIndex += 2;
					break;
				}
				case 0x0C: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('f');
					theIndex += 2;
					break;
				}
				case 0x0A: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('n');
					theIndex += 2;
					break;
				}
				case 0x0D: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('r');
					theIndex += 2;
					break;
				}
				case 0x0B: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('v');
					theIndex += 2;
					break;
				}
				case 0x09: {
					theStringNew[theIndex] = static_cast<char>('\\');
					theStringNew[theIndex + 1] = static_cast<char>('t');
					theIndex += 2;
					break;
				}
				default: {
					theStringNew[theIndex] = theString[x];
					theIndex++;
					break;
				}
			}
		}
		if (theStringNew.size() > 1) {
			if (theStringNew[theStringNew.size() - 1] == '\0') {
				theStringNew.erase(theStringNew.size() - 1);
			}
		}
		theStringNew.resize(theIndex);
		return theStringNew;
	}

	template<typename Object> std::unordered_map<std::string, UnboundedMessageBlock<Object>*> ObjectCollector<Object>::objectsBufferMap{};
};

namespace DiscordCoreInternal {

	std::string_view StringBuffer::operator[](LengthData size) {
		if (this->whichOneAreWeOn == 0) {
			std::string_view theString{ this->theString01.data() + size.offSet, size.length };
			return theString;
		} else {
			std::string_view theString{ this->theString02.data() + size.offSet, size.length };
			return theString;
		}
	}

	void StringBuffer::erase(size_t offSet, size_t amount) {
		this->theSize = this->theSize - amount;
		if (this->whichOneAreWeOn == 0) {
			memcpy(this->theString02.data(), this->theString01.data() + amount, this->theSize);
			this->whichOneAreWeOn = 1;
		} else {
			memcpy(this->theString01.data(), this->theString02.data() + amount, this->theSize);
			this->whichOneAreWeOn = 0;
		}
	}

	void StringBuffer::writeData(const char* thePtr, size_t theSize) {
		if (this->whichOneAreWeOn == 0) {
			if (this->theSize + theSize < this->theString01.size()) {
				memcpy(this->theString01.data() + this->theSize, thePtr, theSize);
				this->theSize += theSize;
			}
		} else {
			if (this->theSize + theSize < this->theString02.size()) {
				memcpy(this->theString02.data() + this->theSize, thePtr, theSize);
				this->theSize += theSize;
			}
		}
	}

	StringBuffer::operator std::string_view() {
		if (this->whichOneAreWeOn == 0) {
			std::string_view theString{ this->theString01.data(), this->theSize };
			return theString;
		} else {
			std::string_view theString{ this->theString02.data(), this->theSize };
			return theString;
		}
	}

	char StringBuffer::operator[](size_t theIndex) {
		if (this->whichOneAreWeOn == 0) {
			return this->theString01[theIndex];
		} else {
			return this->theString02[theIndex];
		}
	}

	size_t StringBuffer::size() {
		return this->theSize;
	}

	void StringBuffer::clear() {
		this->whichOneAreWeOn = 0;
		this->theSize = 0;
	}

	char* StringBuffer::data() {
		if (this->whichOneAreWeOn == 0) {
			return this->theString01.data();
		} else {
			return this->theString02.data();
		}
	}
}