/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
		value = static_cast<WebSocketCloseCode>(valueNew);
		return *this;
	};

	WebSocketClose::WebSocketClose(uint16_t valueNew) {
		*this = valueNew;
	};

	WebSocketClose::operator std::string() {
		return WebSocketClose::outputErrorValues[mappingValues[static_cast<uint16_t>(*this)]];
	}

	WebSocketClose::operator bool() {
		return static_cast<std::underlying_type_t<decltype(value)>>(value) &
			static_cast<std::underlying_type_t<decltype(value)>>(WebSocketCloseCode::We_Do_Reconnect);
	}

	VoiceWebSocketClose& VoiceWebSocketClose::operator=(uint16_t valueNew) {
		value = static_cast<VoiceWebSocketCloseCode>(valueNew);
		return *this;
	};

	VoiceWebSocketClose::VoiceWebSocketClose(uint16_t value) {
		*this = value;
	};

	VoiceWebSocketClose::operator std::string() {
		return VoiceWebSocketClose::outputErrorValues[mappingValues[static_cast<uint16_t>(*this)]];
	}

	VoiceWebSocketClose::operator bool() {
		return true;
	}

	HttpsResponseCode& HttpsResponseCode::operator=(uint32_t valueNew) {
		value = static_cast<HttpsResponseCodes>(valueNew);
		return *this;
	}

	HttpsResponseCode::HttpsResponseCode(uint32_t value) {
		*this = value;
	}

	HttpsResponseCode::operator std::string() {
		return std::string{ "Code: " + std::to_string(static_cast<uint32_t>(value)) + ", Message: " + HttpsResponseCode::outputErrorValues[value] };
	}

	HttpsResponseCode::operator uint32_t() {
		return static_cast<uint32_t>(value);
	}
}

namespace DiscordCoreAPI {

