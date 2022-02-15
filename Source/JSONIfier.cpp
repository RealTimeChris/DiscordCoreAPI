// JSONIfier.cpp - Source file for all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "JSONIfier.hpp"
#include "GuildEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "ChannelEntities.hpp"
#include "MessageEntities.hpp"
#include "RoleEntities.hpp"
#include "WebHookEntities.hpp"
#include "ReactionEntities.hpp"
#include "InteractionEntities.hpp"
#include "ApplicationCommandEntities.hpp"
#include "UserEntities.hpp"
#include "ThreadEntities.hpp"
#include "GuildScheduledEventEntities.hpp"

namespace DiscordCoreInternal {

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData) {
		auto permOws = nlohmann::json::array();
		for (auto& [key, value] : channelData.channelData.permissionOverwrites) {
			nlohmann::json newData = { {"type", value.type},{"id", value.id}, {"deny", static_cast<std::string>(value.deny.getCurrentPermissionString())}, {"allow", static_cast<std::string>(value.allow.getCurrentPermissionString())}, {"channel_id",value.channelId} };
			permOws.push_back(newData);
		}

		nlohmann::json data = {
			{"permission_overwrites", permOws},
			{"video_quality_mode",channelData.channelData.videoQualityMode},
			{"rate_limit_per_user",channelData.channelData.rateLimitPerUser},
			{"user_limit",channelData.channelData.userLimit},
			{"position", channelData.channelData.position},
			{"parent_id", channelData.channelData.parentId},
			{"bitrate", channelData.channelData.bitrate},
			{"topic",channelData.channelData.topic},
			{"nsfw", channelData.channelData.nsfw},
			{"name", channelData.channelData.name},
			{"parent_id", channelData.channelData.parentId},
			{"rtc_region", channelData.channelData.rtcRgion},
			{"type", channelData.channelData.type},
			{"default_auto_archive_duration", channelData.channelData.defaultAutoArchiveDuration}
		};
		
		return data.dump();
	}

	nlohmann::json JSONIFY(std::string botToken, int32_t intents) {
		nlohmann::json data;
		data = {
				{"op", 2},
				{"d", {
				{"token", botToken},
				{"properties", {
					{"$os", "Windows"},
					{"$browser", "DiscordCoreAPI"},
					{"$device", "DiscordCoreAPI"}
				}},
				{"compress", false},
				{"large_threshold", 250},
				{"shard", {0, 1}},
				{"intents", intents}
				}}
		};
		return data;
	};

	std::string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage) {
		nlohmann::json data;
		if (dataPackage.deleteMessageDays != 0) {
			nlohmann::json dataNew = { {"delete_message_days", dataPackage.deleteMessageDays} };
			data.update(dataNew);
		}
		if (dataPackage.reason != "") {
			nlohmann::json dataNew = { {"reason", dataPackage.reason} };
			data.update(dataNew);
		}
		return data.dump();
	}

	std::vector<uint8_t> JSONIFY(bool isSpeaking, int32_t ssrc, int32_t delay) {
		nlohmann::json data;
		if (isSpeaking) {
			data = {
				{"op", 5,},
				{"d" ,{ {"speaking",1},
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
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	nlohmann::json JSONIFY(int32_t lastReceivedNumber) {
		nlohmann::json data = {
			{"d", lastReceivedNumber},
			{"op", 1}
		};
		return data;
	};

	std::vector<uint8_t> JSONIFY(int64_t nonce) {
		nlohmann::json data = {
			{"d", nonce},
			{"op", int32_t(3)}
		};
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	};

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage) {

		auto activitiesArray = nlohmann::json::array();

		for (auto &value : dataPackage.activities) {
			nlohmann::json dataNew = { { "name",value.name},
				{"type" ,value.type}
			};
			if (value.url != "") {
				nlohmann::json dataNewer = { {"url",value.url } };
				dataNew.update(dataNewer);
			}
			activitiesArray.push_back(dataNew);
		}
		nlohmann::json data = { {"op", 3},{"d" , {{
			"since", dataPackage.since},
			{"activities" ,activitiesArray} ,
			{"status" ,dataPackage.status},
			{"afk" ,dataPackage.afk}}}
		};

		return data;
	}

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage) {
		nlohmann::json data =
		{
		{"op",4},
		{"d" , {
			{"guild_id", dataPackage.guildId},
			{"self_mute" , dataPackage.selfMute},
			{"self_deaf", dataPackage.selfDeaf}
		}} };
		if (dataPackage.channelId == "") {
			nlohmann::json dataNew = { {"channel_id", nullptr} };
			data.at("d").update(dataNew);
		}
		else {
			nlohmann::json dataNew = { {"channel_id", dataPackage.channelId} };
			data.at("d").update(dataNew);
		}

		return data;
	}

	std::vector<uint8_t> JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02) {
		nlohmann::json data = {
			{"d" , {
			{"user_id" , dataPackage02.userId},
			{"server_id", dataPackage02.guildId},
			{"session_id" , dataPackage.sessionId},
			{"token" , dataPackage.token}
			}},{"op", 0}
		};
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	nlohmann::json JSONIFY(std::string serverId, std::string sessionId, std::string token, std::string testString) {
		nlohmann::json data = { {"op", 7},
			{"d" , {{
				"server_id", serverId},
			{"session_id" , sessionId},
			{"token" , token}
			}}
		};
		return data;
	}

	std::vector<uint8_t> JSONIFY(std::string localPort, std::string localIp, std::string encryptionMode, int32_t testValue) {
		nlohmann::json data = {
			{"op", 1},
			{"d" , {
				{"protocol", "udp"},
			{"data" ,{
				{"address", localIp},
			{"port" , stol(localPort)},
			{"mode" , encryptionMode}

		}} } } };
		std::vector<uint8_t> newVector{};
		std::string newString = data.dump();
		newString.shrink_to_fit();
		newVector.insert(newVector.begin(), newString.begin(), newString.end());
		return newVector;
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage) {
		nlohmann::json roleIdArray = nlohmann::json::array();

		for (auto& value : dataPackage.roleIds) {
			roleIdArray.push_back(value);
		}

		nlohmann::json data = { {"nick",dataPackage.nick} };

		if (roleIdArray.size() == 0) {
			nlohmann::json dataNew = { { "roles", nullptr } };
			data.update(dataNew);
		}
		else {
			nlohmann::json dataNew = { { "roles", roleIdArray} };
			data.update(dataNew);
		}

		if (dataPackage.newVoiceChannelId != "") {
			nlohmann::json dataNew = { {"mute", dataPackage.mute},
				{"deaf", dataPackage.deaf},
				{"channel_id", dataPackage.newVoiceChannelId} };
			data.update(dataNew);
		}
		else {
			nlohmann::json dataNew = { {"mute", dataPackage.mute},
				{"deaf", dataPackage.deaf},
				{"channel_id", nullptr} };
			data.update(dataNew);
		}

		return data.dump();
	}

	nlohmann::json JSONIFY(std::string botToken, std::string sessionID, int32_t lastReceivedNumber) {
		nlohmann::json data = {
			{"op", 6},
			{"d" , {
				{"token", botToken},
				{"session_id" , sessionID},
				{"seq" , lastReceivedNumber}
			}}
		};
		return data;
	};

	int32_t JSONIFY(std::string initialPayload) {
		nlohmann::json jsonVal = nlohmann::json::parse(initialPayload);
		int32_t finalValue = 0;
		if (jsonVal.contains("heartbeat_interval")) {
			finalValue = jsonVal.at("heartbeat_interval");
		}
		return finalValue;
	};

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage) {
		auto componentsActionRow = nlohmann::json::array();

		for (auto& value : dataPackage.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
						if (value01.emoji.name != "") {
							nlohmann::json dataNew = { {"emoji", {{"name",value01.emoji.name},{"animated", value01.emoji.animated}}} };
							option.update(dataNew);
						}
						if (value01.emoji.id != "") {
							nlohmann::json dataNew = { {"id",value01.emoji.id} };
							option.at("emoji").update(dataNew);
						}
						optionsArray.push_back(option);
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}
		auto stickersArray = nlohmann::json::array();

		for (auto& value : dataPackage.stickerIds) {
			stickersArray.push_back(value);
		}

		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream{};
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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

		nlohmann::json data = {
					{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
				{"components", componentsActionRow},
					{"sticker_ids", stickersArray},
					{"content", dataPackage.content},
			{"tts" , dataPackage.tts}
		};

		if (dataPackage.messageReference.guildId != "") {
			nlohmann::json dataNew = {
			{"message_reference",{{"message_id", dataPackage.messageReference.messageId},
		{"channel_id", dataPackage.messageReference.channelId},
		{"fail_if_not_exists", dataPackage.messageReference.failIfNotExists},
		}} };
			data.update(dataNew);
		}
			

		if (dataPackage.embeds.size() > 0) {
			nlohmann::json dataNew = { {"embeds", embedsArray } };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage) {
		auto componentsActionRow = nlohmann::json::array();

		for (auto& value : dataPackage.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
						if (value01.emoji.name != "") {
							nlohmann::json dataNew = { {"emoji", {{"name",value01.emoji.name},{"animated", value01.emoji.animated}}} };
							option.update(dataNew);
						}
						if (value01.emoji.id != "") {
							nlohmann::json dataNew = { {"id",value01.emoji.id} };
							option.at("emoji").update(dataNew);
						}
						optionsArray.push_back(option);
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			 int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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
			};
			embedsArray.push_back(embed);
		}

		auto attachments = nlohmann::json::array();

		for (auto& value : dataPackage.attachments) {
			nlohmann::json attachment = { {"content_type", value.contentType},
				{"file_name",value.filename},
				{"height",value.height},
				{"id",value.id},
				{"proxy_url",value.proxyUrl},
				{"size",value.size},
				{"url",value.url},
				{"width",value.width} };
			attachments.push_back(attachment);
		}

		nlohmann::json data = {
			{"allowed_mentions", {
				{"parse", dataPackage.allowedMentions.parse},
				{"replied_user", dataPackage.allowedMentions.repliedUser},
				{"roles", dataPackage.allowedMentions.roles},
				{"users", dataPackage.allowedMentions.users}
				}},
			{"attachments", attachments},
			{"components", componentsActionRow},
			{"embeds" ,embedsArray},
			{"content", dataPackage.content},
			{"flags", dataPackage.flags}
		};

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage) {
		std::stringstream stream;
		 int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();

		nlohmann::json data = {
			{"color", roleColorReal},
			{"hoist", dataPackage.hoist},
			{"mendtionable", dataPackage.mentionable},
			{"name", dataPackage.name},
			{"permissions", stol(dataPackage.permissions)}
		};

		return data.dump();
	}

	void JSONIFY(DiscordCoreAPI::ApplicationCommandOptionData dataPackage, nlohmann::json* pJSONData) {
		nlohmann::json newOption;
		newOption.emplace(std::make_pair("description", dataPackage.description));
		newOption.emplace(std::make_pair("name", dataPackage.name));
		newOption.emplace(std::make_pair("type", dataPackage.type));
		if (dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && dataPackage.type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
			newOption.emplace(std::make_pair("required", dataPackage.required));
		}
		nlohmann::json dataArray;
		newOption.emplace(std::make_pair("options", dataArray));
		newOption.emplace(std::make_pair("choices", dataArray));
		if (dataPackage.choices.size() > 0) {
			for (int32_t y = 0; y < dataPackage.choices.size(); y += 1) {
				DiscordCoreAPI::ApplicationCommandOptionChoiceData choiceData;
				choiceData.name = dataPackage.choices.at(y).name;
				if (dataPackage.choices.at(y).valueString != "") {
					choiceData.valueString = dataPackage.choices.at(y).valueString;
					nlohmann::json jsonValue = { {"name", choiceData.name},{"value", choiceData.valueString} };
					newOption.at("choices").emplace_back(jsonValue);
				}
				else if (dataPackage.choices.at(y).valueInt != 0) {
					choiceData.valueInt = dataPackage.choices.at(y).valueInt;
					nlohmann::json jsonValue = { {"name", choiceData.name},{"value", choiceData.valueInt} };
					newOption.at("choices").emplace_back(jsonValue);
				}
			}
		}
		if (dataPackage.options.size() > 0) {
			for (auto& value : dataPackage.options) {
				JSONIFY(value, &newOption.at("options"));
			}
		}
		pJSONData->emplace_back(newOption);
		return;
	}

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage) {
		nlohmann::json newDataArray = nlohmann::json::array();

		for (auto& value : dataPackage.permissions) {
			nlohmann::json newData = {
				{"id",value.id},
				{"permission",value.permission},
				{"type", value.type}
			};
			newDataArray.push_back(newData);
		}

		return newDataArray.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage) {
		nlohmann::json newDataArray = nlohmann::json();

		for (auto& value : dataPackage.permissions) {
			nlohmann::json newData01 = nlohmann::json::array();
			for (auto& value01 : value.permissions) {
				newData01.push_back({ {"id", value01.id}, {"permission",value01.permission},{"type",value01.type} });
			}
			newDataArray.push_back({ {"id", value.id}, {"permissions",newData01} });
		}

		return newDataArray.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage) {
		nlohmann::json data;
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandType::Message || dataPackage.type == DiscordCoreAPI::ApplicationCommandType::User) {
			data = { {"name", dataPackage.name},{"type", dataPackage.type} };
		}
		else {
			data = {
			{"name",dataPackage.name },
			{"description", dataPackage.description},
			{"default_permission", dataPackage.defaultPermission},
			{"type", dataPackage.type}
			};

			nlohmann::json arrayValue{};

			data.emplace(std::make_pair("options", arrayValue));

			if (dataPackage.options.size() > 0) {
				for ( int32_t x = 0; x < dataPackage.options.size(); x += 1) {
					JSONIFY(dataPackage.options.at(x), &data.at("options"));
				}
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage) {
		nlohmann::json data;
		if (dataPackage.type == DiscordCoreAPI::ApplicationCommandType::Message || dataPackage.type == DiscordCoreAPI::ApplicationCommandType::User) {
			data = { {"name", dataPackage.name},{"type", dataPackage.type} };
		}
		else {
			data = {
			{"name",dataPackage.name },
			{"description", dataPackage.description},
			{"default_permission", dataPackage.defaultPermission},
			{"type", dataPackage.type}
			};

			nlohmann::json arrayValue{};

			data.emplace(std::make_pair("options", arrayValue));

			if (dataPackage.options.size() > 0) {
				for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
					JSONIFY(dataPackage.options.at(x), &data.at("options"));
				}
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage) {
		nlohmann::json data = {
			{"name",dataPackage.name },
			{"description", dataPackage.description},
			{"default_permission", dataPackage.defaultPermission}
		};

		nlohmann::json arrayValue;

		data.emplace(std::make_pair("options", arrayValue));

		if (dataPackage.options.size() > 0) {
			for ( int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options.at(x), &data.at("options"));
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage) {
		nlohmann::json data = {
			{"name",dataPackage.name },
			{"description", dataPackage.description},
			{"default_permission", dataPackage.defaultPermission}
		};

		nlohmann::json arrayValue;

		data.emplace(std::make_pair("options", arrayValue));

		if (dataPackage.options.size() > 0) {
			for (int32_t x = 0; x < dataPackage.options.size(); x += 1) {
				JSONIFY(dataPackage.options.at(x), &data.at("options"));
			}
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::EditFollowUpMessageData dataPackage) {
		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			 int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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
			};
			embedsArray.push_back(embed);
		}

		auto parseArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.parse) {
			parseArray.push_back(value);
		};

		auto rolesArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.roles) {
			rolesArray.push_back(value);
		}

		auto usersArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.users) {
			usersArray.push_back(value);
		}

		auto componentsActionRow = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
						if (value01.emoji.name != "") {
							nlohmann::json dataNew = { {"emoji", {{"name",value01.emoji.name},{"animated", value01.emoji.animated}}} };
							option.update(dataNew);
						}
						if (value01.emoji.id != "") {
							nlohmann::json dataNew = { {"id",value01.emoji.id} };
							option.at("emoji").update(dataNew);
						}
						optionsArray.push_back(option);
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		if (dataPackage.data.data.content == "") {
			nlohmann::json data = {
			{"embeds", embedsArray},
			{"allowed_mentions",
			{{"parse", parseArray},
	{"roles", rolesArray},
	{"users", usersArray},
	{"repliedUser", dataPackage.data.data.allowedMentions.repliedUser}}},
				{"components", componentsActionRow} };
			return data.dump();
		}
		else {
			nlohmann::json data = { {"content", dataPackage.data.data.content},
				{"embeds", embedsArray},
				{"allowed_mentions",
				{{"parse", parseArray},
		{"roles", rolesArray},
		{"users", usersArray},
		{"repliedUser", dataPackage.data.data.allowedMentions.repliedUser}},
			},{"components", componentsActionRow} };
			return data.dump();
		};
	}

	std::string	JSONIFY(DiscordCoreAPI::EditInteractionResponseData dataPackage) {
		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			 int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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
			};
			embedsArray.push_back(embed);
		}

		auto parseArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.parse) {
			parseArray.push_back(value);
		};

		auto rolesArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.roles) {
			rolesArray.push_back(value);
		}

		auto usersArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.users) {
			usersArray.push_back(value);
		}

		auto componentsActionRow = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
						if (value01.emoji.name != "") {
							nlohmann::json dataNew = { {"emoji", {{"name",value01.emoji.name},{"animated", value01.emoji.animated}}} };
							option.update(dataNew);
						}
						if (value01.emoji.id != "") {
							nlohmann::json dataNew = { {"id",value01.emoji.id} };
							option.at("emoji").update(dataNew);
						}
						optionsArray.push_back(option);
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		std::cout << "SIZE OF THE ARRAY: " << embedsArray.size() << std::endl;

		nlohmann::json data = { {"type", dataPackage.data.type},
				{"tts", dataPackage.data.data.tts},
				{"data",{{"embeds", embedsArray},
				{"flags", dataPackage.data.data.flags },
			{"title", dataPackage.data.data.title},
			{"custom_id", dataPackage.data.data.customId},
			{"allowed_mentions",
				{{"parse", parseArray},
		{"roles", rolesArray},
		{"users", usersArray},
		{"repliedUser", dataPackage.data.data.allowedMentions.repliedUser}}},
				{"components", componentsActionRow},
				}}
		};

		if (dataPackage.data.data.content != "") {
			nlohmann::json dataNew = { { "content", dataPackage.data.data.content } };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage) {

		nlohmann::json data = { {"type", dataPackage.data.type},{"flags", dataPackage.data.data.flags}
		};
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateInteractionResponseData dataPackage) {

		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			if (value.hexColorValue == "") {
				value.hexColorValue = "fefefe";
			}
			 int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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
			};
			embedsArray.push_back(embed);
		}

		auto parseArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.parse) {
			nlohmann::json parseValue = parseValue.parse(value);
			parseArray.push_back(parseValue);
		};

		auto rolesArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.roles) {
			nlohmann::json parseValue = parseValue.parse(value);
			rolesArray.push_back(parseValue);
		}

		auto usersArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.users) {
			nlohmann::json parseValue = parseValue.parse(value);
			usersArray.push_back(parseValue);
		}

		auto componentsActionRow = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
						if (value01.emoji.name != "") {
							nlohmann::json dataNew = { {"emoji", {{"name",value01.emoji.name},{"animated", value01.emoji.animated}}} };
							option.update(dataNew);
						}
						if (value01.emoji.id != "") {
							nlohmann::json dataNew = { {"id",value01.emoji.id} };
							option.at("emoji").update(dataNew);
						}
						optionsArray.push_back(option);
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		nlohmann::json data = { {"type", dataPackage.data.type},
				{"tts", dataPackage.data.data.tts},
				{"data",{{"embeds", embedsArray},
				{"flags", dataPackage.data.data.flags },
			{"title", dataPackage.data.data.title},
			{"custom_id", dataPackage.data.data.customId},
			{"allowed_mentions",
				{{"parse", parseArray},
		{"roles", rolesArray},
		{"users", usersArray},
		{"repliedUser", dataPackage.data.data.allowedMentions.repliedUser}}},
				{"components", componentsActionRow},
				}}
		};

		if (dataPackage.data.data.content != "") {
			nlohmann::json dataNew = { { "content", dataPackage.data.data.content } };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage) {
		 int32_t roleColorInt = stol(dataPackage.hexColorValue, 0, 16);
		std::stringstream stream;
		stream << std::setbase(10) << roleColorInt;
		std::string roleColorReal = stream.str();

		nlohmann::json data = { {"color", roleColorReal

			},
			{"hoist", dataPackage.hoist},{"permissions", dataPackage.permissions.getCurrentPermissionString()},
			{"mentionable", dataPackage.mentionable},
			{"name", dataPackage.name }
		};
		return data.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage) {
		nlohmann::json dataArray = nlohmann::json::array();

		for (auto& value : dataPackage.rolePositions) {
			nlohmann::json data = { {"id", value.roleId },{"position", value.rolePosition} };
			dataArray.push_back(data);
		}
		return dataArray.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage) {
		nlohmann::json data = { {"allow", stoll(dataPackage.allow)},
			{"deny", stoll(dataPackage.deny)},
			{"type", dataPackage.type} };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateFollowUpMessageData dataPackage) {
		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			 int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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
			};
			embedsArray.push_back(embed);
		}

		auto parseArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.parse) {
			parseArray.push_back(value);
		};

		auto rolesArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.roles) {
			rolesArray.push_back(value);
		}

		auto usersArray = nlohmann::json::array();
		for (auto& value : dataPackage.data.data.allowedMentions.users) {
			usersArray.push_back(value);
		}

		auto componentsActionRow = nlohmann::json::array();

		for (auto& value : dataPackage.data.data.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							nlohmann::json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							nlohmann::json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		nlohmann::json data = { {"type", dataPackage.data.type},
				{"tts", dataPackage.data.data.tts},
				{"data",{{"embeds", embedsArray},
				{"flags", dataPackage.data.data.flags },
			{"title", dataPackage.data.data.title},
			{"custom_id", dataPackage.data.data.customId},
			{"allowed_mentions",
				{{"parse", parseArray},
		{"roles", rolesArray},
		{"users", usersArray},
		{"repliedUser", dataPackage.data.data.allowedMentions.repliedUser}}},
				{"components", componentsActionRow},
				}}
		};
		std::cout << "EMBEDS SIZE 01: " << dataPackage.data.data.embeds.size() << std::endl;
		if (dataPackage.data.data.content != "") {
			nlohmann::json dataNew = { { "content", dataPackage.data.data.content } };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage) {
		auto messageIdArray = nlohmann::json::array();

		for (auto& value : dataPackage.messageIds) {
			messageIdArray.push_back(value);
		}
		nlohmann::json data = { {"messages", messageIdArray} };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage) {
		nlohmann::json data{};
		if (dataPackage.targetUserId != "") {
			data = {
			{"target_application_id", dataPackage.targetApplicationId},
			{"target_type", dataPackage.targetType},
			{"target_user_id", dataPackage.targetUserId},
			{"temporary", dataPackage.temporary},
			{"unique", dataPackage.unique},
			{"max_uses", dataPackage.maxUses},
			{"max_age", dataPackage.maxAge} };
		}
		else {
			data = {
			{"temporary", dataPackage.temporary},
			{"unique", dataPackage.unique},
			{"max_uses", dataPackage.maxUses},
			{"max_age", dataPackage.maxAge} };
		}		
		return data.dump();
		
	};

	std::string JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage) {
		nlohmann::json data{};
		data = { {"webhook_channel_id", dataPackage.targetChannelId} };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage) {
		nlohmann::json data{};
		data = { {"access_token",dataPackage.token},{"nick", dataPackage.nick} };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage) {
		nlohmann::json data{};
		data = { {"name", dataPackage.threadName},{"auto_archive_duration", dataPackage.autoArchiveDuration} };
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage) {
		nlohmann::json data{};

		data = { {"invitable", dataPackage.invitable},
			{"auto_archive_duration", dataPackage.autoArchiveDuration},
			{"name",dataPackage.threadName},
			{"type", dataPackage.type} };
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage) {
		nlohmann::json data{};

		nlohmann::json rolesArray = nlohmann::json::array();
		for (auto& value : dataPackage.roles) {
			rolesArray.push_back(value);
		}

		data = { {"name", dataPackage.name},
			{dataPackage.imageDataFinal, dataPackage.imageData},
			{"roles", rolesArray} };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage) {
		nlohmann::json data{};

		nlohmann::json rolesArray = nlohmann::json::array();
		for (auto& value : dataPackage.roles) {
			rolesArray.push_back(value);
		}

		data = { {"name", dataPackage.name},
			{"roles", rolesArray} };

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage) {
		nlohmann::json data{};

		nlohmann::json channelsArray = nlohmann::json::array();

		for (auto& value : dataPackage.channels) {
			nlohmann::json newData = { {"name", value.name},
				{"type", value.type},
				{"id", value.id},
				{"parent_id", value.parentId } };
			channelsArray.push_back(newData);
		}

		nlohmann::json rolesArray = nlohmann::json::array();

		for (auto& value : dataPackage.roles) {
			nlohmann::json newData = { {"mentionable", value.mentionable },
			{ "permissions", value.permissions.getCurrentPermissionString()},
			{ "position", value.position },
			{ "managed", value.managed },
			{ "hoist", value.hoist },
				{"tags", {{"bot_id",value.tags.botId},{"integration_id",value.tags.integrationId},{"premium_subscriber",value.tags.premiumSubscriber}}},
			{ "color", value.color },
			{ "name", value.name }
			};
			rolesArray.push_back(newData);
		}
		if (dataPackage.afkChannelId != ""){
			nlohmann::json dataNew = { {"afk_channel_id", dataPackage.afkChannelId} };
			data.update(dataNew);
		}
		if (dataPackage.systemChannelId != "") {
			nlohmann::json dataNew = { {"system_channel_id",dataPackage.systemChannelId} };
			data.update(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage) {
		nlohmann::json data{};

		data = { {"default_message_notifications", dataPackage.defaultMessageNotifications},
			{ "explicit_content_filter", dataPackage.explicitContentFilter },
			{ "system_channel_flags", dataPackage.systemChannelFlags },
			{ "verification_level", dataPackage.verificationLevel },
			{ "icon", dataPackage.icon },
			{ "afk_timeout", dataPackage.afkTimeout },
			{ "name", dataPackage.name },
			{ "afk_timeout", dataPackage.afkTimeout },
			{ "banner",dataPackage.banner },
			{ "description",dataPackage.description },
			{ "discovery_splash",dataPackage.discoverySplash },
			{ "features",dataPackage.features },
			{ "owner_id",dataPackage.ownerId },
			{ "preferred_locale",dataPackage.preferredLocale },
			{ "splash",dataPackage.splash } };

		if (dataPackage.afkChannelId != "") {
			nlohmann::json dataNew = { {"afk_channel_id", dataPackage.afkChannelId } };
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
		for (auto& value : dataPackage.permissionOverwrites) {
			nlohmann::json newData = { {"allow",value.allow.getCurrentPermissionString()},
				{"channel_id",value.channelId},
				{"deny",value.deny.getCurrentPermissionString()},
				{"id",value.id},
				{"type",value.type} };
			overwrites.push_back(newData);
		}
		data = { {"name",dataPackage.name},
			{"nsfw",dataPackage.nsfw},
			{"parent_id",dataPackage.parentId},
			{"permission_overwrites",overwrites},
			{"position",dataPackage.position},
			{"rate_limit_per_user",dataPackage.rateLimitPerUser},
			{"topic",dataPackage.topic},
			{"type",dataPackage.type}, };
		if (dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Voice || dataPackage.type == DiscordCoreAPI::ChannelType::Guild_Stage_Voice) {
			nlohmann::json dataNew = { {"bitrate", dataPackage.bitrate},{"user_limit", dataPackage.userLimit} };
			data.update(dataNew);
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage) {
		nlohmann::json data{};

		for (auto& value : dataPackage.modifyChannelData) {
			nlohmann::json dataNew = { {"id",value.id},
				{"lock_permissions",value.lockPermissions},
				{"position",value.position}
			};
			if (value.parentId != "") {
				nlohmann::json dataNewer = { {"parent_id", value.parentId} };
				dataNew.update(dataNewer);
			}
			data.push_back(dataNew);
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage) {
		nlohmann::json data{};

		data = { {"access_token",dataPackage.accessToken},
			{"deaf",dataPackage.deaf},
			{"mute",dataPackage.mute},
			{"nick",dataPackage.nick},
			{"roles",dataPackage.roles} };

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

		for (auto& value : dataPackage.welcomeChannels) {
			nlohmann::json newData = { {"channel_id",value.channelId},
				{"description",value.description},
				{"emoji_name",value.emojiName} };
			if (value.emojiId != "") {
				nlohmann::json dataNewer = { {"emoji_id", value.emojiId} };
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

		data = { {"description",dataPackage.description},
			{"entity_metadata",{{"location",dataPackage.entityMetadata.location}}},
			{"entity_type",dataPackage.entityType},
			{"name",dataPackage.name},
			{"privacy_level",dataPackage.privacyLevel},
			{"scheduled_end_time",dataPackage.scheduledEndTime} ,
			{"scheduled_start_time", dataPackage.scheduledStartTime} };

		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data.update({ {"channel_id",nullptr} });
		}
		else {
			data.update({ {"channel_id",dataPackage.channelId} });
		}
		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage) {
		nlohmann::json data{};

		data = { {"description",dataPackage.description},
			{"entity_metadata",{{"location",dataPackage.entityMetadata.location}}},
			{"entity_type",dataPackage.entityType},
			{"name",dataPackage.name},
			{"privacy_level",dataPackage.privacyLevel},
			{"scheduled_end_time",dataPackage.scheduledEndTime} ,
			{"scheduled_start_time", dataPackage.scheduledStartTime},
			{"status",dataPackage.status} };

		if (dataPackage.entityType == DiscordCoreAPI::GuildScheduledEventEntityType::External) {
			data.update({ {"channel_id",nullptr} });
		}
		else {
			data.update({ {"channel_id",dataPackage.channelId} });
		}

		return data.dump();
	}

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage) {
		nlohmann::json componentsActionRow{};
		for (auto& value : dataPackage.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							nlohmann::json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							nlohmann::json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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

		nlohmann::json data = { {"allowed_mentions",{ {"parse",dataPackage.allowedMentions.parse},
			{"replied_user",dataPackage.allowedMentions.repliedUser},
			{"roles",dataPackage.allowedMentions.roles},
			{"users",dataPackage.allowedMentions.users}}
			},{"avatar_url",dataPackage.avatarUrl},
			{"tts",dataPackage.tts},
			{"username",dataPackage.username}
		};

		if (dataPackage.content != "") {
			data.update({ {"content", dataPackage.content} });
		}

		if (dataPackage.components.size() > 0) {
			data.update({ {"components", componentsActionRow} });
		}

		if (dataPackage.embeds.size() > 0) {
			data.update({ {"embeds", embedsArray} });
		}

		return data.dump();
	};

	std::string JSONIFY(DiscordCoreAPI::EditWebHookMessageData dataPackage) {
		nlohmann::json componentsActionRow{};
		for (auto& value : dataPackage.components) {
			auto components = nlohmann::json::array();

			for (auto& valueNew : value.components) {
				if (valueNew.type == DiscordCoreAPI::ComponentType::Button) {
					nlohmann::json component = { {"custom_id", valueNew.customId},
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
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::TextInput) {
					nlohmann::json component = { {"min_length", valueNew.minLength},
						{"max_length", valueNew.maxLength},
						{"value", valueNew.value},
						{"label", valueNew.label},
						{"placeholder", valueNew.placeholder},
						{"type", valueNew.type},
						{"custom_id", valueNew.customId},
						{"style", valueNew.style},
						{"required", valueNew.required}
					};
					if (valueNew.emoji.id != "") {
						nlohmann::json componentNew = { {"id", valueNew.emoji.id} };
						component.at("emoji").update(componentNew);
					}
					components.push_back(component);
				}
				else if (valueNew.type == DiscordCoreAPI::ComponentType::SelectMenu) {
					nlohmann::json optionsArray = nlohmann::json::array();
					for (auto& value01 : valueNew.options) {
						if (value01.emoji.id == "" && value01.emoji.name == "") {
							nlohmann::json option = { {"description",value01.description},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);

						}
						else if (value01.emoji.id == "") {
							nlohmann::json option = { {"description",value01.description},
							{"emoji", {{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
						else {
							nlohmann::json option = { {"description",value01.description},
							{"emoji", {{"id",value01.emoji.id},{"name", value01.emoji.name},{"animated", value01.emoji.animated}}},
							{"label", value01.label},
							{"value", value01.value},{"default", value01._default} };
							optionsArray.push_back(option);
						}
					};
					{
						nlohmann::json component = { {"custom_id", valueNew.customId},{"options", optionsArray},
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
			nlohmann::json componentActionRow = { {"type", 1},{
					"components", components} };
			componentsActionRow.push_back(componentActionRow);
		}

		auto embedsArray = nlohmann::json::array();

		for (auto& value : dataPackage.embeds) {

			auto fields = nlohmann::json::array();

			for (auto& value2 : value.fields) {
				nlohmann::json field =
				{ {"inline", value2.Inline},
							{"value", value2.value},
							{"name", value2.name} };
				fields.push_back(field);
			}

			int32_t colorValInt = stol(value.hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();

			nlohmann::json embed =
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

		nlohmann::json data = { {"allowed_mentions",{ {"parse",dataPackage.allowedMentions.parse},
			{"replied_user",dataPackage.allowedMentions.repliedUser},
			{"roles",dataPackage.allowedMentions.roles},
			{"users",dataPackage.allowedMentions.users}}
		} };

		if (dataPackage.content != "") {
			data.update({ {"content", dataPackage.content} });
		}

		if (dataPackage.components.size() > 0) {
			data.update({ {"components", componentsActionRow} });
		}

		if (dataPackage.embeds.size() > 0) {
			data.update({ {"embeds", embedsArray} });
		}

		return data.dump();
	};

};
