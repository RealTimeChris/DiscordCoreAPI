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

	WebSocketClose& WebSocketClose::operator=(uint16_t valueNew) {
		this->value = this->mappingValues[valueNew];
		return *this;
	};

	WebSocketClose::WebSocketClose(uint16_t valueNew) {
		*this = valueNew;
	};

	WebSocketClose::operator std::string() {
		return this->outputErrorValues[this->mappingValues[static_cast<uint16_t>(*this)]];
	}

	WebSocketClose::operator uint16_t() {
		for (auto& [key, value]: this->mappingValues) {
			if (value == value) {
				return key;
			}
		}
		return uint16_t{};
	}

	WebSocketClose::operator bool() {
		return static_cast<std::underlying_type_t<decltype(this->value)>>(this->value) &
			static_cast<std::underlying_type_t<decltype(WebSocketCloseCode::We_Do_Reconnect)>>(WebSocketCloseCode::We_Do_Reconnect);
	}

	VoiceWebSocketClose& VoiceWebSocketClose::operator=(uint16_t valueNew) {
		this->value = this->mappingValues[valueNew];
		return *this;
	};

	VoiceWebSocketClose::VoiceWebSocketClose(uint16_t valueNew) {
		*this = valueNew;
	};

	VoiceWebSocketClose::operator std::string() {
		return this->outputErrorValues[this->mappingValues[static_cast<uint16_t>(*this)]];
	}

	VoiceWebSocketClose::operator uint16_t() {
		for (auto& [key, value]: this->mappingValues) {
			if (value == value) {
				return key;
			}
		}
		return uint16_t{};
	}

	VoiceWebSocketClose::operator bool() {
		return true;
	}

	HttpsResponseCode& HttpsResponseCode::operator=(uint32_t valueNew) {
		this->value = static_cast<HttpsResponseCodes>(valueNew);
		return *this;
	}

	HttpsResponseCode::HttpsResponseCode(uint32_t valueNew) {
		*this = valueNew;
	}

	HttpsResponseCode::operator std::string() {
		std::string response{};
		response += "Code: " + std::to_string(static_cast<uint32_t>(this->value)) + ", Message: " + this->outputErrorValues[this->value];
		return response;
	}

	HttpsResponseCode::operator uint32_t() {
		return static_cast<uint32_t>(this->value);
	}
}

namespace DiscordCoreAPI {

	Snowflake& Snowflake::operator=(const std::string& other) noexcept {
		for (auto& value: other) {
			if (!std::isdigit(static_cast<uint8_t>(value))) {
				return *this;
			}
		}
		if (other.size() == 0) {
			return *this;
		}
		this->id = stoull(other);
		return *this;
	}

	Snowflake::Snowflake(const std::string& other) noexcept {
		*this = other;
	}

	Snowflake& Snowflake::operator=(const uint64_t other) noexcept {
		this->id = other;
		return *this;
	}

	Snowflake::Snowflake(const uint64_t other) noexcept {
		*this = other;
	}

	Snowflake::operator std::string() const noexcept {
		return std::to_string(this->id);
	}

	Snowflake::operator uint64_t() const {
		return this->id;
	}

	Snowflake::operator std::string() noexcept {
		return std::to_string(this->id);
	}

	Snowflake::operator uint64_t() {
		return this->id;
	}

	EnumConverter::operator std::vector<uint64_t>() const noexcept {
		return this->vector;
	}

	EnumConverter::operator uint64_t() const noexcept {
		return this->integer;
	}

	bool EnumConverter::isItAVector() const noexcept {
		return this->vectorType;
	}

	Jsonifier& Jsonifier::operator=(Jsonifier&& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = std::move(*data.jsonValue.object);
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = std::move(*data.jsonValue.array);
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = std::move(*data.jsonValue.string);
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->string = std::move(data.string);
		this->type = data.type;
		return *this;
	}

	Jsonifier::Jsonifier(Jsonifier&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const Jsonifier& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = *data.jsonValue.object;
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = *data.jsonValue.array;
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = *data.jsonValue.string;
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->string = data.string;
		this->type = data.type;
		return *this;
	}

	Jsonifier::Jsonifier(const Jsonifier& data) noexcept {
		*this = data;
	}

	Jsonifier::operator std::string&&() noexcept {
		return std::move(this->string);
	}

	Jsonifier::operator std::string() noexcept {
		return this->string;
	}

	JsonType Jsonifier::getType() noexcept {
		return this->type;
	}

	void Jsonifier::refreshString(JsonifierSerializeType opCode) {
		this->string.clear();
		if (opCode == JsonifierSerializeType::Etf) {
			this->appendVersion();
			this->serializeJsonToEtfString(this);
		} else {
			this->serializeJsonToJsonString(this);
		}
	}

