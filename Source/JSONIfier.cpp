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
	
	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::MessageReferenceData& valueIn) {
		nlohmann::json newValue{};
		newValue["fail_if_not_exists"] = valueIn.failIfNotExists;
		newValue["message_id"] = valueIn.messageId;
		newValue["channel_id"] = valueIn.channelId;
		newValue["guild_id"] = valueIn.guildId;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AllowedMentionsData& valueIn) {
		nlohmann::json newValue{};
		newValue["replied_user"] = valueIn.repliedUser;
		newValue["parse"] = valueIn.parse;
		newValue["roles"] = valueIn.roles;
		newValue["users"] = valueIn.users;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedData& valueIn) {
		auto fields = nlohmann::json::array();

		for (auto& value2: valueIn.fields) {
			nlohmann::json field{ value2 };
			fields.push_back(field);
		}

		int32_t colorValInt = stol(valueIn.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << colorValInt;
		std::string realColorVal = stream.str();

		nlohmann::json embed{};
		embed["author"]["icon_url"] = valueIn.author.iconUrl;
		embed["author"]["name"] = valueIn.author.name;
		embed["author"]["url"] = valueIn.author.url;
		embed["author"]["proxy_icon_url"] = valueIn.author.proxyIconUrl;
		embed["image"]["height"] = valueIn.image.height;
		embed["image"]["width"] = valueIn.image.width;
		embed["image"]["url"] = valueIn.image.url;
		embed["image"]["proxy_url"] = valueIn.image.proxyUrl;
		embed["provider"]["name"] = valueIn.provider.name;
		embed["provider"]["url"] = valueIn.provider.url;
		embed["thumbnail"]["height"] = valueIn.thumbnail.height;
		embed["thumbnail"]["width"] = valueIn.thumbnail.width;
		embed["thumbnail"]["url"] = valueIn.thumbnail.url;
		embed["thumbnail"]["proxy_url"] = valueIn.thumbnail.proxyUrl;
		embed["footer"]["icon_url"] = valueIn.footer.iconUrl;
		embed["thumbnail"]["proxy_icon_url"] = valueIn.footer.proxyIconUrl;
		embed["thumbnail"]["text"] = valueIn.footer.text;
		embed["description"] = valueIn.description;
		embed["title"] = valueIn.title;
		embed["fields"] = fields;
		embed["color"] = realColorVal;
		embed["timestamp"] = valueIn.timestamp;
		jsonOut = embed;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedFieldData& valueIn) {
		nlohmann::json newValue{};
		newValue["inline"] = valueIn.Inline;
		newValue["value"] = valueIn.value;
		newValue["name"] = valueIn.name;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AttachmentData& valueIn) {
		nlohmann::json newValue{};
		newValue["content_type"] = valueIn.contentType;
		newValue["description"] = valueIn.description;
		newValue["file_name"] = valueIn.filename;
		newValue["proxy_url"] = valueIn.proxyUrl;
		newValue["ephemeral"] = valueIn.ephemeral;
		newValue["url"] = valueIn.url;
		newValue["height"] = valueIn.height;
		newValue["width"] = valueIn.width;
		newValue["size"] = valueIn.size;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::ComponentData& valueIn) {
		if (valueIn.type == DiscordCoreAPI::ComponentType::Button) {
			nlohmann::json component{};
			component["custom_id"] = valueIn.customId;
			component["disabled"] = valueIn.disabled;
			component["emoji"]["name"] = valueIn.emoji.name;
			component["emoji"]["animated"] = valueIn.emoji.animated;
			component["label"] = valueIn.label;
			component["style"] = valueIn.style;
			component["type"] = valueIn.type;
			component["url"] = valueIn.url;
			if (valueIn.emoji.id != "") {
				component["emoji"]["id"] = valueIn.emoji.id;
			}
			jsonOut = component;
		} else if (valueIn.type == DiscordCoreAPI::ComponentType::SelectMenu) {
			nlohmann::json optionsArray = nlohmann::json::array();
			for (auto& value01: valueIn.options) {
				nlohmann::json option{};
				option["description"] = value01.description;
				option["label"] = value01.label;
				option["value"] = value01.value;
				option["default"] = value01._default;
				if (value01.emoji.name != "") {
					option["emoji"]["name"] = value01.emoji.name;
					option["emoji"]["animated"] = value01.emoji.animated;
				}
				if (value01.emoji.id != "") {
					option["emoji"]["id"] = value01.emoji.id;
				}
				optionsArray.push_back(option);
			};
		
			nlohmann::json component{};
			component["custom_id"] = valueIn.customId;
			component["options"] = optionsArray;
			component["disabled"] = valueIn.disabled;
			component["type"] = valueIn.type;
			component["max_values"] = valueIn.maxValues;
			component["min_values"] = valueIn.minValues;
			component["placeholder"] = valueIn.placeholder;
			jsonOut = component;
		
		} else if (valueIn.type == DiscordCoreAPI::ComponentType::TextInput) {
			nlohmann::json component{};
			component["min_length"] = valueIn.minLength;
			component["max_length"] = valueIn.maxLength;
			component["label"] = valueIn.label;
			component["placeholder"] = valueIn.placeholder;
			component["type"] = valueIn.type;
			component["custom_id"] = valueIn.customId;
			component["style"] = valueIn.style;
			component["required"] = valueIn.required;
			jsonOut = component;
		}
	}
}

