// Blackjack.hpp - Header for the blackjack command.
// May 31, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BLACKJACK_
#define _BLACKJACK_

#include "../DiscordCoreClient.hpp"

/**
	Refreshes a blackjack stack of Cards.
*/
vector<DiscordCoreAPI::Card> refreshBlackjackStack() {
	vector<DiscordCoreAPI::Card> newCardStack;
	if (newCardStack.size() == 0) {
		newCardStack.resize(312);
		for (unsigned int x = 0; x < 6; x += 1) {
			DiscordCoreAPI::Deck newDeck;
			for (unsigned int y = 0; y < 52; y += 1) {
				newCardStack[x * 52 + y] = newDeck.drawRandomcard();
			}
		}
	}
	return newCardStack;
}

/**
	Draws the next card from a stack of blackjack cards.
 */
DiscordCoreAPI::Card drawNextBlackjackCard(vector<DiscordCoreAPI::Card>* cardStack) {
	if (cardStack->size() == 0) {
		*cardStack = refreshBlackjackStack();
		DiscordCoreAPI::Card currentCard = cardStack->at(0);
		cardStack->erase(cardStack->begin());
		return currentCard;
	}

	DiscordCoreAPI::Card currentCard = cardStack->at(0);
	cardStack->erase(cardStack->begin());
	return currentCard;
}

void checkAndSetAceValues(vector<DiscordCoreAPI::Card>* playerHand, vector<unsigned int>* playerAceIndices) {
	auto newPlayerAceIndices = playerAceIndices;
	if (playerHand->at(playerHand->size() - 1).type ==  "Ace") {
		newPlayerAceIndices->resize(newPlayerAceIndices->size() + 1);
		newPlayerAceIndices->at((int)newPlayerAceIndices->size() - 1) = (int)playerHand->size() - 1;
	}
	auto playerNonAceValue = 0;
	for (auto x = 0; x < playerHand->size(); x += 1) {
		if (playerHand->at(x).type != "Ace") {
			playerNonAceValue += playerHand->at(x).value;
		}
	}
	auto newPlayerHand = playerHand;
	if (playerNonAceValue + 11 > 21 && newPlayerAceIndices->size()> 0) {
		newPlayerHand->at(newPlayerAceIndices->at(0)).value = 1;
	}
	else if (playerNonAceValue + 11 <= 21 && playerAceIndices->size()> 0) {
		newPlayerHand->at(playerAceIndices->at(0)).value = 11;
	}
	for (auto x = 0; x < playerAceIndices->size(); x += 1) {
		if (x > 0) {
			newPlayerHand->at(playerAceIndices->at(x)).value = 1;
		}
	}
}

