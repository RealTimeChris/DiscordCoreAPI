// Withdraw.hpp - Header for the "withdraw" command.
// Jun 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _WITHDRAW_
#define _WITHDRAW_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class Withdraw : public BaseFunction {
	public:
		Withdraw() {
			this->commandName = "withdraw";
			this->helpDescription = "__**Withdraw Usage:**__ !withdraw = AMOUNT, or /withdraw AMOUNT";

		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm ==  true) {
				co_return;
			}

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			InputEventManager::deleteInputEventResponse(args->eventData);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (!areWeAllowed) {
				co_return;
			}

			unsigned int withdrawAmount = 0;

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), args->eventData.getAuthorId() }).get();
			DiscordGuildMember discordGuildMember(guildMember.data);

			regex amountRegExp("\\d{1,18}");
			if (args->argumentsArray.size() == 0 || !regex_search(args->argumentsArray[0], amountRegExp)|| stoll(args->argumentsArray[0]) <= 0  ) {
				string  msgString = "------\n**Please enter a valid withdrawl amount! (!withdraw = AMOUNT)**\n------";
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
			else {
				cmatch matchResults;
				regex_search(args->argumentsArray[0].c_str(), matchResults, amountRegExp);
				withdrawAmount = (unsigned int)stoll(matchResults.str());
			}

			if (withdrawAmount > discordGuildMember.data.currency.bank) {
				string msgString = "-------\n**Sorry, but you do not have sufficient funds to withdraw that much!**\n------";
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
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
				}
				co_return;
			}

			discordGuildMember.data.currency.wallet += withdrawAmount;
			discordGuildMember.data.currency.bank -= withdrawAmount;
			discordGuildMember.writeDataToDB();

			string msgString = "Congratulations! You've withdrawn " + to_string(withdrawAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + " from your bank account to your wallet!\n------\n__**Your new balances are:**__\n" +
				"__Bank:__ " + to_string(discordGuildMember.data.currency.bank) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n" + "__Wallet:__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName 
				+ "\n------";

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Bank Withdrawal:**__");
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
	Withdraw withdraw;
}

#endif