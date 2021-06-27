// Buy.hpp - Header for the "Buy" command.
// June 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BUY_
#define _BUY_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Buy : public BaseFunction {
	public:
		Buy() {
			this->commandName = "buy";
			this->helpDescription = "__**Buy**__ !buy = ITEMNAME/ROLENAME";
		}
		virtual task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

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

				User currentUser = args->eventData.discordCoreClient->users->getUserAsync({ args->eventData.getRequesterId() }).get();

				if (args->argumentsArray.size() == 0) {
					string msgString = "------\n**Please enter an item name!(!buy = ITEMNAME)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					InputEventData event01;
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateEphemeralInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					co_return;
				}

				string objectName = args->argumentsArray.at(0);
				unsigned int objectShopIndex = 0;
				string objectType;
				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->fetchAsync({ .guildId = args->eventData.getGuildId(),.guildMemberId = currentUser.data.id }).get();
				DiscordGuildMember discordGuildMember(guildMember.data);
				vector<Role> rolesArray = args->eventData.discordCoreClient->roles->getGuildMemberRoles({ .guildId = args->eventData.getGuildId(),.guildMember = guildMember });

				for (unsigned int x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					bool isRoleFound = false;
					InventoryRole shopRole = discordGuildMember.data.roles.at(x);
					for (unsigned int y = 0; y < rolesArray.size(); y += 1) {
						if (rolesArray.at(y).data.id == shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						for (unsigned int z = 0; z < discordGuildMember.data.roles.size(); z += 1) {
							if (shopRole.roleId == discordGuildMember.data.roles.at(z).roleId) {
								discordGuildMember.data.roles.erase(discordGuildMember.data.roles.begin() + z);
							}
						}
						string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild member cache!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Removed Guild Member Role:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateEphemeralInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						x -= 1;
					}
					discordGuildMember.writeDataToDB();
				}

				bool isFoundInShop = false;
				for (unsigned int x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
					if (objectName == discordGuild.data.guildShop.items.at(x).itemName) {
						isFoundInShop = true;
						objectShopIndex = x;
						objectType = "item";
						break;
					}
				}
				for (unsigned int x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
					if (objectName == discordGuild.data.guildShop.roles.at(x).roleName) {
						isFoundInShop = true;
						objectShopIndex = x;
						objectType = "role";
						break;
					}
				}
				if (isFoundInShop == false) {
					string msgString = "------\n**Sorry, but we could not find that object in the shop!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Object:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateEphemeralInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					co_return;
				}

				bool isFoundInInventory = false;

				for (unsigned int x = 0; x < discordGuildMember.data.items.size(); x += 1) {
					if (objectName == discordGuildMember.data.items.at(x).itemName) {
						isFoundInInventory = true;
						break;
					}
				}

				for (unsigned int x = 0; x < discordGuildMember.data.roles .size(); x += 1) {
					if (objectName == discordGuildMember.data.roles.at(x).roleName) {
						isFoundInInventory = true;
						break;
					}
				}

				if (isFoundInInventory == true) {
					string msgString = "------\n**Sorry, but you already have one of those " + objectType + "s.** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Duplicate Object:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateEphemeralInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
						InputEventManager::deleteInputEventResponse(event01, 20000);
					}
					co_return;
				}

				if (objectType == "role") {
					unsigned int roleCost = discordGuild.data.guildShop.roles.at(objectShopIndex).roleCost;
					unsigned int userBalance = discordGuildMember.data.currency.wallet;

					if (roleCost > userBalance) {
						string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Insufficient Funds:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateEphemeralInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						co_return;
					}

					InventoryRole newRole = discordGuild.data.guildShop.roles.at(objectShopIndex);
					discordGuildMember.data.roles.push_back(newRole);
					discordGuildMember.data.currency.wallet -= roleCost;
					discordGuildMember.writeDataToDB();

					unsigned int newBalance = discordGuildMember.data.currency.wallet;

					string roleID = discordGuild.data.guildShop.roles.at(objectShopIndex).roleId;

					args->eventData.discordCoreClient->roles->addRoleToGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .userId = currentUser.data.id, .roleId = roleID });

					string msgString = "------\nCongratulations! You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ <@&" + newRole.roleId + "> (" + newRole.roleName + ")\n------\n__**Your new wallet balance:**__ " + to_string(newBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";
					EmbedData msgEmbed;
					msgEmbed.setTitle("__**New Role Purchased:**__");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
					}
					
					unsigned int maxIdx = 0;
					InventoryRole tempItem;
					unsigned int len = (unsigned int)discordGuildMember.data.roles.size();
					for (unsigned int x = 0; x < len; x += 1) {
						maxIdx = x;
						for (unsigned int y = x + 1; y < len; y += 1) {
							if (discordGuildMember.data.roles.at(y).roleCost
						> discordGuildMember.data.roles.at(maxIdx).roleCost) {
								maxIdx = y;
							}
						}
						tempItem = discordGuildMember.data.roles.at(x);
						discordGuildMember.data.roles.at(x) = discordGuildMember.data.roles.at(maxIdx);
						discordGuildMember.data.roles.at(maxIdx) = tempItem;
					}
					discordGuildMember.writeDataToDB();
					co_return;
				}
				else if (objectType ==  "item") {
					unsigned int  itemCost = discordGuild.data.guildShop.items.at(objectShopIndex).itemCost;
					unsigned int userBalance = discordGuildMember.data.currency.wallet;

					if (itemCost > userBalance) {
						string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setDescription(msgString);
						msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setTitle("__**Insufficient Funds:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateEphemeralInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData);
							InputEventManager::deleteInputEventResponse(event01, 20000);
						}
						co_return;
					}

					InventoryItem newItem = discordGuild.data.guildShop.items.at(objectShopIndex);
					discordGuildMember.data.items.push_back(newItem);
					discordGuildMember.data.currency.wallet -= itemCost;
					discordGuildMember.writeDataToDB();

					string itemEmoji = discordGuild.data.guildShop.items.at(objectShopIndex).emoji;
					string itemName = discordGuild.data.guildShop.items.at(objectShopIndex).itemName;
					unsigned int newBalance = discordGuildMember.data.currency.wallet;
					string msgString = "------\nCongratulations!You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ " + itemEmoji + itemName + "\n------\n__**Your new wallet balance:**__ " + to_string(newBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";
					EmbedData msgEmbed;
					msgEmbed.setTitle("__**New Item Purchased:**__");

					unsigned int maxIdx = 0;
					InventoryItem tempItem;
					unsigned int len = (unsigned int)discordGuildMember.data.items.size();
					for (unsigned int x = 0; x < len; x += 1) {
						maxIdx = x;
						for (unsigned int y = x + 1; y < len; y += 1) {
							if (discordGuildMember.data.items.at(y).itemCost
						> discordGuildMember.data.items.at(maxIdx).itemCost) {
								maxIdx = y;
							}
						}
						tempItem = discordGuildMember.data.items.at(x);
						discordGuildMember.data.items.at(x) = discordGuildMember.data.items.at(maxIdx);
						discordGuildMember.data.items.at(maxIdx) = tempItem;
					}
					discordGuildMember.writeDataToDB();
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
					}
					co_return;
				}
				co_return;
			}
			catch (exception& e) {
				cout << "Buy::execute() Error: " << e.what() << endl << endl;
			}

			
			}
		};
		Buy buy{};
	};
	

#endif