void executeCrossResponse( DiscordCoreAPI::DiscordGuildMember* discordGuildMember, unsigned int* betAmount, DiscordCoreAPI::GuildMember* guildMember, DiscordCoreAPI::DiscordGuild* discordGuild, DiscordCoreAPI::InputEventData* newEvent,
	 vector<DiscordCoreAPI::Card>* userHand, vector<unsigned int>* dealerAceIndices, string* userID, vector<DiscordCoreAPI::Card>* dealerHand) {
	discordGuildMember->getDataFromDB();
	unsigned int fineAmount = 0;
	fineAmount = 1 * *betAmount;
	if (fineAmount > discordGuildMember->data.currency.wallet){
		string inPlayFooterString = "------\n__***Sorry, but you have insufficient funds for placing that wager now!***__\n------";
		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("FF0000");
		msgEmbed.setTitle("__**Blackjack Fail:**__");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("__**Game Status: Failed Wager**__", inPlayFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		return;
	}

	unsigned int newUserHandScore = 0;
	for (auto x = 0; x < userHand->size(); x += 1) {
		newUserHandScore += userHand->at(x).value;
	}
	unsigned int newDealerHandScore = 0;
	while (newDealerHandScore < 17) {
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += dealerHand->at(x).value;
		}
		if (newDealerHandScore >= 17) {
			break;
		}
		discordGuild->getDataFromDB();
		dealerHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
		discordGuild->writeDataToDB();

		checkAndSetAceValues(dealerHand, dealerAceIndices);
	}

	newDealerHandScore = 0;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		newDealerHandScore += dealerHand->at(x).value;
	}

	string dealerHandString;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		dealerHandString += dealerHand->at(x).suit + dealerHand->at(x).type;
	}

	string userHandString;
	for (auto x = 0; x < userHand->size(); x += 1) {
		userHandString += userHand->at(x).suit + userHand->at(x).type;
	}

	if ((newUserHandScore == 21 && newDealerHandScore != 21) || (newUserHandScore < 21
		&& newUserHandScore > newDealerHandScore) || (newUserHandScore < 21
			&& newDealerHandScore > 21)) {
		int payAmount = *betAmount;
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->data.user.username;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		discordGuildMember->getDataFromDB();
		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();

		string winFooterString = "------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " +
			newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Win:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			if (newEvent->getEmbeds()[0].description[0] != (char)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds()[0].description);
			}
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			if (newEvent->getEmbeds()[0].description[0] != (char)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds()[0].description);
			}
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}

		return;
	}
	else if (newUserHandScore == newDealerHandScore) {

		string tieFooterString = "------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Tie:**__");
		msgEmbed.setColor("0000FF");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			msgEmbed.setDescription(newEvent->getEmbeds()[0].description);
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			msgEmbed.setDescription(newEvent->getEmbeds()[0].description);
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		return;
	}
	else {
		int payAmount = -1 * *betAmount;
		discordGuildMember->getDataFromDB();
		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->data.user.username;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();
		string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			msgEmbed.setDescription(newEvent->getEmbeds()[0].description);
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			if (newEvent->getEmbeds()[0].description[0] != (char)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds()[0].description);
			}			
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		return;
	}
}

