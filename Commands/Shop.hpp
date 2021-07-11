// Shop.hpp - Header for the "shop" command.
// June 2, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SHOP_
#define _SHOP_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Shop : public BaseFunction {
	public:
		Shop() {
			this->commandName = "shop";
			this->helpDescription = "__**Shop Usage:**__ Enter !shop or /shop!";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm ==  true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (areWeAllowed == false) {
				co_return;
			}

			GuildMember botMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.discordCoreClient->currentUser->data.id }).get();
			if (!(DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, Permissions::MANAGE_MESSAGES))) {
				string msgString = "------\n**I need the Manage Messages permission in this channel, for this command!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Permissions Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.embeds.push_back(msgEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(responseData);
					InputEventManager::deleteInputEventResponse(event01, 20000);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(msgEmbed);
					InputEventData event = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(event, 20000);
				}
				co_return;
			}

			vector<Role> rolesArray = args->eventData.discordCoreClient->roles->getGuildRolesAsync({ .guildId = args->eventData.getGuildId() }).get();
			InputEventData event02 = args->eventData;
			
			for (unsigned int x = 0; x < discordGuild.data.guildShop.roles.size(); x+=1) {
				bool isRoleFound = false;
				InventoryRole shopRole = discordGuild.data.guildShop.roles[x];
				for (auto value2:rolesArray) {
					if (value2.data.id == shopRole.roleId) {
						isRoleFound = true;
						break;
					}
				}
				if (isRoleFound == false) {
					discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + x);
					discordGuild.writeDataToDB();
					string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild cache!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Removed Guild Role:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						event02 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event02, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateDeferredInteractionResponseData responseData(args->eventData);
						event02 = InputEventManager::respondToEvent(responseData);
						CreateFollowUpMessageData responseData2(args->eventData);
						responseData2.embeds.push_back(msgEmbed);
						event02 = InputEventManager::respondToEvent(responseData2);
						InputEventManager::deleteInputEventResponse(event02, 20000);
					}
					x -= 1;
				}
			}
			unsigned int maxIdx = 0;
			InventoryItem tempItem;
			unsigned int len = (unsigned int)discordGuild.data.guildShop.items.size();
			for (unsigned int x = 0; x < len; x += 1) {
				maxIdx = x;
				for (unsigned int  y = x + 1; y < len; y += 1) {
					if (discordGuild.data.guildShop.items.at(y).itemCost
				> discordGuild.data.guildShop.items.at(maxIdx).itemCost) {
						maxIdx = y;
					}
				}
				tempItem = discordGuild.data.guildShop.items.at(x);
				discordGuild.data.guildShop.items.at(x) = discordGuild.data.guildShop.items.at(maxIdx);
				discordGuild.data.guildShop.items.at(maxIdx) = tempItem;
			}

			maxIdx = 0;
			InventoryRole tempRole;
			len = (unsigned int)discordGuild.data.guildShop.roles.size();
			for (unsigned int x = 0; x < len; x += 1) {
				maxIdx = x;
				for (unsigned int y = x + 1; y < len; y += 1) {
					if (discordGuild.data.guildShop.roles.at(y).roleCost
				> discordGuild.data.guildShop.roles.at(maxIdx).roleCost) {
						maxIdx = y;
					}
				}
				tempRole = discordGuild.data.guildShop.roles.at(x);
				discordGuild.data.guildShop.roles.at(x) = discordGuild.data.guildShop.roles.at(maxIdx);
				discordGuild.data.guildShop.roles.at(maxIdx) = tempRole;
			}
			
			vector<string> itemsMsgStrings;
			vector<EmbedData> itemsMessageEmbeds;
			unsigned int currentPage = 0;
			bool firstLoop = true;

			for (auto& value:discordGuild.data.guildShop.items) {
				if (firstLoop) {
					itemsMsgStrings.resize(1);
					itemsMessageEmbeds.resize(1);
					firstLoop = false;
				}
				string itemsMsgStringTemp = "**| __Item:__** " + value.emoji + " " + value.itemName + " **| __Cost:__** " + to_string(value.itemCost) + " **| __Self-Mod:__** " + to_string(value.selfMod) + " **| __Opp-Mod:__** " + to_string(value.oppMod) + "\n";
					if (itemsMsgStringTemp.length() + itemsMsgStrings.at(currentPage).length() >= 2048) {
						currentPage += 1;
						itemsMsgStrings.resize(currentPage + 1);
						itemsMessageEmbeds.resize(currentPage + 1);
					}
					itemsMessageEmbeds[currentPage] = *new EmbedData();
					itemsMsgStrings[currentPage] += itemsMsgStringTemp;
			}

			vector<string> rolesMsgStrings;
			vector<EmbedData> rolesMsgEmbeds;
			unsigned int currentPage2 = 0;
			bool firstLoop2 = true;

			for (auto& value:discordGuild.data.guildShop.roles) {
				if (firstLoop2) {
					rolesMsgStrings.resize(1);
					rolesMsgEmbeds.resize(1);
					firstLoop2 = false;
				}
				string rolesMsgStringTemp;
				rolesMsgStringTemp = "**| __Role:__** <@&" + value.roleId + "> **| __Cost:__** " + to_string(value.roleCost) + "\n";
					if (rolesMsgStringTemp.length() + rolesMsgStrings[currentPage2].length() > 2048) {
						currentPage2 += 1;
						rolesMsgStrings.resize(currentPage2);
						rolesMsgEmbeds.resize(currentPage2);
					}
					rolesMsgEmbeds[currentPage2] = *new EmbedData;
					rolesMsgStrings[currentPage2] += rolesMsgStringTemp;
			}

			for (auto x = 0; x < rolesMsgEmbeds.size(); x += 1) {
				rolesMsgEmbeds[x].setTimeStamp(getTimeAndDate());
				rolesMsgEmbeds[x].setTitle("__**Shop Inventory (Roles) Page " + to_string(x + 1) + " of " + to_string(itemsMessageEmbeds.size() + rolesMsgEmbeds.size()) + ":**__");
				rolesMsgEmbeds[x].setDescription(rolesMsgStrings[x]);
				rolesMsgEmbeds[x].setColor(discordGuild.data.borderColor);
				rolesMsgEmbeds[x].setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			}

			for (auto x = 0; x < itemsMessageEmbeds.size(); x += 1) {
				itemsMessageEmbeds[x].setTimeStamp(getTimeAndDate());
				itemsMessageEmbeds[x].setTitle("__**Shop Inventory (Items) Page " + to_string(rolesMsgEmbeds.size() + x + 1) + " of " + to_string(itemsMessageEmbeds.size() + rolesMsgEmbeds.size()) + ":**__");
				itemsMessageEmbeds[x].setDescription(itemsMsgStrings[x]);
				itemsMessageEmbeds[x].setColor(discordGuild.data.borderColor);
				itemsMessageEmbeds[x].setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			}

			vector<EmbedData> finalMsgEmbedsArray;

			for (auto x = 0; x < rolesMsgEmbeds.size(); x += 1) {
				finalMsgEmbedsArray.push_back(rolesMsgEmbeds[x]);
			}

			for (auto x = 0; x < itemsMessageEmbeds.size(); x += 1) {
				finalMsgEmbedsArray.push_back(itemsMessageEmbeds[x]);
			}

			if (rolesMsgEmbeds.size()==  0 && itemsMessageEmbeds.size()== 0) {
				string msgString = "Sorry, but we are all out of inventory!";
				EmbedData messageEmbed;
				messageEmbed.setDescription(msgString);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setTitle("__**Empty Inventory:**__");
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.embeds.push_back(messageEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(responseData);
					InputEventManager::deleteInputEventResponse(event01, 20000);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(messageEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(event01, 20000);
				}
				co_return;
			}

			unsigned int currentPageIndex = 0;
			InputEventData newEvent;
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData responseData(args->eventData);
				responseData.embeds.push_back(finalMsgEmbedsArray[currentPageIndex]);
				newEvent = InputEventManager::respondToEvent(responseData);
			}
			else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateDeferredInteractionResponseData responseData01(event02);
				newEvent = InputEventManager::respondToEvent(responseData01);
				CreateFollowUpMessageData responseData(newEvent);
				responseData.embeds.push_back(finalMsgEmbedsArray[currentPageIndex]);
				newEvent = InputEventManager::respondToEvent(responseData);
			}

			discordGuild.writeDataToDB();
			string userID = args->eventData.getAuthorId();
			recurseThroughMessagePages(userID, newEvent, currentPageIndex, finalMsgEmbedsArray, true, 120000, false);

			co_return;
		}
	};
	Shop shop{};
}
#endif