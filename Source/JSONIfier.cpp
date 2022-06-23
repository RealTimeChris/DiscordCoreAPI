/*
*
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
/// JSONIfier.cpp - Source file for all things related to JSON parsing etc.
/// Apr 21, 2021
/// https://discordcoreapi.com
/// \file JSONIfier.cpp

#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>

namespace DiscordCoreAPI {

	void to_json(nlohmann::json& jsonOut, const StringWrapper& valueNew) {
		nlohmann::json newValue{};
		std::string theValue{};
		for (auto& value: static_cast<std::string>(static_cast<StringWrapper>(valueNew))) {
			theValue.push_back(value);
		}
		newValue = theValue;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const MessageReferenceData& valueNew) {
		nlohmann::json newValue{};
		newValue["fail_if_not_exists"] = valueNew.failIfNotExists;
		newValue["message_id"] = valueNew.messageId;
		newValue["channel_id"] = valueNew.channelId;
		newValue["guild_id"] = valueNew.guildId;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const AllowedMentionsData& valueNew) {
		nlohmann::json newValue{};
		newValue["replied_user"] = valueNew.repliedUser;
		newValue["parse"] = valueNew.parse;
		newValue["roles"] = valueNew.roles;
		newValue["users"] = valueNew.users;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const AttachmentData& valueNew) {
		nlohmann::json newValue{};
		newValue["content_type"] = valueNew.contentType;
		newValue["description"] = valueNew.description;
		newValue["ephemeral"] = valueNew.ephemeral;
		newValue["file_name"] = valueNew.filename;
		newValue["proxy_url"] = valueNew.proxyUrl;
		newValue["height"] = valueNew.height;
		newValue["width"] = valueNew.width;
		newValue["size"] = valueNew.size;
		newValue["url"] = valueNew.url;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const EmbedFieldData& valueNew) {
		nlohmann::json newValue{};
		newValue["inline"] = valueNew.Inline;
		newValue["value"] = valueNew.value;
		newValue["name"] = valueNew.name;
		jsonOut.update(newValue);
	}

	void to_json(nlohmann::json& jsonOut, const std::vector<ActionRowData>& dataPackage) {
		nlohmann::json componentsActionRow{};
		for (auto& value: dataPackage) {
			nlohmann::json components{};
			for (auto& valueNew: value.components) {
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
							option["emoji"]["name"] = value01.emoji.name;
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
			nlohmann::json componentActionRow{};
			componentActionRow["components"] = components;
			componentActionRow["type"] = 1;
			jsonOut = componentActionRow;
		}
	}

	void to_json(nlohmann::json& jsonOut, const EmbedData& valueNew) {
		nlohmann::json fields{};
		for (auto& value2: valueNew.fields) {
			fields.push_back(value2);
		}
		int32_t colorValInt = stol(valueNew.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << colorValInt;
		std::string realColorVal = stream.str();
		nlohmann::json embed{};
		embed["footer"]["proxy_icon_url"] = valueNew.footer.proxyIconUrl;
		embed["author"]["proxy_icon_url"] = valueNew.author.proxyIconUrl;
		embed["thumbnail"]["proxy_url"] = valueNew.thumbnail.proxyUrl;
		embed["thumbnail"]["height"] = valueNew.thumbnail.height;
		embed["thumbnail"]["width"] = valueNew.thumbnail.width;
		embed["image"]["proxy_url"] = valueNew.image.proxyUrl;
		embed["author"]["icon_url"] = valueNew.author.iconUrl;
		embed["footer"]["icon_url"] = valueNew.footer.iconUrl;
		embed["video"]["proxy_url"] = valueNew.video.proxyUrl;
		embed["provider"]["name"] = valueNew.provider.name;
		embed["thumbnail"]["url"] = valueNew.thumbnail.url;
		embed["provider"]["url"] = valueNew.provider.url;
		embed["video"]["height"] = valueNew.video.height;
		embed["image"]["height"] = valueNew.image.height;
		embed["author"]["name"] = valueNew.author.name;
		embed["image"]["width"] = valueNew.image.width;
		embed["footer"]["text"] = valueNew.footer.text;
		embed["video"]["width"] = valueNew.video.width;
		embed["author"]["url"] = valueNew.author.url;
		embed["description"] = valueNew.description;
		embed["image"]["url"] = valueNew.image.url;
		embed["video"]["url"] = valueNew.video.url;
		embed["timestamp"] = valueNew.timestamp;
		embed["title"] = valueNew.title;
		embed["color"] = realColorVal;
		embed["type"] = valueNew.type;
		embed["url"] = valueNew.url;
		embed["fields"] = fields;
		jsonOut = embed;
	}
}

namespace DiscordCoreInternal {

	int32_t JSONIFY(const std::string& initialPayload) {
		nlohmann::json jsonVal = nlohmann::json::parse(initialPayload);
		int32_t finalValue = 0;
		if (jsonVal.contains("heartbeat_interval")) {
			finalValue = jsonVal["heartbeat_interval"];
		}
		return finalValue;
	};

	nlohmann::json JSONIFY(const std::string& serverId, const std::string& sessionId, const std::string& token, std::string) {
		nlohmann::json data{};
		data["d"]["server_id"] = serverId;
		data["d"]["session_id"] = sessionId;
		data["d"]["token"] = token;
		data["op"] = 7;
		return data;
	}

	nlohmann::json JSONIFY(const std::string& botToken, int64_t intents, int32_t currentShard, int32_t numberOfShards) {
		nlohmann::json data{};
		data["d"]["properties"]["browser"] = "DiscordCoreAPI";
		data["d"]["properties"]["device"] = "DiscordCoreAPI";
		data["d"]["shard"] = { currentShard, numberOfShards };
		data["d"]["large_threshold"] = 250;
		data["d"]["intents"] = intents;
		data["d"]["compress"] = false;
		data["d"]["token"] = botToken;
		data["op"] = 2;
#ifdef _WIN32
		data["d"]["properties"]["os"] = "Windows";
#else
		data["d"]["properties"]["os"] = "Linux";
#endif
		return data;
	};

	nlohmann::json JSONIFY(const std::string& botToken, std::string sessionID, int32_t lastReceivedNumber) {
		nlohmann::json data{};
		data["d"]["seq"] = lastReceivedNumber;
		data["d"]["session_id"] = sessionID;
		data["d"]["token"] = botToken;
		data["op"] = 6;
		return data;
	};

	nlohmann::json JSONIFY(const DiscordCoreAPI::UpdateVoiceStateData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.channelId == 0) {
			data["d"]["channel_id"] = nullptr;
		} else {
			data["d"]["channel_id"] = std::to_string(dataPackage.channelId);
		}
		data["d"]["self_deaf"] = dataPackage.selfDeaf;
		data["d"]["self_mute"] = dataPackage.selfMute;
		data["d"]["guild_id"] = std::to_string(dataPackage.guildId);
		data["op"] = 4;
		return data;
	}

	nlohmann::json JSONIFY(const DiscordCoreAPI::UpdatePresenceData& dataPackage) {
		nlohmann::json data{};
		data["d"]["activities"] = nlohmann::json{};
		for (auto& value: dataPackage.activities) {
			nlohmann::json dataNew{};
			if (value.url != "") {
				dataNew["url"] = value.url;
			}
			dataNew["name"] = value.name;
			dataNew["type"] = value.type;
			data["d"]["activities"].push_back(dataNew);
		}
		data["d"]["status"] = dataPackage.status;
		data["d"]["since"] = dataPackage.since;
		data["d"]["afk"] = dataPackage.afk;
		data["op"] = 3;
		return data;
	}

	nlohmann::json JSONIFY(int32_t lastReceivedNumber) {
		nlohmann::json data{};
		data["d"] = lastReceivedNumber;
		data["op"] = 1;
		return data;
	};

	std::vector<uint8_t> JSONIFY(const std::string& localPort, const std::string& localIp, const std::string& encryptionMode, int32_t) {
		nlohmann::json data{};
		data["d"]["data"]["port"] = stol(localPort);
		data["d"]["data"]["mode"] = encryptionMode;
		data["d"]["data"]["address"] = localIp;
		data["d"]["protocol"] = "udp";
		data["op"] = 1;
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	std::vector<uint8_t> JSONIFY(const VoiceConnectionData& dataPackage, const VoiceConnectInitData& dataPackage02) {
		nlohmann::json data{};
		data["d"]["session_id"] = dataPackage.sessionId;
		data["d"]["server_id"] = std::to_string(dataPackage02.guildId);
		data["d"]["user_id"] = std::to_string(dataPackage02.userId);
		data["d"]["token"] = dataPackage.token;
		data["op"] = 0;
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	std::vector<uint8_t> JSONIFY(int32_t ssrc, int32_t delay) {
		nlohmann::json data{};
		data["d"]["speaking"] = 1 << 0;
		data["d"]["delay"] = delay;
		data["d"]["ssrc"] = ssrc;
		data["op"] = 5;
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	std::vector<uint8_t> JSONIFY(int64_t nonce) {
		nlohmann::json data{};
		data["d"] = nonce;
		data["op"] = int32_t(3);
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	void JSONIFY(const DiscordCoreAPI::ApplicationCommandOptionData& dataPackage, nlohmann::json* pJSONData) {
		nlohmann::json newOption{};
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
			newOption["channel_types"] = dataPackage.channelTypes;
		}
		if (dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
			newOption["required"] = dataPackage.required;
		}
		newOption["description_localizations"] = dataPackage.descriptionLocalizations;
		newOption["name_localizations"] = dataPackage.nameLocalizations;
		newOption["description"] = dataPackage.description;
		newOption["min_value"] = dataPackage.minValue;
		newOption["max_value"] = dataPackage.maxValue;
		newOption["required"] = dataPackage.required;
		newOption["name"] = dataPackage.name;
		newOption["type"] = dataPackage.type;
		newOption["options"] = nlohmann::json{};
		newOption["choices"] = nlohmann::json{};
		if (dataPackage.choices.size() > 0) {
			for (int32_t x = 0; x < dataPackage.choices.size(); x++) {
				nlohmann::json jsonValue{};
				jsonValue["name_localizations"] = dataPackage.choices[x].nameLocalizations;
				jsonValue["value"] = dataPackage.choices[x].value;
				jsonValue["name"] = dataPackage.choices[x].name;
				newOption["choices"].push_back(jsonValue);
			}
		}
		if (newOption["choices"].size() == 0) {
			newOption["autocomplete"] = dataPackage.autocomplete;
		}
		if (dataPackage.options.size() > 0) {
			for (auto& value: dataPackage.options) {
				JSONIFY(value, &newOption["options"]);
			}
		}
		pJSONData->emplace_back(newOption);
		return;
	}

	std::string JSONIFY(const DiscordCoreAPI::EditGuildApplicationCommandPermissionsData& dataPackage) {
		nlohmann::json newDataArray{};
		for (auto& value: dataPackage.permissions) {
			nlohmann::json newData{};
			newData["permission"] = value.permission;
			newData["type"] = value.type;
			newData["id"] = std::to_string(value.id);
			newDataArray.push_back(newData);
		}
		return newDataArray.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::EditChannelPermissionOverwritesData& dataPackage) {
		nlohmann::json data{};
		data["allow"] = dataPackage.allow;
		data["deny"] = dataPackage.deny;
		data["type"] = dataPackage.type;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::CreateGlobalApplicationCommandData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		data["type"] = dataPackage.type;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x++) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildApplicationCommandData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		data["type"] = dataPackage.type;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x++) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
			data["options"] = nlohmann::json{};
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::EditGlobalApplicationCommandData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x++) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::EditGuildApplicationCommandData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x++) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
			data["options"] = nlohmann::json{};
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildChannelPositionsData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.modifyChannelData) {
			nlohmann::json dataNew{};
			dataNew["lock_permissions"] = value.lockPermissions;
			if (value.parentId != 0) {
				dataNew["parent_id"] = value.parentId;
			}
			dataNew["position"] = value.position;
			dataNew["id"] = std::to_string(value.id);
			data.push_back(dataNew);
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::StartThreadWithoutMessageData& dataPackage) {
		nlohmann::json data{};
		data["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		data["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		data["invitable"] = dataPackage.invitable;
		data["name"] = dataPackage.threadName;
		data["type"] = dataPackage.type;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::StartThreadInForumChannelData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.message.attachments) {
			data["message"]["attachments"].push_back(value);
		}
		if (dataPackage.message.components.size() == 0) {
			data["message"]["components"] = nlohmann::json::array();
		} else {
			data["message"]["components"] = nlohmann::json{ dataPackage.message.components };
		}
		data["message"]["allowed_mentions"] = dataPackage.message.allowedMentions;
		for (auto& value: dataPackage.message.stickerIds) {
			data["message"]["sticker_ids"].push_back(value);
		}
		if (dataPackage.message.embeds.size() == 0) {
			data["message"]["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: dataPackage.message.embeds) {
				data["message"]["embeds"].push_back(value);
			}
		}
		if (dataPackage.message.content != "") {
			data["message"]["content"] = dataPackage.message.content;
		}
		data["message"]["flags"] = dataPackage.message.flags;
		data["name"] = dataPackage.name;
		data["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		data["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildScheduledEventData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data["channel_id"] = nullptr;
		} else {
			data["channel_id"] = std::to_string(dataPackage.channelId);
		}
		data["entity_metadata"]["location"] = dataPackage.entityMetadata.location;
		data["entity_metadata"]["entity_type"] = dataPackage.entityType;
		data["scheduled_start_time"] = dataPackage.scheduledStartTime;
		data["scheduled_end_time"] = dataPackage.scheduledEndTime;
		data["privacy_level"] = dataPackage.privacyLevel;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildRolePositionsData& dataPackage) {
		nlohmann::json dataArray{};
		for (auto& value: dataPackage.rolePositions) {
			nlohmann::json data{};
			data["position"] = value.rolePosition;
			data["id"] = std::to_string(value.roleId);
			dataArray.push_back(data);
		}
		return dataArray.dump();
	};

	std::string JSONIFY(const DiscordCoreAPI::CreateAutoModerationRuleData& dataPackage) {
		nlohmann::json data{};
		data["actions"];
		for (auto& value: dataPackage.actions) {
			nlohmann::json dataNew{};
			dataNew["metadata"]["channel_id"] = value.metadata.channelId;
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			data["actions"].push_back(dataNew);
		}
		data["enabled"] = dataPackage.enabled;
		data["event_type"] = dataPackage.eventType;
		data["exempt_channels"];
		for (auto& value: dataPackage.exemptChannels) {
			data["exempt_channels"].push_back(value);
		}
		data["exempt_roles"];
		for (auto& value: dataPackage.exemptRoles) {
			data["exempt_roles"].push_back(value);
		}
		data["name"] = dataPackage.name;
		data["trigger_metadata"]["keyword_filter"] = dataPackage.triggerMetadata.keywordFilter;
		data["trigger_metadata"]["presets"] = dataPackage.triggerMetadata.presets;
		data["trigger_type"] = dataPackage.triggerType;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::ModifyAutoModerationRuleData& dataPackage) {
		nlohmann::json data{};
		data["actions"];
		for (auto& value: dataPackage.actions) {
			nlohmann::json dataNew{};
			dataNew["metadata"]["channel_id"] = value.metadata.channelId;
			dataNew["metadata"]["duration_seconds"] = value.metadata.durationSeconds;
			dataNew["type"] = value.type;
			data["actions"].push_back(dataNew);
		}
		data["enabled"] = dataPackage.enabled;
		data["event_type"] = dataPackage.eventType;
		data["exempt_channels"];
		for (auto& value: dataPackage.exemptChannels) {
			data["exempt_channels"].push_back(value);
		}
		data["exempt_roles"];
		for (auto& value: dataPackage.exemptRoles) {
			data["exempt_roles"].push_back(value);
		}
		data["name"] = dataPackage.name;
		data["trigger_metadata"]["keyword_filter"] = dataPackage.triggerMetadata.keywordFilter;
		data["trigger_metadata"]["presets"] = dataPackage.triggerMetadata.presets;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildScheduledEventData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data["channel_id"] = nullptr;
		} else {
			data["channel_id"] = std::to_string(dataPackage.channelId);
		}
		data["entity_metadata"]["location"] = dataPackage.entityMetadata.location;
		data["entity_metadata"]["entity_type"] = dataPackage.entityType;
		data["scheduled_start_time"] = dataPackage.scheduledStartTime;
		data["scheduled_end_time"] = dataPackage.scheduledEndTime;
		data["privacy_level"] = dataPackage.privacyLevel;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildWelcomeScreenData& dataPackage) {
		nlohmann::json channelsArray{};
		for (auto& value: dataPackage.welcomeChannels) {
			nlohmann::json newData{};
			newData["description"] = value.description;
			newData["channel_id"] = value.channelId;
			newData["emoji_name"] = value.emojiName;
			if (value.emojiId != 0) {
				newData["emoji_id"] = std::to_string(value.emojiId);
			}
			channelsArray.push_back(newData);
		}
		nlohmann::json data{};
		data["description"] = dataPackage.description;
		data["welcome_channels"] = channelsArray;
		data["enabled"] = dataPackage.enabled;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::StartThreadWithMessageData& dataPackage) {
		nlohmann::json data{};
		data["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		data["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		data["name"] = dataPackage.threadName;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::AddRecipientToGroupDMData& dataPackage) {
		nlohmann::json data{};
		data["access_token"] = dataPackage.token;
		data["nick"] = dataPackage.nick;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::InteractionResponseData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.data.attachments) {
			data["data"]["attachments"].push_back(value);
		}
		if (dataPackage.data.components.size() == 0) {
			data["data"]["components"] = nlohmann::json::array();
		} else {
			data["data"]["components"] = nlohmann::json{ dataPackage.data.components };
		}
		data["data"]["allowed_mentions"] = dataPackage.data.allowedMentions;
		if (dataPackage.data.choices.size() > 0) {
			nlohmann::json::array_t theArray{};
			for (auto& value: dataPackage.data.choices) {
				nlohmann::json theValue{};
				theValue["name"] = value.name;
				theValue["name_localizations"] = value.nameLocalizations;
				theValue["value"] = value.value;
				theArray.push_back(theValue);
			}
			data["data"]["choices"] = theArray;
		}
		if (dataPackage.data.embeds.size() == 0) {
			data["data"]["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: dataPackage.data.embeds) {
				data["data"]["embeds"].push_back(value);
			}
		}
		if (dataPackage.data.customId != "") {
			data["data"]["custom_id"] = dataPackage.data.customId;
		}
		if (dataPackage.data.content != "") {
			data["data"]["content"] = dataPackage.data.content;
		}
		if (dataPackage.data.title != "") {
			data["data"]["title"] = dataPackage.data.title;
		}
		data["data"]["flags"] = dataPackage.data.flags;
		data["data"]["tts"] = dataPackage.data.tts;
		data["type"] = dataPackage.type;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::CreateChannelInviteData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.targetUserId != 0) {
			data["target_application_id"] = dataPackage.targetApplicationId;
			data["target_user_id"] = std::to_string(dataPackage.targetUserId);
			data["target_type"] = dataPackage.targetType;
		}
		data["temporary"] = dataPackage.temporary;
		data["max_uses"] = dataPackage.maxUses;
		data["max_age"] = dataPackage.maxAge;
		data["unique"] = dataPackage.unique;
		return data.dump();
	};

	std::string JSONIFY(const DiscordCoreAPI::DeleteMessagesBulkData& dataPackage) {
		nlohmann::json data{};
		data["messages"] = dataPackage.messageIds;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Voice || dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			data["user_limit"] = dataPackage.userLimit;
			data["bitrate"] = dataPackage.bitrate;
		}
		nlohmann::json overwrites{};
		for (auto& value: dataPackage.permissionOverwrites) {
			nlohmann::json newData{};
			newData["allow"] = value.allow.getCurrentPermissionString();
			newData["deny"] = value.deny.getCurrentPermissionString();
			newData["channel_id"] = std::to_string(value.channelId);
			newData["type"] = value.type;
			newData["id"] = std::to_string(value.id);
			overwrites.push_back(newData);
		}
		data["default_auto_archive_duration"] = dataPackage.defaultAutoArchiveDuration;
		data["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		data["permission_overwrites"] = overwrites;
		data["parent_id"] = std::to_string(dataPackage.parentId);
		data["position"] = dataPackage.position;
		data["topic"] = dataPackage.topic;
		data["name"] = dataPackage.name;
		data["nsfw"] = dataPackage.nsfw;
		data["type"] = dataPackage.type;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData& dataPackage) {
		nlohmann::json data{};
		data["nick"] = dataPackage.nick;
		data["communication_disabled_until"] = std::string(dataPackage.communicationDisabledUntil);
		if (dataPackage.roleIds.size() == 0) {
			data["roles"] = nullptr;
		} else {
			nlohmann::json roleIdArray{};
			for (auto& value: dataPackage.roleIds) {
				roleIdArray.push_back(value);
			}
			data["roles"] = roleIdArray;
		}
		if (dataPackage.newVoiceChannelId != 0) {
			data["channel_id"] = std::to_string(dataPackage.newVoiceChannelId);
			data["mute"] = dataPackage.mute;
			data["deaf"] = dataPackage.deaf;
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::FollowNewsChannelData& dataPackage) {
		nlohmann::json data{};
		data["webhook_channel_id"] = std::to_string(dataPackage.targetChannelId);
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildEmojiData& dataPackage) {
		nlohmann::json data{};
		nlohmann::json rolesArray{};
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}
		data["image"] = dataPackage.imageDataFinal;
		data["name"] = dataPackage.name;
		data["roles"] = rolesArray;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData& dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		nlohmann::json data{};
		data["permissions"] = dataPackage.permissions.getCurrentPermissionString();
		data["mentionable"] = dataPackage.mentionable;
		data["hoist"] = dataPackage.hoist;
		data["name"] = dataPackage.name;
		data["color"] = roleColorReal;
		if (dataPackage.icon.size() > 0) {
			data["icon"] = dataPackage.icon;
		}
		if (dataPackage.unicodeEmoji.size() > 0) {
			data["unicode_emoji"] = dataPackage.unicodeEmoji;
		}
		return data.dump();
	};

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildEmojiData& dataPackage) {
		nlohmann::json data{};
		nlohmann::json rolesArray{};
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}
		data["name"] = dataPackage.name;
		data["roles"] = rolesArray;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData& dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();
		nlohmann::json data{};
		data["permissions"] = dataPackage.permissions.getCurrentPermissionString();
		data["mentionable"] = dataPackage.mentionable;
		data["hoist"] = dataPackage.hoist;
		data["name"] = dataPackage.name;
		data["color"] = roleColorReal;
		if (dataPackage.icon.size() > 0) {
			data["icon"] = dataPackage.icon;
		}
		if (dataPackage.unicodeEmoji.size() > 0) {
			data["unicode_emoji"] = dataPackage.unicodeEmoji;
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::BeginGuildPruneData& dataPackage) {
		nlohmann::json data{};
		data["compute_prune_count"] = dataPackage.computePruneCount;
		data["include_roles"] = dataPackage.includeRoles;
		data["days"] = dataPackage.days;
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildBanData& dataPackage) {
		nlohmann::json data{};
		if (dataPackage.deleteMessageDays != 0) {
			data["delete_message_days"] = dataPackage.deleteMessageDays;
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::AddGuildMemberData& dataPackage) {
		nlohmann::json data{};
		data["access_token"] = dataPackage.accessToken;
		data["roles"] = dataPackage.roles;
		data["deaf"] = dataPackage.deaf;
		data["mute"] = dataPackage.mute;
		data["nick"] = dataPackage.nick;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}
		if (dataPackage.components.size() == 0) {
			data["components"] = nlohmann::json::array();
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}
		data["allowed_mentions"] = dataPackage.allowedMentions;
		if (dataPackage.embeds.size() == 0) {
			data["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: dataPackage.embeds) {
				data["embeds"].push_back(value);
			}
		}
		if (dataPackage.avatarUrl != "") {
			data["avatar_url"] = dataPackage.userName;
		}
		if (dataPackage.userName != "") {
			data["userName"] = dataPackage.userName;
		}
		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}
		data["flags"] = dataPackage.flags;
		data["tts"] = dataPackage.tts;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData& dataPackage) {
		nlohmann::json permOws{};
		for (auto& [key, value]: dataPackage.channelData.permissionOverwrites) {
			nlohmann::json newData{};
			newData["allow"] = value.allow.getCurrentPermissionString();
			newData["deny"] = value.deny.getCurrentPermissionString();
			newData["channel_id"] = value.channelId;
			newData["type"] = value.type;
			newData["id"] = value.id;
			permOws.push_back(newData);
		}
		nlohmann::json data{};
		data["default_auto_archive_duration"] = dataPackage.channelData.defaultAutoArchiveDuration;
		data["video_quality_mode"] = dataPackage.channelData.videoQualityMode;
		data["rate_limit_per_user"] = dataPackage.channelData.rateLimitPerUser;
		data["user_limit"] = dataPackage.channelData.userLimit;
		data["rtc_region"] = std::string{ dataPackage.channelData.rtcRgion };
		data["parent_id"] = std::string{ dataPackage.channelData.parentId };
		data["position"] = dataPackage.channelData.position;
		data["bitrate"] = dataPackage.channelData.bitrate;
		data["topic"] = std::string{ dataPackage.channelData.topic };
		data["nsfw"] = dataPackage.channelData.nsfw;
		data["name"] = std::string{ dataPackage.channelData.name };
		data["type"] = dataPackage.channelData.type;
		data["permission_overwrites"] = permOws;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}
		if (dataPackage.messageReference.messageId != 0) {
			data["message_reference"] = dataPackage.messageReference;
		}
		if (dataPackage.components.size() == 0) {
			data["components"] = nlohmann::json::array();
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}
		data["allowed_mentions"] = dataPackage.allowedMentions;
		for (auto& value: dataPackage.stickerIds) {
			data["sticker_ids"].push_back(value);
		}
		if (dataPackage.embeds.size() == 0) {
			data["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: dataPackage.embeds) {
				data["embeds"].push_back(value);
			}
		}
		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}
		data["flags"] = dataPackage.flags;
		data["tts"] = dataPackage.tts;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditMessageData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}
		if (dataPackage.components.size() == 0) {
			data["components"] = nlohmann::json::array();
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}
		data["allowed_mentions"] = dataPackage.allowedMentions;
		if (dataPackage.embeds.size() == 0) {
			data["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: dataPackage.embeds) {
				data["embeds"].push_back(value);
			}
		}
		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}
		data["flags"] = dataPackage.flags;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData& dataPackage) {
		nlohmann::json data{};
		data["default_message_notifications"] = dataPackage.defaultMessageNotifications;
		data["explicit_content_filter"] = dataPackage.explicitContentFilter;
		data["system_channel_flags"] = dataPackage.systemChannelFlags;
		data["verification_level"] = dataPackage.verificationLevel;
		data["afk_timeout"] = dataPackage.afkTimeout;
		data["region"] = dataPackage.region;
		data["name"] = dataPackage.name;
		data["icon"] = dataPackage.icon;
		for (auto& value: dataPackage.channels) {
			nlohmann::json newData{};
			newData["parent_id"] = std::to_string(value.parentId);
			newData["name"] = std::string{ value.name };
			newData["id"] = value.id;
			newData["type"] = value.type;
			data["channels"].push_back(newData);
		}
		for (auto& value: dataPackage.roles) {
			nlohmann::json newData{};
			newData["permissions"] = value.permissions.getCurrentPermissionString();
			newData["tags"]["premium_subscriber"] = value.tags.premiumSubscriber;
			newData["tags"]["integration_id"] = value.tags.integrationId;
			newData["mentionable"] = DiscordCoreAPI::getBool<int8_t, DiscordCoreAPI::RoleFlags>(value.flags, DiscordCoreAPI::RoleFlags::Mentionable);
			newData["tags"]["bot_id"] = value.tags.botId;
			newData["managed"] = DiscordCoreAPI::getBool<int8_t, DiscordCoreAPI::RoleFlags>(value.flags, DiscordCoreAPI::RoleFlags::Managed);
			newData["position"] = value.position;
			newData["hoist"] = DiscordCoreAPI::getBool<int8_t, DiscordCoreAPI::RoleFlags>(value.flags, DiscordCoreAPI::RoleFlags::Hoist);
			newData["color"] = value.color;
			newData["name"] = value.name;
			data["roles"].push_back(newData);
		}
		if (dataPackage.systemChannelId != "") {
			data["system_channel_id"] = dataPackage.systemChannelId;
		}
		if (dataPackage.afkChannelId != "") {
			data["afk_channel_id"] = dataPackage.afkChannelId;
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildData& dataPackage) {
		nlohmann::json data{};
		data["premium_progress_bar_enabled"] = dataPackage.premiumProgressBarEnabled;
		data["default_message_notifications"] = dataPackage.defaultMessageNotifications;
		data["explicit_content_filter"] = dataPackage.explicitContentFilter;
		data["system_channel_flags"] = dataPackage.systemChannelFlags;
		data["verification_level"] = dataPackage.verificationLevel;
		data["discovery_splash"] = dataPackage.discoverySplash;
		data["preferred_locale"] = std::string{ dataPackage.preferredLocale };
		data["rules_channel_id"] == std::string{ dataPackage.rulesChannelId };
		data["description"] = std::string{ dataPackage.description };
		data["afk_timeout"] = dataPackage.afkTimeout;
		data["features"] = dataPackage.features;
		data["splash"] = dataPackage.splash;
		data["banner"] = dataPackage.banner;
		data["name"] = std::string{ dataPackage.name };
		data["icon"] = dataPackage.icon;
		if (dataPackage.publicUpdatesChannelId != "") {
			data["public_updates_channel_id"] = std::string{ dataPackage.publicUpdatesChannelId };
		}
		if (dataPackage.afkChannelId != "") {
			data["afk_channel_id"] = std::string{ dataPackage.afkChannelId };
		}
		if (dataPackage.systemChannelId != "") {
			data["system_channel_id"] = std::string{ dataPackage.systemChannelId };
		}
		if (dataPackage.ownerId != "") {
			data["owner_id"] = std::string{ dataPackage.ownerId };
		}
		return data.dump();
	}

	std::string JSONIFY(const DiscordCoreAPI::EditWebHookData& dataPackage) {
		nlohmann::json data{};
		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}
		if (dataPackage.components.size() == 0) {
			data["components"] = nlohmann::json::array();
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}
		data["allowed_mentions"] = dataPackage.allowedMentions;
		if (dataPackage.embeds.size() == 0) {
			data["embeds"] = nlohmann::json::array();
		} else {
			for (auto& value: dataPackage.embeds) {
				data["embeds"].push_back(value);
			}
		}
		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}
		return data.dump();
	}
}