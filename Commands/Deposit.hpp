// Deposit.hpp - Header for the "Deposit" command.
// Jun 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DEPOSIT_
#define _DEPOSIT_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Deposit : public BaseFunction {
	public:
		Deposit() {
			this->commandName = "deposit";
			this->helpDescription = "__**Deposit Usage:**__ !deposit = DEPOSITAMOUNT, or /deposit DEPOSITAMOUNT";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

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

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
			DiscordGuildMember discordGuildMember(guildMember.data);

			regex depositAmountRegExp("\\d{1,18}");
			cmatch matchResults;
			unsigned int depositAmount = 0;
			if (args->argumentsArray.size() == 0 || args->argumentsArray[0] == "all") {
				depositAmount = discordGuildMember.data.currency.wallet;
			}
			else if (args->argumentsArray.size() == 0 ||args->argumentsArray[0] == "" || !regex_search(args->argumentsArray[0].c_str(),matchResults, depositAmountRegExp) || stoll(matchResults.str()) <= 0) {
				string msgString = "------\n**Please enter a valid deposit amount!(!deposit = AMOUNT)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(replyData);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(responseData);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				co_return;
			}
			else if (regex_search(args->argumentsArray[0], depositAmountRegExp)) {
				depositAmount = (unsigned int)stoll(matchResults.str());
			}

			if (depositAmount > discordGuildMember.data.currency.wallet) {
				string msgString = "------\n**Sorry, but you do not have sufficient funds to deposit that much!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(replyData);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					auto newEvent = InputEventManager::respondToEvent(responseData);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				co_return;
			}

			unsigned int msPerSecond = 1000;
			unsigned int  SecondsPerMinute = 60;
			unsigned int msPerMinute = msPerSecond * SecondsPerMinute;
			unsigned int MinutesPerHour = 60;
			unsigned int msPerHour = msPerMinute * MinutesPerHour;
			unsigned int msPerDepositCycle = msPerHour * args->eventData.discordCoreClient->discordUser->data.hoursOfDepositCooldown;
			unsigned int currentTime = static_cast<unsigned int>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
			
			unsigned int timeSinceLastDeposit = currentTime - discordGuildMember.data.currency.timeOfLastDeposit;

			string msgString = "";
			if (timeSinceLastDeposit >= msPerDepositCycle) {
				discordGuildMember.data.currency.bank += depositAmount;
				discordGuildMember.data.currency.wallet -= depositAmount;
				discordGuildMember.data.currency.timeOfLastDeposit = currentTime;
				discordGuildMember.writeDataToDB();

				msgString = "Congratulations! You've deposited " + to_string(depositAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + " from your wallet into your bank!\n------\n__**Your new balances are:**__\n__Bank:__ " + to_string(discordGuildMember.data.currency.bank) + " ";
				msgString += args->eventData.discordCoreClient->discordUser->data.currencyName + "\n" + "__Wallet:__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";
			}
			else {
				unsigned int timeRemaining = msPerDepositCycle - timeSinceLastDeposit;
				unsigned int hoursRemain = (unsigned int)trunc(timeRemaining / msPerHour);
				unsigned int minutesRemain = (unsigned int)trunc((timeRemaining % msPerHour) / msPerMinute);
				unsigned int secondsRemain = (unsigned int)trunc(((timeRemaining % msPerHour) % msPerMinute) / msPerSecond);

				if (hoursRemain > 0) {
					msgString = "Sorry, but you need to wait " + to_string(hoursRemain) + " hours, " + to_string(minutesRemain) + " minutes, and " + to_string(secondsRemain) + " seconds before you can make another deposit!";
				}
				else if (minutesRemain > 0) {
					msgString = "Sorry, but you need to wait " + to_string(minutesRemain) + " minutes, and " + to_string(secondsRemain) + " seconds before you can make another deposit!";
				}
				else {
					msgString = "Sorry, but you need to wait " + to_string(secondsRemain) + " seconds before you can make another deposit!";
				}
			}

			EmbedData messageEmbed;
			messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			messageEmbed.setColor(discordGuild.data.borderColor);
			messageEmbed.setTitle("__**Bank Deposit:**__");
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setDescription(msgString);
			if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData replyData(args->eventData);
				replyData.embeds.push_back(messageEmbed);
				auto newEvent = InputEventManager::respondToEvent(replyData);;
			}
			else {
				CreateEphemeralInteractionResponseData responseData(args->eventData);
				responseData.data.embeds.push_back(messageEmbed);
				auto newEvent = InputEventManager::respondToEvent(responseData);
			}
			co_return;
		}
	};
	Deposit deposit;
}

#endif
