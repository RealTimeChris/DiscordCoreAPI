/*
*
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
/// Chris M.
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
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>

namespace DiscordCoreAPI {

	void to_json(nlohmann::json& jsonOut, const MessageReferenceData& inputValue) {
		nlohmann::json returnValue{};
		returnValue["fail_if_not_exists"] = inputValue.failIfNotExists;
		returnValue["message_id"] = inputValue.messageId;
		returnValue["channel_id"] = inputValue.channelId;
		returnValue["guild_id"] = inputValue.guildId;
		jsonOut = returnValue;
	}

	void to_json(nlohmann::json& jsonOut, const AllowedMentionsData& inputValue) {
		nlohmann::json returnValue{};
		returnValue["replied_user"] = inputValue.repliedUser;
		returnValue["parse"] = inputValue.parse;
		returnValue["roles"] = inputValue.roles;
		returnValue["users"] = inputValue.users;
		jsonOut = returnValue;
	}

	void to_json(nlohmann::json& jsonOut, const AttachmentData& valueNew) {
		nlohmann::json returnValue{};
		returnValue["content_type"] = valueNew.contentType;
		returnValue["description"] = valueNew.description;
		returnValue["ephemeral"] = valueNew.ephemeral;
		returnValue["file_name"] = valueNew.filename;
		returnValue["proxy_url"] = valueNew.proxyUrl;
		returnValue["height"] = valueNew.height;
		returnValue["width"] = valueNew.width;
		returnValue["size"] = valueNew.size;
		returnValue["url"] = valueNew.url;
		jsonOut = returnValue;
	}

	void to_json(nlohmann::json& jsonOut, const EmbedFieldData& inputValue) {
		nlohmann::json returnValue{};
		returnValue["inline"] = inputValue.Inline;
		returnValue["value"] = inputValue.value;
		returnValue["name"] = inputValue.name;
		jsonOut.update(returnValue);
	}

	void to_json(nlohmann::json& jsonOut, const std::vector<ActionRowData>& dataPackage) {
		for (auto& value: dataPackage) {
			nlohmann::json components{};
			for (auto& inputValue: value.components) {
				if (inputValue.type == ComponentType::Button) {
					nlohmann::json component{};
					component["emoji"]["animated"] = inputValue.emoji.animated;
					component["emoji"]["name"] = inputValue.emoji.name;
					if (inputValue.emoji.id != "") {
						component["emoji"]["id"] = inputValue.emoji.id;
					}
					component["custom_id"] = inputValue.customId;
					component["disabled"] = inputValue.disabled;
					component["label"] = inputValue.label;
					component["style"] = inputValue.style;
					component["type"] = inputValue.type;
					component["url"] = inputValue.url;
					components.push_back(component);
				} else if (inputValue.type == ComponentType::SelectMenu) {
					nlohmann::json optionsArray{};
					for (auto& valueNew01: inputValue.options) {
						nlohmann::json option{};
						if (valueNew01.emoji.name != "") {
							option["emoji"]["name"] = valueNew01.emoji.name;
							option["emoji"]["animated"] = valueNew01.emoji.animated;
						}
						if (valueNew01.emoji.id != "") {
							option["emoji"]["id"] = valueNew01.emoji.id;
						}
						option["description"] = valueNew01.description;
						option["default"] = valueNew01._default;
						option["label"] = valueNew01.label;
						option["value"] = valueNew01.value;
						optionsArray.push_back(option);
					};

					nlohmann::json component{};
					component["placeholder"] = inputValue.placeholder;
					component["max_values"] = inputValue.maxValues;
					component["min_values"] = inputValue.minValues;
					component["custom_id"] = inputValue.customId;
					component["disabled"] = inputValue.disabled;
					component["options"] = optionsArray;
					component["type"] = inputValue.type;
					components.push_back(component);

				} else if (inputValue.type == ComponentType::TextInput) {
					nlohmann::json component{};
					component["placeholder"] = inputValue.placeholder;
					component["min_length"] = inputValue.minLength;
					component["max_length"] = inputValue.maxLength;
					component["custom_id"] = inputValue.customId;
					component["required"] = inputValue.required;
					component["style"] = inputValue.style;
					component["label"] = inputValue.label;
					component["value"] = inputValue.value;
					component["type"] = inputValue.type;
					components.push_back(component);
				}
			}
			nlohmann::json returnValue{};
			returnValue["components"] = components;
			returnValue["type"] = 1;
			jsonOut = returnValue;
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

	nlohmann::json JSONIFY(std::string serverId, std::string sessionId, std::string token, std::string) {
		nlohmann::json returnValue{};
		returnValue["d"]["server_id"] = serverId;
		returnValue["d"]["session_id"] = sessionId;
		returnValue["d"]["token"] = token;
		returnValue["op"] = 7;
		return returnValue;
	}

	nlohmann::json JSONIFY(std::string botToken, int64_t intents, int32_t currentShard, int32_t numberOfShards) {
		nlohmann::json returnValue{};
		returnValue["d"]["properties"]["$browser"] = "DiscordCoreAPI";
		returnValue["d"]["properties"]["$device"] = "DiscordCoreAPI";
		returnValue["d"]["shard"] = { currentShard, numberOfShards };
		returnValue["d"]["large_threshold"] = 250;
		returnValue["d"]["intents"] = intents;
		returnValue["d"]["compress"] = false;
		returnValue["d"]["token"] = botToken;
		returnValue["op"] = 2;
#ifdef _WIN32
		returnValue["d"]["properties"]["$os"] = "Windows";
#else
		returnValue["d"]["properties"]["$os"] = "Linux";
#endif
		return returnValue;
	};

	nlohmann::json JSONIFY(std::string botToken, std::string sessionID, int32_t lastReceivedNumber) {
		nlohmann::json returnValue{};
		returnValue["d"]["seq"] = lastReceivedNumber;
		returnValue["d"]["session_id"] = sessionID;
		returnValue["d"]["token"] = botToken;
		returnValue["op"] = 6;
		return returnValue;
	};

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.channelId == "") {
			returnValue["d"]["channel_id"] = nullptr;
		} else {
			returnValue["d"]["channel_id"] = dataPackage.channelId;
		}
		returnValue["d"]["self_deaf"] = dataPackage.selfDeaf;
		returnValue["d"]["self_mute"] = dataPackage.selfMute;
		returnValue["d"]["guild_id"] = dataPackage.guildId;
		returnValue["op"] = 4;
		return returnValue;
	}

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage) {
		nlohmann::json activitiesArray{};
		nlohmann::json returnValue{};
		returnValue["activities"] = nlohmann::json{};
		for (auto& value: dataPackage.activities) {
			nlohmann::json dataNew{};
			if (value.url != "") {
				dataNew["url"] = value.url;
			}
			dataNew["name"] = value.name;
			dataNew["type"] = value.type;
			activitiesArray.push_back(dataNew);
		}
		returnValue["d"]["activities"] = activitiesArray;
		returnValue["d"]["status"] = dataPackage.status;
		returnValue["d"]["since"] = dataPackage.since;
		returnValue["d"]["afk"] = dataPackage.afk;
		returnValue["op"] = 3;
		return returnValue;
	}

	nlohmann::json JSONIFY(int32_t lastReceivedNumber) {
		nlohmann::json returnValue{};
		returnValue["d"] = lastReceivedNumber;
		returnValue["op"] = 1;
		return returnValue;
	};

	std::vector<uint8_t> JSONIFY(std::string localPort, std::string localIp, std::string encryptionMode, int32_t) {
		nlohmann::json returnValue{};
		returnValue["d"]["data"]["port"] = stol(localPort);
		returnValue["d"]["data"]["mode"] = encryptionMode;
		returnValue["d"]["data"]["address"] = localIp;
		returnValue["d"]["protocol"] = "udp";
		returnValue["op"] = 1;
		std::vector<uint8_t> newVector{};
		std::string newString = returnValue.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	std::vector<uint8_t> JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02) {
		nlohmann::json returnValue{};
		returnValue["d"]["session_id"] = dataPackage.sessionId;
		returnValue["d"]["server_id"] = dataPackage02.guildId;
		returnValue["d"]["user_id"] = dataPackage02.userId;
		returnValue["d"]["token"] = dataPackage.token;
		returnValue["op"] = 0;
		std::vector<uint8_t> newVector{};
		std::string newString = returnValue.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	std::vector<uint8_t> JSONIFY(int32_t ssrc, int32_t delay) {
		nlohmann::json returnValue{};
		returnValue["d"]["speaking"] = 1 << 0;
		returnValue["d"]["delay"] = delay;
		returnValue["d"]["ssrc"] = ssrc;
		returnValue["op"] = 5;
		std::vector<uint8_t> newVector{};
		std::string newString = returnValue.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	std::vector<uint8_t> JSONIFY(int64_t nonce) {
		nlohmann::json returnValue{};
		returnValue["d"] = nonce;
		returnValue["op"] = int32_t(3);
		std::vector<uint8_t> newVector{};
		std::string newString = returnValue.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	void JSONIFY(DiscordCoreAPI::ApplicationCommandOptionData dataPackage, nlohmann::json* pJSONData) {
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
			for (int32_t x = 0; x < dataPackage.choices.size(); x += 1) {
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

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage) {
		nlohmann::json newDataArray{};

		for (auto& value: dataPackage.permissions) {
			nlohmann::json newValue{};
			newValue["permission"] = value.permission;
			newValue["type"] = value.type;
			newValue["id"] = value.id;
			newDataArray.push_back(newValue);
		}

		return newDataArray.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["allow"] = stoll(dataPackage.allow);
		returnValue["deny"] = stoll(dataPackage.deny);
		returnValue["type"] = dataPackage.type;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.defaultMemberPermissions != "") {
			returnValue["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		returnValue["description_localizations"] = dataPackage.descriptionLocalizations;
		returnValue["name_localizations"] = dataPackage.nameLocalizations;
		returnValue["dm_permission"] = dataPackage.dmPermission;
		returnValue["description"] = dataPackage.description;
		returnValue["name"] = dataPackage.name;
		returnValue["type"] = dataPackage.type;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &returnValue["options"]);
			}
		}
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.defaultMemberPermissions != "") {
			returnValue["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		returnValue["description_localizations"] = dataPackage.descriptionLocalizations;
		returnValue["name_localizations"] = dataPackage.nameLocalizations;
		returnValue["dm_permission"] = dataPackage.dmPermission;
		returnValue["description"] = dataPackage.description;
		returnValue["name"] = dataPackage.name;
		returnValue["type"] = dataPackage.type;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &returnValue["options"]);
			}
			returnValue["options"] = nlohmann::json{};
		}

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.defaultMemberPermissions != "") {
			returnValue["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		returnValue["description_localizations"] = dataPackage.descriptionLocalizations;
		returnValue["name_localizations"] = dataPackage.nameLocalizations;
		returnValue["dm_permission"] = dataPackage.dmPermission;
		returnValue["description"] = dataPackage.description;
		returnValue["name"] = dataPackage.name;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &returnValue["options"]);
			}
		}
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.defaultMemberPermissions != "") {
			returnValue["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		returnValue["description_localizations"] = dataPackage.descriptionLocalizations;
		returnValue["name_localizations"] = dataPackage.nameLocalizations;
		returnValue["dm_permission"] = dataPackage.dmPermission;
		returnValue["description"] = dataPackage.description;
		returnValue["name"] = dataPackage.name;
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &returnValue["options"]);
			}
			returnValue["options"] = nlohmann::json{};
		}

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.modifyChannelData) {
			nlohmann::json dataNew{};
			dataNew["lock_permissions"] = value.lockPermissions;
			if (value.parentId != "") {
				dataNew["parent_id"] = value.parentId;
			}
			dataNew["position"] = value.position;
			dataNew["id"] = value.id;
			returnValue.push_back(dataNew);
		}

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		returnValue["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		returnValue["invitable"] = dataPackage.invitable;
		returnValue["name"] = dataPackage.threadName;
		returnValue["type"] = dataPackage.type;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadInForumChannelData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.message.attachments) {
			returnValue["message"]["attachments"].push_back(value);
		}

		if (dataPackage.message.components.size() == 0) {
			returnValue["message"]["components"] = nlohmann::json{};
		} else {
			returnValue["message"]["components"] = nlohmann::json{ dataPackage.message.components };
		}

		returnValue["message"]["allowed_mentions"] = dataPackage.message.allowedMentions;

		for (auto& value: dataPackage.message.stickerIds) {
			returnValue["message"]["sticker_ids"].push_back(value);
		}

		if (dataPackage.message.embeds.size() == 0) {
			returnValue["message"]["embeds"] = nlohmann::json{};
		} else {
			for (auto& value: dataPackage.message.embeds) {
				returnValue["message"]["embeds"].push_back(value);
			}
		}

		if (dataPackage.message.content != "") {
			returnValue["message"]["content"] = dataPackage.message.content;
		}

		returnValue["message"]["flags"] = dataPackage.message.flags;

		returnValue["name"] = dataPackage.name;
		returnValue["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		returnValue["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildScheduledEventData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			returnValue["channel_id"] = nullptr;
		} else {
			returnValue["channel_id"] = dataPackage.channelId;
		}
		returnValue["entity_metadata"]["location"] = dataPackage.entityMetadata.location;
		returnValue["entity_metadata"]["entity_type"] = dataPackage.entityType;
		returnValue["scheduled_start_time"] = dataPackage.scheduledStartTime;
		returnValue["scheduled_end_time"] = dataPackage.scheduledEndTime;
		returnValue["privacy_level"] = dataPackage.privacyLevel;
		returnValue["description"] = dataPackage.description;
		returnValue["name"] = dataPackage.name;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage) {
		nlohmann::json dataArray{};

		for (auto& value: dataPackage.rolePositions) {
			nlohmann::json returnValue{};
			returnValue["position"] = value.rolePosition;
			returnValue["id"] = value.roleId;
			dataArray.push_back(returnValue);
		}
		return dataArray.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			returnValue["channel_id"] = nullptr;
		} else {
			returnValue["channel_id"] = dataPackage.channelId;
		}
		returnValue["entity_metadata"]["location"] = dataPackage.entityMetadata.location;
		returnValue["entity_metadata"]["entity_type"] = dataPackage.entityType;
		returnValue["scheduled_start_time"] = dataPackage.scheduledStartTime;
		returnValue["scheduled_end_time"] = dataPackage.scheduledEndTime;
		returnValue["privacy_level"] = dataPackage.privacyLevel;
		returnValue["description"] = dataPackage.description;
		returnValue["name"] = dataPackage.name;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage) {
		nlohmann::json channelsArray{};

		for (auto& value: dataPackage.welcomeChannels) {
			nlohmann::json newValue{};
			newValue["description"] = value.description;
			newValue["channel_id"] = value.channelId;
			newValue["emoji_name"] = value.emojiName;
			if (value.emojiId != "") {
				newValue["emoji_id"] = value.emojiId;
			}
			channelsArray.push_back(newValue);
		}

		nlohmann::json returnValue{};
		returnValue["description"] = dataPackage.description;
		returnValue["welcome_channels"] = channelsArray;
		returnValue["enabled"] = dataPackage.enabled;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		returnValue["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		returnValue["name"] = dataPackage.threadName;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["access_token"] = dataPackage.token;
		returnValue["nick"] = dataPackage.nick;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::InteractionResponseData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.data.attachments) {
			returnValue["data"]["attachments"].push_back(value);
		}

		if (dataPackage.data.components.size() == 0) {
			returnValue["data"]["components"] = nlohmann::json{};
		} else {
			returnValue["data"]["components"] = nlohmann::json{ dataPackage.data.components };
		}

		returnValue["data"]["allowed_mentions"] = dataPackage.data.allowedMentions;

		if (dataPackage.data.choices.size() > 0) {
			nlohmann::json::array_t theArray{};
			for (auto& value: dataPackage.data.choices) {
				nlohmann::json theValue{};
				theValue["name"] = value.name;
				theValue["name_localizations"] = value.nameLocalizations;
				theValue["value"] = value.value;
				theArray.push_back(theValue);
			}
			returnValue["data"]["choices"] = theArray;
		}

		if (dataPackage.data.embeds.size() == 0) {
			returnValue["data"]["embeds"] = nlohmann::json{};
		} else {
			for (auto& value: dataPackage.data.embeds) {
				returnValue["data"]["embeds"].push_back(value);
			}
		}

		if (dataPackage.data.customId != "") {
			returnValue["data"]["custom_id"] = dataPackage.data.customId;
		}

		if (dataPackage.data.content != "") {
			returnValue["data"]["content"] = dataPackage.data.content;
		}

		if (dataPackage.data.title != "") {
			returnValue["data"]["title"] = dataPackage.data.title;
		}

		returnValue["data"]["flags"] = dataPackage.data.flags;

		returnValue["data"]["tts"] = dataPackage.data.tts;

		returnValue["type"] = dataPackage.type;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.targetUserId != "") {
			returnValue["target_application_id"] = dataPackage.targetApplicationId;
			returnValue["target_user_id"] = dataPackage.targetUserId;
			returnValue["target_type"] = dataPackage.targetType;
		}
		returnValue["temporary"] = dataPackage.temporary;
		returnValue["max_uses"] = dataPackage.maxUses;
		returnValue["max_age"] = dataPackage.maxAge;
		returnValue["unique"] = dataPackage.unique;
		return returnValue.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["messages"] = dataPackage.messageIds;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Voice || dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			returnValue["user_limit"] = dataPackage.userLimit;
			returnValue["bitrate"] = dataPackage.bitrate;
		}
		nlohmann::json overwrites{};
		for (auto& value: dataPackage.permissionOverwrites) {
			nlohmann::json newValue{};
			newValue["allow"] = value.allow.getCurrentPermissionString();
			newValue["deny"] = value.deny.getCurrentPermissionString();
			newValue["channel_id"] = value.channelId;
			newValue["type"] = value.type;
			newValue["id"] = value.id;
			overwrites.push_back(newValue);
		}
		returnValue["default_auto_archive_duration"] = dataPackage.defaultAutoArchiveDuration;
		returnValue["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		returnValue["permission_overwrites"] = overwrites;
		returnValue["parent_id"] = dataPackage.parentId;
		returnValue["position"] = dataPackage.position;
		returnValue["topic"] = dataPackage.topic;
		returnValue["name"] = dataPackage.name;
		returnValue["nsfw"] = dataPackage.nsfw;
		returnValue["type"] = dataPackage.type;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["nick"] = dataPackage.nick;
		returnValue["communication_disabled_until"] = std::string(dataPackage.communicationDisabledUntil);

		if (dataPackage.roleIds.size() == 0) {
			returnValue["roles"] = nullptr;
		} else {
			nlohmann::json roleIdArray{};
			for (auto& value: dataPackage.roleIds) {
				roleIdArray.push_back(value);
			}
			returnValue["roles"] = roleIdArray;
		}

		if (dataPackage.newVoiceChannelId != "") {
			returnValue["channel_id"] = dataPackage.newVoiceChannelId;
			returnValue["mute"] = dataPackage.mute;
			returnValue["deaf"] = dataPackage.deaf;
		}
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["webhook_channel_id"] = dataPackage.targetChannelId;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage) {
		nlohmann::json returnValue{};

		nlohmann::json rolesArray{};
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}
		returnValue["image"] = dataPackage.imageDataFinal;
		returnValue["name"] = dataPackage.name;
		returnValue["roles"] = rolesArray;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream{};
		stream << std::setbase(10) << roleColorInt;
		std::string colorReal = stream.str();

		nlohmann::json returnValue{};
		returnValue["permissions"] = dataPackage.permissions.getCurrentPermissionString();
		returnValue["mentionable"] = dataPackage.mentionable;
		returnValue["hoist"] = dataPackage.hoist;
		returnValue["name"] = dataPackage.name;
		returnValue["color"] = colorReal;
		if (dataPackage.icon.size() > 0) {
			returnValue["icon"] = dataPackage.icon;
		}
		if (dataPackage.unicodeEmoji.size() > 0) {
			returnValue["unicode_emoji"] = dataPackage.unicodeEmoji;
		}
		return returnValue.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage) {
		nlohmann::json returnValue{};

		nlohmann::json rolesArray{};
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}

		returnValue["name"] = dataPackage.name;
		returnValue["roles"] = rolesArray;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream{};
		stream << std::setbase(10) << roleColorInt;
		std::string colorReal = stream.str();

		nlohmann::json returnValue{};
		returnValue["permissions"] = dataPackage.permissions.getCurrentPermissionString();
		returnValue["mentionable"] = dataPackage.mentionable;
		returnValue["hoist"] = dataPackage.hoist;
		returnValue["name"] = dataPackage.name;
		returnValue["color"] = colorReal;
		if (dataPackage.icon.size() > 0) {
			returnValue["icon"] = dataPackage.icon;
		}
		if (dataPackage.unicodeEmoji.size() > 0) {
			returnValue["unicode_emoji"] = dataPackage.unicodeEmoji;
		}
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::BeginGuildPruneData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["compute_prune_count"] = dataPackage.computePruneCount;
		returnValue["include_roles"] = dataPackage.includeRoles;
		returnValue["days"] = dataPackage.days;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage) {
		nlohmann::json returnValue{};
		if (dataPackage.deleteMessageDays != 0) {
			returnValue["delete_message_days"] = dataPackage.deleteMessageDays;
		}
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage) {
		nlohmann::json returnValue{};

		returnValue["access_token"] = dataPackage.accessToken;
		returnValue["roles"] = dataPackage.roles;
		returnValue["deaf"] = dataPackage.deaf;
		returnValue["mute"] = dataPackage.mute;
		returnValue["nick"] = dataPackage.nick;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.attachments) {
			returnValue["attachments"].push_back(value);
		}

		if (dataPackage.components.size() == 0) {
			returnValue["components"] = nlohmann::json{};
		} else {
			returnValue["components"] = nlohmann::json{ dataPackage.components };
		}

		returnValue["allowed_mentions"] = dataPackage.allowedMentions;

		if (dataPackage.embeds.size() == 0) {
			returnValue["embeds"] = nlohmann::json{};
		} else {
			for (auto& value: dataPackage.embeds) {
				returnValue["embeds"].push_back(value);
			}
		}

		if (dataPackage.avatarUrl != "") {
			returnValue["avatar_url"] = dataPackage.username;
		}

		if (dataPackage.username != "") {
			returnValue["username"] = dataPackage.username;
		}

		if (dataPackage.content != "") {
			returnValue["content"] = dataPackage.content;
		}

		returnValue["flags"] = dataPackage.flags;

		returnValue["tts"] = dataPackage.tts;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData dataPackage) {
		nlohmann::json permOws{};
		for (auto& [key, value]: dataPackage.channelData.permissionOverwrites) {
			nlohmann::json newValue{};
			newValue["allow"] = value.allow.getCurrentPermissionString();
			newValue["deny"] = value.deny.getCurrentPermissionString();
			newValue["channel_id"] = value.channelId;
			newValue["type"] = value.type;
			newValue["id"] = value.id;
			permOws.push_back(newValue);
		}

		nlohmann::json returnValue{};
		returnValue["default_auto_archive_duration"] = dataPackage.channelData.defaultAutoArchiveDuration;
		returnValue["video_quality_mode"] = dataPackage.channelData.videoQualityMode;
		returnValue["rate_limit_per_user"] = dataPackage.channelData.rateLimitPerUser;
		returnValue["user_limit"] = dataPackage.channelData.userLimit;
		returnValue["rtc_region"] = dataPackage.channelData.rtcRgion;
		returnValue["parent_id"] = dataPackage.channelData.parentId;
		returnValue["parent_id"] = dataPackage.channelData.parentId;
		returnValue["position"] = dataPackage.channelData.position;
		returnValue["bitrate"] = dataPackage.channelData.bitrate;
		returnValue["topic"] = dataPackage.channelData.topic;
		returnValue["nsfw"] = dataPackage.channelData.nsfw;
		returnValue["name"] = dataPackage.channelData.name;
		returnValue["type"] = dataPackage.channelData.type;
		returnValue["permission_overwrites"] = permOws;
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.attachments) {
			returnValue["attachments"].push_back(value);
		}

		if (dataPackage.messageReference.messageId != "") {
			returnValue["message_reference"] = dataPackage.messageReference;
		}

		if (dataPackage.components.size() == 0) {
			returnValue["components"] = nlohmann::json{};
		} else {
			returnValue["components"] = nlohmann::json{ dataPackage.components };
		}

		returnValue["allowed_mentions"] = dataPackage.allowedMentions;

		for (auto& value: dataPackage.stickerIds) {
			returnValue["sticker_ids"].push_back(value);
		}

		if (dataPackage.embeds.size() == 0) {
			returnValue["embeds"] = nlohmann::json{};
		} else {
			for (auto& value: dataPackage.embeds) {
				returnValue["embeds"].push_back(value);
			}
		}

		if (dataPackage.content != "") {
			returnValue["content"] = dataPackage.content;
		}

		returnValue["flags"] = dataPackage.flags;

		returnValue["tts"] = dataPackage.tts;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.attachments) {
			returnValue["attachments"].push_back(value);
		}

		if (dataPackage.components.size() == 0) {
			returnValue["components"] = nlohmann::json{};
		} else {
			returnValue["components"] = nlohmann::json{ dataPackage.components };
		}

		returnValue["allowed_mentions"] = dataPackage.allowedMentions;

		if (dataPackage.embeds.size() == 0) {
			returnValue["embeds"] = nlohmann::json{};
		} else {
			for (auto& value: dataPackage.embeds) {
				returnValue["embeds"].push_back(value);
			}
		}

		if (dataPackage.content != "") {
			returnValue["content"] = dataPackage.content;
		}

		returnValue["flags"] = dataPackage.flags;

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["default_message_notifications"] = dataPackage.defaultMessageNotifications;
		returnValue["explicit_content_filter"] = dataPackage.explicitContentFilter;
		returnValue["system_channel_flags"] = dataPackage.systemChannelFlags;
		returnValue["verification_level"] = dataPackage.verificationLevel;
		returnValue["afk_timeout"] = dataPackage.afkTimeout;
		returnValue["region"] = dataPackage.region;
		returnValue["name"] = dataPackage.name;
		returnValue["icon"] = dataPackage.icon;

		for (auto& value: dataPackage.channels) {
			nlohmann::json newValue{};
			newValue["parent_id"] = value.parentId;
			newValue["name"] = value.name;
			newValue["type"] = value.type;
			newValue["id"] = value.id;
			returnValue["channels"].push_back(newValue);
		}

		for (auto& value: dataPackage.roles) {
			nlohmann::json newValue{};
			newValue["permissions"] = value.permissions.getCurrentPermissionString();
			newValue["tags"]["premium_subscriber"] = value.tags.premiumSubscriber;
			newValue["tags"]["integration_id"] = value.tags.integrationId;
			newValue["mentionable"] = value.getMentionable();
			newValue["tags"]["bot_id"] = value.tags.botId;
			newValue["managed"] = value.getManaged();
			newValue["position"] = value.position;
			newValue["hoist"] = value.getHoist();
			newValue["color"] = value.color;
			newValue["name"] = value.name;
			returnValue["roles"].push_back(newValue);
		}
		if (dataPackage.systemChannelId != "") {
			returnValue["system_channel_id"] = dataPackage.systemChannelId;
		}
		if (dataPackage.afkChannelId != "") {
			returnValue["afk_channel_id"] = dataPackage.afkChannelId;
		}
		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage) {
		nlohmann::json returnValue{};
		returnValue["premium_progress_bar_enabled"] = dataPackage.premiumProgressBarEnabled;
		returnValue["default_message_notifications"] = dataPackage.defaultMessageNotifications;
		returnValue["explicit_content_filter"] = dataPackage.explicitContentFilter;
		returnValue["system_channel_flags"] = dataPackage.systemChannelFlags;
		returnValue["verification_level"] = dataPackage.verificationLevel;
		returnValue["discovery_splash"] = dataPackage.discoverySplash;
		returnValue["preferred_locale"] = dataPackage.preferredLocale;
		returnValue["rules_channel_id"] = dataPackage.rulesChannelId;
		returnValue["description"] = dataPackage.description;
		returnValue["afk_timeout"] = dataPackage.afkTimeout;
		returnValue["features"] = dataPackage.features;
		returnValue["splash"] = dataPackage.splash;
		returnValue["banner"] = dataPackage.banner;
		returnValue["name"] = dataPackage.name;
		returnValue["icon"] = dataPackage.icon;

		if (dataPackage.publicUpdatesChannelId != "") {
			returnValue["public_updates_channel_id"] = dataPackage.publicUpdatesChannelId;
		}
		if (dataPackage.afkChannelId != "") {
			returnValue["afk_channel_id"] = dataPackage.afkChannelId;
		}
		if (dataPackage.systemChannelId != "") {
			returnValue["system_channel_id"] = dataPackage.systemChannelId;
		}
		if (dataPackage.ownerId != "") {
			returnValue["owner_id"] = dataPackage.ownerId;
		}

		return returnValue.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditWebHookData dataPackage) {
		nlohmann::json returnValue{};

		for (auto& value: dataPackage.attachments) {
			returnValue["attachments"].push_back(value);
		}

		if (dataPackage.components.size() == 0) {
			returnValue["components"] = nlohmann::json{};
		} else {
			returnValue["components"] = nlohmann::json{ dataPackage.components };
		}

		returnValue["allowed_mentions"] = dataPackage.allowedMentions;

		if (dataPackage.embeds.size() == 0) {
			returnValue["embeds"] = nlohmann::json{};
		} else {
			for (auto& value: dataPackage.embeds) {
				returnValue["embeds"].push_back(value);
			}
		}

		if (dataPackage.content != "") {
			returnValue["content"] = dataPackage.content;
		}

		return returnValue.dump();
	}

}