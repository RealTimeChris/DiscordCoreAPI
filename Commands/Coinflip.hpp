// Coinflip.hpp - Header for the "coinflip" game/command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COIN_FLIP_
#define _COIN_FLIP_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Coinflip :public BaseFunction {
	public:
		Coinflip() {
			this->commandName = "coinflip";
			this->helpDescription = "";
		}
		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
		try {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);
				
			if (areWeInADm == true) {
				co_return;
			}

			if (args->eventData.eventType != DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
				InputEventManager::deleteInputEventResponse(args->eventData).get();
			}

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (areWeAllowed == false) {
				co_return;
			}

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getRequesterId() }).get();
			GuildMember botMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.discordCoreClient->currentUser->data.id }).get();

			if (!DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, DiscordCoreAPI::Permissions::MANAGE_MESSAGES)) {
				string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(),args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Permissions Issue:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(replyData).get();
				}
				else {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(responseData).get();
				}
				co_return;
			}

			regex betAmountRegExp("\\d{1,18}");

			if (args->argumentsArray.size() == 0 || !std::regex_search(args->argumentsArray.at(0), betAmountRegExp) || stoll(args->argumentsArray.at(0)) < 1) {
				string msgString = "------\n**Please enter a valid amount to bet! 1 " + args->eventData.discordCoreClient->discordUser->data.currencyName + " or more! (!coinflip = BETAMOUNT)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing or Invalid Arguments:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(replyData).get();
				}
				else {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(responseData).get();
				}
				co_return;
			}

			DiscordGuildMember discordGuildMember(guildMember.data);

			cmatch matchResults;
			std::regex_search(args->argumentsArray.at(0).c_str(), matchResults, betAmountRegExp);
			int betAmount = (int)stoll(matchResults.str());
			int currencyAmount = discordGuildMember.data.currency.wallet;

			if (betAmount > currencyAmount) {
				string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to place that wager!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Insufficient Funds:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(replyData).get();
				}
				else {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(responseData).get();
				}
				co_return;
			}

			string newBetString = "Welcome, <@!" + guildMember.data.user.id + "> , you have placed a bet of **" + to_string(betAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "**.\n";
			newBetString += "React with :exploding_head: to choose heads, or with :snake: to choose tails!";

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor("0000FF");
			msgEmbed.setDescription(newBetString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Heads, or Tails!?**__");
			InputEventData inputData;
			if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData replyData(args->eventData);
				replyData.addButton(false, "Heads", "Heads", "🤯", ButtonStyle::Success);
				replyData.addButton(false, "Tails", "Tails", "🐍", ButtonStyle::Success);
				replyData.embeds.push_back(msgEmbed);
				inputData = InputEventManager::respondToEvent(replyData).get();
			}
			else {
				CreateInteractionResponseData responseData(args->eventData);
				responseData.addButton(false, "Heads", "Heads", "🤯", ButtonStyle::Success);
				responseData.addButton(false, "Tails", "Tails", "🐍", ButtonStyle::Success);
				responseData.data.embeds.push_back(msgEmbed);
				inputData = InputEventManager::respondToEvent(responseData).get();
				EditInteractionResponseData responseData2(inputData);
				responseData2.embeds.push_back(msgEmbed);
				responseData2.addButton(false, "Heads", "Heads", "🤯", ButtonStyle::Success);
				responseData2.addButton(false, "Tails", "Tails", "🐍", ButtonStyle::Success);
				inputData = InputEventManager::respondToEvent(responseData2).get();
			}
			Button button2(inputData);
			ButtonInteractionData buttonInteractionData = button2.getOurButtonData(false, 60000);
			if (buttonInteractionData.customId == ""){
				string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
				EmbedData msgEmbed2;
				msgEmbed2.setColor("00FF00");
				msgEmbed2.setTimeStamp(getTimeAndDate());
				msgEmbed2.setTitle("__**Heads, or Tails?:**__");
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed2.setDescription(timeOutString);
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed2);
					InputEventManager::respondToEvent(replyData).get();
				}
				else {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed2);
					InputEventManager::respondToEvent(responseData).get();
				}
				co_return;
			}
				
			std::srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			float number = ((float)rand() / (float)RAND_MAX);
			unsigned int newBalance = 0;

			discordGuildMember.getDataFromDB();
			currencyAmount = discordGuildMember.data.currency.wallet;

			if (betAmount > currencyAmount) {
				string completionString = "------\nSorry, but you have insufficient funds in your wallet to place that wager.\n------";
				EmbedData msgEmbed3;
				msgEmbed3.setColor("FF0000");
				msgEmbed3.setDescription(completionString);
				msgEmbed3.setTimeStamp(getTimeAndDate());
				msgEmbed3.setTitle("__**Heads, or Tails**__");
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyData(args->eventData);
					replyData.embeds.push_back(msgEmbed3);
					InputEventManager::respondToEvent(replyData).get();
				}
				else {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed3);
					InputEventManager::respondToEvent(responseData).get();
				}
				co_return;
			}

			EmbedData msgEmbed4;
			if (button2.getButtonId() == "Heads" && number > 0.50 || button2.getButtonId() == "Tails" && number < 0.50) {
				discordGuildMember.data.currency.wallet += betAmount;
				discordGuildMember.writeDataToDB();
				discordGuild.data.casinoStats.totalCoinFlipPayout += betAmount;
				discordGuild.data.casinoStats.totalPayout += betAmount;
				if (betAmount > discordGuild.data.casinoStats.largestCoinFlipPayout.amount) {
					discordGuild.data.casinoStats.largestCoinFlipPayout.amount = betAmount;
					discordGuild.data.casinoStats.largestCoinFlipPayout.timeStamp = getTimeAndDate();
					discordGuild.data.casinoStats.largestCoinFlipPayout.userId = args->eventData.getAuthorId();
					discordGuild.data.casinoStats.largestCoinFlipPayout.userName = args->eventData.getUserName();
					discordGuild.writeDataToDB();
				}
				newBalance = discordGuildMember.data.currency.wallet;
				string completionString = "------\nNICELY DONE FAGGOT! YOU WON!\nYour new wallet balance is: " + to_string(newBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + ".\n------";
				msgEmbed4.setColor("00FF00");
				msgEmbed4.setDescription(completionString);
				msgEmbed4.setTimeStamp(getTimeAndDate());
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed4.setTitle("__**Heads, or Tails?**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					EditMessageData editData(inputData);
					editData.embeds.push_back(msgEmbed4);
					InputEventManager::respondToEvent(editData).get();
				}
				else if(args->eventData.eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION){
					EditInteractionResponseData responseData(inputData);
					responseData.embeds.push_back(msgEmbed4);
					InputEventManager::respondToEvent(responseData).get();
				}
			}
			else if (button2.getButtonId() == "Heads" && number <= 0.50 || button2.getButtonId() == "Tails" && number >= 0.50) {
				discordGuildMember.data.currency.wallet -= betAmount;
				discordGuildMember.writeDataToDB();
				discordGuild.data.casinoStats.totalCoinFlipPayout -= betAmount;
				discordGuild.data.casinoStats.totalPayout -= betAmount;
				discordGuild.writeDataToDB();
				newBalance = discordGuildMember.data.currency.wallet;
				string completionString = "------\nOWNED FUCK FACE! YOU LOST!\nYour new wallet balance is: " + to_string(newBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + ".\n------";
				msgEmbed4.setColor("FF0000");
				msgEmbed4.setDescription(completionString);
				msgEmbed4.setTimeStamp(getTimeAndDate());
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed4.setTitle("__**Heads, or Tails?**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					EditMessageData editData(inputData);
					editData.embeds.push_back(msgEmbed4);
					InputEventManager::respondToEvent(editData).get();
				}
				else if (args->eventData.eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
					EditInteractionResponseData responseData(inputData);
					responseData.embeds.push_back(msgEmbed4);
					InputEventManager::respondToEvent(responseData).get();
				}
			}
			co_return;
		}
		catch (exception& e) {
			cout << "Coinflip::execute() Error: " << e.what() << endl;
		}
	}

	};
	Coinflip coinFlip{};
};
#endif
