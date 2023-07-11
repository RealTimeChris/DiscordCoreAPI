/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
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

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		WebSocketClose& WebSocketClose::operator=(uint16_t valueNew) {
			value = static_cast<WebSocketCloseCode>(valueNew);
			return *this;
		};

		WebSocketClose::WebSocketClose(uint16_t valueNew) {
			*this = valueNew;
		};

		WebSocketClose::operator std::string_view() {
			return WebSocketClose::outputErrorValues[mappingValues[static_cast<uint16_t>(value)]];
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

		VoiceWebSocketClose::operator std::string_view() {
			return VoiceWebSocketClose::outputErrorValues[mappingValues[static_cast<uint16_t>(value)]];
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
			return std::string{ "Code: " + std::to_string(static_cast<uint32_t>(value)) + std::string{ ", Message: " } +
				static_cast<std::string>(HttpsResponseCode::outputErrorValues[value]) };
		}

		HttpsResponseCode::operator uint32_t() {
			return static_cast<uint32_t>(value);
		}
	}

	UpdatePresenceData::UpdatePresenceData(PresenceUpdateState state) noexcept {
		status = state;
		switch (status) {
			case PresenceUpdateState::Online: {
				statusReal = "online";
				return;
			}
			case PresenceUpdateState::Do_Not_Disturb: {
				statusReal = "dnd";
				return;
			}
			case PresenceUpdateState::Idle: {
				statusReal = "idle";
				return;
			}
			case PresenceUpdateState::Invisible: {
				statusReal = "invisible";
				return;
			}
			case PresenceUpdateState::Offline: {
				statusReal = "offline";
				return;
			}
		}
	}

	DCAException::DCAException(const std::string& error, std::source_location location) noexcept : std::runtime_error(error) {
		std::stringstream stream{};
		stream << "Thrown From: " << location.file_name() << " (" << std::to_string(location.line()) << ":" << std::to_string(location.column())
			   << ")\n"
			   << error << std::endl
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

	AudioFrameData::AudioFrameData(AudioFrameType frameTypeNew) noexcept {
		type = frameTypeNew;
	}

	AudioFrameData& AudioFrameData::operator+=(std::basic_string_view<uint8_t> other) noexcept {
		if (other.size() > 0) {
			if (data.size() < other.size()) {
				data.resize(other.size());
			}
			std::memcpy(data.data(), other.data(), other.size());
		}
		currentSize = other.size();
		return *this;
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
		RoleDataVector guildMemberRoles{};
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
		RoleDataVector guildRoles{};
		for (auto& value: guild.roles) {
			guildRoles.emplace_back(Roles::getCachedRole({ .guildId = guild.id, .roleId = value.id }));
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
		RoleDataVector guildMemberRoles{};
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

	void reportException(const std::string& currentFunctionName, std::source_location location) {
		try {
			auto exception = std::current_exception();
			if (exception) {
				std::rethrow_exception(exception);
			}
		} catch (const std::exception& e) {
			MessagePrinter::printError<PrintMessageType::General>(e.what(), location);
		}
	}

	void rethrowException(const std::string& currentFunctionName, std::source_location location) {
		try {
			auto currentException = std::current_exception();
			if (currentException) {
				std::rethrow_exception(currentException);
			}
		} catch (const std::exception& e) {
			MessagePrinter::printError<PrintMessageType::General>(e.what(), location);
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

	bool nanoSleep(int64_t ns) {
#ifdef _WIN32
		HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
		LARGE_INTEGER largeInt{ .QuadPart = -ns / 100 };
		if (!timer) {
			return false;
		}

		if (!SetWaitableTimerEx(timer, &largeInt, 0, nullptr, nullptr, nullptr, 0)) {
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
};
