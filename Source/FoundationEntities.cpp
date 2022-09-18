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

namespace DiscordCoreInternal {

	UpdatePresenceData::operator DiscordCoreAPI::JsonSerializer() {
		DiscordCoreAPI::JsonSerializer theData{};
		theData["op"] = static_cast<uint8_t>(3);
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["activities"] = DiscordCoreAPI::JsonParseEvent::Array_Start;
		for (auto& value: this->activities) {
			theData[""] = DiscordCoreAPI::JsonParseEvent::Object_Start;
			if (value.url != "") {
				theData["url"] = std::string{ value.url };
			}
			theData["name"] = std::string{ value.name };
			theData["type"] = static_cast<uint8_t>(value.type);
			theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		}
		theData[""] = DiscordCoreAPI::JsonParseEvent::Array_End;
		theData["status"] = this->status;
		if (this->since != 0) {
			theData["since"] = this->since;
		}
		theData["afl"] = this->afk;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;

	}

	WebSocketResumeData::operator DiscordCoreAPI::JsonSerializer() {
		DiscordCoreAPI::JsonSerializer theData{};
		theData["op"] = static_cast<uint8_t>(6);
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["seq"] = this->lastNumberReceived;
		theData["session_id"] = this->sessionId;
		theData["token"] = this->botToken;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	WebSocketIdentifyData::operator DiscordCoreAPI::JsonSerializer() {
		DiscordCoreAPI::JsonSerializer theData{};
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["compress"] = false;
		theData["intents"] = static_cast<uint32_t>(this->intents);
		theData["large_threshold"] = static_cast<uint32_t>(250);
		theData["presence"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["activities"] = DiscordCoreAPI::JsonParseEvent::Array_Start;

		for (auto& value: this->presence.activities) {
			theData[""] = DiscordCoreAPI::JsonParseEvent::Object_Start;
			if (value.url != "") {
				theData["url"] = std::string{ value.url };
			}
			theData["name"] = std::string{ value.name };
			theData["type"] = static_cast<uint8_t>(value.type);
			theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		}
		theData[""] = DiscordCoreAPI::JsonParseEvent::Array_End;
		theData["afk"] = this->presence.afk;
		if (this->presence.since == 0) {
			theData["since"] = DiscordCoreAPI::JsonParseEvent::Null_Value;
		} else {
			theData["since"] = this->presence.since;
		}

		theData["status"] = this->presence.status;

		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		theData["properties"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["browser"] = "DiscordCoreAPI";
		theData["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theData["os"] = "Windows";
#else
		theData["os"] = "Linux";
#endif
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		theData["shard"] = DiscordCoreAPI::JsonParseEvent::Array_Start;
		theData[""] = static_cast<uint8_t>(this->currentShard);
		theData[""] = static_cast<uint8_t>(this->numberOfShards);
		theData[""] = DiscordCoreAPI::JsonParseEvent::Array_End;
		theData["token"] = this->botToken;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		theData["op"] = static_cast<uint8_t>(2);
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	VoiceSocketProtocolPayloadData::operator DiscordCoreAPI::JsonSerializer() {
		DiscordCoreAPI::JsonSerializer theData{};
		theData["op"] = static_cast<uint8_t>(1);
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["protocol"] = "udp";
		theData["data"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["port"] = this->voicePort;
		theData["mode"] = this->voiceEncryptionMode;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	VoiceIdentifyData::operator DiscordCoreAPI::JsonSerializer() {
		DiscordCoreAPI::JsonSerializer theData{};
		theData["op"] = static_cast<uint8_t>(0);
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["session_id"] = this->connectionData.sessionId;
		theData["token"] = this->connectionData.token;
		theData["server_id"] = this->connectInitData.guildId;
		theData["user_id"] = this->connectInitData.userId;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	SendSpeakingData::operator DiscordCoreAPI::JsonSerializer() {
		DiscordCoreAPI::JsonSerializer theData{};
		theData["op"] = static_cast<uint8_t>(5);
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		theData["speaking"] = static_cast<uint8_t>(this->type);
		theData["delay"] = this->delay;
		theData["ssrc"] = this->ssrc;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(const HttpsWorkloadData& other) noexcept {
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

	HttpsWorkloadData::HttpsWorkloadData(const HttpsWorkloadData& other) noexcept {
		*this = other;
	}

	HttpsWorkloadData::HttpsWorkloadData(DiscordCoreInternal::HttpsWorkloadType theType) noexcept {
		if (!HttpsWorkloadData::workloadIdsExternal.contains(theType)) {
			std::unique_ptr<std::atomic_int64_t> theInt{ std::make_unique<std::atomic_int64_t>() };
			std::unique_ptr<std::atomic_int64_t> theInt02{ std::make_unique<std::atomic_int64_t>() };
			HttpsWorkloadData::workloadIdsExternal[theType] = std::move(theInt);
			HttpsWorkloadData::workloadIdsInternal[theType] = std::move(theInt02);
		}
		this->thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(theType));
		this->workloadType = theType;
	}

	int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) noexcept {
		int64_t theValue{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load() };
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(theValue + 1);
		return theValue;
	}

	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};
}

namespace DiscordCoreAPI {

	std::string DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
		TimeStamp<std::chrono::milliseconds> timeStamp{ (static_cast<uint64_t>(this->id) >> 22) + 1420070400000, timeFormat };
		return timeStamp;
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

	AttachmentData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["content_type"] = this->contentType;
		theData["description"] = this->description;
		theData["ephemeral"] = this->ephemeral;
		theData["file_name"] = this->filename;
		theData["proxy_url"] = this->proxyUrl;
		theData["height"] = this->height;
		theData["width"] = this->width;
		theData["size"] = this->size;
		theData["url"] = this->url;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	EmbedFieldData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["inline"] = this->Inline;
		theData["value"] = this->value;
		theData["name"] = this->name;
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
	}

	EmbedData::operator JsonSerializer() {
		JsonSerializer theData{};
		JsonSerializer fields{};
		fields["fields"] = JsonParseEvent::Array_Start;
		for (auto& value2: this->fields) {
			fields[""] = JsonParseEvent::Object_Start;
			fields["inline"] = value2.Inline;
			fields["value"] = value2.value;
			fields["name"] = value2.name;
			fields[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		}
		fields[""] = JsonParseEvent::Array_End;
		std::string realColorVal = std::to_string(this->hexColorValue.getIntColorValue());
		theData["footer"]["proxy_icon_url"] = this->footer.proxyIconUrl;
		theData["author"]["proxy_icon_url"] = this->author.proxyIconUrl;
		theData["thumbnail"]["proxy_url"] = this->thumbnail.proxyUrl;
		theData["thumbnail"]["height"] = this->thumbnail.height;
		theData["thumbnail"]["width"] = this->thumbnail.width;
		theData["image"]["proxy_url"] = this->image.proxyUrl;
		theData["author"]["icon_url"] = this->author.iconUrl;
		theData["footer"]["icon_url"] = this->footer.iconUrl;
		theData["video"]["proxy_url"] = this->video.proxyUrl;
		theData["provider"]["name"] = this->provider.name;
		theData["thumbnail"]["url"] = this->thumbnail.url;
		theData["provider"]["url"] = this->provider.url;
		theData["video"]["height"] = this->video.height;
		theData["image"]["height"] = this->image.height;
		theData["author"]["name"] = this->author.name;
		theData["image"]["width"] = this->image.width;
		theData["footer"]["text"] = this->footer.text;
		theData["video"]["width"] = this->video.width;
		theData["author"]["url"] = this->author.url;
		theData["description"] = this->description;
		theData["image"]["url"] = this->image.url;
		theData["video"]["url"] = this->video.url;
		theData["timestamp"] = this->timestamp;
		theData["title"] = this->title;
		theData["color"] = realColorVal;
		theData["type"] = this->type;
		theData["url"] = this->url;
		theData["fields"] = fields;
		fields[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		return theData;
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

	MessageReferenceData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["fail_if_not_exists"] = this->failIfNotExists;
		theData["message_id"] = this->messageId;
		theData["channel_id"] = this->channelId;
		theData["guild_id"] = this->guildId;
		return theData;
	}

	GuildApplicationCommandPermissionsDataVector::operator std::vector<GuildApplicationCommandPermissionsData>() {
		return this->theGuildApplicationCommandPermissionsDatas;
	}

	BanDataVector::operator std::vector<BanData>() {
		return this->theBanDatas;
	}

	UpdateVoiceStateData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["d"] = DiscordCoreAPI::JsonParseEvent::Object_Start;
		if (this->channelId == 0) {
			theData["channel_id"] = nullptr;
		} else {
			theData["channel_id"] = std::to_string(this->channelId);
		}
		theData["self_deaf"] = this->selfDeaf;
		theData["self_mute"] = this->selfMute;
		theData["guild_id"] = std::to_string(this->guildId);
		theData[""] = DiscordCoreAPI::JsonParseEvent::Object_End;
		theData["op"] = static_cast<uint8_t>(4);
		return theData;
	}

	GuildDataVector::operator std::vector<GuildData>() {
		return this->theGuildDatas;
	}

	GuildScheduledEventUserDataVector::operator std::vector<GuildScheduledEventUserData>() {
		return this->theGuildScheduledEventUserDatas;
	}

	GuildScheduledEventDataVector::operator std::vector<GuildScheduledEventData>() {
		return this->theGuildScheduledEventDatas;
	}

	InviteDataVector::operator std::vector<InviteData>() {
		return this->theInviteDatas;
	}

	GuildTemplateDataVector::operator std::vector<GuildTemplateData>() {
		return this->theGuildTemplateDatas;
	}

	WebHookDataVector::operator std::vector<WebHookData>() {
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

	ApplicationCommandOptionChoiceData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["name"] = this->name;
		theData["name_localizations"] = this->nameLocalizations;
		switch (this->type) {
			case JsonType::Integer: {
				theData["value"] = this->valueInt;
				break;
			}
			case JsonType::Float: {
				theData["value"] = this->valueFloat;
				break;
			}
			case JsonType::Boolean: {
				theData["value"] = this->valueBool;
				break;
			}
			case JsonType::String: {
				theData["value"] = this->valueStringReal;
				break;
			}
		}
		return theData;
	}

	ApplicationCommandOptionData::operator JsonSerializer() {
		JsonSerializer theData{};
		
		if (this->type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
			for (auto& value: this->channelTypes) {
				auto theNewValue = static_cast<uint8_t>(value);
				theData["channel_types"] = theNewValue;
			}
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
		theData["type"] = static_cast<uint8_t>(this->type);
		theData["choices"] = JsonParseEvent::Array_Start;
		if (this->choices.size() > 0) {
			
			for (auto& value: this->choices) {
				JsonSerializer theChoice{};
				theChoice = value.operator DiscordCoreAPI::JsonSerializer();
				theData[""] = theChoice;
			}
		}
		else {
			theData["autocomplete"] = this->autocomplete;
		}
		theData[""] = JsonParseEvent::Array_End;
		if (this->options.size() > 0) {
			theData["options"] = JsonParseEvent::Array_Start;
			for (auto& value: this->options) {
				JsonSerializer theData = value;
				theData[""] = theData;
			}
		}
		theData[""] = JsonParseEvent::Array_End;
		theData[""] = JsonParseEvent::Object_End;
		return theData;
	}

	ThreadMemberDataVector::operator std::vector<ThreadMemberData>() {
		return this->theThreadMemberDatas;
	}
	YouTubeFormatVector::operator std::vector<YouTubeFormat>() {
		return this->theFormats;
	}

	void AudioFrameData::clearData() noexcept {
		this->type = AudioFrameType::Unset;
		this->guildMemberId = 0;
		this->sampleCount = -1;
		this->data.clear();
	}

	AllowedMentionsData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["replied_user"] = this->repliedUser;
		theData["parse"] = this->parse;
		theData["roles"] = this->roles;
		theData["users"] = this->users;
		return theData;
	}

	ActionRowData::operator JsonSerializer() {
		JsonSerializer theData{};
		theData["components"] = JsonParseEvent::Array_Start;
		for (auto& valueNew: this->components) {
			if (valueNew.type == ComponentType::Button) {
				theData[""] = JsonParseEvent::Object_Start;
				theData["emoji"] = JsonParseEvent::Object_Start;
				theData["animated"] = valueNew.emoji.animated;
				std::string theString = valueNew.emoji.name;
				theData["name"] = static_cast<std::string>(theString);
				if (valueNew.emoji.id != 0) {
					theData["id"] = valueNew.emoji.id;
				}
				theData[""] = JsonParseEvent::Object_End;
				theData["custom_id"] = valueNew.customId;
				theData["disabled"] = valueNew.disabled;
				theData["label"] = valueNew.label;
				theData["style"] = valueNew.style;
				theData["type"] = static_cast<uint8_t>(valueNew.type);
				theData["url"] = valueNew.url;
				theData[""] = JsonParseEvent::Object_End;
			} else if (valueNew.type == ComponentType::SelectMenu) {
				theData[""] = JsonParseEvent::Object_Start;
				theData["options"] = JsonParseEvent::Array_Start;
				for (auto& value01: valueNew.options) {
					theData["emoji"] = JsonParseEvent::Object_Start;
					if (value01.emoji.name != "") {
						theData["name"] = static_cast<std::string>(value01.emoji.name);
						theData["animated"] = value01.emoji.animated;
					}
					if (value01.emoji.id != 0) {
						theData["id"] = value01.emoji.id;
					}
					theData[""] = JsonParseEvent::Object_End;
					theData["description"] = value01.description;
					theData["default"] = value01._default;
					theData["label"] = value01.label;
					theData["value"] = value01.value;
					theData[""] = JsonParseEvent::Object_End;
				};
				theData[""] = JsonParseEvent::Array_End;
				theData["placeholder"] = valueNew.placeholder;
				theData["max_values"] = valueNew.maxValues;
				theData["min_values"] = valueNew.minValues;
				theData["custom_id"] = valueNew.customId;
				theData["disabled"] = valueNew.disabled;
				theData["type"] = static_cast<uint8_t>(valueNew.type);
				theData[""] = JsonParseEvent::Object_End;

			} else if (valueNew.type == ComponentType::TextInput) {
				theData["placeholder"] = valueNew.placeholder;
				theData["min_length"] = valueNew.minLength;
				theData["max_length"] = valueNew.maxLength;
				theData["custom_id"] = valueNew.customId;
				theData["required"] = valueNew.required;
				theData["style"] = valueNew.style;
				theData["label"] = valueNew.label;
				theData["value"] = valueNew.value;
				theData["type"] = static_cast<uint8_t>(valueNew.type);
			}
		}
		theData["type"] = 1;
		theData[""] = JsonParseEvent::Object_End;
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

	StickerPackDataVector::operator std::vector<StickerPackData>() {
		return this->theStickerPackDatas;
	}

	ConnectionDataVector::operator std::vector<ConnectionData>() {
		return this->theConnectionDatas;
	}

	VoiceRegionDataVector::operator std::vector<VoiceRegionData>() {
		return this->theVoiceRegionDatas;
	}

	IntegrationDataVector::operator std::vector<IntegrationData>() {
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

	std::string InputEventData::getUserName() {
		return this->interactionData->user.userName;
	}

	std::string InputEventData::getAvatarUrl() {
		if (this->interactionData->member.getUserData().getAvatarUrl() != "") {
			return this->interactionData->member.getUserData().getAvatarUrl();
		} else {
			return this->interactionData->user.getAvatarUrl();
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

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(simdjson::ondemand::object theValue, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name = theName;
		auto theResult = theValue["value"].get(choiceData.valueBool);
		
		if (theResult == simdjson::error_code::SUCCESS) {
			this->choices.emplace_back(choiceData);
			return *this;
		}
		theResult = theValue["value"].get(choiceData.valueFloat);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->choices.emplace_back(choiceData);
			return *this;
		}
		theResult = theValue["value"].get(choiceData.valueInt);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->choices.emplace_back(choiceData);
			return *this;
		}
		theResult = theValue["value"].get(choiceData.valueString);
		if (theResult == simdjson::error_code::SUCCESS) {
			choiceData.valueStringReal = choiceData.valueString.data();
			this->choices.emplace_back(choiceData);
			return *this;
		}
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

	InteractionResponseData::operator JsonSerializer() {
		JsonSerializer theData{};
		/*
		for (auto& value: this->data.attachments) {
			theData["data"]["attachments"].emplace_back(DiscordCoreAPI::AttachmentData{ value });
		}
		if (this->data.components.size() == 0) {
			theData["data"]["components"] = nlohmann::json::array();
		} else {
			for (auto& value: this->data.components) {
				theData["data"]["components"].emplace_back(value);
			}
		}
		theData["data"]["allowed_mentions"] = DiscordCoreAPI::AllowedMentionsData{ this->data.allowedMentions };
		if (this->data.choices.size() > 0) {
			nlohmann::json::array_t theArray{};
			for (auto& value: this->data.choices) {
				nlohmann::json theValue{};
				theValue["name"] = value.name;
				theValue["name_localizations"] = value.nameLocalizations;
				switch (value.type) {
					case JsonType::Boolean: {
						theValue["value"] = value.valueBool;
					}
					case JsonType::String: {
						theValue["value"] = value.valueStringReal;
					}
					case JsonType::Float: {
						theValue["value"] = value.valueFloat;
					}
					case JsonType::Integer: {
						theValue["value"] = value.valueInt;
					}
				}
				theArray.emplace_back(theValue);
			}
			theData["data"]["choices"] = theArray;
		}
		if (this->data.embeds.size() == 0) {
			theData["data"]["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: this->data.embeds) {
				theData["data"]["embeds"].emplace_back(DiscordCoreAPI::EmbedData{ value });
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
		theData["type"] = this->type;
		*/
		return theData;
	}

	void parseCommandDataOption(std::unordered_map<std::string, JsonValueReal>& theValues, ApplicationCommandInteractionDataOption& theData) {
		JsonValueReal theValue{};
		theValue.theType = theData.value.theType;
		theValue.theValue = theData.value.theValue;
		theValues.emplace(theData.name, theValue);
		for (auto& value: theData.options) {
			JsonValueReal theValueNew{};
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
				JsonValueReal{ .theValue = std::to_string(inputEventData.interactionData->data.messageInteractionData.targetId), .theType = ObjectType::String });
		} else if (inputEventData.interactionData->data.userInteractionData.targetId != 0) {
			this->optionsArgs.theValues.emplace("target_id",
				JsonValueReal{ .theValue = std::to_string(inputEventData.interactionData->data.userInteractionData.targetId), .theType = ObjectType::String });
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
