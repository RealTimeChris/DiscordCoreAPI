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
/// FoundationEntities.cpp - Source file for the foundation entities.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file FoundationEntities.cpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>

namespace DiscordCoreInternal {

	WebSocketResumeData::operator DiscordCoreAPI::Jsonifier() {
		DiscordCoreAPI::Jsonifier data{};
		data["op"] = 6;
		data["d"]["seq"] = this->lastNumberReceived;
		data["d"]["session_id"] = this->sessionId;
		data["d"]["token"] = this->botToken;
		return data;
	}

	WebSocketIdentifyData::operator DiscordCoreAPI::Jsonifier() {
		DiscordCoreAPI::Jsonifier serializer{};
		serializer["d"]["intents"] = this->intents;
		serializer["d"]["large_threshold"] = 250;
		for (auto& value: this->presence.activities) {
			DiscordCoreAPI::Jsonifier serializer01{};
			if (value.url != "") {
				serializer01["url"] = value.url;
			}
			serializer01["name"] = value.name;
			serializer01["type"] = value.type;
			serializer["d"]["presence"]["activities"].emplaceBack(serializer01);
		}
		serializer["d"]["presence"]["afk"] = this->presence.afk;
		if (this->presence.since != 0) {
			serializer["d"]["presence"]["since"] = this->presence.since;
		}
		switch (this->presence.status) {
			case DiscordCoreAPI::UpdatePresenceStatusTypes::online: {
				serializer["d"]["presence"]["status"] = "online";
				break;
			}
			case DiscordCoreAPI::UpdatePresenceStatusTypes::dnd: {
				serializer["d"]["presence"]["status"] = "dnd";
				break;
			}
			case DiscordCoreAPI::UpdatePresenceStatusTypes::invisible: {
				serializer["d"]["presence"]["status"] = "invisible";
				break;
			}
			case DiscordCoreAPI::UpdatePresenceStatusTypes::offline: {
				serializer["d"]["presence"]["status"] = "offline";
				break;
			}
			case DiscordCoreAPI::UpdatePresenceStatusTypes::idle: {
				serializer["d"]["presence"]["status"] = "idle";
				break;
			}
		}
		serializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		serializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		serializer["d"]["properties"]["os"] = "Windows";
#elif __linux__
		serializer["d"]["properties"]["os"] = "Linux";
#endif
		serializer["d"]["shard"].emplaceBack(this->currentShard);
		serializer["d"]["shard"].emplaceBack(this->numberOfShards);
		serializer["d"]["token"] = this->botToken;
		serializer["op"] = 2;
		return serializer;
	}

	VoiceSocketProtocolPayloadData::operator DiscordCoreAPI::Jsonifier() {
		DiscordCoreAPI::Jsonifier data{};
		data["op"] = 1;
		data["d"]["protocol"] = "udp";
		data["d"]["data"]["port"] = this->voicePort;
		data["d"]["data"]["mode"] = this->voiceEncryptionMode;
		std::string externalIpReal{};
		for (auto& value : this->externalIp) {
			externalIpReal.push_back(value);
		}
		data["d"]["data"]["address"] = externalIpReal;
		return data;
	}

	VoiceIdentifyData::operator DiscordCoreAPI::Jsonifier() {
		DiscordCoreAPI::Jsonifier data{};
		data["op"] = 0;
		data["d"]["session_id"] = this->connectionData.sessionId;
		data["d"]["token"] = this->connectionData.token;
		data["d"]["server_id"] = this->connectInitData.guildId;
		data["d"]["user_id"] = this->connectInitData.userId;
		return data;
	}

	SendSpeakingData::operator DiscordCoreAPI::Jsonifier() {
		DiscordCoreAPI::Jsonifier data{};
		data["op"] = 5;
		data["d"]["speaking"] = static_cast<int32_t>(this->type);
		data["d"]["delay"] = this->delay;
		data["d"]["ssrc"] = this->ssrc;
		return data;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadData&& other) noexcept {
		if (this != &other) {
			this->headersToInsert = std::move(other.headersToInsert);
			this->thisWorkerId.store(this->thisWorkerId.load());
			this->relativePath = std::move(other.relativePath);
			this->callStack = std::move(other.callStack);
			this->workloadClass = other.workloadClass;
			this->baseUrl = std::move(other.baseUrl);
			this->content = std::move(other.content);
			this->workloadType = other.workloadType;
			this->payloadType = other.payloadType;
		}
		return *this;
	}

	HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadData&& other) noexcept {
		*this = std::move(other);
	}

	HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadType type) noexcept {
		if (!HttpsWorkloadData::workloadIdsExternal.contains(type)) {
			std::unique_ptr<std::atomic_int64_t> integer{ std::make_unique<std::atomic_int64_t>() };
			std::unique_ptr<std::atomic_int64_t> integer02{ std::make_unique<std::atomic_int64_t>() };
			HttpsWorkloadData::workloadIdsExternal[type] = std::move(integer);
			HttpsWorkloadData::workloadIdsInternal[type] = std::move(integer02);
		}
		this->thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(type));
		this->workloadType = type;
	}

	int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) noexcept {
		int64_t value{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load() };
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(value + 1);
		return value;
	}

	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};

	HelloData::HelloData(simdjson::ondemand::value jsonObjectData) {
		this->heartbeatInterval = DiscordCoreAPI::getUint64(jsonObjectData, "heartbeat_interval");
	}

	WebSocketMessage::WebSocketMessage(simdjson::ondemand::value jsonObjectData) {
		this->op = DiscordCoreAPI::getUint32(jsonObjectData, "op");

		this->s = DiscordCoreAPI::getUint32(jsonObjectData, "s");

		this->t = DiscordCoreAPI::getString(jsonObjectData, "t");
	}

	InvalidSessionData::InvalidSessionData(simdjson::ondemand::value jsonObjectData) {
		this->d = DiscordCoreAPI::getBool(jsonObjectData, "d");
	}

	ReadyData::ReadyData(simdjson::ondemand::value jsonObjectData) {
		this->resumeGatewayUrl = DiscordCoreAPI::getString(jsonObjectData, "resume_gateway_url");
		this->resumeGatewayUrl = this->resumeGatewayUrl.substr(this->resumeGatewayUrl.find("wss://") + std::string{ "wss://" }.size());
		this->resumeGatewayUrl = this->resumeGatewayUrl.substr(0, this->resumeGatewayUrl.find("/"));

		this->sessionId = DiscordCoreAPI::getString(jsonObjectData, "session_id");

		this->v = DiscordCoreAPI::getUint32(jsonObjectData, "v");

		simdjson::ondemand::value user{};
		if (jsonObjectData["user"].get(user) == simdjson::error_code::SUCCESS) {
			this->user = DiscordCoreAPI::UserData{ user };
		}
	}
}

namespace DiscordCoreAPI {

	UpdatePresenceData::operator Jsonifier() {
		Jsonifier data{};
		data["op"] = 3;
		for (auto& value: this->activities) {
			Jsonifier dataNew{};
			if (value.url != "") {
				dataNew["url"] = std::string{ value.url };
			}
			dataNew["name"] = std::string{ value.name };
			dataNew["type"] = value.type;
			data["d"]["activities"].emplaceBack(dataNew);
		}
		switch (this->status) {
			case UpdatePresenceStatusTypes::online: {
				data["status"] = "online";
				break;
			}
			case UpdatePresenceStatusTypes::dnd: {
				data["status"] = "dnd";
				break;
			}
			case UpdatePresenceStatusTypes::invisible: {
				data["status"] = "invisible";
				break;
			}
			case UpdatePresenceStatusTypes::offline: {
				data["status"] = "offline";
				break;
			}
			case UpdatePresenceStatusTypes::idle: {
				data["status"] = "idle";
				break;
			}
		}
		if (this->since != 0) {
			data["since"] = this->since;
		}
		data["afk"] = this->afk;
		return data;
	}

