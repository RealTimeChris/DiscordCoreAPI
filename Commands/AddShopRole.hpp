// AddShopRole.hpp - Header for the "add shop role" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ADD_SHOP_ROLE_
#define _ADD_SHOP_ROLE_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class AddShopRole :public BaseFunction {
	public:
		AddShopRole() {
			this->commandName = "addshoprole";
			this->helpDescription = "__**Add Shop Role Usage:**__ Enter !addshoprole = NAME, HEXCOLORVALUE, COST or /addshoprole NAME, HEXCOLORVALUE, COST.";
		}
		virtual task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(args->eventData, channel);

				if (areWeInADm == true) {
					co_return;
				}

				InputEventManager::deleteInputEventResponse(args->eventData);

				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild.data);

				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

				bool doWeHaveAdmin = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					co_return;
				}
				
				bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData,  discordGuild);

				if (areWeAllowed == false) {
					co_return;
				}

				InputEventData event01;
				regex nameRegExp(".{1,36}");
				regex hexColorRegExp(".{1,24}");
				regex costRegExp("\\d{1,8}");
				if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray.at(0), nameRegExp)) {
					string msgString = "------\n**Please enter a proper role name! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateEphemeralInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						auto event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}
				if (args->argumentsArray.size() < 2 || !regex_search(args->argumentsArray.at(1), hexColorRegExp)) {
					string msgString = "------\n**Please enter a valid hex color value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateEphemeralInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						auto event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}
				if (args->argumentsArray.size() < 3 || !regex_search(args->argumentsArray.at(2), costRegExp) || stoll(args->argumentsArray.at(2)) <= 0) {
					string msgString = "------\n**Please enter a valid cost value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateEphemeralInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						auto event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}

				cmatch matchResults;
				regex_search(args->argumentsArray.at(0).c_str(), matchResults, nameRegExp);
				string roleName = matchResults.str();
				regex_search(args->argumentsArray.at(1).c_str(), matchResults, hexColorRegExp);
				string roleColor = matchResults.str();
				regex_search(args->argumentsArray.at(2).c_str(), matchResults, costRegExp);
				unsigned int roleCost = (unsigned int)stoll(matchResults.str());

				for (auto& value : discordGuild.data.guildShop.roles) {
					if (roleName == value.roleName) {
						string msgString = "------\n**Sorry, but a role by that name already exists!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(msgEmbed);
							event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateEphemeralInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(msgEmbed);
							auto event = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event, 20000);
						}
						
						co_return;
					}
				}

				string rolePermsString;
				vector<Permissions> permissions;
				permissions.push_back(Permissions::CREATE_INSTANT_INVITE);
				permissions.push_back(Permissions::ADD_REACTIONS);
				permissions.push_back(Permissions::VIEW_CHANNEL);
				permissions.push_back(Permissions::SEND_MESSAGES);
				permissions.push_back(Permissions::CHANGE_NICKNAME);
				permissions.push_back(Permissions::USE_EXTERNAL_EMOJIS);
				permissions.push_back(Permissions::CONNECT);
				permissions.push_back(Permissions::EMBED_LINKS);
				permissions.push_back(Permissions::ATTACH_FILES);
				permissions.push_back(Permissions::SPEAK);
				permissions.push_back(Permissions::USE_SLASH_COMMANDS);
				permissions.push_back(Permissions::READ_MESSAGE_HISTORY);
				permissions.push_back(Permissions::STREAM);
				permissions.push_back(Permissions::REQUEST_TO_SPEAK);
				permissions.push_back(Permissions::SEND_TTS_MESSAGES);
				permissions.push_back(Permissions::USE_PRIVATE_THREADS);
				permissions.push_back(Permissions::USE_VAD);

				rolePermsString = DiscordCoreAPI::PermissionsConverter::addPermissionsToString(rolePermsString, permissions);

				CreateRoleData createRoleData;
				createRoleData.hexColorValue = roleColor;
				createRoleData.guildId = args->eventData.getGuildId();
				createRoleData.hoist = true;
				createRoleData.mentionable = true;
				createRoleData.name = roleName;
				createRoleData.permissions = rolePermsString;
				Role role = args->eventData.discordCoreClient->roles->createRoleAsync(createRoleData).get();
				if (role.data.id == "") {
					throw exception("Role not initialized!");
				}
				InventoryRole currentRole;
				currentRole.roleCost = roleCost;
				currentRole.roleId = role.data.id;
				currentRole.roleName = role.data.name;

				discordGuild.data.guildShop.roles.push_back(currentRole);
				discordGuild.writeDataToDB();
				string msgString = "";
				msgString = "Nicely done! You've added a new role to the store's inventory, giving the server access to it!\nIt is as follows:\n------\n__**Role:**__ <@&" + currentRole.roleId + "> __**Cost**__ : " + to_string(roleCost) + " "
					+  args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Role Added:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(responseData);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(responseData);
				}
				co_return;
			}
			catch (exception& e) {
				cout << "AddShopRole::execute() Error: " << e.what() << endl;
			}
		}
	};
	AddShopRole addShopRole{};
}
#endif