namespace DiscordCoreInternal {

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData dataPackage) {
		auto permOws = nlohmann::json::array();
		for (auto& [key, value]: dataPackage.channelData.permissionOverwrites) {
			nlohmann::json newData{};
			newData["type"] = value.type;
			newData["id"] = value.id;
			newData["deny"] = value.deny.getCurrentPermissionString();
			newData["allow"] = value.allow.getCurrentPermissionString();
			newData["channel_id"] = value.channelId;
			permOws.push_back(newData);
		}

		nlohmann::json data{};
		data["permission_overwrites"] = permOws;
		data["video_quality_mode"] = dataPackage.channelData.videoQualityMode;
		data["rate_limit_per_user"] = dataPackage.channelData.rateLimitPerUser;
		data["user_limit"] = dataPackage.channelData.userLimit;
		data["position"] = dataPackage.channelData.position;
		data["parent_id"] = dataPackage.channelData.parentId;
		data["bitrate"] = dataPackage.channelData.bitrate;
		data["topic"] = dataPackage.channelData.topic;
		data["nsfw"] = dataPackage.channelData.nsfw;
		data["name"] = dataPackage.channelData.name;
		data["parent_id"] = dataPackage.channelData.parentId;
		data["rtc_region"] = dataPackage.channelData.rtcRgion;
		data["type"] = dataPackage.channelData.type;
		data["default_auto_archive_duration"] = dataPackage.channelData.defaultAutoArchiveDuration;

		return data.dump();
	}

	nlohmann::json JSONIFY(std::string botToken, int32_t intents, int32_t currentShard, int32_t numberOfShards) {
#ifdef _WIN32
		nlohmann::json data{};
		data["op"] = 2;
		data["d"]["token"] = botToken;
		data["d"]["properties"]["$os"] = "Windows";
		data["d"]["properties"]["$browser"] = "DiscordCoreAPI";
		data["d"]["properties"]["$device"] = "DiscordCoreAPI";
		data["d"]["compress"] = false;
		data["d"]["large_threshold"] = 250;
		data["d"]["shard"] = { currentShard, numberOfShards };
		data["d"]["intents"] = intents;
#else
		nlohmann::json data{};
		data["op"] = 2;
		data["d"]["token"] = botToken;
		data["d"]["properties"]["$os"] = "Linux";
		data["d"]["properties"]["$browser"] = "DiscordCoreAPI";
		data["d"]["properties"]["$device"] = "DiscordCoreAPI";
		data["d"]["compress"] = false;
		data["d"]["large_threshold"] = 250;
		data["d"]["shard"] = { currentShard, numberOfShards };
		data["d"]["intents"] = intents;
#endif
		return data;
	};

	std::string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage) {
		nlohmann::json data;
		if (dataPackage.deleteMessageDays != 0) {
			nlohmann::json dataNew{};
			dataNew["delete_message_days"] = dataPackage.deleteMessageDays;
			data.update(dataNew);
		}
		if (dataPackage.reason != "") {
			nlohmann::json dataNew{};
			dataNew["reason"] = dataPackage.reason;
			data.update(dataNew);
		}
		return data.dump();
	}

	std::vector<uint8_t> JSONIFY(int32_t ssrc, int32_t delay) {
		nlohmann::json data{};
		data["op"] = 5;
		data["d"]["speaking"] = 1 << 0;
		data["d"]["delay"] = delay;
		data["d"]["ssrc"] = ssrc;
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	nlohmann::json JSONIFY(int32_t lastReceivedNumber) {
		nlohmann::json data{};
		data["d"] = lastReceivedNumber;
		data["op"] = 1;
		return data;
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

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage) {
		auto activitiesArray = nlohmann::json::array();
		nlohmann::json data{};
		data["activities"] = nlohmann::json{};
		for (auto& value: dataPackage.activities) {
			nlohmann::json dataNew{};
			dataNew["name"] = value.name;
			dataNew["type"] = value.type;
			if (value.url != "") {
				dataNew["url"] = value.url;
			}
			activitiesArray.push_back(dataNew);
		}
		data["op"] = 3;
		data["d"]["since"] = dataPackage.since;
		data["d"]["activities"] = activitiesArray;
		data["d"]["status"] = dataPackage.status;
		data["d"]["afk"] = dataPackage.afk;

		return data;
	}

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage) {
		nlohmann::json data{};
		data["op"] = 4;
		data["d"]["guild_id"] = dataPackage.guildId;
		data["d"]["self_mute"] = dataPackage.selfMute;
		data["d"]["self_deaf"] = dataPackage.selfDeaf;
		if (dataPackage.channelId == "") {
			data["d"]["channel_id"] = nullptr;
		} else {
			data["d"]["channel_id"] = dataPackage.channelId;
		}

		return data;
	}

	std::vector<uint8_t> JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02) {
		nlohmann::json data{};
		data["op"] = 0;
		data["d"]["user_id"] = dataPackage02.userId;
		data["d"]["server_id"] = dataPackage02.guildId;
		data["d"]["session_id"] = dataPackage.sessionId;
		data["d"]["token"] = dataPackage.token;
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	nlohmann::json JSONIFY(std::string serverId, std::string sessionId, std::string token, std::string) {
		nlohmann::json data{};
		data["op"] = 7;
		data["d"]["server_id"] = serverId;
		data["d"]["session_id"] = sessionId;
		data["d"]["token"] = token;
		return data;
	}

	std::vector<uint8_t> JSONIFY(std::string localPort, std::string localIp, std::string encryptionMode, int32_t) {
		nlohmann::json data{};
		data["op"] = 1;
		data["d"]["protocol"] = "udp";
		data["d"]["data"]["address"] = localIp;
		data["d"]["data"]["port"] = stol(localPort);
		data["d"]["data"]["mode"] = encryptionMode;
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage) {
		nlohmann::json roleIdArray = nlohmann::json::array();

		for (auto& value: dataPackage.roleIds) {
			roleIdArray.push_back(value);
		}

		nlohmann::json data{};
		data["nick"] = dataPackage.nick;
		data["communication_disabled_until"] = std::string(dataPackage.communicationDisabledUntil);

		if (roleIdArray.size() == 0) {
			nlohmann::json dataNew{};
			dataNew["roles"] = nullptr;
			data.update(dataNew);
		} else {
			nlohmann::json dataNew{};
			dataNew["roles"] = roleIdArray;
			data.update(dataNew);
		}

		if (dataPackage.newVoiceChannelId != "") {
			nlohmann::json dataNew{};
			dataNew["mute"] = dataPackage.mute;
			dataNew["deaf"] = dataPackage.deaf;
			dataNew["channel_id"] = dataPackage.newVoiceChannelId;
			data.update(dataNew);
		}

		return data.dump();
	}

	nlohmann::json JSONIFY(std::string botToken, std::string sessionID, int32_t lastReceivedNumber) {
		nlohmann::json data{};
		data["op"] = 6;
		data["d"]["token"] = botToken;
		data["d"]["session_id"] = sessionID;
		data["d"]["seq"] = lastReceivedNumber;
		return data;
	};

	int32_t JSONIFY(std::string initialPayload) {
		nlohmann::json jsonVal = nlohmann::json::parse(initialPayload);
		int32_t finalValue = 0;
		if (jsonVal.contains("heartbeat_interval")) {
			finalValue = jsonVal["heartbeat_interval"];
		}
		return finalValue;
	};

	std::string JSONIFY(DiscordCoreAPI::StartThreadInForumChannelData dataPackage) {
		nlohmann::json data{};

		nlohmann::json attachments{};

		for (auto& value: dataPackage.message.attachments) {
			nlohmann::json attachment{ value };
			attachments.push_back(attachment);
		}

		if (attachments.size() > 0) {
			data["message"]["attachments"] = attachments;
		}

		nlohmann::json componentsActionRow{};

		for (auto& value: dataPackage.message.components) {
			nlohmann::json components{};

			for (auto& valueNew: value.components) {
				components.push_back(nlohmann::json{ valueNew });
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			componentsActionRow.push_back(componentActionRow);
		}

		if (componentsActionRow.size() > 0) {
			data["message"]["components"] = componentsActionRow;
		}

		data["message"]["allowed_mentions"] = dataPackage.message.allowedMentions;

		nlohmann::json stickersArray{};

		for (auto& value: dataPackage.message.stickerIds) {
			stickersArray.push_back(value);
		}

		if (stickersArray.size() > 0) {
			data["message"]["sticker_ids"] = stickersArray;
		}

		nlohmann::json embedsArray{};

		for (auto& value: dataPackage.message.embeds) {
			nlohmann::json embed{ value };
			embedsArray.push_back(embed);
		}

		if (dataPackage.message.embeds.size() > 0) {
			data["message"]["embeds"] = embedsArray;
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

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage) {
		nlohmann::json data{};

		nlohmann::json attachments{};

		for (auto& value: dataPackage.attachments) {
			nlohmann::json attachment{ value };
			attachments.push_back(attachment);
		}

		if (attachments.size() > 0) {
			data["attachments"] = attachments;
		}

		if (dataPackage.messageReference.messageId != "") {
			data["message_reference"] = dataPackage.messageReference;
		}

		nlohmann::json componentsActionRow{};

		for (auto& value: dataPackage.components) {
			nlohmann::json components{};

			for (auto& valueNew: value.components) {
				components.push_back(nlohmann::json{ valueNew });
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			componentsActionRow.push_back(componentActionRow);
		}

		if (componentsActionRow.size() > 0) {
			data["components"] = componentsActionRow;
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;
		
		nlohmann::json stickersArray{};

		for (auto& value: dataPackage.stickerIds) {
			stickersArray.push_back(value);
		}

		if (stickersArray.size() > 0) {
			data["sticker_ids"] = stickersArray;
		}		

		nlohmann::json embedsArray{};		

		for (auto& value: dataPackage.embeds) {
			nlohmann::json embed{ value };
			embedsArray.push_back(embed);
		}
		
		if (dataPackage.embeds.size() > 0) {
			data["embeds"] = embedsArray;
		}

		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}

		data["flags"] = dataPackage.flags;

		data["tts"] = dataPackage.tts;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage) {

		nlohmann::json data{};

		nlohmann::json attachments{};

		for (auto& value: dataPackage.attachments) {
			nlohmann::json attachment{ value };
			attachments.push_back(attachment);
		}

		if (attachments.size() > 0) {
			data["attachments"] = attachments;
		}

		nlohmann::json componentsActionRow{};

		for (auto& value: dataPackage.components) {
			nlohmann::json components{};

			for (auto& valueNew: value.components) {
				components.push_back(nlohmann::json{ valueNew });
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			componentsActionRow.push_back(componentActionRow);
		}

		if (componentsActionRow.size() > 0) {
			data["components"] = componentsActionRow;
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;

		nlohmann::json stickersArray{};

		if (stickersArray.size() > 0) {
			data["sticker_ids"] = stickersArray;
		}

		nlohmann::json embedsArray{};

		for (auto& value: dataPackage.embeds) {
			nlohmann::json embed{ value };
			embedsArray.push_back(embed);
		}

		if (dataPackage.embeds.size() > 0) {
			data["embeds"] = embedsArray;
		}

		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}

		data["flags"] = dataPackage.flags;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage) {
		std::stringstream stream;
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();

		nlohmann::json data{};
		data["color"] = roleColorReal;
		data["hoist"] = dataPackage.hoist;
		data["mendtionable"] = dataPackage.mentionable;
		data["name"] = dataPackage.name;
		data["permissions"] = stol(dataPackage.permissions);
		return data.dump();
	}

	void JSONIFY(DiscordCoreAPI::ApplicationCommandOptionData dataPackage, nlohmann::json* pJSONData) {
		nlohmann::json newOption{};
		newOption["description_localizations"] = dataPackage.descriptionLocalizations;
		newOption["name_localizations"] = dataPackage.nameLocalizations;
		
		newOption["description"] = dataPackage.description;
		newOption["name"] = dataPackage.name;
		newOption["type"] = dataPackage.type;
		newOption["required"] = dataPackage.required;
		newOption["autocomplete"] = dataPackage.autocomplete;
		newOption["min_value"] = dataPackage.minValue;
		newOption["max_value"] = dataPackage.maxValue;
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
			newOption["channel_types"] = dataPackage.channelTypes;
		}
		if (dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
			newOption["required"] = dataPackage.required;
		}
		nlohmann::json dataArray;
		newOption["options"] = dataArray;
		newOption["choices"] = dataArray;
		if (dataPackage.choices.size() > 0) {
			for (int32_t y = 0; y < dataPackage.choices.size(); y += 1) {
				DiscordCoreAPI::ApplicationCommandOptionChoiceData choiceData;
				choiceData.name = dataPackage.choices[y].name;
				if (dataPackage.choices[y].valueString != "") {
					choiceData.valueString = dataPackage.choices[y].valueString;
					nlohmann::json jsonValue{};
					jsonValue["name"] = choiceData.name;
					jsonValue["value"] = choiceData.valueString;
					jsonValue["name_localizations"] = choiceData.nameLocalizations;
					newOption["choices"].push_back(jsonValue);
				} else if (dataPackage.choices[y].valueInt != 0) {
					choiceData.valueInt = dataPackage.choices[y].valueInt;
					nlohmann::json jsonValue{};
					jsonValue["name"] = choiceData.name;
					jsonValue["value"] = choiceData.valueInt;
					jsonValue["name_localizations"] = choiceData.nameLocalizations;
					newOption["choices"].push_back(jsonValue);
				}
			}
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
			nlohmann::json newData{};
			newData["id"] = value.id;
			newData["permission"] = value.permission;
			newData["type"] = value.type;
			newDataArray.push_back(newData);
		}

		return newDataArray.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage) {
		nlohmann::json newDataArray = nlohmann::json();

		for (auto& value: dataPackage.permissions) {
			nlohmann::json newData01 = nlohmann::json::array();
			for (auto& value01: value.permissions) {
				newData01.push_back({ { "id", value01.id }, { "permission", value01.permission }, { "type", value01.type } });
			}
			newDataArray.push_back({ { "id", value.id }, { "permissions", newData01 } });
		}

		return newDataArray.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage) {
		nlohmann::json data;
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandType::Message || dataPackage.type == DiscordCoreAPI::ApplicationCommandType::User) {
			data = { { "name", dataPackage.name }, { "type", dataPackage.type } };
		} else {
			data = { { "name", dataPackage.name }, { "description", dataPackage.description }, { "default_permission", dataPackage.defaultPermission },
				{ "dm_permission", dataPackage.dmPermission }, { "description_localizations", dataPackage.descriptionLocalizations },
				{ "name_localizations", dataPackage.nameLocalizations } };
			if (dataPackage.defaultMemberPermissions != "") {
				nlohmann::json dataNew = { "default_member_permissions", dataPackage.defaultMemberPermissions };
				data.emplace(dataNew);
			}
			nlohmann::json arrayValue{};
			data.emplace(std::make_pair("options", arrayValue));

			if (dataPackage.options.size() > 0) {
				for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
					JSONIFY(dataPackage.options[x], &data["options"]);
				}
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage) {
		nlohmann::json data;
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandType::Message || dataPackage.type == DiscordCoreAPI::ApplicationCommandType::User) {
			data = { { "name", dataPackage.name }, { "type", dataPackage.type } };
		} else {
			data = { { "name", dataPackage.name }, { "description", dataPackage.description }, { "default_permission", dataPackage.defaultPermission },
				{ "dm_permission", dataPackage.dmPermission }, { "description_localizations", dataPackage.descriptionLocalizations },
				{ "name_localizations", dataPackage.nameLocalizations } };
			if (dataPackage.defaultMemberPermissions != "") {
				nlohmann::json dataNew = { "default_member_permissions", dataPackage.defaultMemberPermissions };
				data.emplace(dataNew);
			}

			nlohmann::json arrayValue{};

			data.emplace(std::make_pair("options", arrayValue));

			if (dataPackage.options.size() > 0) {
				for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
					JSONIFY(dataPackage.options[x], &data["options"]);
				}
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage) {
		nlohmann::json data = { { "name", dataPackage.name }, { "description", dataPackage.description }, { "default_permission", dataPackage.defaultPermission },
			{ "dm_permission", dataPackage.dmPermission }, { "description_localizations", dataPackage.descriptionLocalizations },
			{ "name_localizations", dataPackage.nameLocalizations } };
		if (dataPackage.defaultMemberPermissions != "") {
			nlohmann::json dataNew = { "default_member_permissions", dataPackage.defaultMemberPermissions };
			data.emplace(dataNew);
		}

		nlohmann::json arrayValue;

		data.emplace(std::make_pair("options", arrayValue));

		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage) {
		nlohmann::json data = { { "name", dataPackage.name }, { "description", dataPackage.description }, { "default_permission", dataPackage.defaultPermission },
			{ "dm_permission", dataPackage.dmPermission }, { "description_localizations", dataPackage.descriptionLocalizations },
			{ "name_localizations", dataPackage.nameLocalizations } };
		if (dataPackage.defaultMemberPermissions != "") {
			nlohmann::json dataNew = { "default_member_permissions", dataPackage.defaultMemberPermissions };
			data.emplace(dataNew);
		}
		nlohmann::json arrayValue;

		data.emplace(std::make_pair("options", arrayValue));

		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditWebHookData dataPackage) {

		nlohmann::json data{};

		nlohmann::json componentsActionRow{};

		for (auto& value: dataPackage.components) {
			nlohmann::json components{};

			for (auto& valueNew: value.components) {
				components.push_back(nlohmann::json{ valueNew });
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			componentsActionRow.push_back(componentActionRow);
		}

		if (componentsActionRow.size() > 0) {
			data["components"] = componentsActionRow;
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;

		nlohmann::json embedsArray{};

		for (auto& value: dataPackage.embeds) {
			nlohmann::json embed{ value };
			embedsArray.push_back(embed);
		}

		if (dataPackage.embeds.size() > 0) {
			data["embeds"] = embedsArray;
		}

		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::InteractionResponseData dataPackage) {
		nlohmann::json data{};

		nlohmann::json attachments{};

		for (auto& value: dataPackage.data.attachments) {
			nlohmann::json attachment{ value };
			attachments.push_back(attachment);
		}

		if (attachments.size() > 0) {
			data["data"]["attachments"] = attachments;
		}

		nlohmann::json componentsActionRow{};

		for (auto& value: dataPackage.data.components) {
			nlohmann::json components{};

			for (auto& valueNew: value.components) {
				components.push_back(nlohmann::json{ valueNew });
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			componentsActionRow.push_back(componentActionRow);
		}

		if (componentsActionRow.size() > 0) {
			data["data"]["components"] = componentsActionRow;
		}

		data["data"]["allowed_mentions"] = dataPackage.data.allowedMentions;

		if (dataPackage.data.choices.size() > 0) {
			for (auto& value: dataPackage.data.choices) {
				data["data"]["choices"].push_back(value);
			}
		}

		nlohmann::json embedsArray{};

		for (auto& value: dataPackage.data.embeds) {
			nlohmann::json embed{ value };
			embedsArray.push_back(embed);
		}

		if (dataPackage.data.embeds.size() > 0) {
			data["data"]["embeds"] = embedsArray;
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

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();

		nlohmann::json data = { { "color", roleColorReal

								},
			{ "hoist", dataPackage.hoist }, { "permissions", dataPackage.permissions.getCurrentPermissionString() }, { "mentionable", dataPackage.mentionable },
			{ "name", dataPackage.name } };
		return data.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage) {
		nlohmann::json dataArray = nlohmann::json::array();

		for (auto& value: dataPackage.rolePositions) {
			nlohmann::json data = { { "id", value.roleId }, { "position", value.rolePosition } };
			dataArray.push_back(data);
		}
		return dataArray.dump();
	};
	std::string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage) {
		nlohmann::json data = { { "allow", stoll(dataPackage.allow) }, { "deny", stoll(dataPackage.deny) }, { "type", dataPackage.type } };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage) {
		nlohmann::json data{};

		nlohmann::json attachments{};

		for (auto& value: dataPackage.attachments) {
			nlohmann::json attachment{ value };
			attachments.push_back(attachment);
		}

		if (attachments.size() > 0) {
			data["attachments"] = attachments;
		}

		nlohmann::json componentsActionRow{};

		for (auto& value: dataPackage.components) {
			nlohmann::json components{};

			for (auto& valueNew: value.components) {
				components.push_back(nlohmann::json{ valueNew });
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			componentsActionRow.push_back(componentActionRow);
		}

		if (componentsActionRow.size() > 0) {
			data["components"] = componentsActionRow;
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;

		nlohmann::json embedsArray{};

		for (auto& value: dataPackage.embeds) {
			nlohmann::json embed{ value };
			embedsArray.push_back(embed);
		}

		if (dataPackage.embeds.size() > 0) {
			data["embeds"] = embedsArray;
		}

		data["avatar_url"] = dataPackage.avatarUrl;

		if (dataPackage.username != "") {
			data["username"] = dataPackage.username;
		}

		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}

		data["flags"] = dataPackage.flags;

		data["tts"] = dataPackage.tts;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage) {
		nlohmann::json data{};
		data["messages"] = dataPackage.messageIds;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage) {
		nlohmann::json data{};
		data["temporary"] = dataPackage.temporary;
		data["unique"] = dataPackage.unique;
		data["max_uses"] = dataPackage.maxUses;
		data["max_age"] = dataPackage.maxAge;
		if (dataPackage.targetUserId != "") {
			data["target_application_id"] = dataPackage.targetApplicationId;
			data["target_type"] = dataPackage.targetType;
			data["target_user_id"] = dataPackage.targetUserId;	
		}
		return data.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage) {
		nlohmann::json data{};
		data["webhook_channel_id"] = dataPackage.targetChannelId;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage) {
		nlohmann::json data{};
		data["access_token"] = dataPackage.token;
		data["nick"] = dataPackage.nick;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage) {
		nlohmann::json data{};
		data = { { "name", dataPackage.threadName }, { "auto_archive_duration", dataPackage.autoArchiveDuration } };
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage) {
		nlohmann::json data{};

		data = { { "invitable", dataPackage.invitable }, { "auto_archive_duration", dataPackage.autoArchiveDuration }, { "name", dataPackage.threadName },
			{ "type", dataPackage.type } };
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage) {
		nlohmann::json data{};

		nlohmann::json rolesArray = nlohmann::json::array();
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}

		data = { { "name", dataPackage.name }, { "image", dataPackage.imageDataFinal }, { "roles", rolesArray } };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage) {
		nlohmann::json data{};

		nlohmann::json rolesArray = nlohmann::json::array();
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}

		data = { { "name", dataPackage.name }, { "roles", rolesArray } };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage) {
		nlohmann::json data{};

		nlohmann::json channelsArray = nlohmann::json::array();

		for (auto& value: dataPackage.channels) {
			nlohmann::json newData = { { "name", value.name }, { "type", value.type }, { "id", value.id }, { "parent_id", value.parentId } };
			channelsArray.push_back(newData);
		}

		nlohmann::json rolesArray = nlohmann::json::array();

		for (auto& value: dataPackage.roles) {
			nlohmann::json newData = { { "mentionable", value.getMentionable() }, { "permissions", value.permissions.getCurrentPermissionString() }, { "position", value.position },
				{ "managed", value.getManaged() }, { "hoist", value.getHoist() },
				{ "tags", { { "bot_id", value.tags.botId }, { "integration_id", value.tags.integrationId }, { "premium_subscriber", value.tags.premiumSubscriber } } },
				{ "color", value.color }, { "name", value.name } };
			rolesArray.push_back(newData);
		}
		if (dataPackage.afkChannelId != "") {
			nlohmann::json dataNew = { { "afk_channel_id", dataPackage.afkChannelId } };
			data.update(dataNew);
		}
		if (dataPackage.systemChannelId != "") {
			nlohmann::json dataNew = { { "system_channel_id", dataPackage.systemChannelId } };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage) {
		nlohmann::json data{};

		data = { { "default_message_notifications", dataPackage.defaultMessageNotifications }, { "explicit_content_filter", dataPackage.explicitContentFilter },
			{ "system_channel_flags", dataPackage.systemChannelFlags }, { "verification_level", dataPackage.verificationLevel }, { "icon", dataPackage.icon },
			{ "afk_timeout", dataPackage.afkTimeout }, { "name", dataPackage.name }, { "afk_timeout", dataPackage.afkTimeout }, { "banner", dataPackage.banner },
			{ "description", dataPackage.description }, { "discovery_splash", dataPackage.discoverySplash }, { "features", dataPackage.features },
			{ "owner_id", dataPackage.ownerId }, { "preferred_locale", dataPackage.preferredLocale }, { "splash", dataPackage.splash } };

		if (dataPackage.afkChannelId != "") {
			nlohmann::json dataNew = { { "afk_channel_id", dataPackage.afkChannelId } };
			data.update(dataNew);
		}
		if (dataPackage.rulesChannelId != "") {
			nlohmann::json dataNew = { { "rules_channel_id", dataPackage.rulesChannelId } };
			data.update(dataNew);
		}
		if (dataPackage.systemChannelId != "") {
			nlohmann::json dataNew = { { "system_channel_id", dataPackage.systemChannelId } };
			data.update(dataNew);
		}
		if (dataPackage.publicUpdatesChannelId != "") {
			nlohmann::json dataNew = { { "public_updates_channel_id", dataPackage.publicUpdatesChannelId } };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage) {
		nlohmann::json data{};

		nlohmann::json overwrites{};
		for (auto& value: dataPackage.permissionOverwrites) {
			nlohmann::json newData = { { "allow", value.allow.getCurrentPermissionString() }, { "channel_id", value.channelId },
				{ "deny", value.deny.getCurrentPermissionString() }, { "id", value.id }, { "type", value.type } };
			overwrites.push_back(newData);
		}
		data = {
			{ "name", dataPackage.name },
			{ "nsfw", dataPackage.nsfw },
			{ "parent_id", dataPackage.parentId },
			{ "permission_overwrites", overwrites },
			{ "position", dataPackage.position },
			{ "rate_limit_per_user", dataPackage.rateLimitPerUser },
			{ "topic", dataPackage.topic },
			{ "type", dataPackage.type },
		};
		if (dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Voice || dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			nlohmann::json dataNew = { { "bitrate", dataPackage.bitrate }, { "user_limit", dataPackage.userLimit } };
			data.update(dataNew);
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage) {
		nlohmann::json data{};

		for (auto& value: dataPackage.modifyChannelData) {
			nlohmann::json dataNew = { { "id", value.id }, { "lock_permissions", value.lockPermissions }, { "position", value.position } };
			if (value.parentId != "") {
				nlohmann::json dataNewer = { { "parent_id", value.parentId } };
				dataNew.update(dataNewer);
			}
			data.push_back(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage) {
		nlohmann::json data{};

		data = { { "access_token", dataPackage.accessToken }, { "deaf", dataPackage.deaf }, { "mute", dataPackage.mute }, { "nick", dataPackage.nick },
			{ "roles", dataPackage.roles } };

		return data.dump();
	}


	std::string JSONIFY(DiscordCoreAPI::BeginGuildPruneData dataPackage) {
		nlohmann::json data{};

		data["days"] = dataPackage.days;
		data["include_roles"] = dataPackage.includeRoles;
		data["compute_prune_count"] = dataPackage.computePruneCount;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage) {
		nlohmann::json channelsArray = nlohmann::json::array();

		for (auto& value: dataPackage.welcomeChannels) {
			nlohmann::json newData = { { "channel_id", value.channelId }, { "description", value.description }, { "emoji_name", value.emojiName } };
			if (value.emojiId != "") {
				nlohmann::json dataNewer = { { "emoji_id", value.emojiId } };
				newData.update(dataNewer);
			}
			channelsArray.push_back(newData);
		}

		nlohmann::json data{};
		data["description"] = dataPackage.description;
		data["enabled"] = dataPackage.enabled;
		data["welcome_channels"] = channelsArray;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildScheduledEventData dataPackage) {
		nlohmann::json data{};

		data = { { "description", dataPackage.description }, { "entity_metadata", { { "location", dataPackage.entityMetadata.location } } },
			{ "entity_type", dataPackage.entityType }, { "name", dataPackage.name }, { "privacy_level", dataPackage.privacyLevel },
			{ "scheduled_end_time", dataPackage.scheduledEndTime }, { "scheduled_start_time", dataPackage.scheduledStartTime } };

		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data.update({ { "channel_id", nullptr } });
		} else {
			data.update({ { "channel_id", dataPackage.channelId } });
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage) {
		nlohmann::json data{};

		data = { { "description", dataPackage.description }, { "entity_metadata", { { "location", dataPackage.entityMetadata.location } } },
			{ "entity_type", dataPackage.entityType }, { "name", dataPackage.name }, { "privacy_level", dataPackage.privacyLevel },
			{ "scheduled_end_time", dataPackage.scheduledEndTime }, { "scheduled_start_time", dataPackage.scheduledStartTime }, { "status", dataPackage.status } };

		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data.update({ { "channel_id", nullptr } });
		} else {
			data.update({ { "channel_id", dataPackage.channelId } });
		}

		return data.dump();
	}

};