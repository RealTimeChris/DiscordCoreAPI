// SetBalance.hpp - Header for the "set balance" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_BALANCE_
#define _SET_BALANCE_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class SetBalance:public BaseFunction{
	public:
		SetBalance() {
			this->commandName = "setbalance";
			this->helpDescription = "__**Set Balance Usage:**__ !setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION or to set your own balance it's simply !setbalance = NEWBALANCE, BALANCETYPE";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();
			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm == true ) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
			
			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if(!areWeAllowed){
				co_return;
			}

			bool areTheyACommander = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (!areTheyACommander) {
				co_return;
			}

			regex balanceRegExp("\\d{1,18}");
			regex userMentionRegExp("<@!\\d{18}>");
			regex userIDRegExp("\\d{18}");
			string targetUserID;

			if (args->argumentsArray.size() == 0|| !regex_search(args->argumentsArray.at(0),balanceRegExp) || stoll(args->argumentsArray.at(0)) < 0) {
				string msgString = "------\n**Please enter a valid desired balance! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
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
				}
				co_return;
			}
			if (args->argumentsArray.size() < 2|| (args->argumentsArray.at(1) !=  "bank" && args->argumentsArray.at(1) !=  "wallet")) {
				string msgString = "------\n**Please enter a valid balance type! Bank or Wallet! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
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
				}
				co_return;
			}
			if (args->argumentsArray.size() < 3) {
				targetUserID = args->eventData.getRequesterId();
			}
			else if (args->argumentsArray.size() == 3 && !regex_search(args->argumentsArray.at(2), userMentionRegExp) && !regex_search(args->argumentsArray.at(2), userIDRegExp)) {
				string msgString = "------\n**Please enter a valid target user mention, or leave it blank to select yourself as the target! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
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
				}
				co_return;
			}
			else if (args->argumentsArray.at(2) != "") {
				cmatch matchResults;
				regex_search(args->argumentsArray.at(2).c_str(), matchResults, userIDRegExp);
				string targetUserIDOne = matchResults.str();
				targetUserID = targetUserIDOne;
			}

			unsigned int targetUserBalance = (unsigned int)stoll(args->argumentsArray.at(0));
			string balanceType = args->argumentsArray.at(1);

			GuildMember targetMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), targetUserID }).get();

			if (targetMember.data.user.username == "") {
				string msgString = "------\n**Sorry, but the specified user could not be found!**\n------";
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
				}
				co_return;
			}

			DiscordGuildMember discordGuildMember(targetMember.data);

			string msgString;

			if (balanceType == "bank") {
				discordGuildMember.data.currency.bank = targetUserBalance;
				discordGuildMember.writeDataToDB();

				unsigned int newBalance = discordGuildMember.data.currency.bank;

				msgString = "__You've set the user <@!" + targetUserID + "> 's bank balance to:__ " + to_string(newBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			}
			else if (balanceType == "wallet") {
				discordGuildMember.data.currency.wallet = targetUserBalance;
				discordGuildMember.writeDataToDB();

				unsigned int newBalance = discordGuildMember.data.currency.wallet;

				msgString = "__You've set the user <@!" + targetUserID + ">'s wallet balance to:__ " + to_string(newBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			}

			EmbedData messageEmbed;
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			messageEmbed.setColor(discordGuild.data.borderColor);
			messageEmbed.setDescription(msgString);
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setTitle("__**Set New Balance:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.content = "<@!" + targetUserID + ">";
				dataPackage.embeds.push_back(messageEmbed);
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.content = "<@!" + targetUserID + ">";
				dataPackage.data.embeds.push_back(messageEmbed);
				auto newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			co_return;
		}
	};

	SetBalance setBalance;
}
#endif