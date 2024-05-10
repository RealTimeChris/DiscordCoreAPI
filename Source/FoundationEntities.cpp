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
#include <discordcoreapi/Utilities/UnicodeEmoji.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		websocket_resume_data::operator discord_core_internal::etf_serializer() {
			etf_serializer data{};
			data["seq"]		   = lastNumberReceived;
			data["session_id"] = sessionId;
			data["token"]	   = botToken;
			return data;
		}

		websocket_identify_data::operator discord_core_internal::etf_serializer() {
			etf_serializer serializer{};
			serializer["intents"]				= intents;
			serializer["large_threshold"]		= 250;
			serializer["presence"]				= presence.operator discord_core_internal::etf_serializer();
			serializer["properties"]["browser"] = "DiscordCoreAPI";
			serializer["properties"]["device"]	= "DiscordCoreAPI";
			serializer["properties"]["os"]		= properties.os;
			serializer["shard"].emplaceBack(shard.at(0));
			serializer["shard"].emplaceBack(shard.at(1));
			serializer["token"] = botToken;
			return serializer;
		}

		https_workload_data& https_workload_data::operator=(https_workload_data&& other) noexcept {
			if (this != &other) {
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

		https_workload_data::https_workload_data(https_workload_data&& other) noexcept {
			*this = std::move(other);
		}

		https_workload_data& https_workload_data::operator=(https_workload_type type) {
			workloadType = type;
			return *this;
		}

		https_workload_type https_workload_data::getWorkloadType() const {
			return workloadType;
		}

		https_workload_data::https_workload_data(https_workload_type type) {
			*this = type;
		}
	}

	update_presence_data::operator discord_core_internal::etf_serializer() {
		discord_core_internal::etf_serializer data{};
		for (auto& value: activities) {
			discord_core_internal::etf_serializer newData{};
			if (value.url != "") {
				newData["url"] = value.url;
			}
			if (value.state != "") {
				newData["state"] = value.state;
			}
			newData["name"] = value.name;
			newData["type"] = value.type;
			data["activities"].emplaceBack(newData);
		}
		data["status"] = statusReal;
		data["since"]  = since;
		data["afk"]	   = false;
		return data;
	}

	update_voice_state_data::operator discord_core_internal::etf_serializer() {
		discord_core_internal::etf_serializer data{};
		if (channelId == 0) {
			data["channel_id"] = discord_core_internal::json_type::null_t;
		} else {
			data["channel_id"] = channelId.operator jsonifier::string();
		}
		data["self_deaf"] = selfDeaf;
		data["self_mute"] = selfMute;
		data["guild_id"]  = guildId.operator jsonifier::string();
		return data;
	}

	update_voice_state_data_dc::operator discord_core_internal::etf_serializer() {
		discord_core_internal::etf_serializer data{};
		data["channel_id"] = discord_core_internal::json_type::null_t;
		data["self_deaf"]  = selfDeaf;
		data["self_mute"]  = selfMute;
		data["guild_id"]   = guildId.operator jsonifier::string();
		return data;
	}

	voice_state_data_light guild_member_data::getVoiceStateData() {
		return guild_members::getVoiceStateData(*this);
	}

	user_cache_data guild_member_data::getUserData() {
		if (user.id != 0) {
			return users::getCachedUser({ .userId = user.id });
		} else {
			return {};
		}
	}

	voice_state_data_light guild_member_cache_data::getVoiceStateData() {
		return guild_members::getVoiceStateData(*this);
	}

	user_cache_data guild_member_cache_data::getUserData() {
		if (user.id != 0) {
			return users::getCachedUser({ .userId = user.id });
		} else {
			return {};
		}
	}

	bool guild_cache_data::areWeConnected() {
		return getDiscordCoreClient()->getVoiceConnection(id).areWeConnected();
	}

	void guild_cache_data::disconnect() {
		update_voice_state_data data{};
		data.channelId = 0;
		data.selfDeaf  = false;
		data.selfMute  = false;
		data.guildId   = id;
		discord_core_client::getSongAPI(id).disconnect();
		discord_core_client::getBotUser().updateVoiceStatus(data);
		discord_core_client::getVoiceConnection(id).disconnect();
	}

	bool guild_data::areWeConnected() {
		return getDiscordCoreClient()->getVoiceConnection(id).areWeConnected();
	}

	void guild_data::disconnect() {
		discord_core_internal::websocket_message_data<update_voice_state_data> data{};
		data.jsonifierExcludedKeys.emplace("t");
		data.jsonifierExcludedKeys.emplace("s");
		data.d.channelId = 0;
		data.d.selfDeaf	 = false;
		data.d.selfMute	 = false;
		data.d.guildId	 = id;
		discord_core_client::getSongAPI(id).disconnect();
		discord_core_client::getBotUser().updateVoiceStatus(data.d);
		discord_core_client::getVoiceConnection(id).disconnect();
	}

	bool application_command_option_data::operator==(const application_command_option_data& rhs) const {
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

	embed_data& embed_data::setAuthor(const jsonifier::string& authorName, const jsonifier::string& authorAvatarUrl) {
		author.name	   = authorName;
		author.iconUrl = authorAvatarUrl;
		return *this;
	}

	embed_data& embed_data::setFooter(const jsonifier::string& footerText, const jsonifier::string& footerIconUrlText) {
		footer.text	   = footerText;
		footer.iconUrl = footerIconUrlText;
		return *this;
	}

	embed_data& embed_data::setTimeStamp(const jsonifier::string& timeStampNew) {
		timeStamp = static_cast<jsonifier::string>(timeStampNew);
		return *this;
	}

	embed_data& embed_data::addField(const jsonifier::string& name, const jsonifier::string& value, bool Inline) {
		embed_field_data field{};
		field.Inline = Inline;
		field.value	 = value;
		field.name	 = name;
		fields.emplace_back(field);
		return *this;
	}

	embed_data& embed_data::setDescription(const jsonifier::string& descriptionNew) {
		description = descriptionNew;
		return *this;
	}

	embed_data& embed_data::setColor(const jsonifier::string& hexColorValueNew) {
		hexColorValue = jsonifier::strToUint64<16>(static_cast<jsonifier::string>(hexColorValueNew));
		return *this;
	}

	embed_data& embed_data::setThumbnail(const jsonifier::string& thumbnailUrl) {
		thumbnail.url = thumbnailUrl;
		return *this;
	}

	embed_data& embed_data::setTitle(const jsonifier::string& titleNew) {
		title = titleNew;
		return *this;
	}

	embed_data& embed_data::setImage(const jsonifier::string& imageUrl) {
		image.url = imageUrl;
		return *this;
	}

	void embed_image_data::generateExcludedKeys() {
		if (width == 0) {
			jsonifierExcludedKeys.emplace("width");
		}
		if (height == 0) {
			jsonifierExcludedKeys.emplace("height");
		}
		if (proxyUrl == "") {
			jsonifierExcludedKeys.emplace("proxy_url");
		}
	}

	void embed_author_data::generateExcludedKeys() {
		if (name == "") {
			jsonifierExcludedKeys.emplace("name");
		}
		if (url == "") {
			jsonifierExcludedKeys.emplace("url");
		}
		if (iconUrl == "") {
			jsonifierExcludedKeys.emplace("icon_url");
		}
		if (proxyIconUrl == "") {
			jsonifierExcludedKeys.emplace("proxy_icon_url");
		}
	}

	void embed_data::generateExcludedKeys() {
		if (fields.size() == 0) {
			jsonifierExcludedKeys.emplace("fields");
		}
		if (url == "") {
			jsonifierExcludedKeys.emplace("url");
		}
		if (hexColorValue == 0) {
			jsonifierExcludedKeys.emplace("color");
		}
		if (title == "") {
			jsonifierExcludedKeys.emplace("title");
		}
		if (description == "") {
			jsonifierExcludedKeys.emplace("description");
		}
		if (type == "") {
			jsonifierExcludedKeys.emplace("type");
		}
		if (author.iconUrl == "" && author.name == "" && author.proxyIconUrl == "" && author.url == "") {
			jsonifierExcludedKeys.emplace("author");
		} else {
			author.generateExcludedKeys();
		}
		if (video.proxyUrl == "" && video.height == 0 && video.width == 0 && video.url == "") {
			jsonifierExcludedKeys.emplace("video");
		}
		if (footer.iconUrl == "" && footer.proxyIconUrl == "" && footer.text == "") {
			jsonifierExcludedKeys.emplace("footer");
		}
		if (image.height == 0 && image.width == 0 && image.proxyUrl == "" && image.url == "") {
			jsonifierExcludedKeys.emplace("image");
		}
		if (provider.name == "" && provider.url == "") {
			jsonifierExcludedKeys.emplace("provider");
		}
		if (thumbnail.height == 0 && thumbnail.width == 0 && thumbnail.proxyUrl == "" && thumbnail.url == "") {
			jsonifierExcludedKeys.emplace("thumbnail");
		}
		if (timeStamp == "") {
			jsonifierExcludedKeys.emplace("timestamp");
		}
	}

	auto audit_log_data::getAuditLogData(const snowflake userIdOfChanger, audit_log_event auditLogType) {
		for (auto& value: auditLogEntries) {
			if (value.id == userIdOfChanger && value.actionType == auditLogType) {
				return value;
			}
		}
		return audit_log_entry_data();
	}

	auto audit_log_data::getAuditLogData(audit_log_event auditLogType, const snowflake userIdOfTarget) {
		for (auto& value: auditLogEntries) {
			if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
				return value;
			}
		}
		return audit_log_entry_data();
	}

	void partial_emoji_data::generateExcludedKeys() {
		if (name == "") {
			jsonifierExcludedKeys.emplace("name");
		}
		if (id == 0) {
			jsonifierExcludedKeys.emplace("id");
		}
	}

	void emoji_data::generateExcludedKeys() {
		if (id == 0) {
			jsonifierExcludedKeys.emplace("id");
		}
		if (managed == false) {
			jsonifierExcludedKeys.emplace("managed");
		}
		if (name == "") {
			jsonifierExcludedKeys.emplace("name");
		}
		if (roles.size() == 0) {
			jsonifierExcludedKeys.emplace("roles");
		}
		if (unicodeName.size() == 0) {
			jsonifierExcludedKeys.emplace("unicode_name");
		}
	}

	bool application_command_data::operator==(const application_command_data& rhs) const {
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

	bool application_command_option_choice_data::operator==(const application_command_option_choice_data& rhs) const {
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

	input_event_data& input_event_data::operator=(input_event_data&& other) noexcept {
		if (this != &other) {
			interactionData				   = std::move(other.interactionData);
			interactionData->member.guildId = interactionData->guildId;
			responseType = other.responseType;
		}
		return *this;
	}

	input_event_data::input_event_data(input_event_data&& other) noexcept {
		*this = std::move(other);
	}

	input_event_data& input_event_data::operator=(const input_event_data& other) {
		if (this != &other) {
			interactionData					= makeUnique<interaction_data>(*other.interactionData);
			interactionData->member.guildId = interactionData->guildId;
			responseType	 = other.responseType;
		}
		return *this;
	}

	input_event_data::input_event_data(const input_event_data& other) {
		*this = other;
	}

	input_event_data& input_event_data::operator=(const interaction_data& other) {
		interactionData					= makeUnique<interaction_data>(other);
		interactionData->member.guildId = interactionData->guildId;
		return *this;
	}

	input_event_data::input_event_data(const interaction_data& interactionData) {
		*this = interactionData;
	}

	const user_data& input_event_data::getUserData() const {
		if (interactionData->member.user.id != 0) {
			return interactionData->member.user;
		} else {
			return interactionData->user;
		}
	}

	const channel_data& input_event_data::getChannelData() const {
		return interactionData->channel;
	}

	const guild_member_data& input_event_data::getGuildMemberData() const {
		return interactionData->member;
	}

	void interaction_callback_data::generateExcludedKeys() {
		if (allowedMentions.parse.size() == 0 && allowedMentions.roles.size() == 0 && allowedMentions.users.size() == 0) {
			jsonifierExcludedKeys.emplace("allowed_mentions");
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
			jsonifierExcludedKeys.emplace("attachments");
		}
		if (choices.size() == 0) {
			jsonifierExcludedKeys.emplace("choices");
		}
		if (flags == 0) {
			jsonifierExcludedKeys.emplace("flags");
		}
		if (content == "") {
			jsonifierExcludedKeys.emplace("content");
		}
		if (customId == "") {
			jsonifierExcludedKeys.emplace("custom_id");
		}
		if (files.size() == 0) {
			jsonifierExcludedKeys.emplace("files");
		}
		if (title == "") {
			jsonifierExcludedKeys.emplace("title");
		}
	}

	void action_row_data::generateExcludedKeys() {
		if (components.size() == 0) {
			jsonifierExcludedKeys.emplace("components");
		} else {
			for (auto& value: components) {
				value.generateExcludedKeys();
			}
		}
	}

	void component_data::generateExcludedKeys() {
		switch (type) {
			case component_type::Button: {
				if (customId == "") {
					jsonifierExcludedKeys.emplace("custom_id");
				}
				if (emoji.name == "") {
					jsonifierExcludedKeys.emplace("emoji");
				} else {
					emoji.generateExcludedKeys();
				}
				if (label == "") {
					jsonifierExcludedKeys.emplace("label");
				}
				if (placeholder == "") {
					jsonifierExcludedKeys.emplace("placeholder");
				}
				if (url == "") {
					jsonifierExcludedKeys.emplace("url");
				}
				jsonifierExcludedKeys.emplace("options");
				jsonifierExcludedKeys.emplace("channel_types");
				jsonifierExcludedKeys.emplace("placeholder");
				jsonifierExcludedKeys.emplace("min_values");
				jsonifierExcludedKeys.emplace("max_values");
				jsonifierExcludedKeys.emplace("min_length");
				jsonifierExcludedKeys.emplace("max_length");
				jsonifierExcludedKeys.emplace("required");
				jsonifierExcludedKeys.emplace("value");
				jsonifierExcludedKeys.emplace("title");
				break;
			}
			case component_type::Text_Input: {
				if (customId == "") {
					jsonifierExcludedKeys.emplace("custom_id");
				}
				if (label == "") {
					jsonifierExcludedKeys.emplace("label");
				}
				if (maxLength == 0) {
					jsonifierExcludedKeys.emplace("max_length");
				}
				if (minLength == 0) {
					jsonifierExcludedKeys.emplace("min_length");
				}
				if (placeholder == "") {
					jsonifierExcludedKeys.emplace("placeholder");
				}
				if (title == "") {
					jsonifierExcludedKeys.emplace("title");
				}
				if (value == "") {
					jsonifierExcludedKeys.emplace("value");
				}
				jsonifierExcludedKeys.emplace("style");
				jsonifierExcludedKeys.emplace("label");
				jsonifierExcludedKeys.emplace("emoji");
				jsonifierExcludedKeys.emplace("url");
				jsonifierExcludedKeys.emplace("min_length");
				jsonifierExcludedKeys.emplace("max_length");
				jsonifierExcludedKeys.emplace("required");
				jsonifierExcludedKeys.emplace("title");
				break;
			}
			case component_type::Action_Row:
				[[fallthrough]];
			case component_type::Channel_Select:
				[[fallthrough]];
			case component_type::Mentionable_Select:
				[[fallthrough]];
			case component_type::Role_Select:
				[[fallthrough]];
			case component_type::String_Select:
				[[fallthrough]];
			case component_type::User_Select:
				[[fallthrough]];
			default: {
				if (customId == "") {
					jsonifierExcludedKeys.emplace("custom_id");
				}
				if (channelTypes.size() == 0) {
					jsonifierExcludedKeys.emplace("channel_types");
				}
				if (label == "") {
					jsonifierExcludedKeys.emplace("label");
				}
				if (maxValues == 0) {
					jsonifierExcludedKeys.emplace("max_values");
				}
				if (minValues == 0) {
					jsonifierExcludedKeys.emplace("min_values");
				}
				if (options.size() == 0) {
					jsonifierExcludedKeys.emplace("options");
				}
				if (placeholder == "") {
					jsonifierExcludedKeys.emplace("placeholder");
				}
				jsonifierExcludedKeys.emplace("style");
				jsonifierExcludedKeys.emplace("label");
				jsonifierExcludedKeys.emplace("emoji");
				jsonifierExcludedKeys.emplace("url");
				jsonifierExcludedKeys.emplace("min_length");
				jsonifierExcludedKeys.emplace("max_length");
				jsonifierExcludedKeys.emplace("required");
				jsonifierExcludedKeys.emplace("value");
				jsonifierExcludedKeys.emplace("title");
				jsonifierExcludedKeys.emplace("placeholer");
				break;
			}
		}
	}

	void application_command_option_data::generateExcludedKeys() {
		for (auto& value: options) {
			value.generateExcludedKeys();
		}
		for (auto& value: choices) {
			value.generateExcludedKeys();
		}
		if (maxValue == std::numeric_limits<int64_t>::min()) {
			jsonifierExcludedKeys.emplace("max_value");
		}
		if (minValue == std::numeric_limits<int64_t>::max()) {
			jsonifierExcludedKeys.emplace("min_value");
		}
		if (nameLocalizations.size() == 0) {
			jsonifierExcludedKeys.emplace("name_localizations");
		}
		if (choices.size() == 0) {
			jsonifierExcludedKeys.emplace("choices");
		}
		if (options.size() == 0) {
			jsonifierExcludedKeys.emplace("options");
		}
		if (!autocomplete) {
			jsonifierExcludedKeys.emplace("autocomplete");
		}
		if (descriptionLocalizations.size() == 0) {
			jsonifierExcludedKeys.emplace("description_localizations");
		}
		if (channelTypes.size() == 0) {
			jsonifierExcludedKeys.emplace("channel_types");
		}
	}

	void application_command_option_choice_data::generateExcludedKeys() {
		if (nameLocalizations.size() == 0) {
			jsonifierExcludedKeys.emplace("name_localizations");
		}
	}

	void application_command_data::generateExcludedKeys() {
		if (descriptionLocalizations.size() == 0) {
			jsonifierExcludedKeys.emplace("description_localizations");
		}
		if (nameLocalizations.size() == 0) {
			jsonifierExcludedKeys.emplace("name_localizations");
		}
		if (id == 0) {
			jsonifierExcludedKeys.emplace("id");
		}
		if (version == "") {
			jsonifierExcludedKeys.emplace("version");
		}
		if (guildId == 0) {
			jsonifierExcludedKeys.emplace("guild_id");
		}
		for (auto& value: options) {
			value.generateExcludedKeys();
		}
		if (options.size() == 0) {
			jsonifierExcludedKeys.emplace("options");
		}
	}

	const interaction_data& input_event_data::getInteractionData() const {
		return *interactionData;
	}

	const message_data& input_event_data::getMessageData() const {
		return interactionData->message;
	}

	respond_to_input_event_data::operator interaction_callback_data() const {
		interaction_callback_data returnData{};
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

	respond_to_input_event_data& respond_to_input_event_data::operator=(const interaction_data dataPackage) {
		applicationId	 = dataPackage.applicationId;
		messageId		 = dataPackage.message.id;
		channelId		 = dataPackage.channelId;
		interactionToken = dataPackage.token;
		eventType		 = dataPackage.type;
		interactionId	 = dataPackage.id;
		return *this;
	};

	respond_to_input_event_data::respond_to_input_event_data(const interaction_data dataPackage) {
		*this = dataPackage;
	}

	respond_to_input_event_data& respond_to_input_event_data::operator=(const input_event_data dataPackage) {
		applicationId	 = dataPackage.getInteractionData().applicationId;
		interactionToken = dataPackage.getInteractionData().token;
		channelId		 = dataPackage.interactionData->channelId;
		interactionId	 = dataPackage.getInteractionData().id;
		messageId		 = dataPackage.getMessageData().id;
		type			 = dataPackage.responseType;
		return *this;
	}

	respond_to_input_event_data::respond_to_input_event_data(const input_event_data dataPackage) {
		*this = dataPackage;
	}

	respond_to_input_event_data& respond_to_input_event_data::addButton(bool disabled, const jsonifier::string& customIdNew, const jsonifier::string& buttonLabel,
		button_style buttonStyle, const jsonifier::string& emojiName, snowflake emojiId, const jsonifier::string& url) {
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data component;
				component.type		 = component_type::Button;
				component.emoji.name = emojiName;
				component.label		 = buttonLabel;
				component.style		 = static_cast<uint64_t>(buttonStyle);
				component.customId	 = customIdNew;
				component.disabled	 = disabled;
				component.emoji.id	 = emojiId;
				component.url		 = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addSelectMenu(bool disabled, const jsonifier::string& customIdNew, const jsonifier::vector<select_option_data>& options,
		const jsonifier::string& placeholder, uint64_t maxValues, uint64_t minValues, select_menu_type typeNew, jsonifier::vector<channel_type> channelTypes) {
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data componentData;
				componentData.type		   = static_cast<component_type>(typeNew);
				componentData.channelTypes = channelTypes;
				componentData.placeholder  = placeholder;
				componentData.customId	   = customIdNew;
				componentData.maxValues	   = maxValues;
				componentData.minValues	   = minValues;
				componentData.disabled	   = disabled;
				componentData.options	   = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addModal(const jsonifier::string& topTitleNew, const jsonifier::string& topCustomIdNew, const jsonifier::string& titleNew,
		const jsonifier::string& customIdNew, bool required, uint64_t minLength, uint64_t maxLength, text_input_style inputStyle, const jsonifier::string& label,
		const jsonifier::string& placeholder) {
		title	 = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data component{};
				component.type		  = component_type::Text_Input;
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
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addFile(const file& theFile) {
		files.emplace_back(theFile);
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addAllowedMentions(const allowed_mentions_data& dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::setResponseType(input_event_response_type typeNew) {
		type = typeNew;
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addComponentRow(const action_row_data& dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addMessageEmbed(const embed_data dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::addContent(const jsonifier::string& dataPackage) {
		content = dataPackage;
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::setAutoCompleteChoice(discord_core_internal::etf_serializer value, const jsonifier::string& theName,
		const unordered_map<jsonifier::string, jsonifier::string>& theNameLocalizations) {
		application_command_option_choice_data choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name				 = theName;
		choiceData.value			 = jsonifier::string{ value.operator jsonifier::string_base<uint8_t>() };
		choices.emplace_back(choiceData);
		return *this;
	}

	respond_to_input_event_data& respond_to_input_event_data::setTargetUserID(const snowflake targetUserIdNew) {
		targetUserId = targetUserIdNew;
		return *this;
	}

	message_response_base& message_response_base::addButton(bool disabled, const jsonifier::string& customIdNew, const jsonifier::string& buttonLabel, button_style buttonStyle,
		const jsonifier::string& emojiName, snowflake emojiId, const jsonifier::string& url) {
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data component;
				component.type		 = component_type::Button;
				component.emoji.name = emojiName;
				component.label		 = buttonLabel;
				component.style		 = static_cast<uint64_t>(buttonStyle);
				component.customId	 = customIdNew;
				component.disabled	 = disabled;
				component.emoji.id	 = emojiId;
				component.url		 = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	message_response_base& message_response_base::addSelectMenu(bool disabled, const jsonifier::string& customIdNew, const jsonifier::vector<select_option_data>& options,
		const jsonifier::string& placeholder, uint64_t maxValues, uint64_t minValues, select_menu_type type, jsonifier::vector<channel_type> channelTypes) {
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data componentData;
				componentData.type		   = static_cast<component_type>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder  = placeholder;
				componentData.customId	   = customIdNew;
				componentData.maxValues	   = maxValues;
				componentData.minValues	   = minValues;
				componentData.disabled	   = disabled;
				componentData.options	   = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	message_response_base& message_response_base::addModal(const jsonifier::string& topTitleNew, const jsonifier::string& topCustomIdNew, const jsonifier::string& titleNew,
		const jsonifier::string& customIdNew, bool required, uint64_t minLength, uint64_t maxLength, text_input_style inputStyle, const jsonifier::string& label,
		const jsonifier::string& placeholder) {
		title	 = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data component{};
				component.type		  = component_type::Text_Input;
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
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	message_response_base& message_response_base::addFile(const file& theFile) {
		files.emplace_back(theFile);
		return *this;
	}

	message_response_base& message_response_base::addAllowedMentions(const allowed_mentions_data& dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	message_response_base& message_response_base::addComponentRow(const action_row_data& dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	message_response_base& message_response_base::addMessageEmbed(const embed_data& dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	message_response_base& message_response_base::addContent(const jsonifier::string& dataPackage) {
		content = dataPackage;
		return *this;
	}

	message_response_base& message_response_base::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	interaction_response_data& interaction_response_data::operator=(const respond_to_input_event_data& other) {
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

	interaction_response_data::interaction_response_data(const respond_to_input_event_data& other) {
		*this = other;
	}

	void parseCommandDataOption(unordered_map<jsonifier::string, jsonifier::raw_json_data>& values, application_command_interaction_data_option& data) {
		jsonifier::raw_json_data valueNew{};
		valueNew		  = data.value;
		values[data.name] = valueNew;
		for (auto& value: data.options) {
			parseCommandDataOption(values, value);
		}
	}

	serializer_value command_data::getCommandArguments() const {
		return optionsArgs;
	}

	command_data& command_data::operator=(const command_data& other) {
		subCommandGroupName = other.subCommandGroupName;
		subCommandName		= other.subCommandName;
		commandName			= other.commandName;
		optionsArgs			= other.optionsArgs;
		eventData			= makeUnique<input_event_data>(*other.eventData);
		return *this;
	}

	command_data::command_data(const command_data& other) {
		*this = other;
	}

	command_data::command_data(const input_event_data& inputEventData) {
		if (inputEventData.interactionData->data.name != "") {
			commandName = inputEventData.interactionData->data.name;
		}
		if (inputEventData.interactionData->data.targetId != 0) {
			optionsArgs.values.emplace("target_id", jsonifier::raw_json_data{ inputEventData.interactionData->data.targetId.operator jsonifier::string() });
		} else if (inputEventData.interactionData->data.targetId != 0) {
			optionsArgs.values.emplace("target_id", jsonifier::raw_json_data{ inputEventData.interactionData->data.targetId.operator jsonifier::string() });
		}
		eventData = makeUnique<input_event_data>(inputEventData);
		for (auto& value: eventData->interactionData->data.options) {
			jsonifier::raw_json_data serializer{ value.value.operator jsonifier::string() };
			optionsArgs.values[value.name] = serializer;
			parseCommandDataOption(optionsArgs.values, value);
		}
		for (auto& value: eventData->interactionData->data.options) {
			if (value.type == application_command_option_type::Sub_Command) {
				subCommandName = value.name;
			}
			if (value.type == application_command_option_type::Sub_Command_Group) {
				subCommandGroupName = value.name;
			}
		}
	}

	const interaction_data& command_data::getInteractionData() const {
		return *eventData->interactionData;
	}

	const guild_member_data& command_data::getGuildMemberData() const {
		return eventData->getGuildMemberData();
	}

	const channel_data& command_data::getChannelData() const {
		return eventData->getChannelData();
	}

	const message_data& command_data::getMessageData() const {
		return eventData->getMessageData();
	}

	const user_data& command_data::getUserData() const {
		return eventData->getUserData();
	}

	jsonifier::string command_data::getCommandName() const {
		return commandName;
	}

	jsonifier::string command_data::getSubCommandName() const {
		return subCommandName;
	}

	jsonifier::string command_data::getSubCommandGroupName() const {
		return subCommandGroupName;
	}

	const input_event_data& command_data::getInputEventData() const {
		return *eventData;
	}

	base_function_arguments::base_function_arguments(const command_data& commanddataNew) : command_data{ commanddataNew } {};

	move_through_message_pages_data moveThroughMessagePages(snowflake userID, input_event_data originalEvent, uint32_t currentPageIndex,
		const jsonifier::vector<embed_data>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		unique_ptr<move_through_message_pages_data> returnData{ makeUnique<move_through_message_pages_data>() };
		uint32_t newCurrentPageIndex = currentPageIndex;
		stop_watch<milliseconds> stopWatch{ milliseconds{ waitForMaxMs } };
		stopWatch.reset();
		auto createResponseData	 = makeUnique<create_interaction_response_data>(originalEvent);
		auto interactionResponse = makeUnique<respond_to_input_event_data>(originalEvent);
		auto embedData			 = makeUnique<embed_data>();
		embedData->setColor("FEFEFE");
		embedData->setTitle("__**Permissions Issue:**__");
		embedData->setTimeStamp(getTimeAndDate());
		embedData->setDescription("Sorry, but that button can only be pressed by <@" + userID + ">!");
		createResponseData->addMessageEmbed(*embedData);
		createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
		if (messageEmbeds.size() > 0) {
			interactionResponse->addMessageEmbed(messageEmbeds[currentPageIndex]);
		}
		if (returnResult) {
			interactionResponse->addButton(false, "select", "Select", button_style::Success, unicode_emojis::x);
		}
		interactionResponse->addButton(false, "backwards", "Prev Page", button_style::Primary, unicode_emojis::arrow_left);
		interactionResponse->addButton(false, "forwards", "Next Page", button_style::Primary, unicode_emojis::arrow_right);
		interactionResponse->addButton(false, "exit", "Exit", button_style::Danger, unicode_emojis::x);
		interactionResponse->setResponseType(input_event_response_type::Edit_Interaction_Response);
		originalEvent = input_events::respondToInputEventAsync(*interactionResponse).get();
		while (!stopWatch.hasTimeElapsed()) {
			std::this_thread::sleep_for(1ms);
			unique_ptr<button_collector> button{ makeUnique<button_collector>(originalEvent) };
			jsonifier::vector<button_response_data> buttonIntData{ button->collectButtonData(false, waitForMaxMs, 1, *createResponseData, userID).get() };
			unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };
			if (buttonIntData.size() == 0) {
				unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(originalEvent) };

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
					action_row_data actionRow{};
					for (uint64_t y = 0; y < originalEvent.getMessageData().components[x].components.size(); ++y) {
						component_data component = originalEvent.getMessageData().components[x].components[y];
						component.disabled		= true;
						actionRow.components.emplace_back(component);
					}
					if (returnResult) {
						dataPackage02->addButton(false, "select", "Select", button_style::Success, unicode_emojis::x);
					}
					dataPackage02->addButton(false, "backwards", "Prev Page", button_style::Primary, unicode_emojis::arrow_left);
					dataPackage02->addButton(false, "forwards", "Next Page", button_style::Primary, unicode_emojis::arrow_right);
					dataPackage02->addButton(false, "exit", "Exit", button_style::Danger, unicode_emojis::x);
				}
				if (deleteAfter == true) {
					input_event_data dataPackage03{ originalEvent };
					input_events::deleteInputEventResponseAsync(dataPackage03);
				} else {
					dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
					input_events::respondToInputEventAsync(*dataPackage02).get();
				}
				unique_ptr<move_through_message_pages_data> dataPackage03{ makeUnique<move_through_message_pages_data>() };
				dataPackage03->inputEventData = originalEvent;
				dataPackage03->buttonId		  = "exit";
				return *dataPackage03;

			} else if (buttonIntData[0].buttonId == "empty" || buttonIntData[0].buttonId == "exit") {
				unique_ptr<respond_to_input_event_data> dataPackage02{ makeUnique<respond_to_input_event_data>(originalEvent) };
				if (buttonIntData[0].buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					interactionData = makeUnique<interaction_data>(buttonIntData[0]);
					*dataPackage02	= respond_to_input_event_data{ *interactionData };
				}

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
					action_row_data actionRow{};
					for (uint64_t y = 0; y < originalEvent.getMessageData().components[x].components.size(); ++y) {
						component_data component = originalEvent.getMessageData().components[x].components[y];
						component.disabled		 = true;
						actionRow.components.emplace_back(component);
					}
					if (returnResult) {
						dataPackage02->addButton(true, "select", "Select", button_style::Success, unicode_emojis::x);
					}
					dataPackage02->addButton(true, "backwards", "Prev Page", button_style::Primary, unicode_emojis::arrow_left);
					dataPackage02->addButton(true, "forwards", "Next Page", button_style::Primary, unicode_emojis::arrow_right);
					dataPackage02->addButton(true, "exit", "Exit", button_style::Danger, unicode_emojis::x);
				}
				if (deleteAfter == true) {
					input_event_data dataPackage03{ originalEvent };
					input_events::deleteInputEventResponseAsync(dataPackage03);
				} else {
					dataPackage02->setResponseType(input_event_response_type::Edit_Interaction_Response);
					input_events::respondToInputEventAsync(*dataPackage02).get();
				}
				unique_ptr<move_through_message_pages_data> dataPackage03{ makeUnique<move_through_message_pages_data>() };
				dataPackage03->inputEventData = originalEvent;
				dataPackage03->buttonId		  = "exit";
				return *dataPackage03;
			} else if (buttonIntData[0].buttonId == "forwards" || buttonIntData[0].buttonId == "backwards") {
				if (buttonIntData[0].buttonId == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
					newCurrentPageIndex = 0;
				} else if (buttonIntData[0].buttonId == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
					++newCurrentPageIndex;
				} else if (buttonIntData[0].buttonId == "backwards" && (newCurrentPageIndex > 0)) {
					--newCurrentPageIndex;
				} else if (buttonIntData[0].buttonId == "backwards" && (newCurrentPageIndex == 0)) {
					newCurrentPageIndex = static_cast<uint32_t>(messageEmbeds.size()) - 1;
				}
				interactionData	 = makeUnique<interaction_data>(buttonIntData[0]);
				auto dataPackage = respond_to_input_event_data{ *interactionData };
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
					action_row_data actionRow{};
					for (uint64_t y = 0; y < originalEvent.getMessageData().components[x].components.size(); ++y) {
						component_data component = originalEvent.getMessageData().components[x].components[y];
						component.disabled		 = false;
						actionRow.components.emplace_back(component);
					}
					if (returnResult) {
						dataPackage.addButton(false, "select", "Select", button_style::Success, unicode_emojis::x);
					}
					dataPackage.addButton(false, "backwards", "Prev Page", button_style::Primary, unicode_emojis::arrow_left);
					dataPackage.addButton(false, "forwards", "Next Page", button_style::Primary, unicode_emojis::arrow_right);
					dataPackage.addButton(false, "exit", "Exit", button_style::Danger, unicode_emojis::x);
				}
				dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				input_events::respondToInputEventAsync(dataPackage).get();
			} else if (buttonIntData[0].buttonId == "select") {
				if (deleteAfter == true) {
					input_event_data dataPackage03{ originalEvent };
					input_events::deleteInputEventResponseAsync(dataPackage03);
				} else {
					interactionData	 = makeUnique<interaction_data>(buttonIntData[0]);
					auto dataPackage = respond_to_input_event_data{ *interactionData };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					for (uint64_t x = 0; x < originalEvent.getMessageData().components.size(); ++x) {
						action_row_data actionRow{};
						for (uint64_t y = 0; y < originalEvent.getMessageData().components[x].components.size(); ++y) {
							component_data component = originalEvent.getMessageData().components[x].components[y];
							component.disabled		 = true;
							actionRow.components.emplace_back(component);
						}
						if (returnResult) {
							dataPackage.addButton(true, "select", "Select", button_style::Success, unicode_emojis::x);
						}
						dataPackage.addButton(true, "backwards", "Prev Page", button_style::Primary, unicode_emojis::arrow_left);
						dataPackage.addButton(true, "forwards", "Next Page", button_style::Primary, unicode_emojis::arrow_right);
						dataPackage.addButton(true, "exit", "Exit", button_style::Danger, unicode_emojis::x);
					}
					dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
					input_events::respondToInputEventAsync(dataPackage).get();
				}
				returnData->currentPageIndex = newCurrentPageIndex;
				returnData->inputEventData	= originalEvent;
				returnData->buttonId		 = buttonIntData[0].buttonId;
				return *returnData;
			}
		};
		return *returnData;
	};

};