void executeCheckResponse(DiscordCoreAPI::DiscordGuildMember* discordGuildMember, unsigned int* betAmount, DiscordCoreAPI::GuildMember* guildMember, DiscordCoreAPI::DiscordGuild* discordGuild, DiscordCoreAPI::InputEventData* newEvent,
	unsigned int* newCardCount, vector<DiscordCoreAPI::Card>* userHand, vector<unsigned int>* userAceIndices, vector<unsigned int>* dealerAceIndices, string* userID, vector<DiscordCoreAPI::Card>* dealerHand) {

	discordGuildMember->getDataFromDB();
	DiscordCoreAPI::User currentUser = newEvent->discordCoreClient->users->getUserAsync({ newEvent->getRequesterId() }).get();

	unsigned int fineAmount = 0;
	fineAmount = 1 * *betAmount;
	if (fineAmount > discordGuildMember->data.currency.wallet) {
		string inPlayFooterString;
		inPlayFooterString = "------\n__***Sorry, but you have insufficient funds for placing that wager now!***__\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("FF0000");
		msgEmbed.setTitle("__**Blackjack Fail:**__");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("__**Game Status: Failed Wager**__", inPlayFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData* editData = new DiscordCoreAPI::EditMessageData(*newEvent);
			editData->components.push_back(newEvent->getComponents().at(0));
			editData->embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
			delete editData;
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData* editData = new DiscordCoreAPI::EditInteractionResponseData(*newEvent);
			editData->components.push_back(newEvent->getComponents().at(0));
			editData->embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
			delete editData;
		}
		return;
	}

	newCardCount += 1;
	discordGuild->getDataFromDB();
	userHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
	discordGuild->writeDataToDB();

	checkAndSetAceValues(userHand, userAceIndices);

	unsigned int newUserHandScore = 0;
	for (auto x = 0; x < userHand->size(); x += 1) {
		newUserHandScore += (*userHand).at(x).value;
	}

	if (newUserHandScore > 21) {
		int payAmount = (int)((float)*betAmount * -1.0);
		discordGuildMember->getDataFromDB();
		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->data.user.username;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		unsigned int newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += (*dealerHand).at(x).value;
		}

		string dealerHandString;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
		}

		string userHandString;
		for (auto x = 0; x < userHand->size(); x += 1) {
			userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
		}

		string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData* editData = new DiscordCoreAPI::EditMessageData(*newEvent);
			msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
			editData->components = vector<DiscordCoreAPI::ActionRowData>();
			editData->embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
			delete editData;
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData* editData = new DiscordCoreAPI::EditInteractionResponseData(*newEvent);
			editData->components = vector<DiscordCoreAPI::ActionRowData>();
			msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
			editData->embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
			delete editData;
		}
		return;
	}
	else if (newUserHandScore == 21) {
		unsigned int newDealerHandScore = 0;
		while (newDealerHandScore < 17) {
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				newDealerHandScore += (*dealerHand).at(x).value;
			}
			if (newDealerHandScore >= 17) {
				break;
			}
			discordGuild->getDataFromDB();
			dealerHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
			discordGuild->writeDataToDB();

			checkAndSetAceValues(dealerHand, dealerAceIndices);
		}

		newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += (*dealerHand).at(x).value;
		}

		if (newDealerHandScore == 21) {
			string dealerHandString;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
			}

			string userHandString;
			for (auto x = 0; x < userHand->size(); x += 1) {
				userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
			}

			string tieFooterString = "------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
			msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed.setTitle("__**Blackjack Tie:**__");
			msgEmbed.setColor("0000FF");
			msgEmbed.setFooter("Cards Remaining: " + discordGuild->data.blackjackStack.size());
			msgEmbed.setDescription(newEvent->getMessageData().embeds[0].description);
			msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
			msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
			msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
			if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
				DiscordCoreAPI::EditMessageData* editData = new DiscordCoreAPI::EditMessageData(*newEvent);
				msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
				editData->components = vector<DiscordCoreAPI::ActionRowData>();
				editData->embeds.push_back(msgEmbed);
				DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
				delete editData;
			}
			else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreAPI::EditInteractionResponseData* editData = new DiscordCoreAPI::EditInteractionResponseData(*newEvent);
				editData->components = vector<DiscordCoreAPI::ActionRowData>();
				msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
				editData->embeds.push_back(msgEmbed);
				DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
				delete editData;
			}
			return;
		}
		else {
			int payAmount = *betAmount;
			discordGuildMember->getDataFromDB();
			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB();
			discordGuild->getDataFromDB();
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->data.user.username;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB();

			string dealerHandString;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
			}

			string userHandString;
			for (auto x = 0; x < userHand->size(); x += 1) {
				userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
			}

			string winFooterString;
			winFooterString = "------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " +
				newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
			msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed.setTitle("__**Blackjack Win:**__");
			msgEmbed.setColor("00FF00");
			msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
			msgEmbed.setDescription(newEvent->getMessageData().embeds[0].description);
			msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
			msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
			msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
			if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
				DiscordCoreAPI::EditMessageData* editData = new DiscordCoreAPI::EditMessageData(*newEvent);
				msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
				editData->components = vector<DiscordCoreAPI::ActionRowData>();
				editData->embeds.push_back(msgEmbed);
				DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
				delete editData;
			}
			else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreAPI::EditInteractionResponseData* editData = new DiscordCoreAPI::EditInteractionResponseData(*newEvent);
				editData->components = vector<DiscordCoreAPI::ActionRowData>();
				msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
				editData->embeds.push_back(msgEmbed);
				DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
				delete editData;
			}
			return;
		}
	}
	else if (newUserHandScore < 21) {
 		unsigned int newDealerHandScore = (*dealerHand).at(0).value;

		string dealerHandString;
		dealerHandString += (*dealerHand).at(0).suit + (*dealerHand).at(0).type;

		string userHandString;
		for (auto x = 0; x < userHand->size(); x += 1) {
			userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
		}

		string inPlayFooterString = "------\n------";
		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		if (newEvent->getEmbeds()[0].description[0] != (char)0x40) {
			msgEmbed.setDescription(newEvent->getMessageData().embeds[0].description);
		}
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: In Play**__", inPlayFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			DiscordCoreAPI::EditMessageData* editData = new DiscordCoreAPI::EditMessageData(*newEvent);
			msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
			editData->components.push_back(newEvent->getComponents().at(0));
			editData->embeds.push_back(msgEmbed);
			*newEvent = DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
			delete editData;
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			DiscordCoreAPI::EditInteractionResponseData* editData = new DiscordCoreAPI::EditInteractionResponseData(*newEvent);
			editData->components.push_back(newEvent->getComponents().at(0));
			msgEmbed.setDescription(newEvent->getMessageData().embeds.at(0).description);
			editData->embeds.push_back(msgEmbed);
			*newEvent = DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
			delete editData;
		}
		DiscordCoreAPI::Button* button = new DiscordCoreAPI::Button(*newEvent);
		DiscordCoreAPI::ButtonInteractionData* buttonInteractionData = new DiscordCoreAPI::ButtonInteractionData(button->getOurButtonData(false, 120000));

		if (buttonInteractionData->customId == "") {
			string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
			DiscordCoreAPI::EmbedData msgEmbed2;
			msgEmbed2.setColor("FF0000");
			msgEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed2.setTitle("__**Blackjack Game:**__");
			msgEmbed.setAuthor(currentUser.data.username, currentUser.data.avatar);
			msgEmbed2.setDescription(timeOutString);
			vector<DiscordCoreAPI::EmbedData> embeds;
			embeds.push_back(msgEmbed2);
			if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
				DiscordCoreAPI::EditMessageData* editData = new DiscordCoreAPI::EditMessageData(*newEvent);
				editData->components = vector<DiscordCoreAPI::ActionRowData>();
				editData->embeds.push_back(msgEmbed2);
				*newEvent = DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
				delete editData;
			}
			else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreAPI::EditInteractionResponseData* editData = new DiscordCoreAPI::EditInteractionResponseData(*newEvent);
				editData->components = vector<DiscordCoreAPI::ActionRowData>();
				editData->embeds = embeds;
				*newEvent = DiscordCoreAPI::InputEventManager::respondToEvent(*editData);
				delete editData;
			}
			return;
		}
		else {
			DiscordCoreAPI::DeferButtonResponseData deferButtonData(*buttonInteractionData);
			DiscordCoreAPI::InputEventManager::respondToEvent(deferButtonData);
		}

		if (button->getButtonId() == "check") {
			delete button;
			delete buttonInteractionData;
			executeCheckResponse(discordGuildMember, betAmount, guildMember, discordGuild, newEvent, newCardCount, userHand, userAceIndices, dealerAceIndices, userID, dealerHand);
		}
		else if (button->getButtonId() == "cross") {
			delete button;
			delete buttonInteractionData;
			executeCrossResponse(discordGuildMember, betAmount, guildMember, discordGuild, newEvent, userHand, dealerAceIndices, userID, dealerHand);
		};
	};
	return;
};

