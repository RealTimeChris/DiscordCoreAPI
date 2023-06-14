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
/// FoundationEntities.cpp - Source file for the foundation entities.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file FoundationEntities.cpp

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
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Etf.hpp>

namespace DiscordCoreInternal {

	WebSocketResumeData::operator EtfSerializer() {
		EtfSerializer data{};
		data["op"] = 6;
		data["d"]["seq"] = lastNumberReceived;
		data["d"]["session_id"] = sessionId;
		data["d"]["token"] = botToken;
		return data;
	}

	WebSocketIdentifyData::operator EtfSerializer() {
		EtfSerializer serializer{};
		serializer["d"]["intents"] = intents;
		serializer["d"]["large_threshold"] = 250;
		for (auto& value: presence.activities) {
			EtfSerializer serializer01{};
			if (value.url != "") {
				serializer01["url"] = value.url;
			}
			serializer01["name"] = value.name;
			serializer01["type"] = value.type;
			serializer["d"]["presence"]["activities"].emplaceBack(serializer01);
		}
		serializer["d"]["presence"]["afk"] = presence.afk;
		if (presence.since != 0) {
			serializer["d"]["presence"]["since"] = presence.since;
		}
		serializer["d"]["presence"]["status"] = presence.status;
		serializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		serializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		serializer["d"]["properties"]["os"] = "Windows";
#elif __linux__
		serializer["d"]["properties"]["os"] = "Linux";
#endif
		serializer["d"]["shard"].emplaceBack(shard[0]);
		serializer["d"]["shard"].emplaceBack(shard[1]);
		serializer["d"]["token"] = botToken;
		serializer["op"] = 2;
		return serializer;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadData&& other) noexcept {
		if (this != &other) {
			headersToInsert = std::move(other.headersToInsert);
			thisWorkerId.store(thisWorkerId.load());
			relativePath = std::move(other.relativePath);
			callStack = std::move(other.callStack);
			workloadClass = other.workloadClass;
			baseUrl = std::move(other.baseUrl);
			content = std::move(other.content);
			workloadType = other.workloadType;
			payloadType = other.payloadType;
		}
		return *this;
	}

	HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadData&& other) noexcept {
		*this = std::move(other);
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadType type) noexcept {
		if (!HttpsWorkloadData::workloadIdsExternal.contains(type)) {
			HttpsWorkloadData::workloadIdsExternal[type] = std::make_unique<std::atomic_int64_t>();
			HttpsWorkloadData::workloadIdsInternal[type] = std::make_unique<std::atomic_int64_t>();
		}
		thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(type));
		workloadType = type;
		return *this;
	}

	HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadType type) noexcept {
		*this = type;
	}

	int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) noexcept {
		int64_t value{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load() };
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(value + 1);
		return value;
	}

	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};

}

namespace DiscordCoreAPI {

	std::string Snowflake::getCreatedAtTimeStamp(TimeFormat timeFormat) {
		TimeStamp timeStamp{ (id >> 22) + 1420070400000, timeFormat };
		return timeStamp.operator std::string();
	}