	DCAException::DCAException(const std::string& error, std::source_location location) noexcept
		: std::runtime_error(error) {
		std::stringstream stream{};
		stream << shiftToBrightRed() << "Thrown From: " << location.file_name() << " (" << std::to_string(location.line()) << ":"
			   << std::to_string(location.column()) << ")\n"
			   << error << reset() << std::endl
			   << std::endl;
		*static_cast<std::runtime_error*>(this) = std::runtime_error{ stream.str() };
	}

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*function)( void )) {
		outputSttream << function();
		return outputSttream;
	}

	ConfigManager::ConfigManager(const DiscordCoreClientConfig& configNew) {
		config = configNew;
	}

	const bool ConfigManager::doWePrintWebSocketSuccessMessages() const {
		return config.logOptions.logWebSocketSuccessMessages;
	}

	const bool ConfigManager::doWePrintWebSocketErrorMessages() const {
		return config.logOptions.logWebSocketErrorMessages;
	}

	const bool ConfigManager::doWePrintHttpsSuccessMessages() const {
		return config.logOptions.logHttpsSuccessMessages;
	}

	const bool ConfigManager::doWePrintHttpsErrorMessages() const {
		return config.logOptions.logHttpsErrorMessages;
	}

	const bool ConfigManager::doWePrintGeneralSuccessMessages() const {
		return config.logOptions.logGeneralSuccessMessages;
	}

	const bool ConfigManager::doWePrintGeneralErrorMessages() const {
		return config.logOptions.logGeneralErrorMessages;
	}

	const bool ConfigManager::doWeCacheChannels() const {
		return config.cacheOptions.cacheChannels;
	}

	const bool ConfigManager::doWeCacheUsers() const {
		return config.cacheOptions.cacheUsers;
	}

	const bool ConfigManager::doWeCacheGuilds() const {
		return config.cacheOptions.cacheGuilds;
	}

	const bool ConfigManager::doWeCacheRoles() const {
		return config.cacheOptions.cacheRoles;
	}

	const UpdatePresenceData ConfigManager::getPresenceData() const {
		return config.presenceData;
	}

	const std::string ConfigManager::getBotToken() const {
		return config.botToken;
	}

	const uint32_t ConfigManager::getTotalShardCount() const {
		return config.shardOptions.totalNumberOfShards;
	}

	const uint32_t ConfigManager::getStartingShard() const {
		return config.shardOptions.startingShard;
	}

	const uint32_t ConfigManager::getShardCountForThisProcess() const {
		return config.shardOptions.numberOfShardsForThisProcess;
	}

	const std::string ConfigManager::getConnectionAddress() const {
		return config.connectionAddress;
	}

	void ConfigManager::setConnectionAddress(const std::string& connectionAddressNew) {
		config.connectionAddress = connectionAddressNew;
	}

	const uint16_t ConfigManager::getConnectionPort() const {
		return config.connectionPort;
	}

	void ConfigManager::setConnectionPort(const uint16_t connectionPortNew) {
		config.connectionPort = connectionPortNew;
	}

	const std::vector<RepeatedFunctionData> ConfigManager::getFunctionsToExecute() const {
		return config.functionsToExecute;
	}

	const TextFormat ConfigManager::getTextFormat() const {
		return config.textFormat;
	}

	const GatewayIntents ConfigManager::getGatewayIntents() {
		return config.intents;
	}

	AudioFrameData& AudioFrameData::operator+=(std::basic_string_view<uint8_t> other) noexcept {
		if (other.size() > 0) {
			if (data.size() < other.size()) {
				data.resize(other.size());
			}
			std::memcpy(data.data(), other.data(), other.size());
		}
		currentSize = other.size();
		type = AudioFrameType::RawPCM;
		return *this;
	}

	AudioFrameData& AudioFrameData::operator+=(uint8_t character) {
		if (data.size() < currentSize + 1) {
			data.resize((currentSize + 1) * 2);
		}
		data[currentSize++] = character;
		return *this;
	}

	bool operator==(const AudioFrameData& lhs, const AudioFrameData& rhs) {
		if (lhs.data != rhs.data) {
			return false;
		}
		if (lhs.currentSize != rhs.currentSize) {
			return false;
		}
		if (lhs.guildMemberId != rhs.guildMemberId) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		return true;
	}

	void AudioFrameData::clearData() noexcept {
		type = AudioFrameType::Unset;
		guildMemberId = 0;
		currentSize = 0;
		data.clear();
	}

	ColorValue::ColorValue(uint32_t colorValue) {
		color = colorValue;
	}

	ColorValue::ColorValue(std::string hexColorValue) {
		if (hexColorValue == "") {
			hexColorValue = "fefefe";
		}
		color = stoull(hexColorValue, nullptr, 16);
	}

	RGBColorValue ColorValue::getRgbColorValue() {
		uint8_t red = static_cast<uint8_t>(color >> 16);
		uint8_t green = static_cast<uint8_t>(color >> 8);
		uint8_t blue = static_cast<uint8_t>(color);
		RGBColorValue color{};
		color.green = green;
		color.blue = blue;
		color.red = red;
		return color;
	}

	HexColorValue ColorValue::getHexColorValue() {
		std::stringstream stream{};
		stream << std::hex << color;
		return stream.str();
	}

	uint32_t ColorValue::getIntColorValue() {
		return color;
	}

	IconHash& IconHash::operator=(const std::string& string) {
		std::string newHash{ string };
		if (newHash.empty() || newHash == "0") {
			highBits = 0;
			lowBits = 0;
			return *this;
		}
		if (newHash.length() == 34 && newHash.substr(0, 2) == "a_") {
			newHash = newHash.substr(2);
		}
		if (newHash.length() != 32 && newHash.length() != 33) {
			throw std::length_error(
				"IconHash must be exactly 32 characters in length, passed value is: '" + std::to_string(newHash.size()) + "', in length.");
		}
		lowBits = fromString<uint64_t>(newHash.substr(0, 16), std::hex);
		highBits = fromString<uint64_t>(newHash.substr(16, 16), std::hex);
		return *this;
	}

	IconHash::IconHash(const std::string& string) noexcept {
		*this = string;
	}

	IconHash::operator std::string() noexcept {
		if (highBits == 0 || lowBits == 0) {
			return {};
		} else {
			return std::string{ toHex(lowBits) + toHex(highBits) };
		}
	}

	bool IconHash::operator==(const IconHash& other) {
		return lowBits == other.lowBits && highBits == other.highBits;
	}

	uint64_t strtoull(std::string_view string) {
		return stoull(std::string{ string });
	}

	Permissions& Permissions::operator=(Permission&& other) {
		permissions = static_cast<uint64_t>(other);
		return *this;
	}

	Permissions& Permissions::operator=(const Permission& other) {
		permissions = static_cast<uint64_t>(other);
		return *this;
	}

	Permissions::Permissions(const Permission& permsNew) {
		*this = permsNew;
	}

	Permissions& Permissions::operator=(std::string&& other) {
		if (other.size() == 0 || other == "") {
			permissions = 0;
		} else {
			permissions = stoull(other);
		}
		other = "";
		return *this;
	}

	Permissions::Permissions(std::string&& permsNew) {
		*this = std::move(permsNew);
	}

	Permissions& Permissions::operator=(const std::string& other) {
		if (other.size() == 0 || other == "") {
			permissions = 0;
		} else {
			permissions = stoull(other);
		}
		return *this;
	}

	Permissions::Permissions(const std::string& permsNew) {
		*this = permsNew;
	}

	Permissions& Permissions::operator=(uint64_t other) {
		permissions = other;
		return *this;
	}

	Permissions::Permissions(uint64_t permsNew) {
		*this = permsNew;
	}

	Permissions::operator uint64_t() {
		return permissions;
	}

	Permissions::operator std::string() {
		return std::to_string(permissions);
	}

	std::string Permissions::getCurrentChannelPermissions(const GuildMember& guildMember, const ChannelData& channel) {
		std::string permsString = Permissions::computePermissions(guildMember, channel);
		return permsString;
	}

	bool Permissions::checkForPermission(const GuildMember& guildMember, const ChannelData& channel, Permission permission) {
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
		uint64_t permissionsInteger = permissions;
		for (auto value: permissionsToRemove) {
			permissionsInteger &= ~static_cast<uint64_t>(value);
		}
		std::stringstream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	void Permissions::addPermissions(const std::vector<Permission>& permissionsToAdd) {
		uint64_t permissionsInteger = permissions;
		for (auto value: permissionsToAdd) {
			permissionsInteger |= static_cast<uint64_t>(value);
		}
		std::stringstream sstream{};
		sstream << permissionsInteger;
		*this = sstream.str();
	}

	std::vector<std::string> Permissions::displayPermissions() {
		std::vector<std::string> returnVector{};
		uint64_t permissionsInteger = permissions;
		if (permissionsInteger & (1ll << 3)) {
			for (int64_t x = 0; x < 41; ++x) {
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
		std::string returnString = std::to_string(permissions);
		return returnString;
	}

	std::string Permissions::getAllPermissions() {
		uint64_t allPerms{};
		for (int64_t x = 0; x < 41; ++x) {
			allPerms |= 1ll << x;
		}
		std::stringstream stream{};
		stream << allPerms;
		return stream.str();
	}

	std::string Permissions::computeOverwrites(const std::string& basePermissions, const GuildMember& guildMember, const ChannelData& channel) {
		if ((stoull(basePermissions) & static_cast<uint64_t>(Permission::Administrator)) & static_cast<uint64_t>(Permission::Administrator)) {
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
		uint64_t allow{};
		uint64_t deny{};
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
			if (channel.permissionOverwrites[x].id == guildMember.user.id) {
				permissions &= ~channel.permissionOverwrites[x].deny;
				permissions |= channel.permissionOverwrites[x].allow;
				break;
			}
		}
		return std::to_string(permissions);
	}

	std::string Permissions::computePermissions(const GuildMember& guildMember, const ChannelData& channel) {
		std::string permissions = Permissions::computeBasePermissions(guildMember);
		permissions = Permissions::computeOverwrites(permissions, guildMember, channel);
		return permissions;
	}

	std::string Permissions::computeBasePermissions(const GuildMember& guildMember) {
		const GuildData guild = Guilds::getCachedGuild({ .guildId = guildMember.guildId });
		if (guild.ownerId == guildMember.user.id) {
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
		if (roleEveryone.permissions.operator uint64_t() != 0) {
			permissions = roleEveryone.permissions.operator uint64_t();
		}
		GetGuildMemberRolesData getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId = guildMember.guildId;
		std::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			auto valueNew = Roles::getCachedRole({ .guildId = guild.id, .roleId = value });
			guildMemberRoles.emplace_back(valueNew);
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions.operator uint64_t();
		}

		if (permissions & static_cast<uint64_t>(Permission::Administrator)) {
			return Permissions::getAllPermissions();
		}

		return std::to_string(permissions);
	}

	void reportException(const std::string& currentFunctionName, std::source_location location, std::exception_ptr ptr) {
		try {
			auto exception = std::current_exception();
			if (exception) {
				std::rethrow_exception(exception);
			}
		} catch (const std::exception& e) {
			std::stringstream stream{};
			stream << shiftToBrightRed() << "Error Report: \n"
				   << "Caught At: " << currentFunctionName << ", in File: " << location.file_name() << " (" << std::to_string(location.line()) << ":"
				   << std::to_string(location.column()) << ")"
				   << "\nThe Error: \n"
				   << e.what() << reset() << std::endl
				   << std::endl;
			cout << stream.str();
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
			stream << shiftToBrightRed() << "Error Report: \n"
				   << "Caught At: " << currentFunctionName << ", in File: " << location.file_name() << " (" << std::to_string(location.line()) << ":"
				   << std::to_string(location.column()) << ")"
				   << "\nThe Error: \n"
				   << e.what() << reset() << std::endl
				   << std::endl;
			cout << stream.str();
			if (std::current_exception()) {
				std::rethrow_exception(std::current_exception());
			}
		}
	}

	std::string constructMultiPartData(const std::string& data, const std::vector<File>& files) {
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
			for (int8_t x = 0; x < files.size(); ++x) {
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
		StopWatch stopWatch{ 1500ms };
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
			if (static_cast<uint8_t>(value) >= 128 || value < 0) {
				int32_t difference = 0 - value;
				if (value + difference == '\0') {
					continue;
				} else {
					returnString.push_back(value + difference);
				}
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

			if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
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
		uint64_t startTime = std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count();
		uint64_t timePassed{};
		while (timePassed < timeInNsToSpinLockFor) {
			timePassed = std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count() - startTime;
		}
	}

	std::string generateBase64EncodedKey() {
		std::string returnString{};
		returnString.resize(16);
		std::mt19937_64 randomEngine{ static_cast<uint64_t>(HRClock::now().time_since_epoch().count()) };
		for (int32_t x = 0; x < 16; ++x) {
			returnString[x] = static_cast<uint8_t>((static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max())) * 255.0f);
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

	TimeStamp::TimeStamp(TimeFormat formatNew) {
		timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
	}

	TimeStamp::TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second,
		TimeFormat formatNew) {
		getTimeSinceEpoch(stoull(year), stoull(month), stoull(day), stoull(hour), stoull(minute), stoull(second));
	}

	TimeStamp::operator std::string() {
		return getISO8601TimeStamp(TimeFormat::LongDateTime);
	}

	TimeStamp::operator uint64_t() {
		if (timeStampInTimeUnits == 0) {
			timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
		}
		return timeStampInTimeUnits;
	}

	TimeStamp& TimeStamp::operator=(std::string&& originalTimeStampNew) {
		convertTimeStampToTimeUnits(TimeFormat::LongDateTime, originalTimeStampNew);
		return *this;
	}

	TimeStamp::TimeStamp(std::string&& originalTimeStampNew) {
		*this = std::move(originalTimeStampNew);
		if (timeStampInTimeUnits == 0) {
			timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
		}
	}

	TimeStamp& TimeStamp::operator=(std::string& originalTimeStampNew) {
		convertTimeStampToTimeUnits(TimeFormat::LongDateTime, originalTimeStampNew);
		return *this;
	}

	TimeStamp::TimeStamp(std::string& originalTimeStampNew) {
		*this = originalTimeStampNew;
	}

	TimeStamp::TimeStamp(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, TimeFormat formatNew) {
		getTimeSinceEpoch(year, month, day, hour, minute, second);
	};

	TimeStamp::TimeStamp(uint64_t timeInTimeUnits, TimeFormat formatNew) {
		timeStampInTimeUnits = timeInTimeUnits;
	}

	std::string TimeStamp::convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd,
		int32_t yearsToAdd, TimeFormat formatNew) {
		std::time_t result = std::time(nullptr);
		int32_t secondsPerMinute{ 60 };
		int32_t minutesPerHour{ 60 };
		int32_t secondsPerHour{ minutesPerHour * secondsPerMinute };
		int32_t hoursPerDay{ 24 };
		int32_t secondsPerDay{ secondsPerHour * hoursPerDay };
		int32_t daysPerMonth{ 30 };
		int32_t secondsPerMonth{ secondsPerDay * daysPerMonth };
		int32_t daysPerYear{ 365 };
		int32_t secondsPerYear{ secondsPerDay * daysPerYear };
		int32_t secondsToAdd = (yearsToAdd * secondsPerYear) + (monthsToAdd * secondsPerMonth) + (daysToAdd * secondsPerDay) +
			((hoursToAdd + 8) * secondsPerHour) + (minutesToAdd * secondsPerMinute);
		result += secondsToAdd;
		auto resultTwo = std::localtime(&result);
		std::string returnString{};
		if (resultTwo->tm_isdst) {
			if (resultTwo->tm_hour + 4 >= 24) {
				resultTwo->tm_hour = resultTwo->tm_hour - 24;
				++resultTwo->tm_mday;
			}
			TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
				std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), formatNew };
			timeStamp.getISO8601TimeStamp(formatNew);
			returnString = static_cast<std::string>(timeStamp);
		} else {
			if (resultTwo->tm_hour + 5 >= 24) {
				resultTwo->tm_hour = resultTwo->tm_hour - 24;
				++resultTwo->tm_mday;
			}
			TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
				std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), formatNew };
			timeStamp.getISO8601TimeStamp(formatNew);
			returnString = static_cast<std::string>(timeStamp);
		}
		return returnString;
	}

	std::string TimeStamp::convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
		std::time_t result = std::time(nullptr);
		auto resultTwo = std::localtime(&result);
		std::string returnString{};
		if (resultTwo->tm_isdst) {
			if (resultTwo->tm_hour + 4 >= 24) {
				resultTwo->tm_hour = resultTwo->tm_hour - 24;
				++resultTwo->tm_mday;
			}
			TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
				std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), timeFormat };
			returnString = timeStamp.getISO8601TimeStamp(timeFormat);
		} else {
			if (resultTwo->tm_hour + 5 >= 24) {
				resultTwo->tm_hour = resultTwo->tm_hour - 24;
				++resultTwo->tm_mday;
			}
			TimeStamp timeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
				std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), timeFormat };
			returnString = timeStamp.getISO8601TimeStamp(timeFormat);
		}
		return returnString;
	}

	bool TimeStamp::hasTimeElapsed(uint64_t days, uint64_t hours, uint64_t minutes) {
		if (timeStampInTimeUnits <= 0) {
			timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
		}
		int64_t startTimeRaw = timeStampInTimeUnits;
		auto currentTime = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
		int64_t secondsPerMinute = 60;
		int64_t secondsPerHour = secondsPerMinute * 60;
		int64_t secondsPerDay = secondsPerHour * 24;
		auto targetElapsedTime = ((static_cast<int64_t>(days) * secondsPerDay) + (static_cast<int64_t>(hours) * secondsPerHour) +
									 (static_cast<int64_t>(minutes) * secondsPerMinute)) *
			1000;
		auto actualElapsedTime = currentTime - startTimeRaw;
		if (actualElapsedTime <= 0) {
			return false;
		}
		if (actualElapsedTime >= targetElapsedTime) {
			return true;
		} else {
			return false;
		}
	}

	std::string TimeStamp::convertMsToDurationString(uint64_t durationInMs) {
		std::string newString{};
		uint64_t msPerSecond{ 1000 };
		uint64_t secondsPerMinute{ 60 };
		uint64_t minutesPerHour{ 60 };
		uint64_t msPerMinute{ msPerSecond * secondsPerMinute };
		uint64_t msPerHour{ msPerMinute * minutesPerHour };
		uint64_t hoursLeft = static_cast<uint64_t>(trunc(durationInMs / msPerHour));
		uint64_t minutesLeft = static_cast<uint64_t>(trunc((durationInMs % msPerHour) / msPerMinute));
		uint64_t secondsLeft = static_cast<uint64_t>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
		if (hoursLeft >= 1) {
			newString += std::to_string(hoursLeft) + " Hours, ";
			newString += std::to_string(minutesLeft) + " Minutes, ";
			newString += std::to_string(secondsLeft) + " Seconds.";
		} else if (minutesLeft >= 1) {
			newString += std::to_string(minutesLeft) + " Minutes, ";
			newString += std::to_string(secondsLeft) + " Seconds.";
		} else {
			newString += std::to_string(secondsLeft) + " Seconds.";
		}
		return newString;
	}

	void TimeStamp::getTimeSinceEpoch(int64_t year, int64_t month, int64_t day, int64_t hour, int64_t minute, int64_t second) {
		const uint32_t secondsInJan{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInFeb{ 28 * 24 * 60 * 60 };
		const uint32_t secondsInMar{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInApr{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInMay{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInJun{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInJul{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInAug{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInSep{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInOct{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInNov{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInDec{ 31 * 24 * 60 * 60 };
		const uint32_t secondsPerMinute{ 60 };
		const uint32_t secondsPerHour{ 60 * 60 };
		const uint32_t secondsPerDay{ 60 * 60 * 24 };
		Seconds value{};
		for (int32_t x = 1970; x < year; ++x) {
			value += Seconds{ secondsInJan };
			value += Seconds{ secondsInFeb };
			value += Seconds{ secondsInMar };
			value += Seconds{ secondsInApr };
			value += Seconds{ secondsInMay };
			value += Seconds{ secondsInJun };
			value += Seconds{ secondsInJul };
			value += Seconds{ secondsInAug };
			value += Seconds{ secondsInSep };
			value += Seconds{ secondsInOct };
			value += Seconds{ secondsInNov };
			value += Seconds{ secondsInDec };
			if (x % 4 == 0) {
				value += Seconds{ secondsPerDay };
			}
		}
		if (month > 0) {
			value += Seconds{ static_cast<uint64_t>((day - 1) * secondsPerDay) };
			value += Seconds{ static_cast<uint64_t>(hour * secondsPerHour) };
			value += Seconds{ static_cast<uint64_t>(minute * secondsPerMinute) };
			value += Seconds{ second };
		}
		if (month > 1) {
			value += Seconds{ secondsInJan };
		}
		if (month > 2) {
			value += Seconds{ secondsInFeb };
		}
		if (month > 3) {
			value += Seconds{ secondsInMar };
		}
		if (month > 4) {
			value += Seconds{ secondsInApr };
		}
		if (month > 5) {
			value += Seconds{ secondsInMay };
		}
		if (month > 6) {
			value += Seconds{ secondsInJun };
		}
		if (month > 7) {
			value += Seconds{ secondsInJul };
		}
		if (month > 8) {
			value += Seconds{ secondsInAug };
		}
		if (month > 9) {
			value += Seconds{ secondsInSep };
		}
		if (month > 10) {
			value += Seconds{ secondsInOct };
		}
		if (month > 11) {
			value += Seconds{ secondsInNov };
		}
		timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(value).count();
	}

	void TimeStamp::convertTimeStampToTimeUnits(TimeFormat formatNew, std::string originalTimeStamp) {
		try {
			if (originalTimeStamp != "" && originalTimeStamp != "0") {
				TimeStamp timeValue = TimeStamp{ stoi(originalTimeStamp.substr(0, 4)), stoi(originalTimeStamp.substr(5, 6)),
					stoi(originalTimeStamp.substr(8, 9)), stoi(originalTimeStamp.substr(11, 12)), stoi(originalTimeStamp.substr(14, 15)),
					stoi(originalTimeStamp.substr(17, 18)), formatNew };
				timeStampInTimeUnits = static_cast<uint64_t>(timeValue);
			} else {
				timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
			}
		} catch (...) {
			reportException("TimeStamp::convertTimeStampToTimeUnits()");
		}
	}

	std::string TimeStamp::getISO8601TimeStamp(TimeFormat timeFormat) {
		if (timeStampInTimeUnits <= 0) {
			timeStampInTimeUnits = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()).count();
		}
		uint64_t timeValue = timeStampInTimeUnits / 1000;
		time_t rawTime(timeValue);
		tm timeInfo = *localtime(&rawTime);
		std::string timeStamp{};
		timeStamp.resize(48);
		switch (timeFormat) {
			case TimeFormat::LongDate: {
				uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &timeInfo);
				timeStamp.resize(sizeResponse);
				break;
			}
			case TimeFormat::LongDateTime: {
				uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%FT%ValueType", &timeInfo);
				timeStamp.resize(sizeResponse);
				break;
			}
			case TimeFormat::LongTime: {
				uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%ValueType", &timeInfo);
				timeStamp.resize(sizeResponse);
				break;
			}
			case TimeFormat::ShortDate: {
				uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &timeInfo);
				timeStamp.resize(sizeResponse);
				break;
			}
			case TimeFormat::ShortDateTime: {
				uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &timeInfo);
				timeStamp.resize(sizeResponse);
				break;
			}
			case TimeFormat::ShortTime: {
				uint64_t sizeResponse = strftime(timeStamp.data(), 48, "%R", &timeInfo);
				timeStamp.resize(sizeResponse);
				break;
			}
			default: {
				break;
			}
		}
		return timeStamp;
	}
};
