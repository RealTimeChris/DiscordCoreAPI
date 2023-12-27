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
/// Utilities.cpp - Source file for the utilities.
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
#include <time.h>

namespace discord_core_api {

	update_presence_data::update_presence_data(presence_update_state state) {
		status = state;
		switch (status) {
			case presence_update_state::online: {
				statusReal = "online";
				return;
			}
			case presence_update_state::Do_Not_Disturb: {
				statusReal = "dnd";
				return;
			}
			case presence_update_state::idle: {
				statusReal = "idle";
				return;
			}
			case presence_update_state::invisible: {
				statusReal = "invisible";
				return;
			}
			case presence_update_state::offline: {
				statusReal = "offline";
				return;
			}
		}
	}

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, jsonifier::string_view (*function)(void)) {
		outputSttream << function();
		return outputSttream;
	}

	config_manager::config_manager(const discord_core_client_config& configNew) {
		config = configNew;
	}

	bool config_manager::doWePrintWebSocketSuccessMessages() const {
		return config.logOptions.logWebSocketSuccessMessages;
	}

	bool config_manager::doWePrintWebSocketErrorMessages() const {
		return config.logOptions.logWebSocketErrorMessages;
	}

	bool config_manager::doWePrintHttpsSuccessMessages() const {
		return config.logOptions.logHttpsSuccessMessages;
	}

	bool config_manager::doWePrintHttpsErrorMessages() const {
		return config.logOptions.logHttpsErrorMessages;
	}

	bool config_manager::doWePrintGeneralSuccessMessages() const {
		return config.logOptions.logGeneralSuccessMessages;
	}

	bool config_manager::doWePrintGeneralErrorMessages() const {
		return config.logOptions.logGeneralErrorMessages;
	}

	bool config_manager::doWeCacheGuildMembers() const {
		return config.cacheOptions.cacheGuildMembers;
	}

	bool config_manager::doWeCacheChannels() const {
		return config.cacheOptions.cacheChannels;
	}

	bool config_manager::doWeCacheUsers() const {
		return config.cacheOptions.cacheUsers;
	}

	bool config_manager::doWeCacheVoiceStates() const {
		return config.cacheOptions.cacheVoiceStates;
	}

	bool config_manager::doWeCacheGuilds() const {
		return config.cacheOptions.cacheGuilds;
	}

	bool config_manager::doWeCacheRoles() const {
		return config.cacheOptions.cacheRoles;
	}

	update_presence_data config_manager::getPresenceData() const {
		return config.presenceData;
	}

	jsonifier::string config_manager::getBotToken() const {
		return config.botToken;
	}

	std::ostream* config_manager::getErrorStream() const {
		return config.logOptions.errorStream;
	}

	std::ostream* config_manager::getOutputStream() const {
		return config.logOptions.outputStream;
	}

	uint64_t config_manager::getTotalShardCount() const {
		return config.shardOptions.totalNumberOfShards;
	}

	uint64_t config_manager::getStartingShard() const {
		return config.shardOptions.startingShard;
	}

	uint64_t config_manager::getShardCountForThisProcess() const {
		return config.shardOptions.numberOfShardsForThisProcess;
	}

	jsonifier::string config_manager::getConnectionAddress() const {
		return config.connectionAddress;
	}

	void config_manager::setConnectionAddress(jsonifier::string_view connectionAddressNew) {
		config.connectionAddress = connectionAddressNew;
	}

	uint16_t config_manager::getConnectionPort() const {
		return config.connectionPort;
	}

	void config_manager::setConnectionPort(const uint16_t connectionPortNew) {
		config.connectionPort = connectionPortNew;
	}

	jsonifier::vector<repeated_function_data> config_manager::getFunctionsToExecute() const {
		return config.functionsToExecute;
	}

	text_format config_manager::getTextFormat() const {
		return config.textFormat;
	}

	gateway_intents config_manager::getGatewayIntents() {
		return config.intents;
	}

	audio_frame_data::audio_frame_data(audio_frame_type frameTypeNew) {
		type = frameTypeNew;
	}

	audio_frame_data& audio_frame_data::operator+=(jsonifier::string_view_base<uint8_t> other) {
		if (other.size() > 0) {
			if (data.size() < other.size()) {
				data.resize(other.size());
			}
			std::memcpy(data.data(), other.data(), other.size());
		}
		currentSize = static_cast<int64_t>(other.size());
		return *this;
	}

	audio_frame_data& audio_frame_data::operator+=(jsonifier::vector<uint8_t> other) {
		if (other.size() > 0) {
			if (data.size() < other.size()) {
				data.resize(other.size());
			}
			std::memcpy(data.data(), other.data(), other.size());
		}
		currentSize = static_cast<int64_t>(other.size());
		return *this;
	}

	void audio_frame_data::clearData() {
		type		= audio_frame_type::Unset;
		currentSize = 0;
		data.clear();
	}

	color_value::color_value(uint32_t colorValue) {
		color = colorValue;
	}

	color_value::color_value(jsonifier::string_view hexColorValue) {
		jsonifier::string returnString{};
		if (hexColorValue == "") {
			returnString = jsonifier::string{ "fefefe" };
		} else {
			returnString = hexColorValue;
		}
		color = static_cast<uint32_t>(jsonifier::strToInt64<16>(returnString));
	}

	rgbcolor_value color_value::getRgbColorValue() {
		uint8_t red	  = static_cast<uint8_t>(color >> 16);
		uint8_t green = static_cast<uint8_t>(color >> 8);
		uint8_t blue  = static_cast<uint8_t>(color);
		rgbcolor_value colorNew{};
		colorNew.green = green;
		colorNew.blue  = blue;
		colorNew.red   = red;
		return colorNew;
	}

	hex_color_value color_value::getHexColorValue() {
		std::stringstream stream{};
		stream << std::hex << color;
		return jsonifier::string{ stream.str() };
	}

	uint32_t color_value::getIntColorValue() {
		return color;
	}

	icon_hash& icon_hash::operator=(jsonifier::string_view string) {
		jsonifier::string newHash{ string };
		if (newHash.empty() || newHash == "0") {
			highBits = 0;
			lowBits	 = 0;
			return *this;
		}
		if (newHash.size() >= 32 && newHash.find("a_") != jsonifier::string::npos) {
			newHash = newHash.substr(newHash.find("a_") + 2);
		}
		if (newHash.size() != 32) {
			throw dca_exception{ "Sorry, but that is an incorrect icon_hash length, it must be 32 characters long." };
		}
		lowBits	 = fromString<uint64_t>(newHash.substr(0, 16), std::hex);
		highBits = fromString<uint64_t>(newHash.substr(16, 16), std::hex);
		return *this;
	}

	icon_hash::icon_hash(jsonifier::string_view string) {
		*this = string;
	}

	icon_hash::operator jsonifier::string() const {
		if (highBits == 0 || lowBits == 0) {
			return {};
		} else {
			return toHex(lowBits) + toHex(highBits);
		}
	}

	jsonifier::string operator+(const icon_hash& lhs, jsonifier::string_view rhs) {
		jsonifier::string newString = lhs.operator jsonifier::string() += rhs;
		return newString;
	}

	bool icon_hash::operator==(const icon_hash& rhs) const {
		return highBits == rhs.highBits && lowBits == rhs.lowBits;
	}

	bool icon_hash::operator==(jsonifier::string_view rhs) const {
		return operator jsonifier::string() == rhs;
	}

	template<>
	jsonifier::string permissions_base<permissions>::computeOverwrites(jsonifier::string_view basePermissions, const guild_member_data& guildMember, const channel_data& channel) {
		if ((jsonifier::strToUint64(basePermissions.data()) & static_cast<uint64_t>(permission::administrator)) & static_cast<uint64_t>(permission::administrator)) {
			return getAllPermissions();
		}

		uint64_t permissions = jsonifier::strToUint64(basePermissions.data());
		for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites.at(x).id == guildMember.guildId) {
				permissions &= ~channel.permissionOverwrites.at(x).deny;
				permissions |= channel.permissionOverwrites.at(x).allow;
				break;
			}
		}
		jsonifier::vector<role_data> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(roles::getCachedRole({ .guildId = guildMember.guildId, .roleId = value }));
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
		return jsonifier::toString(permissions);
	}

	template<> jsonifier::string permissions_base<permissions>::computeBasePermissions(const guild_member_data& guildMember) {
		const guild_cache_data guild = guilds::getCachedGuild({ .guildId = guildMember.guildId });
		if (guild.ownerId == guildMember.user.id) {
			return getAllPermissions();
		}
		jsonifier::vector<role_data> guildRoles{};
		if (roles::doWeCacheRoles()) {
			for (auto& value: guild.roles) {
				guildRoles.emplace_back(roles::getCachedRole({ .roleId = value }));
			}
		} else {
			guildRoles = roles::getGuildRolesAsync({ .guildId = guildMember.guildId }).get();
		}
		role_data roleEveryone{};
		for (auto& value: guildRoles) {
			if (value.id == guild.id) {
				roleEveryone = value;
			}
		}
		uint64_t permissions{};
		if (roleEveryone.permissions.operator std::basic_string_view<char, std::char_traits<char>>() != "0") {
			permissions = roleEveryone.permissions;
		}
		get_guild_member_roles_data getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId	 = guildMember.guildId;
		jsonifier::vector<role_data> guildMemberRoles{};
		if (roles::doWeCacheRoles()) {
			for (auto& value: guildMember.roles) {
				guildMemberRoles.emplace_back(roles::getCachedRole({ .roleId = value }));
			}
		} else {
			guildMemberRoles = roles::getGuildMemberRolesAsync({ .guildMember = guildMember, .guildId = guildMember.guildId }).get();
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions.operator uint64_t();
		}
		if (permissions & static_cast<uint64_t>(permission::administrator)) {
			return getAllPermissions();
		}

		return jsonifier::toString(permissions);
	}

	template<> jsonifier::string permissions_base<permissions_parse>::computeOverwrites(jsonifier::string_view basePermissions, const guild_member_data& guildMember,
		const channel_data& channel) {
		if ((jsonifier::strToUint64(basePermissions.data()) & static_cast<uint64_t>(permission::administrator)) & static_cast<uint64_t>(permission::administrator)) {
			return getAllPermissions();
		}

		uint64_t permissions = jsonifier::strToUint64(basePermissions.data());
		for (uint64_t x = 0; x < channel.permissionOverwrites.size(); ++x) {
			if (channel.permissionOverwrites.at(x).id == guildMember.guildId) {
				permissions &= ~channel.permissionOverwrites.at(x).deny;
				permissions |= channel.permissionOverwrites.at(x).allow;
				break;
			}
		}
		jsonifier::vector<role_data> guildMemberRoles{};
		if (roles::doWeCacheRoles()) {
			for (auto& value: guildMember.roles) {
				guildMemberRoles.emplace_back(roles::getCachedRole({ .roleId = value }));
			}
		} else {
			guildMemberRoles = roles::getGuildMemberRolesAsync({ .guildMember = guildMember, .guildId = guildMember.guildId }).get();
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
		return jsonifier::toString(permissions);
	}

	template<> jsonifier::string permissions_base<permissions_parse>::computeBasePermissions(const guild_member_data& guildMember) {
		const guild_data guild = guilds::getCachedGuild({ .guildId = guildMember.guildId });
		if (guild.ownerId == guildMember.user.id) {
			return getAllPermissions();
		}
		jsonifier::vector<role_data> guildRoles{};
		for (auto& value: guild.roles) {
			guildRoles.emplace_back(value);
		}
		role_data roleEveryone{};
		for (auto& value: guildRoles) {
			if (value.id == guild.id) {
				roleEveryone = value;
			}
		}
		uint64_t permissions{};
		if (roleEveryone.permissions.operator std::basic_string_view<char, std::char_traits<char>>() != "0") {
			permissions = roleEveryone.permissions;
		}
		get_guild_member_roles_data getRolesData{};
		getRolesData.guildMember = guildMember;
		getRolesData.guildId	 = guildMember.guildId;
		jsonifier::vector<role_data> guildMemberRoles{};
		for (auto& value: guildMember.roles) {
			guildMemberRoles.emplace_back(value);
		}
		for (auto& value: guildMemberRoles) {
			permissions |= value.permissions.operator uint64_t();
		}

		if (permissions & static_cast<uint64_t>(permission::administrator)) {
			return getAllPermissions();
		}

		return jsonifier::toString(permissions);
	}

	jsonifier::string constructMultiPartData(jsonifier::string_view data, const jsonifier::vector<file>& files) {
		static constexpr jsonifier::string_view boundary{ "boundary25" };
		static constexpr jsonifier::string_view partStart("--boundary25\r\nContent-type: application/octet-stream\r\nContent-disposition: form-data; ");

		jsonifier::string content("--" + boundary);

		content += "\r\nContent-type: application/json\r\nContent-disposition: form-data; "
				   "name=\"payload_json\"\r\n\r\n";
		content += data + "\r\n";
		if (files.size() == 1) {
			content += partStart + "name=\"file\"; filename=\"" + files.at(0).fileName + "\"" + "\r\n\r\n";
			content += files.at(0).data;
		} else {
			for (uint64_t x = 0; x < files.size(); ++x) {
				content += partStart + "name=\"files[" + jsonifier::toString(x) + "]\"; filename=\"" + files.at(x).fileName + "\"\r\n\r\n";
				content += files.at(x).data;
				content += "\r\n";
			}
		}
		content += "\r\n--" + boundary + "--";
		return content;
	}

	jsonifier::string convertToLowerCase(jsonifier::string_view stringToConvert) {
		jsonifier::string newString;
		for (auto& value: stringToConvert) {
			if (isupper(static_cast<uint8_t>(value))) {
				newString += static_cast<char>(tolower(static_cast<uint8_t>(value)));
			} else {
				newString += value;
			}
		}
		return newString;
	}

	jsonifier::string base64Encode(jsonifier::string_view string, bool url) {
		static constexpr const char* base64CharsArray[2] = { "abcdefghijklmnopqrstuvwxyz"
											"abcdefghijklmnopqrstuvwxyz"
											"0123456789"
											"+/",

			"abcdefghijklmnopqrstuvwxyz"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789"
			"-_" };

		uint64_t encodedLength = (string.size() + 2) / 3 * 4;

		char trailing_char = url ? '.' : '=';

		const char* base64Chars = base64CharsArray[url];

		jsonifier::string returnString{};
		returnString.reserve(encodedLength);
		stop_watch<milliseconds> stopWatch{ 1500ms };
		stopWatch.reset();
		uint64_t pos = 0;
		while (pos < string.size()) {
			if (stopWatch.hasTimeElapsed()) {
				break;
			}
			returnString.emplace_back(base64Chars[(string[static_cast<uint64_t>(pos + 0)] & 0xfc) >> 2]);

			if (static_cast<uint64_t>(pos + 1) < string.size()) {
				returnString.emplace_back(base64Chars[((string[static_cast<uint64_t>(pos + 0)] & 0x03) << 4) + ((string[static_cast<uint64_t>(pos + 1)] & 0xf0) >> 4)]);

				if (static_cast<uint64_t>(pos + 2) < string.size()) {
					returnString.emplace_back(base64Chars[((string[static_cast<uint64_t>(pos + 1)] & 0x0f) << 2) + ((string[static_cast<uint64_t>(pos + 2)] & 0xc0) >> 6)]);
					returnString.emplace_back(base64Chars[string[static_cast<uint64_t>(pos + 2)] & 0x3f]);
				} else {
					returnString.emplace_back(base64Chars[(string[static_cast<uint64_t>(pos + 1)] & 0x0f) << 2]);
					returnString.emplace_back(trailing_char);
				}
			} else {
				returnString.emplace_back(base64Chars[(string[static_cast<uint64_t>(pos + 0)] & 0x03) << 4]);
				returnString.emplace_back(trailing_char);
				returnString.emplace_back(trailing_char);
			}

			pos += 3;
		}

		return returnString;
	}

	jsonifier::string loadFileContents(jsonifier::string_view filePath) {
		std::ifstream file(filePath.data(), std::ios::in | std::ios::binary);
		std::ostringstream stream{};
		stream << file.rdbuf();
		return jsonifier::string{ stream.str() };
	}

	jsonifier::string utf8MakeValid(jsonifier::string_view inputString) {
		jsonifier::string returnString{};
		for (auto& value: inputString) {
			if (static_cast<uint8_t>(value) >= 128 || value < 0) {
				uint64_t difference = 0ULL - value;
				if (value + difference == '\0') {
					continue;
				} else {
					returnString.emplace_back(value + static_cast<char>(difference));
				}
			} else {
				returnString.emplace_back(value);
			}
		}
		return returnString;
	}

	jsonifier::string urlEncode(jsonifier::string_view inputString) {
		std::ostringstream escaped{};
		escaped.fill('0');
		escaped << std::hex;

		for (jsonifier::string::const_iterator x = inputString.begin(), n = inputString.end(); x != n; ++x) {
			jsonifier::string::value_type c = (*x);

			if (isalnum(static_cast<uint8_t>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << uint64_t(static_cast<uint8_t>(c));
			escaped << std::nouppercase;
		}
		return jsonifier::string{ escaped.str() };
	}

	void spinLock(uint64_t timeInNsToSpinLockFor) {
		int64_t startTime = std::chrono::duration_cast<nanoseconds>(sys_clock::now().time_since_epoch()).count();
		int64_t timePassed{};
		while (timePassed < static_cast<int64_t>(timeInNsToSpinLockFor)) {
			timePassed = std::chrono::duration_cast<nanoseconds>(sys_clock::now().time_since_epoch()).count() - startTime;
		}
	}

	jsonifier::string generateBase64EncodedKey() {
		jsonifier::string returnString{};
		returnString.resize(16);
		std::mt19937_64 randomEngine{ static_cast<uint64_t>(sys_clock::now().time_since_epoch().count()) };
		for (uint64_t x = 0; x < 16; ++x) {
			returnString.at(x) = static_cast<char>((static_cast<double>(randomEngine()) / static_cast<double>(randomEngine.max())) * 255.0f);
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
		std::this_thread::sleep_for(nanoseconds{ ns });
#endif
		return true;
	}	

	jsonifier::string getTimeAndDate() {
		std::time_t time = std::time({});
		time			 = std::chrono::current_zone()->to_sys(std::chrono::local_time<std::chrono::seconds>(std::chrono::seconds{ time })).time_since_epoch().count();
		char timeString[std::size("yyyy-mm-ddThh:mm:ss")];
		tm timeNew{ getTime(time) };
		std::strftime(std::data(timeString), std::size(timeString), "%FT%T", &timeNew);
		return timeString;
	}
};