	Jsonifier& Jsonifier::operator=(EnumConverter&& data) noexcept {
		if (data.isItAVector()) {
			this->setValue(JsonType::Array);
			for (auto& value: data.operator std::vector<uint64_t>()) {
				this->jsonValue.array->emplace_back(std::move(value));
			}
		} else {
			this->jsonValue.numberUint = uint64_t{ data };
			this->type = JsonType::Uint64;
		}
		return *this;
	}

	Jsonifier::Jsonifier(EnumConverter&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const EnumConverter& data) noexcept {
		if (data.isItAVector()) {
			this->setValue(JsonType::Array);
			for (auto& value: data.operator std::vector<uint64_t>()) {
				this->jsonValue.array->emplace_back(value);
			}
		} else {
			this->jsonValue.numberUint = uint64_t{ data };
			this->type = JsonType::Uint64;
		}
		return *this;
	}

	Jsonifier::Jsonifier(const EnumConverter& data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(std::string&& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = std::move(data);
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(std::string&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const std::string& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(const std::string& data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(const char* data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(const char* data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(double data) noexcept {
		this->jsonValue.numberDouble = data;
		this->type = JsonType::Float;
		return *this;
	}

	Jsonifier::Jsonifier(double data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(float data) noexcept {
		this->jsonValue.numberDouble = data;
		this->type = JsonType::Float;
		return *this;
	}

	Jsonifier::Jsonifier(float data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint64_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint64_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint32_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint32_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint16_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint16_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint8_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint8_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int64_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int64_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int32_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int32_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int16_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int16_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int8_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int8_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(std::nullptr_t) noexcept {
		this->type = JsonType::Null;
		return *this;
	}

	Jsonifier::Jsonifier(std::nullptr_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(bool data) noexcept {
		this->jsonValue.boolean = data;
		this->type = JsonType::Bool;
		return *this;
	}

	Jsonifier::Jsonifier(bool data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(JsonType typeNew) noexcept {
		this->type = typeNew;
		this->setValue(this->type);
		return *this;
	}

	Jsonifier::Jsonifier(JsonType type) noexcept {
		*this = type;
	}

	Jsonifier& Jsonifier::operator[](typename ObjectType::key_type key) {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Object);
			this->type = JsonType::Object;
		}

		if (this->type == JsonType::Object) {
			auto result = this->jsonValue.object->emplace(std::move(key), Jsonifier{});
			return result.first->second;
		}
		throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
	}

	Jsonifier& Jsonifier::operator[](uint64_t index) {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			if (index >= this->jsonValue.array->size()) {
				this->jsonValue.array->resize(index + 1);
			}

			return this->jsonValue.array->operator[](index);
		}
		throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
	}

	void Jsonifier::emplaceBack(Jsonifier&& other) noexcept {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			this->jsonValue.array->emplace_back(std::move(other));
		}
	}

	void Jsonifier::emplaceBack(Jsonifier& other) noexcept {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			this->jsonValue.array->emplace_back(other);
		}
	}

	void Jsonifier::serializeJsonToEtfString(const Jsonifier* dataToParse) {
		switch (dataToParse->type) {
			case JsonType::Object: {
				return this->writeEtfObject(*dataToParse->jsonValue.object);
			}
			case JsonType::Array: {
				return this->writeEtfArray(*dataToParse->jsonValue.array);
			}
			case JsonType::String: {
				return this->writeEtfString(*dataToParse->jsonValue.string);
			}
			case JsonType::Float: {
				return this->writeEtfFloat(dataToParse->jsonValue.numberDouble);
			}
			case JsonType::Uint64: {
				return this->writeEtfUint(dataToParse->jsonValue.numberUint);
			}
			case JsonType::Int64: {
				return this->writeEtfInt(dataToParse->jsonValue.numberInt);
			}
			case JsonType::Bool: {
				return this->writeEtfBool(dataToParse->jsonValue.boolean);
			}
			case JsonType::Null: {
				return this->writeEtfNull();
			}
		}
	}

	void Jsonifier::serializeJsonToJsonString(const Jsonifier* dataToParse) {
		switch (dataToParse->type) {
			case JsonType::Object: {
				return this->writeJsonObject(*dataToParse->jsonValue.object);
			}
			case JsonType::Array: {
				return this->writeJsonArray(*dataToParse->jsonValue.array);
			}
			case JsonType::String: {
				return this->writeJsonString(*dataToParse->jsonValue.string);
			}
			case JsonType::Float: {
				return this->writeJsonFloat(dataToParse->jsonValue.numberDouble);
			}
			case JsonType::Uint64: {
				return this->writeJsonInt(dataToParse->jsonValue.numberUint);
			}
			case JsonType::Int64: {
				return this->writeJsonInt(dataToParse->jsonValue.numberInt);
			}
			case JsonType::Bool: {
				return this->writeJsonBool(dataToParse->jsonValue.boolean);
			}
			case JsonType::Null: {
				return this->writeJsonNull();
			}
		}
	}

	void Jsonifier::writeJsonObject(const ObjectType& objectNew) {
		if (objectNew.empty()) {
			this->writeString("{}", 2);
			return;
		}
		this->writeCharacter('{');

		int32_t index{};
		for (auto& [key, value]: objectNew) {
			this->writeJsonString(key);
			this->writeCharacter(':');
			this->serializeJsonToJsonString(&value);

			if (index != objectNew.size() - 1) {
				this->writeCharacter(',');
			}
			index++;
		}

		this->writeCharacter('}');
	}

	void Jsonifier::writeJsonArray(const ArrayType& arrayNew) {
		if (arrayNew.empty()) {
			this->writeString("[]", 2);
			return;
		}

		this->writeCharacter('[');

		int32_t index{};
		for (auto& value: arrayNew) {
			this->serializeJsonToJsonString(&value);
			if (index != arrayNew.size() - 1) {
				this->writeCharacter(',');
			}
			index++;
		}

		this->writeCharacter(']');
	}

	void Jsonifier::writeJsonString(const StringType& stringNew) {
		this->writeCharacter('\"');
		for (auto& value: stringNew) {
			switch (static_cast<std::uint8_t>(value)) {
				case 0x08: {
					this->writeCharacter('b');
					break;
				}
				case 0x09: {
					this->writeCharacter('t');
					break;
				}
				case 0x0A: {
					this->writeCharacter('n');
					break;
				}
				case 0x0C: {
					this->writeCharacter('f');
					break;
				}
				case 0x0D: {
					this->writeCharacter('r');
					break;
				}
				case 0x22: {
					this->writeCharacter('\"');
					break;
				}
				case 0x5C: {
					this->writeCharacter('\\');
					break;
				}
				default: {
					this->writeCharacter(value);
					break;
				}
			}
		}
		this->writeCharacter('\"');
	}

	void Jsonifier::writeJsonFloat(const FloatType x) {
		auto floatValue = std::to_string(x);
		this->writeString(floatValue.data(), floatValue.size());
	}

	void Jsonifier::writeJsonBool(const BoolType jsonValueNew) {
		if (jsonValueNew) {
			this->writeString("true", 4);
		} else {
			this->writeString("false", 5);
		}
	}

	void Jsonifier::writeJsonNull() {
		this->writeString("null", 4);
	}

	void Jsonifier::writeEtfObject(const ObjectType& jsonData) {
		this->appendMapHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& [key, value]: jsonData) {
			this->appendBinaryExt(key, static_cast<uint32_t>(key.size()));
			this->serializeJsonToEtfString(&value);
		}
	}

	void Jsonifier::writeEtfArray(const ArrayType& jsonData) {
		this->appendListHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& value: jsonData) {
			this->serializeJsonToEtfString(&value);
		}
		this->appendNilExt();
	}

	void Jsonifier::writeEtfString(const StringType& jsonData) {
		this->appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
	}

	void Jsonifier::writeEtfUint(const UintType jsonData) {
		if (jsonData <= 255) {
			this->appendSmallIntegerExt(static_cast<uint8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<uint32_t>::max()) {
			this->appendIntegerExt(static_cast<uint32_t>(jsonData));
		} else {
			this->appendUnsignedLongLong(jsonData);
		}
	}

	void Jsonifier::writeEtfInt(const IntType jsonData) {
		if (jsonData <= 127 && jsonData >= -127) {
			this->appendSmallIntegerExt(static_cast<uint8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
			this->appendIntegerExt(static_cast<uint32_t>(jsonData));
		} else {
			this->appendUnsignedLongLong(static_cast<uint64_t>(jsonData));
		}
	}

	void Jsonifier::writeEtfFloat(const FloatType jsonData) {
		this->appendNewFloatExt(jsonData);
	}

	void Jsonifier::writeEtfBool(const BoolType jsonData) {
		this->appendBool(jsonData);
	}

	void Jsonifier::writeEtfNull() {
		this->appendNil();
	}

	void Jsonifier::writeString(const char* data, std::size_t length) {
		this->string.append(data, length);
	}

	void Jsonifier::writeCharacter(const char charValue) {
		this->string.push_back(charValue);
	}

	bool operator==(const Jsonifier& lhs, const Jsonifier& rhs) {
		if (lhs.type != rhs.type) {
			return false;
		}
		switch (rhs.type) {
			case JsonType::Object: {
				if (*lhs.jsonValue.object != *rhs.jsonValue.object) {
					return false;
				}
				break;
			}
			case JsonType::Array: {
				if (*lhs.jsonValue.array != *rhs.jsonValue.array) {
					return false;
				}
				break;
			}
			case JsonType::String: {
				if (*lhs.jsonValue.string != *rhs.jsonValue.string) {
					return false;
				}
				break;
			}
			case JsonType::Float: {
				if (lhs.jsonValue.numberDouble != rhs.jsonValue.numberDouble) {
					return false;
				}
				break;
			}
			case JsonType::Uint64: {
				if (lhs.jsonValue.numberUint != rhs.jsonValue.numberUint) {
					return false;
				}
				break;
			}
			case JsonType::Int64: {
				if (lhs.jsonValue.numberInt != rhs.jsonValue.numberInt) {
					return false;
				}
				break;
			}
			case JsonType::Bool: {
				if (lhs.jsonValue.boolean != rhs.jsonValue.boolean) {
					return false;
				}
				break;
			}
		}
		return true;
	}

	void Jsonifier::appendBinaryExt(const std::string& bytes, uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Binary_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
		this->writeString(bytes.data(), bytes.size());
	}

	void Jsonifier::appendUnsignedLongLong(uint64_t value) {
		char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
		char encodedBytes{};
		while (value > 0) {
			newBuffer[3 + encodedBytes] = value & 0xFF;
			value >>= 8;
			encodedBytes++;
		}
		newBuffer[1] = encodedBytes;
		newBuffer[2] = 0;
		this->writeString(newBuffer, 1 + 2 + encodedBytes);
	}

	void Jsonifier::appendNewFloatExt(const double FloatValue) {
		char newBuffer[9]{ static_cast<unsigned char>(EtfType::New_Float_Ext) };
		const void* punner{ &FloatValue };
		storeBits(newBuffer + 1, *static_cast<const uint64_t*>(punner));
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendSmallIntegerExt(const uint8_t value) {
		char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendIntegerExt(const uint32_t value) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
		storeBits(newBuffer + 1, value);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendListHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::List_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendMapHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Map_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendBool(bool data) {
		if (data) {
			char newBuffer[6]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
			this->writeString(newBuffer, std::size(newBuffer));

		} else {
			char newBuffer[7]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
			this->writeString(newBuffer, std::size(newBuffer));
		}
	}

	void Jsonifier::appendVersion() {
		char newBuffer[1]{ static_cast<int8_t>(formatVersion) };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendNilExt() {
		this->writeCharacter(static_cast<uint8_t>(EtfType::Nil_Ext));
	}

	void Jsonifier::appendNil() {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::setValue(JsonType typeNew) {
		this->destroy();
		this->type = typeNew;
		switch (this->type) {
			case JsonType::Object: {
				AllocatorType<ObjectType> allocator{};
				this->jsonValue.object = AllocatorTraits<ObjectType>::allocate(allocator, 1);
				AllocatorTraits<ObjectType>::construct(allocator, this->jsonValue.object);
				break;
			}
			case JsonType::Array: {
				AllocatorType<ArrayType> allocator{};
				this->jsonValue.array = AllocatorTraits<ArrayType>::allocate(allocator, 1);
				AllocatorTraits<ArrayType>::construct(allocator, this->jsonValue.array);
				break;
			}
			case JsonType::String: {
				AllocatorType<StringType> allocator{};
				this->jsonValue.string = AllocatorTraits<StringType>::allocate(allocator, 1);
				AllocatorTraits<StringType>::construct(allocator, this->jsonValue.string);
				break;
			}
		}
	}

	void Jsonifier::destroy() noexcept {
		switch (this->type) {
			case JsonType::Object: {
				AllocatorType<ObjectType> allocator{};
				AllocatorTraits<ObjectType>::destroy(allocator, this->jsonValue.object);
				AllocatorTraits<ObjectType>::deallocate(allocator, this->jsonValue.object, 1);
				break;
			}
			case JsonType::Array: {
				AllocatorType<ArrayType> allocator{};
				AllocatorTraits<ArrayType>::destroy(allocator, this->jsonValue.array);
				AllocatorTraits<ArrayType>::deallocate(allocator, this->jsonValue.array, 1);
				break;
			}
			case JsonType::String: {
				AllocatorType<StringType> allocator{};
				AllocatorTraits<StringType>::destroy(allocator, this->jsonValue.string);
				AllocatorTraits<StringType>::deallocate(allocator, this->jsonValue.string, 1);
				break;
			}
		}
	}

	Jsonifier::~Jsonifier() noexcept {
		this->destroy();
	}

	void reportException(const std::string& stackTrace, UnboundedMessageBlock<std::exception>* sendBuffer, bool rethrow) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (std::exception& e) {
			if (rethrow) {
				std::rethrow_exception(std::current_exception());
				return;
			}
			if (sendBuffer) {
				sendBuffer->send(e);
			} else {
				if (stackTrace.back() == '\n') {
					cout << shiftToBrightRed() << stackTrace + "Error: " << e.what() << reset() << std::endl << std::endl;
				} else {
					cout << shiftToBrightRed() << stackTrace + " Error: " << e.what() << reset() << std::endl << std::endl;
				}
			}
		}
	}

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*function)( void )) {
		outputSttream << function();
		return outputSttream;
	}

	ConfigManager::ConfigManager(const DiscordCoreClientConfig& configNew) {
		this->config = configNew;
	}

	const bool ConfigManager::doWePrintWebSocketSuccessMessages() const {
		return this->config.logOptions.logWebSocketSuccessMessages;
	}

	const bool ConfigManager::doWePrintWebSocketErrorMessages() const {
		return this->config.logOptions.logWebSocketErrorMessages;
	}

	const bool ConfigManager::doWePrintHttpsSuccessMessages() const {
		return this->config.logOptions.logHttpsSuccessMessages;
	}

	const bool ConfigManager::doWePrintHttpsErrorMessages() const {
		return this->config.logOptions.logHttpsErrorMessages;
	}

	const bool ConfigManager::doWePrintFFMPEGSuccessMessages() const {
		return this->config.logOptions.logFFMPEGSuccessMessages;
	}

	const bool ConfigManager::doWePrintFFMPEGErrorMessages() const {
		return this->config.logOptions.logFFMPEGErrorMessages;
	}

	const bool ConfigManager::doWePrintGeneralSuccessMessages() const {
		return this->config.logOptions.logGeneralSuccessMessages;
	}

	const bool ConfigManager::doWePrintGeneralErrorMessages() const {
		return this->config.logOptions.logGeneralErrorMessages;
	}

	const bool ConfigManager::doWeCacheChannels() const {
		return this->config.cacheOptions.cacheChannels;
	}

	const bool ConfigManager::doWeCacheUsers() const {
		return this->config.cacheOptions.cacheUsers;
	}

	const bool ConfigManager::doWeCacheGuilds() const {
		return this->config.cacheOptions.cacheGuilds;
	}

	const bool ConfigManager::doWeCacheRoles() const {
		return this->config.cacheOptions.cacheRoles;
	}

	const UpdatePresenceData ConfigManager::getPresenceData() const {
		return this->config.presenceData;
	}

	const std::string ConfigManager::getBotToken() const {
		return this->config.botToken;
	}

	const uint32_t ConfigManager::getTotalShardCount() const {
		return this->config.shardOptions.totalNumberOfShards;
	}

	const uint32_t ConfigManager::getStartingShard() const {
		return this->config.shardOptions.startingShard;
	}

	const uint32_t ConfigManager::getShardCountForThisProcess() const {
		return this->config.shardOptions.numberOfShardsForThisProcess;
	}

	const std::string ConfigManager::getConnectionAddress() const {
		return this->config.connectionAddress;
	}

	void ConfigManager::setConnectionAddress(const std::string& connectionAddressNew) {
		this->config.connectionAddress = connectionAddressNew;
	}

	const std::string ConfigManager::getConnectionPort() const {
		return this->config.connectionPort;
	}

	void ConfigManager::setConnectionPort(const std::string& connectionPortNew) {
		this->config.connectionPort = connectionPortNew;
	}

	const std::vector<RepeatedFunctionData> ConfigManager::getFunctionsToExecute() const {
		return this->config.functionsToExecute;
	}

	const TextFormat ConfigManager::getTextFormat() const {
		return this->config.textFormat;
	}

	const GatewayIntents ConfigManager::getGatewayIntents() {
		return this->config.intents;
	}

	StringWrapper& StringWrapper::operator=(StringWrapper&& other) noexcept {
		if (this != &other) {
			this->ptr.reset(nullptr);
			this->ptr = std::move(other.ptr);
			other.ptr.reset(nullptr);
			other.ptr = nullptr;
		}
		return *this;
	}

	StringWrapper::StringWrapper(StringWrapper&& other) noexcept {
		*this = std::move(other);
	}

	StringWrapper& StringWrapper::operator=(const StringWrapper& other) {
		if (this != &other) {
			this->ptr.reset(nullptr);
			std::stringstream stream{};
			if (other.ptr) {
				stream << other.ptr;
			}
			auto length = stream.str().size();
			this->ptr = std::make_unique<char[]>(length + 1);
			for (uint64_t x = 0; x < length; ++x) {
				this->ptr[x] = other.ptr[x];
			}
		}
		return *this;
	}

	StringWrapper::StringWrapper(const StringWrapper& other) {
		*this = other;
	}

	StringWrapper& StringWrapper::operator=(const std::string& string) {
		auto length = string.size();
		this->ptr.reset(nullptr);
		this->ptr = std::make_unique<char[]>(length + 1);
		for (int32_t x = 0; x < length; ++x) {
			this->ptr[x] = string[x];
		}
		return *this;
	}

	StringWrapper::StringWrapper(const std::string& string) {
		*this = string;
	}

	StringWrapper& StringWrapper::operator=(const char* string) {
		if (string) {
			this->ptr.reset(nullptr);
			std::stringstream stream{};
			stream << string;
			int64_t length = stream.str().size();
			this->ptr = std::make_unique<char[]>(length + 1);
			for (int64_t x = 0; x < length; ++x) {
				this->ptr[x] = string[x];
			}
		}
		return *this;
	}

	StringWrapper::StringWrapper(const char* string) {
		*this = string;
	}

	StringWrapper::operator std::string() {
		std::stringstream stream{};
		if (this->ptr) {
			stream << this->ptr;
		}
		std::string string{};
		for (uint32_t x = 0; x < stream.str().size(); ++x) {
			string.push_back(stream.str()[x]);
		}
		return string;
	}

	void StringWrapper::emplace_back(char value) {
		std::stringstream stream{};
		if (this->ptr) {
			stream << this->ptr;
		}
		auto length = stream.str().size();
		this->ptr = std::make_unique<char[]>(length + 2);
		for (uint64_t x = 0; x < length; ++x) {
			this->ptr[x] = stream.str()[x];
		}
		this->ptr[length] = value;
	}

	uint64_t StringWrapper::size() {
		std::stringstream stream{};
		if (this->ptr) {
			stream << this->ptr;
		}
		auto length = stream.str().size();
		return length;
	}

	const char* StringWrapper::data() {
		return this->ptr.get();
	}

	ColorValue::ColorValue(uint32_t colorValue) {
		this->color = colorValue;
	}

	ColorValue::ColorValue(std::string hexColorValue) {
		if (hexColorValue == "") {
			hexColorValue = "fefefe";
		}
		this->color = stoull(hexColorValue, nullptr, 16);
	}

	RGBColorValue ColorValue::getRgbColorValue() {
		uint8_t red = static_cast<uint8_t>(this->color >> 16);
		uint8_t green = static_cast<uint8_t>(this->color >> 8);
		uint8_t blue = static_cast<uint8_t>(this->color);
		RGBColorValue color{};
		color.green = green;
		color.blue = blue;
		color.red = red;
		return color;
	}

	HexColorValue ColorValue::getHexColorValue() {
		std::stringstream stream{};
		stream << std::hex << this->color;
		return stream.str();
	}

	uint32_t ColorValue::getIntColorValue() {
		return this->color;
	}

	IconHash& IconHash::operator=(const std::string& string) {
		std::string newHash{ string };
		if (newHash.empty() || newHash == "0") {
			this->highBits = 0;
			this->lowBits = 0;
			return *this;
		}
		if (newHash.length() == 34 && newHash.substr(0, 2) == "a_") {
			newHash = newHash.substr(2);
		}
		if (newHash.length() != 32 && newHash.length() != 33) {
			throw std::length_error(
				"IconHash must be exactly 32 characters in length, passed value is: '" + std::to_string(newHash.size()) + "', in length.");
		}
		this->lowBits = fromString<uint64_t>(newHash.substr(0, 16), std::hex);
		this->highBits = fromString<uint64_t>(newHash.substr(16, 16), std::hex);
		return *this;
	}

	IconHash::IconHash(const std::string& string) noexcept {
		*this = string;
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

	uint64_t strtoull(const std::string_view string) {
		for (auto& value: string) {
			if (!isdigit(value)) {
				return 0;
			}
		}
		if (!string.empty() && string != "") {
			return stoull(std::string{ string });
		} else {
			return 0;
		}
	}

	Permissions& Permissions::operator=(Permission&& other) {
		this->permissions = static_cast<uint64_t>(other);
		return *this;
	}

	Permissions::Permissions(Permission&& permsNew) {
		*this = std::move(permsNew);
	}

	Permissions& Permissions::operator=(const Permission& other) {
		this->permissions = static_cast<uint64_t>(other);
		return *this;
	}

	Permissions::Permissions(const Permission& permsNew) {
		*this = permsNew;
	}

	Permissions& Permissions::operator=(std::string&& other) {
		if (other.size() == 0 || other == "") {
			this->permissions = 0;
		} else {
			for (auto& value: other) {
				this->permissions = stoull(other);
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
			this->permissions = 0;
		} else {
			this->permissions = stoull(other);
		}
		return *this;
	}

	Permissions::Permissions(const std::string& permsNew) {
		*this = permsNew;
	}


	Permissions& Permissions::operator=(uint64_t other) {
		this->permissions = other;
		return *this;
	}

	Permissions::Permissions(uint64_t permsNew) {
		*this = permsNew;
	}

	Permissions::operator uint64_t() {
		return this->permissions;
	}

	Permissions::operator std::string() {
		return std::string{ std::to_string(this->permissions) };
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
		uint64_t permissionsInteger = this->permissions;
		for (auto value: permissionsToRemove) {
			permissionsInteger &= ~static_cast<uint64_t>(value);
		}
		std::stringstream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	void Permissions::addPermissions(const std::vector<Permission>& permissionsToAdd) {
		uint64_t permissionsInteger = this->permissions;
		for (auto value: permissionsToAdd) {
			permissionsInteger |= static_cast<uint64_t>(value);
		}
		std::stringstream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	std::vector<std::string> Permissions::displayPermissions() {
		std::vector<std::string> returnVector{};
		uint64_t permissionsInteger = this->permissions;
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
		std::string returnString = std::to_string(this->permissions);
		return returnString;
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
			guildMemberRoles.emplace_back(Roles::getCachedRole({ .guildId = guildMember.guildId, .roleId = value }));
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
		const GuildData guild = Guilds::getCachedGuild({ .guildId = guildMember.guildId });
		if (guild.ownerId == guildMember.id) {
			return Permissions::getAllPermissions();
		}
		std::vector<RoleData> guildRoles{};
		for (auto& value: guild.roles) {
			guildRoles.emplace_back(Roles::getCachedRole({ .guildId = guild.id, .roleId = value }));
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
			guildMemberRoles.emplace_back(Roles::getCachedRole({ .guildId = guild.id, .roleId = value }));
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions;
		}

		if ((permissions & static_cast<uint64_t>(Permission::Administrator)) == static_cast<uint64_t>(Permission::Administrator)) {
			return Permissions::getAllPermissions();
		}

		return std::to_string(permissions);
	}

	void reportException(const std::string& currentFunctionName, std::source_location location) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			std::stringstream stream{};
			stream << shiftToBrightRed() << "Error Report: \n"
				   << "Caught At: " << currentFunctionName << ", in File: " << location.file_name() << " (" << std::to_string(location.line()) << ":"
				   << std::to_string(location.column()) << ")"
				   << "\nThe Error: \n"
				   << e.what() << reset() << std::endl
				   << std::endl;
			auto returnString = stream.str();
			cout << returnString;
		}
	}

	void rethrowException(const std::string& currentFunctionName, std::source_location location) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			std::stringstream stream{};
			stream << shiftToBrightRed() << "Caught At: " << currentFunctionName << ", in File: " << location.file_name() << " ("
				   << std::to_string(location.line()) << ":" << std::to_string(location.column()) << ")"
				   << "\nThe Error: \n"
				   << e.what() << reset();
			auto returnString = stream.str();
			cout << returnString;
			if (std::current_exception()) {
				std::rethrow_exception(std::current_exception());
			}
		}
	}

	std::string constructMultiPartData(std::string data, const std::vector<File>& files) {
		const std::string boundary("boundary25");
		const std::string partStart("--" + boundary + "\r\nContent-Type: application/octet-stream\r\nContent-Disposition: form-data; ");

		std::string content("--" + boundary);

		content += "\r\nContent-Type: application/json\r\nContent-Disposition: form-data; "
				   "name=\"payload_json\"\r\n\r\n";
		content += data + "\r\n";
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

	std::string base64Encode(const std::string& string, bool url) {
		const char* base64CharsArray[2] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"abcdefghijklmnopqrstuvwxyz"
											"0123456789"
											"+/",

			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789"
			"-_" };

		uint64_t encodedLength = (string.size() + 2) / 3 * 4;

		uint8_t trailing_char = url ? '.' : '=';

		const char* base64Chars = base64CharsArray[url];

		std::string returnString{};
		returnString.reserve(encodedLength);
		DiscordCoreAPI::StopWatch stopWatch{ 1500ms };
		uint64_t pos = 0;
		while (pos < string.size()) {
			if (stopWatch.hasTimePassed()) {
				break;
			}
			returnString.push_back(base64Chars[(string[static_cast<uint64_t>(pos + 0)] & 0xfc) >> 2]);

			if (static_cast<uint64_t>(pos + 1) < string.size()) {
				returnString.push_back(
					base64Chars[((string[static_cast<int64_t>(pos + 0)] & 0x03) << 4) + ((string[static_cast<int64_t>(pos + 1)] & 0xf0) >> 4)]);

				if (static_cast<uint64_t>(pos + 2) < string.size()) {
					returnString.push_back(
						base64Chars[((string[static_cast<int64_t>(pos + 1)] & 0x0f) << 2) + ((string[static_cast<int64_t>(pos + 2)] & 0xc0) >> 6)]);
					returnString.push_back(base64Chars[string[static_cast<int64_t>(pos + 2)] & 0x3f]);
				} else {
					returnString.push_back(base64Chars[(string[static_cast<int64_t>(pos + 1)] & 0x0f) << 2]);
					returnString.push_back(trailing_char);
				}
			} else {
				returnString.push_back(base64Chars[(string[static_cast<int64_t>(pos + 0)] & 0x03) << 4]);
				returnString.push_back(trailing_char);
				returnString.push_back(trailing_char);
			}

			pos += 3;
		}

		return returnString;
	}

	std::string loadFileContents(const std::string& filePath) {
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		std::ostringstream stream{};
		stream << file.rdbuf();
		return stream.str();
	}

	std::string utf8MakeValid(const std::string& inputString) {
		std::string returnString{};
		for (auto& value: inputString) {
			if (value >= 128 || value < 0) {
				int32_t difference = 0 - value;
				returnString.push_back(value + difference);
			} else {
				returnString.push_back(value);
			}
		}
		return returnString;
	}

	std::string urlEncode(const std::string& inputString) {
		std::ostringstream escaped{};
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator x = inputString.begin(), n = inputString.end(); x != n; ++x) {
			std::string::value_type c = (*x);

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
		uint64_t startTime =
			std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count();
		uint64_t timePassed{ 0 };
		while (timePassed < timeInNsToSpinLockFor) {
			timePassed = std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count() -
				startTime;
		}
	}

	std::string generateBase64EncodedKey() {
		std::string returnString{};
		returnString.resize(16);
		std::mt19937_64 randomEngine{ static_cast<uint64_t>(HRClock::now().time_since_epoch().count()) };
		for (uint32_t x = 0; x < 16; ++x) {
			returnString[x] = static_cast<uint8_t>((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * 255.0f);
		}
		returnString = base64Encode(returnString, false);
		return returnString;
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
		std::this_thread::sleep_for(Nanoseconds{ ns });
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
		uint64_t size = strftime(timeStamp.data(), 48, "%F %R", &time);
		timeStamp.resize(size);
		return timeStamp;
	}

	std::string escapeCharacters(std::string_view string) {
		std::string stringNew{};
		if (stringNew.size() <= string.size() * 2) {
			stringNew.resize(string.size() * 2);
		}
		uint64_t index{};
		for (uint32_t x = 0; x < string.size(); ++x) {
			switch (static_cast<char>(string[x])) {
				case 0x00: {
					break;
				}
				case 0x22: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('"');
					index += 2;
					break;
				}
				case 0x5c: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('\\');
					index += 2;
					break;
				}
				case 0x07: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('a');
					index += 2;
					break;
				}
				case 0x08: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('b');
					index += 2;
					break;
				}
				case 0x0C: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('f');
					index += 2;
					break;
				}
				case 0x0A: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('n');
					index += 2;
					break;
				}
				case 0x0D: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('r');
					index += 2;
					break;
				}
				case 0x0B: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('v');
					index += 2;
					break;
				}
				case 0x09: {
					stringNew[index] = static_cast<char>('\\');
					stringNew[index + 1] = static_cast<char>('t');
					index += 2;
					break;
				}
				default: {
					stringNew[index] = string[x];
					index++;
					break;
				}
			}
		}
		if (stringNew.size() > 1) {
			if (stringNew[stringNew.size() - 1] == '\0') {
				stringNew.erase(stringNew.size() - 1);
			}
		}
		stringNew.resize(index);
		return stringNew;
	}

	template<typename Object> std::unordered_map<std::string, UnboundedMessageBlock<Object>*> ObjectCollector<Object>::objectsBuffersMap{};
};

