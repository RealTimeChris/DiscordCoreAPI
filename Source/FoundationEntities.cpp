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
#include <discordcoreapi/Utilities/Etf.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		WebSocketResumeData::operator DiscordCoreInternal::EtfSerializer() {
			EtfSerializer data{};
			data["seq"]		   = lastNumberReceived;
			data["session_id"] = sessionId;
			data["token"]	   = botToken;
			return data;
		}

		WebSocketIdentifyData::operator DiscordCoreInternal::EtfSerializer() {
			EtfSerializer serializer{};
			serializer["intents"]				= intents;
			serializer["large_threshold"]		= 250;
			serializer["presence"]				= presence.operator DiscordCoreInternal::EtfSerializer();
			serializer["properties"]["browser"] = "DiscordCoreAPI";
			serializer["properties"]["device"]	= "DiscordCoreAPI";
#ifdef _WIN32
			serializer["properties"]["os"] = "Windows";
#else
			serializer["properties"]["os"] = "Linux";
#endif
			serializer["shard"].emplaceBack(shard.at(0));
			serializer["shard"].emplaceBack(shard.at(1));
			serializer["token"] = botToken;
			return serializer;
		}

		HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadData&& other) noexcept {
			if (this != &other) {
				thisWorkerId.store(thisWorkerId.load(std::memory_order_acquire), std::memory_order_release);
				headersToInsert = std::move(other.headersToInsert);
				relativePath	= std::move(other.relativePath);
				callStack		= std::move(other.callStack);
				baseUrl			= std::move(other.baseUrl);
				content			= std::move(other.content);
				workloadClass	= other.workloadClass;
				workloadType	= other.workloadType;
				payloadType		= other.payloadType;
			}
			return *this;
		}

		HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadData&& other) noexcept {
			*this = std::move(other);
		}

		HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadType type) {
			if (!HttpsWorkloadData::workloadIdsExternal.contains(type)) {
				HttpsWorkloadData::workloadIdsExternal[type] = makeUnique<std::atomic_int64_t>();
				HttpsWorkloadData::workloadIdsInternal[type] = makeUnique<std::atomic_int64_t>();
			}
			thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(type), std::memory_order_release);
			workloadType = type;
			return *this;
		}

		HttpsWorkloadType HttpsWorkloadData::getWorkloadType() const {
			return workloadType;
		}

		HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadType type) {
			*this = type;
		}

		int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) {
			int64_t value{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load(std::memory_order_acquire) };
			HttpsWorkloadData::workloadIdsExternal[workloadType]->store(value + 1, std::memory_order_release);
			return value;
		}

		UnorderedMap<HttpsWorkloadType, UniquePtr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
		UnorderedMap<HttpsWorkloadType, UniquePtr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};
	}

	UpdatePresenceData::operator DiscordCoreInternal::EtfSerializer() {
		DiscordCoreInternal::EtfSerializer data{};
		for (auto& value: activities) {
			DiscordCoreInternal::EtfSerializer newData{};
			if (value.url != "") {
				newData["url"] = jsonifier::string_view{ value.url };
			}
			if (value.state != "") {
				newData["state"] = value.state;
			}
			newData["name"] = jsonifier::string_view{ value.name };
			newData["type"] = static_cast<uint32_t>(value.type);
			data["activities"].emplaceBack(newData);
		}
		data["status"] = statusReal;
		data["since"]  = since;
		data["afk"]	   = false;
		return data;
	}

	UpdateVoiceStateData::operator DiscordCoreInternal::EtfSerializer() {
		DiscordCoreInternal::EtfSerializer data{};
		if (channelId == 0) {
			data["channel_id"] = DiscordCoreInternal::JsonType::Null;
		} else {
			data["channel_id"] = channelId.operator jsonifier::string();
		}
		data["self_deaf"] = selfDeaf;
		data["self_mute"] = selfMute;
		data["guild_id"]  = guildId.operator jsonifier::string();
		return data;
	}

	UpdateVoiceStateDataDC::operator DiscordCoreInternal::EtfSerializer() {
		DiscordCoreInternal::EtfSerializer data{};
		data["channel_id"] = DiscordCoreInternal::JsonType::Null;
		data["self_deaf"]  = selfDeaf;
		data["self_mute"]  = selfMute;
		data["guild_id"]   = guildId.operator jsonifier::string();
		return data;
	}

	VoiceStateDataLight GuildMemberData::getVoiceStateData() {
		return GuildMembers::getVoiceStateData(*this);
	}

	UserCacheData GuildMemberData::getUserData() {
		if (user.id != 0) {
			return Users::getCachedUser(GetUserData{ .userId = user.id });
		} else {
			return {};
		}
	}

	VoiceStateDataLight GuildMemberCacheData::getVoiceStateData() {
		return GuildMembers::getVoiceStateData(*this);
	}

	UserCacheData GuildMemberCacheData::getUserData() {
		if (user.id != 0) {
			return Users::getCachedUser(GetUserData{ .userId = user.id });
		} else {
			return {};
		}
	}

	bool GuildCacheData::areWeConnected() {
		return getDiscordCoreClient()->getVoiceConnection(id).areWeConnected();
	}

	void GuildCacheData::disconnect() {
		UpdateVoiceStateData data{};
		data.channelId = 0;
		data.selfDeaf  = false;
		data.selfMute  = false;
		data.guildId   = id;
		DiscordCoreClient::getSongAPI(id).disconnect();
		DiscordCoreClient::getBotUser().updateVoiceStatus(data);
		DiscordCoreClient::getVoiceConnection(id).disconnect();
	}

	bool GuildData::areWeConnected() {
		return getDiscordCoreClient()->getVoiceConnection(id).areWeConnected();
	}

	void GuildData::disconnect() {
		DiscordCoreInternal::WebSocketMessageData<UpdateVoiceStateData> data{};
		data.excludedKeys.emplace("t");
		data.excludedKeys.emplace("s");
		data.d.channelId = 0;
		data.d.selfDeaf	 = false;
		data.d.selfMute	 = false;
		data.d.guildId	 = id;
		DiscordCoreClient::getSongAPI(id).disconnect();
		DiscordCoreClient::getBotUser().updateVoiceStatus(data.d);
		DiscordCoreClient::getVoiceConnection(id).disconnect();
	}

	bool ApplicationCommandOptionData::operator==(const ApplicationCommandOptionData& rhs) const {
		if (autocomplete != rhs.autocomplete) {
			return false;
		}
		if (channelTypes != rhs.channelTypes) {
			return false;
		}
		if (description != rhs.description) {
			return false;
		}
		if (descriptionLocalizations != rhs.descriptionLocalizations) {
			return false;
		}
		if (maxValue != rhs.maxValue) {
			return false;
		}
		if (minValue != rhs.minValue) {
			return false;
		}
		if (name != rhs.name) {
			return false;
		}
		if (nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (options.size() != rhs.options.size()) {
			return false;
		}
		if (required != rhs.required) {
			return false;
		}
		if (type != rhs.type) {
			return false;
		}
		if (choices.size() != rhs.choices.size()) {
			return false;
		}
		for (uint64_t x = 0; x < rhs.choices.size(); ++x) {
			if (choices.at(x) != rhs.choices.at(x)) {
				return false;
			}
		}
		return true;
	}

	EmbedData& EmbedData::setAuthor(jsonifier::string_view authorName, jsonifier::string_view authorAvatarUrl) {
		author.name	   = authorName;
		author.iconUrl = authorAvatarUrl;
		return *this;
	}

	EmbedData& EmbedData::setFooter(jsonifier::string_view footerText, jsonifier::string_view footerIconUrlText) {
		footer.text = footerText;
		footer.iconUrl = footerIconUrlText;
		return *this;
	}

	EmbedData& EmbedData::setTimeStamp(jsonifier::string_view timeStampNew) {
		timeStamp = timeStampNew;
		return *this;
	}

	EmbedData& EmbedData::addField(jsonifier::string_view name, jsonifier::string_view value, bool Inline) {
		EmbedFieldData field{};
		field.Inline = Inline;
		field.value	 = value;
		field.name	 = name;
		fields.emplace_back(field);
		return *this;
	}

	EmbedData& EmbedData::setDescription(jsonifier::string_view descriptionNew) {
		description = descriptionNew;
		return *this;
	}

	EmbedData& EmbedData::setColor(jsonifier::string_view hexColorValueNew) {
		hexColorValue = std::stoul(hexColorValueNew.data(), nullptr, 16);
		return *this;
	}

	EmbedData& EmbedData::setThumbnail(jsonifier::string_view thumbnailUrl) {
		thumbnail.url = thumbnailUrl;
		return *this;
	}

	EmbedData& EmbedData::setTitle(jsonifier::string_view titleNew) {
		title = titleNew;
		return *this;
	}

	EmbedData& EmbedData::setImage(jsonifier::string_view imageUrl) {
		image.url = imageUrl;
		return *this;
	}

	void EmbedImageData::generateExcludedKeys() {
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

	void EmbedAuthorData::generateExcludedKeys() {
		if (name == "") {
			excludedKeys.emplace("name");
		}
		if (url == "") {
			excludedKeys.emplace("url");
		}
		if (iconUrl == "") {
			excludedKeys.emplace("icon_url");
		}
		if (proxyIconUrl == "") {
			excludedKeys.emplace("proxy_icon_url");
		}
	}

	void EmbedData::generateExcludedKeys() {
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
		} else {
			author.generateExcludedKeys();
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

	void PartialEmojiData::generateExcludedKeys() {
		if (name == "") {
			excludedKeys.emplace("name");
		}
		if (id == 0) {
			excludedKeys.emplace("id");
		}
	}

	void EmojiData::generateExcludedKeys() {
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

	bool ApplicationCommandData::operator==(const ApplicationCommandData& rhs) const {
		if (description != rhs.description) {
			return false;
		}
		if (name != rhs.name) {
			return false;
		}
		if (type != rhs.type) {
			return false;
		}
		if (options.size() != rhs.options.size()) {
			return false;
		}
		for (uint64_t x = 0; x < options.size(); ++x) {
			if (options.at(x) != rhs.options.at(x)) {
				return false;
			}
		}
		return true;
	}

	bool ApplicationCommandOptionChoiceData::operator==(const ApplicationCommandOptionChoiceData& rhs) const {
		if (name != rhs.name) {
			return false;
		}
		if (nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (value != rhs.value) {
			return false;
		}
		return true;
	}

	InputEventData& InputEventData::operator=(InputEventData&& other) noexcept {
		if (this != &other) {
			interactionData.swap(other.interactionData);
			responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData&& other) noexcept {
		*this = std::move(other);
	}

	InputEventData& InputEventData::operator=(const InputEventData& other) {
		if (this != &other) {
			*interactionData = *other.interactionData;
			responseType	 = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(const InputEventData& other) {
		*this = other;
	}

	InputEventData& InputEventData::operator=(const InteractionData& other) {
		*interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(const InteractionData& interactionData) {
		*this = interactionData;
	}

	UserData InputEventData::getUserData() const {
		if (interactionData->member.user.id != 0) {
			return interactionData->member.user;
		} else {
			return interactionData->user;
		}
	}

	ChannelData InputEventData::getChannelData() const {
		return interactionData->channel;
	}

	GuildMemberData InputEventData::getGuildMemberData() const {
		interactionData->member.guildId = interactionData->guildId;
		return interactionData->member;
	}

	void InteractionCallbackData::generateExcludedKeys() {
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

	void ComponentData::generateExcludedKeys() {
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

	void ApplicationCommandOptionData::generateExcludedKeys() {
		for (auto& value: options) {
			value.generateExcludedKeys();
		}
		for (auto& value: choices) {
			value.generateExcludedKeys();
		}
		if (maxValue == std::numeric_limits<int64_t>::min()) {
			excludedKeys.emplace("max_value");
		}
		if (minValue == std::numeric_limits<int64_t>::max()) {
			excludedKeys.emplace("min_value");
		}
		if (nameLocalizations.size() == 0) {
			excludedKeys.emplace("name_localizations");
		}
		if (choices.size() == 0) {
			excludedKeys.emplace("choices");
		}
		if (options.size() == 0) {
			excludedKeys.emplace("options");
		}
		if (!autocomplete) {
			excludedKeys.emplace("autocomplete");
		}
		if (descriptionLocalizations.size() == 0) {
			excludedKeys.emplace("description_localizations");
		}
		if (channelTypes.size() == 0) {
			excludedKeys.emplace("channel_types");
		}
	}

	void ApplicationCommandOptionChoiceData::generateExcludedKeys() {
		if (nameLocalizations.size() == 0) {
			excludedKeys.emplace("name_localizations");
		}
	}

	void ApplicationCommandData::generateExcludedKeys() {
		if (descriptionLocalizations.size() == 0) {
			excludedKeys.emplace("description_localizations");
		}
		if (nameLocalizations.size() == 0) {
			excludedKeys.emplace("name_localizations");
		}
		if (id == 0) {
			excludedKeys.emplace("id");
		}
		if (version == "") {
			excludedKeys.emplace("version");
		}
		if (guildId == 0) {
			excludedKeys.emplace("guild_id");
		}
		for (auto& value: options) {
			value.generateExcludedKeys();
		}
		if (options.size() == 0) {
			excludedKeys.emplace("options");
		}
	}

	InteractionData InputEventData::getInteractionData() const {
		return *interactionData;
	}

	MessageData InputEventData::getMessageData() const {
		return interactionData->message;
	}

	RespondToInputEventData::operator InteractionCallbackData() const {
		InteractionCallbackData returnData{};
		returnData.allowedMentions = allowedMentions;
		returnData.components	   = components;
		returnData.customId		   = customId;
		returnData.choices		   = choices;
		returnData.content		   = content;
		returnData.embeds		   = embeds;
		returnData.files		   = files;
		returnData.flags		   = flags;
		returnData.title		   = title;
		returnData.tts			   = tts;
		return returnData;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InteractionData& dataPackage) {
		applicationId	 = dataPackage.applicationId;
		interactionToken = dataPackage.token;
		messageId		 = dataPackage.message.id;
		channelId		 = dataPackage.channelId;
		interactionId	 = dataPackage.id;
		eventType		 = dataPackage.type;
		return *this;
	};

	RespondToInputEventData::RespondToInputEventData(const InteractionData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InputEventData& dataPackage) {
		applicationId	 = dataPackage.getInteractionData().applicationId;
		interactionToken = dataPackage.getInteractionData().token;
		interactionId	 = dataPackage.getInteractionData().id;
		channelId		 = dataPackage.interactionData->channelId;
		messageId		 = dataPackage.getMessageData().id;
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(const InputEventData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, ButtonStyle buttonStyle,
		jsonifier::string_view emojiName, Snowflake emojiId, jsonifier::string_view url) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type		 = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label		 = buttonLabel;
				component.style		 = static_cast<uint64_t>(buttonStyle);
				component.customId	 = customIdNew;
				component.disabled	 = disabled;
				component.emoji.id	 = emojiId;
				component.url		 = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, jsonifier::string_view customIdNew, const jsonifier::vector<SelectOptionData>& options,
		jsonifier::string_view placeholder, uint64_t maxValues, uint64_t minValues, SelectMenuType typeNew, jsonifier::vector<ChannelType> channelTypes) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type		   = static_cast<ComponentType>(typeNew);
				componentData.channelTypes = channelTypes;
				componentData.placeholder  = placeholder;
				componentData.customId	   = customIdNew;
				componentData.maxValues	   = maxValues;
				componentData.minValues	   = minValues;
				componentData.disabled	   = disabled;
				componentData.options	   = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
		jsonifier::string_view customIdNew, bool required, uint64_t minLength, uint64_t maxLength, TextInputStyle inputStyle, jsonifier::string_view label,
		jsonifier::string_view placeholder) {
		title	 = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type		  = ComponentType::Text_Input;
				component.customId	  = customIdNew;
				component.style		  = static_cast<uint64_t>(inputStyle);
				component.title		  = titleNew;
				component.maxLength	  = maxLength;
				component.minLength	  = minLength;
				component.label		  = label;
				component.required	  = required;
				component.placeholder = placeholder;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addFile(const File& theFile) {
		files.emplace_back(theFile);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addAllowedMentions(const AllowedMentionsData& dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setResponseType(InputEventResponseType typeNew) {
		type = typeNew;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addComponentRow(const ActionRowData& dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addMessageEmbed(const EmbedData& dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addContent(jsonifier::string_view dataPackage) {
		content = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(DiscordCoreInternal::EtfSerializer value, jsonifier::string_view theName,
		UnorderedMap<jsonifier::string, jsonifier::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name				 = theName;
		choiceData.value			 = jsonifier::string{ value.operator jsonifier::string_base<uint8_t>() };
		choices.emplace_back(choiceData);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const Snowflake targetUserIdNew) {
		targetUserId = targetUserIdNew;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, ButtonStyle buttonStyle,
		jsonifier::string_view emojiName, Snowflake emojiId, jsonifier::string_view url) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type		 = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label		 = buttonLabel;
				component.style		 = static_cast<uint64_t>(buttonStyle);
				component.customId	 = customIdNew;
				component.disabled	 = disabled;
				component.emoji.id	 = emojiId;
				component.url		 = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, jsonifier::string_view customIdNew, jsonifier::vector<SelectOptionData> options,
		jsonifier::string_view placeholder, uint64_t maxValues, uint64_t minValues, SelectMenuType type, jsonifier::vector<ChannelType> channelTypes) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type		   = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder  = placeholder;
				componentData.customId	   = customIdNew;
				componentData.maxValues	   = maxValues;
				componentData.minValues	   = minValues;
				componentData.disabled	   = disabled;
				componentData.options	   = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
		jsonifier::string_view customIdNew, bool required, uint64_t minLength, uint64_t maxLength, TextInputStyle inputStyle, jsonifier::string_view label,
		jsonifier::string_view placeholder) {
		title	 = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type		  = ComponentType::Text_Input;
				component.customId	  = customIdNew;
				component.style		  = static_cast<uint64_t>(inputStyle);
				component.title		  = titleNew;
				component.maxLength	  = maxLength;
				component.minLength	  = minLength;
				component.label		  = label;
				component.required	  = required;
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

	MessageResponseBase& MessageResponseBase::addAllowedMentions(const AllowedMentionsData& dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addComponentRow(const ActionRowData& dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addMessageEmbed(const EmbedData& dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addContent(jsonifier::string_view dataPackage) {
		content = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	InteractionResponseData& InteractionResponseData::operator=(const RespondToInputEventData& other) {
		data.allowedMentions = other.allowedMentions;
		data.components		 = other.components;
		data.customId		 = other.customId;
		data.choices		 = other.choices;
		data.content		 = other.content;
		data.embeds			 = other.embeds;
		data.title			 = other.title;
		data.files			 = other.files;
		data.flags			 = other.flags;
		data.tts			 = other.tts;
		return *this;
	}

	InteractionResponseData::InteractionResponseData(const RespondToInputEventData& other) {
		*this = other;
	}

	void parseCommandDataOption(UnorderedMap<jsonifier::string, JsonStringValue>& values, ApplicationCommandInteractionDataOption& data) {
		JsonStringValue valueNew{};
		auto stringSize{ data.value.operator jsonifier::string_view().size() };
		if (stringSize > 1) {
			if (data.value.operator jsonifier::string_view().at(0) == '"' && data.value.operator jsonifier::string_view().at(stringSize - 1) == '"') {
				data.value = data.value.operator jsonifier::string().substr(1, data.value.operator jsonifier::string().size() - 2);
			}
		}
		valueNew.value	  = data.value;
		values[data.name] = valueNew;
		for (auto& value: data.options) {
			parseCommandDataOption(values, value);
		}
	}

	SerializerValue CommandData::getCommandArguments() const {
		return optionsArgs;
	}

	CommandData& CommandData::operator=(const CommandData& other) {
		subCommandGroupName = other.subCommandGroupName;
		subCommandName		= other.subCommandName;
		commandName			= other.commandName;
		optionsArgs			= other.optionsArgs;
		eventData			= other.eventData;
		return *this;
	}

	CommandData::CommandData(const CommandData& other) {
		*this = other;
	}

	CommandData::CommandData(const InputEventData& inputEventData) {
		if (inputEventData.interactionData->data.name != "") {
			commandName = inputEventData.interactionData->data.name;
		}
		if (inputEventData.interactionData->data.targetId != 0) {
			optionsArgs.values.emplace("target_id",
				JsonStringValue{ .type = DiscordCoreInternal::JsonType::String, .value = inputEventData.interactionData->data.targetId.operator jsonifier::string() });
		} else if (inputEventData.interactionData->data.targetId != 0) {
			optionsArgs.values.emplace("target_id",
				JsonStringValue{ .type = DiscordCoreInternal::JsonType::String, .value = inputEventData.interactionData->data.targetId.operator jsonifier::string() });
		}
		eventData = inputEventData;
		for (auto& value: eventData.interactionData->data.options) {
			JsonStringValue serializer{ .value = value.value.operator jsonifier::string() };
			optionsArgs.values[value.name] = serializer;
			parseCommandDataOption(optionsArgs.values, value);
		}
		for (auto& value: eventData.interactionData->data.options) {
			if (value.type == ApplicationCommandOptionType::Sub_Command) {
				subCommandName = value.name;
			}
			if (value.type == ApplicationCommandOptionType::Sub_Command_Group) {
				subCommandGroupName = value.name;
			}
		}
	}

	InteractionData CommandData::getInteractionData() const {
		return *eventData.interactionData;
	}

	GuildMemberData CommandData::getGuildMemberData() const {
		return eventData.getGuildMemberData();
	}

	ChannelData CommandData::getChannelData() const {
		return eventData.getChannelData();
	}

	MessageData CommandData::getMessageData() const {
		return eventData.getMessageData();
	}

	UserData CommandData::getUserData() const {
		return eventData.getUserData();
	}

	jsonifier::string CommandData::getCommandName() const {
		return commandName;
	}

	jsonifier::string CommandData::getSubCommandName() const {
		return subCommandName;
	}

	jsonifier::string CommandData::getSubCommandGroupName() const {
		return subCommandGroupName;
	}

	InputEventData CommandData::getInputEventData() const {
		return eventData;
	}

	BaseFunctionArguments::BaseFunctionArguments(const CommandData& commanddataNew) : CommandData{ commanddataNew } {};

	MoveThroughMessagePagesData moveThroughMessagePages(jsonifier::string_view userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const jsonifier::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		uint32_t newCurrentPageIndex = currentPageIndex;
		StopWatch<Milliseconds> stopWatch{ Milliseconds{ waitForMaxMs } };
		stopWatch.reset();
		auto createResponseData	 = makeUnique<CreateInteractionResponseData>(originalEvent);
		auto interactionResponse = makeUnique<RespondToInputEventData>(originalEvent);
		EmbedData embedData{};
		embedData.setColor("FEFEFE");
		embedData.setTitle("__**Permissions Issue:**__");
		embedData.setTimeStamp(getTimeAndDate());
		embedData.setDescription("Sorry, but that button can only be pressed by <@" + userID + ">!");
		createResponseData->addMessageEmbed(embedData);
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
		while (!stopWatch.hasTimeElapsed()) {
			std::this_thread::sleep_for(1ms);
			UniquePtr<ButtonCollector> button{ makeUnique<ButtonCollector>(originalEvent) };
			jsonifier::vector<ButtonResponseData> buttonIntData{ button->collectButtonData(false, waitForMaxMs, 1, *createResponseData, userID).get() };
			UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };
			if (buttonIntData.size() == 0) {
				UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(originalEvent) };

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
					ActionRowData actionRow{};
					for (uint64_t y = 0; y < originalEvent.getMessageData().components.at(x).components.size(); ++y) {
						ComponentData component = originalEvent.getMessageData().components.at(x).components.at(y);
						component.disabled		= true;
						actionRow.components.emplace_back(component);
					}
					dataPackage02->addComponentRow(actionRow);
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
				dataPackage03.buttonId		 = "exit";
				return dataPackage03;

			} else if (buttonIntData.at(0).buttonId == "empty" || buttonIntData.at(0).buttonId == "exit") {
				UniquePtr<RespondToInputEventData> dataPackage02{ makeUnique<RespondToInputEventData>(originalEvent) };
				if (buttonIntData.at(0).buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					interactionData = makeUnique<InteractionData>(buttonIntData.at(0));
					*dataPackage02	= RespondToInputEventData{ *interactionData };
				}

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
					ActionRowData actionRow{};
					for (uint64_t y = 0; y < originalEvent.getMessageData().components.at(x).components.size(); ++y) {
						ComponentData component = originalEvent.getMessageData().components.at(x).components.at(y);
						component.disabled		= true;
						actionRow.components.emplace_back(component);
					}
					dataPackage02->addComponentRow(actionRow);
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
				dataPackage03.buttonId		 = "exit";
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
				interactionData	 = makeUnique<InteractionData>(buttonIntData.at(0));
				auto dataPackage = RespondToInputEventData{ *interactionData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
					ActionRowData actionRow{};
					for (uint64_t y = 0; y < originalEvent.getMessageData().components.at(x).components.size(); ++y) {
						ComponentData component = originalEvent.getMessageData().components.at(x).components.at(y);
						component.disabled		= false;
						actionRow.components.emplace_back(component);
					}
					dataPackage.addComponentRow(actionRow);
				}
				dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				InputEvents::respondToInputEventAsync(dataPackage).get();
			} else if (buttonIntData.at(0).buttonId == "select") {
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					UniquePtr<InteractionData> interactionDataNew = makeUnique<InteractionData>(buttonIntData.at(0));
					auto dataPackage							  = RespondToInputEventData{ *interactionDataNew };
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
					for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
						ActionRowData actionRow{};
						for (uint64_t y = 0; y < originalEvent.getMessageData().components.at(x).components.size(); ++y) {
							ComponentData component = originalEvent.getMessageData().components.at(x).components.at(y);
							component.disabled		= true;
							actionRow.components.emplace_back(component);
						}
						dataPackage.addComponentRow(actionRow);
					}
					originalEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				}
				returnData.currentPageIndex = newCurrentPageIndex;
				returnData.inputEventData	= originalEvent;
				returnData.buttonId			= buttonIntData.at(0).buttonId;
				return returnData;
			}
		};
		return returnData;
	};

};