// Duel.hpp - Header for the "duel" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DUEL_
#define _DUEL_

#include "../DiscordCoreClient.hpp"

void executeCheck(DiscordCoreAPI::DiscordGuildMember* discordFromGuildMember, DiscordCoreAPI::DiscordGuildMember* discordToGuildMember,DiscordCoreAPI::DiscordGuild* discordGuild,  DiscordCoreAPI::InputEventData* newEvent, int* betAmount, 
	string* msgEmbedString,  string* fromUserIDNew, string* toUserIDNew) {
	discordFromGuildMember->getDataFromDB();
	int fromUserCurrency = discordFromGuildMember->data.currency.wallet;
	discordToGuildMember->getDataFromDB();
	int toUserCurrency = discordToGuildMember->data.currency.wallet;
	DiscordCoreAPI::User currentUser = newEvent->discordCoreClient->users->getUserAsync({ .userId = newEvent->getRequesterId() }).get();
	*fromUserIDNew;
	
	if (*betAmount > fromUserCurrency) {
		string msgString;
		msgString = *msgEmbedString + "\n\n__**Sorry, but you have insufficient funds in your wallet for placing that wager!**__";

		DiscordCoreAPI::EmbedData messageEmbed3;
		messageEmbed3.setAuthor(currentUser.data.username, currentUser.data.avatar);
		messageEmbed3.setDescription(msgString);
		messageEmbed3.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		messageEmbed3.setColor(discordGuild->data.borderColor);
		messageEmbed3.setTitle("__**Insufficient Funds:**__");
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::ReplyMessageData dataPackage(*newEvent);
			dataPackage.embeds.push_back(messageEmbed3);
			auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
			DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent02, 20000);
		}
		else {
			DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(*newEvent);
			auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
			DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
			dataPackage2.flags = 64;
			dataPackage2.embeds.push_back(messageEmbed3);
			auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
			DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
		}
		return;
	}
	if (*betAmount > toUserCurrency) {
		string msgString;
		msgString += *msgEmbedString + "\n\n__**Sorry, but they have insufficient funds in their wallet for accepting that wager!**__";
		DiscordCoreAPI::EmbedData messageEmbed4;
		messageEmbed4.setAuthor(currentUser.data.username, currentUser.data.avatar);
		messageEmbed4.setDescription(msgString);
		messageEmbed4.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		messageEmbed4.setColor(discordGuild->data.borderColor);
		messageEmbed4.setTitle("__**Insufficient Funds:**__");
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::ReplyMessageData dataPackage(*newEvent);
			dataPackage.embeds.push_back(messageEmbed4);
			auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
			DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent02, 20000);
		}
		else {
			DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(*newEvent);
			auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
			DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
			dataPackage2.embeds.push_back(messageEmbed4);
			dataPackage2.flags = 64;
			auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
			DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
		}
		return;
	}
	
	srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	int fromUserRoll = (int)trunc(((float)rand() / (float)RAND_MAX) * 100.0f);
	int toUserRoll = (int)trunc(((float)rand() / (float)RAND_MAX) * 100.0f);
	vector<DiscordCoreAPI::EmbedData> messageEmbeds;
	vector<string> finalStrings;
	vector<string> fromUserGainStrings;
	vector<string> fromUserLossStrings;
	vector<string> toUserGainStrings;
	vector<string> toUserLossStrings;
	int fromUserSelfMod = 0;
	int fromUserOppMod = 0;
	int toUserSelfMod = 0;
	int toUserOppMod = 0;
	int finalFromUserRoll = 0;
	int finalToUserRoll = 0;
	
	for (auto value : discordFromGuildMember->data.items) {
		if (value.selfMod > 0) {
			string currentString = "+" + to_string(value.selfMod);
			currentString += " of base roll from <@!" + *fromUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			fromUserGainStrings.push_back(currentString);
			fromUserSelfMod += value.selfMod;
		}
		if (value.oppMod < 0) {
			string currentString = to_string(value.oppMod);
			currentString += " of base roll from <@!" + *fromUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			toUserLossStrings.push_back(currentString);
			toUserOppMod += value.oppMod;
		}
	}

	for (auto value : discordToGuildMember->data.items) {
		if (value.selfMod > 0) {
			string currentString = "+" + to_string(value.selfMod);
			currentString += " of base roll from <@!" + *toUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			toUserGainStrings.push_back(currentString);
			toUserSelfMod += value.selfMod;
		}
		if (value.oppMod < 0) {
			string currentString = to_string(value.oppMod);
			currentString += " of base roll from <@!" + *toUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			fromUserLossStrings.push_back(currentString);
			fromUserOppMod += value.oppMod;
		}
	}
	
	finalFromUserRoll = (int)DiscordCoreAPI::applyAsymptoticTransform((float)(fromUserRoll + fromUserSelfMod + fromUserOppMod), 2000, 100);
	finalToUserRoll = (int)DiscordCoreAPI::applyAsymptoticTransform((float)(toUserRoll + toUserSelfMod + toUserOppMod), 2000, 100);

	string fromUserFooterString;
	if (finalFromUserRoll != fromUserRoll || fromUserOppMod != 0
		|| fromUserSelfMod != 0) {
		fromUserFooterString = "__**For a final roll of:**__ " + to_string(finalFromUserRoll) + "\n";
	}

	string toUserFooterString;
	if (finalToUserRoll != toUserRoll || toUserOppMod != 0 || toUserSelfMod != 0) {
		toUserFooterString = "__**For a final roll of:**__ " + to_string(finalToUserRoll) + "\n";
	}

	if (finalFromUserRoll > finalToUserRoll) {
		discordFromGuildMember->data.currency.wallet += *betAmount;
		discordFromGuildMember->writeDataToDB();
		discordToGuildMember->getDataFromDB();
		discordToGuildMember->data.currency.wallet -= *betAmount;
		discordToGuildMember->writeDataToDB();

		unsigned int currentPage = 0;

		string fromUserVicHeaderString;
		fromUserVicHeaderString = "<@!" + *fromUserIDNew + "> has defeated <@!" + *toUserIDNew + ">!\n__Your rolls were__:\n";

		finalStrings.resize(1);
		finalStrings[currentPage] += fromUserVicHeaderString;

		string midFooter1 = "__**<@!" + *fromUserIDNew + ">:**__ " + to_string(fromUserRoll) + "\n";
		string midFooter2 = "__**<@!" + *toUserIDNew + ">:**__ " + to_string(toUserRoll) + "\n";

		finalStrings[currentPage] += midFooter1;

		string finalFooterString;
		finalFooterString = "------\n__Your new wallet balances are:__\n";

		finalFooterString += "<@!" + *fromUserIDNew + ">: " + to_string(discordFromGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n" + "<@!" + *toUserIDNew + ">: " +
			to_string(discordToGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		vector<string> fromUserModStrings;
		for (auto value : fromUserLossStrings) {
			fromUserGainStrings.push_back(value);
		}
		fromUserModStrings = fromUserGainStrings;

		for (auto value : fromUserModStrings) {
			if ((finalStrings[currentPage].length() + value.length()
				+ midFooter1.length() + fromUserFooterString.length()) >= 2048) {
				finalStrings.resize(finalStrings.size() + 1);
				currentPage += 1;
				finalStrings[currentPage] = fromUserVicHeaderString;
			}
			finalStrings[currentPage] += value;
			if (value == fromUserModStrings.at(fromUserModStrings.size() - 1)) {
				finalStrings[currentPage] += fromUserFooterString;
			}
		}

		finalStrings[currentPage] += midFooter2;

		vector<string> toUserModStrings;
		for (auto value : toUserLossStrings) {
			toUserGainStrings.push_back(value);
		}
		toUserModStrings = toUserGainStrings;

		for (auto value : toUserModStrings) {
			if ((finalStrings[currentPage].size() + value.size()
				+ midFooter1.size() + toUserFooterString.size()) >= 2048) {
				finalStrings.resize(finalStrings.size() + 1);
				currentPage += 1;
				finalStrings[currentPage] = fromUserVicHeaderString;
			}
			finalStrings[currentPage] += value;
			if (value == toUserModStrings.at(toUserModStrings.size() - 1)) {
				finalStrings[currentPage] += toUserFooterString;
			}
		}
		finalStrings[currentPage] += finalFooterString;

		messageEmbeds.resize(currentPage + 1);
		for (unsigned int x = 0; x < finalStrings.size(); x += 1) {
			messageEmbeds[x] = DiscordCoreAPI::EmbedData();
			messageEmbeds[x].setAuthor(currentUser.data.username, currentUser.data.avatar);
			messageEmbeds[x].setColor("00FE00");
			messageEmbeds[x].setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			messageEmbeds[x].setTitle("__**Duel Results: " + to_string(x + 1) + " of " + to_string(finalStrings.size()) + "**__");
			messageEmbeds[x].setDescription(finalStrings[x]);
		}
	}
	else if (finalToUserRoll > finalFromUserRoll) {
		discordToGuildMember->data.currency.wallet += *betAmount;
		discordToGuildMember->writeDataToDB();
		discordFromGuildMember->getDataFromDB();
		discordFromGuildMember->data.currency.wallet -= *betAmount;
		discordFromGuildMember->writeDataToDB();

		unsigned int currentPage = 0;

		string toUserVicHeaderString;
		toUserVicHeaderString = "<@!" + *toUserIDNew + "> has defeated <@!" + *fromUserIDNew + "> !!\n__Your rolls were__:\n";

		finalStrings.resize(1);
		finalStrings[currentPage] += toUserVicHeaderString;

		string midFooter1 = "__**<@!" + *toUserIDNew + ">:**__ " + to_string(toUserRoll) + "\n";
		string midFooter2 = "__**<@!" + *fromUserIDNew + ">:**__ " + to_string(fromUserRoll) + "\n";

		finalStrings[currentPage] += midFooter1;
		
		string finalFooterString = "-----\n__Your new wallet balances are: __\n<@!" + *toUserIDNew + ">: " + to_string(discordToGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n<@!" +
			*fromUserIDNew + ">: " + to_string(discordFromGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";
			
		vector<string> toUserModStrings;
		for (auto value : toUserLossStrings) {
			toUserGainStrings.push_back(value);
		}
		toUserModStrings = toUserGainStrings;

		for (auto value : toUserModStrings) {
			if ((finalStrings[currentPage].length() + value.length()
				+ midFooter1.length() + toUserFooterString.length()) >= 2048) {
				finalStrings.resize(finalStrings.size() + 1);
				currentPage += 1;
				finalStrings[currentPage] = toUserVicHeaderString;
			}
			finalStrings[currentPage] += value;
			if (value == toUserModStrings.at(toUserModStrings.size() - 1)) {
				finalStrings[currentPage] += toUserFooterString;
			}
		}

		finalStrings[currentPage] += midFooter2;

		vector<string> fromUserModStrings;
		for (auto value : fromUserLossStrings) {
			fromUserGainStrings.push_back(value);
		}
		fromUserModStrings = fromUserGainStrings;

		for (auto value : fromUserModStrings) {
			if ((finalStrings[currentPage].size() + value.size()
				+ fromUserFooterString.size()) >= 2048) {
				finalStrings.resize(finalStrings.size() + 1);
				currentPage += 1;
				finalStrings[currentPage] = toUserVicHeaderString;
			}
			finalStrings[currentPage] += value;
			if (value == fromUserModStrings.at(fromUserModStrings.size() - 1)) {
				finalStrings[currentPage] += fromUserFooterString;
			}
		}
		finalStrings[currentPage] += finalFooterString;

		messageEmbeds.resize(currentPage + 1);
		for (unsigned int x = 0; x < finalStrings.size(); x += 1) {
			messageEmbeds[x] = DiscordCoreAPI::EmbedData();
			messageEmbeds[x].setAuthor(currentUser.data.username, currentUser.data.avatar);
			messageEmbeds[x].setColor("FE0000");
			messageEmbeds[x].setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			messageEmbeds[x].setTitle("__**Duel Results: " + to_string(x + 1) + " of " + to_string(finalStrings.size()) + "**__");
			messageEmbeds[x].setDescription(finalStrings[x]);
		}
	}
	else if (finalToUserRoll == finalFromUserRoll) {
		finalStrings.resize(1);
		messageEmbeds.push_back(DiscordCoreAPI::EmbedData());
		finalStrings[0] = "__**Looks like it was a draw! Nicely done!**__";
		messageEmbeds[0].setAuthor(currentUser.data.username, currentUser.data.avatar);
		messageEmbeds[0].setColor("FEFEFE");
		messageEmbeds[0].setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		messageEmbeds[0].setTitle("__**Duel Results: " + to_string(0 + 1) + " of " + to_string(finalStrings.size()) + "**__");
		messageEmbeds[0].setDescription(finalStrings[0]);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData dataPackage(*newEvent);
			dataPackage.embeds.push_back(messageEmbeds[0]);
			DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
		}
		else {
			DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(*newEvent);
			auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
			DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent02);
			DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
			dataPackage2.embeds.push_back(messageEmbeds[0]);
			dataPackage2.flags = 64;
			DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
		}
	}
	unsigned int currentPageIndex = 0;
	recurseThroughMessagePages(*fromUserIDNew, *newEvent, currentPageIndex, messageEmbeds, false, 120000, true);
}

void executeExit(string fromUserID, string toUserID, DiscordCoreAPI::DiscordGuild discordGuild, DiscordCoreAPI::InputEventData originalEvent) {
	string rejectedString;
	rejectedString = "Sorry, <@!" + fromUserID + ">, but <@!" + toUserID + "> has rejected your duel offer! (Timed Out!)";
	DiscordCoreAPI::User currentUser = originalEvent.discordCoreClient->users->getUserAsync({ originalEvent.getRequesterId() }).get();
	DiscordCoreAPI::EmbedData messageEmbed2;
	messageEmbed2.setAuthor(currentUser.data.username, currentUser.data.avatar);
	messageEmbed2.setColor(discordGuild.data.borderColor);
	messageEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
	messageEmbed2.setTitle("__**DUEL REJECTED!**__");
	messageEmbed2.setDescription(rejectedString);
	if (originalEvent.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
		DiscordCoreAPI::EditMessageData dataPackage(originalEvent);
		dataPackage.embeds.push_back(messageEmbed2);
		DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
	}
	else {
		DiscordCoreAPI::EditFollowUpMessageData dataPackage(originalEvent);
		dataPackage.embeds.push_back(messageEmbed2);
		DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
	}
}

namespace DiscordCoreAPI {
	class Duel : public BaseFunction {
	public:
		Duel() {
			this->commandName = "duel";
			this->helpDescription = "__**Duel Usage:**__ !duel = BETAMOUNT, @USERMENTION or /duel BETAMOUNT, @USERMENTIONTARGETUSERMENTION";
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

			regex numberRegExp("\\d{1,18}");
			regex idRegExp("\\d{18}");
			if (args->argumentsArray.size() < 1 || !regex_search(args->argumentsArray.at(0), idRegExp)) {
				string msgString = "------\n**Please enter a valid user mention or user ID! (!duel = BETAMOUNT, @USERMENTION)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent02, 20000);
				}
			 	else {
					DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(args->eventData);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
					dataPackage2.embeds.push_back(msgEmbed);
					dataPackage2.flags = 64;
					auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
					DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
				}
				co_return;
			}

			if (args->argumentsArray.size() < 2 || !regex_search(args->argumentsArray.at(1), numberRegExp) || stoll(args->argumentsArray.at(1)) < 0) {
				string msgString = "------\n**Please enter a valid bet amount! (!duel = BETAMOUNT, @USERMENTION)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent02, 20000);
				}
				else {
					DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(args->eventData);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
					dataPackage2.embeds.push_back(msgEmbed);
					dataPackage2.flags = 64;
					auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
					DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
				}
				co_return;
			}

			cmatch matchResults;
			regex_search(args->argumentsArray.at(1).c_str(), matchResults, numberRegExp);
			int betAmount = (int)stoll(matchResults.str());
			regex_search(args->argumentsArray.at(0).c_str(), matchResults, idRegExp);
			string toUserID = matchResults.str();
			string fromUserID = args->eventData.getAuthorId();

			GuildMember fromGuildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(),args->eventData.getAuthorId() }).get();
			DiscordGuildMember discordFromGuildMember(fromGuildMember.data);

			GuildMember toGuildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), toUserID }).get();
			DiscordGuildMember discordToGuildMember(toGuildMember.data);

			if (toGuildMember.data.user.username == "") {
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
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent02, 20000);
				}
				else {
					DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(args->eventData);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
					dataPackage2.embeds.push_back(msgEmbed);
					auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
					DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
				}
				co_return;
			}

			int fromUserCurrency = discordFromGuildMember.data.currency.wallet;
			int toUserCurrency = discordToGuildMember.data.currency.wallet;

			if (betAmount > fromUserCurrency) {
				string msgString = "------\n**Sorry, but you have insufficient funds in your wallet for placing that wager!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Insufficient Funds:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent02, 20000);
				}
				else {
					DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(args->eventData);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
					dataPackage2.embeds.push_back(msgEmbed);
					dataPackage2.flags = 64;
					auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
					DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
				}
				co_return;
			}
			if (betAmount > toUserCurrency) {
				string msgString = "------\n**Sorry, but they have insufficient funds in their wallet for accepting that wager!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Insufficient Funds:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					InputEventManager::deleteInputEventResponse(newEvent02, 20000);
				}
				else {
					DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage(args->eventData);
					auto newEvent02 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
					DiscordCoreAPI::CreateFollowUpMessageData dataPackage2(newEvent02);
					dataPackage2.embeds.push_back(msgEmbed);
					dataPackage2.flags = 64;
					auto newEvent03 = DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage2);
					DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent03, 20000);
				}
				co_return;
			}

			string msgEmbedString = "You've been challenged to a duel! :crossed_swords: \nBy user: <@!" + fromUserID + ">\nFor a wager of: " + to_string(betAmount) + " " +
				args->eventData.discordCoreClient->discordUser->data.currencyName + "\nReact with :white_check_mark: to accept or :x: to reject!";
			EmbedData messageEmbed;
			messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			messageEmbed.setDescription(msgEmbedString);
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setTitle("__**IT'S TIME TO DUEL!**__");
			messageEmbed.setColor(discordGuild.data.borderColor);
			InputEventData newEvent02;
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.content = "<@!" + toUserID + ">";
				dataPackage.embeds.push_back(messageEmbed);
				dataPackage.addButton(false, "check", "Accept", "✅", ButtonStyle::Success);
				dataPackage.addButton(false, "cross", "Reject", "❌", ButtonStyle::Success);
				newEvent02 = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateDeferredInteractionResponseData dataPackage(args->eventData);
				auto newEvent01 = InputEventManager::respondToEvent(dataPackage);
				CreateFollowUpMessageData dataPackage2(newEvent01);
				dataPackage2.embeds.push_back(messageEmbed);
				dataPackage2.content = "<@!" + toUserID + ">";
				dataPackage2.addButton(false, "check", "Accept", "✅", ButtonStyle::Success);
				dataPackage2.addButton(false, "cross", "Reject", "❌", ButtonStyle::Success);
				newEvent02 = InputEventManager::respondToEvent(dataPackage2);
			}
			Button button(newEvent02);
			ButtonInteractionData buttonInteractionData = button.getOurButtonData(false, 120000, toUserID);
			if (button.getButtonId() == "exit") {
				DiscordCoreAPI::DeferButtonResponseData deferButtonData(buttonInteractionData);
				DiscordCoreAPI::InputEventManager::respondToEvent(deferButtonData);
				executeExit(fromUserID, toUserID, discordGuild, newEvent02);
			}
			else if (button.getButtonId() == "check") {
				DiscordCoreAPI::DeferButtonResponseData deferButtonData(buttonInteractionData);
				DiscordCoreAPI::InputEventManager::respondToEvent(deferButtonData);
				executeCheck(&discordFromGuildMember, &discordToGuildMember, &discordGuild, &newEvent02, &betAmount, &msgEmbedString, &fromUserID, &toUserID);
			}
			else if (button.getButtonId() == "cross") {
				DiscordCoreAPI::DeferButtonResponseData deferButtonData(buttonInteractionData);
				DiscordCoreAPI::InputEventManager::respondToEvent(deferButtonData);
				string rejectedString = "Sorry, <@!" + fromUserID + ">, but <@!" + toUserID + "> has rejected your duel offer!";
				EmbedData messageEmbed5;
				messageEmbed5 = EmbedData();
				messageEmbed5.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed5.setColor("FE0000");
				messageEmbed5.setTimeStamp(getTimeAndDate());
				messageEmbed5.setTitle("__**DUEL REJECTED!**__");
				messageEmbed5.setDescription(rejectedString);
				if (newEvent02.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_RESPONSE || newEvent02.inputEventResponseType == InputEventResponseType::REGULAR_MESSAGE_EDIT) {
					EditMessageData dataPackage(newEvent02);
					dataPackage.embeds.push_back(messageEmbed5);
					dataPackage.content = "<@!" + fromUserID + ">";
					InputEventManager::respondToEvent(dataPackage);
				}
				else if (newEvent02.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_DEFERRED || newEvent02.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE
					|| newEvent02.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EDIT || newEvent02.inputEventResponseType == InputEventResponseType::INTERACTION_RESPONSE_EPHEMERAL) {
					EditInteractionResponseData dataPackage(newEvent02);
					dataPackage.embeds.push_back(messageEmbed5);
					dataPackage.content = "<@!" + fromUserID + ">";
					InputEventManager::respondToEvent(dataPackage);
				}
				else if (newEvent02.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE || newEvent02.inputEventResponseType == InputEventResponseType::INTERACTION_FOLLOW_UP_MESSAGE_EDIT) {
					EditFollowUpMessageData dataPackage(newEvent02);
					dataPackage.embeds.push_back(messageEmbed5);
					dataPackage.content = "<@!" + fromUserID + ">";
					InputEventManager::respondToEvent(dataPackage);
				}
			}
			co_return;
		}
	};
	Duel duel;
}
#endif