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
		int64_t theValue{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load() };
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(theValue + 1);
		return theValue;
	}

	std::map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
	std::map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};
}

namespace DiscordCoreAPI {

	std::string DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
		TimeStamp<std::chrono::milliseconds> timeStamp{ (static_cast<uint64_t>(this->id) >> 22) + 1420070400000, timeFormat };
		return timeStamp;
	}

	RoleTagsData& RoleTagsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	RoleTagsData::RoleTagsData(const nlohmann::json* jsonObjectData) {
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

	AttachmentData& AttachmentData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AttachmentData::AttachmentData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedFooterData& EmbedFooterData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedFooterData::EmbedFooterData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedImageData& EmbedImageData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedImageData::EmbedImageData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedThumbnailData& EmbedThumbnailData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedThumbnailData::EmbedThumbnailData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedVideoData& EmbedVideoData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedVideoData::EmbedVideoData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedProviderData& EmbedProviderData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedProviderData::EmbedProviderData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedAuthorData& EmbedAuthorData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedAuthorData::EmbedAuthorData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmbedFieldData::operator nlohmann::json() {
		nlohmann::json newValue{};
		newValue["inline"] = this->Inline;
		newValue["value"] = this->value;
		newValue["name"] = this->name;
		return newValue;
	}

	EmbedFieldData& EmbedFieldData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedFieldData::EmbedFieldData(const nlohmann::json* jsonObjectData) {
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

	EmbedData& EmbedData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmbedData::EmbedData(const nlohmann::json* jsonObjectData) {
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

	MessageReferenceData& MessageReferenceData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageReferenceData::MessageReferenceData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadMetadataData& ThreadMetadataData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMetadataData::ThreadMetadataData(nlohmann::json* other) {
		*this = other;
	}

	ThreadMemberData& ThreadMemberData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMemberData::ThreadMemberData(const nlohmann::json* other) {
		*this = other;
	}

	ThreadMemberDataVector::operator std::vector<ThreadMemberData>() {
		return this->theThreadMemberDatas;
	}

	ThreadMemberDataVector& ThreadMemberDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMemberDataVector::ThreadMemberDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActiveThreadsData& ActiveThreadsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActiveThreadsData::ActiveThreadsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ArchivedThreadsData& ArchivedThreadsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ArchivedThreadsData::ArchivedThreadsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void RoleData::insertRole(std::unique_ptr<RoleData> theRole) {
		Roles::insertRole(std::move(theRole));
	}

	void RoleData::parseObject(const nlohmann::json* jsonObjectData) {
		DiscordCoreAPI::parseObject(jsonObjectData, *this);
	}

	ActionMetaData& ActionMetaData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActionMetaData::ActionMetaData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TriggerMetaData& TriggerMetaData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TriggerMetaData::TriggerMetaData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActionData& ActionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActionData::ActionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AutoModerationRuleData& AutoModerationRuleData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AutoModerationRuleData::AutoModerationRuleData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandPermissionData& ApplicationCommandPermissionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandPermissionData::ApplicationCommandPermissionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildApplicationCommandPermissionsData& GuildApplicationCommandPermissionsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildApplicationCommandPermissionsData::GuildApplicationCommandPermissionsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildApplicationCommandPermissionsDataVector::operator std::vector<GuildApplicationCommandPermissionsData>() {
		return this->theGuildApplicationCommandPermissionsDatas;
	}

	GuildApplicationCommandPermissionsDataVector& GuildApplicationCommandPermissionsDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildApplicationCommandPermissionsDataVector::GuildApplicationCommandPermissionsDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmojiData& EmojiData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmojiData::EmojiData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	EmojiDataVector& EmojiDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	EmojiDataVector::EmojiDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ReactionData& ReactionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ReactionData::ReactionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceRegionData& VoiceRegionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	VoiceRegionData::VoiceRegionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	VoiceRegionDataVector& VoiceRegionDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	VoiceRegionDataVector::VoiceRegionDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageActivityData& MessageActivityData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageActivityData::MessageActivityData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	BanData& BanData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	BanData::BanData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	BanDataVector::operator std::vector<BanData>() {
		return this->theBanDatas;
	}

	BanDataVector& BanDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	BanDataVector::BanDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TeamMembersObjectData& TeamMembersObjectData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TeamMembersObjectData::TeamMembersObjectData(const nlohmann::json* jsonObjectData) {
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

	TeamObjectData& TeamObjectData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TeamObjectData::TeamObjectData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InstallParamsData& InstallParamsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InstallParamsData::InstallParamsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationData& ApplicationData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationData::ApplicationData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AuthorizationInfoData& AuthorizationInfoData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuthorizationInfoData::AuthorizationInfoData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AccountData& AccountData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AccountData::AccountData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildWidgetData& GuildWidgetData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildWidgetData::GuildWidgetData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildWidgetImageData& GuildWidgetImageData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildWidgetImageData::GuildWidgetImageData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	IntegrationData& IntegrationData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	IntegrationData::IntegrationData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	IntegrationDataVector& IntegrationDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	IntegrationDataVector::IntegrationDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	OptionalAuditEntryInfoData& OptionalAuditEntryInfoData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	OptionalAuditEntryInfoData::OptionalAuditEntryInfoData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AuditLogChangeData& AuditLogChangeData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuditLogChangeData::AuditLogChangeData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildPruneCountData& GuildPruneCountData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildPruneCountData::GuildPruneCountData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AuditLogEntryData& AuditLogEntryData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuditLogEntryData::AuditLogEntryData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	PartyData& PartyData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	PartyData::PartyData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	AssetsData& AssetsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AssetsData::AssetsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	SecretsData& SecretsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	SecretsData::SecretsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TimestampData& TimestampData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TimestampData::TimestampData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActivityData& ActivityData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ActivityData::ActivityData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ClientStatusData& ClientStatusData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ClientStatusData::ClientStatusData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WelcomeScreenChannelData& WelcomeScreenChannelData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WelcomeScreenChannelData::WelcomeScreenChannelData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WelcomeScreenData& WelcomeScreenData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WelcomeScreenData::WelcomeScreenData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	PresenceUpdateData& PresenceUpdateData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	PresenceUpdateData::PresenceUpdateData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StageInstanceData& StageInstanceData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StageInstanceData::StageInstanceData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerData& StickerData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerData::StickerData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildPreviewData& GuildPreviewData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildPreviewData::GuildPreviewData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildDataVector::operator std::vector<GuildData>() {
		return this->theGuildDatas;
	}

	GuildDataVector& GuildDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildDataVector::GuildDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventMetadata& GuildScheduledEventMetadata::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventMetadata::GuildScheduledEventMetadata(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventData& GuildScheduledEventData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventData::GuildScheduledEventData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventUserData& GuildScheduledEventUserData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventUserData::GuildScheduledEventUserData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventUserDataVector::operator std::vector<GuildScheduledEventUserData>() {
		return this->theGuildScheduledEventUserDatas;
	}

	GuildScheduledEventUserDataVector& GuildScheduledEventUserDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventUserDataVector::GuildScheduledEventUserDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildScheduledEventDataVector::operator std::vector<GuildScheduledEventData>() {
		return this->theGuildScheduledEventDatas;
	}

	GuildScheduledEventDataVector& GuildScheduledEventDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildScheduledEventDataVector::GuildScheduledEventDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InviteData& InviteData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InviteData::InviteData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InviteDataVector::operator std::vector<InviteData>() {
		return this->theInviteDatas;
	}

	InviteDataVector& InviteDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InviteDataVector::InviteDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildTemplateData& GuildTemplateData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildTemplateData::GuildTemplateData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildTemplateDataVector::operator std::vector<GuildTemplateData>() {
		return this->theGuildTemplateDatas;
	}

	GuildTemplateDataVector& GuildTemplateDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildTemplateDataVector::GuildTemplateDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WebHookData& WebHookData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WebHookData::WebHookData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	WebHookDataVector::operator std::vector<WebHookData>() {
		return this->theWebHookDatas;
	}

	WebHookDataVector& WebHookDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	WebHookDataVector::WebHookDataVector(const nlohmann::json* jsonObjectData) {
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

	AuditLogData& AuditLogData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AuditLogData::AuditLogData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ReactionRemoveData& ReactionRemoveData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ReactionRemoveData::ReactionRemoveData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandOptionChoiceData::operator nlohmann::json() {
		nlohmann::json theData{};
		theData["name"] = this->name;
		theData["name_localizations"] = this->nameLocalizations;
		theData["value"] = this->value;
		return theData;
	}

	ApplicationCommandOptionChoiceData& ApplicationCommandOptionChoiceData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandOptionChoiceData::ApplicationCommandOptionChoiceData(const nlohmann::json* jsonObjectData) {
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

	ApplicationCommandOptionData& ApplicationCommandOptionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandOptionData::ApplicationCommandOptionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	TypingStartData& TypingStartData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	TypingStartData::TypingStartData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	YouTubeFormatVector::operator std::vector<YouTubeFormat>() {
		return this->theFormats;
	}

	YouTubeFormatVector& YouTubeFormatVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	YouTubeFormatVector::YouTubeFormatVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	UserCommandInteractionData& UserCommandInteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	UserCommandInteractionData::UserCommandInteractionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageCommandInteractionData& MessageCommandInteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageCommandInteractionData::MessageCommandInteractionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ComponentInteractionData& ComponentInteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ComponentInteractionData::ComponentInteractionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	void AudioFrameData::clearData() {
		this->type = AudioFrameType::Unset;
		this->guildMemberId = 0;
		this->sampleCount = -1;
		this->data.clear();
	}

	ModalInteractionData& ModalInteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ModalInteractionData::ModalInteractionData(const nlohmann::json* jsonObjectData) {
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

	AllowedMentionsData& AllowedMentionsData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	AllowedMentionsData::AllowedMentionsData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	SelectOptionData& SelectOptionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	SelectOptionData::SelectOptionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ActionRowData& ActionRowData::operator=(const nlohmann::json* jsonObjectData) {
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

	ActionRowData::ActionRowData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandData& ApplicationCommandData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandData::ApplicationCommandData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ChannelMentionData& ChannelMentionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ChannelMentionData::ChannelMentionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ChannelPinsUpdateEventData& ChannelPinsUpdateEventData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ChannelPinsUpdateEventData::ChannelPinsUpdateEventData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadListSyncData& ThreadListSyncData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadListSyncData::ThreadListSyncData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ThreadMembersUpdateData& ThreadMembersUpdateData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ThreadMembersUpdateData::ThreadMembersUpdateData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageInteractionData& MessageInteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageInteractionData::MessageInteractionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerItemData& StickerItemData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerItemData::StickerItemData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	MessageDataOld& MessageDataOld::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageDataOld::MessageDataOld(const nlohmann::json* jsonObjectData) {
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

	MessageData& MessageData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	MessageData::MessageData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerPackData& StickerPackData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerPackData::StickerPackData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	StickerPackDataVector::operator std::vector<StickerPackData>() {
		return this->theStickerPackDatas;
	}

	StickerPackDataVector& StickerPackDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	StickerPackDataVector::StickerPackDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ConnectionData& ConnectionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ConnectionData::ConnectionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ConnectionDataVector::operator std::vector<ConnectionData>() {
		return this->theConnectionDatas;
	}

	ConnectionDataVector& ConnectionDataVector::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ConnectionDataVector::ConnectionDataVector(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandInteractionDataOption& ApplicationCommandInteractionDataOption::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandInteractionDataOption::ApplicationCommandInteractionDataOption(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	ApplicationCommandInteractionData& ApplicationCommandInteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	ApplicationCommandInteractionData::ApplicationCommandInteractionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InteractionDataData& InteractionDataData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InteractionDataData::InteractionDataData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	InteractionData& InteractionData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	InteractionData::InteractionData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	SessionStartData& SessionStartData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	SessionStartData::SessionStartData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GatewayBotData& GatewayBotData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GatewayBotData::GatewayBotData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildEmojisUpdateEventData& GuildEmojisUpdateEventData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildEmojisUpdateEventData::GuildEmojisUpdateEventData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildStickersUpdateEventData& GuildStickersUpdateEventData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildStickersUpdateEventData::GuildStickersUpdateEventData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
	}

	GuildMembersChunkEventData& GuildMembersChunkEventData::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	GuildMembersChunkEventData::GuildMembersChunkEventData(const nlohmann::json* jsonObjectData) {
		*this = jsonObjectData;
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

	std::string InputEventData::getUserName() {
		return this->interactionData->user.userName;
	}

	std::string InputEventData::getAvatarUrl() {
		if (this->interactionData->member.userAvatar.getHashUrl(this->interactionData->user.id, 0) != "") {
			return this->interactionData->member.userAvatar.getHashUrl(this->interactionData->user.id, 0);
		} else {
			return this->interactionData->user.avatar.getHashUrl(this->interactionData->user.id, 0);
		}
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
		std::map<std::string, std::string> theNameLocalizations) {
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

	Song& Song::operator=(const nlohmann::json* jsonObjectData) {
		this->parseObject(jsonObjectData);
		return *this;
	}

	Song::Song(const nlohmann::json* jsonObjectData) {
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
