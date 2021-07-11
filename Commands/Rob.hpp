// Rob.hpp - Header for the "rob" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROB_
#define _ROB_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class Rob: public BaseFunction {
	public:
		Rob() {
			this->commandName = "rob";
			this->helpDescription = "__**Rob Usage:**__ !rob = TARGETMENTION, or /rob TARGETMENTION";
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

			if (areWeAllowed == false) {
				co_return;
			}

			regex userMentionRegExp(".{2,3}\\d{18}>");
			regex userIDRegExp("\\d{18}");
			if (args->argumentsArray.size() == 0 || (!regex_search(args->argumentsArray.at(0), userMentionRegExp) && !regex_search(args->argumentsArray.at(0), userIDRegExp))) {
				string msgString = "------\n**Please enter a valid user mention for the target!(!rob = @USERMENTION)**\n------";
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

			string userID = args->eventData.getAuthorId();

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
			DiscordGuildMember discordGuildMember(guildMember.data);

			cmatch matchResults;
			regex_search(args->argumentsArray.at(0).c_str(), matchResults, userIDRegExp);
			string targetUserID = matchResults.str();
			GuildMember targetMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = targetUserID }).get();
			DiscordGuildMember targetGuildMember(targetMember.data);

			if (targetMember.data.user.username == "") {
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
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				co_return;
			}

			if (userID == targetUserID) {
				string msgString = "------\n**You can't rob yourself, dumbass!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Robbery Issue:**__");
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

			unsigned int msPerSecond = 1000;
			unsigned int secondsPerMinute = 60;
			unsigned int msPerMinute = msPerSecond * secondsPerMinute;
			unsigned int minutesPerHour = 60;
			unsigned int msPerHour = msPerMinute * minutesPerHour;
			unsigned int timeBetweenRobberies = (unsigned int)(args->eventData.discordCoreClient->discordUser->data.hoursOfRobberyCooldown * (float)msPerHour);
			unsigned int currentTime =  (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			unsigned int currentTimeDifference = currentTime - discordGuildMember.data.lastTimeRobbed;

			if (currentTimeDifference >= timeBetweenRobberies) {
				string userGainString;
				string userLossString;

				unsigned int userLossAmount = 0;
				unsigned int userGainAmount = 0;

				for (unsigned int x = 0; x < discordGuildMember.data.items.size(); x += 1) {
					if (discordGuildMember.data.items[x].selfMod > 0) {
						userGainAmount += discordGuildMember.data.items[x].selfMod;
						userGainString += "+" + to_string(discordGuildMember.data.items[x].selfMod) + " of base roll from <@!" + discordGuildMember.data.guildMemberId + "> 's " + discordGuildMember.data.items[x].emoji + discordGuildMember.data.items[x].itemName + "\n";
					}
				}

				for (unsigned int x = 0; x < targetGuildMember.data.items.size(); x += 1) {
					if (targetGuildMember.data.items[x].oppMod < 0) {
						userLossAmount += targetGuildMember.data.items[x].oppMod;
						userLossString += "-" + to_string(targetGuildMember.data.items[x].oppMod) + " of base roll from <@!" + targetGuildMember.data.guildMemberId + "> 's " + targetGuildMember.data.items[x].emoji + targetGuildMember.data.items[x].itemName + "\n";
					}
				}

				unsigned int userRollMod = userGainAmount + userLossAmount;

				unsigned int userRollModTotal = (unsigned int)applyAsymptoticTransform((float)userRollMod, 2000.0f, 40.0f);

				unsigned int baseProbabilityOfSuccess = 40;
				unsigned int totalProbabilityOfSuccess = baseProbabilityOfSuccess + userRollModTotal;

				srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

				unsigned int currentProbabilityValue = (unsigned int)trunc((float)(rand() / (float)RAND_MAX) * 100.0f);
				bool currentSuccessValue = currentProbabilityValue > (100 - totalProbabilityOfSuccess);

				string msgString;

				if (currentSuccessValue == true) {
					msgString = "Nicely done! You robbed that motherfucker, <@!" + targetUserID + ">, good!\n\n__Base probability of success: __ " + to_string(baseProbabilityOfSuccess) + "% \n";

					if (userGainAmount > 0 || userLossAmount < 0) {
						msgString += userGainString + userLossString + "\n__Resulting in a net probability of success gain of:__ " + to_string(userRollModTotal) + "% \n__For a final probability of success of:__ " + to_string(totalProbabilityOfSuccess) + "% \n";
					}

					unsigned int currencyRobPercentage = (unsigned int)trunc((float)rand() / (float)RAND_MAX * 60.0f);

					unsigned int currencyRobAmount = (unsigned int)trunc((float)targetGuildMember.data.currency.wallet * ((float)currencyRobPercentage / 100.0f));

					if (currencyRobAmount < 0) {
						string msgStringNew = "------\n**Cannot rob for debt!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgStringNew);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Target Issue:**__");
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

					targetGuildMember.data.currency.wallet -= currencyRobAmount;
					targetGuildMember.writeDataToDB();
					discordGuildMember.data.currency.wallet += currencyRobAmount;
					discordGuildMember.writeDataToDB();
					unsigned int targetUserNewBalance = targetGuildMember.data.currency.wallet;
					unsigned int userNewBalance = discordGuildMember.data.currency.wallet;

					msgString = msgString + "------\n**You've robbed <@!" + targetUserID + "> for " + to_string(currencyRobPercentage) + "% of their wallet, which is " + to_string(currencyRobAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + ".\n"
						+ "**\n__Your new wallet balances are:__\n<@!" + userID + ">: " + to_string(userNewBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n<@!" + targetUserID + ">: " + to_string(targetUserNewBalance) + " " +
						args->eventData.discordCoreClient->discordUser->data.currencyName;

					discordGuildMember.data.lastTimeRobbed = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					discordGuildMember.writeDataToDB();
					EmbedData messageEmbed;
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setDescription(msgString);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setTitle("__**Succesful Robbery:**__");
					messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
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
				}
				else if (currentSuccessValue == false) {
					unsigned int finedPercentage = (unsigned int)trunc((float)rand() / (float)RAND_MAX * 30.0f);
					unsigned int finedAmount = (unsigned int)trunc(((float)discordGuildMember.data.currency.wallet * ((float)finedPercentage / 100.0f)));
					discordGuildMember.data.currency.wallet -= finedAmount;
					discordGuildMember.writeDataToDB();
					unsigned int userNewBalance = discordGuildMember.data.currency.wallet;

					unsigned int repaidPercentage = (unsigned int)trunc((float)rand() / (float)RAND_MAX * 50.0f);
					unsigned int repaidAmount = (unsigned int)trunc(((float)finedAmount * ((float)repaidPercentage / 100.0f)));
					targetGuildMember.data.currency.wallet += repaidAmount;
					targetGuildMember.writeDataToDB();
					unsigned int targetUserNewBalance = targetGuildMember.data.currency.wallet;

					msgString = "Oof! You've been caught while attempting to rob <@!" + targetUserID + ">!\n\n__Base probability of success:__ " + to_string(baseProbabilityOfSuccess) + "% \n";

					if (userGainAmount > 0 || userLossAmount < 0) {
						msgString += userGainString + userLossString + "\n__Resulting in a net probability of success gain of:__ " + to_string(userRollModTotal) + "%\n__For a final probability of success of:__ " + to_string(totalProbabilityOfSuccess) + "%\n";
					}

					msgString += "------\n**You've been fined " + to_string(finedPercentage) + "% of your wallet balance, which is " + to_string(finedAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "!\n<@!" + targetUserID + "> has been reimbursed " +
						to_string(repaidAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + " (" + to_string(repaidPercentage) + "%).**";

					msgString += "\n\n__Your new wallet balances are:__\n<@!" + userID + ">: " + to_string(userNewBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n<@!" + targetUserID
						+ ">: " + to_string(targetUserNewBalance) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;

					discordGuildMember.data.lastTimeRobbed = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					discordGuildMember.writeDataToDB();

					EmbedData messageEmbed;
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setDescription(msgString);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setTitle("__**Failed Robbery:**__");
					messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData dataPackage(args->eventData);
						dataPackage.embeds.push_back(messageEmbed);
						auto newEvent = InputEventManager::respondToEvent(dataPackage);
					}
					else {
						CreateInteractionResponseData dataPackage(args->eventData);
						dataPackage.data.embeds.push_back(messageEmbed);
						auto newEvent = InputEventManager::respondToEvent(dataPackage);
					}
				
				}
			}
			else {
				string msgString;
				unsigned int timeLeft = timeBetweenRobberies - currentTimeDifference;
				unsigned int hoursRemain = (unsigned int)trunc(timeLeft / msPerHour);
				unsigned int minutesRemain = (unsigned int)trunc((timeLeft % msPerHour) / msPerMinute);
				unsigned int secondsRemain = (unsigned int)trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

				if (hoursRemain > 0) {
					msgString = "Sorry, but you need to wait " + to_string(hoursRemain) + " hours, " + to_string(minutesRemain) + " minutes, and " + to_string(secondsRemain) + " seconds before you can rob someone again!";
				}
				else if (minutesRemain > 0) {
					msgString = "Sorry, but you need to wait " + to_string(minutesRemain) + " minutes and " + to_string(secondsRemain) + " seconds before you can rob someone again!";
				}
				else {
					msgString = "Sorry, but you need to wait " + to_string(secondsRemain) + " seconds before you can rob someone again!";
				}

				EmbedData messageEmbed;
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setDescription(msgString);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setTitle("__**Failed Robbery:**__");
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(messageEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
				else {
					CreateEphemeralInteractionResponseData dataPackage(args->eventData);
					dataPackage.data.embeds.push_back(messageEmbed);
					auto newEvent = InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent, 20000);
				}
			}
			co_return;
		}
	};
	Rob rob;
}

#endif