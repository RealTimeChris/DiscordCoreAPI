// Inventory.hpp - Header for the "inventory" command.
// Jun 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INVENTORY_
#define _INVENTORY_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Inventory :public BaseFunction {
	public:
		Inventory() {
			this->commandName = "inventory";
			this->helpDescription = "__**Inventory Usage:**__ !inventory or /inventory.";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();
			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm == true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (areWeAllowed == false) {
				co_return;
			}

			string userID;

			regex userIDRegExp(".{2,3}\\d{18}>");
			regex idRegExp("\\d{18}");
			if (args->argumentsArray.size() == 0) {
				userID = args->eventData.getAuthorId();
			}
			else if (!regex_search(args->argumentsArray.at(0), userIDRegExp) && !regex_search(args->argumentsArray.at(0), idRegExp)) {
				string msgString = "------\n**Sorry, please enter a valid user mention or user ID!(!inventory = @USERMENTION, or just !inventory)**\n------";
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
				string argZero = args->argumentsArray.at(0);
				cmatch userIDMatch;
				regex_search(argZero.c_str(), userIDMatch, idRegExp);
				string userIDOne = userIDMatch.str();
				userID = userIDOne;
			}

			GuildMember currentGuildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = userID }).get();

			if (currentGuildMember.data.user.username == "") {
				string msgString = "-------\n**Sorry, but that user could not be found!**\n------";
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

			DiscordGuildMember discordGuildMember(currentGuildMember.data);

			if (currentGuildMember.data.user.username == "") {
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

			string userName = currentGuildMember.data.user.username;

			vector<Role> rolesArray = args->eventData.discordCoreClient->roles->getGuildRolesAsync({ .guildId = args->eventData.getGuildId() }).get();

			for (unsigned int x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
				bool isRoleFound = false;
				InventoryRole userRole = discordGuildMember.data.roles[x];
				for (auto value2 : rolesArray) {
					if (value2.data.id == userRole.roleId) {
						isRoleFound = true;
						break;
					}
				}
				if (isRoleFound == false) {
					discordGuildMember.data.roles.erase(discordGuildMember.data.roles.begin() + x);
					discordGuildMember.writeDataToDB();
					string msgString = "------\n**Removing role " + userRole.roleName + " from user cache!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Role Issue:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						auto event02 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event02, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateDeferredInteractionResponseData responseData(args->eventData);
						auto event02 = InputEventManager::respondToEvent(responseData);
						CreateFollowUpMessageData responseData2(args->eventData);
						responseData2.embeds.push_back(msgEmbed);
						event02 = InputEventManager::respondToEvent(responseData2);
						InputEventManager::deleteInputEventResponse(event02, 20000);
					}
					x -= 1;
				}
			}

			vector<string> itemsMsgString;
			unsigned int currentPage = 0;

			for (unsigned int x = 0; x < discordGuildMember.data.items.size(); x+= 1) {
				if (x == 0) {
					itemsMsgString.push_back(""); 
				}
				string itemsMsgStringTemp = "";
				itemsMsgStringTemp = "**| __Item:__** " + discordGuildMember.data.items[x].emoji + discordGuildMember.data.items[x].itemName + " **| __Value:__** " + to_string(discordGuildMember.data.items[x].itemCost) + " **| __Self-Mod:__** " +
					to_string(discordGuildMember.data.items[x].selfMod) + " **| __Opp-Mod:__** " + to_string(discordGuildMember.data.items[x].oppMod) + "\n";
				if (itemsMsgStringTemp.length() + itemsMsgString[currentPage].length() >= 2048) {
					currentPage += 1;
					itemsMsgString.push_back("");
				}
				itemsMsgString[currentPage] += itemsMsgStringTemp;
			}
			
			unsigned int currentPage2 = 0;
			vector<string> rolesMsgStrings;

			for (unsigned int x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
				if (x == 0) {
					rolesMsgStrings.push_back("");
				}
				string rolesMsgStringTemp = "";
				rolesMsgStringTemp = "**| __Role:__** <@&" + discordGuildMember.data.roles.at(x).roleId + "> **| __Value:__** " + to_string(discordGuildMember.data.roles.at(x).roleCost) + "\n";
				if (rolesMsgStringTemp.length() + rolesMsgStrings[currentPage2].length() > 2048) {
					currentPage2 += 1;
					rolesMsgStrings.push_back("");
				}				
				rolesMsgStrings[currentPage2] += rolesMsgStringTemp;
			}

			vector<EmbedData> itemsMessageEmbeds;

			for (unsigned int x = 0; x < itemsMsgString.size(); x += 1) {
				EmbedData newEmbed;
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**" + userName + "'s Inventory (Items) Page " + to_string(x + 1) + " of " + to_string(itemsMsgString.size() + rolesMsgStrings.size()) + ":**__");
				newEmbed.setDescription(itemsMsgString[x]);
				newEmbed.setColor(discordGuild.data.borderColor);
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				itemsMessageEmbeds.push_back(newEmbed);
			}
			
			vector<EmbedData> rolesMsgEmbeds;

			for (unsigned int x = 0; x < rolesMsgStrings.size(); x += 1) {
				EmbedData newEmbed;
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**" + userName + "'s Inventory (Roles) Page " + to_string(itemsMessageEmbeds.size() + x + 1) + " of " + to_string(itemsMsgString.size() + rolesMsgStrings.size()) + ":**__");
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription(rolesMsgStrings[x]);
				newEmbed.setColor(discordGuild.data.borderColor);
				rolesMsgEmbeds.push_back(newEmbed);
			}
			
			vector<EmbedData> finalMsgEmbedsArray;
			for (auto value : itemsMessageEmbeds) {
				finalMsgEmbedsArray.push_back(value);
			}
			for (auto value : rolesMsgEmbeds) {
				finalMsgEmbedsArray.push_back(value);
			}

			if (rolesMsgEmbeds.size()== 0 && itemsMessageEmbeds.size()==  0) {
				string msgString = "";
				msgString = "Sorry, but the specified user, (<@!" + userID + ">) has no inventory!";
				EmbedData messageEmbed;
				messageEmbed.setDescription(msgString);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setTitle("__**Empty Inventory:**__");
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(messageEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(messageEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				co_return;
			}
			
			unsigned int currentPageIndex = 0;
			InputEventData newEvent;
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.embeds.push_back(finalMsgEmbedsArray[currentPageIndex]);
				newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateDeferredInteractionResponseData responseData01(args->eventData);
				newEvent = InputEventManager::respondToEvent(responseData01);
				CreateFollowUpMessageData responseData(newEvent);
				responseData.embeds.push_back(finalMsgEmbedsArray[currentPageIndex]);
				newEvent = InputEventManager::respondToEvent(responseData);
			}

			
			recurseThroughMessagePages(userID, newEvent, currentPageIndex, finalMsgEmbedsArray, true, 120000);
			co_return;
		}
	};

	Inventory inventory;
}
#endif