void executeDoubleResponse(DiscordCoreAPI::DiscordGuildMember* discordGuildMember, unsigned int* betAmount, DiscordCoreAPI::GuildMember* guildMember, DiscordCoreAPI::DiscordGuild* discordGuild, DiscordCoreAPI::InputEventData* newEvent, 
	unsigned int* newCardCount, vector<DiscordCoreAPI::Card>* userHand, vector<unsigned int>* userAceIndices, vector<unsigned int>* dealerAceIndices, string* userID, vector<DiscordCoreAPI::Card>* dealerHand) {
	unsigned int fineAmount = 2 * *betAmount;
	if (fineAmount > discordGuildMember->data.currency.wallet || *newCardCount > 2) {
		string failedFooterString;
		discordGuildMember->getDataFromDB();
		if ((newEvent->getEmbeds()[0].fields[2].value.find("⏬") == string::npos) || *newCardCount > 2) {
			failedFooterString = "__***Sorry, but you do not have the option to double down!***__\n------\n✅ to Hit, ❎ to Stand.\n------";
		}
		else if (fineAmount > discordGuildMember->data.currency.wallet){
			failedFooterString = "__***Sorry, but you have insufficient funds for placing that wager now!***__\n------\n✅ to Hit, ❎ to Stand.\n------";
		}

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("00FF00");
		msgEmbed.setTitle("__**Blackjack:**__");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		DiscordCoreAPI::InputEventData eventData002;
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			msgEmbed.addField(newEvent->getEmbeds()[0].fields[0].name, newEvent->getEmbeds()[0].fields[0].value, newEvent->getEmbeds()[0].fields[0].Inline);
			msgEmbed.addField(newEvent->getEmbeds()[0].fields[1].name, newEvent->getEmbeds()[0].fields[1].value, newEvent->getEmbeds()[0].fields[1].Inline);
			msgEmbed.addField("__**Game Status: In Play**__", failedFooterString, false);
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = newEvent->getComponents();
			editData.embeds.push_back(msgEmbed);
			eventData002 = DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			msgEmbed.addField(newEvent->getEmbeds()[0].fields[0].name, newEvent->getEmbeds()[0].fields[0].value, newEvent->getEmbeds()[0].fields[0].Inline);
			msgEmbed.addField(newEvent->getEmbeds()[0].fields[1].name, newEvent->getEmbeds()[0].fields[1].value, newEvent->getEmbeds()[0].fields[1].Inline);
			msgEmbed.addField("__**Game Status: In Play**__", failedFooterString, false);
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = newEvent->getComponents();
			editData.embeds.push_back(msgEmbed);
			eventData002 = DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		DiscordCoreAPI::Button button(eventData002);
		DiscordCoreAPI::ButtonInteractionData buttonIntData = button.getOurButtonData(false, 60000);
		if (button.getButtonId() == "check") {
			DiscordCoreAPI::DeferButtonResponseData deferButtonData(buttonIntData);
			DiscordCoreAPI::InputEventManager::respondToEvent(deferButtonData);
			executeCheckResponse(discordGuildMember, betAmount, guildMember, discordGuild, &eventData002, newCardCount, userHand, userAceIndices, dealerAceIndices, userID, dealerHand);
		}
		else if (button.getButtonId() == "cross") {
			executeCrossResponse(discordGuildMember, betAmount, guildMember, discordGuild, &eventData002, userHand, dealerAceIndices, userID, dealerHand);
		}
		else if (button.getButtonId() == "exit") {
			string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
			DiscordCoreAPI::EmbedData msgEmbed2;
			msgEmbed2.setColor("FF0000");
			msgEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed2.setTitle("__**Blackjack Game:**__");
			msgEmbed2.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
			msgEmbed2.setDescription(timeOutString);
			if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
				DiscordCoreAPI::EditMessageData editMessageData(*newEvent);
				editMessageData.embeds.push_back(msgEmbed2);
				editMessageData.components = vector<DiscordCoreAPI::ActionRowData>();
				DiscordCoreAPI::InputEventManager::respondToEvent(editMessageData);
			}
			else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
				editData.components = vector<DiscordCoreAPI::ActionRowData>();
				editData.embeds.push_back(msgEmbed2);
				DiscordCoreAPI::InputEventManager::respondToEvent(editData);
			}
		}
		return;
	};

	newCardCount += 1;
	discordGuild->getDataFromDB();
	userHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
	discordGuild->writeDataToDB();

	checkAndSetAceValues(userHand, userAceIndices);

	unsigned int newUserHandScore = 0;
	for (auto x = 0; x < userHand->size(); x += 1) {
		newUserHandScore += userHand->at(x).value;
	}
	unsigned int newDealerHandScore = 0;
	while (newDealerHandScore < 17) {
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += dealerHand->at(x).value;
		}
		if (newDealerHandScore >= 17) {
			break;
		}
		discordGuild->getDataFromDB();
		dealerHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
		discordGuild->writeDataToDB();

		checkAndSetAceValues(dealerHand, dealerAceIndices);
	}

	newDealerHandScore = 0;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		newDealerHandScore += dealerHand->at(x).value;
	}

	string dealerHandString;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		dealerHandString += dealerHand->at(x).suit + dealerHand->at(x).type;
	}

	string userHandString;
	for (auto x = 0; x < userHand->size(); x += 1) {
		userHandString += userHand->at(x).suit + userHand->at(x).type;
	}

	if ((newUserHandScore == 21 && newDealerHandScore != 21)
		|| (newUserHandScore < 21 && newUserHandScore > newDealerHandScore)
		|| (newUserHandScore < 21 && newDealerHandScore > 21)) {
		int payAmount = 2 * *betAmount;

		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->data.user.username;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		string winFooterString = "------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " "
			+ newEvent->discordCoreClient->currentUser->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Win:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		return;
	}
	else if (newUserHandScore == newDealerHandScore) {
		string tieFooterString = "------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Tie:**__");
		msgEmbed.setColor("0000FF");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		return;
	}
	else {
		int payAmount = -2 * *betAmount;

		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->data.user.username;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember->data.currency.wallet) + " " + newEvent->discordCoreClient->discordUser->data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->data.user.username, guildMember->data.user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		if (newEvent->eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			DiscordCoreAPI::EditMessageData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		else if (newEvent->eventType == DiscordCoreAPI::InputEventType::SLASH_COMMAND_INTERACTION) {
			if (newEvent->getEmbeds().at(0).description[0] != (int)0x40) {
				msgEmbed.setDescription(newEvent->getEmbeds().at(0).description);
			}
			DiscordCoreAPI::EditInteractionResponseData editData(*newEvent);
			editData.components = vector<DiscordCoreAPI::ActionRowData>();
			editData.embeds.push_back(msgEmbed);
			DiscordCoreAPI::InputEventManager::respondToEvent(editData);
		}
		return;
	}
}

