// RemoveObject.hpp - Header for the "remove object" command.
// Jun 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REMOVE_OBJECT_
#define _REMOVE_OBJECT_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class RemoveObject :public BaseFunction {
	public:
		RemoveObject() {
			this->commandName = "removeobject";
			this->helpDescription = "__**Remove Object Usage:**__ !removeobject = OBJECTNAME or /removeobject OBJECTNAME";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm ==  true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), args->eventData.getAuthorId() }).get();
			DiscordGuildMember discordGuildMember(guildMember.data);

			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				co_return;
			}

			regex idRegExp("\\d{18}");
			cmatch matchResults;
			string roleID;
			string objectName;
			if (args->argumentsArray.size() ==0) {
				string msgString = "------\n**Please enter an object name! (!removeobject = ITEMNAME / ROLENAME, @USERMENTION, or just !removeobject = ITEMNAME / ROLENAME)**\n------";
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
			else if (regex_search(args->argumentsArray.at(0).c_str(), matchResults, idRegExp)) {
				roleID = matchResults.str();
			}
			else {
				objectName = args->argumentsArray.at(0);
			}

			string userID;
			regex userMentionRegExp("<@!\\d{18}>");
			if (args->argumentsArray.size() == 1) {
				userID = args->eventData.getAuthorId();
			}
			else if (!regex_search(args->argumentsArray.at(1), userMentionRegExp) && !regex_search(args->argumentsArray.at(1), idRegExp)) {
				string msgString = "------\n**Please enter a proper usermention argument, or leave it blank! (!removeobject = ITEMNAME / ROLENAME, @USERMENTION, or just !removeobject = ITEMNAME / ROLENAME)**\n------";
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
			else {
				string argOne = args->argumentsArray.at(1);
				cmatch matchResultsNew;
				regex_search(argOne.c_str(), matchResultsNew, idRegExp);
				string userIDOne = matchResultsNew.str();
				userID = userIDOne;
			}

			GuildMember targetMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), userID }).get();

			if (targetMember.data.user.username == "") {
				string msgString = "------\n**Sorry, but that user could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
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

			DiscordGuildMember guildMemberData(targetMember.data);

			if (guildMemberData.data.userName == "") {
				string msgString = "------\n**Sorry, but the specified user data could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
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

			string objectType;
			bool isObjectFound = false;

			for (unsigned int x = 0; x < guildMemberData.data.items.size(); x += 1) {
				if (objectName ==  guildMemberData.data.items[x].itemName) {
					objectType = "item";
					isObjectFound = true;
					guildMemberData.data.items.erase(guildMemberData.data.items.begin() + x);
					guildMemberData.writeDataToDB();
				}
			}

			for (unsigned int x = 0; x < guildMemberData.data.roles.size(); x += 1) {
				if (roleID ==  guildMemberData.data.roles[x].roleId || objectName == guildMemberData.data.roles[x].roleName) {
					objectType = "role";
					isObjectFound = true;
					objectName = guildMemberData.data.roles[x].roleName;
					roleID = guildMemberData.data.roles[x].roleId;
					guildMemberData.data.roles.erase(guildMemberData.data.roles.begin() + x);
					guildMemberData.writeDataToDB();
				}
			}

			EmbedData messageEmbed;

			if (isObjectFound == false) {
				string msgString= "-------\n**Sorry, but the item was not found in the inventory!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Item Issue:**__");
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

			string msgString;

			if (objectType ==  "role") {
				args->eventData.discordCoreClient->roles->removeRoleFromGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .userId = targetMember.data.user.id, .roleId = roleID, }).get();
				msgString = "------\n**You've removed the following role from <@!" + userID + ">'s inventory:**\n------\n __**" + objectName + "**__\n------";
				messageEmbed.setTitle("__**Role Removed:**__");
			}
			else if (objectType == "item") {
				msgString = "------\n**You've removed the following item from <@!" + userID + ">'s inventory:**\n------\n __**" + objectName + "**__\n------";
				messageEmbed.setTitle("__**Item Removed:**__");
			}

			messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL())->setColor(discordGuild.data.borderColor)->setDescription(msgString)->setTimeStamp(getTimeAndDate());
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
	RemoveObject removeObject;
}

#endif