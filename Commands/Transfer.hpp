// Transfer.hpp - Header for the "transfer" command.
// Jun 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TRANSFER_
#define _TRANSFER_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class Transfer :public BaseFunction {
	public:
		Transfer() {
			this->commandName = "transfer";
			this->helpDescription = "__**Transfer Usage:**__ !transfer = AMOUNT, @USERMENTION, or /transfer AMOUNT, USERMENTION";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();
			bool  areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm == true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (!areWeAllowed) {
				co_return;
			}

			regex userMentionRegExp("\\d{18}");
			regex amountRegExp("\\d{1,18}");
			if (args->argumentsArray.size() == 0 || args->argumentsArray.size() == 1) {
				string msgString = "------\n**Please enter the valid arguments: (!transfer = AMOUNT, @USERMENTION)**\n------";
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
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}
			if (!regex_search(args->argumentsArray[1], userMentionRegExp)) {
				string msgString = "------\n**Please enter a valid user mention! (!transfer = AMOUNT, @USERMENTION)**\n------";
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
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}
			if (!regex_search(args->argumentsArray[0], amountRegExp) || stoll(args->argumentsArray[0]) <= 0) {
				string msgString = "------\n**Please enter a valid number for amount! (!transfer = AMOUNT, @USERMENTION)**\n------";
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
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}

			cmatch matchResults;
			regex_search(args->argumentsArray[1].c_str(), matchResults, userMentionRegExp);
			string toUserID = matchResults.str();
			string fromUserID = args->eventData.getAuthorId();
			unsigned int amount = (unsigned int)stoll(args->argumentsArray[0]);
			GuildMember toUserMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), toUserID }).get();

			if (toUserID == fromUserID) {
				string msgString = "------\n**Sorry, but you cannot transfer to yourself!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Transfer Issue:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}

			if (toUserMember.data.user.username == "") {
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
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}

			DiscordGuildMember discordToGuildMember(toUserMember.data);

			if (discordToGuildMember.data.userName == "") {
				string msgString = "------\n * *Sorry, but that user data could not be found!**\n------";
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
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}

			GuildMember fromGuildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), fromUserID }).get();
			DiscordGuildMember discordFromGuildMember(fromGuildMember.data);

			if (amount > discordFromGuildMember.data.currency.wallet) {
				string msgString = "------\n**Sorry, but you don't have sufficient funds in your wallet for that transfer!**\n-------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Insufficient Funds:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}
			discordFromGuildMember.data.currency.wallet -= amount;
			discordFromGuildMember.writeDataToDB();
			discordToGuildMember.data.currency.wallet += amount;
			discordToGuildMember.writeDataToDB();

			string msgString;
			msgString += "<@!" + fromUserID + "> succesfully transferred " + to_string(amount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + " to <@!" + toUserID + ">.";
			msgString += "\n__Your new wallet balances are:__ \n<@!" + fromUserID + ">: " + to_string(discordFromGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			msgString += "\n<@!" + toUserID + ">: " + to_string(discordToGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Balance Transfer:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.embeds.push_back(msgEmbed);
				dataPackage.content = "<@!" + toUserID + ">";
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.embeds.push_back(msgEmbed);
				dataPackage.data.content = "<@!" + toUserID + ">";
				InputEventManager::respondToEvent(dataPackage);
			}
			co_return;
		}
	};
	Transfer transfer;
}
#endif