namespace DiscordCoreAPI {

	class Blackjack :public BaseFunction {
	public:
		Blackjack() {
			this->commandName = "blackjack";
			this->helpDescription = "__**Blackjack:**__ Enter !blackjack = BETAMOUNT, or /blackjack BETAMOUNT.";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			try {
				apartment_context mainThread;
				co_await resume_background();
				Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(args->eventData, channel);

				if (areWeInADm == true) {
					co_return;
				}

				if (args->eventData.eventType != InputEventType::SLASH_COMMAND_INTERACTION) {
					InputEventManager::deleteInputEventResponse(args->eventData);
				}

				Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild.data);

				bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

				if (areWeAllowed == false) {
					co_return;
				}

				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->fetchAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getRequesterId() }).get();
				GuildMember botMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getMessageData().guildId, .guildMemberId = args->eventData.discordCoreClient->discordUser->data.userId }).get();
				if (!DiscordCoreAPI::PermissionsConverter::checkForPermission(botMember, channel, Permissions::MANAGE_MESSAGES)) {
					string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Permissions Issue:**__");
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

				regex betRegExp("\\d{1,18}");
				if (args->argumentsArray.size() == 0 || !std::regex_search(args->argumentsArray.at(0), betRegExp) || stoll(args->argumentsArray.at(0)) < 1) {
					string msgString = "------\n**Please enter a valid bet amount!(!blackjack = BETAMOUNT)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
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
					co_await mainThread;
					co_return;
				}

				cmatch matchResults;
				std::regex_search(args->argumentsArray.at(0).c_str(), matchResults, betRegExp);
				unsigned int betAmount = (unsigned int)stoll(matchResults.str());

				std::srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

				string userID = guildMember.data.user.id;
				DiscordCoreAPI::DiscordGuildMember discordGuildMember(guildMember.data);

				if (betAmount > discordGuildMember.data.currency.wallet) {
					string msgString = "------\n**Sorry, but you have insufficient funds for placing that wager!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Funds:**__");
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
					co_await mainThread;
					co_return;
				}

				string finalMsgString;
				finalMsgString = "__**Your Bet Amount:**__ " + to_string(betAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n";

				string footerMsgStringOld = "------\n------";

				discordGuild.getDataFromDB();
				vector<Card> userHand;
				vector<unsigned int> userAceIndices;
				userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				unsigned int userHandScore = userHand.at(0).value + userHand.at(1).value;
				discordGuild.writeDataToDB();

				vector<Card> dealerHand;
				vector<unsigned int> dealerAceIndices;
				dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				unsigned int newDealerHandScore = dealerHand[0].value;
				discordGuild.writeDataToDB();
				string footerMsgString = footerMsgStringOld;

				if (userHandScore == 21) {
					if (dealerHand[0].value == 10 && dealerHand[1].type == "Ace") {
						dealerHand[1].value = 11;
					}
					else if (dealerHand[1].value == 10 && dealerHand[0].type == "Ace") {
						dealerHand[0].value = 11;
					}
					newDealerHandScore = dealerHand[0].value + dealerHand[1].value;
					string footerMsgString2;
					if (newDealerHandScore == 21) {
						footerMsgString2 = "\n------\n__**Your Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

						EmbedData finalMessageEmbed;
						finalMessageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						finalMessageEmbed.setTimeStamp(getTimeAndDate());
						finalMessageEmbed.setColor("0000FF");
						finalMessageEmbed.setDescription(finalMsgString);
						finalMessageEmbed.setTitle("__**Blackjack Tie:**__");
						finalMessageEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
						finalMessageEmbed.addField("Dealer's Hand: " + to_string(newDealerHandScore), dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
						finalMessageEmbed.addField("Player's Hand: " + to_string(userHandScore), userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
						finalMessageEmbed.addField("__**Game Status: Tie**__", footerMsgString2, false);

						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							ReplyMessageData responseData(args->eventData);
							responseData.embeds.push_back(finalMessageEmbed);
							InputEventManager::respondToEvent(responseData);
						}
						else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							CreateInteractionResponseData responseData(args->eventData);
							responseData.data.embeds.push_back(finalMessageEmbed);
							InputEventManager::respondToEvent(responseData);
						}
						co_await mainThread;
						co_return;
					}

					discordGuildMember.getDataFromDB();
					int payAmount = (unsigned int)trunc(1.5 * betAmount);
					discordGuildMember.data.currency.wallet += payAmount;
					discordGuildMember.writeDataToDB();
					discordGuild.getDataFromDB();
					if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
						discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
						discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
						discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
						discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.data.user.username;
					}
					discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
					discordGuild.data.casinoStats.totalPayout += payAmount;
					discordGuild.writeDataToDB();

					footerMsgString2 = "\n------\n__**Payout Amount:**__ " + to_string(payAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Your New Wallet Balance:**__ " +
						to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

					DiscordCoreAPI::EmbedData finalMessageEmbed;
					finalMessageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					finalMessageEmbed.setTimeStamp(getTimeAndDate());
					finalMessageEmbed.setColor("00FF00");
					finalMessageEmbed.setDescription(finalMsgString);
					finalMessageEmbed.setTitle("__**Blackjack Win:**__");
					finalMessageEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
					finalMessageEmbed.addField("Dealer's Hand: ", dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
					finalMessageEmbed.addField("Player's Hand: ", userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
					finalMessageEmbed.addField("__**Game Status: Player Wins**__", footerMsgString2, false);
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData responseData(args->eventData);
						responseData.embeds.push_back(finalMessageEmbed);
						InputEventManager::respondToEvent(responseData);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						CreateInteractionResponseData responseData(args->eventData);
						responseData.data.embeds.push_back(finalMessageEmbed);
						InputEventManager::respondToEvent(responseData);
					}
					co_await mainThread;
					co_return;
				}
				bool canWeDoubleDown = false;
				if ((userHandScore == 9) || (userHandScore == 10) || (userHandScore == 11)) {
					canWeDoubleDown = true;
				}
				EmbedData finalMessageEmbed;
				finalMessageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				finalMessageEmbed.setTimeStamp(getTimeAndDate());
				finalMessageEmbed.setColor("00FF00");
				finalMessageEmbed.setDescription(finalMsgString);
				finalMessageEmbed.setTitle("__**Blackjack:**__");
				finalMessageEmbed.setFooter("Cards Remaining: " + to_string(discordGuild.data.blackjackStack.size()));
				DiscordCoreAPI::EmbedFieldData field01;
				field01.name = "Dealer's Hand: " + to_string(newDealerHandScore);
				field01.value = dealerHand[0].suit + dealerHand[0].type;
				field01.Inline = true;
				finalMessageEmbed.fields.push_back(field01);
				DiscordCoreAPI::EmbedFieldData field02;
				field02.name = "Player's Hand: " + to_string(userHandScore);
				field02.value = userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type;
				field02.Inline = true;
				finalMessageEmbed.fields.push_back(field02);
				DiscordCoreAPI::EmbedFieldData field03;
				field03.name = "__**Game Status: In Play**__";
				field03.value = footerMsgString;
				field03.Inline = false;
				finalMessageEmbed.fields.push_back(field03);
				DiscordCoreAPI::InputEventData event001;
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData replyMessageData(args->eventData);
					replyMessageData.embeds.push_back(finalMessageEmbed);
					replyMessageData.addButton(false, "check", "Hit", "✅", ButtonStyle::Success);
					replyMessageData.addButton(false, "cross", "Stand", "❎", ButtonStyle::Success);
					if (canWeDoubleDown) {
						replyMessageData.addButton(false, "double", "Double-Down", "⏬", ButtonStyle::Primary);
					}
					event001 = InputEventManager::respondToEvent(replyMessageData);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateInteractionResponseData replyInteractionData(args->eventData);
					replyInteractionData.data.embeds.push_back(finalMessageEmbed);
					replyInteractionData.addButton(false, "check", "Hit", "✅", ButtonStyle::Success);
					replyInteractionData.addButton(false, "cross", "Stand", "❎", ButtonStyle::Success);
					if (canWeDoubleDown) {
						replyInteractionData.addButton(false, "double", "Double-Down", "⏬", ButtonStyle::Primary);
					}
					event001 = InputEventManager::respondToEvent(replyInteractionData);
					EditInteractionResponseData editData(event001);
					editData.embeds.push_back(finalMessageEmbed);
					editData.components = event001.getComponents();
					event001 = InputEventManager::respondToEvent(editData);
				}
				DiscordCoreAPI::Button button(event001);
				ButtonInteractionData buttonIntData = button.getOurButtonData(false, 480000);
				if (button.getButtonId() == "exit"){
					string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
					EmbedData msgEmbed2;
					msgEmbed2.setColor("FF0000");
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setTitle("__**Blackjack Game:**__");
					msgEmbed2.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed2.setDescription(timeOutString);
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						EditMessageData editMessageData(event001);
						editMessageData.embeds.push_back(msgEmbed2);
						editMessageData.components = vector<DiscordCoreAPI::ActionRowData>();
						InputEventManager::respondToEvent(editMessageData);
					}
					else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						EditInteractionResponseData editData(event001);
						editData.components = vector<DiscordCoreAPI::ActionRowData>();
						editData.embeds.push_back(msgEmbed2);
						InputEventManager::respondToEvent(editData);
					}
					co_await mainThread;
					co_return;
				}
				else {
					DiscordCoreAPI::DeferButtonResponseData deferButtonData(buttonIntData);
					DiscordCoreAPI::InputEventManager::respondToEvent(deferButtonData);
				}

				if (event001.eventType== InputEventType::REGULAR_MESSAGE) {
					EditMessageData editData(event001);
					finalMessageEmbed.fields.at(2).value = footerMsgStringOld;
					editData.components = event001.getComponents();
					if (canWeDoubleDown) {
						editData.components.at(0).components.erase(editData.components.at(0).components.begin() + 2);
					}					
					editData.embeds.push_back(finalMessageEmbed);
					event001 = InputEventManager::respondToEvent(editData);
				}
				else if (event001.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					EditInteractionResponseData editData(event001);
					editData.components = event001.getComponents();
					if (canWeDoubleDown) {
						editData.components.at(0).components.erase(editData.components.at(0).components.begin() + 2);
					}
					finalMessageEmbed.fields.at(2).value = footerMsgStringOld;
					editData.embeds.push_back(finalMessageEmbed);
					event001 = InputEventManager::respondToEvent(editData);
				}
				unsigned int newCardCount = 0;
				if (button.getButtonId() == "check") {
					executeCheckResponse(&discordGuildMember, &betAmount, &guildMember, &discordGuild, &event001, &newCardCount, &userHand, &userAceIndices, &dealerAceIndices, &userID, &dealerHand);
				}
				else if (button.getButtonId() == "cross") {
					executeCrossResponse(&discordGuildMember, &betAmount, &guildMember, &discordGuild, &event001, &userHand, &dealerAceIndices, &userID, &dealerHand);
				}
				else if (button.getButtonId() == "double") {
					executeDoubleResponse(&discordGuildMember, &betAmount, &guildMember, &discordGuild, &event001, &newCardCount, &userHand, &userAceIndices, &dealerAceIndices, &userID, &dealerHand);
				};
				co_await mainThread;
				co_return;
			}
				catch (exception& e) {
					cout << "Blackjack::execute() Error: " << e.what() << endl << endl;
				}
			};
			
		};
		Blackjack blackJack{};
	};
#endif