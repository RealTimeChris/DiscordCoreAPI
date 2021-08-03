// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _JSONIFIER_
#define _JSONIFIER_

#include "../pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {
	
	string getIdentifyPayload(string botToken, int intents) {
		json data;
		data = {
				{"op", 2},
					{"d", {
					{"token", botToken},
					{"properties", {
					{"$os", "Windows"},
					{"$browser", "MyBotFW"},
					{"$device", "MyBotFW"}
				}},
				{"compress", false},
				{"large_threshold", 250},
				{"shard", {0, 1}},
				{"intents", intents}
				}}
		};
		return data.dump();
	};

	string getIsSpeakingPayload(bool isSpeaking, int ssrc, int delay = 0) {
		json data;
		if (isSpeaking) {
			data = {
			{"op", 5,},
			{"d" ,{ {"speaking",1 << 0},
		{"delay" , delay},
		{"ssrc" , ssrc}}} };
		}
		else {
			data = {
			{"op", 5,},
				{"d" , { {"speaking",0},
		{"delay" , delay},
		{"ssrc" , ssrc}} }
			};
		}
		return data.dump();
	};

	string getHeartbeatPayload(int lastReceivedNumber) {
		json data = {
			{"d", lastReceivedNumber},
			{"op", 1}
		};
		return data.dump();
	};

	string getVoiceHeartbeatPayload(__int64 nonce) {
		json data = {
			{"d", nonce},
			{"op", int(3)}
		};
		return data.dump();
	};

	string getPresenceUpdatePayload(UpdatePresenceData dataPackage) {

		auto activitiesArray = json::array();

		for (auto value : dataPackage.activities) {
			if (value.url == "") {
				json dataNew = { { "name",value.name},
				{"type" ,value.type}
				};
				activitiesArray.push_back(dataNew);
			}
			else {
				json dataNew = { { "name",value.name},
				{"type" ,value.type},{"url", value.url}
				};
				activitiesArray.push_back(dataNew);
			}			
		}
		json data ={ {"op", 3},{"d" , {{
	"since", dataPackage.since},
			{"activities" ,activitiesArray} ,
			{"status" ,dataPackage.status},
			{"afk" ,dataPackage.afk}}}
		};

		return data.dump();
	}

	string getVoiceStateUpdatePayload(UpdateVoiceStateData dataPackage) {
		json data;
		if (dataPackage.channelId != "") {
			data =
			{
			{"op",4},
			{"d" , {
				{"guild_id", dataPackage.guildId},
				{"channel_id" , dataPackage.channelId},
				{"self_mute" , dataPackage.selfMute},
				{"self_deaf", dataPackage.selfDeaf}
			}} };

		}
		else {
			data =
			{
			{"op",4},
			{"d" , {
				{"guild_id", dataPackage.guildId},
				{"channel_id" , nullptr},
				{"self_mute" , dataPackage.selfMute},
				{"self_deaf", dataPackage.selfDeaf}
			}} };
		}
				

		return data.dump();
	}

	string getVoiceIdentifyPayload(DiscordCoreInternal::VoiceConnectionData dataPackage) {
		json data;

		data = {
			{"d" , {
			{"user_id" , dataPackage.userId},
			{"server_id", dataPackage.guildId},
			{"session_id" , dataPackage.sessionId},
			{"token" , dataPackage.token}
			}},{	"op", int(0)}
		};

		return data.dump();
	}

	string getResumeVoicePayload(string serverId, string sessionId, string token) {
		json data = { {"op", 7},
			{"d" , {{
				"server_id", serverId},
			{"session_id" , sessionId},
			{"token" , token}
			}}
		};
		return data.dump();
	}

	string getSelectProtocolPayload(string localPort, string localIp, string encryptionMode) {
		json data = {
			{"op", 1},
			{"d" , {
				{"protocol", "udp"},
			{"data" ,{
				{"address", localIp},
			{"port" , stol(localPort)},
			{"mode" , encryptionMode}

		}} } } };
		return data.dump();
	}

	string getModifyGuildMemberPayload(ModifyGuildMemberData dataPackage) {
		json roleIdArray = json::array();

		for (auto value : dataPackage.roleIds) {
			roleIdArray.push_back(value);
		}
		json data;
		if (dataPackage.currentChannelId == "") {
			data = { {"nick",dataPackage.nick},
				{"roles", roleIdArray } };
		}
		else if(dataPackage.newVoiceChannelId == "") {
			data = { {"nick",dataPackage.nick},
				{"mute", dataPackage.mute},
				{"deaf", dataPackage.deaf},
				{"channel_id", nullptr} ,
				{"roles", roleIdArray } };
		}
		else {
			data = { {"nick",dataPackage.nick},
				{"mute", dataPackage.mute},
				{"deaf", dataPackage.deaf},
				{"channel_id", dataPackage.newVoiceChannelId} ,
				{"roles", roleIdArray } };
		}
		
		
		return data.dump();
	}

	string getResumePayload(string botToken, string sessionID, int lastReceivedNumber) {
		json data;
		data = {
			{"op", 6},
			{"d" , {
				{"token", botToken},
				{"session_id" , sessionID},
				{"seq" , lastReceivedNumber}
			}}
		};
		return data.dump();
	};

	string getSocketPath(hstring initialPayload) {
		string finalString = "";
		json jsonVal;
		jsonVal = jsonVal.parse(to_string(initialPayload));
		if (jsonVal.contains("url")){
			finalString = jsonVal.at("url");
		}
		return finalString;
	};

	int getHeartBeatInterval(hstring initialPayload) {
		json jsonVal;
		int finalValue = 0;
		jsonVal = jsonVal.parse(to_string(initialPayload));
		if (jsonVal.contains("heartbeat_interval")) {
			finalValue = jsonVal.at("heartbeat_interval");
		}
		return finalValue;
	};

	string getCreateMessagePayload(PostMessageData dataPackage) {
		auto componentsActionRow = json::array();

		for (auto& value : dataPackage.components) {
			auto components = json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == ComponentType::Button) {
					if (valueNew.emoji.id == "") {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
					else {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
							{"id", valueNew.emoji.id},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
				}
				else if (valueNew.type == ComponentType::SelectMenu) {
					json optionsArray = json::array();
					for (auto value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
						{"disabled", valueNew.disabled},
						{"type", valueNew.type},
							{"max_values", valueNew.maxValues},
							{"min_values", valueNew.minValues},
							{"placeholder", valueNew.placeholder }
						};
						components.push_back(component);
					}
				}
			}
			json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}
		
		auto embedsArray = json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = json::array();

			for (auto& value2 : value.fields) {
				json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
			stringstream stream;
			stream << setbase(10) << colorValInt;
			string realColorVal = stream.str();

			json embed =
			{ { "author", {
		   {"icon_url", value.author.iconUrl},
		   {"name", value.author.name},
		   {"url", value.author.url },
		   {"proxy_icon_url", value.author.proxyIconUrl}
   } },
			{ "image", {
				{"height", value.image.height},
				{"width", value.image.width},
				{"url", value.image.url},
				{"proxy_url",value.image.proxyUrl}
		} },
			{ "provider" , {
				{"name", value.provider.name},
				{"url", value.provider.url}
		} },
			{ "thumbnail", {
				{"height", value.thumbnail.height},
				{"width", value.thumbnail.width},
				{"url", value.thumbnail.url},
				{"proxy_url", value.thumbnail.proxyUrl}
			} },
			{ "footer", {
				{"icon_url", value.footer.iconUrl},
				{"proxy_icon_url", value.footer.proxyIconUrl},
				{"text", value.footer.text}
	} },
			{ "description" , value.description },
			{ "title", value.title },
			{ "fields", fields },
			{ "color",realColorVal },
				{"timestamp", value.timestamp},
				{"components", componentsActionRow}
			};
			embedsArray.push_back(embed);
		}

		if (dataPackage.embeds.size() > 0){
			if (dataPackage.messageReference.guildId != "") {
				json data = {
		{"allowed_mentions", {
			{"parse", dataPackage.allowedMentions.parse},
			{"replied_user", dataPackage.allowedMentions.repliedUser},
			{"roles", dataPackage.allowedMentions.roles},
			{"users", dataPackage.allowedMentions.users}
			}},
			{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
			{"channel_id", dataPackage.messageReference.channelId},
			{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
				}},
		{"content", dataPackage.content},
		{"tts" , dataPackage.tts},
		{"embeds" ,embedsArray},
					{"components", componentsActionRow}
				};
				return data.dump();
			}
				else {
				json data = {
	{"allowed_mentions", {
		{"parse", dataPackage.allowedMentions.parse},
		{"replied_user", dataPackage.allowedMentions.repliedUser},
		{"roles", dataPackage.allowedMentions.roles},
		{"users", dataPackage.allowedMentions.users}
		}},
	{"content", dataPackage.content},
	{"tts" , dataPackage.tts},
	{"embeds" ,embedsArray},
				{"components", componentsActionRow}
				};
				return data.dump();
			}
		}
		else {
			if (dataPackage.messageReference.guildId != "") {
				json data = {
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
				{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
				{"channel_id", dataPackage.messageReference.channelId},
				{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
					}},
			{"content", dataPackage.content},
			{"tts" , dataPackage.tts},
					{"components", componentsActionRow}
				};
				return data.dump();
			}
			else {
				json data = {
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
			{"content", dataPackage.content},
			{"tts" , dataPackage.tts},
				{"components", componentsActionRow}
				};
				return data.dump();
			}
			
		}
		
	}

	string getCreateMessagePayload(SendDMData dataPackage) {

		auto componentsActionRow = json::array();

		for (auto& value : dataPackage.components) {
			auto components = json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == ComponentType::Button) {
					if (valueNew.emoji.id == "") {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
					else {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
							{"id", valueNew.emoji.id},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
				}
				else if (valueNew.type == ComponentType::SelectMenu) {
					json optionsArray = json::array();
					for (auto value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
						{"disabled", valueNew.disabled},
						{"type", valueNew.type},
							{"max_values", valueNew.maxValues},
							{"min_values", valueNew.minValues},
							{"placeholder", valueNew.placeholder }
						};
						components.push_back(component);
					}
				}
			}
			json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		auto embedsArray = json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = json::array();

			for (auto& value2 : value.fields) {
				json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
			stringstream stream;
			stream << setbase(10) << colorValInt;
			string realColorVal = stream.str();

			json embed =
			{ { "author", {
		   {"icon_url", value.author.iconUrl},
		   {"name", value.author.name},
		   {"url", value.author.url },
		   {"proxy_icon_url", value.author.proxyIconUrl}
   } },
			{ "image", {
				{"height", value.image.height},
				{"width", value.image.width},
				{"url", value.image.url},
				{"proxy_url",value.image.proxyUrl}
		} },
			{ "provider" , {
				{"name", value.provider.name},
				{"url", value.provider.url}
		} },
			{ "thumbnail", {
				{"height", value.thumbnail.height},
				{"width", value.thumbnail.width},
				{"url", value.thumbnail.url},
				{"proxy_url", value.thumbnail.proxyUrl}
			} },
			{ "footer", {
				{"icon_url", value.footer.iconUrl},
				{"proxy_icon_url", value.footer.proxyIconUrl},
				{"text", value.footer.text}
	} },
			{ "description" , value.description },
			{ "title", value.title },
			{ "fields", fields },
			{ "color",realColorVal },
				{"timestamp", value.timestamp},
				{"components", componentsActionRow}
			};
			embedsArray.push_back(embed);
		}

		if (dataPackage.embeds.size()>0) {
			if (dataPackage.messageReference.guildId != "") {
				json data = {
		{"allowed_mentions", {
			{"parse", dataPackage.allowedMentions.parse},
			{"replied_user", dataPackage.allowedMentions.repliedUser},
			{"roles", dataPackage.allowedMentions.roles},
			{"users", dataPackage.allowedMentions.users}
			}},
			{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
			{"channel_id", dataPackage.messageReference.channelId},
			{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
				}},
		{"content", dataPackage.content},
		{"tts" , dataPackage.tts},
		{"embeds" ,embedsArray},
					{"components", componentsActionRow}
				};
				return data.dump();
			}
			else {
				json data = {
	{"allowed_mentions", {
		{"parse", dataPackage.allowedMentions.parse},
		{"replied_user", dataPackage.allowedMentions.repliedUser},
		{"roles", dataPackage.allowedMentions.roles},
		{"users", dataPackage.allowedMentions.users}
		}},
	{"content", dataPackage.content},
	{"tts" , dataPackage.tts},
	{"embeds" ,embedsArray},
				{"components", componentsActionRow}
				};
				return data.dump();
			}
		}
		else {
			if (dataPackage.messageReference.guildId != "") {
				json data = {
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
				{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
				{"channel_id", dataPackage.messageReference.channelId},
				{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
					}},
			{"content", dataPackage.content},
			{"tts" , dataPackage.tts},
					{"components", componentsActionRow}
				};
				return data.dump();
			}
			else {
				json data = {
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
					{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
				{"channel_id", dataPackage.messageReference.channelId},
				{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
					}},
			{"content", dataPackage.content},
			{"tts" , dataPackage.tts},
				{"components", componentsActionRow}
				};
				return data.dump();
			}

		}

	}

	string getReplyMessagePayload(PostMessageData dataPackage) {

		auto componentsActionRow = json::array();

		for (auto& value : dataPackage.components) {
			auto components = json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == ComponentType::Button) {
					if (valueNew.emoji.id == "") {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
					else {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
							{"id", valueNew.emoji.id},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
				}
				else if (valueNew.type == ComponentType::SelectMenu) {
					json optionsArray = json::array();
					for (auto value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
						{"disabled", valueNew.disabled},
						{"type", valueNew.type},
							{"max_values", valueNew.maxValues},
							{"min_values", valueNew.minValues},
							{"placeholder", valueNew.placeholder }
						};
						components.push_back(component);
					}
				}
			}
			json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		auto embedsArray = json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = json::array();

			for (auto& value2 : value.fields) {
				json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
			stringstream stream;
			stream << setbase(10) << colorValInt;
			string realColorVal = stream.str();

			json embed =
			{ { "author", {
		   {"icon_url", value.author.iconUrl},
		   {"name", value.author.name},
		   {"url", value.author.url },
		   {"proxy_icon_url", value.author.proxyIconUrl}
   } },
			{ "image", {
				{"height", value.image.height},
				{"width", value.image.width},
				{"url", value.image.url},
				{"proxy_url",value.image.proxyUrl}
		} },
			{ "provider" , {
				{"name", value.provider.name},
				{"url", value.provider.url}
		} },
			{ "thumbnail", {
				{"height", value.thumbnail.height},
				{"width", value.thumbnail.width},
				{"url", value.thumbnail.url},
				{"proxy_url", value.thumbnail.proxyUrl}
			} },
			{ "footer", {
				{"icon_url", value.footer.iconUrl},
				{"proxy_icon_url", value.footer.proxyIconUrl},
				{"text", value.footer.text}
	} },
			{ "description" , value.description },
			{ "title", value.title },
			{ "fields", fields },
			{ "color",realColorVal },
				{"timestamp", value.timestamp}
			};
			embedsArray.push_back(embed);
		}

			if (dataPackage.embeds.size() > 0) {
				if (dataPackage.messageReference.guildId != "") {
					json data = {
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
				{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
				{"channel_id", dataPackage.messageReference.channelId},
				{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
					}},
			{"content", dataPackage.content},
			{"tts" , dataPackage.tts},
			{"embeds" ,embedsArray} ,
			{"components", componentsActionRow} };

					return data.dump();
				}
				else {
					json data = {
		{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
				{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
				{"channel_id", dataPackage.messageReference.channelId},
				{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
					}},
		{"content", dataPackage.content},
		{"tts" , dataPackage.tts},
		{"embeds" ,embedsArray},
		{"components", componentsActionRow}
					};

					return data.dump();
				}
			}
			else {
				if (dataPackage.messageReference.guildId != "") {
					json data = {
				{"allowed_mentions", {
					{"parse", dataPackage.allowedMentions.parse},
					{"replied_user", dataPackage.allowedMentions.repliedUser},
					{"roles", dataPackage.allowedMentions.roles},
					{"users", dataPackage.allowedMentions.users}
					}},
					{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
					{"channel_id", dataPackage.messageReference.channelId},
					{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
						}},
				{"content", dataPackage.content},
				{"tts" , dataPackage.tts},
						{"components", componentsActionRow}
					};

					return data.dump();
				}
				else {
					json data = {
				{"allowed_mentions", {
					{"parse", dataPackage.allowedMentions.parse},
					{"replied_user", dataPackage.allowedMentions.repliedUser},
					{"roles", dataPackage.allowedMentions.roles},
					{"users", dataPackage.allowedMentions.users}
					}},
						{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
					{"channel_id", dataPackage.messageReference.channelId},
					{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
						}},
				{"content", dataPackage.content},
				{"tts" , dataPackage.tts},
					{"components", componentsActionRow}
					};

					return data.dump();
				}
			}

	}

	string getEditMessagePayload(PatchMessageData dataPackage) {
		auto componentsActionRow = json::array();

		for (auto& value : dataPackage.components) {
			auto components = json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == ComponentType::Button) {
					if (valueNew.emoji.id == "") {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
					else {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
							{"id", valueNew.emoji.id},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
				}
				else if (valueNew.type == ComponentType::SelectMenu) {
					json optionsArray = json::array();
					for (auto value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
						{"disabled", valueNew.disabled},
						{"type", valueNew.type},
							{"max_values", valueNew.maxValues},
							{"min_values", valueNew.minValues},
							{"placeholder", valueNew.placeholder }
						};
						components.push_back(component);
					}
				}
			}
			json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		auto embedsArray = json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = json::array();

			for (auto& value2 : value.fields) {
				json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
			stringstream stream;
			stream << setbase(10) << colorValInt;
			string realColorVal = stream.str();

			json embed =
			{ { "author", {
		   {"icon_url", value.author.iconUrl},
		   {"name", value.author.name},
		   {"url", value.author.url },
		   {"proxy_icon_url", value.author.proxyIconUrl}
   } },
			{ "image", {
				{"height", value.image.height},
				{"width", value.image.width},
				{"url", value.image.url},
				{"proxy_url",value.image.proxyUrl}
		} },
			{ "provider" , {
				{"name", value.provider.name},
				{"url", value.provider.url}
		} },
			{ "thumbnail", {
				{"height", value.thumbnail.height},
				{"width", value.thumbnail.width},
				{"url", value.thumbnail.url},
				{"proxy_url", value.thumbnail.proxyUrl}
			} },
			{ "footer", {
				{"icon_url", value.footer.iconUrl},
				{"proxy_icon_url", value.footer.proxyIconUrl},
				{"text", value.footer.text}
	} },
			{ "description" , value.description },
			{ "title", value.title },
			{ "fields", fields },
			{ "color",realColorVal },
				{"timestamp", value.timestamp}
			};
			embedsArray.push_back(embed);
		}

		auto attachments = json::array();

		for (unsigned int x = 0; x < dataPackage.attachments.size(); x += 1) {
			json attachment = { {"content_type", dataPackage.attachments.at(x).contentType},
				{"file_name",dataPackage.attachments.at(x).filename},
				{"height",dataPackage.attachments.at(x).height},
				{"id",dataPackage.attachments.at(x).id},
				{"proxy_url",dataPackage.attachments.at(x).proxyUrl},
				{"size",dataPackage.attachments.at(x).size},
				{"url",dataPackage.attachments.at(x).url},
				{"width",dataPackage.attachments.at(x).width} };
			attachments.push_back(attachment);
		}

		json data = {
			{"flags", dataPackage.flags},
			{"attachments", attachments},
			{"components", componentsActionRow},
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
			{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
				{"channel_id", dataPackage.messageReference.channelId},
				{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists}
					}},
			{"content", dataPackage.content},
			{"embeds" ,embedsArray},
			{"components", componentsActionRow}

		};

		return data.dump();
	}

	string getModifyRolePayload(PatchRoleData dataPackage) {
		stringstream stream;
		unsigned int roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		stream << setbase(10) << roleColorInt;
		string roleColorReal = stream.str();

		json data = {
			{"color", roleColorReal},
			{"hoist", dataPackage.hoist},
			{"mendtionable", dataPackage.mentionable},
			{"name", dataPackage.name},
			{"permissions", dataPackage.permissions}
		};

		return data.dump();
	}

	void addOptionsData(ApplicationCommandOptionData dataPackage, json* pJSONData) {
		json newOption;
		newOption.emplace(make_pair("description", dataPackage.description));
		newOption.emplace(make_pair("name", dataPackage.name));
		newOption.emplace(make_pair("type", dataPackage.type));
		if (dataPackage.type != ApplicationCommandOptionType::SUB_COMMAND && dataPackage.type != ApplicationCommandOptionType::SUB_COMMAND_GROUP){
			newOption.emplace(make_pair("required", dataPackage.required));
		}		
		json dataArray;
		newOption.emplace(make_pair("options", dataArray));
		newOption.emplace(make_pair("choices", dataArray));
		if (dataPackage.choices.size() > 0) {
			for (unsigned int y = 0; y < dataPackage.choices.size(); y += 1) {
				ApplicationCommandOptionChoiceData choiceData;
				choiceData.name = dataPackage.choices.at(y).name;
				if (dataPackage.choices.at(y).valueString != "") {
					choiceData.valueString = dataPackage.choices.at(y).valueString;
					json jsonValue = { {"name", choiceData.name},{"value", choiceData.valueString} };
					newOption.at("choices").emplace_back(jsonValue);
				}
				else if (dataPackage.choices.at(y).valueInt != 0) {
					choiceData.valueInt = dataPackage.choices.at(y).valueInt;
					json jsonValue = { {"name", choiceData.name},{"value", choiceData.valueInt} };
					newOption.at("choices").emplace_back(jsonValue);
				}
			}
		}
		if (dataPackage.options.size() > 0) {
			for (auto value : dataPackage.options) {
				addOptionsData(value, &newOption.at("options"));
			}			
		}
		pJSONData->emplace_back(newOption);
	}

	string getCreateApplicationCommandPayload(CreateApplicationCommandData dataPackage) {
		json data = {
			{"name",dataPackage.name },
			{"description", dataPackage.description},
			{"default_permission", dataPackage.defaultPermission}
		};

		json arrayValue;

		data.emplace(make_pair("options", arrayValue));

		if (dataPackage.options.size() > 0) {
			for (unsigned int x = 0; x < dataPackage.options.size(); x += 1) {
				addOptionsData(dataPackage.options.at(x), &data.at("options"));
			}
		}

		return data.dump();
	}

	string getEditApplicationCommandPayload(EditApplicationCommandData dataPackage) {
		json data = {
			{"name",dataPackage.name },
			{"description", dataPackage.description},
			{"default_permission", dataPackage.defaultPermission}
		};

		json arrayValue;

		data.emplace(make_pair("options", arrayValue));

		if (dataPackage.options.size() > 0) {
			for (unsigned int x = 0; x < dataPackage.options.size(); x += 1) {
				addOptionsData(dataPackage.options.at(x), &data.at("options"));
			}
		}

		return data.dump();
	}
	
	string	getEditFollowUpMessagePayload(EditFollowUpMessageData dataPackage) {
		auto embedsArray = json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = json::array();

			for (auto& value2 : value.fields) {
				json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
			stringstream stream;
			stream << setbase(10) << colorValInt;
			string realColorVal = stream.str();

			json embed =
			{ { "author", {
		   {"icon_url", value.author.iconUrl},
		   {"name", value.author.name},
		   {"url", value.author.url },
		   {"proxy_icon_url", value.author.proxyIconUrl}
   } },
			{ "image", {
				{"height", value.image.height},
				{"width", value.image.width},
				{"url", value.image.url},
				{"proxy_url",value.image.proxyUrl}
		} },
			{ "provider" , {
				{"name", value.provider.name},
				{"url", value.provider.url}
		} },
			{ "thumbnail", {
				{"height", value.thumbnail.height},
				{"width", value.thumbnail.width},
				{"url", value.thumbnail.url},
				{"proxy_url", value.thumbnail.proxyUrl}
			} },
			{ "footer", {
				{"icon_url", value.footer.iconUrl},
				{"proxy_icon_url", value.footer.proxyIconUrl},
				{"text", value.footer.text}
	} },
			{ "description" , value.description },
			{ "title", value.title },
			{ "fields", fields },
			{ "color",realColorVal },
				{"timestamp", value.timestamp}
			};
			embedsArray.push_back(embed);
		}

		auto parseArray = json::array();
		for (auto& value : dataPackage.allowedMentions.parse) {
			parseArray.push_back(value);
		};

		auto rolesArray = json::array();
		for (auto& value : dataPackage.allowedMentions.roles) {
			rolesArray.push_back(value);
		}

		auto usersArray = json::array();
		for (auto& value : dataPackage.allowedMentions.users) {
			usersArray.push_back(value);
		}

		auto componentsActionRow = json::array();

		for (auto& value : dataPackage.components) {
			auto components = json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == ComponentType::Button) {
					if (valueNew.emoji.id == "") {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
					else {
						json component = { {"custom_id", valueNew.customId},
						{"disabled", valueNew.disabled},
						{"emoji",{
							{"name", valueNew.emoji.name},
							{"id", valueNew.emoji.id},
						{"animated", valueNew.emoji.animated}
					} },
						{"label", valueNew.label},
						{"style", valueNew.style},
						{"type", valueNew.type},
						{"url", valueNew.url}
						};
						components.push_back(component);
					}
				}
				else if (valueNew.type == ComponentType::SelectMenu) {
					json optionsArray = json::array();
					for (auto value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
						{"disabled", valueNew.disabled},
						{"type", valueNew.type},
							{"max_values", valueNew.maxValues},
							{"min_values", valueNew.minValues},
							{"placeholder", valueNew.placeholder }
						};
						components.push_back(component);
					}
				}
			}
			json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		if (dataPackage.content == "") {
			json data = {
			{"embeds", embedsArray},
			{"allowed_mentions",
			{{"parse", parseArray},
	{"roles", rolesArray},
	{"users", usersArray},
	{"repliedUser", dataPackage.allowedMentions.repliedUser}}},
				{"components", componentsActionRow} };
			return data.dump();
		}
		else {
			json data = { {"content", dataPackage.content},
				{"embeds", embedsArray},
				{"allowed_mentions",
				{{"parse", parseArray},
		{"roles", rolesArray},
		{"users", usersArray},
		{"repliedUser", dataPackage.allowedMentions.repliedUser}},
			},{"components", componentsActionRow} };
			return data.dump();
		};
	}

		string	getEditInteractionResponsePayload(EditInteractionResponseData dataPackage) {
			auto embedsArray = json::array();

			for (auto& value : dataPackage.embeds) {

				auto fields = json::array();

				for (auto& value2 : value.fields) {
					json field =
					{ {"inline", value2.Inline},
								{"value", value2.value},
								{"name", value2.name} };
					fields.push_back(field);
				}

				unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
				stringstream stream;
				stream << setbase(10) << colorValInt;
				string realColorVal = stream.str();

				json embed =
				{ { "author", {
			   {"icon_url", value.author.iconUrl},
			   {"name", value.author.name},
			   {"url", value.author.url },
			   {"proxy_icon_url", value.author.proxyIconUrl}
	   } },
				{ "image", {
					{"height", value.image.height},
					{"width", value.image.width},
					{"url", value.image.url},
					{"proxy_url",value.image.proxyUrl}
			} },
				{ "provider" , {
					{"name", value.provider.name},
					{"url", value.provider.url}
			} },
				{ "thumbnail", {
					{"height", value.thumbnail.height},
					{"width", value.thumbnail.width},
					{"url", value.thumbnail.url},
					{"proxy_url", value.thumbnail.proxyUrl}
				} },
				{ "footer", {
					{"icon_url", value.footer.iconUrl},
					{"proxy_icon_url", value.footer.proxyIconUrl},
					{"text", value.footer.text}
		} },
				{ "description" , value.description },
				{ "title", value.title },
				{ "fields", fields },
				{ "color",realColorVal },
					{"timestamp", value.timestamp}
				};
				embedsArray.push_back(embed);
			}

			auto parseArray = json::array();
			for (auto& value : dataPackage.allowedMentions.parse) {
				parseArray.push_back(value);
			};

			auto rolesArray = json::array();
			for (auto& value : dataPackage.allowedMentions.roles) {
				rolesArray.push_back(value);
			}

			auto usersArray = json::array();
			for (auto& value : dataPackage.allowedMentions.users) {
				usersArray.push_back(value);
			}

			auto componentsActionRow = json::array();

			for (auto& value : dataPackage.components) {
				auto components = json::array();

				for (auto& valueNew : value.components) {
					if (valueNew.type == ComponentType::Button) {
						if (valueNew.emoji.id == "") {
							json component = { {"custom_id", valueNew.customId},
							{"disabled", valueNew.disabled},
							{"emoji",{
								{"name", valueNew.emoji.name},
							{"animated", valueNew.emoji.animated}
						} },
							{"label", valueNew.label},
							{"style", valueNew.style},
							{"type", valueNew.type},
							{"url", valueNew.url}
							};
							components.push_back(component);
						}
						else {
							json component = { {"custom_id", valueNew.customId},
							{"disabled", valueNew.disabled},
							{"emoji",{
								{"name", valueNew.emoji.name},
								{"id", valueNew.emoji.id},
							{"animated", valueNew.emoji.animated}
						} },
							{"label", valueNew.label},
							{"style", valueNew.style},
							{"type", valueNew.type},
							{"url", valueNew.url}
							};
							components.push_back(component);
						}
					}
					else if (valueNew.type == ComponentType::SelectMenu) {
						json optionsArray = json::array();
						for (auto value01 : valueNew.options) {
							if (value01.emoji.id == "" && value01.emoji.name == "") {
								json option = { {"description",value01.description},
								{"label", value01.label},
								{"value", value01.value},{"default", value01._default} };
								optionsArray.push_back(option);

							}
							else if (value01.emoji.id == "") {
								json option = { {"description",value01.description},
								{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
								{"label", value01.label},
								{"value", value01.value},{"default", value01._default} };
								optionsArray.push_back(option);
							}
							else {
								json option = { {"description",value01.description},
								{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
								{"label", value01.label},
								{"value", value01.value},{"default", value01._default} };
								optionsArray.push_back(option);
							}
						};
						{
							json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
							{"disabled", valueNew.disabled},
							{"type", valueNew.type},
								{"max_values", valueNew.maxValues},
								{"min_values", valueNew.minValues},
								{"placeholder", valueNew.placeholder }
							};
							components.push_back(component);
						}
					}
				}
				json componentActionRow = { {"type", 1},{
						"components", components} };
				componentsActionRow.push_back(componentActionRow);
			}

			if (dataPackage.content == "") {
				json data = { 
					{"flags", dataPackage.flags}, 
					{"type", dataPackage.type},
				{"embeds", embedsArray},
				{"allowed_mentions",
				{{"parse", parseArray},
		{"roles", rolesArray},
		{"users", usersArray},
		{"repliedUser", dataPackage.allowedMentions.repliedUser}}},
					{"components", componentsActionRow} };
				return data.dump();
			}
			else {
				json data = { 
					{"flags", dataPackage.flags},{"flags", dataPackage.flags},
					{"type", dataPackage.type},
					{"content", dataPackage.content},
					{"embeds", embedsArray},
					{"allowed_mentions",
					{{"parse", parseArray},
			{"roles", rolesArray},
			{"users", usersArray},
			{"repliedUser", dataPackage.allowedMentions.repliedUser}},
				},{"components", componentsActionRow} };
				return data.dump();
			};
		}

		string getCreateDeferredInteractionResponsePayload(CreateDeferredInteractionResponseData dataPackage) {

			json data = { {"type", dataPackage.type}
			};
			return data.dump();
		}

		string getCreateInteractionResponsePayload(CreateInteractionResponseData dataPackage) {

			auto embedsArray = json::array();

			for (auto& value : dataPackage.data.embeds) {

				auto fields = json::array();

				for (auto& value2 : value.fields) {
					json field =
					{ {"inline", value2.Inline},
								{"value", value2.value},
								{"name", value2.name} };
					fields.push_back(field);
				}

				unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
				stringstream stream;
				stream << setbase(10) << colorValInt;
				string realColorVal = stream.str();

				json embed =
				{ { "author", {
			   {"icon_url", value.author.iconUrl},
			   {"name", value.author.name},
			   {"url", value.author.url },
			   {"proxy_icon_url", value.author.proxyIconUrl}
	   } },
				{ "image", {
					{"height", value.image.height},
					{"width", value.image.width},
					{"url", value.image.url},
					{"proxy_url",value.image.proxyUrl}
			} },
				{ "provider" , {
					{"name", value.provider.name},
					{"url", value.provider.url}
			} },
				{ "thumbnail", {
					{"height", value.thumbnail.height},
					{"width", value.thumbnail.width},
					{"url", value.thumbnail.url},
					{"proxy_url", value.thumbnail.proxyUrl}
				} },
				{ "footer", {
					{"icon_url", value.footer.iconUrl},
					{"proxy_icon_url", value.footer.proxyIconUrl},
					{"text", value.footer.text}
		} },
				{ "description" , value.description },
				{ "title", value.title },
				{ "fields", fields },
				{ "color",realColorVal },
					{"timestamp", value.timestamp}
				};
				embedsArray.push_back(embed);
			}

			auto parseArray = json::array();
			for (auto& value : dataPackage.data.allowedMentions.parse) {
				parseArray.push_back(value);
			};

			auto rolesArray = json::array();
			for (auto& value : dataPackage.data.allowedMentions.roles) {
				rolesArray.push_back(value);
			}

			auto usersArray = json::array();
			for (auto& value : dataPackage.data.allowedMentions.users) {
				usersArray.push_back(value);
			}

			auto componentsActionRow = json::array();

			for (auto& value : dataPackage.data.components) {
				auto components = json::array();

				for (auto& valueNew : value.components) {
					if (valueNew.type == ComponentType::Button) {
						if (valueNew.emoji.id == "") {
							json component = { {"custom_id", valueNew.customId},
							{"disabled", valueNew.disabled},
							{"emoji",{
								{"name", valueNew.emoji.name},
							{"animated", valueNew.emoji.animated}
						} },
							{"label", valueNew.label},
							{"style", valueNew.style},
							{"type", valueNew.type},
							{"url", valueNew.url}
							};
							components.push_back(component);
						}
						else {
							json component = { {"custom_id", valueNew.customId},
							{"disabled", valueNew.disabled},
							{"emoji",{
								{"name", valueNew.emoji.name},
								{"id", valueNew.emoji.id},
							{"animated", valueNew.emoji.animated}
						} },
							{"label", valueNew.label},
							{"style", valueNew.style},
							{"type", valueNew.type},
							{"url", valueNew.url}
							};
							components.push_back(component);
						}
					}
					else if (valueNew.type == ComponentType::SelectMenu) {
						json optionsArray = json::array();
						for (auto value01 : valueNew.options) {
							if (value01.emoji.id == "" && value01.emoji.name == "") {
								json option = { {"description",value01.description},
								{"label", value01.label},
								{"value", value01.value},{"default", value01._default} };
								optionsArray.push_back(option);

							}
							else if (value01.emoji.id == "") {
								json option = { {"description",value01.description},
								{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
								{"label", value01.label},
								{"value", value01.value},{"default", value01._default} };
								optionsArray.push_back(option);
							}
							else {
								json option = { {"description",value01.description},
								{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
								{"label", value01.label},
								{"value", value01.value},{"default", value01._default} };
								optionsArray.push_back(option);
							}
						};
						{
							json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
							{"disabled", valueNew.disabled},
							{"type", valueNew.type},
								{"max_values", valueNew.maxValues},
								{"min_values", valueNew.minValues},
								{"placeholder", valueNew.placeholder }
							};
							components.push_back(component);
						}
					}
				}
				json componentActionRow = { {"type", 1},{
						"components", components} };
				componentsActionRow.push_back(componentActionRow);
			}

			if (dataPackage.data.content == "") {
				json data = { {"type", dataPackage.type},
					{"data",{{"embeds", embedsArray},
					{"flags", dataPackage.data.flags },
				{"allowed_mentions",
					{{"parse", parseArray},
			{"roles", rolesArray},
			{"users", usersArray},
			{"repliedUser", dataPackage.data.allowedMentions.repliedUser}}},
					{"components", componentsActionRow},
					}}
				};
				return data.dump();
			}
			else {
				json data = { {"type", dataPackage.type},
					{"data",{{"embeds", embedsArray},
					{"flags", dataPackage.data.flags },
					{"content", dataPackage.data.content},
				{"allowed_mentions",
					{{"parse", parseArray},
			{"roles", rolesArray},
			{"users", usersArray},
			{"repliedUser", dataPackage.data.allowedMentions.repliedUser}}},
					{"components", componentsActionRow},
					}}
				};
				cout << data.dump() << endl;
				return data.dump();
			};
		 }

		 string getCreateRolePayload(PostRoleData dataPackage) {
			 unsigned int roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
			 stringstream stream;
			 stream << setbase(10) << roleColorInt;
			 string roleColorReal = stream.str();

			 json data = { {"color", roleColorReal

				 },
				 {"hoist", dataPackage.hoist},{"permissions", dataPackage.permissions},
				 {"mentionable", dataPackage.mentionable},
				 {"name", dataPackage.name }
			 };
			 return data.dump();
		 };

		 string getUpdateRolePositionsPayload(UpdateRolePositionData dataPackage) {
			 json dataArray = json::array();

			 for (auto value : dataPackage.rolePositions) {
				 json data = { {"id", value.roleId },{"position", value.rolePosition} };
				 dataArray.push_back(data);
			 }
			 return dataArray.dump();
		 };

		 string getEditChannelPermissionOverwritesPayload(PutPermissionOverwritesData dataPackage) {
			 json data = { {"allow", stoll(dataPackage.allow)},
				 {"deny", stoll(dataPackage.deny)},
				 {"type", dataPackage.type} };

			 return data.dump();
		 }

		 string getPostFollowUpMessagePayload(CreateFollowUpMessageData dataPackage) {
			 auto embedsArray = json::array();

			 for (auto& value : dataPackage.embeds) {

				 auto fields = json::array();

				 for (auto& value2 : value.fields) {
					 json field =
					 { {"inline", value2.Inline},
								 {"value", value2.value},
								 {"name", value2.name} };
					 fields.push_back(field);
				 }

				 unsigned int colorValInt = stol(value.hexColorValue, 0, 16);
				 stringstream stream;
				 stream << setbase(10) << colorValInt;
				 string realColorVal = stream.str();

				 json embed =
				 { { "author", {
				{"icon_url", value.author.iconUrl},
				{"name", value.author.name},
				{"url", value.author.url },
				{"proxy_icon_url", value.author.proxyIconUrl}
		} },
				 { "image", {
					 {"height", value.image.height},
					 {"width", value.image.width},
					 {"url", value.image.url},
					 {"proxy_url",value.image.proxyUrl}
			 } },
				 { "provider" , {
					 {"name", value.provider.name},
					 {"url", value.provider.url}
			 } },
				 { "thumbnail", {
					 {"height", value.thumbnail.height},
					 {"width", value.thumbnail.width},
					 {"url", value.thumbnail.url},
					 {"proxy_url", value.thumbnail.proxyUrl}
				 } },
				 { "footer", {
					 {"icon_url", value.footer.iconUrl},
					 {"proxy_icon_url", value.footer.proxyIconUrl},
					 {"text", value.footer.text}
		 } },
				 { "description" , value.description },
				 { "title", value.title },
				 { "fields", fields },
				 { "color",realColorVal },
					 {"timestamp", value.timestamp}
				 };
				 embedsArray.push_back(embed);
			 }

			 auto parseArray = json::array();
			 for (auto& value : dataPackage.allowedMentions.parse) {
				 parseArray.push_back(value);
			 };

			 auto rolesArray = json::array();
			 for (auto& value : dataPackage.allowedMentions.roles) {
				 rolesArray.push_back(value);
			 }

			 auto usersArray = json::array();
			 for (auto& value : dataPackage.allowedMentions.users) {
				 usersArray.push_back(value);
			 }

			 auto componentsActionRow = json::array();

			 for (auto& value : dataPackage.components) {
				 auto components = json::array();

				 for (auto& valueNew : value.components) {
					 if (valueNew.type == ComponentType::Button) {
						 if (valueNew.emoji.id == "") {
							 json component = { {"custom_id", valueNew.customId},
							 {"disabled", valueNew.disabled},
							 {"emoji",{
								 {"name", valueNew.emoji.name},
							 {"animated", valueNew.emoji.animated}
						 } },
							 {"label", valueNew.label},
							 {"style", valueNew.style},
							 {"type", valueNew.type},
							 {"url", valueNew.url}
							 };
							 components.push_back(component);
						 }
						 else {
							 json component = { {"custom_id", valueNew.customId},
							 {"disabled", valueNew.disabled},
							 {"emoji",{
								 {"name", valueNew.emoji.name},
								 {"id", valueNew.emoji.id},
							 {"animated", valueNew.emoji.animated}
						 } },
							 {"label", valueNew.label},
							 {"style", valueNew.style},
							 {"type", valueNew.type},
							 {"url", valueNew.url}
							 };
							 components.push_back(component);
						 }
					 }
					 else if (valueNew.type == ComponentType::SelectMenu) {
						 json optionsArray = json::array();
						 for (auto value01 : valueNew.options) {
							 if (value01.emoji.id == "" && value01.emoji.name == "") {
								 json option = { {"description",value01.description},
								 {"label", value01.label},
								 {"value", value01.value},{"default", value01._default} };
								 optionsArray.push_back(option);

							 }
							 else if (value01.emoji.id == "") {
								 json option = { {"description",value01.description},
								 {"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
								 {"label", value01.label},
								 {"value", value01.value},{"default", value01._default} };
								 optionsArray.push_back(option);
							 }
							 else {
								 json option = { {"description",value01.description},
								 {"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
								 {"label", value01.label},
								 {"value", value01.value},{"default", value01._default} };
								 optionsArray.push_back(option);
							 }
						 };
						 {
							 json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
							 {"disabled", valueNew.disabled},
							 {"type", valueNew.type},
								 {"max_values", valueNew.maxValues},
								 {"min_values", valueNew.minValues},
								 {"placeholder", valueNew.placeholder }
							 };
							 components.push_back(component);
						 }
					 }
				 }
				 json componentActionRow = { {"type", 1},{
						 "components", components} };
				 componentsActionRow.push_back(componentActionRow);
			 }

			 if (dataPackage.content == "") {
				 json data = {
					 {"tts", dataPackage.tts},
					 {"flags", dataPackage.flags},
				 { "embeds", embedsArray },
				 { "allowed_mentions", {
				 {"parse", parseArray},
			 {"roles", rolesArray},
			 {"users", usersArray},
			 {"repliedUser", dataPackage.allowedMentions.repliedUser},

		 } }, {"components", componentsActionRow}
				 };

				 return data.dump();
			 }
			 else {
				 json data = { {"content", dataPackage.content},
					 {"flags", dataPackage.flags},
					 {"tts", dataPackage.tts},
				 {"embeds", embedsArray},
				 {"allowed_mentions", {
				 {"parse", parseArray},
			 {"roles", rolesArray},
			 {"users", usersArray},
			 {"repliedUser", dataPackage.allowedMentions.repliedUser}

		 } }, {"components", componentsActionRow} };

				 return data.dump();
			 };

		 }

		 string getDeleteMessagesBulkPayload(DeleteMessagesBulkData dataPackage) {
			 auto messageIdArray = json::array();

			 for (auto value : dataPackage.messageIds) {
				 messageIdArray.push_back(value);
			 }
			 json data = { {"messages", messageIdArray} };

			 return data.dump();
		 }
}
#endif