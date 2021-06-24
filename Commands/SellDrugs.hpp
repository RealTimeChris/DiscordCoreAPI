// SellDrugs.hpp - Header for the "sell drugs" command.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SELL_DRUGS_
#define _SELL_DRUGS_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class SellDrugs :public BaseFunction {
	public:
		SellDrugs() {
			this->commandName = "selldrugs";
			this->helpDescription = "__**Sell Drugs:**__ Enter !selldrugs or /selldrugs to gain currency.";
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

				if (areWeAllowed ==  false) {
					co_return;
				}
				
				unsigned long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				unsigned int msPerWorkCycle = args->eventData.discordCoreClient->discordUser->data.hoursOfDrugSaleCooldown * 60 * 60 * 1000;
				unsigned int msPerSecond = 1000;
				unsigned int msPerMinute = 60 * msPerSecond;
				unsigned int msPerHour = 60 * msPerMinute;

				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getRequesterId() }).get();
				DiscordGuildMember discordGuildMember(guildMember.data);
				User currentUser = args->eventData.discordCoreClient->users->getUserAsync({ .userId = args->eventData.getRequesterId() }).get();
				unsigned int lastTimeWorked = discordGuildMember.data.lastTimeWorked;

				unsigned int timeDifference = (unsigned int)currentTime - lastTimeWorked;

				if (timeDifference >= msPerWorkCycle) {
					srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					unsigned int amountEarned = (unsigned int)trunc(((float)rand() / (float)RAND_MAX) * 5000.0f);

					discordGuildMember.data.currency.wallet += amountEarned;
					discordGuildMember.writeDataToDB();

					string msgString = "";
					msgString += "You've been busy dealing drugs... and you've earned " + to_string(amountEarned) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\nNice job and watch out for cops!\nYour new wallet balance is: ";
					msgString += to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
					EmbedData messageEmbed;
					messageEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Drug Dealing:**__");
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(messageEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(messageEmbed);
						InputEventData event01 = InputEventManager::respondToEvent(responseData);
					}

					discordGuildMember.data.lastTimeWorked = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					discordGuildMember.writeDataToDB();
				}
				else {
					unsigned int timeLeft = msPerWorkCycle - timeDifference;
					unsigned int hoursLeft = (unsigned int)trunc(timeLeft / msPerHour);
					unsigned int minutesLeft = (unsigned int)trunc((timeLeft % msPerHour) / msPerMinute);
					unsigned int secondsLeft = (unsigned int)trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

					string msgString = "";
				if (hoursLeft > 0) {
					msgString += "Sorry, but you need to wait " + to_string(hoursLeft) + " hours, " + to_string(minutesLeft) + " minutes, and " + to_string(secondsLeft) + " seconds before you can get paid again!";
				}
				else if (minutesLeft > 0) {
					msgString += "Sorry, but you need to wait " + to_string(minutesLeft) + " minutes, and " + to_string(secondsLeft) + " seconds before you can get paid again!";
				}
				else {
					msgString += "Sorry, but you need to wait " + to_string(secondsLeft) + " seconds before you can get paid again!";
				}
				EmbedData  messageEmbed;
				messageEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Drug Dealing:**__");
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.embeds.push_back(messageEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(responseData);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION){
					CreateEphemeralInteractionResponseData responseData(args->eventData);
					responseData.data.embeds.push_back(messageEmbed);
					InputEventData event01 = InputEventManager::respondToEvent(responseData);
				}
				}
			}
			catch (exception& e) {
				cout << "SellDrugs::execute() Error: " << e.what() << endl << endl;
			}
		}
	};
	SellDrugs sellDrugs{};
}
#endif
