﻿/*
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

	WebSocketClose& WebSocketClose::operator=(Uint16 theValueNew) {
		this->theValue = this->mappingValues[theValueNew];
		return *this;
	};

	WebSocketClose::WebSocketClose(Uint16 theValueNew) {
		*this = theValueNew;
	};

	WebSocketClose::operator Uint16() {
		for (auto& [key, value]: this->mappingValues) {
			if (theValue == value) {
				return key;
			}
		}
		return Uint16{};
	}

	WebSocketClose::operator Bool() {
		return utCast(this->theValue) & utCast(WebSocketCloseCode::We_Do_Reconnect);
	}
}

namespace DiscordCoreAPI {

	EnumConverter& EnumConverter::operator=(EnumConverter&& other) noexcept {
		this->theVector = std::move(other.theVector);
		this->vectorType = other.vectorType;
		this->theUint = other.theUint;
		return *this;
	}

	EnumConverter::EnumConverter(EnumConverter&& other) noexcept {
		*this = std::move(other);
	}

	EnumConverter::operator Vector<Uint64>() const noexcept {
		Vector<Uint64> theObject{};
		for (auto& value: this->theVector) {
			theObject.emplace_back(value);
		}
		return theObject;
	}

	EnumConverter::operator Uint64() const noexcept {
		return this->theUint;
	}

	bool EnumConverter::isItAVector() const noexcept {
		return this->vectorType;
	}

	bool EnumConverter::isItAVector() noexcept {
		return this->vectorType;
	}

	JsonObject::JsonValue::JsonValue() noexcept {};

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(const StringType& theData) noexcept {
		*this->string = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(StringType&& theData) noexcept {
		*this->string = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(const char* theData) noexcept {
		*this->string = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint64 theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint32 theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint16 theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint8 theData) noexcept {
		this->numberUint = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int64 theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int32 theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int16 theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int8 theData) noexcept {
		this->numberInt = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Double theData) noexcept {
		this->numberDouble = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Float theData) noexcept {
		this->numberDouble = theData;
		return *this;
	}

	JsonObject::JsonValue& JsonObject::JsonValue::operator=(Bool theData) noexcept {
		this->boolean = theData;
		return *this;
	}

	JsonObject::JsonValue::~JsonValue() noexcept {};

	JsonObject& JsonObject::operator=(EnumConverter&& theData) noexcept {
		if (theData.isItAVector()) {
			this->set(std::make_unique<ArrayType>());
			for (auto& value: theData.operator Vector<Uint64>()) {
				this->theValue.array->push_back(value);
			}
		} else {
			this->theValue.numberUint = Uint64{ theData };
			this->theType = ValueType::Uint64;
		}
		return *this;
	}

	JsonObject::JsonObject(EnumConverter&& theData) noexcept {
		*this = std::move(theData);
	}

	JsonObject& JsonObject::operator=(const EnumConverter& theData) noexcept {
		if (theData.isItAVector()) {
			this->set(std::make_unique<ArrayType>());
			for (auto& value: theData.operator Vector<Uint64>()) {
				this->theValue.array->push_back(value);
			}
		} else {
			this->theValue.numberUint = Uint64{ theData };
			this->theType = ValueType::Uint64;
		}
		return *this;
	}

	JsonObject::JsonObject(const EnumConverter& theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(JsonObject&& theKey) noexcept {
		switch (theKey.theType) {
			case ValueType::Object: {
				this->set(std::make_unique<ObjectType>());
				*this->theValue.object = *theKey.theValue.object;
				break;
			}
			case ValueType::Array: {
				this->set(std::make_unique<ArrayType>());
				*this->theValue.array = *theKey.theValue.array;
				break;
			}
			case ValueType::String: {
				this->set(std::make_unique<StringType>());
				*this->theValue.string = *theKey.theValue.string;
				break;
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
		this->theType = theKey.theType;
		return *this;
	}

	JsonObject::JsonObject(JsonObject&& theKey) noexcept {
		*this = std::move(theKey);
	}

	JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
		switch (theKey.theType) {
			case ValueType::Object: {
				this->set(std::make_unique<ObjectType>());
				for (auto& [key, value]: *theKey.theValue.object) {
					this->theValue.object->emplace(key, std::move(value));
				}
				this->theType = ValueType::Object;
				break;
			}
			case ValueType::Array: {
				this->set(std::make_unique<ArrayType>());
				for (auto& value: *theKey.theValue.array) {
					this->theValue.array->emplace_back(std::move(value));
				}
				this->theType = ValueType::Array;
				break;
			}
			case ValueType::String: {
				this->set(std::make_unique<StringType>());
				*this->theValue.string = *theKey.theValue.string;
				this->theType = ValueType::String;
				break;
			}
			case ValueType::Bool: {
				this->theValue.boolean = theKey.theValue.boolean;
				this->theType = ValueType::Bool;
				break;
			}
			case ValueType::Int64: {
				this->theValue.numberInt = theKey.theValue.numberInt;
				this->theType = ValueType::Int64;
				break;
			}
			case ValueType::Uint64: {
				this->theValue.numberUint = theKey.theValue.numberUint;
				this->theType = ValueType::Uint64;
				break;
			}
			case ValueType::Float: {
				this->theValue.numberDouble = theKey.theValue.numberDouble;
				this->theType = ValueType::Float;
				break;
			}
			case ValueType::Null: {
				this->theType = ValueType::Null;
				break;
			}
			default: {
				break;
			}
		}
		this->theType = theKey.theType;
		return *this;
	}

	JsonObject::JsonObject(const JsonObject& theKey) noexcept {
		*this = theKey;
	}

	JsonObject& JsonObject::operator=(String&& theData) noexcept {
		this->set(std::make_unique<StringType>());
		*this->theValue.string = theData;
		this->theType = ValueType::String;
		return *this;
	}

	JsonObject::JsonObject(String&& theData) noexcept {
		*this = std::move(theData);
	}

	JsonObject& JsonObject::operator=(const String& theData) noexcept {
		this->set(std::make_unique<StringType>());
		*this->theValue.string = theData;
		this->theType = ValueType::String;
		return *this;
	}

	JsonObject::JsonObject(const String& theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(const char* theData) noexcept {
		this->set(std::make_unique<StringType>());
		*this->theValue.string = theData;
		this->theType = ValueType::String;
		return *this;
	}

	JsonObject::JsonObject(const char* theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Uint64 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(Uint64 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Uint32 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(Uint32 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Uint16 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(Uint16 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Uint8 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Uint64;
		return *this;
	}

	JsonObject::JsonObject(Uint8 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Int64 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(Int64 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Int32 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(Int32 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Int16 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(Int16 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Int8 theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Int64;
		return *this;
	}

	JsonObject::JsonObject(Int8 theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Double theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Float;
		return *this;
	}

	JsonObject::JsonObject(Double theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(Float theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Float;
		return *this;
	}

	JsonObject::JsonObject(Float theData) noexcept {
		this->theValue = theData;
	}

	JsonObject& JsonObject::operator=(Bool theData) noexcept {
		this->theValue = theData;
		this->theType = ValueType::Bool;
		return *this;
	}

	JsonObject::JsonObject(Bool theData) noexcept {
		*this = theData;
	}

	JsonObject& JsonObject::operator=(ValueType theType) noexcept {
		this->theType = theType;
		return *this;
	}

	JsonObject::JsonObject(ValueType theType) noexcept {
		*this = theType;
	}

	JsonObject& JsonObject::operator[](Uint64 index) const {
		return this->theValue.array->operator[](index);
	}

	JsonObject& JsonObject::operator[](Uint64 index) {
		if (this->theType == ValueType::Null) {
			this->set(std::make_unique<ArrayType>());
			this->theType = ValueType::Array;
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
			this->set(std::make_unique<ObjectType>());
			this->theType = ValueType::Object;
		}

		if (this->theType == ValueType::Object) {
			auto result = this->theValue.object->emplace(std::move(key), JsonObject{});
			return result.first->second;
		}
		throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
	}

	Void JsonObject::pushBack(JsonObject&& other) noexcept {
		if (this->theType == ValueType::Null) {
			this->set(std::make_unique<ArrayType>());
			this->theType = ValueType::Array;
		}

		if (this->theType == ValueType::Array) {
			this->theValue.array->emplace_back(std::move(other));
		}
	}

	Void JsonObject::pushBack(JsonObject& other) noexcept {
		if (this->theType == ValueType::Null) {
			this->set(std::make_unique<ArrayType>());
			this->theType = ValueType::Array;
		}

		if (this->theType == ValueType::Array) {
			this->theValue.array->emplace_back(std::move(other));
		}
	}

	JsonObject::operator String() const noexcept {
		String theString{};
		switch (this->theType) {
			case ValueType::Object: {
				if (this->theValue.object->empty()) {
					theString += "{}";
				}

				theString += '{';

				Uint64 theIndex{};
				for (auto iterator = this->theValue.object->cbegin(); iterator != this->theValue.object->cend(); ++iterator) {
					theString += '\"';
					theString += iterator->first;
					theString += "\":";
					theString += iterator->second;
					if (theIndex < this->theValue.object->size() - 1) {
						theString += ',';
					}
					theIndex++;
				}
				theString += '}';
				break;
			}
			case ValueType::Array: {
				if (this->theValue.array->empty()) {
					theString += "[]";
					break;
				}

				theString += '[';

				for (auto iterator = this->theValue.array->cbegin(); iterator != this->theValue.array->cend() - 1; ++iterator) {
					theString += *iterator;
					theString += ',';
				}

				theString += this->theValue.array->back();

				theString += ']';
				break;
			}

			case ValueType::String: {
				theString += '\"';
				theString += std::move(*this->theValue.string);
				theString += '\"';
				break;
			}
			case ValueType::Bool: {
				StringStream theStream{};
				theStream << std::boolalpha << this->theValue.boolean;
				theString += theStream.str();
				break;
			}
			case ValueType::Float: {
				theString += std::to_string(this->theValue.numberDouble);
				break;
			}
			case ValueType::Uint64: {
				theString += std::to_string(this->theValue.numberUint);
				break;
			}
			case ValueType::Int64: {
				theString += std::to_string(this->theValue.numberInt);
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
		return theString;
	}

	JsonObject::operator String() noexcept {
		String theString{};
		switch (this->theType) {
			case ValueType::Object: {
				if (this->theValue.object->empty()) {
					theString += "{}";
				}

				theString += '{';

				Uint64 theIndex{};
				for (auto iterator = this->theValue.object->cbegin(); iterator != this->theValue.object->cend(); ++iterator) {
					theString += '\"';
					theString += iterator->first;
					theString += "\":";
					theString += iterator->second;
					if (theIndex < this->theValue.object->size() - 1) {
						theString += ',';
					}
					theIndex++;
				}
				theString += '}';
				break;
			}
			case ValueType::Array: {
				if (this->theValue.array->empty()) {
					theString += "[]";
					break;
				}

				theString += '[';

				for (auto iterator = this->theValue.array->cbegin(); iterator != this->theValue.array->cend() - 1; ++iterator) {
					theString += *iterator;
					theString += ',';
				}

				theString += this->theValue.array->back();

				theString += ']';
				break;
			}

			case ValueType::String: {
				theString += '\"';
				theString += std::move(*this->theValue.string);
				theString += '\"';
				break;
			}
			case ValueType::Bool: {
				StringStream theStream{};
				theStream << std::boolalpha << this->theValue.boolean;
				theString += theStream.str();
				break;
			}
			case ValueType::Float: {
				theString += std::to_string(this->theValue.numberDouble);
				break;
			}
			case ValueType::Uint64: {
				theString += std::to_string(this->theValue.numberUint);
				break;
			}
			case ValueType::Int64: {
				theString += std::to_string(this->theValue.numberInt);
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
		return theString;
	}

	Void JsonObject::set(UniquePtr<String> p) {
		destroy();
		new (&this->theValue.string) UniquePtr<String>{ std::move(p) };
		this->theType = ValueType::String;
	}

	Void JsonObject::set(UniquePtr<ArrayType> p) {
		destroy();
		new (&this->theValue.string) UniquePtr<ArrayType>{ std::move(p) };
		this->theType = ValueType::Array;
	}

	Void JsonObject::set(UniquePtr<ObjectType> p) {
		destroy();
		new (&this->theValue.string) UniquePtr<ObjectType>{ std::move(p) };
		this->theType = ValueType::Object;
	}

	Void JsonObject::destroy() noexcept {
		switch (this->theType) {
			case ValueType::Array: {
				this->theValue.array.reset(nullptr);
				break;
			}
			case ValueType::Object: {
				this->theValue.object.reset(nullptr);
				break;
			}
			case ValueType::String: {
				this->theValue.string.reset(nullptr);
				break;
			}
		}
	}

	JsonObject::~JsonObject() noexcept {
		this->destroy();
	}

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const String& (*theFunction)(Void)) {
		outputSttream << theFunction();
		return outputSttream;
	}

	ConfigManager::ConfigManager(const DiscordCoreClientConfig& theConfigNew) {
		this->theConfig = theConfigNew;
	}

	const Bool ConfigManager::doWePrintWebSocketSuccessMessages() {
		return this->theConfig.logOptions.logWebSocketSuccessMessages;
	}

	const Bool ConfigManager::doWePrintWebSocketErrorMessages() {
		return this->theConfig.logOptions.logWebSocketErrorMessages;
	}

	const Bool ConfigManager::doWePrintHttpsSuccessMessages() {
		return this->theConfig.logOptions.logHttpsSuccessMessages;
	}

	const Bool ConfigManager::doWePrintHttpsErrorMessages() {
		return this->theConfig.logOptions.logHttpsErrorMessages;
	}

	const Bool ConfigManager::doWePrintFFMPEGSuccessMessages() {
		return this->theConfig.logOptions.logFFMPEGSuccessMessages;
	}

	const Bool ConfigManager::doWePrintFFMPEGErrorMessages() {
		return this->theConfig.logOptions.logFFMPEGErrorMessages;
	}

	const Bool ConfigManager::doWePrintGeneralSuccessMessages() {
		return this->theConfig.logOptions.logGeneralSuccessMessages;
	}

	const Bool ConfigManager::doWePrintGeneralErrorMessages() {
		return this->theConfig.logOptions.logGeneralErrorMessages;
	}

	const Bool ConfigManager::doWeCacheChannels() {
		return this->theConfig.cacheOptions.cacheChannels;
	}

	const Bool ConfigManager::doWeCacheUsers() {
		return this->theConfig.cacheOptions.cacheUsers;
	}

	const Bool ConfigManager::doWeCacheGuilds() {
		return this->theConfig.cacheOptions.cacheGuilds;
	}

	const Bool ConfigManager::doWeCacheRoles() {
		return this->theConfig.cacheOptions.cacheRoles;
	}

	const DiscordCoreInternal::UpdatePresenceData ConfigManager::getPresenceData() {
		return this->theConfig.presenceData;
	}

	const String ConfigManager::getBotToken() {
		return this->theConfig.botToken;
	}

	const Uint32 ConfigManager::getTotalShardCount() {
		return this->theConfig.shardOptions.totalNumberOfShards;
	}

	const Uint32 ConfigManager::getStartingShard() {
		return this->theConfig.shardOptions.startingShard;
	}

	const Uint32 ConfigManager::getShardCountForThisProcess() {
		return this->theConfig.shardOptions.numberOfShardsForThisProcess;
	}

	const String ConfigManager::getConnectionAddress() {
		return this->theConfig.connectionAddress;
	}

	Void ConfigManager::setConnectionAddress(const String& connectionAddressNew) {
		this->theConfig.connectionAddress = connectionAddressNew;
	}

	const String ConfigManager::getConnectionPort() {
		return this->theConfig.connectionPort;
	}

	Void ConfigManager::setConnectionPort(const String& connectionPortNew) {
		this->theConfig.connectionPort = connectionPortNew;
	}

	const Vector<RepeatedFunctionData> ConfigManager::getFunctionsToExecute() {
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
			StringStream theStream{};
			if (other.thePtr) {
				theStream << other.thePtr;
			}
			auto theLength = theStream.str().size();
			this->thePtr = std::make_unique<char[]>(theLength + 1);
			for (Uint64 x = 0; x < theLength; ++x) {
				this->thePtr[x] = other.thePtr[x];
			}
		}
		return *this;
	}

	StringWrapper::StringWrapper(const StringWrapper& other) {
		*this = other;
	}

	StringWrapper& StringWrapper::operator=(const String& theString) {
		auto theLength = theString.size();
		this->thePtr.reset(nullptr);
		this->thePtr = std::make_unique<char[]>(theLength + 1);
		for (Int32 x = 0; x < theLength; ++x) {
			this->thePtr[x] = theString[x];
		}
		return *this;
	}

	StringWrapper::StringWrapper(const String& theString) {
		*this = theString;
	}

	StringWrapper& StringWrapper::operator=(const char* theString) {
		if (theString) {
			this->thePtr.reset(nullptr);
			StringStream theStream{};
			theStream << theString;
			Int64 theLength = theStream.str().size();
			this->thePtr = std::make_unique<char[]>(theLength + 1);
			for (Int64 x = 0; x < theLength; ++x) {
				this->thePtr[x] = theString[x];
			}
		}
		return *this;
	}

	StringWrapper::StringWrapper(const char* theString) {
		*this = theString;
	}

	StringWrapper::operator String() {
		StringStream theStream{};
		if (this->thePtr) {
			theStream << this->thePtr;
		}
		String theString{};
		for (Uint32 x = 0; x < theStream.str().size(); ++x) {
			theString.push_back(theStream.str()[x]);
		}
		return theString;
	}

	Void StringWrapper::emplace_back(char theChar) {
		StringStream theStream{};
		if (this->thePtr) {
			theStream << this->thePtr;
		}
		auto theLength = theStream.str().size();
		this->thePtr = std::make_unique<char[]>(theLength + 2);
		for (Uint64 x = 0; x < theLength; ++x) {
			this->thePtr[x] = theStream.str()[x];
		}
		this->thePtr[theLength] = theChar;
	}

	Uint64 StringWrapper::size() {
		StringStream theStream{};
		if (this->thePtr) {
			theStream << this->thePtr;
		}
		auto theLength = theStream.str().size();
		return theLength;
	}

	const char* StringWrapper::data() {
		return this->thePtr.get();
	}

	ColorValue::ColorValue(Uint32 theColorValue) {
		this->theColor = theColorValue;
	}

	ColorValue::ColorValue(String theHexColorValue) {
		if (theHexColorValue == "") {
			theHexColorValue = "fefefe";
		}
		this->theColor = stoull(theHexColorValue, nullptr, 16);
	}

	RGBColorValue ColorValue::getRgbColorValue() {
		Uint8 red = static_cast<Uint8>(this->theColor >> 16);
		Uint8 green = static_cast<Uint8>(this->theColor >> 8);
		Uint8 blue = static_cast<Uint8>(this->theColor);
		RGBColorValue theColor{};
		theColor.blue = blue;
		theColor.green = green;
		theColor.red = red;
		return theColor;
	}

	HexColorValue ColorValue::getHexColorValue() {
		StringStream theStream{};
		theStream << std::hex << this->theColor;
		return theStream.str();
	}

	Uint32 ColorValue::getIntColorValue() {
		return this->theColor;
	}

	IconHash& IconHash::operator=(const String theString) {
		String newHash{ theString };
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
		this->lowBits = fromString<Uint64>(newHash.substr(0, 16), std::hex);
		this->highBits = fromString<Uint64>(newHash.substr(16, 16), std::hex);
		return *this;
	}

	IconHash::IconHash(const String theString) noexcept {
		*this = theString;
	}

	String IconHash::getIconHash() noexcept {
		if (this->highBits == 0 || this->lowBits == 0) {
			return {};
		} else {
			return String{ toHex(this->lowBits) + toHex(this->highBits) };
		}
	}

	Bool IconHash::operator==(const IconHash& other) {
		return this->lowBits == other.lowBits && this->highBits == other.highBits;
	}

	Uint64 strtoull(const StringView theString) {
		for (auto& value: theString) {
			if (!isdigit(value)) {
				return 0;
			}
		}
		if (!theString.empty() && theString != "") {
			return stoull(std::string{ theString });
		} else {
			return 0;
		}
	}

	Uint64 strtoull(const String& theString) {
		for (auto& value: theString) {
			if (!isdigit(value)) {
				return 0;
			}
		}
		if (!theString.empty() && theString != "") {
			return stoull(theString);
		} else {
			return 0;
		}
	}

	Permissions& Permissions::operator=(Permission&& other) {
		this->thePermissions = static_cast<Uint64>(other);
		return *this;
	}

	Permissions::Permissions(Permission&& permsNew) {
		*this = std::move(permsNew);
	}

	Permissions& Permissions::operator=(const Permission& other) {
		this->thePermissions = static_cast<Uint64>(other);
		return *this;
	}

	Permissions::Permissions(const Permission& permsNew) {
		*this = permsNew;
	}

	Permissions& Permissions::operator=(String&& other) {
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

	Permissions::Permissions(String&& permsNew) {
		*this = std::move(permsNew);
	}

	Permissions& Permissions::operator=(const String& other) {
		if (other.size() == 0 || other == "") {
			this->thePermissions = 0;
		} else {
			this->thePermissions = stoull(other);
		}
		return *this;
	}

	Permissions::Permissions(const String& permsNew) {
		*this = permsNew;
	}


	Permissions& Permissions::operator=(Uint64 other) {
		this->thePermissions = other;
		return *this;
	}

	Permissions::Permissions(Uint64 permsNew) {
		*this = permsNew;
	}

	Permissions::operator Uint64() {
		return this->thePermissions;
	}

	Permissions::operator String() {
		return String{ std::to_string(this->thePermissions) };
	}

	String Permissions::getCurrentChannelPermissions(const GuildMember& guildMember, ChannelData& channel) {
		String permsString = Permissions::computePermissions(guildMember, channel);
		return permsString;
	}

	Bool Permissions::checkForPermission(const GuildMember& guildMember, ChannelData& channel, Permission permission) {
		String permissionsString = Permissions::computePermissions(guildMember, channel);
		if ((stoull(permissionsString) & static_cast<Uint64>(permission)) == static_cast<Uint64>(permission)) {
			return true;
		} else {
			return false;
		}
	}

	String Permissions::getCurrentGuildPermissions(const GuildMember& guildMember) {
		String permissions = Permissions::computeBasePermissions(guildMember);
		return permissions;
	}

	Void Permissions::removePermissions(const Vector<Permission>& permissionsToRemove) {
		Uint64 permissionsInteger = this->thePermissions;
		for (auto value: permissionsToRemove) {
			permissionsInteger &= ~static_cast<Uint64>(value);
		}
		StringStream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	Void Permissions::addPermissions(const Vector<Permission>& permissionsToAdd) {
		Uint64 permissionsInteger = this->thePermissions;
		for (auto value: permissionsToAdd) {
			permissionsInteger |= static_cast<Uint64>(value);
		}
		StringStream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	Vector<String> Permissions::displayPermissions() {
		Vector<String> returnVector{};
		Uint64 permissionsInteger = this->thePermissions;
		if (permissionsInteger & (1ll << 3)) {
			for (Uint64 x = 0; x < 41; ++x) {
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

	String Permissions::getCurrentPermissionString() {
		String theReturnString = std::to_string(this->thePermissions);
		return theReturnString;
	}

	String Permissions::getAllPermissions() {
		Uint64 allPerms{ 0 };
		for (Uint64 x = 0; x < 41; ++x) {
			allPerms |= 1ll << x;
		}
		StringStream stream{};
		stream << allPerms;
		return stream.str();
	}

	String Permissions::computeOverwrites(const String& basePermissions, const GuildMember& guildMember, ChannelData& channel) {
		if ((stoull(basePermissions) & static_cast<Uint64>(Permission::Administrator)) == static_cast<Uint64>(Permission::Administrator)) {
			return Permissions::getAllPermissions();
		}

		Uint64 permissions = stoull(basePermissions);
		for (Int32 x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites[x].id == guildMember.guildId) {
				permissions &= ~channel.permissionOverwrites[x].deny;
				permissions |= channel.permissionOverwrites[x].allow;
				break;
			}
		}
		Vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(Roles::getCachedRoleAsync({ .guildId = guildMember.guildId, .roleId = value }).get());
		}
		Uint64 allow{ 0 };
		Uint64 deny{ 0 };
		for (auto& value: guildMemberRoles) {
			for (Int32 x = 0; x < channel.permissionOverwrites.size(); ++x) {
				if (value.id == channel.permissionOverwrites[x].id) {
					allow |= channel.permissionOverwrites[x].allow;
					deny |= channel.permissionOverwrites[x].deny;
				}
			}
		}
		permissions &= ~deny;
		permissions |= allow;
		for (Int32 x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites[x].id == guildMember.id) {
				permissions &= ~channel.permissionOverwrites[x].deny;
				permissions |= channel.permissionOverwrites[x].allow;
				break;
			}
		}
		return std::to_string(permissions);
	}

	String Permissions::computePermissions(const GuildMember& guildMember, ChannelData& channel) {
		String permissions = Permissions::computeBasePermissions(guildMember);
		permissions = Permissions::computeOverwrites(permissions, guildMember, channel);
		return permissions;
	}

	String Permissions::computeBasePermissions(const GuildMember& guildMember) {
		const GuildData guild = Guilds::getCachedGuildAsync({ .guildId = guildMember.guildId }).get();
		if (guild.ownerId == guildMember.id) {
			return Permissions::getAllPermissions();
		}
		Vector<RoleData> guildRoles{};
		for (auto& value: guild.roles) {
			guildRoles.emplace_back(Roles::getCachedRoleAsync({ .guildId = guild.id, .roleId = value }).get());
		}
		RoleData roleEveryone{};
		for (auto& value: guildRoles) {
			if (value.id == guild.id) {
				roleEveryone = value;
			}
		}
		Uint64 permissions{};
		if (roleEveryone.permissions != 0) {
			permissions = roleEveryone.permissions;
		}
		GetGuildMemberRolesData getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId = guildMember.guildId;
		Vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(Roles::getCachedRoleAsync({ .guildId = guild.id, .roleId = value }).get());
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions;
		}

		if ((permissions & static_cast<Uint64>(Permission::Administrator)) == static_cast<Uint64>(Permission::Administrator)) {
			return Permissions::getAllPermissions();
		}

		return std::to_string(permissions);
	}

	Void reportException(const String& currentFunctionName, std::source_location theLocation) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			StringStream theStream{};
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

	Void rethrowException(const String& currentFunctionName, std::source_location theLocation) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			StringStream theStream{};
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

	String constructMultiPartData(String theData, const Vector<File>& files) {
		const String boundary("boundary25");
		const String partStart("--" + boundary + "\r\nContent-Type: application/octet-stream\r\nContent-Disposition: form-data; ");

		String content("--" + boundary);

		content += "\r\nContent-Type: application/json\r\nContent-Disposition: form-data; "
				   "name=\"payload_json\"\r\n\r\n";
		content += theData + "\r\n";
		if (files.size() == 1) {
			content += partStart + "name=\"file\"; filename=\"" + files[0].fileName + "\"" + "\r\n\r\n";
			content += files[0].data;
		} else {
			for (Uint8 x = 0; x < files.size(); ++x) {
				content += partStart + "name=\"files[" + std::to_string(x) + "]\"; filename=\"" + files[x].fileName + "\"\r\n\r\n";
				content += files[x].data;
				content += "\r\n";
			}
		}
		content += "\r\n--" + boundary + "--";
		return content;
	}

	String convertToLowerCase(const String& stringToConvert) {
		String newString;
		for (auto& value: stringToConvert) {
			if (isupper(static_cast<Uint8>(value))) {
				newString += static_cast<char>(tolower(static_cast<Uint8>(value)));
			} else {
				newString += value;
			}
		}
		return newString;
	}

	String base64Encode(const String& theString, Bool url) {
		const char* base64CharsArray[2] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"abcdefghijklmnopqrstuvwxyz"
											"0123456789"
											"+/",

			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789"
			"-_" };

		Uint64 encodedLength = (theString.size() + 2) / 3 * 4;

		Uint8 trailing_char = url ? '.' : '=';

		const char* base64Chars = base64CharsArray[url];

		String theReturnString{};
		theReturnString.reserve(encodedLength);
		DiscordCoreAPI::StopWatch theStopWatch{ 1500ms };
		Uint64 pos = 0;
		while (pos < theString.size()) {
			if (theStopWatch.hasTimePassed()) {
				break;
			}
			theReturnString.push_back(base64Chars[(theString[static_cast<Uint64>(pos + 0)] & 0xfc) >> 2]);

			if (static_cast<Uint64>(pos + 1) < theString.size()) {
				theReturnString.push_back(base64Chars[((theString[static_cast<Int64>(pos + 0)] & 0x03) << 4) + ((theString[static_cast<Int64>(pos + 1)] & 0xf0) >> 4)]);

				if (static_cast<Uint64>(pos + 2) < theString.size()) {
					theReturnString.push_back(base64Chars[((theString[static_cast<Int64>(pos + 1)] & 0x0f) << 2) + ((theString[static_cast<Int64>(pos + 2)] & 0xc0) >> 6)]);
					theReturnString.push_back(base64Chars[theString[static_cast<Int64>(pos + 2)] & 0x3f]);
				} else {
					theReturnString.push_back(base64Chars[(theString[static_cast<Int64>(pos + 1)] & 0x0f) << 2]);
					theReturnString.push_back(trailing_char);
				}
			} else {
				theReturnString.push_back(base64Chars[(theString[static_cast<Int64>(pos + 0)] & 0x03) << 4]);
				theReturnString.push_back(trailing_char);
				theReturnString.push_back(trailing_char);
			}

			pos += 3;
		}

		return theReturnString;
	}

	String loadFileContents(const String& filePath) {
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		std::ostringstream stream{};
		stream << file.rdbuf();
		return stream.str();
	}

	String utf8MakeValid(const String& inputString) {
		String theReturnString{};
		for (auto& value: inputString) {
			if (value >= 128 || value < 0) {
				Int32 theDifference = 0 - value;
				theReturnString.push_back(value + theDifference);
			} else {
				theReturnString.push_back(value);
			}
		}
		return theReturnString;
	}

	String urlEncode(const String& inputString) {
		std::ostringstream escaped{};
		escaped.fill('0');
		escaped << std::hex;

		for (String::const_iterator i = inputString.begin(), n = inputString.end(); i != n; ++i) {
			String::value_type c = (*i);

			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << Int32(static_cast<Uint8>(c));
			escaped << std::nouppercase;
		}
		return escaped.str();
	}

	Void spinLock(Uint64 timeInNsToSpinLockFor) {
		Uint64 startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		Uint64 timePassed{ 0 };
		while (timePassed < timeInNsToSpinLockFor) {
			timePassed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - startTime;
		}
	}

	String generateBase64EncodedKey() {
		String theReturnString{};
		theReturnString.resize(16);
		std::mt19937_64 randomEngine{ static_cast<Uint64>(std::chrono::steady_clock::now().time_since_epoch().count()) };
		for (Uint32 x = 0; x < 16; ++x) {
			theReturnString[x] = static_cast<Uint8>((static_cast<Float>(randomEngine()) / static_cast<Float>(randomEngine.max())) * 255.0f);
		}
		theReturnString = base64Encode(theReturnString, false);
		return theReturnString;
	}

	String shiftToBrightGreen() {
		return String("\033[1;40;92m");
	}

	String shiftToBrightBlue() {
		return String("\033[1;40;96m");
	}

	String shiftToBrightRed() {
		return String("\033[1;40;91m");
	}

	Bool nanoSleep(Int64 ns) {
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

	String reset() {
		return String("\033[0m");
	}

	String getTimeAndDate() {
		const time_t now = std::time(nullptr);
		tm time = *std::localtime(&now);
		String timeStamp{};
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
		Uint64 size = strftime(timeStamp.data(), 48, "%F %R", &time);
		timeStamp.resize(size);
		return timeStamp;
	}

	String escapeCharacters(StringView theString) {
		String theStringNew{};
		if (theStringNew.size() <= theString.size() * 2) {
			theStringNew.resize(theString.size() * 2);
		}
		Uint64 theIndex{};
		for (Uint32 x = 0; x < theString.size(); ++x) {
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

	template<typename Object> UMap<String, UnboundedMessageBlock<Object>*> ObjectCollector<Object>::objectsBufferMap{};
};

namespace DiscordCoreInternal {

	StringBuffer::StringBuffer() noexcept {
		this->theString01.resize(1024 * 16);
	}

	StringView StringBuffer::operator[](LengthData size) {
		StringView theString{ this->theString01.data() + size.offSet, size.length };
		return theString;
	}

	char StringBuffer::operator[](Uint64 theIndex) {
		return this->theString01[theIndex];
	}

	Void StringBuffer::writeData(const char* thePtr, Uint64 theSize) {
		if (this->theSize + theSize > this->theString01.size()) {
			this->theString01.resize(this->theString01.size() + theSize);
		}
		memcpy(this->theString01.data() + this->theSize, thePtr, theSize);
		this->theSize += theSize;
	}

	String::iterator StringBuffer::begin() {
		return this->theString01.begin();
	}

	String::iterator StringBuffer::end() {
		return this->theString01.end();
	}

	Void StringBuffer::erase(Uint64 amount) {
		this->theSize = this->theSize - amount;
		memcpy(this->theString01.data(), this->theString01.data() + amount, this->theSize);
	}

	Uint64 StringBuffer::size() {
		return this->theSize;
	}

	Void StringBuffer::clear() {
		this->theSize = 0;
	}

	char* StringBuffer::data() {
		return this->theString01.data();
	}
}