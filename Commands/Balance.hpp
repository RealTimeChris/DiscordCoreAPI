// Balance.hpp - Header for the "balance" command.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BALANCE_
#define _BALANCE_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Balance : public BaseFunction {
	public:
		Balance() {
			this->commandName = "balance";
			this->helpDescription = "__**Balance Usage:**__ Enter !balance or /balance to view your own balance, or !balance = @USERMENTION /balance @USERMENTION to view someone else's balances.";
		}
		virtual task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(args->eventData, channel);

				if (areWeInADm == true) {
					co_return;
				}

				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					InputEventManager::deleteInputEventResponse(args->eventData).get();
				}

				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild = args->eventData.discordCoreClient->getDiscordGuild(guild.data);

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

				if (areWeAllowed == false) {
					co_return;
				}

				string userID = "";
				unsigned int bankAmount = 0;
				unsigned int walletAmount = 0;

				regex mentionRegExp("<@!\\d{18,}>");
				regex idRegExp("\\d{18,}");
				if (args->argumentsArray.size() == 0) {
					userID = args->eventData.getRequesterId();
				}
				else if (args->argumentsArray.at(0) != "") {
					if (!regex_search(args->argumentsArray.at(0), mentionRegExp, regex_constants::match_flag_type::format_first_only) && !regex_search(args->argumentsArray.at(0), idRegExp, regex_constants::match_flag_type::format_first_only)) {
						string msgString = "------\n* *Please, enter a valid user mention, or enter none at all!(!balance = @USERMENTION)**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(msgEmbed);
							InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
							InputEventManager::deleteInputEventResponse(event01, 20000).get();
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(msgEmbed);
							InputEventData event = InputEventManager::respondToEvent(responseData).get();
							InputEventManager::deleteInputEventResponse(event, 20000).get();
						}
						co_return;
					}
					cmatch matchResults;
					regex_search(args->argumentsArray.at(0).c_str(), matchResults, idRegExp);
					userID = matchResults.str();
				}

				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = userID }).get();

				if (guildMember.data.user.id == "") {
					string msgString = "------\n**Sorry, but that user could not be found!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(msgEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
						InputEventManager::deleteInputEventResponse(event01, 20000).get();
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(msgEmbed);
						InputEventData event = InputEventManager::respondToEvent(responseData).get();
						InputEventManager::deleteInputEventResponse(event, 20000).get();
					}
					co_return;
				}

				DiscordGuildMember discordGuildMember(guildMember.data);

				string msgString = "";
				bankAmount = discordGuildMember.data.currency.bank;
				walletAmount = discordGuildMember.data.currency.wallet;
				msgString = "<@!" + guildMember.data.user.id + "> 's balances are:\n------\n__**Bank Balance:**__ " + to_string(bankAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName +
					"\n__**Wallet Balance:**__ " + to_string(walletAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setDescription(msgString);
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Current Balances:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.embeds.push_back(msgEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(msgEmbed);
					InputEventData event = InputEventManager::respondToEvent(responseData).get();
				}
			}
			catch (exception& e) {
				cout << "Balance::execute() Error: " << e.what() << endl;
			}
		}
	};
	Balance balance{};

}
#endif