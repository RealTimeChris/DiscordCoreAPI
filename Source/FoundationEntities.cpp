/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
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

namespace DiscordCoreInternal {

	UpdatePresenceData::operator nlohmann::json() {
		nlohmann::json data{};
		for (auto& value: this->activities) {
			nlohmann::json dataNew{};
			if (static_cast<std::string>(value.url) != "") {
				dataNew["url"] = value.url;
			}
			dataNew["name"] = value.name;
			dataNew["type"] = value.type;
			data["d"]["activities"].emplace_back(dataNew);
		}
		data["d"]["status"] = this->status;
		data["d"]["since"] = nullptr;
		data["d"]["afk"] = this->afk;
		data["op"] = 3;
		return data;
	}

	WebSocketResumeData::operator nlohmann::json() {
		nlohmann::json theData{};
		theData["d"]["seq"] = this->lastNumberReceived;
		theData["d"]["session_id"] = this->sessionId;
		theData["d"]["token"] = this->botToken;
		theData["op"] = 6;
		return theData;
	}

	WebSocketIdentifyData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["properties"]["browser"] = "DiscordCoreAPI";
		data["d"]["properties"]["device"] = "DiscordCoreAPI";
		data["d"]["shard"] = { this->currentShard, this->numberOfShards };
		data["d"]["large_threshold"] = 250;
		data["d"]["intents"] = this->intents;
		data["d"]["compress"] = false;
		data["d"]["token"] = this->botToken;
		nlohmann::json dataNewReal{};
		for (auto& value: this->presence.activities) {
			nlohmann::json dataNew{};
			if (static_cast<std::string>(value.url) != "") {
				dataNew["url"] = value.url;
			}
			dataNew["name"] = value.name;
			dataNew["type"] = value.type;
			dataNewReal["activities"].emplace_back(dataNew);
		}
		dataNewReal["status"] = this->presence.status;
		dataNewReal["since"] = nullptr;
		dataNewReal["afk"] = this->presence.afk;
		data["d"]["presence"] = dataNewReal;
		data["op"] = 2;
#ifdef _WIN32
		data["d"]["properties"]["os"] = "Windows";
#else
		data["d"]["properties"]["os"] = "Linux";
#endif
		return data;
	}

	VoiceSocketProtocolPayloadData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["data"]["port"] = stol(this->voicePort);
		data["d"]["data"]["mode"] = this->voiceEncryptionMode;
		data["d"]["data"]["address"] = this->externalIp;
		data["d"]["protocol"] = "udp";
		data["op"] = 1;
		return data;
	}

	VoiceIdentifyData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["session_id"] = this->connectionData.sessionId;
		data["d"]["server_id"] = std::to_string(this->connectInitData.guildId);
		data["d"]["user_id"] = std::to_string(this->connectInitData.userId);
		data["d"]["token"] = this->connectionData.token;
		data["op"] = 0;
		return data;
	}

	SendSpeakingData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["speaking"] = this->type;
		data["d"]["delay"] = this->delay;
		data["d"]["ssrc"] = this->ssrc;
		data["op"] = 5;
		return data;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(const HttpsWorkloadData& other) {
		if (this != &other) {
			this->thisWorkerId.store(this->thisWorkerId.load());
			this->headersToInsert = other.headersToInsert;
			this->workloadClass = other.workloadClass;
			this->workloadType = other.workloadType;
			this->relativePath = other.relativePath;
			this->payloadType = other.payloadType;
			this->callStack = other.callStack;
			this->baseUrl = other.baseUrl;
			this->content = other.content;
		}
		return *this;
	}

	HttpsWorkloadData::HttpsWorkloadData(const HttpsWorkloadData& other) {
		*this = other;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadData& other) {
		if (this != &other) {
			this->thisWorkerId.store(this->thisWorkerId.load());
			this->headersToInsert = other.headersToInsert;
			this->workloadClass = other.workloadClass;
			this->workloadType = other.workloadType;
			this->relativePath = other.relativePath;
			this->payloadType = other.payloadType;
			this->callStack = other.callStack;
			this->baseUrl = other.baseUrl;
			this->content = other.content;
		}
		return *this;
	}

	HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadData& other) {
		*this = other;
	}

	HttpsWorkloadData::HttpsWorkloadData(DiscordCoreInternal::HttpsWorkloadType theType) {
		if (!HttpsWorkloadData::workloadIdsExternal.contains(theType)) {
			std::unique_ptr<std::atomic_int64_t> theInt{ std::make_unique<std::atomic_int64_t>() };
			std::unique_ptr<std::atomic_int64_t> theInt02{ std::make_unique<std::atomic_int64_t>() };
			HttpsWorkloadData::workloadIdsExternal[theType] = std::move(theInt);
			HttpsWorkloadData::workloadIdsInternal[theType] = std::move(theInt02);
		}
		this->thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(theType));
		this->workloadType = theType;
	}

	int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) {
		std::unique_lock theLock{ HttpsWorkloadData::accessMutex };
		int64_t theValue{};
		theValue = HttpsWorkloadData::workloadIdsExternal[workloadType]->load();
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(theValue + 1);
		return theValue;
	}

	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};
	std::mutex HttpsWorkloadData::accessMutex{};
}

