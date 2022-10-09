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

	WebSocketResumeData::operator DiscordCoreAPI::JsonObject() {
		DiscordCoreAPI::JsonObject theData{};
		theData["op"] = 6;
		theData["d"]["seq"] = this->lastNumberReceived;
		theData["d"]["session_id"] = this->sessionId;
		theData["d"]["token"] = this->botToken;
		return theData;
	}

	WebSocketIdentifyData::operator DiscordCoreAPI::JsonObject() {
		DiscordCoreAPI::JsonObject theSerializer{};
		theSerializer["d"]["intents"] = this->intents;
		theSerializer["d"]["large_threshold"] = 250;
		for (auto& value: this->presence.activities) {
			DiscordCoreAPI::JsonObject theSerializer01{};
			if (value.url != "") {
				theSerializer01["url"] = value.url;
			}
			theSerializer01["name"] = value.name;
			theSerializer01["type"] = value.type;
			theSerializer["d"]["presence"]["activities"].pushBack(theSerializer01);
		}
		theSerializer["d"]["presence"]["afk"] = this->presence.afk;
		if (this->presence.since != 0) {
			theSerializer["d"]["presence"]["since"] = this->presence.since;
		}

		theSerializer["d"]["presence"]["status"] = this->presence.status;
		theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["d"]["properties"]["os"] = "Windows";
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		theSerializer["d"]["shard"].pushBack(this->currentShard);
		theSerializer["d"]["shard"].pushBack(this->numberOfShards);
		theSerializer["d"]["token"] = this->botToken;
		theSerializer["op"] = 2;
		return theSerializer;
	}

	VoiceSocketProtocolPayloadData::operator DiscordCoreAPI::JsonObject() {
		DiscordCoreAPI::JsonObject theData{};
		theData["op"] = 1;
		theData["d"]["protocol"] = "udp";
		theData["d"]["data"]["port"] = this->voicePort;
		theData["d"]["data"]["mode"] = this->voiceEncryptionMode;
		theData["d"]["data"]["address"] = this->externalIp;
		return theData;
	}

	UpdatePresenceData::operator DiscordCoreAPI::JsonObject() {
		DiscordCoreAPI::JsonObject theData{};
		theData["op"] = 3;
		for (auto& value: this->activities) {
			DiscordCoreAPI::JsonObject theDataNew{};
			if (value.url != "") {
				theDataNew["url"] = String{ value.url };
			}
			theDataNew["name"] = String{ value.name };
			theDataNew["type"] = value.type;
			theData["d"]["activities"].pushBack(theDataNew);
		}
		theData["status"] = this->status;
		if (this->since != 0) {
			theData["since"] = this->since;
		}
		theData["afk"] = this->afk;
		return theData;
	}

	VoiceIdentifyData::operator DiscordCoreAPI::JsonObject() {
		DiscordCoreAPI::JsonObject theData{};
		theData["op"] = 0;
		theData["d"]["session_id"] = this->connectionData.sessionId;
		theData["d"]["token"] = this->connectionData.token;
		theData["d"]["server_id"] = std::to_string(this->connectInitData.guildId);
		theData["d"]["user_id"] = std::to_string(this->connectInitData.userId);
		return theData;
	}

	SendSpeakingData::operator DiscordCoreAPI::JsonObject() {
		DiscordCoreAPI::JsonObject theData{};
		theData["op"] = 5;
		theData["d"]["speaking"] = this->type;
		theData["d"]["delay"] = this->delay;
		theData["d"]["ssrc"] = this->ssrc;
		return theData;
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

	HttpsWorkloadData::HttpsWorkloadData(DiscordCoreInternal::HttpsWorkloadType theType) noexcept {
		if (!HttpsWorkloadData::workloadIdsExternal.contains(theType)) {
			UniquePtr<AtomicInt64> theInt{ std::make_unique<AtomicInt64>() };
			UniquePtr<AtomicInt64> theInt02{ std::make_unique<AtomicInt64>() };
			HttpsWorkloadData::workloadIdsExternal[theType] = std::move(theInt);
			HttpsWorkloadData::workloadIdsInternal[theType] = std::move(theInt02);
		}
		this->thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(theType));
		this->workloadType = theType;
	}

	Int64 HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) noexcept {
		Int64 theValue{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load() };
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(theValue + 1);
		return theValue;
	}

	UMap<HttpsWorkloadType, UniquePtr<AtomicInt64>> HttpsWorkloadData::workloadIdsExternal{};
	UMap<HttpsWorkloadType, UniquePtr<AtomicInt64>> HttpsWorkloadData::workloadIdsInternal{};

	HelloData::HelloData(simdjson::ondemand::value jsonObjectData) {
		this->heartbeatInterval = DiscordCoreAPI::getUint64(jsonObjectData, "heartbeat_interval");
	}

	WebSocketMessage::WebSocketMessage(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::object theValue{};
		if (jsonObjectData.get(theValue) == simdjson::error_code::SUCCESS) {
			this->op = DiscordCoreAPI::getUint32(theValue, "op");

			this->s = DiscordCoreAPI::getUint32(theValue, "s");

			this->t = DiscordCoreAPI::getString(theValue, "t");

			if (jsonObjectData["d"].get(this->d) != simdjson::error_code::SUCCESS) {
				throw std::runtime_error{ "Failed to collect the 'd'." };
			}
		}
	}

	InvalidSessionData::InvalidSessionData(simdjson::ondemand::value jsonObjectData) {
		this->d = DiscordCoreAPI::getBool(jsonObjectData, "d");
	}

	ReadyData::ReadyData(simdjson::ondemand::value jsonObjectData) {
		this->resumeGatewayUrl = DiscordCoreAPI::getString(jsonObjectData, "resume_gateway_url");

		this->sessionId = DiscordCoreAPI::getString(jsonObjectData, "session_id");

		this->v = DiscordCoreAPI::getUint32(jsonObjectData, "v");

		simdjson::ondemand::value theUser{};
		if (jsonObjectData["user"].get(theUser) == simdjson::error_code::SUCCESS) {
			this->user = DiscordCoreAPI::UserData{ theUser };
		}
	}
}

namespace DiscordCoreAPI {

	String DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
		TimeStamp<std::chrono::milliseconds> timeStamp{ (static_cast<Uint64>(this->id) >> 22) + 1420070400000, timeFormat };
		return timeStamp.operator DiscordCoreAPI::String();
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

		this->flags |= setBool(this->flags, UserFlags::MFAEnabled, getBool(jsonObjectData, "mfa_enabled"));

		this->flags |= setBool(this->flags, UserFlags::Verified, getBool(jsonObjectData, "verified"));

		this->flags |= setBool(this->flags, UserFlags::System, getBool(jsonObjectData, "system"));

		this->flags |= setBool(this->flags, UserFlags::Bot, getBool(jsonObjectData, "bot"));

		this->flags |= getUint32(jsonObjectData, "public_flags");

		this->userName = getString(jsonObjectData, "username");

		this->avatar = getString(jsonObjectData, "avatar");