namespace DiscordCoreInternal {

	StringBuffer::StringBuffer() noexcept {
		this->string01.resize(1024 * 16);
	}

	std::string_view StringBuffer::operator[](LengthData size) {
		std::string_view string{ this->string01.data() + size.offSet, size.length };
		return string;
	}

	char StringBuffer::operator[](uint64_t index) {
		return this->string01[index];
	}

	void StringBuffer::writeData(const char* ptr, uint64_t size) {
		if (this->sizeValue + size > this->string01.size()) {
			this->string01.resize(this->string01.size() + size);
		}
		std::copy(ptr, ptr + size, this->string01.data() + this->sizeValue);
		this->sizeValue += size;
	}

	std::string::iterator StringBuffer::begin() {
		return this->string01.begin();
	}

	std::string::iterator StringBuffer::end() {
		return this->string01.end();
	}

	void StringBuffer::erase(uint64_t amount) {
		std::copy(this->string01.data() + amount, this->string01.data() + this->sizeValue, this->string01.data());
		this->sizeValue = this->sizeValue - amount;
	}

	uint64_t StringBuffer::size() {
		return this->sizeValue;
	}

	void StringBuffer::clear() {
		this->sizeValue = 0;
	}

	char* StringBuffer::data() {
		return this->string01.data();
	}
}