namespace DiscordCoreAPI {

	std::string DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
		TimeStamp<std::chrono::milliseconds> timeStamp{ (static_cast<uint64_t>(this->id) >> 22) + 1420070400000, timeFormat };
		return timeStamp;
	}

	RoleTagsData& RoleTagsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	RoleTagsData::RoleTagsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	UserData& UserData::operator=(nlohmann::json* jsonObjectData) noexcept {
		this->parseObject(std::move(jsonObjectData));
		return *this;
	}

	UserData::UserData(nlohmann::json* jsonObjectData) noexcept {
		*this = std::move(jsonObjectData);
	}

	UserData& UserData::operator=(UserData&& other) noexcept {
		if (this != &other) {
			this->discriminator = std::move(other.discriminator);
			this->userName = std::move(other.userName);
			this->avatar = std::move(other.avatar);
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	UserData::UserData(UserData&& other) noexcept {
		*this = std::move(other);
	}

	std::string UserData::getAvatarUrl() {
		return this->avatar.getHashUrl(this->id, 0);
	}

	AttachmentData::operator nlohmann::json() {
		nlohmann::json newValue{};
		newValue["content_type"] = this->contentType;
		newValue["description"] = this->description;
		newValue["ephemeral"] = this->ephemeral;
		newValue["file_name"] = this->filename;
		newValue["proxy_url"] = this->proxyUrl;
		newValue["height"] = this->height;
		newValue["width"] = this->width;
		newValue["size"] = this->size;
		newValue["url"] = this->url;
		return newValue;
	}

	AttachmentData& AttachmentData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AttachmentData::AttachmentData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedFooterData& EmbedFooterData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedFooterData::EmbedFooterData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedImageData& EmbedImageData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedImageData::EmbedImageData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedThumbnailData& EmbedThumbnailData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedThumbnailData::EmbedThumbnailData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedVideoData& EmbedVideoData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedVideoData::EmbedVideoData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedProviderData& EmbedProviderData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedProviderData::EmbedProviderData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedAuthorData& EmbedAuthorData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedAuthorData::EmbedAuthorData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedFieldData::operator nlohmann::json() {
		nlohmann::json newValue{};
		newValue["inline"] = this->Inline;
		newValue["value"] = this->value;
		newValue["name"] = this->name;
		return newValue;
	}

	EmbedFieldData& EmbedFieldData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedFieldData::EmbedFieldData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedData::operator nlohmann::json() {
		nlohmann::json fields{};
		for (auto& value2: this->fields) {
			fields.emplace_back(value2);
		}
		int32_t colorValInt = stol(this->hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << colorValInt;
		std::string realColorVal = stream.str();
		nlohmann::json embed{};
		embed["footer"]["proxy_icon_url"] = this->footer.proxyIconUrl;
		embed["author"]["proxy_icon_url"] = this->author.proxyIconUrl;
		embed["thumbnail"]["proxy_url"] = this->thumbnail.proxyUrl;
		embed["thumbnail"]["height"] = this->thumbnail.height;
		embed["thumbnail"]["width"] = this->thumbnail.width;
		embed["image"]["proxy_url"] = this->image.proxyUrl;
		embed["author"]["icon_url"] = this->author.iconUrl;
		embed["footer"]["icon_url"] = this->footer.iconUrl;
		embed["video"]["proxy_url"] = this->video.proxyUrl;
		embed["provider"]["name"] = this->provider.name;
		embed["thumbnail"]["url"] = this->thumbnail.url;
		embed["provider"]["url"] = this->provider.url;
		embed["video"]["height"] = this->video.height;
		embed["image"]["height"] = this->image.height;
		embed["author"]["name"] = this->author.name;
		embed["image"]["width"] = this->image.width;
		embed["footer"]["text"] = this->footer.text;
		embed["video"]["width"] = this->video.width;
		embed["author"]["url"] = this->author.url;
		embed["description"] = this->description;
		embed["image"]["url"] = this->image.url;
		embed["video"]["url"] = this->video.url;
		embed["timestamp"] = this->timestamp;
		embed["title"] = this->title;
		embed["color"] = realColorVal;
		embed["type"] = this->type;
		embed["url"] = this->url;
		embed["fields"] = fields;
		return embed;
	}

	EmbedData& EmbedData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedData::EmbedData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
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
		this->timestamp = timeStamp;
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

	MessageReferenceData::operator nlohmann::json() {
		nlohmann::json newValue{};
		newValue["fail_if_not_exists"] = this->failIfNotExists;
		newValue["message_id"] = this->messageId;
		newValue["channel_id"] = this->channelId;
		newValue["guild_id"] = this->guildId;
		return newValue;
	}

	MessageReferenceData& MessageReferenceData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageReferenceData::MessageReferenceData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadMetadataData& ThreadMetadataData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMetadataData::ThreadMetadataData(nlohmann::json* other) {
		*this = other;
	}

	ThreadMemberData& ThreadMemberData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMemberData::ThreadMemberData(nlohmann::json* other) {
		*this = other;
	}

	ThreadMemberDataVector::operator std::vector<ThreadMemberData>() {
		return this->theThreadMemberDatas;
	}

	ThreadMemberDataVector& ThreadMemberDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMemberDataVector::ThreadMemberDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	bool operator==(const GuildMemberId lhs, const GuildMemberId rhs) {
		if (lhs.guildMemberId == rhs.guildMemberId && lhs.guildId == rhs.guildId) {
			return true;
		} else {
			return false;
		}
	}

	bool operator<(const GuildMemberId& lhs, const GuildMemberId& rhs) {
		if ((lhs.guildId + lhs.guildMemberId) < (rhs.guildId + rhs.guildMemberId)) {
			return true;
		} else {
			return false;
		}
	}

	GuildMemberData& GuildMemberData::operator=(nlohmann::json* jsonObjectData) noexcept {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildMemberData::GuildMemberData(nlohmann::json* jsonObjectData) noexcept {
		*this = jsonObjectData;
	}

	GuildMemberData& GuildMemberData::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			this->currentVoiceChannel = other.currentVoiceChannel;
			this->userAvatar = std::move(other.userAvatar);
			this->userName = std::move(other.userName);
			this->joinedAt = std::move(other.joinedAt);
			this->avatar = std::move(other.avatar);
			this->permissions = other.permissions;
			this->roles = std::move(other.roles);
			this->nick = std::move(other.nick);
			this->guildId = other.guildId;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	GuildMemberData::GuildMemberData(GuildMemberData&& other) noexcept {
		*this = std::move(other);
	}

	VoiceStateData& VoiceStateData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	VoiceStateData::VoiceStateData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	bool operator==(const VoiceStateId& lhs, const VoiceStateId& rhs) {
		if (lhs.guildMemberId == rhs.guildMemberId && lhs.guildId == rhs.guildId) {
			return true;
		} else {
			return false;
		}
	}

	bool operator<(const VoiceStateId& lhs, const VoiceStateId& rhs) {
		if ((lhs.guildId + lhs.guildMemberId) < (rhs.guildId + rhs.guildMemberId)) {
			return true;
		} else {
			return false;
		}
	}


	OverWriteData& OverWriteData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	OverWriteData::OverWriteData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ChannelData& ChannelData::operator=(nlohmann::json* jsonObjectData) noexcept {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ChannelData::ChannelData(nlohmann::json* other) noexcept {
		*this = other;
	}

	ChannelData& ChannelData::operator=(ChannelData&& other) noexcept {
		if (this != &other) {
			this->permissionOverwrites = std::move(other.permissionOverwrites);
			this->parentId = std::move(other.parentId);
			this->guildId = std::move(other.guildId);
			this->ownerId = std::move(other.ownerId);
			this->memberCount = other.memberCount;
			this->topic = std::move(other.topic);
			this->name = std::move(other.name);
			this->position = other.position;
			this->id = std::move(other.id);
			this->flags = other.flags;
			this->type = other.type;
		}
		return *this;
	}

	ChannelData::ChannelData(ChannelData&& other) noexcept {
		*this = std::move(other);
	}

	ActiveThreadsData& ActiveThreadsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActiveThreadsData::ActiveThreadsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ArchivedThreadsData& ArchivedThreadsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ArchivedThreadsData::ArchivedThreadsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	RoleData& RoleData::operator=(nlohmann::json* jsonObjectData) noexcept {
		this->parseObject(jsonObjectData);
		return *this;
	}

	RoleData::RoleData(nlohmann::json* jsonObjectData) noexcept {
		*this = jsonObjectData;
	}

	RoleData& RoleData::operator=(RoleData&& other) noexcept {
		if (this != &other) {
			this->unicodeEmoji = std::move(other.unicodeEmoji);
			this->permissions = other.permissions;
			this->name = std::move(other.name);
			this->position = other.position;
			this->color = other.color;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	RoleData::RoleData(RoleData&& other) noexcept {
		*this = std::move(other);
	}

	ActionMetaData& ActionMetaData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActionMetaData::ActionMetaData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TriggerMetaData& TriggerMetaData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TriggerMetaData::TriggerMetaData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActionData& ActionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActionData::ActionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AutoModerationRuleData& AutoModerationRuleData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AutoModerationRuleData::AutoModerationRuleData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandPermissionData& ApplicationCommandPermissionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandPermissionData::ApplicationCommandPermissionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildApplicationCommandPermissionsData& GuildApplicationCommandPermissionsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildApplicationCommandPermissionsData::GuildApplicationCommandPermissionsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildApplicationCommandPermissionsDataVector& GuildApplicationCommandPermissionsDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildApplicationCommandPermissionsDataVector::GuildApplicationCommandPermissionsDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmojiData& EmojiData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmojiData::EmojiData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmojiDataVector& EmojiDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmojiDataVector::EmojiDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ReactionData& ReactionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ReactionData::ReactionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceRegionData& VoiceRegionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	VoiceRegionData::VoiceRegionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceRegionDataVector& VoiceRegionDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	VoiceRegionDataVector::VoiceRegionDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageActivityData& MessageActivityData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageActivityData::MessageActivityData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	BanData& BanData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	BanData::BanData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	BanDataVector::operator std::vector<BanData>() {
		return this->theBanDatas;
	}

	BanDataVector& BanDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	BanDataVector::BanDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TeamMembersObjectData& TeamMembersObjectData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TeamMembersObjectData::TeamMembersObjectData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	UpdateVoiceStateData::operator nlohmann::json() {
		nlohmann::json data{};
		if (this->channelId == 0) {
			data["d"]["channel_id"] = nullptr;
		} else {
			data["d"]["channel_id"] = std::to_string(this->channelId);
		}
		data["d"]["self_deaf"] = this->selfDeaf;
		data["d"]["self_mute"] = this->selfMute;
		data["d"]["guild_id"] = std::to_string(this->guildId);
		data["op"] = 4;
		return data;
	}

	TeamObjectData& TeamObjectData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TeamObjectData::TeamObjectData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InstallParamsData& InstallParamsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InstallParamsData::InstallParamsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationData& ApplicationData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationData::ApplicationData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AuthorizationInfoData& AuthorizationInfoData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuthorizationInfoData::AuthorizationInfoData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AccountData& AccountData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AccountData::AccountData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildWidgetData& GuildWidgetData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildWidgetData::GuildWidgetData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildWidgetImageData& GuildWidgetImageData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildWidgetImageData::GuildWidgetImageData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	IntegrationData& IntegrationData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	IntegrationData::IntegrationData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	IntegrationDataVector& IntegrationDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	IntegrationDataVector::IntegrationDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	OptionalAuditEntryInfoData& OptionalAuditEntryInfoData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	OptionalAuditEntryInfoData::OptionalAuditEntryInfoData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AuditLogChangeData& AuditLogChangeData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuditLogChangeData::AuditLogChangeData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildPruneCountData& GuildPruneCountData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildPruneCountData::GuildPruneCountData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AuditLogEntryData& AuditLogEntryData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuditLogEntryData::AuditLogEntryData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	PartyData& PartyData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	PartyData::PartyData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AssetsData& AssetsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AssetsData::AssetsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	SecretsData& SecretsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	SecretsData::SecretsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TimestampData& TimestampData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TimestampData::TimestampData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActivityData& ActivityData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActivityData::ActivityData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ClientStatusData& ClientStatusData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ClientStatusData::ClientStatusData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WelcomeScreenChannelData& WelcomeScreenChannelData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WelcomeScreenChannelData::WelcomeScreenChannelData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WelcomeScreenData& WelcomeScreenData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WelcomeScreenData::WelcomeScreenData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	PresenceUpdateData& PresenceUpdateData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	PresenceUpdateData::PresenceUpdateData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StageInstanceData& StageInstanceData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StageInstanceData::StageInstanceData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerData& StickerData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerData::StickerData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildPreviewData& GuildPreviewData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildPreviewData::GuildPreviewData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildData& GuildData::operator=(nlohmann::json* jsonObjectData) noexcept {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildData::GuildData(nlohmann::json* jsonObjectData) noexcept {
		*this = jsonObjectData;
	}

	GuildData& GuildData::operator=(GuildData&& other) noexcept {
		if (this != &other) {
			this->voiceConnectionPtr = other.voiceConnectionPtr;
			this->discordCoreClient = other.discordCoreClient;
			this->channels = std::move(other.channels);
			this->joinedAt = std::move(other.joinedAt);
			this->members = std::move(other.members);
			this->memberCount = other.memberCount;
			this->roles = std::move(other.roles);
			this->name = std::move(other.name);
			this->icon = std::move(other.icon);
			this->ownerId = other.ownerId;
			this->flags = other.flags;
			this->id = other.id;
		}
		return *this;
	}

	GuildData::GuildData(GuildData&& other) noexcept {
		*this = std::move(other);
	}

	GuildDataVector::operator std::vector<GuildData>() {
		return this->theGuildDatas;
	}

	GuildDataVector& GuildDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildDataVector::GuildDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventMetadata& GuildScheduledEventMetadata::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventMetadata::GuildScheduledEventMetadata(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventData& GuildScheduledEventData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventData::GuildScheduledEventData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventUserData& GuildScheduledEventUserData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventUserData::GuildScheduledEventUserData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventUserDataVector::operator std::vector<GuildScheduledEventUserData>() {
		return this->theGuildScheduledEventUserDatas;
	}

	GuildScheduledEventUserDataVector& GuildScheduledEventUserDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventUserDataVector::GuildScheduledEventUserDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InviteData& InviteData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InviteData::InviteData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InviteDataVector::operator std::vector<InviteData>() {
		return this->theInviteDatas;
	}

	InviteDataVector& InviteDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InviteDataVector::InviteDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildTemplateData& GuildTemplateData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildTemplateData::GuildTemplateData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildTemplateDataVector::operator std::vector<GuildTemplateData>() {
		return this->theGuildTemplateDatas;
	}

	GuildTemplateDataVector& GuildTemplateDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildTemplateDataVector::GuildTemplateDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WebHookData& WebHookData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WebHookData::WebHookData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WebHookDataVector::operator std::vector<WebHookData>() {
		return this->theWebHookDatas;
	}

	WebHookDataVector& WebHookDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WebHookDataVector::WebHookDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
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

	AuditLogData& AuditLogData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuditLogData::AuditLogData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ReactionRemoveData& ReactionRemoveData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ReactionRemoveData::ReactionRemoveData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandOptionChoiceData::operator nlohmann::json() {
		nlohmann::json theData{};
		theData["name"] = this->name;
		theData["name_localizations"] = this->nameLocalizations;
		theData["value"] = this->value;
		return theData;
	}

	ApplicationCommandOptionChoiceData& ApplicationCommandOptionChoiceData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandOptionChoiceData::ApplicationCommandOptionChoiceData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandOptionData::operator nlohmann::json() {
		nlohmann::json newOption{};
		if (this->type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
			newOption["channel_types"] = this->channelTypes;
		}
		if (this->type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && this->type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
			newOption["required"] = this->required;
		}
		newOption["description_localizations"] = this->descriptionLocalizations;
		newOption["name_localizations"] = this->nameLocalizations;
		newOption["description"] = this->description;
		if (maxValue != 0) {
			newOption["min_value"] = this->minValue;
		}
		if (maxValue != 0) {
			newOption["max_value"] = this->maxValue;
		}
		newOption["required"] = this->required;
		newOption["name"] = this->name;
		newOption["type"] = this->type;
		if (this->choices.size() > 0) {
			newOption["choices"] = nlohmann::json{};
			for (auto& value: this->choices) {
				nlohmann::json theData = value;
				newOption["choices"].emplace_back(theData);
			}
		}
		if (newOption["choices"].size() == 0) {
			newOption["autocomplete"] = this->autocomplete;
		}
		if (this->options.size() > 0) {
			newOption["options"] = nlohmann::json{};
			for (auto& value: this->options) {
				nlohmann::json theData = value;
				newOption["options"].emplace_back(theData);
			}
		}
		return newOption;
	}

	ApplicationCommandOptionData& ApplicationCommandOptionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandOptionData::ApplicationCommandOptionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TypingStartData& TypingStartData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TypingStartData::TypingStartData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	YouTubeFormatVector::operator std::vector<YouTubeFormat>() {
		return this->theFormats;
	}

	YouTubeFormatVector& YouTubeFormatVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	YouTubeFormatVector::YouTubeFormatVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	UserCommandInteractionData& UserCommandInteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	UserCommandInteractionData::UserCommandInteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageCommandInteractionData& MessageCommandInteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageCommandInteractionData::MessageCommandInteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ComponentInteractionData& ComponentInteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ComponentInteractionData::ComponentInteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void AudioFrameData::clearData() {
		this->type = AudioFrameType::Unset;
		this->guildMemberId = 0;
		this->sampleCount = -1;
		this->data.clear();
	}

	ModalInteractionData& ModalInteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ModalInteractionData::ModalInteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AllowedMentionsData::operator nlohmann::json() {
		nlohmann::json newValue{};
		newValue["replied_user"] = this->repliedUser;
		newValue["parse"] = this->parse;
		newValue["roles"] = this->roles;
		newValue["users"] = this->users;
		return newValue;
	}

	AllowedMentionsData& AllowedMentionsData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AllowedMentionsData::AllowedMentionsData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	SelectOptionData& SelectOptionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	SelectOptionData::SelectOptionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActionRowData& ActionRowData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActionRowData::operator nlohmann::json() {
		nlohmann::json components{};
		for (auto& valueNew: this->components) {
			if (valueNew.type == ComponentType::Button) {
				nlohmann::json component{};
				component["emoji"]["animated"] = valueNew.emoji.animated;
				StringWrapper theString = valueNew.emoji.name;
				component["emoji"]["name"] = static_cast<std::string>(theString);
				if (valueNew.emoji.id != 0) {
					component["emoji"]["id"] = valueNew.emoji.id;
				}
				component["custom_id"] = valueNew.customId;
				component["disabled"] = valueNew.disabled;
				component["label"] = valueNew.label;
				component["style"] = valueNew.style;
				component["type"] = valueNew.type;
				component["url"] = valueNew.url;
				components.emplace_back(component);
			} else if (valueNew.type == ComponentType::SelectMenu) {
				nlohmann::json optionsArray{};
				for (auto& value01: valueNew.options) {
					nlohmann::json option{};
					if (value01.emoji.name != "") {
						option["emoji"]["name"] = static_cast<std::string>(value01.emoji.name);
						option["emoji"]["animated"] = value01.emoji.animated;
					}
					if (value01.emoji.id != 0) {
						option["emoji"]["id"] = value01.emoji.id;
					}
					option["description"] = value01.description;
					option["default"] = value01._default;
					option["label"] = value01.label;
					option["value"] = value01.value;
					optionsArray.emplace_back(option);
				};
				nlohmann::json component{};
				component["placeholder"] = valueNew.placeholder;
				component["max_values"] = valueNew.maxValues;
				component["min_values"] = valueNew.minValues;
				component["custom_id"] = valueNew.customId;
				component["disabled"] = valueNew.disabled;
				component["options"] = optionsArray;
				component["type"] = valueNew.type;
				components.emplace_back(component);

			} else if (valueNew.type == ComponentType::TextInput) {
				nlohmann::json component{};
				component["placeholder"] = valueNew.placeholder;
				component["min_length"] = valueNew.minLength;
				component["max_length"] = valueNew.maxLength;
				component["custom_id"] = valueNew.customId;
				component["required"] = valueNew.required;
				component["style"] = valueNew.style;
				component["label"] = valueNew.label;
				component["value"] = valueNew.value;
				component["type"] = valueNew.type;
				components.emplace_back(component);
			}
		}
		nlohmann::json theData{};
		theData["type"] = 1;
		theData["components"] = components;
		return theData;
	}

	ActionRowData::ActionRowData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandData& ApplicationCommandData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandData::ApplicationCommandData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ChannelMentionData& ChannelMentionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ChannelMentionData::ChannelMentionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ChannelPinsUpdateEventData& ChannelPinsUpdateEventData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ChannelPinsUpdateEventData::ChannelPinsUpdateEventData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadListSyncData& ThreadListSyncData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadListSyncData::ThreadListSyncData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadMembersUpdateData& ThreadMembersUpdateData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMembersUpdateData::ThreadMembersUpdateData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageInteractionData& MessageInteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageInteractionData::MessageInteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerItemData& StickerItemData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerItemData::StickerItemData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageDataOld& MessageDataOld::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageDataOld::MessageDataOld(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
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
			this->timestamp = other.timestamp;
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

	MessageData& MessageData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageData::MessageData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerPackData& StickerPackData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerPackData::StickerPackData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerPackDataVector::operator std::vector<StickerPackData>() {
		return this->theStickerPackDatas;
	}

	StickerPackDataVector& StickerPackDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerPackDataVector::StickerPackDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ConnectionData& ConnectionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ConnectionData::ConnectionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ConnectionDataVector::operator std::vector<ConnectionData>() {
		return this->theConnectionDatas;
	}

	ConnectionDataVector& ConnectionDataVector::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ConnectionDataVector::ConnectionDataVector(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandInteractionDataOption& ApplicationCommandInteractionDataOption::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandInteractionDataOption::ApplicationCommandInteractionDataOption(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandInteractionData& ApplicationCommandInteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandInteractionData::ApplicationCommandInteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InteractionDataData& InteractionDataData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InteractionDataData::InteractionDataData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InteractionData& InteractionData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InteractionData::InteractionData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	SessionStartData& SessionStartData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	SessionStartData::SessionStartData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GatewayBotData& GatewayBotData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GatewayBotData::GatewayBotData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildEmojisUpdateEventData& GuildEmojisUpdateEventData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildEmojisUpdateEventData::GuildEmojisUpdateEventData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildStickersUpdateEventData& GuildStickersUpdateEventData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildStickersUpdateEventData::GuildStickersUpdateEventData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildMembersChunkEventData& GuildMembersChunkEventData::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildMembersChunkEventData::GuildMembersChunkEventData(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InputEventData& InputEventData::operator=(InputEventData&& other) noexcept {
		*this->interactionData = std::move(*other.interactionData);
		this->responseType = other.responseType;
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

	InputEventData& InputEventData::operator=(InputEventData& other) noexcept {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData& other) noexcept {
		*this = other;
	}

	InputEventData& InputEventData::operator=(InteractionData& other) noexcept {
		*this->interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(InteractionData& interactionData) noexcept {
		*this = interactionData;
	}

	std::string InputEventData::getUserName() {
		return this->interactionData->user.userName;
	}

	std::string InputEventData::getAvatarUrl() {
		return this->interactionData->user.avatar.getHashUrl(this->interactionData->user.id, 0);
	}

	std::vector<EmbedData> InputEventData::getEmbeds() {
		return this->interactionData->message.embeds;
	}

	std::vector<ActionRowData> InputEventData::getComponents() {
		return this->interactionData->message.components;
	}

	uint64_t InputEventData::getAuthorId() {
		return this->interactionData->user.id;
	}

	uint64_t InputEventData::getInteractionId() {
		return this->interactionData->id;
	}

	uint64_t InputEventData::getApplicationId() {
		return this->interactionData->applicationId;
	}

	uint64_t InputEventData::getChannelId() {
		return this->interactionData->channelId;
	}

	std::string InputEventData::getInteractionToken() {
		return this->interactionData->token;
	}

	uint64_t InputEventData::getGuildId() {
		return this->interactionData->guildId;
	}

	uint64_t InputEventData::getMessageId() {
		return this->interactionData->message.id;
	}

	InteractionData InputEventData::getInteractionData() {
		return *this->interactionData;
	}

	MessageData InputEventData::getMessageData() {
		return this->interactionData->message;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(InteractionData& dataPackage) {
		this->applicationId = dataPackage.applicationId;
		this->interactionToken = dataPackage.token;
		this->messageId = dataPackage.message.id;
		this->channelId = dataPackage.channelId;
		this->interactionId = dataPackage.id;
		this->eventType = dataPackage.type;
		return *this;
	};

	RespondToInputEventData::RespondToInputEventData(InteractionData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(InputEventData& dataPackage) {
		this->interactionToken = dataPackage.getInteractionToken();
		this->applicationId = dataPackage.getApplicationId();
		this->interactionId = dataPackage.getInteractionId();
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(InputEventData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
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

	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues) {
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

	RespondToInputEventData& RespondToInputEventData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
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

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(nlohmann::json theValue, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.value = theValue;
		choiceData.name = theName;
		this->choices.emplace_back(choiceData);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const Snowflake targetUserIdNew) {
		this->targetUserId = targetUserIdNew;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
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

	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder,
		int32_t maxValues, int32_t minValues) {
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

	MessageResponseBase& MessageResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
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

	InteractionResponseData::operator std::string() {
		nlohmann::json data{};
		for (auto& value: this->data.attachments) {
			data["data"]["attachments"].emplace_back(DiscordCoreAPI::AttachmentData{ value });
		}
		if (this->data.components.size() == 0) {
			data["data"]["components"] = nlohmann::json::array();
		} else {
			for (auto& value: this->data.components) {
				data["data"]["components"].emplace_back(value);
			}
		}
		data["data"]["allowed_mentions"] = DiscordCoreAPI::AllowedMentionsData{ this->data.allowedMentions };
		if (this->data.choices.size() > 0) {
			nlohmann::json::array_t theArray{};
			for (auto& value: this->data.choices) {
				nlohmann::json theValue{};
				theValue["name"] = value.name;
				theValue["name_localizations"] = value.nameLocalizations;
				theValue["value"] = value.value;
				theArray.emplace_back(theValue);
			}
			data["data"]["choices"] = theArray;
		}
		if (this->data.embeds.size() == 0) {
			data["data"]["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: this->data.embeds) {
				data["data"]["embeds"].emplace_back(DiscordCoreAPI::EmbedData{ value });
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
		data["type"] = this->type;
		return data.dump(-1, static_cast<char>(32), false, nlohmann::json::error_handler_t::ignore);
		;
	}

	Song& Song::operator=(nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	Song::Song(nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	CommandData::CommandData(InputEventData inputEventData) {
		if (inputEventData.interactionData->data.applicationCommandData.name != "") {
			this->commandName = inputEventData.interactionData->data.applicationCommandData.name;
		}
		if (inputEventData.interactionData->data.messageInteractionData.targetId != 0) {
			this->optionsArgs.emplace_back(std::to_string(inputEventData.interactionData->data.messageInteractionData.targetId));
		} else if (inputEventData.interactionData->data.userInteractionData.targetId != 0) {
			this->optionsArgs.emplace_back(std::to_string(inputEventData.interactionData->data.userInteractionData.targetId));
		}
		this->eventData = inputEventData;
		auto theData = inputEventData.getInteractionData().rawData;
		this->parseObject(&theData);
	}

	BaseFunctionArguments::BaseFunctionArguments(CommandData commandDataNew, DiscordCoreClient* discordCoreClientNew) : CommandData(commandDataNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	MoveThroughMessagePagesData moveThroughMessagePages(const std::string& userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const std::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		uint32_t newCurrentPageIndex = currentPageIndex;
		std::unique_ptr<RespondToInputEventData> dataPackage{ std::make_unique<RespondToInputEventData>(originalEvent) };
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
		while (true) {
			std::this_thread::sleep_for(1ms);
			std::unique_ptr<ButtonCollector> button{ std::make_unique<ButtonCollector>(originalEvent) };

			std::vector<ButtonResponseData> buttonIntData{ button->collectButtonData(false, waitForMaxMs, 1, stoull(userID)).get() };

			if (buttonIntData.size() == 0 || buttonIntData.at(0).buttonId == "empty" || buttonIntData.at(0).buttonId == "exit") {
				std::unique_ptr<RespondToInputEventData> dataPackage02{ std::make_unique<RespondToInputEventData>(originalEvent) };
				if (buttonIntData.at(0).buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(static_cast<InteractionData>(buttonIntData.at(0)));
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
					newCurrentPageIndex = static_cast<uint8_t>(messageEmbeds.size()) - 1;
				}
				std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(static_cast<InteractionData>(buttonIntData.at(0)));
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
					std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(static_cast<InteractionData>(buttonIntData.at(0)));
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
	};
};