	GuildMemberData& GuildMemberData::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			permissions = std::move(other.permissions);
			joinedAt = std::move(other.joinedAt);
			avatar = std::move(other.avatar);
			roles = std::move(other.roles);
			flags = std::move(other.flags);
			nick = std::move(other.nick);
			guildId = other.guildId;
			user.id = other.user.id;
		}
		return *this;
	}

	UpdatePresenceData::operator DiscordCoreInternal::EtfSerializer() {
		DiscordCoreInternal::EtfSerializer data{};
		data["op"] = 3;
		for (auto& value: activities) {
			DiscordCoreInternal::EtfSerializer newData{};
			if (value.url != "") {
				newData["url"] = std::string{ value.url };
			}
			newData["name"] = std::string{ value.name };
			newData["type"] = value.type;
			data["d"]["activities"].emplaceBack(newData);
		}
		data["status"] = status;
		if (since != 0) {
			data["since"] = since;
		}
		data["afk"] = afk;
		return data;
	}

	UpdateVoiceStateData::operator DiscordCoreInternal::EtfSerializer() {
		DiscordCoreInternal::EtfSerializer data{};
		data["op"] = 4;
		if (channelId == 0) {
			data["d"]["channel_id"] = DiscordCoreInternal::JsonType::Null;
		} else {
			data["d"]["channel_id"] = channelId.operator std::string();
		}
		data["d"]["self_deaf"] = selfDeaf;
		data["d"]["self_mute"] = selfMute;
		data["d"]["guild_id"] = guildId.operator std::string();
		return data;
	}

	UpdateVoiceStateDataDC::operator DiscordCoreInternal::EtfSerializer() {
		DiscordCoreInternal::EtfSerializer data{};
		data["op"] = 4;
		data["d"]["channel_id"] = DiscordCoreInternal::JsonType::Null;
		data["d"]["self_deaf"] = selfDeaf;
		data["d"]["self_mute"] = selfMute;
		data["d"]["guild_id"] = guildId.operator std::string();
		return data;
	}

	std::string GuildMemberData::getAvatarUrl() noexcept {
		if (avatar.operator std::string() != "") {
			std::string stringNew{ "https://cdn.discordapp.com/" };
			stringNew += "guilds/" + guildId + "/users/" + user.id + "/avatars/" + avatar.operator std::string();
			return stringNew;
		} else {
			return getUserData().getAvatarUrl();
		}
	}

	UserData GuildMemberData::getUserData() {
		if (user.id.operator uint64_t() != 0) {
			return Users::getCachedUser(GetUserData{ .userId = user.id });
		} else {
			return {};
		}
	}

	VoiceConnection* GuildData::connectToVoice(const Snowflake guildMemberId, const Snowflake channelId, bool selfDeaf, bool selfMute,
		StreamInfo streamInfoNew) {
		if (DiscordCoreClient::getVoiceConnection(id) && DiscordCoreClient::getVoiceConnection(id)->areWeConnected()) {
			voiceConnection = DiscordCoreClient::getVoiceConnection(id);
			return voiceConnection;
		} else if (static_cast<Snowflake>(guildMemberId) != 0 || static_cast<Snowflake>(channelId) != 0) {
			Snowflake channelId{};
			if (static_cast<Snowflake>(guildMemberId) != 0) {
				VoiceStateDataLight dataLight{};
				dataLight.guildId = id;
				dataLight.id = guildMemberId;
				GuildMemberData getData{};
				getData.guildId = id;
				getData.user.id = guildMemberId;
				auto voiceStateData = GuildMembers::getVoiceStateData(getData);
				if (voiceStateData.channelId != 0) {
					channelId = voiceStateData.channelId;
				}
			} else {
				channelId = channelId;
			}
			uint64_t theShardId{ (id.operator uint64_t() >> 22) % discordCoreClient->configManager.getTotalShardCount() };
			auto theBaseSocketAgentIndex{ static_cast<int32_t>(floor(static_cast<double>(theShardId) /
				static_cast<double>(discordCoreClient->configManager.getTotalShardCount()) * discordCoreClient->baseSocketAgentsMap.size())) };
			VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = theShardId;
			voiceConnectInitData.streamInfo = streamInfoNew;
			voiceConnectInitData.channelId = channelId;
			voiceConnectInitData.guildId = id;
			voiceConnectInitData.userId = discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			StopWatch stopWatch{ 10000ms };
			auto voiceConnection = DiscordCoreClient::getVoiceConnection(id);
			voiceConnection->connect(voiceConnectInitData);
			while (voiceConnection->areWeConnected()) {
				std::this_thread::sleep_for(1ms);
			}
			voiceConnection = voiceConnection;
			return voiceConnection;
		} else {
			return nullptr;
		}
	}

	std::string GuildData::getIconUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "icons/" + id + "/" + icon.operator std::string() + ".png";
		return stringNew;
	}

	bool GuildData::areWeConnected() {
		return discordCoreClient->getVoiceConnection(id)->areWeConnected();
	}

	void GuildData::disconnect() {
		if (DiscordCoreClient::getVoiceConnection(id)) {
			DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData> data{};
			data.excludedKeys.emplace("t");
			data.excludedKeys.emplace("s");
			data.d.channelId = 0;
			data.d.selfDeaf = false;
			data.d.selfMute = false;
			data.d.guildId = id;
			discordCoreClient->getBotUser().updateVoiceStatus(data.d);
			DiscordCoreClient::getVoiceConnection(id)->disconnect();
			voiceConnection = nullptr;
		}
	}

	bool operator==(const ApplicationCommandOptionData& lhs, const ApplicationCommandOptionData& rhs) {
		if (lhs.autocomplete != rhs.autocomplete) {
			return false;
		}
		if (lhs.channelTypes != rhs.channelTypes) {
			return false;
		}
		if (lhs.description != rhs.description) {
			return false;
		}
		if (lhs.descriptionLocalizations != rhs.descriptionLocalizations) {
			return false;
		}
		if (lhs.maxValue != rhs.maxValue) {
			return false;
		}
		if (lhs.minValue != rhs.minValue) {
			return false;
		}
		if (lhs.name != rhs.name) {
			return false;
		}
		if (lhs.nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (lhs.options.size() != rhs.options.size()) {
			return false;
		}
		if (lhs.required != rhs.required) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		if (lhs.choices.size() != rhs.choices.size()) {
			return false;
		}
		for (int32_t x = 0; x < rhs.choices.size(); ++x) {
			if (lhs.choices[x] != rhs.choices[x]) {
			}
		}
		return true;
	}

	EmbedData& EmbedData::setAuthor(const std::string& authorName, const std::string& authorAvatarUrl) {
		author.name = authorName;
		author.iconUrl = authorAvatarUrl;
		return *this;
	}

	EmbedData& EmbedData::setFooter(const std::string& footerText, const std::string& footerIconUrlText) {
		footer.text = footerText;
		footer.iconUrl = footerIconUrlText;
		return *this;
	}

	EmbedData& EmbedData::setTimeStamp(const std::string& timeStampNew) {
		timeStamp = timeStampNew;
		return *this;
	}

	EmbedData& EmbedData::addField(const std::string& name, const std::string& value, bool Inline) {
		EmbedFieldData field{};
		field.Inline = Inline;
		field.value = value;
		field.name = name;
		fields.emplace_back(field);
		return *this;
	}

	void EmbedImageData::generateExcludedKeys() noexcept {
		if (width == 0) {
			excludedKeys.emplace("width");
		}
		if (height == 0) {
			excludedKeys.emplace("height");
		}
		if (proxyUrl == "") {
			excludedKeys.emplace("proxy_url");
		}
	}

	void EmbedData::generateExcludedKeys() noexcept {
		if (fields.size() == 0) {
			excludedKeys.emplace("fields");
		}
		if (url == "") {
			excludedKeys.emplace("url");
		}
		if (hexColorValue == 0) {
			excludedKeys.emplace("color");
		}
		if (title == "") {
			excludedKeys.emplace("title");
		}
		if (description == "") {
			excludedKeys.emplace("description");
		}
		if (type == "") {
			excludedKeys.emplace("type");
		}
		if (author.iconUrl == "" && author.name == "" && author.proxyIconUrl == "" && author.url == "") {
			excludedKeys.emplace("author");
		}
		if (video.proxyUrl == "" && video.height == 0 && video.width == 0 && video.url == "") {
			excludedKeys.emplace("video");
		}
		if (footer.iconUrl == "" && footer.proxyIconUrl == "" && footer.text == "") {
			excludedKeys.emplace("footer");
		}
		if (image.height == 0 && image.width == 0 && image.proxyUrl == "" && image.url == "") {
			excludedKeys.emplace("image");
		}
		if (provider.name == "" && provider.url == "") {
			excludedKeys.emplace("provider");
		}
		if (thumbnail.height == 0 && thumbnail.width == 0 && thumbnail.proxyUrl == "" && thumbnail.url == "") {
			excludedKeys.emplace("thumbnail");
		}
		if (timeStamp == "") {
			excludedKeys.emplace("timestamp");
		}
	}

	EmbedData& EmbedData::setDescription(const std::string& descriptionNew) {
		description = descriptionNew;
		return *this;
	}

	EmbedData& EmbedData::setColor(const std::string& hexColorValueNew) {
		hexColorValue = std::stoul(hexColorValueNew, nullptr, 16);
		return *this;
	}

	EmbedData& EmbedData::setThumbnail(const std::string& thumbnailUrl) {
		thumbnail.url = thumbnailUrl;
		return *this;
	}

	EmbedData& EmbedData::setTitle(const std::string& titleNew) {
		title = titleNew;
		return *this;
	}

	EmbedData& EmbedData::setImage(const std::string& imageUrl) {
		image.url = imageUrl;
		return *this;
	}

	GuildScheduledEventDataVector::operator std::vector<GuildScheduledEventData>() {
		return guildScheduledEvents;
	}

	WebHookDataVector::operator std::vector<WebHookData>() {
		return webHooks;
	}

	auto AuditLogData::getAuditLogData(const Snowflake userIdOfChanger, AuditLogEvent auditLogType) {
		for (auto& value: auditLogEntries) {
			if (value.id == userIdOfChanger && value.actionType == auditLogType) {
				return value;
			}
		}
		return AuditLogEntryData();
	}

	auto AuditLogData::getAuditLogData(AuditLogEvent auditLogType, const Snowflake userIdOfTarget) {
		for (auto& value: auditLogEntries) {
			if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
				return value;
			}
		}
		return AuditLogEntryData();
	}

	YouTubeFormatVector::operator std::vector<YouTubeFormat>() {
		return formats;
	}

	void PartialEmojiData::generateExcludedKeys() noexcept {
		if (name == "") {
			excludedKeys.emplace("name");
		}
		if (id == 0) {
			excludedKeys.emplace("id");
		}
	}

	void EmojiData::generateExcludedKeys() noexcept {
		if (id == 0) {
			excludedKeys.emplace("id");
		}
		if (managed == false) {
			excludedKeys.emplace("managed");
		}
		if (name == "") {
			excludedKeys.emplace("name");
		}
		if (roles.size() == 0) {
			excludedKeys.emplace("roles");
		}
		if (unicodeName.size() == 0) {
			excludedKeys.emplace("unicode_name");
		}
	}

	SerializerValue CommandData::getCommandArguments() {
		return optionsArgs;
	}

	bool operator==(const ApplicationCommandData& lhs, const ApplicationCommandData& rhs) {
		if (lhs.description != rhs.description) {
			return false;
		}
		if (lhs.name != rhs.name) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		if (lhs.options.size() != rhs.options.size()) {
			return false;
		}
		for (uint64_t x = 0; x < lhs.options.size(); ++x) {
			if (lhs.options[x] != rhs.options[x]) {
				return false;
			}
		}
		return true;
	}

	std::string UserData::getAvatarUrl() {
		std::string stringNew{ "https://cdn.discordapp.com/avatars/" + id + "/" + avatar.operator std::string() };
		return static_cast<std::string>(stringNew);
	}

	VoiceStateDataLight GuildMemberData::getVoiceStateData() noexcept {
		return GuildMembers::getVoiceStateData(*this);
	}

	bool operator==(const ApplicationCommandOptionChoiceData& lhs, const ApplicationCommandOptionChoiceData& rhs) {
		if (lhs.name != rhs.name) {
			return false;
		}
		if (lhs.nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		if (lhs.value != rhs.value) {
			return false;
		}
		return true;
	}

	InputEventData& InputEventData::operator=(InputEventData&& other) noexcept {
		if (this != &other) {
			*interactionData = std::move(*other.interactionData);
			responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData&& other) noexcept {
		*this = std::move(other);
	}

	InputEventData& InputEventData::operator=(const InputEventData& other) noexcept {
		if (this != &other) {
			*interactionData = *other.interactionData;
			responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(const InputEventData& other) noexcept {
		*this = other;
	}

	InputEventData& InputEventData::operator=(const InteractionData& other) noexcept {
		*interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(const InteractionData& interactionData) noexcept {
		*this = interactionData;
	}

	UserData InputEventData::getUserData() const {
		UserData returnData{};
		if (interactionData->member.user.id != 0) {
			returnData.id = interactionData->member.user.id;
		} else {
			returnData.id = interactionData->user.id;
		}
		returnData = Users::getCachedUser({ .userId = returnData.id });
		return returnData;
	}

	ChannelData InputEventData::getChannelData() const {
		if (interactionData->channelId != 0) {
			return Channels::getCachedChannel({ interactionData->channelId });
		} else {
			return Channels::getCachedChannel({ interactionData->channel.id });
		}
	}

	GuildMemberData InputEventData::getGuildMemberData() const {
		GuildMemberData returnData{ interactionData->member };
		returnData = GuildMembers::getCachedGuildMember({ .guildMemberId = returnData.user.id, .guildId = interactionData->guildId });
		return returnData;
	}

	void InteractionCallbackData::generateExcludedKeys() noexcept {
		if (allowedMentions.parse.size() == 0 && allowedMentions.roles.size() == 0 && allowedMentions.users.size() == 0) {
			excludedKeys.emplace("allowed_mentions");
		}
		for (auto& value: components) {
			for (auto& value02: value.components) {
				value02.generateExcludedKeys();
			}
		}
		for (auto& value: embeds) {
			value.generateExcludedKeys();
		}
		if (attachments.size() == 0) {
			excludedKeys.emplace("attachments");
		}
		if (choices.size() == 0) {
			excludedKeys.emplace("choices");
		}
		if (flags == 0) {
			excludedKeys.emplace("flags");
		}
		if (content == "") {
			excludedKeys.emplace("content");
		}
		if (customId == "") {
			excludedKeys.emplace("custom_id");
		}
		if (files.size() == 0) {
			excludedKeys.emplace("files");
		}
		if (title == "") {
			excludedKeys.emplace("title");
		}
	}

	void ComponentData::generateExcludedKeys() noexcept {
		switch (type) {
			case ComponentType::Button: {
				if (customId == "") {
					excludedKeys.emplace("custom_id");
				}
				if (emoji.name == "") {
					excludedKeys.emplace("emoji");
				} else {
					emoji.generateExcludedKeys();
				}
				if (label == "") {
					excludedKeys.emplace("label");
				}
				if (placeholder == "") {
					excludedKeys.emplace("placeholder");
				}
				if (url == "") {
					excludedKeys.emplace("url");
				}
				excludedKeys.emplace("options");
				excludedKeys.emplace("channel_types");
				excludedKeys.emplace("placeholder");
				excludedKeys.emplace("min_values");
				excludedKeys.emplace("max_values");
				excludedKeys.emplace("min_length");
				excludedKeys.emplace("max_length");
				excludedKeys.emplace("required");
				excludedKeys.emplace("value");
				excludedKeys.emplace("title");
				break;
			}
			case ComponentType::Text_Input: {
				if (customId == "") {
					excludedKeys.emplace("custom_id");
				}
				if (label == "") {
					excludedKeys.emplace("label");
				}
				if (maxLength == 0) {
					excludedKeys.emplace("max_length");
				}
				if (minLength == 0) {
					excludedKeys.emplace("min_length");
				}
				if (placeholder == "") {
					excludedKeys.emplace("placeholder");
				}
				if (title == "") {
					excludedKeys.emplace("title");
				}
				if (value == "") {
					excludedKeys.emplace("value");
				}
				excludedKeys.emplace("style");
				excludedKeys.emplace("label");
				excludedKeys.emplace("emoji");
				excludedKeys.emplace("url");
				excludedKeys.emplace("min_length");
				excludedKeys.emplace("max_length");
				excludedKeys.emplace("required");
				excludedKeys.emplace("title");
				break;
			}
			default: {
				if (customId == "") {
					excludedKeys.emplace("custom_id");
				}
				if (channelTypes.size() == 0) {
					excludedKeys.emplace("channel_types");
				}
				if (label == "") {
					excludedKeys.emplace("label");
				}
				if (maxValues == 0) {
					excludedKeys.emplace("max_values");
				}
				if (minValues == 0) {
					excludedKeys.emplace("min_values");
				}
				if (options.size() == 0) {
					excludedKeys.emplace("options");
				}
				if (placeholder == "") {
					excludedKeys.emplace("placeholder");
				}
				excludedKeys.emplace("style");
				excludedKeys.emplace("label");
				excludedKeys.emplace("emoji");
				excludedKeys.emplace("url");
				excludedKeys.emplace("min_length");
				excludedKeys.emplace("max_length");
				excludedKeys.emplace("required");
				excludedKeys.emplace("value");
				excludedKeys.emplace("title");
				excludedKeys.emplace("placeholer");
				break;
			}
		}
	}

	GuildData InputEventData::getGuildData() const {
		GuildData returnData{};
		returnData.id = interactionData->guildId;
		returnData = Guilds::getCachedGuild({ .guildId = interactionData->guildId });
		return returnData;
	}

	InteractionData InputEventData::getInteractionData() const {
		return *interactionData;
	}

	MessageData InputEventData::getMessageData() const {
		return interactionData->message;
	}

	RespondToInputEventData::operator InteractionCallbackData() const noexcept {
		InteractionCallbackData returnData{};
		returnData.allowedMentions = allowedMentions;
		returnData.components = components;
		returnData.customId = customId;
		returnData.choices = choices;
		returnData.content = content;
		returnData.embeds = embeds;
		returnData.files = files;
		returnData.flags = flags;
		returnData.title = title;
		returnData.tts = tts;
		return returnData;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InteractionData& dataPackage) {
		applicationId = dataPackage.applicationId;
		interactionToken = dataPackage.token;
		messageId = dataPackage.message.id;
		channelId = dataPackage.channelId;
		interactionId = dataPackage.id;
		eventType = dataPackage.type;
		return *this;
	};

	RespondToInputEventData::RespondToInputEventData(const InteractionData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InputEventData& dataPackage) {
		applicationId = dataPackage.getInteractionData().applicationId;
		interactionToken = dataPackage.getInteractionData().token;
		interactionId = dataPackage.getInteractionData().id;
		channelId = dataPackage.interactionData->channelId;
		messageId = dataPackage.getMessageData().id;
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(const InputEventData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, const std::string& customIdNew,
		std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type,
		std::vector<ChannelType> channelTypes) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew,
		const std::string& titleNew, const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
		const std::string& label, const std::string& placeholder) {
		title = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::Text_Input;
				component.customId = customIdNew;
				component.style = static_cast<int32_t>(inputStyle);
				component.title = titleNew;
				component.maxLength = maxLength;
				component.minLength = minLength;
				component.label = label;
				component.required = required;
				component.placeholder = placeholder;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addFile(File theFile) {
		files.emplace_back(theFile);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addAllowedMentions(AllowedMentionsData dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setResponseType(InputEventResponseType typeNew) {
		type = typeNew;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addComponentRow(ActionRowData dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addMessageEmbed(EmbedData dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addContent(const std::string& dataPackage) {
		content = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(DiscordCoreInternal::EtfSerializer value, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name = theName;
		switch (value.getType()) {
			case DiscordCoreInternal::JsonType::String: {
				choiceData.type = DiscordCoreInternal::JsonType::String;
				break;
			}
			case DiscordCoreInternal::JsonType::Float: {
				choiceData.type = DiscordCoreInternal::JsonType::Float;
				break;
			}
			case DiscordCoreInternal::JsonType::Uint: {
				choiceData.type = DiscordCoreInternal::JsonType::Uint;
				break;
			}
			case DiscordCoreInternal::JsonType::Int: {
				choiceData.type = DiscordCoreInternal::JsonType::Int;
				break;
			}
			case DiscordCoreInternal::JsonType::Bool: {
				choiceData.type = DiscordCoreInternal::JsonType::Bool;
				break;
			}
			case DiscordCoreInternal::JsonType::Null:
				[[fallthrough]];
			case DiscordCoreInternal::JsonType::Array:
				[[fallthrough]];
			case DiscordCoreInternal::JsonType::Object: {
				break;
			}
		}
		value.refreshString();
		choiceData.value = Jsonifier::RawJsonData{ value.operator std::string() };
		choices.emplace_back(choiceData);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const Snowflake targetUserIdNew) {
		targetUserId = targetUserIdNew;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type, std::vector<ChannelType> channelTypes) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label,
		const std::string& placeholder) {
		title = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::Text_Input;
				component.customId = customIdNew;
				component.style = static_cast<int32_t>(inputStyle);
				component.title = titleNew;
				component.maxLength = maxLength;
				component.minLength = minLength;
				component.label = label;
				component.required = required;
				component.placeholder = placeholder;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addFile(File theFile) {
		files.emplace_back(theFile);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addAllowedMentions(AllowedMentionsData dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addComponentRow(ActionRowData dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addMessageEmbed(EmbedData dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addContent(const std::string& dataPackage) {
		content = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	InteractionResponseData& InteractionResponseData::operator=(const RespondToInputEventData& other) {
		data.allowedMentions = other.allowedMentions;
		data.components = other.components;
		data.customId = other.customId;
		data.choices = other.choices;
		data.content = other.content;
		data.embeds = other.embeds;
		data.title = other.title;
		data.files = other.files;
		data.flags = other.flags;
		data.tts = other.tts;
		return *this;
	}

	InteractionResponseData::InteractionResponseData(const RespondToInputEventData& other) {
		*this = other;
	}

	void parseCommandDataOption(std::unordered_map<std::string, JsonStringValue>& values, ApplicationCommandInteractionDataOption& data) {
		JsonStringValue valueNew{};
		valueNew.value = data.value;
		values.insert_or_assign(data.name, valueNew);
		for (auto& value: data.options) {
			parseCommandDataOption(values, value);
		}
	}

	CommandData& CommandData::operator=(const CommandData& other) noexcept {
		subCommandGroupName = other.subCommandGroupName;
		subCommandName = other.subCommandName;
		commandName = other.commandName;
		optionsArgs = other.optionsArgs;
		eventData = other.eventData;
		return *this;
	}

	CommandData::CommandData(const CommandData& other) noexcept {
		*this = other;
	}

	CommandData::CommandData(InputEventData inputEventData) {
		if (inputEventData.interactionData->data.name != "") {
			commandName = inputEventData.interactionData->data.name;
		}
		auto newString = inputEventData.interactionData->data.targetId.operator std::string();
		if (inputEventData.interactionData->data.targetId != 0) {
			optionsArgs.values.emplace("target_id",
				JsonStringValue{ .type = DiscordCoreInternal::JsonType::String, .value = Jsonifier::RawJsonData{ newString } });
		} else if (inputEventData.interactionData->data.targetId != 0) {
			optionsArgs.values.emplace("target_id",
				JsonStringValue{ .type = DiscordCoreInternal::JsonType::String, .value = Jsonifier::RawJsonData{ newString } });
		}
		eventData = inputEventData;
		for (auto& value: eventData.interactionData->data.options) {
			JsonStringValue serializer{};
			optionsArgs.values.insert_or_assign(value.name, serializer);
			parseCommandDataOption(optionsArgs.values, value);
		}
		for (auto& value: inputEventData.interactionData->data.options) {
			if (value.type == ApplicationCommandOptionType::Sub_Command) {
				subCommandName = value.name;
			}
			if (value.type == ApplicationCommandOptionType::Sub_Command_Group) {
				subCommandGroupName = value.name;
			}
		}
	}

	InteractionData CommandData::getInteractionData() {
		return *eventData.interactionData;
	}

	GuildMemberData CommandData::getGuildMemberData() {
		return eventData.getGuildMemberData();
	}

	ChannelData CommandData::getChannelData() {
		return eventData.getChannelData();
	}

	MessageData CommandData::getMessageData() {
		return eventData.getMessageData();
	}

	GuildData CommandData::getGuildData() {
		return eventData.getGuildData();
	}

	UserData CommandData::getUserData() {
		auto returnData = eventData.getUserData();
		return returnData;
	}

	std::string CommandData::getCommandName() {
		return commandName;
	}

	std::string CommandData::getSubCommandName() {
		return subCommandName;
	}

	std::string CommandData::getSubCommandGroupName() {
		return subCommandGroupName;
	}

	InputEventData CommandData::getInputEventData() {
		return eventData;
	}

	BaseFunctionArguments::BaseFunctionArguments(CommandData commanddataNew, DiscordCoreClient* discordCoreClientNew) : CommandData(commanddataNew) {
		discordCoreClient = discordCoreClientNew;
	}

	MoveThroughMessagePagesData moveThroughMessagePages(const std::string& userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const std::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		uint32_t newCurrentPageIndex = currentPageIndex;
		StopWatch stopWatch{ Milliseconds{ waitForMaxMs } };
		auto createResponseData = std::make_unique<CreateInteractionResponseData>(originalEvent);
		createResponseData->generateExcludedKeys();
		auto interactionResponse = std::make_unique<RespondToInputEventData>(originalEvent);
		auto embedData = std::make_unique<EmbedData>();
		embedData->setColor("FEFEFE");
		embedData->setTitle("__**Permissions Issue:**__");
		embedData->setTimeStamp(getTimeAndDate());
		embedData->setDescription("Sorry, but that button can only be pressed by <@" + userID + ">!");
		createResponseData->addMessageEmbed(*embedData);
		createResponseData->setResponseType(InteractionCallbackType::Channel_Message_With_Source);
		if (messageEmbeds.size() > 0) {
			interactionResponse->addMessageEmbed(messageEmbeds[currentPageIndex]);
		}
		if (returnResult) {
			interactionResponse->addButton(false, "select", "Select", ButtonStyle::Success, "✅");
		}
		interactionResponse->addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
		interactionResponse->addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
		interactionResponse->addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
		interactionResponse->setResponseType(InputEventResponseType::Edit_Interaction_Response);
		originalEvent = InputEvents::respondToInputEventAsync(*interactionResponse).get();
		while (!stopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
			std::unique_ptr<ButtonCollector> button{ std::make_unique<ButtonCollector>(originalEvent) };
			std::vector<ButtonResponseData> buttonIntData{
				button->collectButtonData(false, waitForMaxMs, 1, *createResponseData, Snowflake{ stoull(userID) }).get()
			};
			std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
			if (buttonIntData.size() == 0 || buttonIntData.at(0).buttonId == "empty" || buttonIntData.at(0).buttonId == "exit") {
				std::unique_ptr<RespondToInputEventData> dataPackage02{ std::make_unique<RespondToInputEventData>(originalEvent) };
				if (buttonIntData.at(0).buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
					*dataPackage02 = RespondToInputEventData{ *interactionData };
				}

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				if (returnResult) {
					dataPackage02->addButton(true, "select", "Select", ButtonStyle::Success, "✅");
				}
				dataPackage02->addButton(true, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
				dataPackage02->addButton(true, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
				dataPackage02->addButton(true, "exit", "Exit", ButtonStyle::Danger, "❌");
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
					InputEvents::respondToInputEventAsync(*dataPackage02).get();
				}
				MoveThroughMessagePagesData dataPackage03{};
				dataPackage03.inputEventData = originalEvent;
				dataPackage03.buttonId = "exit";
				return dataPackage03;
			} else if (buttonIntData.at(0).buttonId == "forwards" || buttonIntData.at(0).buttonId == "backwards") {
				if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
					newCurrentPageIndex = 0;
				} else if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
					++newCurrentPageIndex;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex > 0)) {
					--newCurrentPageIndex;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex == 0)) {
					newCurrentPageIndex = static_cast<uint32_t>(messageEmbeds.size()) - 1;
				}
				interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
				auto dataPackage = RespondToInputEventData{ *interactionData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				if (returnResult) {
					dataPackage.addButton(false, "select", "Select", ButtonStyle::Success, "✅");
				}
				dataPackage.addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
				dataPackage.addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
				dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
				dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				InputEvents::respondToInputEventAsync(dataPackage).get();
			} else if (buttonIntData.at(0).buttonId == "select") {
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
					auto dataPackage = RespondToInputEventData{ *interactionData };
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
					if (returnResult) {
						dataPackage.addButton(false, "select", "Select", ButtonStyle::Success, "✅");
					}
					dataPackage.addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
					dataPackage.addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
					dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
					originalEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				}
				returnData.currentPageIndex = newCurrentPageIndex;
				returnData.inputEventData = originalEvent;
				returnData.buttonId = buttonIntData.at(0).buttonId;
				return returnData;
			}
		};
		return returnData;
	};

};