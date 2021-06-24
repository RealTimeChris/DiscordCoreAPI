// AddShopItem.hpp - Header for the "add shop item" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ADD_SHOP_ITEM_
#define _ADD_SHOP_ITEM_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class AddShopItem :public BaseFunction {
	public:
		AddShopItem() {
			this->commandName = "addshopitem";
			this->helpDescription = "__**Add Shop Item Usage:**__ Enter !addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI, or /addshopitem ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI.";
		}
		virtual task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

				bool areWeInADm = areWeInADM(args->eventData, channel);

				if (areWeInADm == true) {
					co_return;
				}

				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					InputEventManager::deleteInputEventResponse(args->eventData);
				}

				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild.data);

				bool doWeHaveAdmin = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					co_return;
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

				if (areWeAllowed == false) {
					co_return;
				}

				regex itemNameRegExp("\.{1,32}");
				regex selfModRegExp("\\d{1,5}");
				regex oppModRegExp("-{0,1}\\d{1,5}");
				regex itemCostRegExp("\\d{1,6}");
				regex emojiRegExp("\.{1,32}");
				if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray.at(0), itemNameRegExp)) {
					string msgString = "------\n**Please enter a valid item name! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}
				if (args->argumentsArray.size() < 2 || !regex_search(args->argumentsArray.at(1), selfModRegExp) || stoll(args->argumentsArray.at(1)) > 100 || stoll(args->argumentsArray.at(1)) < 0) {
					string msgString = "------\n**Please enter a valid self - mod value, between 0 and 100! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}
				if (args->argumentsArray.size() < 3 || !regex_search(args->argumentsArray.at(2), oppModRegExp) || stoll(args->argumentsArray.at(2)) < -100 || stoll(args->argumentsArray.at(2)) > 0) {
					string msgString = "------\n**Please enter a valid opp - mod value between - 100 and 0! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}
				if (args->argumentsArray.size() < 4 || !regex_search(args->argumentsArray.at(3), itemCostRegExp) || stoll(args->argumentsArray.at(3)) < 1) {
					string msgString = "------\n**Please enter a valid item cost! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}
				if (args->argumentsArray.size() < 5 || !regex_search(args->argumentsArray.at(4), emojiRegExp)) {
					string msgString = "------\n**Please enter a valid emoji! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event, 20000);
					}
					co_return;
				}

				string itemName = args->argumentsArray.at(0);
				cmatch matchResults;
				regex_search(args->argumentsArray.at(1).c_str(), matchResults, selfModRegExp);
				unsigned int selfMod = (unsigned int)stoll(matchResults.str());
				regex_search(args->argumentsArray.at(2).c_str(), matchResults, oppModRegExp);
				int oppMod = (int)stoll(matchResults.str());
				regex_search(args->argumentsArray.at(3).c_str(), matchResults, itemCostRegExp);
				unsigned int itemCost = (unsigned int)stoll(matchResults.str());
				string emoji = args->argumentsArray.at(4);

				for (auto& value : discordGuild.data.guildShop.items) {
					if (itemName == value.itemName) {
						string msgString = "------\n**Sorry, but an item by that name already exists!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Item Issue:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(msgEmbed);
							InputEventData event = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event, 20000);
						}
						co_return;
					}
				}

				InventoryItem newItem;
				newItem.emoji = emoji;
				newItem.itemCost = itemCost;
				newItem.itemName = itemName;
				newItem.oppMod = oppMod;
				newItem.selfMod = selfMod;

				discordGuild.data.guildShop.items.push_back(newItem);
				discordGuild.writeDataToDB();

				string msgString = "";
				msgString = "Good job! You've added a new item to the shop, making it available for purchase by the members of this server!\n\
				The item's stats are as follows:\n__Item Name__: " + itemName + "\n__Self-Mod Value__: " + to_string(selfMod) + "\n__Opp-Mod Value__: " + to_string(oppMod) + "\n\
				__Item Cost__: " + to_string(itemCost) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__Emoji__: " + emoji;
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Shop Item Added:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.embeds.push_back(msgEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(responseData);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					InputEventData event = InputEventManager::respondToEvent(responseData);
				}
				co_return;
			}
			catch (exception& e) {
				cout << "AddShopItem::execute() Error: " << e.what() << endl << endl;
			}

		}

	};
	AddShopItem addShopItem{};

}
#endif