		this->discriminator = getString(jsonObjectData, "discriminator");
	}

	String UserData::getAvatarUrl() {
		String theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "avatars/" + std::to_string(this->id) + "/" + this->avatar.getIconHash();
		return theStringNew;
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

		this->timeStamp = getString(jsonObjectData, "timeStamp");

		this->hexColorValue = getUint32(jsonObjectData, "color");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["footer"].get(theObject) == simdjson::error_code::SUCCESS) {
			EmbedFooterData theData{ theObject };
			this->footer = std::move(theData);
		}

		if (jsonObjectData["image"].get(theObject) == simdjson::error_code::SUCCESS) {
			EmbedImageData theData{ theObject };
			this->image = std::move(theData);
		}

		if (jsonObjectData["provider"].get(theObject) == simdjson::error_code::SUCCESS) {
			EmbedProviderData theData{ theObject };
			this->provider = std::move(theData);
		}

		if (jsonObjectData["thumbnail"].get(theObject) == simdjson::error_code::SUCCESS) {
			EmbedThumbnailData theData{ theObject };
			this->thumbnail = std::move(theData);
		}

		if (jsonObjectData["video"].get(theObject) == simdjson::error_code::SUCCESS) {
			EmbedVideoData theData{ theObject };
			this->video = std::move(theData);
		}

		if (jsonObjectData["author"].get(theObject) == simdjson::error_code::SUCCESS) {
			EmbedAuthorData theData{ theObject };
			this->author = std::move(theData);
		}

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["fields"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->fields.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ThreadMemberData newData{ value.value() };
					this->theThreadMemberDatas.emplace_back(std::move(newData));
				}
			}
		}
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

		StringStream theStream{};
		theStream << getString(jsonObjectData, "unicode_emoji");
		for (auto& value: theStream.str()) {
			this->unicodeEmoji.emplace_back(value);
		}
		if (this->unicodeEmoji.size() > 3) {
			this->unicodeEmoji = static_cast<String>(this->unicodeEmoji).substr(1, this->unicodeEmoji.size() - 3);
		}

		this->guildId = getId(jsonObjectData, "guild_id");

		this->color = getUint32(jsonObjectData, "color");

		this->flags |= setBool(this->flags, RoleFlags::Hoist, getBool(jsonObjectData, "hoist"));

		this->flags |= setBool(this->flags, RoleFlags::Managed, getBool(jsonObjectData, "managed"));

		this->flags |= setBool(this->flags, RoleFlags::Mentionable, getBool(jsonObjectData, "mentionable"));

		this->position = getUint32(jsonObjectData, "position");

		this->permissions = getString(jsonObjectData, "permissions");
	}

	EmojiData::EmojiData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["roles"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				RoleData newData{ value.value() };
				this->roles.emplace_back(std::move(newData));
			}
		}

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}

		this->requireColons = getBool(jsonObjectData, "require_colons");

		this->managed = getBool(jsonObjectData, "managed");

		this->animated = getBool(jsonObjectData, "animated");

		this->available = getBool(jsonObjectData, "available");
	}

	PresenceUpdateData::PresenceUpdateData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		auto theStringNew = getString(jsonObjectData, "status");

		if (theStringNew == "online") {
			this->theStatus = setBool(this->theStatus, PresenceUpdateFlags::Status_Online, true);
		} else if (theStringNew == "idle") {
			this->theStatus = setBool(this->theStatus, PresenceUpdateFlags::Status_Idle, true);
		} else if (theStringNew == "dnd") {
			this->theStatus = setBool(this->theStatus, PresenceUpdateFlags::Status_Dnd, true);
		}

		PresenceUpdateFlags theFlags{};
		parseObject(jsonObjectData, theFlags);

		this->theStatus |= static_cast<Uint8>(theFlags);
	}

	GuildMemberData& GuildMemberData::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			this->permissions = std::move(other.permissions);
			this->voiceChannelId = other.voiceChannelId;
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

	GuildMemberData::GuildMemberData(simdjson::ondemand::value jsonObjectData) {
		this->flags |= setBool(this->flags, GuildMemberFlags::Pending, getBool(jsonObjectData, "pending"));

		this->flags |= setBool(this->flags, GuildMemberFlags::Mute, getBool(jsonObjectData, "mute"));

		this->flags |= setBool(this->flags, GuildMemberFlags::Deaf, getBool(jsonObjectData, "deaf"));

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->guildId = getId(jsonObjectData, "guild_id");
		try {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData["roles"].get(theArray) == simdjson::error_code::SUCCESS) {
				this->roles.clear();
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					this->roles.emplace_back(getId(value.value()));
				}
			}
		} catch (...) {
			reportException("GuildMemberData::GuildMemberData()");
		}

		this->permissions = getString(jsonObjectData, "permissions");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			UserData theUser{ theObject };
			this->id = theUser.id;
			Users::insertUser(std::move(theUser));
		}

		this->avatar = getString(jsonObjectData, "avatar");

		this->flags |= getUint8(jsonObjectData, "flags");

		this->nick = getString(jsonObjectData, "nick");
	}

	String GuildMemberData::getAvatarUrl() {
		if (this->avatar.getIconHash() != "") {
			String theStringNew{ "https://cdn.discordapp.com/" };
			theStringNew += "guilds/" + std::to_string(this->guildId) + "/users/" + std::to_string(this->id) + "/avatars/" + this->avatar.getIconHash();
			return theStringNew;
		} else {
			return this->getUserData().getAvatarUrl();
		}
	}

	UserData GuildMemberData::getUserData() {
		if (this->id != 0) {
			return Users::getCachedUserAsync({ .userId = this->id }).get();
		} else {
			return {};
		}
	}

	OverWriteData::OverWriteData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->allow = getId(jsonObjectData, "allow");

		this->deny = getId(jsonObjectData, "deny");

		this->type = static_cast<PermissionOverwritesType>(getUint8(jsonObjectData, "type"));
	}

	ChannelData::ChannelData(simdjson::ondemand::value jsonObjectData) {
		this->flags |= setBool(this->flags, ChannelFlags::NSFW, getBool(jsonObjectData, "nsfw"));

		this->type = static_cast<ChannelType>(getUint8(jsonObjectData, "type"));

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->id = getId(jsonObjectData, "id");

		this->flags |= getUint8(jsonObjectData, "flags");

		this->parentId = getId(jsonObjectData, "parent_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->position = getUint32(jsonObjectData, "position");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["permission_overwrites"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->permissionOverwrites.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				OverWriteData theDataNew{ value.value() };
				this->permissionOverwrites.emplace_back(std::move(theDataNew));
			}
		}

		this->name = getString(jsonObjectData, "name");
	}

	ActiveThreadsData::ActiveThreadsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["threads"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ThreadMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		this->hasMore = getBool(jsonObjectData, "has_more");
	}

	ArchivedThreadsData::ArchivedThreadsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["threads"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ThreadMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		this->hasMore = getBool(jsonObjectData, "has_more");
	}

	TriggerMetaData::TriggerMetaData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["keyword_filter"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->keywordFilter.emplace_back(value.get_string().value().data());
			}
		}

		if (jsonObjectData["keyword_filter"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->presets.emplace_back(static_cast<KeywordPresetType>(value.get_uint64().value()));
			}
		}
	}

	ActionMetaData::ActionMetaData(simdjson::ondemand::value jsonObjectData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->durationSeconds = getUint64(jsonObjectData, "duration_seconds");
	}

	ActionData::ActionData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value theObject{};
		if (jsonObjectData["metadata"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->metadata = ActionMetaData{ theObject };
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

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["actions"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ActionData newData{ value.value() };
				this->actions.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["exempt_roles"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->exemptRoles.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->exemptRoles.emplace_back(DiscordCoreAPI::strtoull(String{ value.get_string().take_value() }));
			}
		}

		if (jsonObjectData["exempt_channels"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->exemptChannels.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->exemptChannels.emplace_back(DiscordCoreAPI::strtoull(String{ value.get_string().take_value() }));
			}
		}

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["trigger_metadata"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->triggerMetaData = TriggerMetaData{ theObject };
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

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["permissions"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->permissions.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ApplicationCommandPermissionData newData{ value.value() };
				this->permissions.emplace_back(std::move(newData));
			}
		}
	}

	GuildApplicationCommandPermissionsDataVector::GuildApplicationCommandPermissionsDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildApplicationCommandPermissionsData newData{ value.value() };
					this->theGuildApplicationCommandPermissionsDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	EmojiDataVector::EmojiDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					EmojiData newData{ value.value() };
					this->theEmojiDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	ReactionData::ReactionData(simdjson::ondemand::value jsonObjectData) {
		this->count = getUint32(jsonObjectData, "count");

		this->me = getBool(jsonObjectData, "me");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["emoji"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->emoji = EmojiData{ theObject };
		}

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->userId = getId(jsonObjectData, "user_id");

		this->messageId = getId(jsonObjectData, "message_id");

		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
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
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					VoiceRegionData newData{ value.value() };
					this->theVoiceRegionDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	MessageActivityData::MessageActivityData(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<MessageActivityType>(getUint8(jsonObjectData, "type"));

		this->partyId = getString(jsonObjectData, "party_id");
	}

	BanData::BanData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}

		this->reason = getString(jsonObjectData, "reason");
	}

	BanDataVector::BanDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					BanData newData{ value.value() };
					this->theBanDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	TeamMembersObjectData::TeamMembersObjectData(simdjson::ondemand::value jsonObjectData) {
		this->membershipState = getUint32(jsonObjectData, "membership_state");

		this->teamId = std::to_string(getId(jsonObjectData, "team_id"));

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["permissions"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->permissions.emplace_back(Permissions{ value.get_string().value().data() });
			}
		}

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}
	}

	TeamObjectData::TeamObjectData(simdjson::ondemand::value jsonObjectData) {
		this->icon = getString(jsonObjectData, "icon");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				TeamMembersObjectData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		this->ownerUserId = getId(jsonObjectData, "owner_user_id");
	}

	InstallParamsData::InstallParamsData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["scopes"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->scopes.emplace_back(Permissions{ value.get_string().value().data() });
			}
		}

		this->permissions = getString(jsonObjectData, "name");
	}

	ApplicationData::ApplicationData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value theObject{};
		if (jsonObjectData["params"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->params = InstallParamsData{ theObject };
		}

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["tags"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->tags.emplace_back(value.get_string().value().data());
			}
		}

		this->id = getId(jsonObjectData, "id");

		this->name = getString(jsonObjectData, "name");

		this->icon = getString(jsonObjectData, "icon");

		this->description = getString(jsonObjectData, "description");

		if (jsonObjectData["rpc_origins"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->rpcOrigins.emplace_back(value.get_string().value().data());
			}
		}

		this->botPublic = getBool(jsonObjectData, "bot_public");

		this->botRequireCodeGrant = getBool(jsonObjectData, "bot_require_code_grant");

		this->termsOfServiceUrl = getString(jsonObjectData, "terms_of_service_url");

		this->privacyPolicyUrl = getString(jsonObjectData, "privacy_policy_url");

		if (jsonObjectData["owner"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->owner = UserData{ theObject };
		}

		if (jsonObjectData["team"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->team = TeamObjectData{ theObject };
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
		simdjson::ondemand::value theObject{};
		if (jsonObjectData["application"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->application = ApplicationData{ theObject };
		}

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["features"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->scopes.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->scopes.emplace_back(String{ value.get_string().take_value() });
			}
		}

		this->expires = getString(jsonObjectData, "expires");

		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
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

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}

		if (jsonObjectData["account"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->account = AccountData{ theObject };
		}

		if (jsonObjectData["application"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->application = ApplicationData{ theObject };
		}

		this->syncedAt = getString(jsonObjectData, "synced_at");

		this->subscriberCount = getUint32(jsonObjectData, "subscriber_count");

		this->revoked = getBool(jsonObjectData, "revoked");
	}

	IntegrationDataVector::IntegrationDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					IntegrationData newData{ value.value() };
					this->theIntegrationDatas.emplace_back(std::move(newData));
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
		this->newValue.theValue = getString(jsonObjectData, "new_value");

		this->oldValue.theValue = getString(jsonObjectData, "old_value");

		this->key = getString(jsonObjectData, "key");
	}

	GuildPruneCountData::GuildPruneCountData(simdjson::ondemand::value jsonObjectData) {
		this->count = getUint32(jsonObjectData, "pruned");
	}

	AuditLogEntryData::AuditLogEntryData(simdjson::ondemand::value jsonObjectData) {
		this->targetId = getId(jsonObjectData, "target_id");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["changes"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				AuditLogChangeData newData{ value.value() };
				this->changes.emplace_back(std::move(newData));
			}
		}

		this->userId = getId(jsonObjectData, "user_id");

		this->id = getId(jsonObjectData, "id");
		this->createdTimeStamp = this->getCreatedAtTimestamp(TimeFormat::LongDateTime);

		this->actionType = static_cast<AuditLogEvent>(getUint16(jsonObjectData, "action_type"));

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["optinos"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->options = OptionalAuditEntryInfoData{ theObject };
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

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["welcome_channels"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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

		this->stickerFlags |= setBool(this->stickerFlags, StickerFlags::Available, getBool(jsonObjectData, "available"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->packId = getString(jsonObjectData, "pack_id");

		this->type = static_cast<StickerType>(getUint8(jsonObjectData, "type"));

		this->sortValue = getUint32(jsonObjectData, "sort_value");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}
	}

	GuildPreviewData::GuildPreviewData(simdjson::ondemand::value jsonObjectData) {
		this->approximatePresenceCount = getUint32(jsonObjectData, "approximate_presence_count");

		this->approximateMemberCount = getUint32(jsonObjectData, "approximate_member_count");

		this->discoverySplash = getString(jsonObjectData, "discovery_splash");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["emojis"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				EmojiData newData{ value.value() };
				this->emojis.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["stickers"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerData newData{ value.value() };
				this->stickers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["features"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->features.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->features.emplace_back(value.get_string().take_value());
			}
		}

		this->description = getString(jsonObjectData, "description");

		this->splash = getString(jsonObjectData, "splash");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		this->id = getId(jsonObjectData, "id");
	}

	VoiceConnection* GuildData::connectToVoice(const Snowflake guildMemberId, const Snowflake channelId, Bool selfDeaf, Bool selfMute, StreamType streamTypeNew,
		StreamInfo streamInfoNew) {
		if (DiscordCoreClient::getVoiceConnection(this->id) && DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected()) {
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else if (static_cast<Snowflake>(guildMemberId) != 0 || static_cast<Snowflake>(channelId) != 0) {
			Snowflake theChannelId{};
			if (static_cast<Snowflake>(guildMemberId) != 0) {
				auto guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = guildMemberId, .guildId = this->id }).get();
				if (guildMember.voiceChannelId != 0) {
					theChannelId = guildMember.voiceChannelId;
				}
			} else {
				theChannelId = channelId;
			}
			Uint64 theShardId{ (this->id >> 22) % this->discordCoreClient->configManager.getTotalShardCount() };
			auto theBaseSocketAgentIndex{ static_cast<Int32>(floor(static_cast<Float>(theShardId) /
				static_cast<Float>(this->discordCoreClient->configManager.getTotalShardCount()) * this->discordCoreClient->baseSocketAgentMap.size())) };
			DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = theShardId;
			voiceConnectInitData.streamType = streamTypeNew;
			voiceConnectInitData.streamInfo = streamInfoNew;
			voiceConnectInitData.channelId = theChannelId;
			voiceConnectInitData.guildId = this->id;
			voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			StopWatch theStopWatch{ 10000ms };
			DiscordCoreClient::getVoiceConnection(this->id)->connect(voiceConnectInitData);
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else {
			return nullptr;
		}
	}

	String GuildData::getIconUrl() noexcept {
		String theStringNew{ "https://cdn.discordapp.com/" };
		theStringNew += "icons/" + std::to_string(this->id) + "/" + this->icon.getIconHash() + ".png";
		return theStringNew;
	}

	Bool GuildData::areWeConnected() {
		return DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected();
	}

	Void GuildData::disconnect() {
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
		this->flags |= setBool(this->flags, GuildFlags::WidgetEnabled, getBool(jsonObjectData, "widget_enabled"));
		this->flags |= setBool(this->flags, GuildFlags::Unavailable, getBool(jsonObjectData, "unavailable"));

		this->flags |= setBool(this->flags, GuildFlags::Owner, getBool(jsonObjectData, "owner"));

		this->flags |= setBool(this->flags, GuildFlags::Large, getUint8(jsonObjectData, "large"));

		this->ownerId = getId(jsonObjectData, "owner_id");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		this->joinedAt = getString(jsonObjectData, "joined_at");

		this->id = getId(jsonObjectData, "id");

		this->icon = getString(jsonObjectData, "icon");

		this->name = getString(jsonObjectData, "name");

		this->threads.clear();

		simdjson::ondemand::array theArray01{};
		if (jsonObjectData["threads"].get(theArray01) == simdjson::error_code::SUCCESS) {
			this->threads.clear();
			for (auto value: theArray01) {
				auto& theObject = value.value();
				this->threads.emplace_back(getId(theObject, "id"));
			}
		}

		simdjson::ondemand::array theArray02{};
		if (jsonObjectData["stickers"].get(theArray02) == simdjson::error_code::SUCCESS) {
			this->stickers.clear();
			for (auto value: theArray02) {
				this->stickers.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray03{};
		if (jsonObjectData["guild_scheduled_events"].get(theArray03) == simdjson::error_code::SUCCESS) {
			this->guildScheduledEvents.clear();
			for (auto value: theArray03) {
				this->guildScheduledEvents.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray04{};
		if (jsonObjectData["stage_instances"].get(theArray04) == simdjson::error_code::SUCCESS) {
			this->stageInstances.clear();
			for (auto value: theArray04) {
				this->stageInstances.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray05{};
		if (jsonObjectData["emoji"].get(theArray05) == simdjson::error_code::SUCCESS) {
			this->emoji.clear();
			for (auto value: theArray05) {
				this->emoji.emplace_back(getId(value.value(), "id"));
			}
		}

		simdjson::ondemand::array theArray06{};
		if (Roles::doWeCacheRoles) {
			this->roles.clear();

			if (jsonObjectData["roles"].get(theArray06) == simdjson::error_code::SUCCESS) {
				for (auto value: theArray06) {
					RoleData newData{ value.value() };
					newData.guildId = this->id;
					this->roles.emplace_back(newData.id);
					Roles::insertRole(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray08{};
		if (GuildMembers::doWeCacheGuildMembers) {
			this->members.clear();
			if (jsonObjectData["members"].get(theArray08) == simdjson::error_code::SUCCESS) {
				for (auto value: theArray08) {
					GuildMemberData newData{ value.value() };
					newData.guildId = this->id;
					this->members.emplace_back(newData.id);
					GuildMembers::insertGuildMember(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray07{};
		if (GuildMembers::doWeCacheGuildMembers) {
			if (jsonObjectData["voice_states"].get(theArray07) == simdjson::error_code::SUCCESS) {
				for (auto value: theArray07) {
					GuildMemberData theDataNew{ value.value() };
					auto userId = getId(value.value(), "user_id");
					theDataNew.id = userId;
					theDataNew.guildId = this->id;
					GuildMembers::cache[theDataNew].voiceChannelId = getId(value.value(), "channel_id");
				}
			}
		}

		simdjson::ondemand::array theArray09{};
		if (GuildMembers::doWeCacheGuildMembers) {
			this->presences.clear();
			if (jsonObjectData["presences"].get(theArray09) == simdjson::error_code::SUCCESS) {
				for (auto value: theArray09) {
					PresenceUpdateData newData{ value.value() };
					newData.guildId = this->id;
					this->presences.emplace_back(std::move(newData));
				}
			}
		}

		simdjson::ondemand::array theArray10{};
		if (Channels::doWeCacheChannels) {
			this->channels.clear();
			if (jsonObjectData["channels"].get(theArray10) == simdjson::error_code::SUCCESS) {
				for (auto value: theArray10) {
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
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildData newData{ value.value() };
					this->theGuildDatas.emplace_back(std::move(newData));
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

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["entity_metadata"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->entityMetadata = GuildScheduledEventMetadata{ theObject };
		}

		this->scheduledStartTime = getString(jsonObjectData, "scheduled_start_time");

		this->scheduledEndTime = getString(jsonObjectData, "scheduled_end_time");

		this->userCount = getUint32(jsonObjectData, "user_count");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->creatorId = std::to_string(getId(jsonObjectData, "creator_id"));

		this->entityId = std::to_string(getId(jsonObjectData, "entity_id"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->description = getString(jsonObjectData, "description");

		if (jsonObjectData["creator"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->creator = UserData{ theObject };
		}

		this->name = getString(jsonObjectData, "name");
	}

	GuildScheduledEventUserData::GuildScheduledEventUserData(simdjson::ondemand::value jsonObjectData) {
		this->guildScheduledEventId = getId(jsonObjectData, "guild_scheduled_event_id");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
		}

		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}
	}

	GuildScheduledEventUserDataVector::GuildScheduledEventUserDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildScheduledEventUserData newData{ value.value() };
					this->theGuildScheduledEventUserDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	GuildScheduledEventDataVector::GuildScheduledEventDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildScheduledEventData newData{ value.value() };
					this->theGuildScheduledEventDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	InviteData::InviteData(simdjson::ondemand::value jsonObjectData) {
		this->code = getUint32(jsonObjectData, "code");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["guild"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->guild = GuildData{ theObject };
		}

		if (jsonObjectData["channel"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->channel = ChannelData{ theObject };
		}

		if (jsonObjectData["inviter"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->inviter = UserData{ theObject };
		}

		this->targetType = getUint32(jsonObjectData, "target_type");

		if (jsonObjectData["target_user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->targetUser = UserData{ theObject };
		}

		if (jsonObjectData["target_application"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->targetApplication = ApplicationData{ theObject };
		}

		if (jsonObjectData["stage_instance"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->stageInstance = StageInstanceData{ theObject };
		}

		if (jsonObjectData["guild_scheduled_event"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->guildScheduledEvent = GuildScheduledEventData{ theObject };
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
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					InviteData newData{ value.value() };
					this->theInviteDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	GuildTemplateData::GuildTemplateData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value theObject{};
		if (jsonObjectData["serialized_source_guild"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->serializedSourceGuild = GuildData{ theObject };
		}

		if (jsonObjectData["creator"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->creator = UserData{ theObject };
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
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					GuildTemplateData newData{ value.value() };
					this->theGuildTemplateDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	WebHookData::WebHookData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->type = static_cast<WebHookType>(getUint8(jsonObjectData, "type"));

		this->guildId = getId(jsonObjectData, "guild_id");

		this->channelId = getId(jsonObjectData, "channel_id");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}

		this->name = getString(jsonObjectData, "name");

		this->avatar = getString(jsonObjectData, "avatar");

		this->token = getString(jsonObjectData, "token");

		this->applicationId = getId(jsonObjectData, "application_id");

		if (jsonObjectData["source_guild"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->sourceGuild = GuildData{ theObject };
		}

		if (jsonObjectData["source_channel"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->sourceChannel = ChannelData{ theObject };
		}

		this->url = getString(jsonObjectData, "url");
	}

	WebHookDataVector::WebHookDataVector(simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					WebHookData newData{ value.value() };
					this->theWebHookDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	AuditLogData::AuditLogData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["webhooks"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				WebHookData newData{ value.value() };
				this->webhooks.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["guild_scheduled_events"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				GuildScheduledEventData newData{ value.value() };
				this->guildScheduledEvents.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["auto_moderation_rules"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				AutoModerationRuleData newData{ value.value() };
				this->autoModerationRules.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["users"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				UserData newData{ value.value() };
				this->users.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["audit_log_entries"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				AuditLogEntryData newData{ value.value() };
				this->auditLogEntries.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["integrations"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				IntegrationData newData{ value.value() };
				this->integrations.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["threads"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["emoji"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->emoji = EmojiData{ theObject };
		}
	}

	ApplicationCommandOptionChoiceData::ApplicationCommandOptionChoiceData(simdjson::ondemand::value jsonObjectData) {
		StringView theString{};
		auto theResult01 = jsonObjectData["value"].get(theString);
		Uint64 theInt{};
		auto theResult02 = jsonObjectData["value"].get(theInt);
		Bool theBool{};
		auto theResult03 = jsonObjectData["value"].get(theBool);
		Double theDouble{};
		auto theResult04 = jsonObjectData["value"].get(theDouble);

		if (theResult01 == simdjson::error_code::SUCCESS) {
			this->value = static_cast<String>(theString);
			this->type = JsonType::String;
		} else if (theResult02 == simdjson::error_code::SUCCESS) {
			this->value = std::to_string(theInt);
			this->type = JsonType::Integer;
		} else if (theResult03 == simdjson::error_code::SUCCESS) {
			this->type = JsonType::Boolean;
			StringStream theStream{};
			theStream << std::boolalpha << theBool;
			this->value = theStream.str();
		} else if (theResult04 == simdjson::error_code::SUCCESS) {
			this->type = JsonType::Float;
			this->value = std::to_string(theDouble);
		}

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::object theMap{};
		if (jsonObjectData["name_localizations"].get(theMap) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: theMap) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}
	}

	ApplicationCommandOptionData::ApplicationCommandOptionData(simdjson::ondemand::value jsonObjectData) {
		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::object theMap{};
		if (jsonObjectData["name_localizations"].get(theMap) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: theMap) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		if (jsonObjectData["description_localizations"].get(theMap) == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: theMap) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		this->description = getString(jsonObjectData, "description");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["channel_types"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->channelTypes.emplace_back(static_cast<ChannelType>(value.get_uint64().value()));
			}
		}

		this->minValue = getUint32(jsonObjectData, "min_value");

		this->maxValue = getUint32(jsonObjectData, "max_value");

		this->required = getBool(jsonObjectData, "required");

		this->autocomplete = getBool(jsonObjectData, "autocomplete");

		this->type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		if (this->type == ApplicationCommandOptionType::Sub_Command_Group || this->type == ApplicationCommandOptionType::Sub_Command) {
			auto theResult01 = jsonObjectData["options"].get(theArray);
			if (theResult01 == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ApplicationCommandOptionData newData{ value.value() };
					this->options.emplace_back(std::move(newData));
				}
			}
			auto theResult02 = jsonObjectData["choices"].get(theArray);
			if (theResult02 == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ApplicationCommandOptionChoiceData newData{ value.value() };
					this->choices.emplace_back(std::move(newData));
				}
			}
		}
	}

	bool operator==(const ApplicationCommandOptionData&lhs, const ApplicationCommandOptionData&rhs) {
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
		if (lhs.minValue!= rhs.minValue) {
			return false;
		}
		if (lhs.name!= rhs.name) {
			return false;
		}
		if (lhs.nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (lhs.options != rhs.options) {
			return false;
		}
		if (lhs.required != rhs.required) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		return true;
	}

	ApplicationCommandData::ApplicationCommandData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->defaultMemberPermissions = getString(jsonObjectData, "default_member_permissions");

		this->dmPermission = getBool(jsonObjectData, "dm_permission");

		this->version = getString(jsonObjectData, "version");

		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		simdjson::ondemand::object theMap{};
		if (jsonObjectData["name_localizations"].get(theMap) == simdjson::error_code::SUCCESS) {
			this->nameLocalizations.clear();
			for (auto value: theMap) {
				this->nameLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		if (jsonObjectData["description_localizations"].get(theMap) == simdjson::error_code::SUCCESS) {
			this->descriptionLocalizations.clear();
			for (auto value: theMap) {
				this->descriptionLocalizations.emplace(value.unescaped_key().take_value(), value.value().get_string().take_value());
			}
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		this->name = getString(jsonObjectData, "name");

		this->description = getString(jsonObjectData, "description");

		this->version = getString(jsonObjectData, "version");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["options"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->options.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ApplicationCommandOptionData theDataNew{ value.value() };
				this->options.emplace_back(std::move(theDataNew));
			}
		}
	}

	bool operator==(const ApplicationCommandData&lhs, const ApplicationCommandData&rhs) {
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
		for (size_t x = 0; x < lhs.options.size();++x) {
			if (lhs.options[x] != rhs.options[x]) {
				return false;
			}
		}
		return true;
	}

	TypingStartData::TypingStartData(simdjson::ondemand::value jsonObjectData) {
		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
		}

		this->userId = getId(jsonObjectData, "user_id");

		this->timeStamp = getUint32(jsonObjectData, "timeStamp");
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

		String theString = getString(jsonObjectData, "signatureCipher");
		if (theString == "") {
			theString = getString(jsonObjectData, "cipher");
		}

		this->signatureCipher = theString;

		auto ampersandSpFind = this->signatureCipher.find("&sp");
		if (ampersandSpFind != String::npos) {
			this->signature = this->signatureCipher.substr(0, ampersandSpFind);
		}

		auto urlFind = this->signatureCipher.find("url");
		if (urlFind != String::npos) {
			this->downloadUrl = this->signatureCipher.substr(urlFind + 4);
		} else {
			this->downloadUrl = getString(jsonObjectData, "url");
		}
	}

	YouTubeFormatVector::YouTubeFormatVector(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["streamingData"]["formats"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				YouTubeFormat newData{ value.value() };
				this->theFormats.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["streamingData"]["adaptiveFormats"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				YouTubeFormat newData{ value.value() };
				this->theFormats.emplace_back(std::move(newData));
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
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["values"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->values.clear();
			for (auto iterator = theArray.begin(); iterator != theArray.end(); ++iterator) {
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
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["parse"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->parse.emplace_back(static_cast<String>(value.get_string().value()));
			}
		}

		if (jsonObjectData["roles"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->roles.emplace_back(static_cast<String>(value.get_string().value()));
			}
		}

		if (jsonObjectData["users"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->users.emplace_back(static_cast<String>(value.get_string().value()));
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
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["components"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->components.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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

		simdjson::ondemand::value theEmoji{};
		if (jsonObjectData["emoji"].get(theEmoji) == simdjson::error_code::SUCCESS) {
			this->emoji = EmojiData{ theEmoji };
		}

		this->url = getString(jsonObjectData, "url");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["select_option_data"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->options.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				SelectOptionData newData{ value.value() };
				this->options.emplace_back(std::move(newData));
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

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["channel_ids"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->channelIds.emplace_back(value.get_string().take_value());
			}
		}

		if (jsonObjectData["members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ThreadMemberData newData{ value.value() };
				this->members.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["threads"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ChannelData newData{ value.value() };
				this->threads.emplace_back(std::move(newData));
			}
		}
	}

	ThreadMembersUpdateData::ThreadMembersUpdateData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		this->memberCount = getUint32(jsonObjectData, "member_count");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["added_members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ThreadMemberData newData{ value.value() };
				this->addedMembers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["added_members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ThreadMemberData newData{ value.value() };
				this->addedMembers.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["removed_member_ids"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->removedMemberIds.emplace_back(value.get_string().take_value());
			}
		}
	}

	MessageInteractionData::MessageInteractionData(simdjson::ondemand::value jsonObjectData) {
		this->id = getId(jsonObjectData, "id");

		this->type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}

		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
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

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["author"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->author = UserData{ theObject };
		}

		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
		}

		this->timeStamp = getString(jsonObjectData, "timeStamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");


		simdjson::ondemand::array theArray{};
		if (jsonObjectData["mentions"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->mentions.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				UserData newData{ value.value() };
				this->mentions.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["mention_roles"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->mentionRoles.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto theObject = value.get_string().take_value();
				this->mentionRoles.emplace_back(std::move(theObject));
			}
		}

		if (jsonObjectData["mention_channels"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->mentionChannels.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ChannelMentionData newChannelData{ value.value() };
				this->mentionChannels.emplace_back(std::move(newChannelData));
			}
		}

		if (jsonObjectData["attachments"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				AttachmentData newAttachmentData{ value.value() };
				this->attachments.emplace_back(std::move(newAttachmentData));
			}
		}

		if (jsonObjectData["embeds"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->embeds.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				EmbedData newEmbedData{ value.value() };
				this->embeds.emplace_back(std::move(newEmbedData));
			}
		}

		if (jsonObjectData["reactions"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->reactions.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ReactionData newReactionData{ value.value() };
				this->reactions.emplace_back(std::move(newReactionData));
			}
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		if (jsonObjectData["activity"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->activity = MessageActivityData{ theObject };
		}

		if (jsonObjectData["application"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->application = ApplicationData{ theObject };
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		if (jsonObjectData["message_reference"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->messageReference = MessageReferenceData{ theObject };
		}

		this->flags = getUint32(jsonObjectData, "flags");

		if (jsonObjectData["sticker_items"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->stickerItems.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerItemData newReactionData{ value.value() };
				this->stickerItems.emplace_back(std::move(newReactionData));
			}
		}

		if (jsonObjectData["stickers"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->stickers.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerData newReactionData{ value.value() };
				this->stickers.emplace_back(std::move(newReactionData));
			}
		}

		if (jsonObjectData["interaction"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->interaction = MessageInteractionData{ theObject };
		}

		if (jsonObjectData["components"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->components.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ActionRowData newActionRowData{ value.value() };
				this->components.emplace_back(std::move(newActionRowData));
			}
		}

		if (jsonObjectData["thread"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->thread = ChannelData{ theObject };
		}
	}

	MessageData::MessageData(simdjson::ondemand::value jsonObjectData) {
		this->content = getString(jsonObjectData, "content");

		this->id = getId(jsonObjectData, "id");

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["author"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->author = UserData{ theObject };
		}

		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
		}

		this->timeStamp = getString(jsonObjectData, "timeStamp");

		this->editedTimestamp = getString(jsonObjectData, "edited_timestamp");

		this->tts = getBool(jsonObjectData, "tts");

		this->mentionEveryone = getBool(jsonObjectData, "mention_everyone");


		simdjson::ondemand::array theArray{};
		if (jsonObjectData["mentions"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->mentions.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				UserData newData{ value.value() };
				this->mentions.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["mention_roles"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->mentionRoles.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				auto theObject = value.get_string().take_value();
				this->mentionRoles.emplace_back(std::move(theObject));
			}
		}

		if (jsonObjectData["mention_channels"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->mentionChannels.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ChannelMentionData newChannelData{ value.value() };
				this->mentionChannels.emplace_back(std::move(newChannelData));
			}
		}

		if (jsonObjectData["attachments"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				AttachmentData newAttachmentData{ value.value() };
				this->attachments.emplace_back(std::move(newAttachmentData));
			}
		}

		if (jsonObjectData["embeds"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->embeds.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				EmbedData newEmbedData{ value.value() };
				this->embeds.emplace_back(std::move(newEmbedData));
			}
		}

		if (jsonObjectData["reactions"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->reactions.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ReactionData newReactionData{ value.value() };
				this->reactions.emplace_back(std::move(newReactionData));
			}
		}

		this->nonce = getString(jsonObjectData, "nonce");

		this->pinned = getBool(jsonObjectData, "pinned");

		this->webHookId = getId(jsonObjectData, "webhook_id");

		this->type = static_cast<MessageType>(getUint8(jsonObjectData, "type"));

		if (jsonObjectData["activity"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->activity = MessageActivityData{ theObject };
		}

		if (jsonObjectData["application"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->application = ApplicationData{ theObject };
		}

		this->applicationId = getId(jsonObjectData, "application_id");

		if (jsonObjectData["referenced_message"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->referencedMessage = std::make_unique<MessageDataOld>(theObject);
		}

		if (jsonObjectData["message_reference"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->messageReference = MessageReferenceData{ theObject };
		}

		this->flags = getUint32(jsonObjectData, "flags");

		if (jsonObjectData["sticker_items"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->stickerItems.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerItemData newReactionData{ value.value() };
				this->stickerItems.emplace_back(std::move(newReactionData));
			}
		}

		if (jsonObjectData["stickers"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->stickers.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				StickerData newReactionData{ value.value() };
				this->stickers.emplace_back(std::move(newReactionData));
			}
		}

		if (jsonObjectData["interaction"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->interaction = MessageInteractionData{ theObject };
		}

		if (jsonObjectData["components"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->components.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ActionRowData newActionRowData{ value.value() };
				this->components.emplace_back(std::move(newActionRowData));
			}
		}

		if (jsonObjectData["thread"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->thread = ChannelData{ theObject };
		}
	}

	ResolvedData::ResolvedData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::object theArray{};
		if (jsonObjectData["attachments"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->attachments.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				AttachmentData newData{ value.value() };
				this->attachments[DiscordCoreAPI::strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}

		if (jsonObjectData["users"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->users.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				UserData newData{ value.value() };
				this->users[DiscordCoreAPI::strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}

		if (jsonObjectData["channels"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->channels.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				ChannelData newData{ value.value() };
				this->channels[DiscordCoreAPI::strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}

		if (jsonObjectData["roles"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->roles.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				RoleData newData{ value.value() };
				this->roles[DiscordCoreAPI::strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}

		if (jsonObjectData["members"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->members.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				GuildMemberData newData{ value.value() };
				this->members[DiscordCoreAPI::strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}

		if (jsonObjectData["messages"].get(theArray) == simdjson::error_code::SUCCESS) {
			this->messages.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::field> value: theArray) {
				MessageData newData{ value.value() };
				this->messages[DiscordCoreAPI::strtoull(std::string{ value.key().take_value().raw() })] = std::move(newData);
			}
		}
	}

	StickerPackData::StickerPackData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::array theArray{};
		if (jsonObjectData["stickers"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					StickerPackData newData{ value.value() };
					this->theStickerPackDatas.emplace_back(std::move(newData));
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

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["integrations"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				IntegrationData newData{ value.value() };
				this->integrations.emplace_back(std::move(newData));
			}
		}
	}

	ApplicationCommandInteractionDataOption::ApplicationCommandInteractionDataOption(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<ApplicationCommandOptionType>(getUint8(jsonObjectData, "type"));

		this->focused = getBool(jsonObjectData, "focused");

		this->name = getString(jsonObjectData, "name");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["options"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ApplicationCommandInteractionDataOption newData{ value.value() };
				this->options.emplace_back(std::move(newData));
			}
		}

		this->value.theType = ValueType::Null;
		Bool theBool{};
		if (jsonObjectData["value"].get(theBool) == simdjson::error_code::SUCCESS) {
			this->value.theType = ValueType::Bool;
			this->value.theValue = std::to_string(theBool);
			return;
		}

		Uint64 theInteger{};
		if (jsonObjectData["value"].get(theInteger) == simdjson::error_code::SUCCESS) {
			this->value.theType = ValueType::Uint64;
			this->value.theValue = std::to_string(theInteger);
			return;
		}

		StringView theString{};
		if (jsonObjectData["value"].get(theString) == simdjson::error_code::SUCCESS) {
			this->value.theType = ValueType::String;
			this->value.theValue = theString;
			return;
		}

		Double theFloat{};
		if (jsonObjectData["value"].get(theFloat) == simdjson::error_code::SUCCESS) {
			this->value.theType = ValueType::Float;
			this->value.theValue = std::to_string(theFloat);
			return;
		}
	}

	ApplicationCommandInteractionData::ApplicationCommandInteractionData(simdjson::ondemand::value jsonObjectData) {
		this->type = static_cast<ApplicationCommandType>(getUint8(jsonObjectData, "type"));

		this->name = getString(jsonObjectData, "name");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->id = getId(jsonObjectData, "id");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["options"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				ApplicationCommandInteractionDataOption newData{ value.value() };
				this->options.emplace_back(std::move(newData));
			}
		}

		simdjson::ondemand::value theObject{};
		if (jsonObjectData["resolved"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->resolved = ResolvedData{ theObject };
		}
	}

	InteractionDataData::InteractionDataData(simdjson::ondemand::value jsonObjectData) {
		StringView theObject{};
		if (jsonObjectData["id"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->applicationCommandData = ApplicationCommandInteractionData{ jsonObjectData };
		}

		if (jsonObjectData["target_id"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->messageInteractionData = MessageCommandInteractionData{ jsonObjectData };
			this->userInteractionData = UserCommandInteractionData{ jsonObjectData };
		}

		Uint64 componentType{};
		if (jsonObjectData["component_type"].get(componentType) == simdjson::error_code::SUCCESS) {
			this->componentData = ComponentInteractionData{ jsonObjectData };
		}

		simdjson::ondemand::value theObjectNew{};
		if (jsonObjectData["components"].get(theObjectNew) == simdjson::error_code::SUCCESS) {
			this->modalData = ModalInteractionData{ jsonObjectData };
		}
	}

	InteractionData::InteractionData(simdjson::ondemand::value jsonObjectData) {
		simdjson::ondemand::value theObject{};
		if (jsonObjectData["data"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->data = InteractionDataData{ theObject };
		}

		this->appPermissions = getString(jsonObjectData, "app_permissions");

		this->type = static_cast<InteractionType>(getUint8(jsonObjectData, "type"));

		this->token = getString(jsonObjectData, "token");

		if (jsonObjectData["member"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->member = GuildMemberData{ theObject };
			this->user.id = this->member.id;
			this->user.userName = this->member.getUserData().userName;
		}

		if (jsonObjectData["user"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->user = UserData{ theObject };
		}

		this->channelId = getId(jsonObjectData, "channel_id");

		this->guildId = getId(jsonObjectData, "guild_id");

		this->locale = getString(jsonObjectData, "locale");

		this->guildLocale = getString(jsonObjectData, "guild_locale");

		if (jsonObjectData["message"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->message = MessageData{ theObject };
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

		simdjson::ondemand::value theObject{};

		if (jsonObjectData["session_start_limit"].get(theObject) == simdjson::error_code::SUCCESS) {
			this->sessionStartLimit = SessionStartData{ theObject };
		}
	}

	GuildEmojisUpdateEventData::GuildEmojisUpdateEventData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["emojis"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				EmojiData newData{ value.value() };
				this->emojis.emplace_back(std::move(newData));
			}
		}
	}

	GuildStickersUpdateEventData::GuildStickersUpdateEventData(simdjson::ondemand::value jsonObjectData) {
		this->guildId = getId(jsonObjectData, "guild_id");

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["stickers"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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

		simdjson::ondemand::array theArray{};
		if (jsonObjectData["presences"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				PresenceUpdateData newData{ value.value() };
				this->presences.emplace_back(std::move(newData));
			}
		}

		if (jsonObjectData["not_found"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				this->notFound.emplace_back(value.get_string().take_value());
			}
		}

		if (jsonObjectData["members"].get(theArray) == simdjson::error_code::SUCCESS) {
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
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
		try {
			this->duration = getString(getObject(getObject(getObject(jsonObjectData, "lengthText"), "accessibility"), "accessibilityData"), "label");
			String newString = getString(getObject(getArray(getObject(getObject(getArray(jsonObjectData, "detailedMetadataSnippets"), 0), "snippetText"), "runs"), 0), "text");
			if (newString.size() > 256) {
				newString = newString.substr(0, 256);
			}
			this->description = utf8MakeValid(newString);

			this->thumbnailUrl = getString(getObject(getArray(getObject(jsonObjectData, "thumbnail"), "thumbnails"), 0), "url");
			String newTitle01 = getString(getObject(getArray(getObject(jsonObjectData, "title"), "runs"), 0), "text");
			if (newTitle01.size() > 256) {
				newTitle01 = newTitle01.substr(0, 256);
			}
			if (newTitle01.size() > 0) {
				this->songTitle = utf8MakeValid(newTitle01);
			}
			String newTitle02 = getString(getObject(jsonObjectData, "title"), "simpleText");
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

			Vector<MediaTranscoding> theMedia{};
			auto theArrayNew = getArray(getObject(jsonObjectData, "media"), "transcodings");
			if (theArrayNew.didItSucceed) {
				for (auto value: theArrayNew.theArray) {
					MediaTranscoding theDataNew{ value.value() };
					theMedia.emplace_back(theDataNew);
				}
			}

			Bool isItFound{ false };
			for (auto& value: theMedia) {
				if (value.thePreset == "opus_0_0") {
					isItFound = true;
					this->firstDownloadUrl = value.theUrl;
					this->songId = value.theUrl;
					this->doWeGetSaved = true;
				}
			}
			Bool isItFound2{ false };
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

			Uint32 theDuration = getUint32(jsonObjectData, "duration");
			if (theDuration != 0) {
				this->duration = TimeStamp<std::chrono::milliseconds>::convertMsToDurationString(theDuration);
			}

			newString = getString(jsonObjectData, "permalink_url");
			if (newString.size() > 0) {
				this->viewUrl = newString;
			}

		} catch (...) {
			reportException("Song::Song()");
		}
	}

	AttachmentData::operator JsonObject() {
		JsonObject theData{};
		theData["content_type"] = this->contentType;
		theData["description"] = this->description;
		theData["ephemeral"] = this->ephemeral;
		theData["file_name"] = this->filename;
		theData["proxy_url"] = this->proxyUrl;
		theData["height"] = this->height;
		theData["width"] = this->width;
		theData["size"] = this->size;
		theData["url"] = this->url;
		return theData;
	}

	EmbedFieldData::operator JsonObject() {
		JsonObject theData{};
		theData["inline"] = this->Inline;
		theData["value"] = escapeCharacters(this->value);
		theData["name"] = escapeCharacters(this->name);
		return theData;
	}

	EmbedData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value2: this->fields) {
			theData["fields"].pushBack(value2);
		}
		String realColorVal = std::to_string(this->hexColorValue.getIntColorValue());
		theData["footer"]["proxy_icon_url"] = this->footer.proxyIconUrl;
		theData["footer"]["icon_url"] = this->footer.iconUrl;
		theData["footer"]["text"] = escapeCharacters(this->footer.text);
		theData["author"]["proxy_icon_url"] = this->author.proxyIconUrl;
		theData["author"]["icon_url"] = this->author.iconUrl;
		theData["author"]["name"] = this->author.name;
		theData["author"]["url"] = this->author.url;
		theData["thumbnail"]["proxy_url"] = this->thumbnail.proxyUrl;
		theData["thumbnail"]["height"] = this->thumbnail.height;
		theData["thumbnail"]["width"] = this->thumbnail.width;
		theData["thumbnail"]["url"] = this->thumbnail.url;
		theData["image"]["proxy_url"] = this->image.proxyUrl;
		theData["image"]["height"] = this->image.height;
		theData["image"]["width"] = this->image.width;
		theData["image"]["url"] = this->image.url;
		theData["video"]["proxy_url"] = this->video.proxyUrl;
		theData["video"]["height"] = this->video.height;
		theData["video"]["url"] = this->video.url;
		theData["video"]["width"] = this->video.width;
		theData["provider"]["name"] = this->provider.name;
		theData["provider"]["url"] = this->provider.url;
		theData["description"] = escapeCharacters(this->description);
		theData["timeStamp"] = this->timeStamp;
		theData["title"] = escapeCharacters(this->title);
		theData["color"] = realColorVal;
		theData["type"] = this->type;
		theData["url"] = this->url;
		return theData;
	}

	EmbedData& EmbedData::setAuthor(const String& authorName, const String& authorAvatarUrl) {
		this->author.name = authorName;
		this->author.iconUrl = authorAvatarUrl;
		return *this;
	}

	EmbedData& EmbedData::setFooter(const String& footerText, const String& footerIconUrlText) {
		this->footer.text = footerText;
		this->footer.iconUrl = footerIconUrlText;
		return *this;
	}

	EmbedData& EmbedData::setTimeStamp(const String& timeStamp) {
		this->timeStamp = timeStamp;
		return *this;
	}

	EmbedData& EmbedData::addField(const String& name, const String& value, Bool Inline) {
		EmbedFieldData field{};
		field.Inline = Inline;
		field.value = value;
		field.name = name;
		this->fields.emplace_back(field);
		return *this;
	}

	EmbedData& EmbedData::setDescription(const String& descriptionNew) {
		this->description = descriptionNew;
		return *this;
	}

	EmbedData& EmbedData::setColor(const String& hexColorValueNew) {
		this->hexColorValue = hexColorValueNew;
		return *this;
	}

	EmbedData& EmbedData::setThumbnail(const String& thumbnailUrl) {
		this->thumbnail.url = thumbnailUrl;
		return *this;
	}

	EmbedData& EmbedData::setTitle(const String& titleNew) {
		this->title = titleNew;
		return *this;
	}

	EmbedData& EmbedData::setImage(const String& imageUrl) {
		this->image.url = imageUrl;
		return *this;
	}

	MessageReferenceData::operator JsonObject() {
		JsonObject theData{};
		theData["fail_if_not_exists"] = this->failIfNotExists;
		theData["message_id"] = std::to_string(this->messageId);
		theData["channel_id"] = std::to_string(this->channelId);
		theData["guild_id"] = std::to_string(this->guildId);
		return theData;
	}

	GuildApplicationCommandPermissionsDataVector::operator Vector<GuildApplicationCommandPermissionsData>() {
		return this->theGuildApplicationCommandPermissionsDatas;
	}

	BanDataVector::operator Vector<BanData>() {
		return this->theBanDatas;
	}

	UpdateVoiceStateData::operator JsonObject() {
		JsonObject theData{};
		theData["op"] = 4;
		if (this->channelId == 0) {
			theData["d"]["channel_id"] = ValueType::Null;
		} else {
			theData["d"]["channel_id"] = std::to_string(this->channelId);
		}
		theData["d"]["self_deaf"] = this->selfDeaf;
		theData["d"]["self_mute"] = this->selfMute;
		theData["d"]["guild_id"] = std::to_string(this->guildId);
		return theData;
	}

	GuildDataVector::operator Vector<GuildData>() {
		return this->theGuildDatas;
	}

	GuildScheduledEventUserDataVector::operator Vector<GuildScheduledEventUserData>() {
		return this->theGuildScheduledEventUserDatas;
	}

	GuildScheduledEventDataVector::operator Vector<GuildScheduledEventData>() {
		return this->theGuildScheduledEventDatas;
	}

	InviteDataVector::operator Vector<InviteData>() {
		return this->theInviteDatas;
	}

	GuildTemplateDataVector::operator Vector<GuildTemplateData>() {
		return this->theGuildTemplateDatas;
	}

	WebHookDataVector::operator Vector<WebHookData>() {
		return this->theWebHookDatas;
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

	ApplicationCommandOptionChoiceData::operator JsonObject() {
		JsonObject theData{};
		theData["name"] = this->name;
		theData["name_localizations"] = this->nameLocalizations;
		switch (this->type) {
			case JsonType::Integer: {
				theData["value"] = Uint64{ stoull(this->value) };
				break;
			}
			case JsonType::Float: {
				theData["value"] = Double{ stod(this->value) };
				break;
			}
			case JsonType::Boolean: {
				if (this->value == "false") {
					theData["value"] = Bool{ false };
				} else {
					theData["value"] = Bool{ true };
				}
				break;
			}
			case JsonType::String: {
				theData["value"] = this->value;
				break;
			}
		}
		return theData;
	}

	ApplicationCommandOptionData::operator JsonObject() {
		JsonObject theData{};
		if (this->type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
			theData["channel_types"] = this->channelTypes;
		}
		if (this->type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && this->type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
			theData["required"] = this->required;
		}
		theData["description_localizations"] = this->descriptionLocalizations;
		theData["name_localizations"] = this->nameLocalizations;
		theData["description"] = this->description;
		if (maxValue != 0) {
			theData["min_value"] = this->minValue;
		}
		if (maxValue != 0) {
			theData["max_value"] = this->maxValue;
		}
		theData["required"] = this->required;
		theData["name"] = this->name;
		theData["type"] = this->type;
		if (this->choices.size() > 0) {
			for (auto& value: this->choices) {
				theData["choices"].pushBack(value);
			}
		}
		if (this->choices.size() == 0) {
			theData["autocomplete"] = this->autocomplete;
		}
		if (this->options.size() > 0) {
			for (auto& value: this->options) {
				theData["options"].pushBack(value);
			}
		}
		return theData;
	}

	ThreadMemberDataVector::operator Vector<ThreadMemberData>() {
		return this->theThreadMemberDatas;
	}
	YouTubeFormatVector::operator Vector<YouTubeFormat>() {
		return this->theFormats;
	}

	Void AudioFrameData::clearData() noexcept {
		this->type = AudioFrameType::Unset;
		this->guildMemberId = 0;
		this->sampleCount = -1;
		this->data.clear();
	}

	AllowedMentionsData::operator JsonObject() {
		JsonObject theData{};
		for (auto& value: this->parse) {
			theData["parse"].pushBack(value);
		}
		for (auto& value: this->roles) {
			theData["roles"].pushBack(value);
		}
		for (auto& value: this->users) {
			theData["users"].pushBack(value);
		}
		theData["replied_user"] = this->repliedUser;
		return theData;
	}

	ActionRowData::operator JsonObject() {
		JsonObject theData{};
		theData["type"] = 1;
		if (this->components.size() > 0) {
			for (auto& valueNew: this->components) {
				if (valueNew.type == ComponentType::Button) {
					JsonObject component{};
					component["emoji"]["animated"] = valueNew.emoji.animated;
					component["emoji"]["name"] = String{ valueNew.emoji.name };
					if (valueNew.emoji.id != 0) {
						component["emoji"]["id"] = std::to_string(valueNew.emoji.id);
					}
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["label"] = valueNew.label;
					component["style"] = valueNew.style;
					component["type"] = valueNew.type;
					component["url"] = valueNew.url;
					theData["components"].pushBack(component);
				} else if (valueNew.type == ComponentType::SelectMenu) {
					JsonObject component{};
					for (auto& value01: valueNew.options) {
						JsonObject option{};
						component["emoji"]["animated"] = valueNew.emoji.animated;
						component["emoji"]["name"] = String{ valueNew.emoji.name };
						if (valueNew.emoji.id != 0) {
							component["emoji"]["id"] = std::to_string(valueNew.emoji.id);
						}
						option["description"] = value01.description;
						option["default"] = value01._default;
						option["label"] = value01.label;
						option["value"] = value01.value;
						component["options"].pushBack(option);
					};
					component["placeholder"] = valueNew.placeholder;
					component["max_values"] = valueNew.maxValues;
					component["min_values"] = valueNew.minValues;
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["type"] = valueNew.type;
					theData["components"].pushBack(component);

				} else if (valueNew.type == ComponentType::TextInput) {
					JsonObject component{};
					component["placeholder"] = valueNew.placeholder;
					component["min_length"] = valueNew.minLength;
					component["max_length"] = valueNew.maxLength;
					component["custom_id"] = valueNew.customId;
					component["required"] = valueNew.required;
					component["style"] = valueNew.style;
					component["label"] = valueNew.label;
					component["value"] = valueNew.value;
					component["type"] = valueNew.type;
					theData["components"].pushBack(component);
				}
			}
		}
		return theData;
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

	StickerPackDataVector::operator Vector<StickerPackData>() {
		return this->theStickerPackDatas;
	}

	ConnectionDataVector::operator Vector<ConnectionData>() {
		return this->theConnectionDatas;
	}

	ConnectionDataVector::ConnectionDataVector(simdjson::ondemand::value jsonObjectData) noexcept {
		if (jsonObjectData.type() != simdjson::ondemand::json_type::null) {
			simdjson::ondemand::array theArray{};
			if (jsonObjectData.get(theArray) == simdjson::error_code::SUCCESS) {
				for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
					ConnectionData newData{ value.value() };
					this->theConnectionDatas.emplace_back(std::move(newData));
				}
			}
		}
	}

	VoiceRegionDataVector::operator Vector<VoiceRegionData>() {
		return this->theVoiceRegionDatas;
	}

	IntegrationDataVector::operator Vector<IntegrationData>() {
		return this->theIntegrationDatas;
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

	String InputEventData::getUserName() const {
		return this->interactionData->user.userName;
	}

	String InputEventData::getAvatarUrl() const {
		if (this->interactionData->member.getUserData().getAvatarUrl() != "") {
			return this->interactionData->member.getUserData().getAvatarUrl();
		} else {
			return this->interactionData->user.getAvatarUrl();
		}
	}

	Vector<EmbedData> InputEventData::getEmbeds() const {
		return this->interactionData->message.embeds;
	}

	Vector<ActionRowData> InputEventData::getComponents() const {
		return this->interactionData->message.components;
	}

	Snowflake InputEventData::getAuthorId() const {
		return this->interactionData->user.id;
	}

	Snowflake InputEventData::getInteractionId() const {
		return this->interactionData->id;
	}

	Snowflake InputEventData::getApplicationId() const {
		return this->interactionData->applicationId;
	}

	Snowflake InputEventData::getChannelId() const {
		return this->interactionData->channelId;
	}

	String InputEventData::getInteractionToken() const {
		return this->interactionData->token;
	}

	Snowflake InputEventData::getGuildId() const {
		return this->interactionData->guildId;
	}

	Snowflake InputEventData::getMessageId() const {
		return this->interactionData->message.id;
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
		this->interactionToken = dataPackage.getInteractionToken();
		this->applicationId = dataPackage.getApplicationId();
		this->interactionId = dataPackage.getInteractionId();
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(const InputEventData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::addButton(Bool disabled, const String& customIdNew, const String& buttonLabel, ButtonStyle buttonStyle,
		const String& emojiName, Snowflake emojiId, const String& url) {
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
				component.style = static_cast<Int32>(buttonStyle);
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

	RespondToInputEventData& RespondToInputEventData::addSelectMenu(Bool disabled, const String& customIdNew, Vector<SelectOptionData> options, const String& placeholder,
		Int32 maxValues, Int32 minValues) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = ComponentType::SelectMenu;
				componentData.placeholder = placeholder;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.customId = customIdNew;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addModal(const String& topTitleNew, const String& topCustomIdNew, const String& titleNew, const String& customIdNew,
		Bool required, Int32 minLength, Int32 maxLength, TextInputStyle inputStyle, const String& label, const String& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::TextInput;
				component.customId = customIdNew;
				component.style = static_cast<Int32>(inputStyle);
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

	RespondToInputEventData& RespondToInputEventData::addContent(const String& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTTSStatus(Bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(simdjson::ondemand::object theValue, const String& theName, UMap<String, String> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name = theName;
		StringView theString{};
		if (theValue["value"].get(theString) == simdjson::error_code::SUCCESS) {
			choiceData.value = theString;
			this->choices.emplace_back(choiceData);
			return *this;
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const Snowflake targetUserIdNew) {
		this->targetUserId = targetUserIdNew;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addButton(Bool disabled, const String& customIdNew, const String& buttonLabel, ButtonStyle buttonStyle, const String& emojiName,
		Snowflake emojiId, const String& url) {
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
				component.style = static_cast<Int32>(buttonStyle);
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

	MessageResponseBase& MessageResponseBase::addSelectMenu(Bool disabled, const String& customIdNew, Vector<SelectOptionData> options, const String& placeholder, Int32 maxValues,
		Int32 minValues) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = ComponentType::SelectMenu;
				componentData.placeholder = placeholder;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.customId = customIdNew;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addModal(const String& topTitleNew, const String& topCustomIdNew, const String& titleNew, const String& customIdNew, Bool required,
		Int32 minLength, Int32 maxLength, TextInputStyle inputStyle, const String& label, const String& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::TextInput;
				component.customId = customIdNew;
				component.style = static_cast<Int32>(inputStyle);
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

	MessageResponseBase& MessageResponseBase::addContent(const String& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::setTTSStatus(Bool enabledTTs) {
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

	InteractionResponseData::operator JsonObject() {
		JsonObject theData{};
		theData["type"] = this->type;
		if (this->data.attachments.size() > 0) {
			for (auto& value: this->data.attachments) {
				theData["data"]["attachments"].pushBack(value);
			}
		}
		if (this->data.components.size() == 0) {
			theData["data"]["components"] = ValueType::Null_Ext;
		} else {
			for (auto& value: this->data.components) {
				theData["data"]["components"].pushBack(value);
			}
		}
		if (this->data.allowedMentions.parse.size() > 0 || this->data.allowedMentions.roles.size() > 0 || this->data.allowedMentions.users.size() > 0) {
			theData["data"]["allowed_mentions"] = this->data.allowedMentions;
		}
		if (this->data.choices.size() > 0) {
			for (auto& value: this->data.choices) {
				JsonObject theValue{};
				theValue["name"] = value.name;
				theValue["name_localizations"] = value.nameLocalizations;
				theValue["value"] = value.value;
				theData["data"]["choices"].pushBack(theValue);
			}
		}
		if (this->data.embeds.size() == 0) {
			theData["data"]["embeds"] = ValueType::Null_Ext;
		} else {
			for (auto& value: this->data.embeds) {
				theData["data"]["embeds"].pushBack(JsonObject{ value });
			}
		}
		if (this->data.customId != "") {
			theData["data"]["custom_id"] = this->data.customId;
		}
		if (this->data.content != "") {
			theData["data"]["content"] = this->data.content;
		}
		if (this->data.title != "") {
			theData["data"]["title"] = this->data.title;
		}
		theData["data"]["flags"] = this->data.flags;
		theData["data"]["tts"] = this->data.tts;
		return theData;
	}

	Void parseCommandDataOption(UMap<String, JsonStringValue>& theValues, ApplicationCommandInteractionDataOption& theData) {
		JsonStringValue theValue{};
		theValue.theType = theData.value.theType;
		theValue.theValue = theData.value.theValue;
		theValues.emplace(theData.name, theValue);
		for (auto& value: theData.options) {
			JsonStringValue theValueNew{};
			theValueNew.theType = value.value.theType;
			theValueNew.theValue = value.value.theValue;
			theValues.emplace(value.name, theValueNew);
			parseCommandDataOption(theValues, value);
		}
	}

	CommandData::CommandData(InputEventData inputEventData) {
		if (inputEventData.interactionData->data.applicationCommandData.name != "") {
			this->commandName = inputEventData.interactionData->data.applicationCommandData.name;
		}
		if (inputEventData.interactionData->data.messageInteractionData.targetId != 0) {
			this->optionsArgs.theValues.emplace("target_id",
				JsonStringValue{ .theValue = std::to_string(inputEventData.interactionData->data.messageInteractionData.targetId), .theType = ValueType::String });
		} else if (inputEventData.interactionData->data.userInteractionData.targetId != 0) {
			this->optionsArgs.theValues.emplace("target_id",
				JsonStringValue{ .theValue = std::to_string(inputEventData.interactionData->data.userInteractionData.targetId), .theType = ValueType::String });
		}
		this->eventData = inputEventData;
		for (auto& value: this->eventData.interactionData->data.applicationCommandData.options) {
			parseCommandDataOption(this->optionsArgs.theValues, value);
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

	BaseFunctionArguments::BaseFunctionArguments(CommandData commandDataNew, DiscordCoreClient* discordCoreClientNew) : CommandData(commandDataNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	MoveThroughMessagePagesData moveThroughMessagePages(const String& userID, InputEventData originalEvent, Uint32 currentPageIndex, const Vector<EmbedData>& messageEmbeds,
		Bool deleteAfter, Uint32 waitForMaxMs, Bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		Uint32 newCurrentPageIndex = currentPageIndex;
		UniquePtr<RespondToInputEventData> dataPackage{ std::make_unique<RespondToInputEventData>(originalEvent) };
		if (messageEmbeds.size() > 0) {
			dataPackage->addMessageEmbed(messageEmbeds[currentPageIndex]);
		}
		if (returnResult) {
			dataPackage->addButton(false, "select", "Select", ButtonStyle::Success, "✅");
		}
		dataPackage->addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
		dataPackage->addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
		dataPackage->addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
		dataPackage->setResponseType(InputEventResponseType::Edit_Interaction_Response);
		originalEvent = InputEvents::respondToInputEventAsync(*dataPackage).get();
		StopWatch theStopWatch{ std::chrono::milliseconds{ waitForMaxMs } };
		while (!theStopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
			UniquePtr<ButtonCollector> button{ std::make_unique<ButtonCollector>(originalEvent) };

			Vector<ButtonResponseData> buttonIntData{ button->collectButtonData(false, waitForMaxMs, 1, Snowflake{ stoull(userID) }).get() };

			if (buttonIntData.size() == 0 || buttonIntData.at(0).buttonId == "empty" || buttonIntData.at(0).buttonId == "exit") {
				UniquePtr<RespondToInputEventData> dataPackage02{ std::make_unique<RespondToInputEventData>(originalEvent) };
				if (buttonIntData.at(0).buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					UniquePtr<InteractionData> interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
					*dataPackage02 = RespondToInputEventData{ *interactionData };
				}

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				for (auto& value: originalEvent.getComponents()) {
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
					newCurrentPageIndex++;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex > 0)) {
					newCurrentPageIndex--;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex == 0)) {
					newCurrentPageIndex = static_cast<Uint32>(messageEmbeds.size()) - 1;
				}
				UniquePtr<InteractionData> interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
				*dataPackage = RespondToInputEventData{ *interactionData };
				dataPackage->setResponseType(InputEventResponseType::Edit_Interaction_Response);
				for (auto& value: originalEvent.getComponents()) {
					dataPackage->addComponentRow(value);
				}
				dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				InputEvents::respondToInputEventAsync(*dataPackage).get();
			} else if (buttonIntData.at(0).buttonId == "select") {
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					UniquePtr<InteractionData> interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
					*dataPackage = RespondToInputEventData{ *interactionData };
					dataPackage->setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
					for (auto& value: originalEvent.getComponents()) {
						for (auto& value02: value.components) {
							value02.disabled = true;
						}
						dataPackage->addComponentRow(value);
					}
					InputEvents::respondToInputEventAsync(*dataPackage).get();
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
