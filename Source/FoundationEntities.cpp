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
		if (!HttpsWorkloadData::workloadIdsExternal.contains(theType)){
			std::unique_ptr<std::atomic_int64_t> theInt{ std::make_unique<std::atomic_int64_t>() };
			std::unique_ptr<std::atomic_int64_t> theInt02{ std::make_unique<std::atomic_int64_t>() };
			HttpsWorkloadData::workloadIdsExternal.insert_or_assign(theType, std::move(theInt));
			HttpsWorkloadData::workloadIdsInternal.insert_or_assign(theType, std::move(theInt02));
		}
		this->thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(theType));
		this->workloadType = theType;
	}

	int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) {
		std::lock_guard theLock{ HttpsWorkloadData::accessMutex };
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

	RoleTagsData& RoleTagsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	RoleTagsData::RoleTagsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	UserData& UserData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	UserData::UserData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
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

	AttachmentData& AttachmentData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AttachmentData::AttachmentData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedFooterData& EmbedFooterData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedFooterData::EmbedFooterData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedImageData& EmbedImageData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedImageData::EmbedImageData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedThumbnailData& EmbedThumbnailData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedThumbnailData::EmbedThumbnailData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedVideoData& EmbedVideoData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedVideoData::EmbedVideoData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedProviderData& EmbedProviderData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedProviderData::EmbedProviderData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedAuthorData& EmbedAuthorData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedAuthorData::EmbedAuthorData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedFieldData::operator nlohmann::json() {
		nlohmann::json newValue{};
		newValue["inline"] = this->Inline;
		newValue["value"] = this->value;
		newValue["name"] = this->name;
		return newValue;
	}

	EmbedFieldData& EmbedFieldData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedFieldData::EmbedFieldData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedData::operator nlohmann::json() {
		nlohmann::json fields{};
		for (auto& value2: this->fields) {
			fields.push_back(value2);
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

	EmbedData& EmbedData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmbedData::EmbedData(const nlohmann::json& jsonObjectData) {
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
		this->fields.push_back(field);
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

	MessageReferenceData& MessageReferenceData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	MessageReferenceData::MessageReferenceData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadMetadataData& ThreadMetadataData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ThreadMetadataData::ThreadMetadataData(const nlohmann::json& other) {
		*this = other;
	}

	ThreadMemberData& ThreadMemberData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ThreadMemberData::ThreadMemberData(const nlohmann::json& other) {
		*this = other;
	}

	ThreadMemberDataVector::operator std::vector<ThreadMemberData>() {
		return this->theThreadMemberDatas;
	}

	ThreadMemberDataVector& ThreadMemberDataVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ThreadMemberDataVector::ThreadMemberDataVector(const nlohmann::json& jsonObjectData) {
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

	GuildMemberData& GuildMemberData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildMemberData::GuildMemberData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceStateData& VoiceStateData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	VoiceStateData::VoiceStateData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	OverWriteData& OverWriteData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	OverWriteData::OverWriteData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ChannelData& ChannelData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ChannelData::ChannelData(const nlohmann::json& other) {
		*this = other;
	}

	ActiveThreadsData& ActiveThreadsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ActiveThreadsData::ActiveThreadsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ArchivedThreadsData& ArchivedThreadsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ArchivedThreadsData::ArchivedThreadsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	RoleData& RoleData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	RoleData::RoleData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ActionMetaData& ActionMetaData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ActionMetaData::ActionMetaData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	TriggerMetaData& TriggerMetaData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	TriggerMetaData::TriggerMetaData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ActionData& ActionData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ActionData::ActionData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	AutoModerationRuleData& AutoModerationRuleData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AutoModerationRuleData::AutoModerationRuleData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandPermissionData& ApplicationCommandPermissionData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ApplicationCommandPermissionData::ApplicationCommandPermissionData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildApplicationCommandPermissionsData& GuildApplicationCommandPermissionsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildApplicationCommandPermissionsData::GuildApplicationCommandPermissionsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildApplicationCommandPermissionsDataVector& GuildApplicationCommandPermissionsDataVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildApplicationCommandPermissionsDataVector::GuildApplicationCommandPermissionsDataVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmojiData& EmojiData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmojiData::EmojiData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	EmojiDataVector& EmojiDataVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	EmojiDataVector::EmojiDataVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ReactionData& ReactionData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ReactionData::ReactionData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceRegionData& VoiceRegionData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	VoiceRegionData::VoiceRegionData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceRegionDataVector& VoiceRegionDataVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	VoiceRegionDataVector::VoiceRegionDataVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageActivityData& MessageActivityData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	MessageActivityData::MessageActivityData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	BanData& BanData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	BanData::BanData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	BanDataVector::operator std::vector<BanData>() {
		return this->theBanDatas;
	}

	BanDataVector& BanDataVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	BanDataVector::BanDataVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	TeamMembersObjectData& TeamMembersObjectData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	TeamMembersObjectData::TeamMembersObjectData(const nlohmann::json& jsonObjectData) {
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

	TeamObjectData& TeamObjectData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	TeamObjectData::TeamObjectData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	InstallParamsData& InstallParamsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	InstallParamsData::InstallParamsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationData& ApplicationData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ApplicationData::ApplicationData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	AuthorizationInfoData& AuthorizationInfoData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AuthorizationInfoData::AuthorizationInfoData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	AccountData& AccountData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AccountData::AccountData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildWidgetData& GuildWidgetData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildWidgetData::GuildWidgetData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildWidgetImageData& GuildWidgetImageData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildWidgetImageData::GuildWidgetImageData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	IntegrationData& IntegrationData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	IntegrationData::IntegrationData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	IntegrationDataVector& IntegrationDataVector::operator = (const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	IntegrationDataVector::IntegrationDataVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	OptionalAuditEntryInfoData& OptionalAuditEntryInfoData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	OptionalAuditEntryInfoData::OptionalAuditEntryInfoData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	AuditLogChangeData& AuditLogChangeData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AuditLogChangeData::AuditLogChangeData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildPruneCountData& GuildPruneCountData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildPruneCountData::GuildPruneCountData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	AuditLogEntryData& AuditLogEntryData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AuditLogEntryData::AuditLogEntryData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	PartyData& PartyData::operator = (const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	PartyData::PartyData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	AssetsData& AssetsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	AssetsData::AssetsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	SecretsData& SecretsData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	SecretsData::SecretsData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	TimestampData& TimestampData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	TimestampData::TimestampData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ActivityData& ActivityData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ActivityData::ActivityData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	ClientStatusData& ClientStatusData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	ClientStatusData::ClientStatusData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	WelcomeScreenChannelData& WelcomeScreenChannelData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	WelcomeScreenChannelData::WelcomeScreenChannelData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	WelcomeScreenData& WelcomeScreenData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	WelcomeScreenData::WelcomeScreenData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	PresenceUpdateData& PresenceUpdateData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	PresenceUpdateData::PresenceUpdateData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	StageInstanceData& StageInstanceData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	StageInstanceData::StageInstanceData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerData& StickerData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	StickerData::StickerData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildPreviewData& GuildPreviewData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildPreviewData::GuildPreviewData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	
	GuildData& GuildData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildData::GuildData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildDataVector::operator std::vector<GuildData>() {
		return this->theGuildDatas;
	}

	GuildDataVector& GuildDataVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildDataVector::GuildDataVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventMetadata& GuildScheduledEventMetadata::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildScheduledEventMetadata::GuildScheduledEventMetadata(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventData& GuildScheduledEventData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildScheduledEventData::GuildScheduledEventData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventUserData& GuildScheduledEventUserData::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	GuildScheduledEventUserData::GuildScheduledEventUserData(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	/*

	class DiscordCoreAPI_Dll GuildScheduledEventUserDataVector : public DataParser<GuildScheduledEventUserDataVector> {
	  public:
		GuildScheduledEventUserDataVector() = default;

		operator std::vector<GuildScheduledEventUserData>() {
			return this->theGuildScheduledEventUserDatas;
		}

		GuildScheduledEventUserDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildScheduledEventUserDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildScheduledEventUserDataVector() = default;

	  protected:
		std::vector<GuildScheduledEventUserData> theGuildScheduledEventUserDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventUserDataVector* pDataStructure) {
			pDataStructure->theGuildScheduledEventUserDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::GuildScheduledEventUserData newData{ value };
				pDataStructure->theGuildScheduledEventUserDatas.push_back(newData);
			}
			pDataStructure->theGuildScheduledEventUserDatas.shrink_to_fit();
		}
	};

	/// Invite data. \brief Invite data.
	struct DiscordCoreAPI_Dll InviteData : public DataParser<InviteData> {
		GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
		int32_t approximatePresenceCount{ 0 };///< Approximate presence count.
		ApplicationData targetApplication{};///< Application data.
		int32_t approximateMemberCount{ 0 };///< Approximate member count.
		StageInstanceData stageInstance{};///< Stage instance data.
		TimeStamp createdAt{ "" };///< Time it was created at.
		TimeStamp expiresAt{ "" };///< When the invite expires.
		int32_t targetType{ 0 };///< Target type.
		bool temporary{ false };///< Is it temporary?
		UserData targetUser{};///< Target User of the invite.
		ChannelData channel{};///< Channel data of the Channel that the invite is for.
		int32_t maxUses{ 0 };///< Max number of uses.
		int32_t maxAge{ 0 };///< Maximum age of the invite.
		Snowflake guildId{};///< The Guild this invite is for.
		std::string code{};///< Unique invite code.
		UserData inviter{};///< The User who created the invite.
		GuildData guild{};///< Guild data of the Channel that the invite is for.
		int32_t uses{ 0 };///< The current number of uses.

		InviteData() = default;

		InviteData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InviteData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InviteData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InviteData* pDataStructure) {
			if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_string()) {
				pDataStructure->code = jsonObjectData["code"].get<std::string>();
			} else if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_number_integer()) {
				pDataStructure->code = std::to_string(jsonObjectData["code"].get<int32_t>());
			}

			if (jsonObjectData.contains("guild") && !jsonObjectData["guild"].is_null()) {
				pDataStructure->guild = jsonObjectData["guild"];
			}

			if (jsonObjectData.contains("channel") && !jsonObjectData["channel"].is_null()) {
				pDataStructure->channel = jsonObjectData["channel"];
			}

			if (jsonObjectData.contains("inviter") && !jsonObjectData["inviter"].is_null()) {
				pDataStructure->inviter = jsonObjectData["inviter"];
			}

			if (jsonObjectData.contains("target_type") && !jsonObjectData["target_type"].is_null()) {
				pDataStructure->targetType = jsonObjectData["target_type"].get<int32_t>();
			}

			if (jsonObjectData.contains("target_user") && !jsonObjectData["target_user"].is_null()) {
				pDataStructure->targetUser = jsonObjectData["target_user"];
			}

			if (jsonObjectData.contains("target_application") && !jsonObjectData["target_application"].is_null()) {
				pDataStructure->targetApplication = jsonObjectData["target_application"];
			}

			if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
				pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
				pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("expires_at") && !jsonObjectData["expires_at"].is_null()) {
				pDataStructure->expiresAt = jsonObjectData["expires_at"].get<std::string>();
			}

			if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
				pDataStructure->stageInstance = jsonObjectData["stage_instance"];
			}

			if (jsonObjectData.contains("guild_scheduled_event") && !jsonObjectData["guild_scheduled_event"].is_null()) {
				pDataStructure->guildScheduledEvent = jsonObjectData["guild_scheduled_event"];
			}

			if (jsonObjectData.contains("uses") && !jsonObjectData["uses"].is_null()) {
				pDataStructure->uses = jsonObjectData["uses"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_uses") && !jsonObjectData["max_uses"].is_null()) {
				pDataStructure->maxUses = jsonObjectData["max_uses"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_age") && !jsonObjectData["max_age"].is_null()) {
				pDataStructure->maxAge = jsonObjectData["max_age"].get<int32_t>();
			}

			if (jsonObjectData.contains("temporary") && !jsonObjectData["temporary"].is_null()) {
				pDataStructure->temporary = jsonObjectData["temporary"].get<bool>();
			}

			if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
				pDataStructure->createdAt = jsonObjectData["created_at"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll InviteDataVector : public DataParser<InviteDataVector> {
	  public:
		InviteDataVector() = default;

		operator std::vector<InviteData>() {
			return this->theInviteDatas;
		}

		InviteDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InviteDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InviteDataVector() = default;

	  protected:
		std::vector<InviteData> theInviteDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, InviteDataVector* pDataStructure) {
			pDataStructure->theInviteDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::InviteData newData{ value };
				pDataStructure->theInviteDatas.push_back(newData);
			}
			pDataStructure->theInviteDatas.shrink_to_fit();
		}
	};

	/// Represents a Guild Template. \brief Represents a Guild Template.
	struct DiscordCoreAPI_Dll GuildTemplateData : public DataParser<GuildTemplateData> {
		GuildData serializedSourceGuild{};///< The Guild snapshot this template contains.
		std::string sourceGuildId{};///< The ID of the Guild this template is based on.
		std::string description{};///< The description for the template.
		uint32_t usageCount{ 0 };///< Number of times this template has been used.
		std::string creatorId{};///< The ID of the User who created the template.
		std::string createdAt{};///< When this template was created.
		std::string updatedAt{};///< When this template was last synced to the source Guild.
		bool isDirty{ false };///< Whether the template has unsynced changes.
		std::string code{};///< The template code(unique ID).
		std::string name{};///< Template name.
		UserData creator{};///< The User who created the template.

		GuildTemplateData() = default;

		GuildTemplateData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildTemplateData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildTemplateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildTemplateData* pDataStructure) {
			if (jsonObjectData.contains("serialized_source_guild") && !jsonObjectData["serialized_source_guild"].is_null()) {
				pDataStructure->serializedSourceGuild = jsonObjectData["serialized_source_guild"];
			}

			if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
				pDataStructure->creator = jsonObjectData["creator"];
			}

			if (jsonObjectData.contains("source_guild_id") && !jsonObjectData["source_guild_id"].is_null()) {
				pDataStructure->sourceGuildId = jsonObjectData["source_guild_id"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("usage_count") && !jsonObjectData["usage_count"].is_null()) {
				pDataStructure->usageCount = jsonObjectData["usage_count"].get<uint32_t>();
			}

			if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
				pDataStructure->creatorId = jsonObjectData["creator_id"].get<std::string>();
			}

			if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
				pDataStructure->createdAt = jsonObjectData["created_at"].get<std::string>();
			}

			if (jsonObjectData.contains("updated_at") && !jsonObjectData["updated_at"].is_null()) {
				pDataStructure->updatedAt = jsonObjectData["updated_at"].get<std::string>();
			}

			if (jsonObjectData.contains("is_dirty") && !jsonObjectData["is_dirty"].is_null()) {
				pDataStructure->isDirty = jsonObjectData["is_dirty"].get<bool>();
			}

			if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null()) {
				pDataStructure->code = jsonObjectData["code"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll GuildTemplateDataVector : public DataParser<GuildTemplateDataVector> {
	  public:
		GuildTemplateDataVector() = default;

		operator std::vector<GuildTemplateData>() {
			return this->theGuildTemplateDatas;
		}

		GuildTemplateDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildTemplateDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildTemplateDataVector() = default;

	  protected:
		std::vector<GuildTemplateData> theGuildTemplateDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildTemplateDataVector* pDataStructure) {
			pDataStructure->theGuildTemplateDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::GuildTemplateData newData{ value };
				pDataStructure->theGuildTemplateDatas.push_back(newData);
			}
			pDataStructure->theGuildTemplateDatas.shrink_to_fit();
		}
	};

	/// Invite target types. \brief Invite target types.
	enum class InviteTargetTypes : int8_t {
		Stream = 1,///< Stream.
		Embedded_Application = 2///< Embedded application.
	};

	/// WebHook types. \brief WebHook types.
	enum class WebHookType : int8_t {
		Incoming = 1,///< Incoming.
		Channel_Follower = 2,///< Channel follower.
		Application = 3///< Application.
	};

	/// WebHook data. \brief WebHook data.
	class DiscordCoreAPI_Dll WebHookData : public DiscordEntity, public DataParser<WebHookData> {
	  public:
		ChannelData sourceChannel{};///< Channel for which th WebHook was issued.
		Snowflake applicationId{};///< Application id.
		GuildData sourceGuild{};///< Source Guild id.
		WebHookType type{ 0 };///< Type of WebHook.
		std::string avatar{};///< Avatar of the WebHook.
		Snowflake channelId{};///< Channel id for which the WebHook was issued.
		std::string token{};///< Token of the WebHook.
		Snowflake guildId{};///< Guild id for which the WebHook was issued.
		std::string name{};///< Name of the WebHook.
		std::string url{};///< Url of the WebHook.
		UserData user{};///< User which create the WebHook.

		WebHookData() = default;

		WebHookData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		WebHookData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~WebHookData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WebHookData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<WebHookType>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
				pDataStructure->avatar = jsonObjectData["avatar"].get<std::string>();
			}

			if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
				pDataStructure->token = jsonObjectData["token"].get<std::string>();
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
				pDataStructure->sourceGuild = jsonObjectData["source_guild"];
			}

			if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
				pDataStructure->sourceChannel = jsonObjectData["source_channel"];
			}

			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll WebHookDataVector : public DataParser<WebHookDataVector> {
	  public:
		std::vector<WebHookData> theWebHookDatas{};

		WebHookDataVector() = default;

		WebHookDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		WebHookDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~WebHookDataVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WebHookDataVector* pDataStructure) {
			pDataStructure->theWebHookDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::WebHookData newData{ value };
				pDataStructure->theWebHookDatas.push_back(newData);
			}
			pDataStructure->theWebHookDatas.shrink_to_fit();
		}
	};

	/// Audit log data. \brief Audit log data.
	class DiscordCoreAPI_Dll AuditLogData : public DataParser<AuditLogData> {
	  public:
		std::vector<GuildScheduledEventData> guildScheduledEvents{};///< Array of guild scheduled event objects.
		std::vector<AutoModerationRuleData> autoModerationRules{};///< List of auto moderation rules referenced in the audit log.
		std::vector<AuditLogEntryData> auditLogEntries{};///< Array of audit log entry objects.
		std::vector<IntegrationData> integrations{};///< Array of partial integration objects.
		std::vector<WebHookData> webhooks{};///< Array of webhook objects.
		std::vector<ChannelData> threads{};///< Array of thread-specific channel objects.
		std::vector<UserData> users{};///< Array of user objects.
		auto getAuditLogData(const Snowflake& userIdOfChanger, AuditLogEvent auditLogType) {
			for (auto& value: this->auditLogEntries) {
				if (value.id == userIdOfChanger && value.actionType == auditLogType) {
					return value;
				}
			}
			return AuditLogEntryData();
		}
		auto getAuditLogData(AuditLogEvent auditLogType, const Snowflake& userIdOfTarget) {
			for (auto& value: this->auditLogEntries) {
				if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
					return value;
				}
			}
			return AuditLogEntryData();
		}

		AuditLogData() = default;

		AuditLogData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AuditLogData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AuditLogData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogData* pDataStructure) {
			if (jsonObjectData.contains("webhooks") && !jsonObjectData["webhooks"].is_null()) {
				pDataStructure->webhooks.clear();
				for (auto& value: jsonObjectData["webhooks"]) {
					WebHookData newData{ value };
					pDataStructure->webhooks.push_back(newData);
				}
			}

			if (jsonObjectData.contains("guild_scheduled_events") && !jsonObjectData["guild_scheduled_events"].is_null()) {
				pDataStructure->webhooks.clear();
				for (auto& value: jsonObjectData["guild_scheduled_events"]) {
					GuildScheduledEventData newData{ value };
					pDataStructure->guildScheduledEvents.push_back(newData);
				}
			}

			if (jsonObjectData.contains("auto_moderation_rules") && !jsonObjectData["auto_moderation_rules"].is_null()) {
				pDataStructure->users.clear();
				for (auto& value: jsonObjectData["auto_moderation_rules"]) {
					AutoModerationRuleData newData{ value };
					pDataStructure->autoModerationRules.push_back(newData);
				}
			}

			if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
				pDataStructure->users.clear();
				for (auto& value: jsonObjectData["users"]) {
					UserData newData{ value };
					pDataStructure->users.push_back(newData);
				}
			}

			if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData["audit_log_entries"].is_null()) {
				pDataStructure->auditLogEntries.clear();
				for (auto& value: jsonObjectData["audit_log_entries"]) {
					AuditLogEntryData newData{ value };
					pDataStructure->auditLogEntries.push_back(newData);
				}
			}

			if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
				pDataStructure->integrations.clear();
				for (auto& value: jsonObjectData["integrations"]) {
					IntegrationData newData{ value };
					pDataStructure->integrations.push_back(newData);
				}
			}

			if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
				pDataStructure->threads.clear();
				for (auto& value: jsonObjectData["threads"]) {
					ChannelData newData{ value };
					pDataStructure->threads.push_back(newData);
				}
			}
		}
	};

	/// For removing a reaction. \brief For removing a reaction.
	struct DiscordCoreAPI_Dll ReactionRemoveData : public DataParser<ReactionRemoveData> {
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake guildId{};
		Snowflake userId{};
		EmojiData emoji{};

		ReactionRemoveData() = default;

		ReactionRemoveData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ReactionRemoveData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ReactionRemoveData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ReactionRemoveData* pDataStructure) {
			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
				pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
				pDataStructure->emoji = jsonObjectData["emoji"];
			}
		}
	};

	/// For storing Interaction-related values. \brief For storing Interaction-related values.
	struct DiscordCoreAPI_Dll InteractionPackageData {
		std::string interactionToken{};
		Snowflake applicationId{};
		Snowflake interactionId{};
	};

	/// For storing Message-related values. \brief For storing Message-related values.
	struct DiscordCoreAPI_Dll MessagePackageData {
		Snowflake channelId{};
		Snowflake messageId{};
	};

	/// Data structure representing an ApplicationCommand's option choice. \brief Data structure representing an ApplicationCommand's option choice.
	class DiscordCoreAPI_Dll ApplicationCommandOptionChoiceData : public DataParser<ApplicationCommandOptionChoiceData> {
	  public:
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		nlohmann::json value{};///< The value of the option.
		std::string name{};///< The name of the current choice.

		ApplicationCommandOptionChoiceData() = default;

		ApplicationCommandOptionChoiceData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandOptionChoiceData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandOptionChoiceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandOptionChoiceData* pDataStructure) {
			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_string()) {
				if (jsonObjectData["value"].is_string()) {
					pDataStructure->value = jsonObjectData["value"].get<std::string>();
				} else if (jsonObjectData["value"].is_number_float()) {
					pDataStructure->value = jsonObjectData["value"].get<float>();
				} else if (jsonObjectData["value"].is_number()) {
					pDataStructure->value = jsonObjectData["value"].get<int32_t>();
				}
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->nameLocalizations[key] = newValue;
				}
			}
		}
	};

	/// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
	struct DiscordCoreAPI_Dll ApplicationCommandOptionData : public DataParser<ApplicationCommandOptionData> {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary for the description localizations field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary for the name localizations field.
		std::vector<ApplicationCommandOptionChoiceData> choices{};///< A vector of possible choices for the current ApplicationCommand option.
		std::vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand option.
		std::vector<ChannelType> channelTypes{};///< Set when the ApplicationCommand option type is set to Channel.
		ApplicationCommandOptionType type{};///< The type of command option.
		bool autocomplete{ false };///< If autocomplete interactions are enabled for this STRING, INTEGER, or NUMBER type option.
		std::string description{};///< A description of the current ApplicationCommand option.
		bool required{ false };///< If the parameter is required or optional-- default false.
		int32_t minValue{ 0 };///< If the option is an INTEGER or NUMBER type, the minimum value permitted.
		int32_t maxValue{ 0 };///< If the option is an INTEGER or NUMBER type, the maximum value permitted.
		std::string name{};///< Name of the current ApplicationCommand option.

		ApplicationCommandOptionData() = default;

		operator nlohmann::json() {
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
			newOption["min_value"] = this->minValue;
			newOption["max_value"] = this->maxValue;
			newOption["required"] = this->required;
			newOption["name"] = this->name;
			newOption["type"] = this->type;
			newOption["options"] = nlohmann::json{};
			newOption["choices"] = nlohmann::json{};
			if (this->choices.size() > 0) {
				for (int32_t x = 0; x < this->choices.size(); x++) {
					nlohmann::json jsonValue{};
					jsonValue["name_localizations"] = this->choices[x].nameLocalizations;
					jsonValue["value"] = this->choices[x].value;
					jsonValue["name"] = this->choices[x].name;
					newOption["choices"].push_back(jsonValue);
				}
			}
			if (newOption["choices"].size() == 0) {
				newOption["autocomplete"] = this->autocomplete;
			}
			if (this->options.size() > 0) {
				for (auto& value: options) {
					nlohmann::json theData = value;
				}
			}
			return newOption;
		}

		ApplicationCommandOptionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandOptionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandOptionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandOptionData* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->nameLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->descriptionLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("channel_types") && !jsonObjectData["channel_types"].is_null()) {
				pDataStructure->channelTypes.clear();
				for (auto& value: jsonObjectData["channel_types"]) {
					pDataStructure->channelTypes.push_back(value);
				}
			}

			if (jsonObjectData.contains("min_value") && !jsonObjectData["min_value"].is_null()) {
				pDataStructure->minValue = jsonObjectData["min_value"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_value") && !jsonObjectData["max_value"].is_null()) {
				pDataStructure->maxValue = jsonObjectData["max_value"].get<int32_t>();
			}

			if (jsonObjectData.contains("required") && !jsonObjectData["required"].is_null()) {
				pDataStructure->required = jsonObjectData["required"].get<bool>();
			}

			if (jsonObjectData.contains("autocomplete") && !jsonObjectData["autocomplete"].is_null()) {
				pDataStructure->autocomplete = jsonObjectData["autocomplete"].get<bool>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
			}

			if (pDataStructure->type == ApplicationCommandOptionType::Sub_Command_Group || pDataStructure->type == ApplicationCommandOptionType::Sub_Command) {
				if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
					pDataStructure->options.clear();
					for (auto& value: jsonObjectData["options"]) {
						ApplicationCommandOptionData newData{ value };
						pDataStructure->options.push_back(newData);
					}
				}
			} else {
				if (jsonObjectData.contains("choices") && !jsonObjectData["choices"].is_null()) {
					pDataStructure->choices.clear();
					for (auto& value: jsonObjectData["choices"]) {
						ApplicationCommandOptionChoiceData newData{ value };
						pDataStructure->choices.push_back(newData);
					}
				}
			}
		}
	};

	/// Representing "TypingStart" data. \brief Representing "TypingStart" data.
	struct DiscordCoreAPI_Dll TypingStartData : public DataParser<TypingStartData> {
		GuildMemberData member{};
		int32_t timestamp{ 0 };
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};

		TypingStartData() = default;

		TypingStartData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		TypingStartData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~TypingStartData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TypingStartData* pDataStructure) {
			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<int32_t>();
			}
		}
	};

	struct DiscordCoreAPI_Dll YouTubeFormat {
		std::string signatureCipher{};
		std::string audioSampleRate{};
		int32_t averageBitrate{ 0 };
		int64_t contentLength{ 0 };
		std::string audioQuality{};
		std::string downloadUrl{};
		std::string signature{};
		std::string mimeType{};
		std::string quality{};
		std::string codecs{};
		std::string aitags{};
		int32_t bitrate{ 0 };
		int32_t height{ 0 };
		int32_t width{ 0 };
		int32_t itag{ 0 };
		int32_t fps{ 0 };
	};

	/// YouTube format data. \brief YouTube format data.
	struct DiscordCoreAPI_Dll YouTubeFormatVector : public DataParser<YouTubeFormatVector> {
		std::vector<YouTubeFormat> theFormats{};

		YouTubeFormatVector() = default;

		YouTubeFormatVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		YouTubeFormatVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~YouTubeFormatVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, YouTubeFormatVector* pDataStructure) {
			if (!jsonObjectData.is_null()) {
				if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("formats") &&
					!jsonObjectData["streamingData"]["formats"].is_null()) {
					for (auto& value: jsonObjectData["streamingData"]["formats"]) {
						YouTubeFormat newData{};

						if (value.contains("audioQuality") && !value["audioQuality"].is_null()) {
							newData.audioQuality = value["audioQuality"].get<std::string>();
						}

						if (value.contains("averageBitrate") && !value["averageBitrate"].is_null()) {
							newData.averageBitrate = value["averageBitrate"].get<int32_t>();
						}

						if (value.contains("audioSampleRate") && !value["audioSampleRate"].is_null()) {
							newData.audioSampleRate = value["audioSampleRate"].get<std::string>();
						}

						if (value.contains("bitrate") && !value["bitrate"].is_null()) {
							newData.bitrate = value["bitrate"].get<int32_t>();
						}

						if (value.contains("contentLength") && !value["contentLength"].is_null()) {
							newData.contentLength = stoll(value["contentLength"].get<std::string>());
						}

						if (value.contains("fps") && !value["fps"].is_null()) {
							newData.fps = value["fps"].get<int32_t>();
						}

						if (value.contains("height") && !value["height"].is_null()) {
							newData.height = value["height"].get<int32_t>();
						}

						if (value.contains("width") && !value["width"].is_null()) {
							newData.height = value["width"].get<int32_t>();
						}

						if (value.contains("aitags") && !value["aitags"].is_null()) {
							newData.aitags = value["aitags"].get<std::string>();
						}

						if (value.contains("itag") && !value["itag"].is_null()) {
							newData.itag = value["itag"].get<int32_t>();
						}

						if (value.contains("url") && !value["url"].is_null()) {
							newData.downloadUrl = value["url"].get<std::string>();
						}

						if (value.contains("mimeType") && !value["mimeType"].is_null()) {
							newData.mimeType = value["mimeType"].get<std::string>();
						}

						if (value.contains("quality") && !value["quality"].is_null()) {
							newData.quality = value["quality"].get<std::string>();
						}

						if (value.contains("signatureCipher") && !value["signatureCipher"].is_null()) {
							newData.signatureCipher = value["signatureCipher"].get<std::string>();
						} else if (value.contains("cipher") && !value["cipher"].is_null()) {
							newData.signatureCipher = value["cipher"].get<std::string>();
						}

						if (newData.signatureCipher.find("&sp") != std::string::npos) {
							newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
						}

						if (newData.signatureCipher.find("url") != std::string::npos) {
							newData.downloadUrl = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
						} else if (value.contains("url") && !value["url"].is_null()) {
							newData.downloadUrl = value["url"];
						}

						pDataStructure->theFormats.push_back(newData);
					}
				}

				if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("adaptiveFormats") &&
					!jsonObjectData["streamingData"]["adaptiveFormats"].is_null()) {
					for (auto& value: jsonObjectData["streamingData"]["adaptiveFormats"]) {
						YouTubeFormat newData{};

						if (value.contains("audioQuality") && !value["audioQuality"].is_null()) {
							newData.audioQuality = value["audioQuality"].get<std::string>();
						}

						if (value.contains("averageBitrate") && !value["averageBitrate"].is_null()) {
							newData.averageBitrate = value["averageBitrate"].get<int32_t>();
						}

						if (value.contains("audioSampleRate") && !value["audioSampleRate"].is_null()) {
							newData.audioSampleRate = value["audioSampleRate"].get<std::string>();
						}

						if (value.contains("bitrate") && !value["bitrate"].is_null()) {
							newData.bitrate = value["bitrate"].get<int32_t>();
						}

						if (value.contains("contentLength") && !value["contentLength"].is_null()) {
							newData.contentLength = stoll(value["contentLength"].get<std::string>());
						}

						if (value.contains("fps") && !value["fps"].is_null()) {
							newData.fps = value["fps"].get<int32_t>();
						}

						if (value.contains("height") && !value["height"].is_null()) {
							newData.height = value["height"].get<int32_t>();
						}

						if (value.contains("width") && !value["width"].is_null()) {
							newData.height = value["width"].get<int32_t>();
						}

						if (value.contains("aitags") && !value["aitags"].is_null()) {
							newData.aitags = value["aitags"].get<std::string>();
						}

						if (value.contains("itag") && !value["itag"].is_null()) {
							newData.itag = value["itag"].get<int32_t>();
						}

						if (value.contains("mimeType") && !value["mimeType"].is_null()) {
							newData.mimeType = value["mimeType"].get<std::string>();
						}

						if (value.contains("quality") && !value["quality"].is_null()) {
							newData.quality = value["quality"].get<std::string>();
						}

						if (value.contains("signatureCipher") && !value["signatureCipher"].is_null()) {
							newData.signatureCipher = value["signatureCipher"].get<std::string>();
						} else if (value.contains("cipher") && !value["cipher"].is_null()) {
							newData.signatureCipher = value["cipher"].get<std::string>();
						}

						if (newData.signatureCipher.find("&sp") != std::string::npos) {
							newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
						}

						if (newData.signatureCipher.find("url") != std::string::npos) {
							newData.downloadUrl = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
						} else if (value.contains("url") && !value["url"].is_null()) {
							newData.downloadUrl = value["url"];
						}
						pDataStructure->theFormats.push_back(newData);
					}
				}
			}
		}
	};

	/// Application command types. \brief Application command types.
	enum class ApplicationCommandType : int8_t {
		Chat_Input = 1,///< Chat input.
		User = 2,///< User.
		Message = 3///< Message.
	};

	/// User command Interaction data. \brief User command Interaction data.
	struct DiscordCoreAPI_Dll UserCommandInteractionData : public DataParser<UserCommandInteractionData> {
		std::string targetId{};///< The target User's id.

		UserCommandInteractionData() = default;

		UserCommandInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		UserCommandInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~UserCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, UserCommandInteractionData* pDataStructure) {
			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->targetId = jsonObjectData["target_id"].get<std::string>();
			}
		}
	};

	/// Message command interacction data. \brief Message command interacction data.
	struct DiscordCoreAPI_Dll MessageCommandInteractionData : public DataParser<MessageCommandInteractionData> {
		std::string targetId{};///< The target Message's id.

		MessageCommandInteractionData() = default;

		MessageCommandInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageCommandInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageCommandInteractionData* pDataStructure) {
			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->targetId = jsonObjectData["target_id"].get<std::string>();
			}
		}
	};

	/// Component types. \brief Component types.
	enum class ComponentType : int8_t {
		ActionRow = 1,///< A container for other components.
		Button = 2,///< A button object.
		SelectMenu = 3,///< A select menu for picking from choices.
		TextInput = 4///< A text input object
	};

	/// Component Interaction data. \brief Component Interaction data.
	struct DiscordCoreAPI_Dll ComponentInteractionData : public DataParser<ComponentInteractionData> {
		std::vector<std::string> values{};///< The values of the components.
		ComponentType componentType{};///< The type of component.
		std::string customId{};///< The custom id of the Interaction entity.

		ComponentInteractionData() = default;

		ComponentInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ComponentInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ComponentInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ComponentInteractionData* pDataStructure) {
			if (jsonObjectData.contains("values") && !jsonObjectData["values"].is_null()) {
				pDataStructure->values.clear();
				for (auto& value: jsonObjectData["values"]) {
					pDataStructure->values.push_back(value);
				}
			}

			if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
				pDataStructure->customId = jsonObjectData["custom_id"].get<std::string>();
			}

			if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
				pDataStructure->componentType = jsonObjectData["component_type"].get<ComponentType>();
			}
		}
	};

	/// Modal interaction data, for inputs from text modals. \brief Modal interaction data, for inputs from text modals.
	struct DiscordCoreAPI_Dll ModalInteractionData : public DataParser<ModalInteractionData> {
		std::string customIdSmall{};///< The custom id of a particular modal input.
		std::string customId{};///< The custom id of the Interaction entity.
		std::string value{};///< The input value of the modal.

		ModalInteractionData() = default;

		ModalInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ModalInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ModalInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ModalInteractionData* pDataStructure) {
			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->value = jsonObjectData["components"][0]["components"][0]["value"].get<std::string>();
			}

			if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
				pDataStructure->customId = jsonObjectData["custom_id"].get<std::string>();
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->customIdSmall = jsonObjectData["components"][0]["components"][0]["custom_id"].get<std::string>();
			}
		}
	};

	/// Allowable mentions for a Message. \brief Allowable mentions for a Message.
	class DiscordCoreAPI_Dll AllowedMentionsData : public DataParser<AllowedMentionsData> {
	  public:
		std::vector<std::string> parse{};///< A vector of allowed mention types to parse from the content.
		std::vector<std::string> roles{};///< Array of role_ids to mention (Max size of 100)
		std::vector<std::string> users{};///< Array of user_ids to mention (Max size of 100)
		bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).

		AllowedMentionsData() = default;

		operator nlohmann::json() {
			nlohmann::json newValue{};
			newValue["replied_user"] = this->repliedUser;
			newValue["parse"] = this->parse;
			newValue["roles"] = this->roles;
			newValue["users"] = this->users;
			return newValue;
		}

		AllowedMentionsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AllowedMentionsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AllowedMentionsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AllowedMentionsData* pDataStructure) {
			if (jsonObjectData.contains("parse") && !jsonObjectData["parse"].is_null()) {
				pDataStructure->parse.clear();
				for (auto& value: jsonObjectData["parse"]) {
					pDataStructure->parse.push_back(value.get<std::string>());
				}
			}

			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				pDataStructure->roles.clear();
				for (auto& value: jsonObjectData["roles"]) {
					pDataStructure->roles.push_back(value.get<std::string>());
				}
			}

			if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
				pDataStructure->users.clear();
				for (auto& value: jsonObjectData["users"]) {
					pDataStructure->users.push_back(value.get<std::string>());
				}
			}

			if (jsonObjectData.contains("replied_user") && !jsonObjectData["replied_user"].is_null()) {
				pDataStructure->repliedUser = jsonObjectData["replied_user"].get<bool>();
			}
		}
	};

	/// Interaction types. \brief Interaction types.
	enum class InteractionType : int8_t {
		Ping = 1,///< Ping.
		Application_Command = 2,///< Application command.
		Message_Component = 3,///< Message component.
		Application_Command_Autocomplete = 4,///< Application command autocomplete.
		Modal_Submit = 5///< Modal submission.
	};

	/// Represents a single selection from a select-menu. \brief Represents a single selection from a select-menu.
	class DiscordCoreAPI_Dll SelectOptionData : public DataParser<SelectOptionData> {
	  public:
		std::string description{};///< Description of the select-menu-option.
		bool _default{ false };///< Is it the default option?
		std::string label{};///< A visible label for the select-menu-option.
		std::string value{};///< A value for identifying the option.
		EmojiData emoji{};///< An optional emoji to put on it.

		SelectOptionData() = default;

		SelectOptionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		SelectOptionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~SelectOptionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SelectOptionData* pDataStructure) {
			if (jsonObjectData.contains("label") && !jsonObjectData["label"].is_null()) {
				pDataStructure->label = jsonObjectData["label"].get<std::string>();
			}

			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
				pDataStructure->value = jsonObjectData["value"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
				pDataStructure->emoji = jsonObjectData["emoji"];
			}

			if (jsonObjectData.contains("default") && !jsonObjectData["default"].is_null()) {
				pDataStructure->_default = jsonObjectData["default"].get<bool>();
			}
		}
	};

	/// Button styles. \brief Button styles.
	enum class ButtonStyle : int8_t {
		Primary = 1,///< Primary.
		Success = 3,///< Success.
		Secondary = 2,///< Secondary.
		Danger = 4,///< Danger.
		Link = 5///< Link.
	};

	/// Represents a single Message-component. \brief Represents a single Message-component.
	struct DiscordCoreAPI_Dll ComponentData {
		std::vector<SelectOptionData> options{};///< Aray of select options the choices in the select, max 25.
		std::string placeholder{};///< Custom placeholder text if nothing is selected, max 100 characters.
		std::string customId{};///< A developer-defined identifier for the component, max 100 characters.
		int32_t minValues{ 0 };///< The minimum number of items that must be chosen; default 1, min 0, max 25.
		int32_t maxValues{ 0 };///< The maximum number of items that can be chosen; default 1, max 25.
		bool disabled{ false };///< Whether the component is disabled, default false.
		int32_t minLength{ 0 };///< The minimum input length for a text input.
		int32_t maxLength{ 0 };///< The maximum input length for a text input.
		bool required{ false };///< Whether this component is required to be filled.
		ComponentType type{};///< Integer component type.
		std::string label{};///< The label for this component.
		std::string value{};///< A pre-filled value for this component.
		std::string title{};///< Url, for url types.
		int32_t style{};///< One of ButtonStyle, or TextInputStyle.
		std::string url{};///< Url, for url types.
		EmojiData emoji{};///< Emoji name, id, and animated.
	};

	/// Action row data of Message components. \brief Action row data of Message components.
	class DiscordCoreAPI_Dll ActionRowData : public DataParser<ActionRowData> {
	  public:
		std::vector<ComponentData> components{};///< Array of components to make up the action-row.

		ActionRowData() = default;

		ActionRowData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		operator nlohmann::json() {
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
					components.push_back(component);
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
						optionsArray.push_back(option);
					};
					nlohmann::json component{};
					component["placeholder"] = valueNew.placeholder;
					component["max_values"] = valueNew.maxValues;
					component["min_values"] = valueNew.minValues;
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["options"] = optionsArray;
					component["type"] = valueNew.type;
					components.push_back(component);

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
					components.push_back(component);
				}
			}
			nlohmann::json theData{};
			theData["type"] = 1;
			theData["components"] = components;
			return theData;
		}

		ActionRowData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ActionRowData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionRowData* pDataStructure) {
			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->components.clear();
				for (auto& value: jsonObjectData["components"]) {
					ComponentData newData{};
					if (value.contains("type") && !value["type"].is_null()) {
						newData.type = value["type"].get<ComponentType>();
					}

					if (value.contains("custom_id") && !value["custom_id"].is_null()) {
						newData.customId = value["custom_id"].get<std::string>();
					}

					if (value.contains("placeholder") && !value["placeholder"].is_null()) {
						newData.placeholder = value["placeholder"].get<std::string>();
					}

					if (value.contains("disabled") && !value["disabled"].is_null()) {
						newData.disabled = value["disabled"].get<bool>();
					}

					if (value.contains("style") && !value["style"].is_null()) {
						newData.style = value["style"].get<int32_t>();
					}

					if (value.contains("label") && !value["label"].is_null()) {
						newData.label = value["label"].get<std::string>();
					}

					if (value.contains("min_length") && !value["min_length"].is_null()) {
						newData.minLength = value["min_length"].get<int32_t>();
					}

					if (value.contains("max_length") && !value["max_length"].is_null()) {
						newData.maxLength = value["max_length"].get<int32_t>();
					}

					if (value.contains("value") && !value["value"].is_null()) {
						newData.value = value["value"].get<std::string>();
					}

					if (value.contains("required") && !value["required"].is_null()) {
						newData.required = value["required"].get<bool>();
					}

					if (value.contains("emoji") && !value["emoji"].is_null()) {
						newData.emoji = value["emoji"];
					}

					if (value.contains("url") && !value["url"].is_null()) {
						newData.url = value["url"].get<std::string>();
					}

					if (value.contains("options") && !value["options"].is_null()) {
						for (auto& value02: value["options"]) {
							SelectOptionData newerData{ value };
							newData.options.push_back(newerData);
						}
					}

					if (value.contains("max_values") && !value["max_values"].is_null()) {
						newData.maxValues = value["max_values"].get<int32_t>();
					}

					if (value.contains("min_values") && !value["min_values"].is_null()) {
						newData.minValues = value["min_values"].get<int32_t>();
					}

					pDataStructure->components.push_back(newData);
				}
			}
		}
	};

	/// Interaction callback types. \brief Interaction callback types.
	enum class InteractionCallbackType : int8_t {
		Pong = 1,///< ACK a Ping.
		Channel_Message_With_Source = 4,///< Respond to an interaction with a message.
		Deferred_Channel_Message_With_Source = 5,///< ACK an interaction and edit a response later, the user sees a loading state.
		Deferred_Update_Message = 6,///< For components, ACK an interaction and edit the original message later; the user does not see a loading state.
		Update_Message = 7,///< For components, edit the message the component was attached to.
		Application_Command_Autocomplete_Result = 8,///< Respond to an autocomplete interaction with suggested choices.
		Modal = 9///< Respond to an interaction with a popup modal.
	};

	/// Interaction ApplicationCommand callback data. \brief Interaction ApplicationCommand callback data.
	struct DiscordCoreAPI_Dll InteractionCallbackData {
		std::vector<ApplicationCommandOptionChoiceData> choices{};///< Autocomplete choices(max of 25 choices).
		std::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		std::vector<ActionRowData> components{};///< Message components.
		AllowedMentionsData allowedMentions{};///< Allowed mentions data.
		std::vector<EmbedData> embeds{};///< Message embeds.
		std::vector<File> files{};///< Files for uploading.
		std::string customId{};///< A developer-defined identifier for the component, max 100 characters.
		std::string content{};///< Message content.
		std::string title{};///< The title of the popup modal.
		int32_t flags{ 0 };///< Flags.
		bool tts{ false };///< Is it TTS?
	};

	/// Data structure representing an ApplicationCommand. \brief Data structure representing an ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommandData : public DiscordEntity, public DataParser<ApplicationCommandData> {
	  public:
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for name field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for name field.
		std::vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set all
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app, only for globally - scoped commands.
		std::string description{};///< A description of the current ApplicationCommand.
		Snowflake applicationId{};///< The current application id.
		std::string version{};///< An autoincremented version.
		std::string name{};///< Name of the current ApplicationCommand.
		Snowflake guildId{};///< (Where applicable) a Guild id for which guild to assign this ApplicationCommand to.

		ApplicationCommandData() = default;

		ApplicationCommandData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("default_member_permissions") && !jsonObjectData["default_member_permissions"].is_null()) {
				pDataStructure->defaultMemberPermissions = jsonObjectData["default_member_permissions"].get<std::string>();
			}

			if (jsonObjectData.contains("dm_permission") && !jsonObjectData["dm_permission"].is_null()) {
				pDataStructure->dmPermission = jsonObjectData["dm_permission"].get<bool>();
			}

			if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
				pDataStructure->version = jsonObjectData["version"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandType>();
			}

			if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->nameLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->descriptionLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
				pDataStructure->version = jsonObjectData["version"].get<std::string>();
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options.clear();
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandOptionData newData{ value };
					pDataStructure->options.push_back(newData);
				}
			}
		}
	};

	/// Channel mention data. \brief Channel mention data.
	class DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity, public DataParser<ChannelMentionData> {
	  public:
		Snowflake guildId{};///< The id of the Guild where it took place.
		std::string name{};///< The name of the Channel that was mentioned.
		ChannelType type{};///< The type of Channel that was mentioned.

		ChannelMentionData() = default;

		ChannelMentionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ChannelMentionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ChannelMentionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelMentionData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ChannelType>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	/// Data for when some Channel pins are updated. \brief Data for when some Channel pins are updated.
	struct DiscordCoreAPI_Dll ChannelPinsUpdateEventData : public DataParser<ChannelPinsUpdateEventData> {
		TimeStamp lastPinTimestamp{ "" };///< The time of the last pinned Message.
		Snowflake channelId{};///< The id of the Channel within which the Message was pinned.
		Snowflake guildId{};///< The id of the Guild within which the Message was pinned.

		ChannelPinsUpdateEventData() = default;

		ChannelPinsUpdateEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ChannelPinsUpdateEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ChannelPinsUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelPinsUpdateEventData* pDataStructre) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructre->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructre->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
				pDataStructre->lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
			}
		}
	};

	/// Data for when threads are synced. \brief Data for when threads are synced.
	struct DiscordCoreAPI_Dll ThreadListSyncData : public DataParser<ThreadListSyncData> {
		std::vector<ThreadMemberData> members{};///< Array of members that are a part of the Thread.
		std::vector<std::string> channelIds{};///< The parent Channel ids whose threads are being synced. If omitted, then threads were synced for entire Guild.
		std::vector<ChannelData> threads{};///< All active threads in the given channels that the current User can access.
		Snowflake guildId{};///< The id of the Guild for which the threads are being synced.

		ThreadListSyncData() = default;

		ThreadListSyncData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadListSyncData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ThreadListSyncData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadListSyncData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_ids") && !jsonObjectData["channel_ids"].is_null()) {
				pDataStructure->channelIds.clear();
				for (auto& value: jsonObjectData["channel_ids"]) {
					pDataStructure->channelIds.push_back(value.get<std::string>());
				}
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				for (auto& value: jsonObjectData["members"]) {
					ThreadMemberData newData{ value };
					pDataStructure->members.push_back(newData);
				}
			}

			if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
				pDataStructure->threads.clear();
				for (auto& value: jsonObjectData["threads"]) {
					ChannelData newData{ value };
					pDataStructure->threads.push_back(newData);
				}
			}
		}
	};

	/// Represents a Thread-members-update. \brief Represents a Thread-members-update.
	class DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity, public DataParser<ThreadMembersUpdateData> {
	  public:
		std::vector<ThreadMemberData> addedMembers{};///< New members added to the Thread.
		std::vector<std::string> removedMemberIds{};///< Members who have been removed.
		int32_t memberCount{ 0 };///< Number of Guild-members in the Thread.
		Snowflake guildId{};///< Guild id of the Thread.

		ThreadMembersUpdateData() = default;

		ThreadMembersUpdateData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadMembersUpdateData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ThreadMembersUpdateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMembersUpdateData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
				pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("added_members") && !jsonObjectData["added_members"].is_null()) {
				pDataStructure->addedMembers.clear();
				for (auto& value: jsonObjectData["added_members"]) {
					ThreadMemberData newData{ value };
					pDataStructure->addedMembers.push_back(newData);
				}
			}

			if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData["removed_member_ids"].is_null()) {
				pDataStructure->removedMemberIds.clear();
				for (auto& value: jsonObjectData["removed_member_ids"]) {
					pDataStructure->removedMemberIds.push_back(value);
				}
			}
		}
	};

	/// Message Interaction data. \brief Message Interaction data.
	class DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity, public DataParser<MessageInteractionData> {
	  public:
		GuildMemberData member{};
		InteractionType type{};
		std::string name{};
		UserData user{};

		MessageInteractionData() = default;

		MessageInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageInteractionData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<InteractionType>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}
		}
	};

	/// Message types. \brief Message types.
	enum class MessageType : int8_t {
		Default = 0,///< Default.
		Recipient_Add = 1,///< Recipient add.
		Recipient_Remove = 2,///< Recipient remove.
		Call = 3,///< Call.
		Channel_Name_Change = 4,///< Channel name change.
		Channel_Icon_Change = 5,///< Channel icon change.
		Channel_Pinned_Message = 6,///< Channel pinned Message.
		Guild_Member_Join = 7,///< Guild memeber join.
		User_Premium_Guild_Subscription = 8,///< User premium Guild subscription.
		User_Premium_Guild_Subscription_Tier_1 = 9,///< User premium Guild subscription tier 1.
		User_Premium_Guild_Subscription_Tier_2 = 10,///< User premium Guild subscription tier 2.
		User_Premium_Guild_Subscription_Tier_3 = 11,///< User premium Guild subscription tier 3.
		Channel_Follow_Add = 12,///< Channel follow add.
		Guild_Discovery_Disqualified = 14,///< Guild discovery disqualified.
		Guild_Discovery_Requalified = 15,///< Guild discovery requalified.
		Guild_Discovery_Grace_Period_Initial_Warning = 16,///< Guild discovery grade period initial warning.
		Guild_Discovery_Grace_Period_Final_Warning = 17,///< Guild discovery grade period final warning.
		Thread_Created = 18,///< Thread created.
		Reply = 19,///< Reply.
		Chat_Input_Command = 20,///< Chat input command.
		Thread_Starter_Message = 21,///< Thread starter Message.
		Guild_Invite_Reminder = 22,///< Guild invite reminder.
		Context_Menu_Command = 23///< Context menu command.
	};

	/// Message flags. \brief Message flags.
	enum class MessageFlags : uint8_t {
		Crossposted = 1 << 0,///< Crossposted.
		Is_Crosspost = 1 << 1,///< Is crosspost.
		Suppress_Embeds = 1 << 2,///< Supress embeds.
		Source_Message_Deleted = 1 << 3,///< Source Message deleted.
		Urgent = 1 << 4,///< Urgent.
		Has_Thread = 1 << 5,///< Has Thread.
		Ephemeral = 1 << 6,///< Ephemeral.
		Loading = 1 << 7///< Loading.
	};

	/// Sticker item types. \brief Sticker item types.
	enum class StickerItemType : int8_t {
		Png = 1,///< Png.
		Apng = 2,///< Apng.
		Lottie = 3///< Lottie.
	};

	/// Represents a forum thread message. \brief Represents a forum thread message.
	struct DiscordCoreAPI_Dll ForumThreadMessageData {
		std::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename.
		std::vector<ActionRowData> components{};///< Array of message component objects the components to include with the message.
		AllowedMentionsData allowedMentions{};///< Allowed mention object allowed mentions for the message.
		std::vector<std::string> stickerIds{};///< Array of snowflakes IDs of up to 3 stickers in the server to send in the message.
		std::vector<EmbedData> embeds{};///< Array of embed objects	embedded rich content (up to 6000 characters).
		std::vector<File> files{};///< File contents the contents of the file being sent one of content, file, embed(s), sticker_ids.
		std::string content{};///< The message contents (up to 2000 characters).
		int32_t flags{ 0 };///< Flags to be set for the message.
	};

	/// Message Sticker item data. \brief Message Sticker item data.
	class DiscordCoreAPI_Dll StickerItemData : public DiscordEntity, public DataParser<StickerItemData> {
	  public:
		StickerItemType formatType{};///< Message Sticker item type.
		std::string name{};///< The name of the Sticker.

		StickerItemData() = default;

		StickerItemData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerItemData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerItemData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerItemData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
				pDataStructure->formatType = jsonObjectData["format_type"].get<StickerItemType>();
			}
		}
	};

	/// The core of a Message's data structure. \brief The core of a Message's data structure.
	class DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity, public DataParser<MessageDataOld> {
	  public:
		std::vector<ChannelMentionData> mentionChannels{};///< array of Channel mention data.
		std::vector<StickerItemData> stickerItems{};///< Array of Message Sticker item data.
		std::vector<AttachmentData> attachments{};///< Array of attachment data.
		MessageReferenceData messageReference{};///< Message reference data.
		std::vector<std::string> mentionRoles{};///< Vector of "mention roles" ids.
		std::vector<ActionRowData> components{};///< Array of action row data.
		std::vector<ReactionData> reactions{};//< Array of reaction data.
		MessageInteractionData interaction{};///< Message Interaction data.
		std::vector<StickerData> stickers{};///< Array of Message Sticker data.
		std::vector<UserData> mentions{};///< Array of User data, for individual's that were mentioned.
		std::vector<EmbedData> embeds{};///< Array of Message embeds.
		TimeStamp editedTimestamp{ "" };///< The time at which it was edited.
		MessageActivityData activity{};///< Message activity data.
		bool mentionEveryone{ false };///< Does the Message mention everyone?
		ApplicationData application{};///< Application data.
		TimeStamp timestamp{ "" };///< The timestamp of when the Message was created.
		Snowflake applicationId{};///< Application id.
		GuildMemberData member{};///< The author's Guild member data.
		std::string content{};///< The Message's content.
		Snowflake channelId{};///< The Channel it was sent in.
		Snowflake webHookId{};///< WebHook id of the Message, if applicable.
		bool pinned{ false };///< Is it pinned?
		ChannelData thread{};///< The Thread that the Message was sent in, if applicable.
		std::string nonce{};///< Nonce.
		int32_t flags{ 0 };///< Flags.
		Snowflake guildId{};///< The id of the Guild the Message was sent in.
		MessageType type{};///< Message type.
		UserData author{};///< The author's User data.
		bool tts{ false };///< Is it a text-to-speech Message?

		MessageDataOld() = default;

		MessageDataOld& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageDataOld(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageDataOld() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageDataOld* pDataStructure) {
			if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
				pDataStructure->content = jsonObjectData["content"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
				pDataStructure->author = jsonObjectData["author"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
				pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();


				if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
					pDataStructure->tts = jsonObjectData["tts"].get<bool>();
				}

				if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
					pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
				}

				if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
					pDataStructure->mentions.clear();
					for (auto& value: jsonObjectData["mentions"]) {
						UserData newData{ value };
						pDataStructure->mentions.push_back(newData);
					}
				}

				if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
					pDataStructure->mentionRoles.clear();
					for (auto& value: jsonObjectData["mention_roles"]) {
						pDataStructure->mentionRoles.push_back(value.get<std::string>());
					}
				}

				if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
					pDataStructure->mentionChannels.clear();
					for (auto& value: jsonObjectData["mention_channels"]) {
						ChannelMentionData newData{ value };
						pDataStructure->mentionChannels.push_back(newData);
					}
				}

				if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
					pDataStructure->attachments.clear();
					for (auto& value: jsonObjectData["attachments"]) {
						AttachmentData newData{ value };
						pDataStructure->attachments.push_back(newData);
					}
				}

				if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
					pDataStructure->embeds.clear();
					for (auto& value: jsonObjectData["embeds"]) {
						EmbedData newData{ value };
						pDataStructure->embeds.push_back(newData);
					}
				}

				if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
					pDataStructure->reactions.clear();
					for (auto& value: jsonObjectData["reactions"]) {
						ReactionData newData{ value };
						pDataStructure->reactions.push_back(newData);
					}
				}

				if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
					pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
				}

				if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
					pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
				}

				if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
					pDataStructure->webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
				}

				if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
					pDataStructure->type = jsonObjectData["type"].get<MessageType>();
				}

				if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
					pDataStructure->activity = jsonObjectData["activity"];
				}

				if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
					pDataStructure->application = jsonObjectData["application"];
				}

				if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
					pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
				}

				if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
					pDataStructure->messageReference = jsonObjectData["message_reference"];
				}

				if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
					pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
				}

				if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
					pDataStructure->stickerItems.clear();
					for (auto& value: jsonObjectData["sticker_items"]) {
						StickerItemData newData{ value };
						pDataStructure->stickerItems.push_back(newData);
					}
				}

				if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
					pDataStructure->stickers.clear();
					for (auto& value: jsonObjectData["stickers"]) {
						StickerData newData{ value };
						pDataStructure->stickers.push_back(newData);
					}
				}

				if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
					pDataStructure->interaction = jsonObjectData["interaction"];
				}

				if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
					pDataStructure->components.clear();
					for (auto& value: jsonObjectData["components"]) {
						ActionRowData newData{ value };
						pDataStructure->components.push_back(newData);
					}
				}

				if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
					pDataStructure->thread = jsonObjectData["thread"];
				}
			}
		}
	};

	/// Data structure representing a single Message. \brief Data structure representing a single Message.
	class DiscordCoreAPI_Dll MessageData : public MessageDataOld, public DataParser<MessageData> {
	  public:
		std::unique_ptr<MessageDataOld> referencedMessage{ std::make_unique<MessageDataOld>() };///< The referenced Message, to reply to.

		MessageData& operator=(const MessageData& other) {
			if (this != &other) {
				*this->referencedMessage = *other.referencedMessage;
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

		MessageData(const MessageData& other) {
			*this = other;
		}

		MessageData() = default;

		MessageData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageData* pDataStructure) {
			if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
				pDataStructure->content = jsonObjectData["content"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
				pDataStructure->author = jsonObjectData["author"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
				pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
				pDataStructure->tts = jsonObjectData["tts"].get<bool>();
			}

			if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
				pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
			}

			if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
				pDataStructure->mentions.clear();
				for (auto& value: jsonObjectData["mentions"]) {
					UserData newData{ value };
					pDataStructure->mentions.push_back(newData);
				}
			}

			if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
				pDataStructure->mentionRoles.clear();
				for (auto& value: jsonObjectData["mention_roles"]) {
					pDataStructure->mentionRoles.push_back(value.get<std::string>());
				}
			}

			if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
				pDataStructure->mentionChannels.clear();
				for (auto& value: jsonObjectData["mention_channels"]) {
					ChannelMentionData newData{ value };
					pDataStructure->mentionChannels.push_back(newData);
				}
			}

			if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
				pDataStructure->attachments.clear();
				for (auto& value: jsonObjectData["attachments"]) {
					AttachmentData newData{ value };
					pDataStructure->attachments.push_back(newData);
				}
			}

			if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
				pDataStructure->embeds.clear();
				for (auto& value: jsonObjectData["embeds"]) {
					EmbedData newData{ value };
					pDataStructure->embeds.push_back(newData);
				}
			}

			if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
				pDataStructure->reactions.clear();
				for (auto& value: jsonObjectData["reactions"]) {
					ReactionData newData{ value };
					pDataStructure->reactions.push_back(newData);
				}
			}

			if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
				pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
			}

			if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
				pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
			}

			if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
				pDataStructure->webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<MessageType>();
			}

			if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
				pDataStructure->activity = jsonObjectData["activity"];
			}

			if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
				pDataStructure->application = jsonObjectData["application"];
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
				pDataStructure->messageReference = jsonObjectData["message_reference"];
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
			}

			if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
				pDataStructure->stickerItems.clear();
				for (auto& value: jsonObjectData["sticker_items"]) {
					StickerItemData newData{ value };
					pDataStructure->stickerItems.push_back(newData);
				}
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
			}

			if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
				pDataStructure->interaction = jsonObjectData["interaction"];
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->components.clear();
				for (auto& value: jsonObjectData["components"]) {
					ActionRowData newData{ value };
					pDataStructure->components.push_back(newData);
				}
			}

			if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
				pDataStructure->thread = jsonObjectData["thread"];
			}
		}
	};

	/// Resolved data. \brief Resolved data.
	struct DiscordCoreAPI_Dll ResolvedData {
		std::unordered_map<Snowflake, AttachmentData> attachments{};///< Map of Snowflakes to attachment objects the ids and attachment objects.
		std::unordered_map<Snowflake, GuildMemberData> members{};///< Map full of GuildMemeberData.
		std::unordered_map<Snowflake, MessageData> messages{};///< Map full of messageData->
		std::unordered_map<Snowflake, ChannelData> channels{};///< Map full of ChannelData.
		std::unordered_map<Snowflake, UserData> users{};///< Map full of UserData.
		std::unordered_map<Snowflake, RoleData> roles{};///< Map full of RoleData.
	};

	/// Represents a Sticker pack. \brief Represents a Sticker pack.
	struct DiscordCoreAPI_Dll StickerPackData : public DataParser<StickerPackData> {
		std::vector<StickerData> stickers{};///< Array of Sticker objects	the stickers in the pack.
		std::string coverStickerId{};///< Id of a Sticker in the pack which is shown as the pack's icon.
		std::string bannerAssetId{};///< Id of the Sticker pack's banner image.
		std::string description{};///< Description of the Sticker pack.
		std::string skuId{};///< Id of the pack's SKU.
		std::string name{};///< Name of the Sticker pack.
		std::string Id{};///< Id of the Sticker pack.

		StickerPackData() = default;

		StickerPackData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerPackData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerPackData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerPackData* pDataStructure) {
			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
			}

			if (jsonObjectData.contains("cover_sticker_id") && !jsonObjectData["cover_sticker_id"].is_null()) {
				pDataStructure->coverStickerId = jsonObjectData["cover_sticker_id"].get<std::string>();
			}

			if (jsonObjectData.contains("banner_asset_id") && !jsonObjectData["banner_asset_id"].is_null()) {
				pDataStructure->bannerAssetId = jsonObjectData["banner_asset_id"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->Id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("sku_id") && !jsonObjectData["sku_id"].is_null()) {
				pDataStructure->skuId = jsonObjectData["sku_id"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll StickerPackDataVector : public DataParser<StickerPackDataVector> {
	  public:
		StickerPackDataVector() = default;

		operator std::vector<StickerPackData>() {
			return this->theStickerPackDatas;
		}

		StickerPackDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerPackDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerPackDataVector() = default;

	  protected:
		std::vector<StickerPackData> theStickerPackDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, StickerPackDataVector* pDataStructure) {
			pDataStructure->theStickerPackDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::StickerPackData newData{ value };
				pDataStructure->theStickerPackDatas.push_back(newData);
			}
			pDataStructure->theStickerPackDatas.shrink_to_fit();
		}
	};

	/// Connection visibility types. \brief Connection visibility types.
	enum class ConnectionVisibilityTypes : int8_t {
		None = 0,///< None.
		Everyone = 1///< Everyone.
	};

	/// Represents a single User Connection. \brief Represents a single User Connection.
	struct DiscordCoreAPI_Dll ConnectionData : public DataParser<ConnectionData> {
		std::vector<IntegrationData> integrations{};///< An array of partial server integrations.
		ConnectionVisibilityTypes visibility{};///< Visibility of this connection.
		bool showActivity{ false };///< Whether activities related to this connection will be shown in presence updates.
		bool friendSync{ false };///< Whether friend sync is enabled for this connection.
		bool verified{ false };///< Whether the connection is verified.
		bool revoked{ false };///< Whether the connection is revoked.
		std::string name{};///< The userName of the connection account.
		std::string type{};///< The service of the connection(twitch, youtube).
		Snowflake id{};///< Id of the connection account.

		ConnectionData() = default;

		ConnectionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ConnectionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ConnectionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ConnectionData* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<std::string>();
			}

			if (jsonObjectData.contains("show_activity") && !jsonObjectData["show_activity"].is_null()) {
				pDataStructure->showActivity = jsonObjectData["show_activity"].get<bool>();
			}

			if (jsonObjectData.contains("friend_sync") && !jsonObjectData["friend_sync"].is_null()) {
				pDataStructure->friendSync = jsonObjectData["friend_sync"].get<bool>();
			}

			if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
				pDataStructure->revoked = jsonObjectData["revoked"].get<bool>();
			}

			if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
				pDataStructure->verified = jsonObjectData["verified"].get<bool>();
			}

			if (jsonObjectData.contains("visibility") && !jsonObjectData["visibility"].is_null()) {
				pDataStructure->visibility = jsonObjectData["visibility"].get<ConnectionVisibilityTypes>();
			}

			if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
				pDataStructure->integrations.clear();
				for (auto& value: jsonObjectData["integrations"]) {
					IntegrationData newData{ value };
					pDataStructure->integrations.push_back(newData);
				}
			}
		}
	};

	class DiscordCoreAPI_Dll ConnectionDataVector : public DataParser<ConnectionDataVector> {
	  public:
		ConnectionDataVector() = default;

		operator std::vector<ConnectionData>() {
			return this->theConnectionDatas;
		}

		ConnectionDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ConnectionDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ConnectionDataVector() = default;

	  protected:
		std::vector<ConnectionData> theConnectionDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, ConnectionDataVector* pDataStructure) {
			pDataStructure->theConnectionDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::ConnectionData newData{ value };
				pDataStructure->theConnectionDatas.push_back(newData);
			}
			pDataStructure->theConnectionDatas.shrink_to_fit();
		}
	};

	/// ApplicationCommand Interaction data option. \brief ApplicationCommand Interaction data option.
	struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption : public DataParser<ApplicationCommandInteractionDataOption> {
		std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
		std::string valueString{};///< The value if it's a string.
		bool valueBool{ false };///< the value if it's a bool.
		int32_t valueInt{ 0 };///< The value if it's an int32_t.
		bool focused{ false };///< 	True if this option is the currently focused option for autocomplete.
		std::string name{};///< The name of the current option.

		ApplicationCommandInteractionDataOption() = default;

		ApplicationCommandInteractionDataOption& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandInteractionDataOption(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandInteractionDataOption() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionDataOption* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
			}

			if (jsonObjectData.contains("focused") && !jsonObjectData["focused"].is_null()) {
				pDataStructure->focused = jsonObjectData["focused"].get<bool>();
			}

			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
				if (jsonObjectData["value"].is_boolean()) {
					pDataStructure->valueBool = jsonObjectData["value"].get<bool>();
				}
				if (jsonObjectData["value"].is_string()) {
					pDataStructure->valueString = jsonObjectData["value"].get<std::string>();
				}
				if (jsonObjectData["value"].is_number()) {
					pDataStructure->valueInt = jsonObjectData["value"].get<int32_t>();
				}
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options.clear();
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandInteractionDataOption newData{ value };
					pDataStructure->options.push_back(newData);
				}
			}
		}
	};

	/// ApplicationCommand Interaction data. \brief ApplicationCommand Interaction data.
	class DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity, public DataParser<ApplicationCommandInteractionData> {
	  public:
		std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		ResolvedData resolved{};///< Resolved data.
		Snowflake guildId{};///< The guild that the command took place in.
		std::string name{};///< The name of the command.

		ApplicationCommandInteractionData() = default;

		ApplicationCommandInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionData* pDataStructure) {
			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandType>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options.clear();
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandInteractionDataOption newData{ value };
					pDataStructure->options.push_back(newData);
				}
			}

			if (jsonObjectData.contains("resolved") && !jsonObjectData["resolved"].is_null()) {
				auto& value = jsonObjectData["resolved"];

				if (value.contains("attachments") && !value["attachments"].is_null()) {
					pDataStructure->resolved.attachments.clear();
					auto newMap = value["attachments"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						AttachmentData newData{ newValue };
						pDataStructure->resolved.attachments[stoull(key)] = newData;
					}
				}

				if (value.contains("users") && !value["users"].is_null()) {
					pDataStructure->resolved.users.clear();
					auto newMap = value["users"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						UserData newData{ value };
						pDataStructure->resolved.users[stoull(key)] = newData;
					}
				}
				if (value.contains("channels") && !value["channels"].is_null()) {
					pDataStructure->resolved.channels.clear();
					auto newMap = value["channels"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						ChannelData newData{ newValue };
						pDataStructure->resolved.channels[stoull(key)] = newData;
					}
				}
				if (value.contains("roles") && !value["roles"].is_null()) {
					pDataStructure->resolved.roles.clear();
					auto newMap = value["roles"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						RoleData newData{ newValue };
						pDataStructure->resolved.roles[stoull(key)] = newData;
					}
				}
				if (value.contains("members") && !value["members"].is_null()) {
					pDataStructure->resolved.members.clear();
					auto newMap = value["members"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						GuildMemberData newData{ newValue };
						pDataStructure->resolved.members[stoull(key)] = newData;
					}
				}
				if (value.contains("messages") && !value["messages"].is_null()) {
					pDataStructure->resolved.messages.clear();
					auto newMap = value["messages"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						MessageData newData{ newValue };
						pDataStructure->resolved.messages[stoull(key)] = newData;
					}
				}
			}
		}
	};

	/// Interaction data data. \brief Interaction data data.
	struct DiscordCoreAPI_Dll InteractionDataData : public DataParser<InteractionDataData> {
		ApplicationCommandInteractionData applicationCommandData{};///< ApplicationCommand Interaction data.
		MessageCommandInteractionData messageInteractionData{};///< Message command Interaction data.
		UserCommandInteractionData userInteractionData{};///< User command Interaction data.
		ComponentInteractionData componentData{};///< Component Interaction data.
		ModalInteractionData modalData{};///< Modal Interaction data.

		InteractionDataData() = default;

		InteractionDataData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InteractionDataData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InteractionDataData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InteractionDataData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->applicationCommandData = jsonObjectData;
			}

			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->messageInteractionData = jsonObjectData;
				pDataStructure->userInteractionData = jsonObjectData;
			}

			if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
				pDataStructure->componentData = jsonObjectData;
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->modalData = jsonObjectData;
			}
		}
	};

	/// Interaction data. \brief Interaction data.
	class DiscordCoreAPI_Dll InteractionData : public DiscordEntity, public DataParser<InteractionData> {
	  public:
		std::string appPermissions{};///< Bitwise set of permissions the app or bot has within the channel the interaction was sent from.
		InteractionDataData data{};///< The Interaction's data.
		std::string guildLocale{};///< The guild's preferred locale, if invoked in a guild.
		Snowflake applicationId{};///< The application's id.
		nlohmann::json rawData{};///< The Interaction's raw data.
		GuildMemberData member{};///< The data of the Guild member who sent the Interaction, if applicable.
		InteractionType type{};///< The type of Interaction.
		MessageData message{};///< The Message that the Interaction came through on, if applicable.
		Snowflake channelId{};///< The Channel the Interaction was sent in.
		int32_t version{ 0 };///< The Interaction version.
		std::string locale{};///< The selected language of the invoking user.
		std::string token{};///< The Interaction token.
		Snowflake guildId{};///< The Guild id of the Guild it was sent in.
		UserData user{};///< The User data of the sender of the Interaction.

		InteractionData() = default;

		InteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InteractionData* pDataStructure) {
			if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
				pDataStructure->data = jsonObjectData["data"];
				pDataStructure->rawData = jsonObjectData["data"];
			}

			if (jsonObjectData.contains("app_permissions") && !jsonObjectData["app_permissions"].is_null()) {
				pDataStructure->appPermissions = jsonObjectData["app_permissions"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<InteractionType>();
			}

			if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
				pDataStructure->token = jsonObjectData["token"].get<std::string>();
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
				pDataStructure->user.avatar = pDataStructure->member.userAvatar;
				pDataStructure->user.id = pDataStructure->member.id;
				pDataStructure->user.userName = pDataStructure->member.userName;
			} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
				pDataStructure->locale = jsonObjectData["locale"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_locale") && !jsonObjectData["guild_locale"].is_null()) {
				pDataStructure->guildLocale = jsonObjectData["guild_locale"].get<std::string>();
			}

			if (jsonObjectData.contains("message") && !jsonObjectData["message"].is_null()) {
				pDataStructure->message = jsonObjectData["message"];
			}

			if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
				pDataStructure->version = jsonObjectData["version"].get<int32_t>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}
		}
	};

	/// Data from the SessionStart info. \brief Data from the SessionStart info.
	struct DiscordCoreAPI_Dll SessionStartData : public DataParser<SessionStartData> {
		uint32_t maxConcurrency{ 0 };///< The number of identify requests allowed per 5 seconds.
		uint32_t resetAfter{ 0 };///< The number of std::chrono::milliseconds after which the limit resets.
		uint32_t remaining{ 0 };///< The remaining number of session starts the current User is allowed.
		uint32_t total{ 0 };///< The total number of session starts the current User is allowed.

		SessionStartData() = default;

		SessionStartData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		SessionStartData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~SessionStartData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SessionStartData* pDataStructure) {
			if (jsonObjectData.contains("max_concurrency") && !jsonObjectData["max_concurrency"].is_null()) {
				pDataStructure->maxConcurrency = jsonObjectData["max_concurrency"].get<uint32_t>();
			}

			if (jsonObjectData.contains("remaining") && !jsonObjectData["remaining"].is_null()) {
				pDataStructure->remaining = jsonObjectData["remaining"].get<uint32_t>();
			}

			if (jsonObjectData.contains("reset_after") && !jsonObjectData["reset_after"].is_null()) {
				pDataStructure->resetAfter = jsonObjectData["reset_after"].get<uint32_t>();
			}

			if (jsonObjectData.contains("total") && !jsonObjectData["total"].is_null()) {
				pDataStructure->total = jsonObjectData["total"].get<uint32_t>();
			}
		}
	};

	/// Data from the GetGatewatBot endpoint. \brief Data from the GetGatewatBot endpoint.
	struct DiscordCoreAPI_Dll GatewayBotData : public DataParser<GatewayBotData> {
		SessionStartData sessionStartLimit{};///< Information on the current session start limit.
		uint32_t shards{ 0 };///< The recommended number of shards to use when connecting.
		std::string url{};///< The WSS Url that can be used for connecting to the gateway.

		GatewayBotData() = default;

		GatewayBotData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GatewayBotData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GatewayBotData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GatewayBotData* pDataStructure) {
			if (jsonObjectData.contains("session_start_limit") && !jsonObjectData["session_start_limit"].is_null()) {
				pDataStructure->sessionStartLimit = jsonObjectData["session_start_limit"];
			}

			if (jsonObjectData.contains("shards") && !jsonObjectData["shards"].is_null()) {
				pDataStructure->shards = jsonObjectData["shards"].get<uint32_t>();
			}

			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}
		}
	};

	/// Text input style for modals. \brief Text input style for modals.
	enum class TextInputStyle : int8_t {
		Short = 1,///< A single-line input.
		Paragraph = 2///< A multi-line input.
	};

	/// Data representing a Guild Emoji Update event. \brief Data representing a Guild Emoji Update event.
	struct DiscordCoreAPI_Dll GuildEmojisUpdateEventData : public DataParser<GuildEmojisUpdateEventData> {
		std::vector<EmojiData> emojis{};
		Snowflake guildId{};

		GuildEmojisUpdateEventData() = default;

		GuildEmojisUpdateEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildEmojisUpdateEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildEmojisUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildEmojisUpdateEventData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
				pDataStructure->emojis.clear();
				for (auto& value: jsonObjectData["emojis"]) {
					EmojiData newData{ value };
					pDataStructure->emojis.push_back(newData);
				}
			}
		}
	};

	/// Data representing a Guild Sticker Update event. \brief Data representing a Guild Stickers Update event.
	struct DiscordCoreAPI_Dll GuildStickersUpdateEventData : public DataParser<GuildStickersUpdateEventData> {
		std::vector<StickerData> stickers{};
		Snowflake guildId{};

		GuildStickersUpdateEventData() = default;

		GuildStickersUpdateEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildStickersUpdateEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildStickersUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildStickersUpdateEventData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
			}
		}
	};

	struct DiscordCoreAPI_Dll GuildMembersChunkEventData : public DataParser<GuildMembersChunkEventData> {
		std::vector<PresenceUpdateData> presences{};
		std::vector<GuildMemberData> members{};
		std::vector<std::string> notFound{};
		int32_t chunkIndex{ 0 };
		int32_t chunkCount{ 0 };
		std::string nonce{};
		Snowflake guildId{};

		GuildMembersChunkEventData() = default;

		GuildMembersChunkEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildMembersChunkEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildMembersChunkEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildMembersChunkEventData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
				pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
			}

			if (jsonObjectData.contains("chunk_index") && !jsonObjectData["chunk_index"].is_null()) {
				pDataStructure->chunkIndex = jsonObjectData["chunk_index"].get<int32_t>();
			}

			if (jsonObjectData.contains("chunk_count") && !jsonObjectData["chunk_count"].is_null()) {
				pDataStructure->chunkCount = jsonObjectData["chunk_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
				pDataStructure->presences.clear();
				for (auto& value: jsonObjectData["presences"]) {
					PresenceUpdateData newData{ value };
					pDataStructure->presences.push_back(newData);
				}
			}

			if (jsonObjectData.contains("not_found") && !jsonObjectData["not_found"].is_null()) {
				pDataStructure->notFound.clear();
				for (auto& value: jsonObjectData["not_found"]) {
					pDataStructure->notFound.push_back(value);
				}
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				for (auto& value: jsonObjectData["members"]) {
					GuildMemberData newData{ value };
					pDataStructure->members.push_back(newData);
				}
			}
		}
	};

	/// Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input. \brief Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input.
	class DiscordCoreAPI_Dll InputEventData {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend OnInteractionCreationData;
		friend RespondToInputEventData;
		friend BaseFunctionArguments;
		friend DiscordCoreClient;
		friend EventHandler;
		friend CommandData;
		friend InputEvents;

		InputEventResponseType responseType{};///< The type of response that this input value represents.

		InputEventData& operator=(const InputEventData& other);

		InputEventData(const InputEventData& other);

		InputEventData& operator=(InputEventData& other);

		InputEventData(InputEventData& other);

		InputEventData& operator=(InteractionData& other);

		InputEventData(InteractionData& interactionData);

		InputEventData() = default;

		/// Returns the userName of the last User to trigger this input-event. \brief Returns the userName of the last User to trigger this input-event.
		/// \returns std::string A string containing the User name.
		std::string getUserName();

		/// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
		/// \returns std::string A string containing the avatar Url.
		std::string getAvatarUrl();

		/// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
		/// \returns std::vector A vector containing the EmbedData.
		std::vector<EmbedData> getEmbeds();

		/// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
		/// \returns std::vector A vector containing ActionRowData.
		std::vector<ActionRowData> getComponents();

		/// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
		/// \returns Snowflake A Snowflake containing the author's id.
		Snowflake getAuthorId();

		/// Returns the Interaction id, if appplicable, of this input-event. \brief Returns the Interaction id, if appplicable, of this input-event.
		/// \returns Snowflake A Snowflake containing the Interaction id.
		Snowflake getInteractionId();

		/// Returns the application id. \brief Returns the application id.
		/// \returns Snowflake A Snowflake containing the application id.
		Snowflake getApplicationId();

		/// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
		/// \returns Snowflake A Snowflake containing the Channel id.
		Snowflake getChannelId();

		/// Returns the Interaction token, if applicable, of this input-event. \brief Returns the Interaction token, if applicable, of this input-event.
		/// \returns std::string A string containing the Interaction token.
		std::string getInteractionToken();

		/// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
		/// \returns Snowflake A Snowflake containing the Guild id.
		Snowflake getGuildId();

		/// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
		/// \returns Snowflake A Snowflake containing the Message id.
		Snowflake getMessageId();

		/// Returns the Interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
		/// \returns InteractionData An InteractionData structure.
		InteractionData getInteractionData();

		/// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
		/// \returns MessageData A MessageData structure.
		MessageData getMessageData();

		virtual ~InputEventData() = default;

	  protected:
		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
	};

	/// \brief Data for responding to an input-event.
	class DiscordCoreAPI_Dll RespondToInputEventData {
	  public:
		friend CreateEphemeralInteractionResponseData;
		friend CreateDeferredInteractionResponseData;
		friend CreateEphemeralFollowUpMessageData;
		friend struct InteractionResponseData;
		friend DeleteInteractionResponseData;
		friend CreateInteractionResponseData;
		friend EditInteractionResponseData;
		friend DeleteFollowUpMessageData;
		friend CreateFollowUpMessageData;
		friend EditFollowUpMessageData;
		friend CreateMessageData;
		friend EditMessageData;
		friend InputEvents;
		friend SendDMData;

		RespondToInputEventData& operator=(InteractionData& dataPackage);

		RespondToInputEventData(InteractionData& dataPackage);

		RespondToInputEventData& operator=(InputEventData& dataPackage);

		RespondToInputEventData(InputEventData& dataPackage);

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
			const std::string& emojiName = "", Snowflake emojiId = 0, const std::string& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder,
			int32_t maxValues, int32_t minValues);

		/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
		/// \param topTitleNew A title for the modal.
		/// \param topCustomIdNew A custom id to give for the modal.
		/// \param titleNew A title for the modal's individual input.
		/// \param customIdNew A custom id to give for the modal's individual input.
		/// \param required Is it a required response?
		/// \param minLength Minimum length.
		/// \param maxLength Maximum length.
		/// \param inputStyle The input style.
		/// \param label A label for the modal.
		/// \param placeholder A placeholder for the modal.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew,
			bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "");

		/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addFile(File theFile);

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addAllowedMentions(AllowedMentionsData dataPackage);

		/// For setting the type of response to make. \brief For setting the type of response to make.
		/// \param typeNew An InputEventResponseType.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setResponseType(InputEventResponseType typeNew);

		/// For setting the components in a response. \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addComponentRow(ActionRowData dataPackage);

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addMessageEmbed(EmbedData dataPackage);

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addContent(const std::string& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTTSStatus(bool enabledTTs);

		/// For setting the choices of an autocomplete response. \brief For setting the choices of an autocomplete response.
		/// \param theValue An nlohmann::json value that is either a float, int32_t or a string.
		/// \param theName A string for the name of the choice.
		/// \param theNameLocalizations A std::unordered_map<std::string, std::string> for the name localizations.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setAutoCompleteChoice(nlohmann::json theValue, const std::string& theName,
			std::unordered_map<std::string, std::string> theNameLocalizations = std::unordered_map<std::string, std::string>{});

		/// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
		/// \param targetUserIdNew A string, containing the target User's id.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTargetUserID(const Snowflake& targetUserIdNew);

	  protected:
		std::vector<ApplicationCommandOptionChoiceData> choices{};
		std::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		std::vector<EmbedData> embeds{};
		std::string interactionToken{};
		InputEventResponseType type{};///< The type of response to make.
		InteractionType eventType{};
		std::vector<File> files{};
		Snowflake interactionId{};
		Snowflake applicationId{};
		Snowflake targetUserId{};
		std::string customId{};
		std::string content{};
		Snowflake channelId{};
		Snowflake messageId{};
		std::string title{};
		int32_t flags{ 0 };
		bool tts{ false };
	};

	/// Message response base, for responding to messages. \brief Message response base, for responding to messages.
	class DiscordCoreAPI_Dll MessageResponseBase {
	  public:
		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle, const std::string& emojiName = "",
			Snowflake emojiId = 0, const std::string& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues,
			int32_t minValues);

		/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
		/// \param topTitleNew A title for the modal.
		/// \param topCustomIdNew A custom id to give for the modal.
		/// \param titleNew A title for the modal's individual input.
		/// \param customIdNew A custom id to give for the modal's individual input.
		/// \param required Is it a required response?
		/// \param minLength Minimum length.
		/// \param maxLength Maximum length.
		/// \param inputStyle The input style.
		/// \param label A label for the modal.
		/// \param placeholder A placeholder for the modal.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew, bool required,
			int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "");

		/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addFile(File theFile);

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addAllowedMentions(AllowedMentionsData dataPackage);

		/// For setting the components in a response. \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addComponentRow(ActionRowData dataPackage);

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addMessageEmbed(EmbedData dataPackage);

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addContent(const std::string& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& setTTSStatus(bool enabledTTs);

	  protected:
		std::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		std::vector<EmbedData> embeds{};
		std::vector<File> files{};
		std::string customId{};
		std::string content{};
		std::string title{};
		int32_t flags{ 0 };
		bool tts{ false };
	};

	/// Interaction response data. \brief Interaction response data.
	struct DiscordCoreAPI_Dll InteractionResponseData {
		InteractionResponseData& operator=(const RespondToInputEventData& other) {
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
		InteractionResponseData(const RespondToInputEventData& other) {
			*this = other;
		}
		InteractionResponseData() = default;

		operator std::string() {
			nlohmann::json data{};
			for (auto& value: this->data.attachments) {
				data["data"]["attachments"].push_back(DiscordCoreAPI::AttachmentData{ value });
			}
			if (this->data.components.size() == 0) {
				data["data"]["components"] = nlohmann::json::array();
			} else {
				for (auto& value: this->data.components) {
					data["data"]["components"].push_back(value);
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
					theArray.push_back(theValue);
				}
				data["data"]["choices"] = theArray;
			}
			if (this->data.embeds.size() == 0) {
				data["data"]["embeds"] = nlohmann::json::array();
			} else {
				for (auto& value: this->data.embeds) {
					data["data"]["embeds"].push_back(DiscordCoreAPI::EmbedData{ value });
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
			return data.dump();
		}

		InteractionCallbackData data{};///< Interaction ApplicationCommand callback data.
		InteractionCallbackType type{};///< Interaction callback type.
	};

	/// Command data, for functions executed by the CommandController. \brief Command data, for functions executed by the CommandController.
	class DiscordCoreAPI_Dll CommandData : public DataParser<CommandData> {
		friend struct DiscordCoreAPI_Dll BaseFunctionArguments;

		std::vector<std::string> optionsArgs{};
		std::string subCommandGroupName{};
		std::string subCommandName{};
		std::string commandName{};

		CommandData() = default;

		CommandData(InputEventData inputEventData);

		CommandData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		CommandData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~CommandData() = default;

	  protected:
		InputEventData eventData{};

		void parseObject(const nlohmann::json& jsonObjectData, CommandData* pDataStructure) {
			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->optionsArgs.clear();
				for (auto& value: jsonObjectData["options"]) {
					if (value.contains("type") && value["type"] == 1) {
						if (value.contains("name")) {
							pDataStructure->subCommandName = value["name"];
						}
					} else if (value.contains("type") && value["type"] == 2) {
						if (value.contains("name")) {
							pDataStructure->subCommandGroupName = value["name"];
						}
					}
					if (value.contains("options")) {
						parseObject(value, pDataStructure);
					}
					if (value.contains("value") && !value["value"].is_null()) {
						auto& newValueNew = value["value"];
						if (newValueNew.is_string()) {
							pDataStructure->optionsArgs.push_back(newValueNew);
						} else if (newValueNew.is_number()) {
							pDataStructure->optionsArgs.push_back(std::to_string(newValueNew.get<int64_t>()));
						} else if (newValueNew.is_boolean()) {
							pDataStructure->optionsArgs.push_back(std::to_string(newValueNew.get<bool>()));
						}
					}
				}
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->commandName = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	/// Guild application command permissions data. \brief Guild application command permissions data.
	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordEntity {
	  public:
		std::vector<ApplicationCommandPermissionData> permissions{};
		Snowflake applicationId{};
		Snowflake guildId{};

		virtual ~GuildApplicationCommandPermissionData() = default;
	};


	/// Song types. \brief Song types.
	enum class SongType : int8_t {
		YouTube = 0,///< YouTube.
		SoundCloud = 1///< SoundCloud.
	};

	/// Represents a download Url. \brief Represents a download Url.
	struct DiscordCoreAPI_Dll DownloadUrl {
		int32_t contentSize{ 0 };
		std::string urlPath{};
	};

	/// Represents a single frame of raw audio data. \brief Represents a single frame of raw audio data.
	struct DiscordCoreAPI_Dll RawFrameData {
		std::vector<uint8_t> data{};///< The audio data.
		int32_t sampleCount{ -1 };///< The number of samples per this frame.
	};

	/// Represents a single frame of encoded audio data. \brief Represents a single frame of encoded audio data.
	struct DiscordCoreAPI_Dll EncodedFrameData {
		std::vector<uint8_t> data{};///< The audio data.
		int32_t sampleCount{ -1 };///< The number of samples per this frame.
	};

	/// Audio frame types. \brief Audio frame types.
	enum class AudioFrameType : int8_t {
		Unset = 0,///< Unset.
		Encoded = 1,///< Encoded.
		RawPCM = 2,///< Raw PCM.
		Skip = 3///< Skip.
	};

	/// Represents a single frame of audio data. \brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		EncodedFrameData encodedFrameData{};///< To be filled if it's already encoded.
		Snowflake guildMemberId{};///< The Id of the GuildMember from which it was sent.
		RawFrameData rawFrameData{};///< To be filled if it's raw audio data.
		void clearData();
	};

	/// A song from the various platforms. \brief A song from the various platforms.
	class DiscordCoreAPI_Dll Song : public DataParser<Song> {
		friend class DiscordCoreInternal::SoundCloudRequestBuilder;
		friend class DiscordCoreInternal::YouTubeRequestBuilder;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend SongAPI;

		std::vector<DownloadUrl> finalDownloadUrls{};
		SongType type{ SongType::SoundCloud };///< The type of song.
		std::string secondDownloadUrl{};
		std::string firstDownloadUrl{};
		std::string html5PlayerFile{};
		std::string addedByUserName{};///< The User name of the individual who added this Song to the playlist.
		std::string thumbnailUrl{};///< The Url of the thumbnail image of this Song.
		int32_t contentLength{ 0 };
		std::string html5Player{};
		std::string description{};///< A description of the Song.
		Snowflake addedByUserId{};///< The User id of the individual who added this Song to the playlist.
		std::string songTitle{};///< The title of the Song.
		std::string duration{};///< The duration of the Song.
		std::string viewUrl{};///< The url for listening to this Song through a browser.
		std::string songId{};

		Song() = default;

		Song& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		Song(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~Song() = default;

	  protected:
		std::string trackAuthorization{};
		bool doWeGetSaved{ false };
		YouTubeFormat format{};

		void parseObject(const nlohmann::json& jsonObjectData, Song* pDataStructure) {
			if (jsonObjectData.contains("lengthText") && !jsonObjectData["lengthText"].is_null()) {
				pDataStructure->duration = jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
			}

			if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData["detailedMetadataSnippets"].is_null()) {
				for (auto& value: jsonObjectData["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
					std::string newString = value["text"].get<std::string>();
					if (newString.size() > 256) {
						newString = newString.substr(0, 256);
					}
					pDataStructure->description = utf8MakeValid(newString);
				}
			}

			if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
				pDataStructure->thumbnailUrl = jsonObjectData["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
			}

			if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
				if (jsonObjectData["title"].contains("runs")) {
					std::string newString = jsonObjectData["title"]["runs"][0]["text"].get<std::string>();
					if (newString.size() > 256) {
						newString = newString.substr(0, 256);
					}
					pDataStructure->songTitle = utf8MakeValid(newString);
					;
				} else if (jsonObjectData["title"].contains("simpleText")) {
					std::string newString = jsonObjectData["title"]["simpleText"].get<std::string>();
					if (newString.size() > 256) {
						newString = newString.substr(0, 256);
					}
					pDataStructure->songTitle = utf8MakeValid(newString);
				}
			}
			if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
				pDataStructure->songId = jsonObjectData["videoId"].get<std::string>();
			}

			if (jsonObjectData.contains("track_authorization") && !jsonObjectData["track_authorization"].is_null()) {
				pDataStructure->trackAuthorization = jsonObjectData["track_authorization"].get<std::string>();
			}

			if (jsonObjectData.contains("media") && !jsonObjectData["media"].is_null()) {
				bool isItFound{ false };
				for (auto& value: jsonObjectData["media"]["transcodings"]) {
					if (value["preset"] == "opus_0_0") {
						isItFound = true;
						pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
						pDataStructure->songId = value["url"].get<std::string>();
						pDataStructure->doWeGetSaved = true;
					}
				}
				bool isItFound2{ false };
				if (!isItFound) {
					for (auto& value: jsonObjectData["media"]["transcodings"]) {
						if (value["preset"] == "mp3_0_0") {
							pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
							pDataStructure->songId = value["url"].get<std::string>();
							isItFound2 = true;
						}
					}
				}
				if (!isItFound2 && !isItFound) {
					for (auto& value: jsonObjectData["media"]["transcodings"]) {
						pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
						pDataStructure->songId = value["url"].get<std::string>();
					}
				}
			}

			if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null() && !jsonObjectData["title"].is_object()) {
				std::string newString = jsonObjectData["title"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->songTitle = utf8MakeValid(newString);
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				std::string newString = jsonObjectData["description"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->description = utf8MakeValid(newString);
				pDataStructure->description += "...";
			}

			if (jsonObjectData.contains("artwork_url") && !jsonObjectData["artwork_url"].is_null()) {
				pDataStructure->thumbnailUrl = jsonObjectData["artwork_url"].get<std::string>();
			} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				if (jsonObjectData["user"].contains("avatar_url") && !jsonObjectData["user"]["avatar_url"].is_null()) {
					pDataStructure->thumbnailUrl = jsonObjectData["user"]["avatar_url"].get<std::string>();
				}
			}

			if (jsonObjectData.contains("duration") && !jsonObjectData["duration"].is_null()) {
				pDataStructure->duration = TimeStamp::convertMsToDurationString(jsonObjectData["duration"].get<int32_t>());
			}

			if (jsonObjectData.contains("permalink_url") && !jsonObjectData["permalink_url"].is_null()) {
				pDataStructure->viewUrl = jsonObjectData["permalink_url"].get<std::string>();
			}
		}
	};

	/// Song completion event data. \brief Song completion event data.
	struct DiscordCoreAPI_Dll SongCompletionEventData {
		GuildMemberData guildMember{};///< The sending GuildMember.
		bool wasItAFail{ false };///< Is this a replay? (Did a track recently fail to play?)
		Song previousSong{};///< The previously played Song.
		GuildData guild{};///< The sending Guild.
	};

	/// Playlist of songs and other variables. \brief Playlist of songs and other variables.
	struct DiscordCoreAPI_Dll Playlist {
		bool isLoopSongEnabled{ false };///< Is looping of Songs currently enabled?
		bool isLoopAllEnabled{ false };///< Is looping of the entire Playlist currently enabled?
		std::vector<Song> songQueue{};///< The list of Songs that are stored to be played.
		Song currentSong{};///< The current Song that is playing.
	};


	/// Base arguments for the command classes. \brief Base arguments for the command classes.
	struct DiscordCoreAPI_Dll BaseFunctionArguments {
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the instance of DiscordCoreClient.
		InputEventData eventData{};///< InputEventData representing the input event that triggered the command.
		CommandData commandData{};///< The input command's data.

		BaseFunctionArguments() = default;

		BaseFunctionArguments(CommandData commandData, DiscordCoreClient* discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->eventData = commandData.eventData;
			this->commandData = commandData;
		}

		virtual ~BaseFunctionArguments() = default;
	};

	*/

	InputEventData& InputEventData::operator=(const InputEventData& other) {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(const InputEventData& other) {
		*this = other;
	}

	InputEventData& InputEventData::operator=(InputEventData& other) {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData& other) {
		*this = other;
	}

	InputEventData& InputEventData::operator=(InteractionData& other) {
		*this->interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(InteractionData& interactionData) {
		*this = interactionData;
	}

	/// Returns the userName of the last User to trigger this input-event. \brief Returns the userName of the last User to trigger this input-event.
	/// \returns std::string A string containing the User name.
	std::string InputEventData::getUserName() {
		return this->interactionData->user.userName;
	}

	/// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
	/// \returns std::string A string containing the avatar Url.
	std::string InputEventData::getAvatarUrl() {
		return this->interactionData->user.avatar;
	}

	/// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
	/// \returns std::vector A vector containing the EmbedData.
	std::vector<EmbedData> InputEventData::getEmbeds() {
		return this->interactionData->message.embeds;
	}

	/// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
	/// \returns std::vector A vector containing ActionRowData.
	std::vector<ActionRowData> InputEventData::getComponents() {
		return this->interactionData->message.components;
	}

	/// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
	/// \returns uint64_t An uint64_t containing the author's id.
	uint64_t InputEventData::getAuthorId() {
		return this->interactionData->user.id;
	}

	/// Returns the Interaction id, if appplicable, of this input-event. \brief Returns the Interaction id, if appplicable, of this input-event.
	/// \returns uint64_t An uint64_t containing the Interaction id.
	uint64_t InputEventData::getInteractionId() {
		return this->interactionData->id;
	}

	/// Returns the application id. \brief Returns the application id.
	/// \returns uint64_t An uint64_t containing the application id.
	uint64_t InputEventData::getApplicationId() {
		return this->interactionData->applicationId;
	}

	/// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
	/// \returns uint64_t An uint64_t containing the Channel id.
	uint64_t InputEventData::getChannelId() {
		return this->interactionData->channelId;
	}

	/// Returns the Interaction token, if applicable, of this input-event. \brief Returns the Interaction token, if applicable, of this input-event.
	/// \returns std::string A string containing the Interaction token.
	std::string InputEventData::getInteractionToken() {
		return this->interactionData->token;
	}

	/// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
	/// \returns uint64_t An uint64_t containing the Guild id.
	uint64_t InputEventData::getGuildId() {
		return this->interactionData->guildId;
	}

	/// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
	/// \returns uint64_t An uint64_t containing the Message id.
	uint64_t InputEventData::getMessageId() {
		return this->interactionData->message.id;
	}

	/// Returns the Interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
	/// \returns InteractionData An InteractionData structure.
	InteractionData InputEventData::getInteractionData() {
		return *this->interactionData;
	}

	/// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
	/// \returns MessageData A MessageData structure.
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

	/// Adds a button to the response Message. \brief Adds a button to the response Message.
	/// \param disabled Whether the button is active or not.
	/// \param customIdNew A custom id to give for identifying the button.
	/// \param buttonLabel A visible label for the button.
	/// \param buttonStyle The style of the button.
	/// \param emojiName An emoji name, if desired.
	/// \param emojiId An emoji id, if desired.
	/// \param url A url, if applicable.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
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
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
	/// \param disabled Whether the select-menu is active or not.
	/// \param customIdNew A custom id to give for identifying the select-menu.
	/// \param options A vector of select-menu-options to offer.
	/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
	/// \param maxValues Maximum number of selections that are possible.
	/// \param minValues Minimum required number of selections that are required.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
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
				this->components[this->components.size() - 1].components.push_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
	/// \param topTitleNew A title for the modal.
	/// \param topCustomIdNew A custom id to give for the modal.
	/// \param titleNew A title for the modal's individual input.
	/// \param customIdNew A custom id to give for the modal's individual input.
	/// \param required Is it a required response?
	/// \param minLength Minimum length.
	/// \param maxLength Maximum length.
	/// \param inputStyle The input style.
	/// \param label A label for the modal.
	/// \param placeholder A placeholder for the modal.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
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
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
	/// \param theFile The file to be added.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addFile(File theFile) {
		this->files.push_back(theFile);
		return *this;
	}

	/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
	/// \param dataPackage An AllowedMentionsData structure.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	/// For setting the type of response to make. \brief For setting the type of response to make.
	/// \param typeNew An InputEventResponseType.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setResponseType(InputEventResponseType typeNew) {
		this->type = typeNew;
		return *this;
	}

	/// For setting the components in a response. \brief For setting the components in a response.
	/// \param dataPackage An ActionRowData structure.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addComponentRow(ActionRowData dataPackage) {
		this->components.push_back(dataPackage);
		return *this;
	}

	/// For setting the embeds in a response. \brief For setting the embeds in a response.
	/// \param dataPackage An EmbedData structure.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.push_back(dataPackage);
		return *this;
	}

	/// For setting the Message content in a response. \brief For setting the Message content in a response.
	/// \param dataPackage A string, containing the content.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	/// For setting the tts status of a response. \brief For setting the tts status of a response.
	/// \param enabledTTs A bool.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	/// For setting the choices of an autocomplete response. \brief For setting the choices of an autocomplete response.
	/// \param theValue An nlohmann::json value that is either a float, int32_t or a string.
	/// \param theName A string for the name of the choice.
	/// \param theNameLocalizations A std::unordered_map<std::string, std::string> for the name localizations.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(nlohmann::json theValue, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.value = theValue;
		choiceData.name = theName;
		this->choices.push_back(choiceData);
		return *this;
	}

	/// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
	/// \param targetUserIdNew A string, containing the target User's id.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const uint64_t& targetUserIdNew) {
		this->targetUserId = targetUserIdNew;
		return *this;
	}

	/// Adds a button to the response Message. \brief Adds a button to the response Message.
	/// \param disabled Whether the button is active or not.
	/// \param customIdNew A custom id to give for identifying the button.
	/// \param buttonLabel A visible label for the button.
	/// \param buttonStyle The style of the button.
	/// \param emojiName An emoji name, if desired.
	/// \param emojiId An emoji id, if desired.
	/// \param url A url, if applicable.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
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
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
	/// \param disabled Whether the select-menu is active or not.
	/// \param customIdNew A custom id to give for identifying the select-menu.
	/// \param options A vector of select-menu-options to offer.
	/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
	/// \param maxValues Maximum number of selections that are possible.
	/// \param minValues Minimum required number of selections that are required.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder,
		int32_t maxValues, int32_t minValues) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
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
				this->components[this->components.size() - 1].components.push_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
	/// \param topTitleNew A title for the modal.
	/// \param topCustomIdNew A custom id to give for the modal.
	/// \param titleNew A title for the modal's individual input.
	/// \param customIdNew A custom id to give for the modal's individual input.
	/// \param required Is it a required response?
	/// \param minLength Minimum length.
	/// \param maxLength Maximum length.
	/// \param inputStyle The input style.
	/// \param label A label for the modal.
	/// \param placeholder A placeholder for the modal.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
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
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
	/// \param theFile The file to be added.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addFile(File theFile) {
		this->files.push_back(theFile);
		return *this;
	}

	/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
	/// \param dataPackage An AllowedMentionsData structure.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	/// For setting the components in a response. \brief For setting the components in a response.
	/// \param dataPackage An ActionRowData structure.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addComponentRow(ActionRowData dataPackage) {
		this->components.push_back(dataPackage);
		return *this;
	}

	/// For setting the embeds in a response. \brief For setting the embeds in a response.
	/// \param dataPackage An EmbedData structure.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.push_back(dataPackage);
		return *this;
	}

	/// For setting the Message content in a response. \brief For setting the Message content in a response.
	/// \param dataPackage A string, containing the content.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	/// For setting the tts status of a response. \brief For setting the tts status of a response.
	/// \param enabledTTs A bool.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
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