	std::string DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
		TimeStamp timeStamp{ (this->id.operator size_t() >> 22) + 1420070400000, timeFormat };
		return timeStamp.operator std::string();
	}

	RoleTagsData::RoleTagsData(simdjson::ondemand::value jsonObjectData) {
		this->botId = getString(jsonObjectData, "bot_id");

		this->integrationId = getString(jsonObjectData, "integration_id");
	}

	UserData::UserData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");
		if (this->id == 0) {
			return;
		}

		int32_t newFlags{};
		newFlags = setBool(newFlags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		newFlags = setBool(newFlags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		newFlags = setBool(newFlags, UserFlags::System, getBool(jsonObjectData, "system"));

		newFlags = setBool(newFlags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		newFlags += getUint32(jsonObjectData, "public_flags");

		newFlags = setBool(newFlags, this->flags, true);

		this->flags = static_cast<UserFlags>(newFlags);

		this->userName = getString(jsonObjectData, "username");

		this->avatar = getString(jsonObjectData, "avatar");

		this->discriminator = getString(jsonObjectData, "discriminator");
	}

	std::string UserData::getAvatarUrl() {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "avatars/" + this->id + "/" + this->avatar.getIconHash();
		return stringNew;
	}

	AttachmentData ::AttachmentData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->filename = getString(jsonObjectData, "filename");

		this->contentType = getString(jsonObjectData, "content_type");

		this->ephemeral = getBool(jsonObjectData, "ephemeral");

		this->description = getString(jsonObjectData, "description");

		this->size = getUint32(jsonObjectData, "size");

		this->url = getString(jsonObjectData, "url");

		this->proxyUrl = getString(jsonObjectData, "proxy_url");

		this->width = getUint32(jsonObjectData, "width");

		this->height = getUint32(jsonObjectData, "height");
	}

	EmbedFooterData::EmbedFooterData(simdjson::ondemand::value jsonObjectData) {
		this->text = getString(jsonObjectData, "text");

		this->iconUrl = getString(jsonObjectData, "icon_url");

		this->proxyIconUrl = getString(jsonObjectData, "proxy_icon_url");
	}

	EmbedImageData::EmbedImageData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "url");

		this->proxyUrl = getString(jsonObjectData, "proxy_url");

		this->width = getUint32(jsonObjectData, "width");

		this->height = getUint32(jsonObjectData, "height");
	}

	EmbedThumbnailData::EmbedThumbnailData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "url");

		this->proxyUrl = getString(jsonObjectData, "proxy_url");

		this->width = getUint32(jsonObjectData, "width");

		this->height = getUint32(jsonObjectData, "height");
	}

	EmbedVideoData::EmbedVideoData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "url");

		this->proxyUrl = getString(jsonObjectData, "proxy_url");

		this->width = getUint32(jsonObjectData, "width");

		this->height = getUint32(jsonObjectData, "height");
	}

	EmbedProviderData::EmbedProviderData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "url");

		this->name = getString(jsonObjectData, "name");
	}

	EmbedAuthorData::EmbedAuthorData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "url");

		this->proxyIconUrl = getString(jsonObjectData, "proxy_icon_url");

		this->name = getString(jsonObjectData, "name");

		this->iconUrl = getString(jsonObjectData, "icon_url");
	}

	EmbedFieldData::EmbedFieldData(simdjson::ondemand::value jsonObjectData) {
		this->Inline = getBool(jsonObjectData, "inline");

		this->name = getString(jsonObjectData, "name");

		this->value = getString(jsonObjectData, "value");
	}

	EmbedData::EmbedData(simdjson::ondemand::value jsonObjectData) {
		this->title = getString(jsonObjectData, "title");

		this->type = getString(jsonObjectData, "type");

		this->description = getString(jsonObjectData, "description");

		this->url = getString(jsonObjectData, "url");

		this->timeStamp = getString(jsonObjectData, "timestamp");

		this->hexColorValue = getUint32(jsonObjectData, "color");

		simdjson::ondemand::value object{};
		if (getObject(object, "footer", jsonObjectData)) {
			EmbedFooterData data{ object };
			this->footer = std::move(data);
		}

		if (getObject(object, "image", jsonObjectData)) {
			EmbedImageData data{ object };
			this->image = std::move(data);
		}

		if (getObject(object, "provider", jsonObjectData)) {
			EmbedProviderData data{ object };
			this->provider = std::move(data);
		}

		if (getObject(object, "thumbnail", jsonObjectData)) {
			EmbedThumbnailData data{ object };
			this->thumbnail = std::move(data);
		}

		if (getObject(object, "video", jsonObjectData)) {
			EmbedVideoData data{ object };
			this->video = std::move(data);
		}

		if (getObject(object, "author", jsonObjectData)) {
			EmbedAuthorData data{ object };
			this->author = std::move(data);
		}

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "fields", jsonObjectData)) {
			this->fields.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmbedFieldData newData{ value.value() };
				this->fields.emplace_back(std::move(newData));
			}
		}
	}

	MessageReferenceData::MessageReferenceData(simdjson::ondemand::value jsonObjectData) {
		this->messageId = getId(jsonObjectData, "message_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->failIfNotExists = getBool(jsonObjectData, "fail_if_not_exists");
	}

	ThreadMetadataData::ThreadMetadataData(simdjson::ondemand::value jsonObjectData) {
		this->archived = getBool(jsonObjectData, "archived");

		this->invitable = getBool(jsonObjectData, "invitable");

		this->autoArchiveDuration = getUint32(jsonObjectData, "auto_archive_duration");

		this->archiveTimestamp = getString(jsonObjectData, "archive_timestamp");

		this->locked = getBool(jsonObjectData, "locked");
	}

	ThreadMemberData::ThreadMemberData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->userId = getId(jsonObjectData, "user_id");

		this->joinTimestamp = getString(jsonObjectData, "join_timestamp");

		this->flags = getUint32(jsonObjectData, "flags");
	}

	ThreadMemberDataVector::ThreadMemberDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					ThreadMemberData newData{ value.value() };
					this->threadMembers.emplace_back(std::move(newData));
				}
			}
		}
	}

	VoiceStateDataLight::VoiceStateDataLight(simdjson::ondemand::value jsonObjectData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->userId = getId(jsonObjectData, "user_id");
	}

	VoiceStateData::VoiceStateData(simdjson::ondemand::value jsonObjectData) {
		this->requestToSpeakTimestamp = getString(jsonObjectData, "request_to_speak_timestamp");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->selfStream = getBool(jsonObjectData, "self_stream");

		this->userId = getId(jsonObjectData, "user_id");

		this->selfVideo = getBool(jsonObjectData, "self_video");

		this->sessionId = getString(jsonObjectData, "session_id");

		this->selfDeaf = getBool(jsonObjectData, "self_deaf");

		this->selfMute = getBool(jsonObjectData, "self_mute");

		this->suppress = getBool(jsonObjectData, "suppress");

		this->deaf = getBool(jsonObjectData, "deaf");

		this->mute = getBool(jsonObjectData, "mute");
	}

	RoleData::RoleData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		std::stringstream stream{};
		stream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: stream.str()) {
			this->unicodeEmoji.emplace_back(value);
		}
		if (this->unicodeEmoji.size() > 3) {
			this->unicodeEmoji = static_cast<std::string>(this->unicodeEmoji).substr(1, this->unicodeEmoji.size() - 3);
		}

		this->guildId = getId(jsonObjectData, "guild_id");

		this->color = getUint32(jsonObjectData, "color");

		uint8_t newFlags{};
		newFlags = setBool(newFlags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		newFlags = setBool(newFlags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		newFlags = setBool(newFlags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		this->flags = static_cast<RoleFlags>(newFlags);

		this->position = getUint32(jsonObjectData, "position");

		this->permissions = getString(jsonObjectData, "permissions");
	}

	EmojiData::EmojiData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "roles", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				RoleData newData{ value.value() };
				this->roles.emplace_back(std::move(newData));
			}
		}

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}

		this->requireColons = getBool(jsonObjectData, "require_colons");

		this->managed = getBool(jsonObjectData, "managed");

		this->animated = getBool(jsonObjectData, "animated");

		this->available = getBool(jsonObjectData, "available");
	}

	PresenceUpdateData::PresenceUpdateData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		auto stringNew = getString(jsonObjectData, "status");

		if (stringNew == "online") {
			uint8_t newFlags = setBool(uint8_t{}, PresenceUpdateFlags::Status_Online, true);
			this->theStatus = static_cast<PresenceUpdateFlags>(newFlags);
		} else if (stringNew == "idle") {
			uint8_t newFlags = setBool(uint8_t{}, PresenceUpdateFlags::Status_Idle, true);
			this->theStatus = static_cast<PresenceUpdateFlags>(newFlags);
		} else if (stringNew == "dnd") {
			uint8_t newFlags = setBool(uint8_t{}, PresenceUpdateFlags::Status_Dnd, true);
			this->theStatus = static_cast<PresenceUpdateFlags>(newFlags);
		}

		parseObject(jsonObjectData, this->theStatus);
	}

	GuildMemberData& GuildMemberData::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			this->permissions = std::move(other.permissions);
			this->joinedAt = std::move(other.joinedAt);
			this->avatar = std::move(other.avatar);
			this->roles = std::move(other.roles);
			this->flags = std::move(other.flags);
			this->nick = std::move(other.nick);
			this->guildId = other.guildId;
			this->id = other.id;
		}
		return *this;
	}

	GuildMemberData::GuildMemberData(GuildMemberData&& other) noexcept {
		*this = std::move(other);
	}

	GuildMemberData& GuildMemberData::operator=(simdjson::ondemand::value jsonObjectData) {
		uint8_t newFlags{};
		newFlags = setBool(newFlags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		newFlags = setBool(newFlags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		newFlags = setBool(newFlags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		this->flags = static_cast<GuildMemberFlags>(newFlags);

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "roles", jsonObjectData)) {
			this->roles.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->roles.emplace_back(getId(value.value()));
			}
		}

		this->permissions = getString(jsonObjectData, "permissions");

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			UserData user{ object };
			this->id = user.id;
			Users::insertUser(std::move(user));
		}

		this->avatar = getString(jsonObjectData, "avatar");

		newFlags += getUint8(jsonObjectData, "flags");

		this->flags = static_cast<GuildMemberFlags>(newFlags);

		this->nick = getString(jsonObjectData, "nick");
		return *this;
	}

	GuildMemberData::GuildMemberData(simdjson::ondemand::value jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceStateDataLight GuildMemberData::getVoiceStateData() {
		VoiceStateDataLight dataToGet{};
		dataToGet.guildId = this->guildId;
		dataToGet.userId = this->id;
		if (GuildMembers::getVsCache().contains(dataToGet)) {
			return GuildMembers::getVsCache()[dataToGet];
		} else {
			return dataToGet;
		}
	}

	std::string GuildMemberData::getAvatarUrl() {
		if (this->avatar.getIconHash() != "") {
			std::string stringNew{ "https://cdn.discordapp.com/" };
			stringNew += "guilds/" + this->guildId + "/users/" + this->id + "/avatars/" + this->avatar.getIconHash();
			return stringNew;
		} else {
			return this->getUserData().getAvatarUrl();
		}
	}

	UserData GuildMemberData::getUserData() {
		if (this->id.operator size_t() != 0) {
			return Users::getCachedUser({ .userId = this->id });
		} else {
			return {};
		}
	}

	OverWriteData::OverWriteData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->allow = getUint64(jsonObjectData, "allow");

		this->deny = getUint64(jsonObjectData, "deny");

		this->type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	DefaultReactionData::DefaultReactionData(simdjson::ondemand::value jsonObjectData) {
		this->emojiId = getId(jsonObjectData, "emoji_id");

		this->emojiName = getString(jsonObjectData, "emoji_name");
	}

	ForumTagData::ForumTagData(simdjson::ondemand::value jsonObjectData) {
		this->emojiId = getId(jsonObjectData, "emoji_id");

		this->emojiName = getString(jsonObjectData, "emoji_name");

		this->id = getId(jsonObjectData, "id");

		this->moderated = getBool(jsonObjectData, "moderated");

		this->name = getString(jsonObjectData, "name");
	}

	ChannelData::ChannelData(simdjson::ondemand::value jsonObjectData) {
		uint8_t newFlags{};
		newFlags = setBool(newFlags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->defaultSortOrder = static_cast<SortOrderTypes>(getUint8(jsonObjectData, "default_sort_order"));

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->id = getId(jsonObjectData, "id");

		newFlags += getUint8(jsonObjectData, "flags");

		this->flags = static_cast<ChannelFlags>(newFlags);

		this->parentId = getId(jsonObjectData, "parent_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "permission_overwrites", jsonObjectData)) {
			this->permissionOverwrites.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				OverWriteData dataNew{ value.value() };
				this->permissionOverwrites.emplace_back(std::move(dataNew));
			}
		}

		this->name = getString(jsonObjectData, "name");
	}

	ActiveThreadsData::ActiveThreadsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "threads", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "members", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ThreadMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		this->hasMore = getBool(jsonObjectData, "has_more");
	}

	ArchivedThreadsData::ArchivedThreadsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};

		if (getArray(arrayValue, "threads", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}

		;
		if (getArray(arrayValue, "members", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ThreadMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		this->hasMore = getBool(jsonObjectData, "has_more");
	}

	TriggerMetaData::TriggerMetaData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};

		if (getArray(arrayValue, "keyword_filter", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->keywordFilter.emplace_back(value.get_string().value().data());
			}
		}

		if (getArray(arrayValue, "presets", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->presets.emplace_back(static_cast<KeywordPresetType>(value.get_uint64().value()));
			}
		}

		if (getArray(arrayValue, "allow_list", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->allowList.emplace_back(value.get_string().value());
			}
		}

		this->mentionTotalLimit = getUint32(jsonObjectData, "mention_total_limit");
	}

	ActionMetaData::ActionMetaData(simdjson::ondemand::value jsonObjectData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->durationSeconds = getUint64(jsonObjectData, "duration_seconds");
	}

	ActionData::ActionData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "metadata", jsonObjectData)) {
			this->metadata = ActionMetaData{ object };
		}

		this->type = static_cast<ActionType>(getUint64(jsonObjectData, "type"));
	}

	AutoModerationRuleData::AutoModerationRuleData(simdjson::ondemand::value jsonObjectData) {
		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		this->enabled = getBool(jsonObjectData, "enabled");

		this->triggerType = static_cast<TriggerType>(getUint8(jsonObjectData, "trigger_type"));

		this->eventType = static_cast<EventType>(getUint8(jsonObjectData, "event_type"));

		this->creatorId = getId(jsonObjectData, "creator_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "actions", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ActionData newData{ value.value() };
				this->actions.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "exempt_roles", jsonObjectData)) {
			this->exemptRoles.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->exemptRoles.emplace_back(strtoull(std::string{ value.get_string().take_value() }));
			}
		}

		if (getArray(arrayValue, "exempt_channels", jsonObjectData)) {
			this->exemptChannels.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->exemptChannels.emplace_back(strtoull(std::string{ value.get_string().take_value() }));
			}
		}

		simdjson::ondemand::value object{};
		if (getObject(object, "trigger_metadata", jsonObjectData)) {
			this->triggerMetaData = TriggerMetaData{ object };
		}

		this->guildId = getId(jsonObjectData, "guild_id");
	}

	ApplicationCommandPermissionData::ApplicationCommandPermissionData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->permission = getBool(jsonObjectData, "permission");

		this->type = static_cast<ApplicationCommandPermissionType>(getUint8(jsonObjectData, "type"));
	}

	GuildApplicationCommandPermissionsData::GuildApplicationCommandPermissionsData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->applicationId = getId(jsonObjectData, "application_id");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "permissions", jsonObjectData)) {
			this->permissions.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ApplicationCommandPermissionData newData{ value.value() };
				this->permissions.emplace_back(std::move(newData));
			}
		}
	}

	GuildApplicationCommandPermissionsDataVector::GuildApplicationCommandPermissionsDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildApplicationCommandPermissionsData newData{ value.value() };
					this->guildApplicationCommandPermissionss.emplace_back(std::move(newData));
				}
			}
		}
	}

	EmojiDataVector::EmojiDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					EmojiData newData{ value.value() };
					this->theEmojis.emplace_back(std::move(newData));
				}
			}
		}
	}

	ReactionData::ReactionData(simdjson::ondemand::value jsonObjectData) {
		this->count = getUint32(jsonObjectData, "count");

		this->me = getBool(jsonObjectData, "me");

		simdjson::ondemand::value object{};
		if (getObject(object, "emoji", jsonObjectData)) {
			this->emoji = EmojiData{ object };
		}

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->userId = getId(jsonObjectData, "user_id");

		this->messageId = getId(jsonObjectData, "message_id");

		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
		}
	}

	VoiceRegionData::VoiceRegionData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->custom = getBool(jsonObjectData, "custom");

		this->deprecated = getBool(jsonObjectData, "deprecated");

		this->optimal = getBool(jsonObjectData, "optimal");

		this->name = getString(jsonObjectData, "name");
	}

	VoiceRegionDataVector::VoiceRegionDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					VoiceRegionData newData{ value.value() };
					this->voiceRegions.emplace_back(std::move(newData));
				}
			}
		}
	}

	MessageActivityData::MessageActivityData(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<MessageActivityType>(getUint8(jsonObjectData, "type"));

		this->partyId = getString(jsonObjectData, "party_id");
	}

	BanData::BanData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}

		this->reason = getString(jsonObjectData, "reason");
	}

	BanDataVector::BanDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					BanData newData{ value.value() };
					this->bans.emplace_back(std::move(newData));
				}
			}
		}
	}

	TeamMembersObjectData::TeamMembersObjectData(simdjson::ondemand::value jsonObjectData) {
		this->membershipState = getUint32(jsonObjectData, "membership_state");

		this->teamId = getId(jsonObjectData, "team_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "permissions", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->permissions.emplace_back(Permissions{ value.get_string().value().data() });
			}
		}

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}
	}

	TeamObjectData::TeamObjectData(simdjson::ondemand::value jsonObjectData) {
		this->icon = getString(jsonObjectData, "icon");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "members", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				TeamMembersObjectData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		this->ownerUserId = getId(jsonObjectData, "owner_user_id");
	}

	InstallParamsData::InstallParamsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "scopes", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->scopes.emplace_back(Permissions{ value.get_string().value().data() });
			}
		}

		this->permissions = getString(jsonObjectData, "name");
	}

	ApplicationData::ApplicationData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "params", jsonObjectData)) {
			this->params = InstallParamsData{ object };
		}

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "tags", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->tags.emplace_back(value.get_string().value().data());
			}
		}

		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		this->icon = getString(jsonObjectData, "icon");

		this->description = getString(jsonObjectData, "description");

		if (getArray(arrayValue, "rpc_origins", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->rpcOrigins.emplace_back(value.get_string().value().data());
			}
		}

		this->botPublic = getBool(jsonObjectData, "bot_public");

		this->botRequireCodeGrant = getBool(jsonObjectData, "bot_require_code_grant");

		this->termsOfServiceUrl = getString(jsonObjectData, "terms_of_service_url");

		this->privacyPolicyUrl = getString(jsonObjectData, "privacy_policy_url");

		if (getObject(object, "owner", jsonObjectData)) {
			this->owner = UserData{ object };
		}

		if (getObject(object, "team", jsonObjectData)) {
			this->team = TeamObjectData{ object };
		}

		this->summary = getString(jsonObjectData, "summary");

		this->verifyKey = getString(jsonObjectData, "verify_key");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->primarySkuId = getString(jsonObjectData, "primary_sku_id");

		this->slug = getString(jsonObjectData, "slug");

		this->coverImage = getString(jsonObjectData, "cover_image");

		this->flags = static_cast<ApplicationFlags>(getUint8(jsonObjectData, "flags"));
	}

	AuthorizationInfoData::AuthorizationInfoData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "application", jsonObjectData)) {
			this->application = ApplicationData{ object };
		}

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "features", jsonObjectData)) {
			this->scopes.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->scopes.emplace_back(std::string{ value.get_string().take_value() });
			}
		}

		this->expires = getString(jsonObjectData, "expires");
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}
	}

	AccountData::AccountData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");
	}

	GuildWidgetData::GuildWidgetData(simdjson::ondemand::value jsonObjectData) {
		this->enabled = getBool(jsonObjectData, "enabled");

		this->channelId = getId(jsonObjectData, "channel_id");
	};

	GuildWidgetImageData::GuildWidgetImageData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "widget_image");
	}

	IntegrationData::IntegrationData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		this->type = getString(jsonObjectData, "type");

		this->enabled = getBool(jsonObjectData, "enabled");

		this->syncing = getBool(jsonObjectData, "syncing");

		this->roleId = getId(jsonObjectData, "role_id");

		this->enableEmoticons = getBool(jsonObjectData, "enable_emoticons");

		this->expireBehavior = getUint32(jsonObjectData, "expire_behavior");

		this->expireGracePeriod = getUint32(jsonObjectData, "expire_grace_period");

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}

		if (getObject(object, "account", jsonObjectData)) {
			this->account = AccountData{ object };
		}

		if (getObject(object, "application", jsonObjectData)) {
			this->application = ApplicationData{ object };
		}

		this->syncedAt = getString(jsonObjectData, "synced_at");

		this->subscriberCount = getUint32(jsonObjectData, "subscriber_count");

		this->revoked = getBool(jsonObjectData, "revoked");
	}

	IntegrationDataVector::IntegrationDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					IntegrationData newData{ value.value() };
					this->integeregrations.emplace_back(std::move(newData));
				}
			}
		}
	}

	OptionalAuditEntryInfoData::OptionalAuditEntryInfoData(simdjson::ondemand::value jsonObjectData) {
		this->deleteMemberDays = getString(jsonObjectData, "delete_member_days");

		this->membersRemoved = getString(jsonObjectData, "members_removed");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->messageId = getId(jsonObjectData, "message_id");

		this->count = getString(jsonObjectData, "count");

		this->id = getId(jsonObjectData, "id");

		this->type = getString(jsonObjectData, "type");

		this->roleName = getString(jsonObjectData, "role_name");
	}

	AuditLogChangeData::AuditLogChangeData(simdjson::ondemand::value jsonObjectData) {
		this->newValue.value = getString(jsonObjectData, "new_value");

		this->oldValue.value = getString(jsonObjectData, "old_value");

		this->key = getString(jsonObjectData, "key");
	}

	GuildPruneCountData::GuildPruneCountData(simdjson::ondemand::value jsonObjectData) {
		this->count = getUint32(jsonObjectData, "pruned");
	}

	AuditLogEntryData::AuditLogEntryData(simdjson::ondemand::value jsonObjectData) {
		this->targetId = getId(jsonObjectData, "target_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "changes", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				AuditLogChangeData newData{ value.value() };
				this->changes.emplace_back(std::move(newData));
			}
		}

		this->userId = getId(jsonObjectData, "user_id");

		this->id = getId(jsonObjectData, "id");
		this->createdTimeStamp = this->getCreatedAtTimestamp(TimeFormat::LongDateTime);

		this->actionType = static_cast<AuditLogEvent>(getUint16(jsonObjectData, "action_type"));

		simdjson::ondemand::value object{};
		if (getObject(object, "options", jsonObjectData)) {
			this->options = OptionalAuditEntryInfoData{ object };
		}

		this->reason = getString(jsonObjectData, "reason");
	}

	WelcomeScreenChannelData::WelcomeScreenChannelData(simdjson::ondemand::value jsonObjectData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->description = getString(jsonObjectData, "description");

		this->emojiId = getId(jsonObjectData, "emoji_id");

		this->emojiName = getString(jsonObjectData, "emoji_name");
	}

	WelcomeScreenData::WelcomeScreenData(simdjson::ondemand::value jsonObjectData) {
		this->description = getString(jsonObjectData, "description");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "welcome_channels", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				WelcomeScreenChannelData newData{ value.value() };
				this->welcomeChannels.emplace_back(std::move(newData));
			}
		}
	}

	StageInstanceData::StageInstanceData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->topic = getString(jsonObjectData, "topic");

		this->privacyLevel = static_cast<StageInstancePrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->discoverableDisabled = getBool(jsonObjectData, "discoverable_disabled");
	}

	StickerData::StickerData(simdjson::ondemand::value jsonObjectData) {
		this->asset = getString(jsonObjectData, "asset");

		this->description = getString(jsonObjectData, "description");

		this->formatType = static_cast<StickerFormatType>(getUint8(jsonObjectData, "format_type"));

		uint8_t newFlags{};
		newFlags = setBool(newFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		this->flags = static_cast<StickerFlags>(newFlags);

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->packId = getString(jsonObjectData, "pack_id");

		this->type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		this->sortValue = getUint32(jsonObjectData, "sort_value");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}
	}

	GuildPreviewData::GuildPreviewData(simdjson::ondemand::value jsonObjectData) {
		this->approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		this->approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		this->discoverySplash = getString(jsonObjectData, "discovery_splash");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "emojis", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmojiData newData{ value.value() };
				this->emojis.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerData newData{ value.value() };
				this->stickers.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "features", jsonObjectData)) {
			this->features.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->features.emplace_back(value.get_string().take_value());
			}
		}

		this->description = getString(jsonObjectData, "description");

		this->splash = getString(jsonObjectData, "splash");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");
	}

	VoiceConnection* GuildData::connectToVoice(const Snowflake guildMemberId, const Snowflake channelId, bool selfDeaf, bool selfMute,
		StreamInfo streamInfoNew) {
		if (DiscordCoreClient::getVoiceConnection(this->id) && DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected()) {
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else if (static_cast<Snowflake>(guildMemberId) != 0 || static_cast<Snowflake>(channelId) != 0) {
			Snowflake channelId{};
			if (static_cast<Snowflake>(guildMemberId) != 0) {
				VoiceStateDataLight dataLight{};
				dataLight.guildId = this->id;
				dataLight.userId = guildMemberId;
				auto voiceStateData = GuildMembers::getVsCache()[dataLight];
				if (voiceStateData.channelId != 0) {
					channelId = voiceStateData.channelId;
				}
			} else {
				channelId = channelId;
			}
			uint64_t theShardId{ (this->id.operator size_t() >> 22) % this->discordCoreClient->configManager.getTotalShardCount() };
			auto theBaseSocketAgentIndex{ static_cast<int32_t>(
				floor(static_cast<float>(theShardId) / static_cast<float>(this->discordCoreClient->configManager.getTotalShardCount()) *
					this->discordCoreClient->baseSocketAgentsMap.size())) };
			VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = theShardId;
			voiceConnectInitData.streamInfo = streamInfoNew;
			voiceConnectInitData.channelId = channelId;
			voiceConnectInitData.guildId = this->id;
			voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			StopWatch stopWatch{ 10000ms };
			DiscordCoreClient::getVoiceConnection(this->id)->connect(voiceConnectInitData);
			while (!DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected()) {
				std::this_thread::sleep_for(1ms);
			}
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else {
			return nullptr;
		}
	}

	std::string GuildData::getIconUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "icons/" + this->id + "/" + this->icon.getIconHash() + ".png";
		return stringNew;
	}

	bool GuildData::areWeConnected() {
		return DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected();
	}

	void GuildData::disconnect() {
		if (DiscordCoreClient::getVoiceConnection(this->id)) {
			UpdateVoiceStateData updateVoiceData{};
			updateVoiceData.channelId = 0;
			updateVoiceData.selfDeaf = false;
			updateVoiceData.selfMute = false;
			updateVoiceData.guildId = this->id;
			this->discordCoreClient->getBotUser().updateVoiceStatus(updateVoiceData);
			DiscordCoreClient::getVoiceConnection(this->id)->disconnect();
			this->voiceConnectionPtr = nullptr;
		}
	}

	GuildData::GuildData(simdjson::ondemand::value jsonObjectData) {
		uint8_t newFlags{};
		newFlags = setBool(newFlags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));

		newFlags = setBool(newFlags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		newFlags = setBool(newFlags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		newFlags = setBool(newFlags, GuildFlags::Large, getUint8(jsonObjectData, "large"));

		this->flags = static_cast<GuildFlags>(newFlags);

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->id = getId(jsonObjectData, "id");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		this->threads.clear();

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "features", jsonObjectData)) {
			this->threads.clear();
			for (auto value: arrayValue) {
				auto& object = value.value();
				this->threads.emplace_back(getId(object, "id"));
			}
		}

		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			this->stickers.clear();
			for (auto value: arrayValue) {
				this->stickers.emplace_back(getId(value.value(), "id"));
			}
		}

		if (getArray(arrayValue, "guild_scheduled_events", jsonObjectData)) {
			this->guildScheduledEvents.clear();
			for (auto value: arrayValue) {
				this->guildScheduledEvents.emplace_back(getId(value.value(), "id"));
			}
		}

		if (getArray(arrayValue, "stage_instances", jsonObjectData)) {
			this->stageInstances.clear();
			for (auto value: arrayValue) {
				this->stageInstances.emplace_back(getId(value.value(), "id"));
			}
		}

		if (getArray(arrayValue, "emoji", jsonObjectData)) {
			this->emoji.clear();
			for (auto value: arrayValue) {
				this->emoji.emplace_back(getId(value.value(), "id"));
			}
		}

		if (Roles::doWeCacheRoles()) {
			this->roles.clear();
			if (getArray(arrayValue, "roles", jsonObjectData)) {
				for (auto value: arrayValue) {
					RoleData newData{ value.value() };
					newData.guildId = this->id;
					this->roles.emplace_back(newData.id);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		if (GuildMembers::doWeCacheGuildMembers()) {
			this->members.clear();
			GuildMemberData newData{};
			if (getArray(arrayValue, "members", jsonObjectData)) {
				for (auto value: arrayValue) {
					newData = value.value();
					newData.guildId = this->id;
					this->members.emplace_back(newData.id);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		if (getArray(arrayValue, "voice_states", jsonObjectData)) {
			for (auto value: arrayValue) {
				VoiceStateDataLight dataNew{ value.value() };
				dataNew.guildId = this->id;
				GuildMembers::vsCache[dataNew] = dataNew;
			}
		}

		if (GuildMembers::doWeCacheGuildMembers()) {
			this->presences.clear();
			if (getArray(arrayValue, "presences", jsonObjectData)) {
				for (auto value: arrayValue) {
					PresenceUpdateData newData{ value.value() };
					newData.guildId = this->id;
					this->presences.emplace_back(std::move(newData));
				}
			}
		}

		if (Channels::doWeCacheChannels()) {
			this->channels.clear();
			if (getArray(arrayValue, "channels", jsonObjectData)) {
				for (auto value: arrayValue) {
					ChannelData newData{ value.value() };
					newData.guildId = this->id;
					this->channels.emplace_back(newData.id);
					Channels::insertChannel(std::move(newData));
				}
			}
		}
	}

	GuildDataVector::GuildDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildData newData{ value.value() };
					this->guilds.emplace_back(std::move(newData));
				}
			}
		}
	}

	GuildScheduledEventMetadata::GuildScheduledEventMetadata(simdjson::ondemand::value jsonObjectData) {
		this->location = getString(jsonObjectData, "location");
	}

	GuildScheduledEventData::GuildScheduledEventData(simdjson::ondemand::value jsonObjectData) {
		this->privacyLevel = static_cast<GuildScheduledEventPrivacyLevel>(getUint8(jsonObjectData, "privacy_level"));

		this->entityType = static_cast<GuildScheduledEventEntityType>(getUint8(jsonObjectData, "entity_type"));

		this->status = static_cast<GuildScheduledEventStatus>(getUint8(jsonObjectData, "status"));

		simdjson::ondemand::value object{};
		if (getObject(object, "entity_metadata", jsonObjectData)) {
			this->entityMetadata = GuildScheduledEventMetadata{ object };
		}

		this->scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		this->scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		this->userCount = getUint32(jsonObjectData, "user_count");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->creatorId = getId(jsonObjectData, "creator_id");

		this->entityId = getId(jsonObjectData, "entity_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->description = getString(jsonObjectData, "description");

		if (getObject(object, "creator", jsonObjectData)) {
			this->creator = UserData{ object };
		}

		this->name = getString(jsonObjectData, "name");
	}

	GuildScheduledEventUserData::GuildScheduledEventUserData(simdjson::ondemand::value jsonObjectData) {
		this->guildScheduledEventId = getId(jsonObjectData, "guild_scheduled_event_id");

		simdjson::ondemand::value object{};
		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
		}

		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}
	}

	GuildScheduledEventUserDataVector::GuildScheduledEventUserDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildScheduledEventUserData newData{ value.value() };
					this->guildScheduledEventUsers.emplace_back(std::move(newData));
				}
			}
		}
	}

	GuildScheduledEventDataVector::GuildScheduledEventDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildScheduledEventData newData{ value.value() };
					this->guildScheduledEvents.emplace_back(std::move(newData));
				}
			}
		}
	}

	InviteData::InviteData(simdjson::ondemand::value jsonObjectData) {
		this->code = getUint32(jsonObjectData, "code");

		simdjson::ondemand::value object{};
		if (getObject(object, "guild", jsonObjectData)) {
			this->guild = GuildData{ object };
		}

		if (getObject(object, "channel", jsonObjectData)) {
			this->channel = ChannelData{ object };
		}

		if (getObject(object, "inviter", jsonObjectData)) {
			this->inviter = UserData{ object };
		}

		this->targetType = getUint32(jsonObjectData, "target_type");

		if (getObject(object, "target_user", jsonObjectData)) {
			this->targetUser = UserData{ object };
		}

		if (getObject(object, "target_application", jsonObjectData)) {
			this->targetApplication = ApplicationData{ object };
		}

		if (getObject(object, "stage_instance", jsonObjectData)) {
			this->stageInstance = StageInstanceData{ object };
		}

		if (getObject(object, "guild_scheduled_event", jsonObjectData)) {
			this->guildScheduledEvent = GuildScheduledEventData{ object };
		}

		this->approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		this->approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->expiresAt = getString(jsonObjectData, "expires_at");

		this->uses = getUint32(jsonObjectData, "uses");

		this->maxUses = getUint32(jsonObjectData, "max_uses");

		this->maxAge = getUint32(jsonObjectData, "max_age");

		this->temporary = getBool(jsonObjectData, "temporary");

		this->createdAt = getString(jsonObjectData, "created_at");
	}

	InviteDataVector::InviteDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					InviteData newData{ value.value() };
					this->invites.emplace_back(std::move(newData));
				}
			}
		}
	}

	GuildTemplateData::GuildTemplateData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "serialized_source_guild", jsonObjectData)) {
			this->serializedSourceGuild = GuildData{ object };
		}

		if (getObject(object, "creator", jsonObjectData)) {
			this->creator = UserData{ object };
		}

		this->sourceGuildId = getString(jsonObjectData, "source_guild_id");

		this->description = getString(jsonObjectData, "description");

		this->usageCount = getUint32(jsonObjectData, "usage_count");

		this->creatorId = getString(jsonObjectData, "creator_id");

		this->createdAt = getString(jsonObjectData, "created_at");

		this->updatedAt = getString(jsonObjectData, "updated_at");

		this->isDirty = getBool(jsonObjectData, "is_dirty");

		this->code = getString(jsonObjectData, "code");

		this->name = getString(jsonObjectData, "name");
	}

	GuildTemplateDataVector::GuildTemplateDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					GuildTemplateData newData{ value.value() };
					this->guildTemplates.emplace_back(std::move(newData));
				}
			}
		}
	}

	WebHookData::WebHookData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->type = static_cast<WebHookType>(getUint8(jsonObjectData, "type"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}

		this->name = getString(jsonObjectData, "name");

		this->avatar = getString(jsonObjectData, "avatar");

		this->token = getString(jsonObjectData, "token");

		this->applicationId = getId(jsonObjectData, "application_id");

		if (getObject(object, "source_guild", jsonObjectData)) {
			this->sourceGuild = GuildData{ object };
		}

		if (getObject(object, "source_channel", jsonObjectData)) {
			this->sourceChannel = ChannelData{ object };
		}

		this->url = getString(jsonObjectData, "url");
	}

	WebHookDataVector::WebHookDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					WebHookData newData{ value.value() };
					this->webHooks.emplace_back(std::move(newData));
				}
			}
		}
	}

	AuditLogData::AuditLogData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "webhooks", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				WebHookData newData{ value.value() };
				this->webhooks.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "guild_scheduled_events", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				GuildScheduledEventData newData{ value.value() };
				this->guildScheduledEvents.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "auto_moderation_rules", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				AutoModerationRuleData newData{ value.value() };
				this->autoModerationRules.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "users", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				UserData newData{ value.value() };
				this->users.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "audit_log_entries", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				AuditLogEntryData newData{ value.value() };
				this->auditLogEntries.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "integrations", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				IntegrationData newData{ value.value() };
				this->integrations.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "threads", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}
	}

	ReactionRemoveData::ReactionRemoveData(simdjson::ondemand::value jsonObjectData) {
		this->userId = getId(jsonObjectData, "user_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->messageId = getId(jsonObjectData, "message_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value object{};
		if (getObject(object, "emoji", jsonObjectData)) {
			this->emoji = EmojiData{ object };
		}
	}

	ApplicationCommandOptionChoiceData::ApplicationCommandOptionChoiceData(simdjson::ondemand::value jsonObjectData) {
		std::string_view string{};
		uint64_t integer{};
		bool theBool{};
		double doubleVal{};

		if (jsonObjectData["value"].get(string) == simdjson::error_code::SUCCESS) {
			this->value = static_cast<std::string>(string);
			this->type = JsonType::String;
		} else if (jsonObjectData["value"].get(integer) == simdjson::error_code::SUCCESS) {
			this->value = std::to_string(integer);
			this->type = JsonType::Int64;
		} else if (jsonObjectData["value"].get(theBool) == simdjson::error_code::SUCCESS) {
			this->type = JsonType::Bool;
			std::stringstream stream{};
			stream << std::boolalpha << theBool;
			this->value = stream.str();
		} else if (jsonObjectData["value"].get(doubleVal) == simdjson::error_code::SUCCESS) {
			this->type = JsonType::Float;
			this->value = std::to_string(doubleVal);
		}

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::object map{};
		if (jsonObjectData["name_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: map) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}
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

	ApplicationCommandOptionData::ApplicationCommandOptionData(simdjson::ondemand::value jsonObjectData) {
		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::object map{};
		if (jsonObjectData["name_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: map) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		if (jsonObjectData["description_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: map) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		this->description = getString(jsonObjectData, "description");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "channel_types", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->channelTypes.emplace_back(static_cast<ChannelType>(value.get_uint64().value()));
			}
		}

		this->type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		if (this->type == ApplicationCommandOptionType::Integer) {
			this->minValue = getInt64(jsonObjectData, "min_value");
		} else if (this->type == ApplicationCommandOptionType::Number) {
			this->minValue = getFloat(jsonObjectData, "min_value");
		}

		if (this->type == ApplicationCommandOptionType::Integer) {
			this->maxValue = getInt32(jsonObjectData, "max_value");
		} else if (this->type == ApplicationCommandOptionType::Number) {
			this->maxValue = getFloat(jsonObjectData, "max_value");
		}

		this->required = getBool(jsonObjectData, "required");

		this->autocomplete = getBool(jsonObjectData, "autocomplete");

		if (this->type == ApplicationCommandOptionType::Sub_Command_Group || this->type == ApplicationCommandOptionType::Sub_Command) {
			if (getArray(arrayValue, "options", jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					ApplicationCommandOptionData newData{ value.value() };
					this->options.emplace_back(std::move(newData));
				}
			}
		}
		if (getArray(arrayValue, "choices", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ApplicationCommandOptionChoiceData newData{ value.value() };
				this->choices.emplace_back(std::move(newData));
			}
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

	ApplicationCommandData::ApplicationCommandData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		this->dmPermission = getBool(jsonObjectData, "dm_permission");

		this->version = getString(jsonObjectData, "version");

		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		simdjson::ondemand::object map{};
		if (jsonObjectData["name_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: map) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		if (jsonObjectData["description_localizations"].get(map) == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: map) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		this->name = getString(jsonObjectData, "name");

		this->description = getString(jsonObjectData, "description");

		this->version = getString(jsonObjectData, "version");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "options", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ApplicationCommandOptionData newData{ value.value() };
				this->options.emplace_back(std::move(newData));
			}
		}
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
		for (size_t x = 0; x < lhs.options.size(); ++x) {
			if (lhs.options[x] != rhs.options[x]) {
				return false;
			}
		}
		return true;
	}

	TypingStartData::TypingStartData(simdjson::ondemand::value jsonObjectData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value object{};
		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
		}

		this->userId = getId(jsonObjectData, "user_id");

		this->timeStamp = getUint32(jsonObjectData, "timestamp");
	}

	YouTubeFormat::YouTubeFormat(simdjson::ondemand::value jsonObjectData) {
		this->audioQuality = getString(jsonObjectData, "audioQuality");

		this->averageBitrate = getUint32(jsonObjectData, "averageBitrate");

		this->audioSampleRate = getString(jsonObjectData, "audioSampleRate");

		this->bitrate = getUint32(jsonObjectData, "bitrate");

		this->contentLength = strtoull(getString(jsonObjectData, "contentLength"));

		this->fps = getUint32(jsonObjectData, "fps");

		this->height = getUint32(jsonObjectData, "height");

		this->height = getUint32(jsonObjectData, "width");

		this->aitags = getString(jsonObjectData, "aitags");

		this->itag = getUint32(jsonObjectData, "itag");

		this->downloadUrl = getString(jsonObjectData, "url");

		this->mimeType = getString(jsonObjectData, "mimeType");

		this->quality = getString(jsonObjectData, "quality");

		std::string string = getString(jsonObjectData, "signatureCipher");
		if (string == "") {
			string = getString(jsonObjectData, "cipher");
		}

		this->signatureCipher = string;

		auto ampersandSpFind = this->signatureCipher.find("&sp");
		if (ampersandSpFind != std::string::npos) {
			this->signature = this->signatureCipher.substr(0, ampersandSpFind);
		}

		auto urlFind = this->signatureCipher.find("url");
		if (urlFind != std::string::npos) {
			this->downloadUrl = this->signatureCipher.substr(urlFind + 4);
		} else {
			this->downloadUrl = getString(jsonObjectData, "url");
		}
	}

	YouTubeFormatVector::YouTubeFormatVector(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value valueNew{};
		if (getObject(valueNew, "streamingData", jsonObjectData)) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, "formats", valueNew)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					YouTubeFormat newData{ value.value() };
					this->formats.emplace_back(std::move(newData));
				}
			}
		}


		if (getObject(valueNew, "streamingData", jsonObjectData)) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, "adaptiveFormats", valueNew)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					YouTubeFormat newData{ value.value() };
					this->formats.emplace_back(std::move(newData));
				}
			}
		}
	}

	UserCommandInteractionData::UserCommandInteractionData(simdjson::ondemand::value jsonObjectData) {
		this->targetId = getId(jsonObjectData, "target_id");
	}

	MessageCommandInteractionData::MessageCommandInteractionData(simdjson::ondemand::value jsonObjectData) {
		this->targetId = getId(jsonObjectData, "target_id");
	}

	ComponentInteractionData::ComponentInteractionData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "values", jsonObjectData)) {
			this->values.clear();
			for (auto iterator = arrayValue.begin(); iterator != arrayValue.end(); ++iterator) {
				this->values.emplace_back(iterator.value().operator*().get_string().take_value());
			}
		}

		this->customId = getString(jsonObjectData, "custom_id");

		this->componentType = static_cast<ComponentType>(getUint8(jsonObjectData, "component_type"));
	}

	ModalInteractionData::ModalInteractionData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value theComponent{};
		if (jsonObjectData["components"][0]["components"][0].get(theComponent) == simdjson::error_code::SUCCESS) {
			this->value = getString(theComponent, "value");
			this->customIdSmall = getString(theComponent, "value");
		}

		this->customId = getString(jsonObjectData, "custom_id");
	}

	AllowedMentionsData::AllowedMentionsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "parse", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->parse.emplace_back(static_cast<std::string>(value.get_string().value()));
			}
		}

		if (getArray(arrayValue, "roles", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->roles.emplace_back(static_cast<std::string>(value.get_string().value()));
			}
		}

		if (getArray(arrayValue, "users", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->users.emplace_back(static_cast<std::string>(value.get_string().value()));
			}
		}

		this->repliedUser = getBool(jsonObjectData, "replied_user");
	}

	SelectOptionData::SelectOptionData(simdjson::ondemand::value jsonObjectData) {
		this->label = getString(jsonObjectData, "label");

		this->value = getString(jsonObjectData, "value");

		this->description = getString(jsonObjectData, "description");

		simdjson::ondemand::value theEmoji{};
		if (jsonObjectData["emoji"].get(theEmoji) == simdjson::error_code::SUCCESS) {
			this->emoji = EmojiData{ theEmoji };
		}

		this->_default = getBool(jsonObjectData, "default");
	}

	ActionRowData::ActionRowData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "components", jsonObjectData)) {
			this->components.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ComponentData newData{ value.value() };
				this->components.emplace_back(std::move(newData));
			}
		}
	}

	ComponentData::ComponentData(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<ComponentType>(getUint8(jsonObjectData, "type"));

		this->customId = getString(jsonObjectData, "custom_id");

		this->placeholder = getString(jsonObjectData, "placeholder");

		this->disabled = getBool(jsonObjectData, "disabled");

		this->style = getUint32(jsonObjectData, "style");

		this->label = getString(jsonObjectData, "label");

		this->minLength = getUint32(jsonObjectData, "min_length");

		this->maxLength = getUint32(jsonObjectData, "max_length");

		this->maxValues = getUint32(jsonObjectData, "max_values");

		this->maxValues = getUint32(jsonObjectData, "min_values");

		this->title = getString(jsonObjectData, "title");

		this->required = getBool(jsonObjectData, "required");

		simdjson::ondemand::value object{};
		if (getObject(object, "emoji", jsonObjectData)) {
			this->emoji = EmojiData{ object };
		}

		this->url = getString(jsonObjectData, "url");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "select_option_data", jsonObjectData)) {
			this->options.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				SelectOptionData newData{ value.value() };
				this->options.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "channel_types", jsonObjectData)) {
			this->channelTypes.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelType newData{ static_cast<ChannelType>(value.value().get_int64().take_value()) };
				this->channelTypes.emplace_back(std::move(newData));
			}
		}
	}

	ChannelMentionData::ChannelMentionData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");
	}

	ChannelPinsUpdateEventData::ChannelPinsUpdateEventData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->lastPinTimeStamp = getString(jsonObjectData, "last_pin_timestamp");
	}

	ThreadListSyncData::ThreadListSyncData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "channel_ids", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->channelIds.emplace_back(value.get_string().take_value());
			}
		}

		if (getArray(arrayValue, "members", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ThreadMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "threads", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}
	}

	ThreadMembersUpdateData::ThreadMembersUpdateData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "added_members", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ThreadMemberData newData{ value.value() };
				this->addedMembers.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "removed_member_ids", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->removedMemberIds.emplace_back(value.get_string().take_value());
			}
		}
	}

	MessageInteractionData::MessageInteractionData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::value object{};
		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}

		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
		}
	}

	StickerItemData::StickerItemData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		this->formatType = static_cast<StickerItemType>(getUint8(jsonObjectData, "format_type"));
	}

	MessageDataOld::MessageDataOld(simdjson::ondemand::value jsonObjectData) {
		this->content = getString(jsonObjectData, "content");

		this->id = getId(jsonObjectData, "id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value object{};
		if (getObject(object, "author", jsonObjectData)) {
			this->author = UserData{ object };
		}

		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
		}

		this->timeStamp = getString(jsonObjectData, "timestamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");


		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "mentions", jsonObjectData)) {
			this->mentions.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				UserData newData{ value.value() };
				this->mentions.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "mention_roles", jsonObjectData)) {
			this->mentionRoles.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				std::string_view object = value.get_string().take_value();
				this->mentionRoles.emplace_back(std::move(object));
			}
		}

		if (getArray(arrayValue, "mention_channels", jsonObjectData)) {
			this->mentionChannels.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelMentionData newChannelData{ value.value() };
				this->mentionChannels.emplace_back(std::move(newChannelData));
			}
		}

		if (getArray(arrayValue, "attachments", jsonObjectData)) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				AttachmentData newAttachmentData{ value.value() };
				this->attachments.emplace_back(std::move(newAttachmentData));
			}
		}

		if (getArray(arrayValue, "embeds", jsonObjectData)) {
			this->embeds.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmbedData newEmbedData{ value.value() };
				this->embeds.emplace_back(std::move(newEmbedData));
			}
		}

		if (getArray(arrayValue, "reactions", jsonObjectData)) {
			this->reactions.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ReactionData newReactionData{ value.value() };
				this->reactions.emplace_back(std::move(newReactionData));
			}
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));
		if (getObject(object, "activity", jsonObjectData)) {
			this->activity = MessageActivityData{ object };
		}

		if (getObject(object, "application", jsonObjectData)) {
			this->application = ApplicationData{ object };
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		if (getObject(object, "message_reference", jsonObjectData)) {
			this->messageReference = MessageReferenceData{ object };
		}

		this->flags = getUint32(jsonObjectData, "flags");
		if (getArray(arrayValue, "sticker_items", jsonObjectData)) {
			this->stickerItems.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerItemData newReactionData{ value.value() };
				this->stickerItems.emplace_back(std::move(newReactionData));
			}
		}

		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			this->stickers.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerData newReactionData{ value.value() };
				this->stickers.emplace_back(std::move(newReactionData));
			}
		}

		if (getObject(object, "interaction", jsonObjectData)) {
			this->interaction = MessageInteractionData{ object };
		}

		if (getArray(arrayValue, "components", jsonObjectData)) {
			this->components.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ActionRowData newActionRowData{ value.value() };
				this->components.emplace_back(std::move(newActionRowData));
			}
		}

		if (getObject(object, "thread", jsonObjectData)) {
			this->thread = ChannelData{ object };
		}
	}

	MessageData::MessageData(simdjson::ondemand::value jsonObjectData) {
		this->content = getString(jsonObjectData, "content");

		this->id = getId(jsonObjectData, "id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value object{};
		if (getObject(object, "author", jsonObjectData)) {
			this->author = UserData{ object };
		}

		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
		}

		this->timeStamp = getString(jsonObjectData, "timestamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");


		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "mentions", jsonObjectData)) {
			this->mentions.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				UserData newData{ value.value() };
				this->mentions.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "mention_roles", jsonObjectData)) {
			this->mentionRoles.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				std::string_view object = value.get_string().take_value();
				this->mentionRoles.emplace_back(std::move(object));
			}
		}

		if (getArray(arrayValue, "mention_channels", jsonObjectData)) {
			this->mentionChannels.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ChannelMentionData newChannelData{ value.value() };
				this->mentionChannels.emplace_back(std::move(newChannelData));
			}
		}

		if (getArray(arrayValue, "attachments", jsonObjectData)) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				AttachmentData newAttachmentData{ value.value() };
				this->attachments.emplace_back(std::move(newAttachmentData));
			}
		}

		if (getArray(arrayValue, "embeds", jsonObjectData)) {
			this->embeds.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmbedData newEmbedData{ value.value() };
				this->embeds.emplace_back(std::move(newEmbedData));
			}
		}

		if (getArray(arrayValue, "reactions", jsonObjectData)) {
			this->reactions.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ReactionData newReactionData{ value.value() };
				this->reactions.emplace_back(std::move(newReactionData));
			}
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		if (getObject(object, "activity", jsonObjectData)) {
			this->activity = MessageActivityData{ object };
		}

		if (getObject(object, "application", jsonObjectData)) {
			this->application = ApplicationData{ object };
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		if (getObject(object, "referenced_message", jsonObjectData)) {
			this->referencedMessage = std::make_unique<MessageDataOld>(object);
		}

		if (getObject(object, "message_reference", jsonObjectData)) {
			this->messageReference = MessageReferenceData{ object };
		}

		this->flags = getUint32(jsonObjectData, "flags");

		if (getArray(arrayValue, "sticker_items", jsonObjectData)) {
			this->stickerItems.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerItemData newReactionData{ value.value() };
				this->stickerItems.emplace_back(std::move(newReactionData));
			}
		}

		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			this->stickers.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerData newReactionData{ value.value() };
				this->stickers.emplace_back(std::move(newReactionData));
			}
		}

		if (getObject(object, "interaction", jsonObjectData)) {
			this->interaction = MessageInteractionData{ object };
		}

		if (getArray(arrayValue, "components", jsonObjectData)) {
			this->components.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ActionRowData newActionRowData{ value.value() };
				this->components.emplace_back(std::move(newActionRowData));
			}
		}

		if (getObject(object, "thread", jsonObjectData)) {
			this->thread = ChannelData{ object };
		}
	}

	ResolvedData::ResolvedData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::object arrayValue{};
		if (jsonObjectData["attachments"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::field> value: arrayValue) {
				AttachmentData newData{ value.value() };
				this->attachments[strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}
		if (jsonObjectData["users"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->users.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::field> value: arrayValue) {
				UserData newData{ value.value() };
				this->users[strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}
		if (jsonObjectData["channels"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->channels.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::field> value: arrayValue) {
				ChannelData newData{ value.value() };
				this->channels[strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}
		if (jsonObjectData["roles"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->roles.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::field> value: arrayValue) {
				RoleData newData{ value.value() };
			}
		}

		if (jsonObjectData["members"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->members.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::field> value: arrayValue) {
				GuildMemberData newData{ value.value() };
				this->members[strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}

		if (jsonObjectData["messages"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->messages.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::field> value: arrayValue) {
				MessageData newData{ value.value() };
				this->messages[strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}
	}

	StickerPackData::StickerPackData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerData newData{ value.value() };
				this->stickers.emplace_back(std::move(newData));
			}
		}

		this->coverStickerId = getString(jsonObjectData, "cover_sticker_id");

		this->bannerAssetId = getString(jsonObjectData, "banner_asset_id");

		this->description = getString(jsonObjectData, "description");

		this->name = getString(jsonObjectData, "name");

		this->Id = getId(jsonObjectData, "id");

		this->skuId = getString(jsonObjectData, "sku_id");
	}

	StickerPackDataVector::StickerPackDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					StickerPackData newData{ value.value() };
					this->stickerPacks.emplace_back(std::move(newData));
				}
			}
		}
	}

	ConnectionData::ConnectionData(simdjson::ondemand::value jsonObjectData) {
		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");

		this->type = getString(jsonObjectData, "type");

		this->showActivity = getBool(jsonObjectData, "show_activity");

		this->friendSync = getBool(jsonObjectData, "friend_sync");

		this->revoked = getBool(jsonObjectData, "revoked");

		this->verified = getBool(jsonObjectData, "verified");

		this->visibility = static_cast<ConnectionVisibilityTypes>(getUint8(jsonObjectData, "visibility"));

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "integrations", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				IntegrationData newData{ value.value() };
				this->integrations.emplace_back(std::move(newData));
			}
		}
	}

	ApplicationCommandInteractionDataOption::ApplicationCommandInteractionDataOption(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		this->focused = getBool(jsonObjectData, "focused");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "options", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ApplicationCommandInteractionDataOption newData{ value.value() };
				this->options.emplace_back(std::move(newData));
			}
		}

		this->value.type = JsonType::Null;
		bool theBool{};
		if (jsonObjectData["value"].get(theBool) == simdjson::error_code::SUCCESS) {
			this->value.type = JsonType::Bool;
			this->value.value = std::to_string(theBool);
			return;
		}

		uint64_t integereger{};
		if (jsonObjectData["value"].get(integereger) == simdjson::error_code::SUCCESS) {
			this->value.type = JsonType::Uint64;
			this->value.value = std::to_string(integereger);
			return;
		}

		std::string_view string{};
		if (jsonObjectData["value"].get(string) == simdjson::error_code::SUCCESS) {
			this->value.type = JsonType::String;
			this->value.value = string;
			return;
		}

		double theFloat{};
		if (jsonObjectData["value"].get(theFloat) == simdjson::error_code::SUCCESS) {
			this->value.type = JsonType::Float;
			this->value.value = std::to_string(theFloat);
			return;
		}
	}

	ApplicationCommandInteractionData::ApplicationCommandInteractionData(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "options", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				ApplicationCommandInteractionDataOption newData{ value.value() };
				this->options.emplace_back(std::move(newData));
			}
		}

		simdjson::ondemand::value object{};
		if (getObject(object, "resolved", jsonObjectData)) {
			this->resolved = ResolvedData{ object };
		}
	}

	InteractionDataData::InteractionDataData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "id", jsonObjectData)) {
			this->applicationCommandData = ApplicationCommandInteractionData{ jsonObjectData };
		}

		if (getObject(object, "target_id", jsonObjectData)) {
			this->messageInteractionData = MessageCommandInteractionData{ jsonObjectData };
			this->userInteractionData = UserCommandInteractionData{ jsonObjectData };
		}

		uint64_t componentType{};
		if (getObject(object, "component_type", jsonObjectData)) {
			this->componentData = ComponentInteractionData{ jsonObjectData };
		}

		if (getObject(object, "components", jsonObjectData)) {
			this->modalData = ModalInteractionData{ jsonObjectData };
		}
	}

	InteractionData::InteractionData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value object{};
		if (getObject(object, "data", jsonObjectData)) {
			this->data = InteractionDataData{ object };
		}

		this->appPermissions = getString(jsonObjectData, "app_permissions");

		this->type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		this->token = getString(jsonObjectData, "token");
		if (getObject(object, "member", jsonObjectData)) {
			this->member = GuildMemberData{ object };
			this->user.id = this->member.id;
			this->user.userName = this->member.getUserData().userName;
		}

		if (getObject(object, "user", jsonObjectData)) {
			this->user = UserData{ object };
		}

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->locale = getString(jsonObjectData, "locale");

		this->guildLocale = getString(jsonObjectData, "guild_locale");

		if (getObject(object, "message", jsonObjectData)) {
			this->message = MessageData{ object };
		}

		this->version = getUint32(jsonObjectData, "version");

		this->id = getId(jsonObjectData, "id");

		this->applicationId = getId(jsonObjectData, "application_id");
	}

	SessionStartData::SessionStartData(simdjson::ondemand::value jsonObjectData) {
		this->maxConcurrency = getUint32(jsonObjectData, "max_concurrency");

		this->remaining = getUint32(jsonObjectData, "remaining");

		this->resetAfter = getUint32(jsonObjectData, "reset_after");

		this->total = getUint32(jsonObjectData, "total");
	}

	GatewayBotData::GatewayBotData(simdjson::ondemand::value jsonObjectData) {
		this->url = getString(jsonObjectData, "url");

		this->shards = getUint32(jsonObjectData, "shards");

		simdjson::ondemand::value object{};
		if (getObject(object, "session_start_limit", jsonObjectData)) {
			this->sessionStartLimit = SessionStartData{ object };
		}
	}

	GuildEmojisUpdateEventData::GuildEmojisUpdateEventData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "emojis", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				EmojiData newData{ value.value() };
				this->emojis.emplace_back(std::move(newData));
			}
		}
	}

	GuildStickersUpdateEventData::GuildStickersUpdateEventData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "stickers", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				StickerData newData{ value.value() };
				this->stickers.emplace_back(std::move(newData));
			}
		}
	}

	GuildMembersChunkEventData::GuildMembersChunkEventData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->nonce = getString(jsonObjectData, "nonce");

		this->chunkIndex = getUint32(jsonObjectData, "chunk_index");

		this->chunkCount = getUint32(jsonObjectData, "chunk_count");

		simdjson::ondemand::array arrayValue{};
		if (getArray(arrayValue, "presences", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				PresenceUpdateData newData{ value.value() };
				this->presences.emplace_back(std::move(newData));
			}
		}

		if (getArray(arrayValue, "not_found", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				this->notFound.emplace_back(value.get_string().take_value());
			}
		}

		if (getArray(arrayValue, "members", jsonObjectData)) {
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				GuildMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}
	}

	MediaTranscoding::MediaTranscoding(simdjson::ondemand::value jsonObjectData) {
		this->thePreset = getString(jsonObjectData, "preset");

		this->theUrl = getString(jsonObjectData, "url");
	}

	Song::Song(simdjson::ondemand::value jsonObjectData) {
		this->duration = getString(getObject(getObject(getObject(jsonObjectData, "lengthText"), "accessibility"), "accessibilityData"), "label");
		std::string newString = getString(
			getObject(getArray(getObject(getObject(getArray(jsonObjectData, "detailedMetadataSnippets"), 0), "snippetText"), "runs"), 0), "text");
		if (newString.size() > 256) {
			newString = newString.substr(0, 256);
		}
		this->description = utf8MakeValid(newString);

		this->thumbnailUrl = getString(getObject(getArray(getObject(jsonObjectData, "thumbnail"), "thumbnails"), 0), "url");
		std::string newTitle01 = getString(getObject(getArray(getObject(jsonObjectData, "title"), "runs"), 0), "text");
		if (newTitle01.size() > 256) {
			newTitle01 = newTitle01.substr(0, 256);
		}
		if (newTitle01.size() > 0) {
			this->songTitle = utf8MakeValid(newTitle01);
		}
		std::string newTitle02 = getString(getObject(jsonObjectData, "title"), "simpleText");
		if (newTitle02.size() > 256) {
			newTitle02 = newTitle02.substr(0, 256);
		}
		if (newTitle02.size() > 0) {
			this->songTitle = utf8MakeValid(newTitle02);
		}

		if (newTitle01 != "") {
			this->songTitle = newTitle01;
		} else {
			this->songTitle = newTitle02;
		}

		this->songId = getString(jsonObjectData, "videoId");

		this->trackAuthorization = getString(jsonObjectData, "track_authorization");

		std::vector<MediaTranscoding> theMedia{};
		auto arrayValueNew = getArray(getObject(jsonObjectData, "media"), "transcodings");
		if (arrayValueNew.didItSucceed) {
			for (auto value: arrayValueNew.arrayValue) {
				MediaTranscoding dataNew{ value.value() };
				theMedia.emplace_back(dataNew);
			}
		}

		bool isItFound{};
		for (auto& value: theMedia) {
			if (value.thePreset == "opus_0_0") {
				isItFound = true;
				this->firstDownloadUrl = value.theUrl;
				this->songId = value.theUrl;
				this->doWeGetSaved = true;
			}
		}
		bool isItFound2{};
		if (!isItFound) {
			for (auto& value: theMedia) {
				if (value.thePreset == "mp3_0_0") {
					this->firstDownloadUrl = value.theUrl;
					this->songId = value.theUrl;
					isItFound2 = true;
				}
			}
		}
		if (!isItFound2 && !isItFound) {
			for (auto& value: theMedia) {
				this->firstDownloadUrl = value.theUrl;
				this->songId = value.theUrl;
			}
		}

		newString = getString(jsonObjectData, "title");
		if (newString.size() > 0) {
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->songTitle = utf8MakeValid(newString);
		}

		newString = getString(jsonObjectData, "description");
		if (newString.size() > 0) {
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->description = utf8MakeValid(newString);
			this->description += "...";
		}

		newString = getString(jsonObjectData, "artwork_url");
		if (newString.size() > 0) {
			this->thumbnailUrl = newString;
		}

		newString = getString(getObject(jsonObjectData, "user"), "avatar_url");
		if (newString.size() > 0) {
			this->thumbnailUrl = newString;
		}

		uint32_t theDuration = getUint32(jsonObjectData, "duration");
		if (theDuration != 0) {
			this->duration = TimeStamp::convertMsToDurationString(theDuration);
		}

		newString = getString(jsonObjectData, "permalink_url");
		if (newString.size() > 0) {
			this->viewUrl = newString;
		}
	}

	AttachmentData::operator Jsonifier() {
		Jsonifier data{};
		data["content_type"] = this->contentType;
		data["description"] = this->description;
		data["ephemeral"] = this->ephemeral;
		data["file_name"] = this->filename;
		data["proxy_url"] = this->proxyUrl;
		data["height"] = this->height;
		data["width"] = this->width;
		data["size"] = this->size;
		data["url"] = this->url;
		return data;
	}

	EmbedFieldData::operator Jsonifier() {
		Jsonifier data{};
		data["inline"] = this->Inline;
		data["value"] = escapeCharacters(this->value);
		data["name"] = escapeCharacters(this->name);
		return data;
	}

	EmbedData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value2: this->fields) {
			data["fields"].emplaceBack(value2.operator Jsonifier());
		}
		std::string realColorVal = std::to_string(this->hexColorValue.getIntColorValue());
		data["footer"]["proxy_icon_url"] = this->footer.proxyIconUrl;
		data["footer"]["icon_url"] = this->footer.iconUrl;
		data["footer"]["text"] = escapeCharacters(this->footer.text);
		data["author"]["proxy_icon_url"] = this->author.proxyIconUrl;
		data["author"]["icon_url"] = this->author.iconUrl;
		data["author"]["name"] = this->author.name;
		data["author"]["url"] = this->author.url;
		data["thumbnail"]["proxy_url"] = this->thumbnail.proxyUrl;
		data["thumbnail"]["height"] = this->thumbnail.height;
		data["thumbnail"]["width"] = this->thumbnail.width;
		data["thumbnail"]["url"] = this->thumbnail.url;
		data["image"]["proxy_url"] = this->image.proxyUrl;
		data["image"]["height"] = this->image.height;
		data["image"]["width"] = this->image.width;
		data["image"]["url"] = this->image.url;
		data["video"]["proxy_url"] = this->video.proxyUrl;
		data["video"]["height"] = this->video.height;
		data["video"]["url"] = this->video.url;
		data["video"]["width"] = this->video.width;
		data["provider"]["name"] = this->provider.name;
		data["provider"]["url"] = this->provider.url;
		data["description"] = escapeCharacters(this->description);
		data["timestamp"] = this->timeStamp;
		data["title"] = escapeCharacters(this->title);
		data["color"] = realColorVal;
		data["type"] = this->type;
		data["url"] = this->url;
		return data;
	}

	EmbedData& EmbedData::setAuthor(const std::string& authorName, const std::string& authorAvatarUrl) {
		this->author.name = authorName;
		this->author.iconUrl = authorAvatarUrl;
		return *this;
	}

	EmbedData& EmbedData::setFooter(const std::string& footerText, const std::string& footerIconUrlText) {
		this->footer.text = footerText;
		this->footer.iconUrl = footerIconUrlText;
		return *this;
	}

	EmbedData& EmbedData::setTimeStamp(const std::string& timeStamp) {
		this->timeStamp = timeStamp;
		return *this;
	}

	EmbedData& EmbedData::addField(const std::string& name, const std::string& value, bool Inline) {
		EmbedFieldData field{};
		field.Inline = Inline;
		field.value = value;
		field.name = name;
		this->fields.emplace_back(field);
		return *this;
	}

	EmbedData& EmbedData::setDescription(const std::string& descriptionNew) {
		this->description = descriptionNew;
		return *this;
	}

	EmbedData& EmbedData::setColor(const std::string& hexColorValueNew) {
		this->hexColorValue = hexColorValueNew;
		return *this;
	}

	EmbedData& EmbedData::setThumbnail(const std::string& thumbnailUrl) {
		this->thumbnail.url = thumbnailUrl;
		return *this;
	}

	EmbedData& EmbedData::setTitle(const std::string& titleNew) {
		this->title = titleNew;
		return *this;
	}

	EmbedData& EmbedData::setImage(const std::string& imageUrl) {
		this->image.url = imageUrl;
		return *this;
	}

	MessageReferenceData::operator Jsonifier() {
		Jsonifier data{};
		data["fail_if_not_exists"] = this->failIfNotExists;
		data["message_id"] = this->messageId;
		data["channel_id"] = this->channelId;
		data["guild_id"] = this->guildId;
		return data;
	}

	GuildApplicationCommandPermissionsDataVector::operator std::vector<GuildApplicationCommandPermissionsData>() {
		return this->guildApplicationCommandPermissionss;
	}

	BanDataVector::operator std::vector<BanData>() {
		return this->bans;
	}

	UpdateVoiceStateData::operator Jsonifier() {
		Jsonifier data{};
		data["op"] = 4;
		if (this->channelId == 0) {
			data["d"]["channel_id"] = JsonType::Null;
		} else {
			data["d"]["channel_id"] = this->channelId;
		}
		data["d"]["self_deaf"] = this->selfDeaf;
		data["d"]["self_mute"] = this->selfMute;
		data["d"]["guild_id"] = this->guildId;
		return data;
	}

	GuildDataVector::operator std::vector<GuildData>() {
		return this->guilds;
	}

	GuildScheduledEventUserDataVector::operator std::vector<GuildScheduledEventUserData>() {
		return this->guildScheduledEventUsers;
	}

	GuildScheduledEventDataVector::operator std::vector<GuildScheduledEventData>() {
		return this->guildScheduledEvents;
	}

	InviteDataVector::operator std::vector<InviteData>() {
		return this->invites;
	}

	GuildTemplateDataVector::operator std::vector<GuildTemplateData>() {
		return this->guildTemplates;
	}

	WebHookDataVector::operator std::vector<WebHookData>() {
		return this->webHooks;
	}

	auto AuditLogData::getAuditLogData(const Snowflake userIdOfChanger, AuditLogEvent auditLogType) {
		for (auto& value: this->auditLogEntries) {
			if (value.id == userIdOfChanger && value.actionType == auditLogType) {
				return value;
			}
		}
		return AuditLogEntryData();
	}

	auto AuditLogData::getAuditLogData(AuditLogEvent auditLogType, const Snowflake userIdOfTarget) {
		for (auto& value: this->auditLogEntries) {
			if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
				return value;
			}
		}
		return AuditLogEntryData();
	}

	ApplicationCommandOptionChoiceData::operator Jsonifier() {
		Jsonifier data{};
		data["name"] = this->name;
		data["name_localizations"] = this->nameLocalizations;
		switch (this->type) {
			case JsonType::Int64: {
				data["value"] = uint64_t{ stoull(this->value) };
				break;
			}
			case JsonType::Uint64: {
				data["value"] = uint64_t{ stoull(this->value) };
				break;
			}
			case JsonType::Float: {
				data["value"] = double{ stod(this->value) };
				break;
			}
			case JsonType::Bool: {
				if (this->value == "false") {
					data["value"] = bool{};
				} else {
					data["value"] = bool{ true };
				}
				break;
			}
			case JsonType::String: {
				data["value"] = this->value;
				break;
			}
			case JsonType::Null: {
				data["value"] = this->value;
				break;
			}
		}
		return data;
	}

	ApplicationCommandOptionData::operator Jsonifier() {
		Jsonifier data{};
		if (this->type == ApplicationCommandOptionType::Channel) {
			data["channel_types"] = this->channelTypes;
		}
		if (this->type != ApplicationCommandOptionType::Sub_Command &&
			this->type != ApplicationCommandOptionType::Sub_Command_Group) {
			data["required"] = this->required;
		}
		if (this->descriptionLocalizations.size() > 0) {
			data["description_localizations"] = this->descriptionLocalizations;
		}
		if (this->nameLocalizations.size() > 0) {
			data["name_localizations"] = this->nameLocalizations;
		}
		data["description"] = this->description;
		if (this->minValue != std::numeric_limits<int32_t>::max()) {
			data["min_value"] = this->minValue;
		}
		if (this->maxValue != std::numeric_limits<int32_t>::min()) {
			data["max_value"] = this->maxValue;
		}
		data["required"] = this->required;
		data["name"] = this->name;
		data["type"] = this->type;
		for (auto& value: this->choices) {
			data["choices"].emplaceBack(value.operator Jsonifier());
		}
		if (this->choices.size() == 0) {
			data["autocomplete"] = this->autocomplete;
		}
		for (auto& value: this->options) {
			data["options"].emplaceBack(value.operator Jsonifier());
		}
		return data;
	}

	ThreadMemberDataVector::operator std::vector<ThreadMemberData>() {
		return this->threadMembers;
	}

	YouTubeFormatVector::operator std::vector<YouTubeFormat>() {
		return this->formats;
	}

	AudioFrameData& AudioFrameData::operator+=(std::basic_string_view<uint8_t> other) noexcept {
		if (this->data.size() < this->currentSize + other.size()) {
			this->data.resize(other.size() + this->currentSize);
		}
		std::copy(other.data(), other.data() + other.size(), this->data.data() + this->currentSize);
		this->currentSize += other.size();
		this->sampleCount = this->currentSize / 2 / 2;
		this->type = AudioFrameType::RawPCM;
		return *this;
	}

	AudioFrameData& AudioFrameData::operator+=(uint8_t character) {
		this->currentSize++;
		this->data.push_back(character);
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
		if (lhs.sampleCount != rhs.sampleCount) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		return true;
	}

	AudioFrameData& AudioFrameData::operator=(AudioFrameData&& other) noexcept {
		if (this->data.size() < other.data.size()) {
			this->data.resize(other.data.size());
		}
		this->currentSize = other.data.size();
		std::copy(other.data.data(), other.data.data() + other.data.size(), this->data.data());
		this->guildMemberId = std::move(other.guildMemberId);
		this->sampleCount = other.sampleCount;
		this->type = other.type;
		return *this;
	}

	AudioFrameData::AudioFrameData(AudioFrameData&& other) noexcept {
		*this = std::move(other);
	}

	void AudioFrameData::clearData() noexcept {
		this->type = AudioFrameType::Unset;
		this->guildMemberId = 0;
		this->sampleCount = -1;
		this->currentSize = 0;
		this->data.clear();
	}

	AllowedMentionsData::operator Jsonifier() {
		Jsonifier data{};
		for (auto& value: this->parse) {
			data["parse"].emplaceBack(value);
		}
		for (auto& value: this->roles) {
			data["roles"].emplaceBack(value);
		}
		for (auto& value: this->users) {
			data["users"].emplaceBack(value);
		}
		data["replied_user"] = this->repliedUser;
		return data;
	}

	ActionRowData::operator Jsonifier() {
		Jsonifier data{};
		data["type"] = 1;
		if (this->components.size() > 0) {
			for (auto& valueNew: this->components) {
				if (valueNew.type == ComponentType::Button) {
					Jsonifier component{};
					if (valueNew.emoji.name != "") {
						component["emoji"]["animated"] = valueNew.emoji.animated;
						component["emoji"]["name"] = std::string{ valueNew.emoji.name };
						if (valueNew.emoji.id != 0) {
							component["emoji"]["id"] = valueNew.emoji.id;
						}
					}
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["label"] = valueNew.label;
					component["style"] = valueNew.style;
					component["type"] = valueNew.type;
					component["url"] = valueNew.url;
					data["components"].emplaceBack(component);
				} else if (valueNew.type == ComponentType::String_Select || valueNew.type == ComponentType::User_Select ||
					valueNew.type == ComponentType::Role_Select || valueNew.type == ComponentType::Mentionable_Select ||
					valueNew.type == ComponentType::Channel_Select) {
					Jsonifier component{};
					for (auto& value01: valueNew.options) {
						Jsonifier option{};
						if (valueNew.emoji.name != "") {
							component["emoji"]["animated"] = valueNew.emoji.animated;
							component["emoji"]["name"] = std::string{ valueNew.emoji.name };
							if (valueNew.emoji.id != 0) {
								component["emoji"]["id"] = valueNew.emoji.id;
							}
						}
						option["description"] = value01.description;
						option["default"] = value01._default;
						option["label"] = value01.label;
						option["value"] = value01.value;
						component["options"].emplaceBack(option);
					};
					component["placeholder"] = valueNew.placeholder;
					component["max_values"] = valueNew.maxValues;
					component["min_values"] = valueNew.minValues;
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["type"] = valueNew.type;
					data["components"].emplaceBack(component);

				} else if (valueNew.type == ComponentType::Text_Input) {
					Jsonifier component{};
					component["placeholder"] = valueNew.placeholder;
					component["min_length"] = valueNew.minLength;
					component["max_length"] = valueNew.maxLength;
					component["custom_id"] = valueNew.customId;
					component["required"] = valueNew.required;
					component["style"] = valueNew.style;
					component["label"] = valueNew.label;
					component["value"] = valueNew.value;
					component["type"] = valueNew.type;
					data["components"].emplaceBack(component);
				}
			}
		}
		return data;
	}

	MessageData& MessageData::operator=(const MessageData& other) {
		if (this != &other) {
			if (other.referencedMessage) {
				*this->referencedMessage = *other.referencedMessage;
			}
			this->messageReference = other.messageReference;
			this->mentionEveryone = other.mentionEveryone;
			this->mentionChannels = other.mentionChannels;
			this->editedTimestamp = other.editedTimestamp;
			this->applicationId = other.applicationId;
			this->stickerItems = other.stickerItems;
			this->mentionRoles = other.mentionRoles;
			this->application = other.application;
			this->interaction = other.interaction;
			this->attachments = other.attachments;
			this->components = other.components;
			this->timeStamp = other.timeStamp;
			this->channelId = other.channelId;
			this->webHookId = other.webHookId;
			this->reactions = other.reactions;
			this->activity = other.activity;
			this->mentions = other.mentions;
			this->stickers = other.stickers;
			this->content = other.content;
			this->guildId = other.guildId;
			this->member = other.member;
			this->thread = other.thread;
			this->pinned = other.pinned;
			this->author = other.author;
			this->embeds = other.embeds;
			this->nonce = other.nonce;
			this->flags = other.flags;
			this->type = other.type;
			this->tts = other.tts;
			this->id = other.id;
		}
		return *this;
	}

	MessageData::MessageData(const MessageData& other) {
		*this = other;
	}

	StickerPackDataVector::operator std::vector<StickerPackData>() {
		return this->stickerPacks;
	}

	ConnectionDataVector::operator std::vector<ConnectionData>() {
		return this->connections;
	}

	ConnectionDataVector::ConnectionDataVector(simdjson::ondemand::value jsonObjectData) noexcept {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array arrayValue{};
			if (getArray(arrayValue, jsonObjectData)) {
				for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
					ConnectionData newData{ value.value() };
					this->connections.emplace_back(std::move(newData));
				}
			}
		}
	}

	VoiceRegionDataVector::operator std::vector<VoiceRegionData>() {
		return this->voiceRegions;
	}

	IntegrationDataVector::operator std::vector<IntegrationData>() {
		return this->integeregrations;
	}

	InputEventData& InputEventData::operator=(InputEventData&& other) noexcept {
		if (this != &other) {
			*this->interactionData = std::move(*other.interactionData);
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData&& other) noexcept {
		*this = std::move(other);
	}

	InputEventData& InputEventData::operator=(const InputEventData& other) noexcept {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(const InputEventData& other) noexcept {
		*this = other;
	}

	InputEventData& InputEventData::operator=(const InteractionData& other) noexcept {
		*this->interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(const InteractionData& interactionData) noexcept {
		*this = interactionData;
	}

	UserData InputEventData::getUserData() const {
		UserData returnData{};
		returnData.id = this->interactionData->user.id;
		returnData = Users::getCachedUser({ .userId = returnData.id });
		return this->interactionData->user;
	}

	ChannelData InputEventData::getChannelData() const {
		ChannelData returnData{};
		returnData.id = this->interactionData->channelId;
		returnData = Channels::getCachedChannel({ returnData.id });
		return returnData;
	}

	GuildMemberData InputEventData::getGuildMemberData() const {
		GuildMemberData returnData{ this->interactionData->member };
		returnData = GuildMembers::getCachedGuildMember({ .guildMemberId = returnData.id, .guildId = this->interactionData->guildId });
		return returnData;
	}

	GuildData InputEventData::getGuildData() const {
		GuildData returnData{};
		returnData.id = this->interactionData->guildId;
		returnData = Guilds::getCachedGuild({ .guildId = this->interactionData->guildId });
		return returnData;
	}

	InteractionData InputEventData::getInteractionData() const {
		return *this->interactionData;
	}

	MessageData InputEventData::getMessageData() const {
		return this->interactionData->message;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InteractionData& dataPackage) {
		this->applicationId = dataPackage.applicationId;
		this->interactionToken = dataPackage.token;
		this->messageId = dataPackage.message.id;
		this->channelId = dataPackage.channelId;
		this->interactionId = dataPackage.id;
		this->eventType = dataPackage.type;
		return *this;
	};

	RespondToInputEventData::RespondToInputEventData(const InteractionData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InputEventData& dataPackage) {
		this->applicationId = dataPackage.getInteractionData().applicationId;
		this->interactionToken = dataPackage.getInteractionData().token;
		this->interactionId = dataPackage.getInteractionData().id;
		this->channelId = dataPackage.getChannelData().id;
		this->messageId = dataPackage.getMessageData().id;
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(const InputEventData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, const std::string& customIdNew,
		std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type,
		std::vector<ChannelType> channelTypes) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew,
		const std::string& titleNew, const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
		const std::string& label, const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
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
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addFile(File theFile) {
		this->files.emplace_back(theFile);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setResponseType(InputEventResponseType typeNew) {
		this->type = typeNew;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addComponentRow(ActionRowData dataPackage) {
		this->components.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(Jsonifier value, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name = theName;
		switch (value.getType()) {
			case JsonType::String: {
				choiceData.type = JsonType::String;
				break;
			}
			case JsonType::Float: {
				choiceData.type = JsonType::Float;
				break;
			}
			case JsonType::Uint64: {
				choiceData.type = JsonType::Uint64;
				break;
			}
			case JsonType::Int64: {
				choiceData.type = JsonType::Int64;
				break;
			}
			case JsonType::Bool: {
				choiceData.type = JsonType::Bool;
				break;
			}
		}
		value.refreshString(JsonifierSerializeType::Json);
		choiceData.value = value.operator std::string();
		this->choices.emplace_back(choiceData);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const Snowflake targetUserIdNew) {
		this->targetUserId = targetUserIdNew;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type, std::vector<ChannelType> channelTypes) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label,
		const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
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
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addFile(File theFile) {
		this->files.emplace_back(theFile);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addComponentRow(ActionRowData dataPackage) {
		this->components.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	InteractionResponseData& InteractionResponseData::operator=(const RespondToInputEventData& other) {
		this->data.allowedMentions = other.allowedMentions;
		this->data.components = other.components;
		this->data.customId = other.customId;
		this->data.choices = other.choices;
		this->data.content = other.content;
		this->data.embeds = other.embeds;
		this->data.title = other.title;
		this->data.files = other.files;
		this->data.flags = other.flags;
		this->data.tts = other.tts;
		return *this;
	}

	InteractionResponseData::InteractionResponseData(const RespondToInputEventData& other) {
		*this = other;
	}

	InteractionResponseData::operator Jsonifier() {
		Jsonifier data{};
		data["type"] = this->type;
		if (this->data.attachments.size() > 0) {
			for (auto& value: this->data.attachments) {
				data["data"]["attachments"].emplaceBack(value);
			}
		}
		if (this->data.components.size() == 0) {
			data["data"]["components"].emplaceBack(ActionRowData{});
			data["data"]["components"].getValue<Jsonifier::ArrayType>().clear();
		} else {
			for (auto& value: this->data.components) {
				data["data"]["components"].emplaceBack(value);
			}
		}
		if (this->data.allowedMentions.parse.size() > 0 || this->data.allowedMentions.roles.size() > 0 ||
			this->data.allowedMentions.users.size() > 0) {
			data["data"]["allowed_mentions"] = this->data.allowedMentions.operator Jsonifier();
		}
		if (this->data.choices.size() > 0) {
			for (auto& value: this->data.choices) {
				Jsonifier valueNew{};
				valueNew["name"] = value.name;
				valueNew["name_localizations"] = value.nameLocalizations;
				valueNew["value"] = value.value;
				data["data"]["choices"].emplaceBack(value);
			}
		}
		if (this->data.embeds.size() == 0) {
			data["data"]["embeds"].emplaceBack(EmbedData{});
			data["data"]["embeds"].getValue<Jsonifier::ArrayType>().clear();
		} else {
			for (auto& value: this->data.embeds) {
				data["data"]["embeds"].emplaceBack(value);
			}
		}
		if (this->data.customId != "") {
			data["data"]["custom_id"] = this->data.customId;
		}
		if (this->data.content != "") {
			data["data"]["content"] = this->data.content;
		}
		if (this->data.title != "") {
			data["data"]["title"] = this->data.title;
		}
		data["data"]["flags"] = this->data.flags;
		data["data"]["tts"] = this->data.tts;
		return data;
	}

	void parseCommandDataOption(std::unordered_map<std::string, JsonStringValue>& values, ApplicationCommandInteractionDataOption& data) {
		JsonStringValue valueNew{};
		valueNew.type = data.value.type;
		valueNew.value = data.value.value;
		values.insert_or_assign(data.name, valueNew);
		for (auto& value: data.options) {
			parseCommandDataOption(values, value);
		}
	}

	CommandData& CommandData::operator=(const CommandData& other) noexcept {
		this->subCommandGroupName = other.subCommandGroupName;
		this->subCommandName = other.subCommandName;
		this->commandName = other.commandName;
		this->optionsArgs = other.optionsArgs;
		this->eventData = other.eventData;
		return *this;
	}

	CommandData::CommandData(const CommandData& other) noexcept {
		*this = other;
	}

	CommandData::CommandData(InputEventData inputEventData) {
		if (inputEventData.interactionData->data.applicationCommandData.name != "") {
			this->commandName = inputEventData.interactionData->data.applicationCommandData.name;
		}
		if (inputEventData.interactionData->data.messageInteractionData.targetId != 0) {
			this->optionsArgs.values.emplace("target_id",
				JsonStringValue{ .value = inputEventData.interactionData->data.messageInteractionData.targetId, .type = JsonType::String });
		} else if (inputEventData.interactionData->data.userInteractionData.targetId != 0) {
			this->optionsArgs.values.emplace("target_id",
				JsonStringValue{ .value = inputEventData.interactionData->data.userInteractionData.targetId, .type = JsonType::String });
		}
		this->eventData = inputEventData;
		for (auto& value: this->eventData.interactionData->data.applicationCommandData.options) {
			JsonStringValue valueNew{};
			valueNew.type = value.value.type;
			valueNew.value = value.value.value;
			this->optionsArgs.values.insert_or_assign(value.name, valueNew);
			parseCommandDataOption(this->optionsArgs.values, value);
		}
		for (auto& value: inputEventData.interactionData->data.applicationCommandData.options) {
			if (value.type == ApplicationCommandOptionType::Sub_Command) {
				this->subCommandName = value.name;
			}
			if (value.type == ApplicationCommandOptionType::Sub_Command_Group) {
				this->subCommandGroupName = value.name;
			}
		}
	}

	InteractionData CommandData::getInteractionData() noexcept {
		return *this->eventData.interactionData;
	}

	GuildMemberData CommandData::getGuildMemberData() noexcept {
		return this->eventData.getGuildMemberData();
	}

	ChannelData CommandData::getChannelData() noexcept {
		return this->eventData.getChannelData();
	}

	MessageData CommandData::getMessageData() noexcept {
		return this->eventData.getMessageData();
	}

	GuildData CommandData::getGuildData() noexcept {
		return this->eventData.getGuildData();
	}

	UserData CommandData::getUserData() noexcept {
		return this->eventData.getUserData();
	}

	std::string CommandData::getCommandName() noexcept {
		return this->commandName;
	}

	std::string CommandData::getSubCommandName() noexcept {
		return this->subCommandName;
	}

	std::string CommandData::getSubCommandGroupName() noexcept {
		return this->subCommandGroupName;
	}

	JsonifierValue CommandData::getCommandArguments() noexcept {
		return this->optionsArgs;
	}

	InputEventData CommandData::getInputEventData() noexcept {
		return this->eventData;
	}

	BaseFunctionArguments::BaseFunctionArguments(CommandData commanddataNew, DiscordCoreClient* discordCoreClientNew) : CommandData(commanddataNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	MoveThroughMessagePagesData moveThroughMessagePages(const std::string& userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const std::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		uint32_t newCurrentPageIndex = currentPageIndex;
		StopWatch stopWatch{ Milliseconds{ waitForMaxMs } };
		auto createResponseData = std::make_unique<CreateInteractionResponseData>(originalEvent);
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
		auto newEvent = InputEvents::respondToInputEventAsync(*interactionResponse).get();
		while (!stopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
			std::unique_ptr<ButtonCollector> button{ std::make_unique<ButtonCollector>(newEvent) };
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
				for (auto& value: interactionData->message.components) {
					for (auto& value02: value.components) {
						value02.disabled = true;
					}
					dataPackage02->addComponentRow(value);
				}
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
				for (auto& value: interactionData->message.components) {
					dataPackage.addComponentRow(value);
				}
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
					for (auto& value: originalEvent.getMessageData().components) {
						for (auto& value02: value.components) {
							value02.disabled = true;
						}
						dataPackage.addComponentRow(value);
					}
					InputEvents::respondToInputEventAsync(dataPackage).get();
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
