// RemoveShopRole.hpp - Header for the "remove shop role" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REMOVE_SHOP_ROLE_
#define _REMOVE_SHOP_ROLE_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class RemoveShopRole :public BaseFunction {
	public:
		RemoveShopRole() {
			this->commandName = "removeshoprole";
			this->helpDescription = "__**Remove Shop Role Usage:**__ !removeshoprole = ROLENAME or /removeshoprole ROLENAME";
		}
		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();
			bool areWeInAm = areWeInADM(args->eventData, channel);

			if (areWeInAm == true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId() , args->eventData.getAuthorId() }).get();

			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				co_return;
			}

			regex roleNameRegExp(".{1,36}");
			regex roleIdRegExp("\\d{1,18}");
			if (args->argumentsArray.size() == 0 || (!regex_search(args->argumentsArray.at(0), roleNameRegExp) && !regex_search(args->argumentsArray.at(0), roleIdRegExp))) {
				string msgString = "------\n**Please enter a valid role name!(!removeshoprole = ROLENAME)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				co_return;
			}

			string roleName = args->argumentsArray.at(0);

			string roleID;
			cmatch matchResults;
			regex_search(args->argumentsArray.at(0).c_str(), matchResults, roleIdRegExp);
			roleID = matchResults.str();
			string realRoleName;
			
			bool isRoleFound = false;
			for (unsigned int x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
				if (roleName == discordGuild.data.guildShop.roles[x].roleName|| roleID == discordGuild.data.guildShop.roles[x].roleId) {
					roleID = discordGuild.data.guildShop.roles[x].roleId;
					realRoleName = discordGuild.data.guildShop.roles[x].roleName;
					isRoleFound = true;
					discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + x);
					discordGuild.writeDataToDB();
					break;
				}
			}

			string msgString;

			if (isRoleFound == false) {
				msgString = "------\n**Sorry, but that role was not found in the shop's inventory!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Role Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				co_return;
			}
			
			args->eventData.discordCoreClient->roles->removeRoleFromGuildAsync({ .guildId = args->eventData.getGuildId(),.roleId = roleID }).get();

			msgString += "You've just deleted a role from the shop / server!\n------\n__**Role Name:**__ " + realRoleName + "\n------";

			EmbedData messageEmbed;
			messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			messageEmbed.setDescription(msgString);
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setTitle("__**Shop/Server Role Deleted:**__");
			messageEmbed.setColor(discordGuild.data.borderColor);
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.embeds.push_back(messageEmbed);
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.embeds.push_back(messageEmbed);
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
		
			co_return;
		}
	};

	RemoveShopRole removeShopRole;
}

#endif