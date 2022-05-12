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
	
	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::MessageReferenceData& valueNew) {
		nlohmann::json newValue{};
		newValue["fail_if_not_exists"] = valueNew.failIfNotExists;
		newValue["message_id"] = valueNew.messageId;
		newValue["channel_id"] = valueNew.channelId;
		newValue["guild_id"] = valueNew.guildId;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AllowedMentionsData& valueNew) {
		nlohmann::json newValue{};
		newValue["replied_user"] = valueNew.repliedUser;
		newValue["parse"] = valueNew.parse;
		newValue["roles"] = valueNew.roles;
		newValue["users"] = valueNew.users;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedData& valueNew) {
		auto fields = nlohmann::json::array();

		for (auto& value2: valueNew.fields) {
			fields.push_back(value2);
		}

		int32_t colorValInt = stol(valueNew.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << colorValInt;
		std::string realColorVal = stream.str();

		nlohmann::json embed{};
		embed["footer"]["proxy_icon_url"] = valueNew.footer.proxyIconUrl;
		embed["footer"]["icon_url"] = valueNew.footer.iconUrl;
		embed["footer"]["text"] = valueNew.footer.text;
		embed["author"]["proxy_icon_url"] = valueNew.author.proxyIconUrl;
		embed["author"]["icon_url"] = valueNew.author.iconUrl;
		embed["author"]["name"] = valueNew.author.name;
		embed["author"]["url"] = valueNew.author.url;
		embed["image"]["height"] = valueNew.image.height;
		embed["image"]["width"] = valueNew.image.width;
		embed["image"]["url"] = valueNew.image.url;
		embed["image"]["proxy_url"] = valueNew.image.proxyUrl;
		embed["provider"]["name"] = valueNew.provider.name;
		embed["provider"]["url"] = valueNew.provider.url;
		embed["thumbnail"]["height"] = valueNew.thumbnail.height;
		embed["thumbnail"]["width"] = valueNew.thumbnail.width;
		embed["thumbnail"]["url"] = valueNew.thumbnail.url;
		embed["thumbnail"]["proxy_url"] = valueNew.thumbnail.proxyUrl;		
		embed["video"]["height"] = valueNew.video.height;
		embed["video"]["proxy_url"] = valueNew.video.proxyUrl;
		embed["video"]["width"] = valueNew.video.width;
		embed["video"]["url"] = valueNew.video.url;
		embed["type"] = valueNew.type;
		embed["description"] = valueNew.description;
		embed["title"] = valueNew.title;
		embed["fields"] = fields;
		embed["color"] = realColorVal;
		embed["timestamp"] = valueNew.timestamp;
		embed["url"] = valueNew.url;
		jsonOut = embed;
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedFieldData& valueNew) {
		nlohmann::json newValue{};
		newValue["inline"] = valueNew.Inline;
		newValue["value"] = valueNew.value;
		newValue["name"] = valueNew.name;
		jsonOut.update(newValue);
	}

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AttachmentData& valueNew) {
		nlohmann::json newValue{};
		newValue["content_type"] = valueNew.contentType;
		newValue["description"] = valueNew.description;
		newValue["file_name"] = valueNew.filename;
		newValue["proxy_url"] = valueNew.proxyUrl;
		newValue["ephemeral"] = valueNew.ephemeral;
		newValue["url"] = valueNew.url;
		newValue["height"] = valueNew.height;
		newValue["width"] = valueNew.width;
		newValue["size"] = valueNew.size;
		jsonOut = newValue;
	}

	void to_json(nlohmann::json& jsonOut, const std::vector<ActionRowData> &dataPackage) {
		auto componentsActionRow = nlohmann::json::array();
		for (auto& value: dataPackage) {
			auto components = nlohmann::json::array();
			for (auto& valueNew: value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component{};
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["emoji"]["name"] = valueNew.emoji.name;
					component["emoji"]["animated"] = valueNew.emoji.animated;
					component["label"] = valueNew.label;
					component["style"] = valueNew.style;
					component["type"] = valueNew.type;
					component["url"] = valueNew.url;
					if (valueNew.emoji.id != "") {
						component["emoji"]["id"] = valueNew.emoji.id;
					}
					components.push_back(component);
				} else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01: valueNew.options) {
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
					component["custom_id"] = valueNew.customId;
					component["options"] = optionsArray;
					component["disabled"] = valueNew.disabled;
					component["type"] = valueNew.type;
					component["max_values"] = valueNew.maxValues;
					component["min_values"] = valueNew.minValues;
					component["placeholder"] = valueNew.placeholder;
					components.push_back(component);

				} else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component{};
					component["min_length"] = valueNew.minLength;
					component["max_length"] = valueNew.maxLength;
					component["label"] = valueNew.label;
					component["placeholder"] = valueNew.placeholder;
					component["type"] = valueNew.type;
					component["custom_id"] = valueNew.customId;
					component["style"] = valueNew.style;
					component["required"] = valueNew.required;
					components.push_back(component);
				}
			}
			nlohmann::json componentActionRow{};
			componentActionRow["type"] = 1;
			componentActionRow["components"] = components;
			jsonOut = componentActionRow;
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
		nlohmann::json data{};
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

	std::string JSONIFY(DiscordCoreAPI::EditWebHookData dataPackage) {
		nlohmann::json data{};

		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}

		if (dataPackage.components.size() == 0) {
			auto componentsActionRow = nlohmann::json::array();
			data["components"] = componentsActionRow;
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;

		for (auto& value: dataPackage.embeds) {
			data["embeds"].push_back(value);
		}

		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::InteractionResponseData dataPackage) {
		nlohmann::json data{};

		for (auto& value: dataPackage.data.attachments) {
			data["data"]["attachments"].push_back(value);
		}

		if (dataPackage.data.components.size() == 0) {
			auto componentsActionRow = nlohmann::json::array();
			data["data"]["components"] = componentsActionRow;
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
				if (value.valueFloat != nullptr) {
					theValue["value"] = *value.valueFloat;
				} else if (value.valueInt != nullptr) {
					theValue["value"] = *value.valueInt;
				} else if (value.valueString != nullptr) {
					theValue["value"] = *value.valueString;
				}
				theArray.push_back(theValue);
			}
			data["data"]["choices"] = theArray;
		}

		for (auto& value: dataPackage.data.embeds) {
			data["data"]["embeds"].push_back(value);
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

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage) {
		nlohmann::json data{};

		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}

		if (dataPackage.components.size() == 0) {
			auto componentsActionRow = nlohmann::json::array();
			data["components"] = componentsActionRow;
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;

		for (auto& value: dataPackage.embeds) {
			data["embeds"].push_back(value);
		}

		if (dataPackage.avatarUrl != "") {
			data["avatar_url"] = dataPackage.username;
		}

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

	std::string JSONIFY(DiscordCoreAPI::StartThreadInForumChannelData dataPackage) {
		nlohmann::json data{};

		for (auto& value: dataPackage.message.attachments) {
			data["message"]["attachments"].push_back(value);
		}

		if (dataPackage.message.components.size() == 0) {
			auto componentsActionRow = nlohmann::json::array();
			data["message"]["components"] = componentsActionRow;
		} else {
			data["message"]["components"] = nlohmann::json{ dataPackage.message.components };
		}

		data["message"]["allowed_mentions"] = dataPackage.message.allowedMentions;

		for (auto& value: dataPackage.message.stickerIds) {
			data["message"]["sticker_ids"].push_back(value);
		}

		for (auto& value: dataPackage.message.embeds) {
			data["message"]["embeds"].push_back(value);
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

		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}

		if (dataPackage.messageReference.messageId != "") {
			data["message_reference"] = dataPackage.messageReference;
		}

		if (dataPackage.components.size() == 0) {
			auto componentsActionRow = nlohmann::json::array();
			data["components"] = componentsActionRow;
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;
	
		for (auto& value: dataPackage.stickerIds) {
			data["sticker_ids"].push_back(value);
		}

		for (auto& value: dataPackage.embeds) {
			data["embeds"].push_back(value);
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

		for (auto& value: dataPackage.attachments) {
			data["attachments"].push_back(value);
		}

		if (dataPackage.components.size() == 0) {
			auto componentsActionRow = nlohmann::json::array();
			data["components"] = componentsActionRow;
		} else {
			data["components"] = nlohmann::json{ dataPackage.components };
		}

		data["allowed_mentions"] = dataPackage.allowedMentions;

		for (auto& value: dataPackage.embeds) {
			data["embeds"].push_back(value);
		}

		if (dataPackage.content != "") {
			data["content"] = dataPackage.content;
		}

		data["flags"] = dataPackage.flags;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();

		nlohmann::json data{};
		data["color"] = roleColorReal;
		data["hoist"] = dataPackage.hoist;
		data["permissions"] = dataPackage.permissions.getCurrentPermissionString();
		data["mentionable"] = dataPackage.mentionable;
		data["name"] = dataPackage.name;
		if (dataPackage.icon.size() > 0) {
			data["icon"] = dataPackage.icon;
		}
		if (dataPackage.unicodeEmoji.size() > 0) {
			data["unicode_emoji"] = dataPackage.unicodeEmoji;
		}
		return data.dump();
	}

	void JSONIFY(DiscordCoreAPI::ApplicationCommandOptionData dataPackage, nlohmann::json* pJSONData) {
		nlohmann::json newOption{};
		newOption["description_localizations"] = dataPackage.descriptionLocalizations;
		newOption["name_localizations"] = dataPackage.nameLocalizations;
		newOption["description"] = dataPackage.description;
		newOption["min_value"] = dataPackage.minValue;
		newOption["max_value"] = dataPackage.maxValue;
		newOption["required"] = dataPackage.required;
		newOption["name"] = dataPackage.name;
		newOption["type"] = dataPackage.type;		
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
			newOption["channel_types"] = dataPackage.channelTypes;
		}
		if (dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
			newOption["required"] = dataPackage.required;
		}
		
		newOption["options"] = nlohmann::json{};
		newOption["choices"] = nlohmann::json{};
		if (dataPackage.choices.size() > 0) {
			for (int32_t x = 0; x < dataPackage.choices.size(); x += 1) {
				nlohmann::json jsonValue{};
				jsonValue["name_localizations"] = dataPackage.choices[x].nameLocalizations;
				jsonValue["name"] = dataPackage.choices[x].name;
				if (dataPackage.choices[x].valueString != nullptr) {
					jsonValue["value"] = *dataPackage.choices[x].valueString;
					
				} else if (dataPackage.choices[x].valueInt != nullptr) {
					jsonValue["value"] = *dataPackage.choices[x].valueInt;
				
				} else if (dataPackage.choices[x].valueFloat != nullptr) {
					jsonValue["value"] = *dataPackage.choices[x].valueFloat;
				}
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
			nlohmann::json newData{};
			newData["permission"] = value.permission;
			newData["type"] = value.type;
			newData["id"] = value.id;
			newDataArray.push_back(newData);
		}

		return newDataArray.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage) {
		nlohmann::json data{};
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["default_permission"] = dataPackage.defaultPermission;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		data["type"] = dataPackage.type;
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage) {
		nlohmann::json data{};
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["default_permission"] = dataPackage.defaultPermission;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage) {
		nlohmann::json data{};
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["default_permission"] = dataPackage.defaultPermission;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		data["type"] = dataPackage.type;
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
			data["options"] = nlohmann::json{};
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage) {
		nlohmann::json data{};
		data["description_localizations"] = dataPackage.descriptionLocalizations;
		data["name_localizations"] = dataPackage.nameLocalizations;
		data["default_permission"] = dataPackage.defaultPermission;
		data["dm_permission"] = dataPackage.dmPermission;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;
		if (dataPackage.defaultMemberPermissions != "") {
			data["default_member_permissions"] = dataPackage.defaultMemberPermissions;
		}
		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options[x], &data["options"]);
			}
			data["options"] = nlohmann::json{};
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage) {
		int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();

		nlohmann::json data{};
		data["color"] = roleColorReal;
		data["hoist"] = dataPackage.hoist;
		data["permissions"] = dataPackage.permissions.getCurrentPermissionString();
		data["mentionable"] = dataPackage.mentionable;
		data["name"] = dataPackage.name;
		if (dataPackage.icon.size() > 0) {
			data["icon"] = dataPackage.icon;
		}
		if (dataPackage.unicodeEmoji.size() > 0) {
			data["unicode_emoji"] = dataPackage.unicodeEmoji;
		}
		return data.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage) {
		nlohmann::json dataArray{};

		for (auto& value: dataPackage.rolePositions) {
			nlohmann::json data{};
			data["id"] = value.roleId;
			data["position"] = value.rolePosition;
			dataArray.push_back(data);
		}
		return dataArray.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage) {
		nlohmann::json data{};
		data["allow"] = stoll(dataPackage.allow);
		data["deny"] = stoll(dataPackage.deny);
		data["type"] = dataPackage.type;
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
		data["name"] = dataPackage.threadName;
		data["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage) {
		nlohmann::json data{};
		data["invitable"] = dataPackage.invitable;
		data["auto_archive_duration"] = dataPackage.autoArchiveDuration;
		data["name"] = dataPackage.threadName;
		data["type"] = dataPackage.type;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage) {
		nlohmann::json data{};

		nlohmann::json rolesArray{};
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}

		data["name"] = dataPackage.name;
		data["image"] = dataPackage.imageDataFinal;
		data["roles"] = rolesArray;
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage) {
		nlohmann::json data{};

		nlohmann::json rolesArray{};
		for (auto& value: dataPackage.roles) {
			rolesArray.push_back(value);
		}

		data["name"] = dataPackage.name;
		data["roles"] = rolesArray;

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage) {
		nlohmann::json data{};
		data["name"] = dataPackage.name;
		data["region"] = dataPackage.region;
		data["icon"] = dataPackage.icon;
		data["verification_level"] = dataPackage.verificationLevel;
		data["default_message_notifications"] = dataPackage.defaultMessageNotifications;
		data["explicit_content_filter"] = dataPackage.explicitContentFilter;
		data["afk_timeout"] = dataPackage.afkTimeout;
		data["system_channel_flags"] = dataPackage.systemChannelFlags;

		for (auto& value: dataPackage.channels) {
			nlohmann::json newData{};
			newData["name"] = value.name;
			newData["type"] = value.type;
			newData["id"] = value.id;
			newData["parent_id"] = value.parentId;
			data["channels"].push_back(newData);
		}

		for (auto& value: dataPackage.roles) {
			nlohmann::json newData{};
			newData["mentionable"] = value.getMentionable();
			newData["permissions"] = value.permissions.getCurrentPermissionString();
			newData["position"] = value.position;
			newData["managed"] = value.getManaged();
			newData["hoist"] = value.getHoist();
			newData["tags"]["bot_id"] = value.tags.botId;
			newData["tags"]["integration_id"] = value.tags.integrationId;
			newData["tags"]["premium_subscriber"] = value.tags.premiumSubscriber;
			newData["color"] = value.color;
			newData["name"] = value.name;
			data["roles"].push_back(newData);
		}
		if (dataPackage.afkChannelId != "") {
			data["afk_channel_id"] = dataPackage.afkChannelId;
		}
		if (dataPackage.systemChannelId != "") {
			data["system_channel_id"] = dataPackage.systemChannelId;
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage) {
		nlohmann::json data{};
		data["premium_progress_bar_enabled"] = dataPackage.premiumProgressBarEnabled;
		data["default_message_notifications"] = dataPackage.defaultMessageNotifications;
		data["explicit_content_filter"] = dataPackage.explicitContentFilter;
		data["system_channel_flags"] = dataPackage.systemChannelFlags;
		data["verification_level"] = dataPackage.verificationLevel;
		data["discovery_splash"] = dataPackage.discoverySplash;
		data["preferred_locale"] = dataPackage.preferredLocale;
		data["rules_channel_id"] = dataPackage.rulesChannelId;
		data["description"] = dataPackage.description;
		data["afk_timeout"] = dataPackage.afkTimeout;
		data["features"] = dataPackage.features;
		data["splash"] = dataPackage.splash;
		data["banner"] = dataPackage.banner;
		data["name"] = dataPackage.name;
		data["icon"] = dataPackage.icon;

		if (dataPackage.publicUpdatesChannelId != "") {
			data["public_updates_channel_id"] = dataPackage.publicUpdatesChannelId;
		}
		if (dataPackage.afkChannelId != "") {
			data["afk_channel_id"] = dataPackage.afkChannelId;
		}
		if (dataPackage.systemChannelId != "") {
			data["system_channel_id"] = dataPackage.systemChannelId;
		}
		if (dataPackage.ownerId != "") {
			data["owner_id"] = dataPackage.ownerId;
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage) {
		nlohmann::json data{};

		nlohmann::json overwrites{};
		for (auto& value: dataPackage.permissionOverwrites) {
			nlohmann::json newData{};
			newData["allow"] = value.allow.getCurrentPermissionString();
			newData["channel_id"] = value.channelId;
			newData["deny"] = value.deny.getCurrentPermissionString();
			newData["id"] = value.id;
			newData["type"] = value.type;
			overwrites.push_back(newData);
		}
		data["name"] = dataPackage.name;
		data["nsfw"] = dataPackage.nsfw;
		data["parent_id"] = dataPackage.parentId;
		data["permission_overwrites"] = overwrites;
		data["position"] = dataPackage.position;
		if (dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Voice || dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			data["bitrate"] = dataPackage.bitrate;
			data["user_limit"] = dataPackage.userLimit;
		}

        data["rate_limit_per_user"] = dataPackage.rateLimitPerUser;
		data["topic"] = dataPackage.topic;
		data["type"] = dataPackage.type;
		data["default_auto_archive_duration"] = dataPackage.defaultAutoArchiveDuration;
		
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage) {
		nlohmann::json data{};

		for (auto& value: dataPackage.modifyChannelData) {
			nlohmann::json dataNew{};
			dataNew["id"] = value.id;
			dataNew["lock_permissions"] = value.lockPermissions;
			dataNew["position"] = value.position;
			if (value.parentId != "") {
				dataNew["parent_id"] = value.parentId;
			}
			data.push_back(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage) {
		nlohmann::json data{};

		data["access_token"] = dataPackage.accessToken;
		data["roles"] = dataPackage.roles;
		data["deaf"] = dataPackage.deaf;
		data["mute"] = dataPackage.mute;
		data["nick"] = dataPackage.nick;
	
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
			nlohmann::json newData{};
			newData["channel_id"] = value.channelId;
			newData["description"] = value.description;
			newData["emoji_name"] = value.emojiName;
			if (value.emojiId != "") {
				newData["emoji_id"] = value.emojiId;
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

		data["entity_metadata"]["location"] = dataPackage.entityMetadata.location;
		data["entity_metadata"]["entity_type"] = dataPackage.entityType;
		data["scheduled_start_time"] = dataPackage.scheduledStartTime;
		data["scheduled_end_time"] = dataPackage.scheduledEndTime;
		data["privacy_level"] = dataPackage.privacyLevel;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;

		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data["channel_id"] = nullptr;
		} else {
			data["channel_id"] = dataPackage.channelId;
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage) {
		nlohmann::json data{};
		data["entity_metadata"]["location"] = dataPackage.entityMetadata.location;
		data["entity_metadata"]["entity_type"] = dataPackage.entityType;
		data["scheduled_start_time"] = dataPackage.scheduledStartTime;
		data["scheduled_end_time"] = dataPackage.scheduledEndTime;
		data["privacy_level"] = dataPackage.privacyLevel;
		data["description"] = dataPackage.description;
		data["name"] = dataPackage.name;

		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data["channel_id"] = nullptr;
		} else {
			data["channel_id"] = dataPackage.channelId;
		}

		return data.dump();
	}

};