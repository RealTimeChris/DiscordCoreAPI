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
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/Utilities.hpp>
#include <fstream>

namespace DiscordCoreAPI {

	UpdatePresenceData::UpdatePresenceData(PresenceUpdateState state) {
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

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*function)( void )) {
		outputSttream << function();
		return outputSttream;
	}

	ConfigManager::ConfigManager(const DiscordCoreClientConfig& configNew) {
		config = configNew;
	}

	bool ConfigManager::doWePrintWebSocketSuccessMessages() const {
		return config.logOptions.logWebSocketSuccessMessages;
	}

	bool ConfigManager::doWePrintWebSocketErrorMessages() const {
		return config.logOptions.logWebSocketErrorMessages;
	}

	bool ConfigManager::doWePrintHttpsSuccessMessages() const {
		return config.logOptions.logHttpsSuccessMessages;
	}

	bool ConfigManager::doWePrintHttpsErrorMessages() const {
		return config.logOptions.logHttpsErrorMessages;
	}

	bool ConfigManager::doWePrintGeneralSuccessMessages() const {
		return config.logOptions.logGeneralSuccessMessages;
	}

	bool ConfigManager::doWePrintGeneralErrorMessages() const {
		return config.logOptions.logGeneralErrorMessages;
	}

	bool ConfigManager::doWeCacheGuildMembers() const {
		return config.cacheOptions.cacheGuildMembers;
	}

	bool ConfigManager::doWeCacheChannels() const {
		return config.cacheOptions.cacheChannels;
	}

	bool ConfigManager::doWeCacheUsers() const {
		return config.cacheOptions.cacheUsers;
	}

	bool ConfigManager::doWeCacheVoiceStates() const {
		return config.cacheOptions.cacheVoiceStates;
	}

	bool ConfigManager::doWeCacheGuilds() const {
		return config.cacheOptions.cacheGuilds;
	}

	bool ConfigManager::doWeCacheRoles() const {
		return config.cacheOptions.cacheRoles;
	}

	UpdatePresenceData ConfigManager::getPresenceData() const {
		return config.presenceData;
	}

	std::string ConfigManager::getBotToken() const {
		return config.botToken;
	}

	uint32_t ConfigManager::getTotalShardCount() const {
		return config.shardOptions.totalNumberOfShards;
	}

	uint32_t ConfigManager::getStartingShard() const {
		return config.shardOptions.startingShard;
	}

	uint32_t ConfigManager::getShardCountForThisProcess() const {
		return config.shardOptions.numberOfShardsForThisProcess;
	}

	std::string ConfigManager::getConnectionAddress() const {
		return config.connectionAddress;
	}

	void ConfigManager::setConnectionAddress(const std::string& connectionAddressNew) {
		config.connectionAddress = connectionAddressNew;
	}

	uint16_t ConfigManager::getConnectionPort() const {
		return config.connectionPort;
	}

	void ConfigManager::setConnectionPort(const uint16_t connectionPortNew) {
		config.connectionPort = connectionPortNew;
	}

	jsonifier::vector<RepeatedFunctionData> ConfigManager::getFunctionsToExecute() const {
		return config.functionsToExecute;
	}

	TextFormat ConfigManager::getTextFormat() const {
		return config.textFormat;
	}

	GatewayIntents ConfigManager::getGatewayIntents() {
		return config.intents;
	}

	AudioFrameData::AudioFrameData(AudioFrameType frameTypeNew) {
		type = frameTypeNew;
	}

	AudioFrameData& AudioFrameData::operator+=(std::basic_string_view<uint8_t> other) {
		if (other.size() > 0) {
			if (data.size() < other.size()) {
				data.resize(other.size());
			}
			std::memcpy(data.data(), other.data(), other.size());
		}
		currentSize = other.size();
		return *this;
	}

	void AudioFrameData::clearData() {
		type		  = AudioFrameType::Unset;
		guildMemberId = 0;
		currentSize	  = 0;
		data.clear();
	}

	ColorValue::ColorValue(uint32_t colorValue) {
		color = colorValue;
	}

	ColorValue::ColorValue(std::string hexColorValue) {
		if (hexColorValue == "") {
			hexColorValue = "fefefe";
		}
		color = static_cast<uint32_t>(stoi(hexColorValue, nullptr, 16));
	}

	RGBColorValue ColorValue::getRgbColorValue() {
		uint8_t red	  = static_cast<uint8_t>(color >> 16);
		uint8_t green = static_cast<uint8_t>(color >> 8);
		uint8_t blue  = static_cast<uint8_t>(color);
		RGBColorValue colorNew{};
		colorNew.green = green;
		colorNew.blue  = blue;
		colorNew.red   = red;
		return colorNew;
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
			lowBits	 = 0;
			return *this;
		}
		if (newHash.length() >= 32 && newHash.find("a_") != std::string::npos) {
			newHash = newHash.substr(newHash.find("a_") + 2);
		}
		if (newHash.length() != 32) {
			throw DCAException{ "Sorry, but that is an incorrect IconHash length, it must be 32 characters long." };
		}
		lowBits	 = fromString<uint64_t>(newHash.substr(0, 16), std::hex);
		highBits = fromString<uint64_t>(newHash.substr(16, 16), std::hex);
		return *this;
	}

	IconHash::IconHash(const std::string& string) {
		*this = string;
	}

	IconHash::operator std::string() const {
		if (highBits == 0 || lowBits == 0) {
			return {};
		} else {
			return std::string{ toHex(lowBits) + toHex(highBits) };
		}
	}

	std::string operator+(const IconHash& lhs, const std::string& rhs) {
		std::string newString = lhs.operator std::string() += rhs;
		return newString;
	}

	bool IconHash::operator==(const IconHash& rhs) const {
		return highBits == rhs.highBits && lowBits == rhs.lowBits;
	}

	bool IconHash::operator==(const std::string& rhs) const {
		return operator std::string() == rhs;
	}

	uint64_t strtoull(std::string_view string) {
		return stoull(std::string{ string });
	}

	template<> std::string PermissionsBase<Permissions>::computeOverwrites(const std::string& basePermissions, const GuildMemberData& guildMember, const ChannelData& channel) {
		if ((stoull(basePermissions) & static_cast<uint64_t>(Permission::Administrator)) & static_cast<uint64_t>(Permission::Administrator)) {
			return getAllPermissions();
		}

		uint64_t permissions = stoull(basePermissions);
		for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites.at(x).id == guildMember.guildId) {
				permissions &= ~channel.permissionOverwrites.at(x).deny;
				permissions |= channel.permissionOverwrites.at(x).allow;
				break;
			}
		}
		jsonifier::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(Roles::getCachedRole({ .guildId = guildMember.guildId, .roleId = value }));
		}
		uint64_t allow{};
		uint64_t deny{};
		for (auto& value: guildMemberRoles) {
			for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
				if (value.id == channel.permissionOverwrites.at(x).id) {
					allow |= channel.permissionOverwrites.at(x).allow;
					deny |= channel.permissionOverwrites.at(x).deny;
				}
			}
		}
		permissions &= ~deny;
		permissions |= allow;
		for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites.at(x).id == guildMember.user.id) {
				permissions &= ~channel.permissionOverwrites.at(x).deny;
				permissions |= channel.permissionOverwrites.at(x).allow;
				break;
			}
		}
		return std::to_string(permissions);
	}

	template<> std::string PermissionsBase<Permissions>::computeBasePermissions(const GuildMemberData& guildMember) {
		const GuildData guild = Guilds::getCachedGuild({ .guildId = guildMember.guildId });
		if (guild.ownerId == guildMember.user.id) {
			return getAllPermissions();
		}
		jsonifier::vector<RoleData> guildRoles{};
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
		if (roleEveryone.permissions != "0") {
			permissions = roleEveryone.permissions;
		}
		GetGuildMemberRolesData getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId	 = guildMember.guildId;
		jsonifier::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			auto valueNew = Roles::getCachedRole({ .guildId = guild.id, .roleId = value });
			guildMemberRoles.emplace_back(valueNew);
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions.operator uint64_t();
		}

		if (permissions & static_cast<uint64_t>(Permission::Administrator)) {
			return getAllPermissions();
		}

		return std::to_string(permissions);
	}

	template<>
	std::string PermissionsBase<PermissionsParse>::computeOverwrites(const std::string& basePermissions, const GuildMemberData& guildMember, const ChannelData& channel) {
		if ((stoull(basePermissions) & static_cast<uint64_t>(Permission::Administrator)) & static_cast<uint64_t>(Permission::Administrator)) {
			return getAllPermissions();
		}

		uint64_t permissions = stoull(basePermissions);
		for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites.at(x).id == guildMember.guildId) {
				permissions &= ~channel.permissionOverwrites.at(x).deny;
				permissions |= channel.permissionOverwrites.at(x).allow;
				break;
			}
		}
		jsonifier::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(Roles::getCachedRole({ .guildId = guildMember.guildId, .roleId = value }));
		}
		uint64_t allow{};
		uint64_t deny{};
		for (auto& value: guildMemberRoles) {
			for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
				if (value.id == channel.permissionOverwrites.at(x).id) {
					allow |= channel.permissionOverwrites.at(x).allow;
					deny |= channel.permissionOverwrites.at(x).deny;
				}
			}
		}
		permissions &= ~deny;
		permissions |= allow;
		for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites.at(x).id == guildMember.user.id) {
				permissions &= ~channel.permissionOverwrites.at(x).deny;
				permissions |= channel.permissionOverwrites.at(x).allow;
				break;
			}
		}
		return std::to_string(permissions);
	}

	template<> std::string PermissionsBase<PermissionsParse>::computeBasePermissions(const GuildMemberData& guildMember) {
		const GuildData guild = Guilds::getCachedGuild({ .guildId = guildMember.guildId });
		if (guild.ownerId == guildMember.user.id) {
			return getAllPermissions();
		}
		jsonifier::vector<RoleData> guildRoles{};
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
		if (roleEveryone.permissions != "0") {
			permissions = roleEveryone.permissions;
		}
		GetGuildMemberRolesData getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId	 = guildMember.guildId;
		jsonifier::vector<RoleData> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			auto valueNew = Roles::getCachedRole({ .guildId = guild.id, .roleId = value });
			guildMemberRoles.emplace_back(valueNew);
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions.operator uint64_t();
		}

		if (permissions & static_cast<uint64_t>(Permission::Administrator)) {
			return getAllPermissions();
		}

		return std::to_string(permissions);
	}

	std::string constructMultiPartData(const std::string& data, const jsonifier::vector<File>& files) {
		const std::string boundary("boundary25");
		const std::string partStart("--" + boundary + "\r\nContent-Type: application/octet-stream\r\nContent-Disposition: form-data; ");

		std::string content("--" + boundary);

		content += "\r\nContent-Type: application/json\r\nContent-Disposition: form-data; "
				   "name=\"payload_json\"\r\n\r\n";
		content += data + "\r\n";
		if (files.size() == 1) {
			content += partStart + "name=\"file\"; filename=\"" + files.at(0).fileName + "\"" + "\r\n\r\n";
			content += files.at(0).data;
		} else {
			for (uint64_t x = 0; x < files.size(); ++x) {
				content += partStart + "name=\"files[" + std::to_string(x) + "]\"; filename=\"" + files.at(x).fileName + "\"\r\n\r\n";
				content += files.at(x).data;
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
		StopWatch<Milliseconds> stopWatch{ 1500ms };
		stopWatch.reset();
		uint64_t pos = 0;
		while (pos < string.size()) {
			if (stopWatch.hasTimeElapsed()) {
				break;
			}
			returnString.push_back(base64Chars[(string[static_cast<uint64_t>(pos + 0)] & 0xfc) >> 2]);

			if (static_cast<uint64_t>(pos + 1) < string.size()) {
				returnString.push_back(base64Chars[((string[static_cast<int64_t>(pos + 0)] & 0x03) << 4) + ((string[static_cast<int64_t>(pos + 1)] & 0xf0) >> 4)]);

				if (static_cast<uint64_t>(pos + 2) < string.size()) {
					returnString.push_back(base64Chars[((string[static_cast<int64_t>(pos + 1)] & 0x0f) << 2) + ((string[static_cast<int64_t>(pos + 2)] & 0xc0) >> 6)]);
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
				uint64_t difference = 0ull - value;
				if (value + difference == '\0') {
					continue;
				} else {
					returnString.push_back(value + static_cast<char>(difference));
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

			if (isalnum(static_cast<uint8_t>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << uint64_t(static_cast<uint8_t>(c));
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
		for (uint64_t x = 0; x < 16; ++x) {
			returnString.at(x) = static_cast<uint8_t>((static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max())) * 255.0f);
		}
		returnString = base64Encode(returnString, false);
		return returnString;
	}

	bool nanoSleep(int64_t ns) {
#if defined _WIN32
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
		time_t now = std::time(nullptr);
		std::tm time{ getCurrentTimeVal(now) };
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
