// RemoveShopItem.hpp - Header for the "remove shop item" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REMOVE_SHOP_ITEM_
#define _REMOVE_SHOP_ITEM_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class RemoveShopItem : public BaseFunction {
	public:
		RemoveShopItem() {
			this->commandName = "removeshopitem";
			this->helpDescription = "___**Remove Shop Item Usage:*__ !removeshopitem = ITEMNAME, or /removeshopitem ITEMNAME";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments>args){
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm ==  true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), args->eventData.getAuthorId() }).get();
			DiscordGuildMember discordGuildMember(guildMember.data);
			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				co_return;
			}

			if (args->argumentsArray.size() == 0) {
				string msgString = "------\n**Please enter an item name!(!removeshopitem = ITEMNAME)**\n------";
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

			string itemName = args->argumentsArray.at(0);

			unsigned int itemIndex = 0;
			bool itemFound = false;
			for (unsigned int x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
				if (itemName == discordGuild.data.guildShop.items[x].itemName) {
					itemIndex = x;
					itemFound = true;
					break;
				}
			}

			if (itemFound ==  false) {
				string msgString = "------\n**Sorry, but that item was not found in the shop's inventory!**\n------";
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

			msgString = "Alrighty then! You've removed an item from the shop!\n------\n__The removed item: __ " + discordGuild.data.guildShop.items[itemIndex].emoji + discordGuild.data.guildShop.items[itemIndex].itemName + "\n------"; 

			discordGuild.data.guildShop.items.erase(discordGuild.data.guildShop.items.begin() + itemIndex);
			discordGuild.writeDataToDB();

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Shop Item Removed:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.embeds.push_back(msgEmbed);
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.embeds.push_back(msgEmbed);
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			co_return;
		}
	};
	RemoveShopItem removeShopItem;
}

#endif