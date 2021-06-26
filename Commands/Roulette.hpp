// Roulette.hpp - Header for the "roulette" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ROULETTE_
#define _ROULETTE_

#include "../DiscordCoreClient.hpp"

constexpr unsigned int stringToInt(string str, int h = 0)
{
	return !str[h] ? 5381 : (stringToInt(str, h + 1) * 33) ^ str[h];
}

string getNumberString(string inputString, vector<string> redNumbers, vector<string> blackNumbers) {
	string returnString;
	for (unsigned int x = 0; x < redNumbers.size(); x += 1) {
		if (redNumbers[x].find(inputString)!= string::npos) {
			if (redNumbers[x].substr(12) ==  inputString) {
				returnString = redNumbers[x];
			}
		}
	}
	for (unsigned int x = 0; x < blackNumbers.size(); x += 1) {
		if (blackNumbers[x].find(inputString) != string::npos) {
			if (blackNumbers[x].substr(20) == inputString) {
				returnString = blackNumbers[x];
			}
		}
	}
	if (inputString ==  "0") {
		returnString = ":green_square:0";
	}
	else if (inputString ==  "00") {
		returnString = ":green_square:00";
	}
	return returnString;
}

void calculateResults(string finalRoll, DiscordCoreAPI::InputEventData newEvent, DiscordCoreAPI::DiscordUser discordUser, vector<string>redNumbers, vector<string> blackNumbers) {
	string msgStringFinal;
	string finalRollString = getNumberString(finalRoll, redNumbers, blackNumbers);
	msgStringFinal += "------\n__**Final Roll:**__ " + finalRollString + "\n------\n";
	DiscordCoreAPI::Guild guild = newEvent.discordCoreClient->guilds->getGuildAsync({ newEvent.getGuildId() }).get();
	DiscordCoreAPI::DiscordGuild discordGuild(guild.data);
	for (unsigned int x = 0; x < discordGuild.data.rouletteGame.rouletteBets.size(); x += 1) {
		bool isItAWinner = false;
		DiscordCoreAPI::GuildMember guildMember = newEvent.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = newEvent.getGuildId(), .guildMemberId = discordGuild.data.rouletteGame.rouletteBets.at(x).userId }).get();
		DiscordCoreAPI::DiscordGuildMember discordGuildMember(guildMember.data);
		msgStringFinal += "__**<@!" + guildMember.data.user.id + ">**__: ";
		int betAmount = discordGuild.data.rouletteGame.rouletteBets.at(x).betAmount;
		int payoutAmount = discordGuild.data.rouletteGame.rouletteBets.at(x).payoutAmount;
		vector<string> winningNumbers = discordGuild.data.rouletteGame.rouletteBets.at(x).winningNumbers;
		for (auto value : winningNumbers) {
		}
		for (unsigned int y = 0;y < winningNumbers.size(); y += 1) {
			if (finalRoll == "37") {
				finalRoll = "00";
			}
			if (getNumberString(finalRoll, redNumbers,
				blackNumbers) == winningNumbers[y]) {
				isItAWinner = true;
				break;
			}
		}
		if (isItAWinner == false) {
			payoutAmount = (-1 * betAmount);
		}
		if ((unsigned int)betAmount > discordGuildMember.data.currency.wallet) {
			if (discordGuild.data.rouletteGame.rouletteBets[x].betOptions != "") {
				msgStringFinal += "__**NSF:**__ Non-sufficient funds! __**Bet:**__ " + to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + "__**On:**__ " + discordGuild.data.rouletteGame.rouletteBets[x].betType +
					", " + discordGuild.data.rouletteGame.rouletteBets[x].betOptions + "\n";
			}
			else {
				msgStringFinal += "__**NSF:**__ Non-sufficient funds! __ **Bet:**__ " + to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + "__**On:**__ " +
					discordGuild.data.rouletteGame.rouletteBets[x].betType + "\n";
			}
		}
		else {
			if ((int)payoutAmount > discordGuild.data.casinoStats.largestRoulettePayout.amount) {
				discordGuild.data.casinoStats.largestRoulettePayout.amount = payoutAmount;
				discordGuild.data.casinoStats.largestRoulettePayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
				discordGuild.data.casinoStats.largestRoulettePayout.userId = discordGuildMember.data.guildMemberId;
				discordGuild.data.casinoStats.largestRoulettePayout.userName = discordGuildMember.data.userName;
			}
			discordGuild.data.casinoStats.totalRoulettePayout += payoutAmount;
			discordGuild.data.casinoStats.totalPayout += payoutAmount;
			discordGuildMember.data.currency.wallet += payoutAmount;
			discordGuildMember.writeDataToDB();

			if (discordGuild.data.rouletteGame.rouletteBets[x].betOptions != "") {
				msgStringFinal += to_string(payoutAmount) + discordUser.data.currencyName + " __**Bet:**__ " + to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + " __**On:**__ " +
					discordGuild.data.rouletteGame.rouletteBets[x].betType + ", " + discordGuild.data.rouletteGame.rouletteBets[x].betOptions + "\n";
			}
			else {
				msgStringFinal += to_string(payoutAmount) + discordUser.data.currencyName + " __**Bet:**__ " + to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + " __**On:**__ "+
					discordGuild.data.rouletteGame.rouletteBets[x].betType + "\n";
			}
		}
	}
	discordGuild.data.rouletteGame.currentlySpinning = false;
	discordGuild.data.rouletteGame.rouletteBets = vector<DiscordCoreAPI::RouletteBet>();
	discordGuild.writeDataToDB();
	msgStringFinal += "------";
	DiscordCoreAPI::EmbedData msgEmbed;
	msgEmbed.setAuthor(newEvent.getUserName(), newEvent.getAvatarURL());
	msgEmbed.setDescription(msgStringFinal);
	msgEmbed.setTitle("__**Roulette Results:**__");
	msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
	msgEmbed.setColor(discordGuild.data.borderColor);
	if (newEvent.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
		DiscordCoreAPI::ReplyMessageData dataPackage(newEvent);
		dataPackage.embeds.push_back(msgEmbed);
		DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
	}
	else {
		DiscordCoreAPI::CreateFollowUpMessageData dataPackage(newEvent);
		dataPackage.embeds.push_back(msgEmbed);
		DiscordCoreAPI::InputEventManager::respondToEvent(dataPackage);
	}
	return;
}

namespace DiscordCoreAPI {
	class RouletteGame :public BaseFunction {
	public:
		RouletteGame() {
			this->commandName = "roulette";
			this->helpDescription = "\n__**Roulette Usage:**__ !roulette = start to begin a game, and then !roulette = bet, BETAMOUNT, BETTYPE, BETOPTIONS to bet on it!\n__Where BETTYPE and BETOPTIONS are as follows:__\n```isbl\n";
			this->helpDescription += "Bet Type/Payout:     Bet Options:\n------------------------------------------------------------";
			this->helpDescription += "\n0        / 35:1  |";
			this->helpDescription += "\n00       / 35:1  |";
			this->helpDescription += "\nstraight / 35:1  | A single number to choose.";
			this->helpDescription += "\nrow      / 17:1  |";
			this->helpDescription += "\nsplit    / 17:1  | The first of two consecutive numbers.";
			this->helpDescription += "\nstreet   / 11:1  | The first of threee consecutive numbers.";
			this->helpDescription += "\nbasket   / 6:1   |";
			this->helpDescription += "\nsixline  / 5:1   | The first of six consecutive numbers.";
			this->helpDescription += "\n1stcolumn/ 2:1   |";
			this->helpDescription += "\n2ndcolumn/ 2:1   |";
			this->helpDescription += "\n3rdcolumn/ 2:1   |";
			this->helpDescription += "\n1stdozen / 2:1   |";
			this->helpDescription += "\n2nddozen / 2:1   |";
			this->helpDescription += "\n3rddozen / 2:1   |";
			this->helpDescription += "\nodd      / 1:1   |";
			this->helpDescription += "\neven     / 1:1   |";
			this->helpDescription += "\nred      / 1:1   |";
			this->helpDescription += "\nblack    / 1:1   |";
			this->helpDescription += "\n1to18    / 1:1   |";
			this->helpDescription += "\n19to36   / 1:1   |```";
		}
		
		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			apartment_context mainThread;
			co_await resume_background();
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();
			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm ==  true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (areWeAllowed == false) {
				co_return;
			}

			string whatAreWeDoing;
			unsigned int betAmount = 0;
			string betType;
			string betOptions;
			if (args->argumentsArray.size() == 0) {
				string msgString = "------\n**Please, enter either 'start' or 'bet  as the first argument! (!roulette = bet, BETAMOUNT, BETTYPE, BETOPTIONS)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
			if (args->argumentsArray.at(0)!= "start" && args->argumentsArray.at(0) != "bet") {
				string msgString = "------\n**Please, enter either a 'start' or 'bet' as the first arguments! (!roulette = bet, BETAMOUNT, BETTYPE, BETOPTIONS)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
			else if (args->argumentsArray[0] == "start") {
				if (discordGuild.data.rouletteGame.currentlySpinning == true) {
					string msgString = "------\n**Please, wait until the current game is over, before starting another one!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Game Issue:**__");
					if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
						DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
				whatAreWeDoing = "start";
			}
			else if (args->argumentsArray[0]== "bet") {
				if (discordGuild.data.rouletteGame.currentlySpinning ==  false) {
					string msgString = "------\n**Please, start a roulette game before placing any bets!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Game Issue:**__");
					if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
						DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
				whatAreWeDoing = "bet";
			}
			if (args->argumentsArray.size() > 1) {
				if (stoll(args->argumentsArray.at(1)) <= 0) {
					string msgString = "------\n**Please, enter a valid betting amount! (!roulette = bet, BETAMOUNT, BETTYPE, BETOPTIONS)** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
						DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
				else {
					betAmount = (unsigned int)stoll(args->argumentsArray[1]);
				}
			}

			regex digitRegExp("\\d{1,18}");
			vector<string> betTypes{ "0", "00", "straight", "row", "split", "street", "basket", "sixline", "1stcolumn", "2ndcolumn", "3rdcolumn", "1stdozen", "2nddozen", "3rddozen", "odd", "even", "red", "black", "1to18", "19to36" };
			vector<string> redNumbers{ ":red_square:32", ":red_square:19", ":red_square:21", ":red_square:25", ":red_square:34", ":red_square:27", ":red_square:36", ":red_square:30", ":red_square:23", ":red_square:5",
				":red_square:16", ":red_square:1", ":red_square:14", ":red_square:9", ":red_square:18", ":red_square:7", ":red_square:12", ":red_square:3" };
			vector<string> blackNumbers{ ":black_large_square:15", ":black_large_square:4", ":black_large_square:2", ":black_large_square:17", ":black_large_square:6", ":black_large_square:13", ":black_large_square:11", ":black_large_square:8", ":black_large_square:10",
				":black_large_square:24", ":black_large_square:33", ":black_large_square:20", ":black_large_square:31", ":black_large_square:22", ":black_large_square:29", ":black_large_square:28", ":black_large_square:35", ":black_large_square:26" };

			if (whatAreWeDoing == "bet") {
				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
				DiscordGuildMember discordGuildMember(guildMember.data);

				unsigned int currentBetAmount = 0;

				for (unsigned int x = 0; x < discordGuild.data.rouletteGame.rouletteBets.size(); x += 1) {
					if (discordGuildMember.data.guildMemberId == discordGuild.data.rouletteGame.rouletteBets[x].userId) {
						unsigned int number = discordGuild.data.rouletteGame.rouletteBets[x].betAmount;
						currentBetAmount += number;
					}
				}

				if ((currentBetAmount + betAmount) > discordGuildMember.data.currency.wallet) {
					string msgString = "------\n**Sorry, but you have insufficient funds in your wallet for placing that bet!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
						DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

				bool isValidType = false;
				for (unsigned int x = 0; x < betTypes.size(); x += 1) {
					if (args->argumentsArray[2] != "" && args->argumentsArray[2] == betTypes[x]) {
						isValidType = true;
						break;
					}
				}

				if (isValidType == false) {
					string msgString = "------\n**Please enter a valid bet type!Enter '!help = roulette' for more info!(!roulette = BETAMOUNT, BETTYPE, BETOPTIONS)** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
						DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
				else {
					betType = args->argumentsArray[2];
				}

				if (args->argumentsArray.size() > 3) {
					betOptions = args->argumentsArray[3];
				}

				unsigned int payoutAmount = 0;
				vector<string> winningNumbers;
				switch (stringToInt(betType)) {
				case stringToInt("0"): 
				{
					winningNumbers.push_back(":green_square:0");
					payoutAmount = betAmount * 35;
					break;
				}
				case stringToInt("00"):
				{
					winningNumbers.push_back(":green_square:00");
					payoutAmount = betAmount * 35;
					break;
				}
				case stringToInt("red"):
				{
					payoutAmount = betAmount;
					winningNumbers = redNumbers;
					break;
				}
				case stringToInt("black"):
				{
					payoutAmount = betAmount;
					winningNumbers = blackNumbers;
					break;
				}
				case stringToInt("straight"):
				{
					payoutAmount = betAmount * 35;
					if (args->argumentsArray.size() < 4 || args->argumentsArray[3] == "" || !regex_search(args->argumentsArray[3], digitRegExp)) {
						string msgString = "------\n**Please enter a valid value from the roulette wheel!(1 - 36)**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					if (stoll(args->argumentsArray[3]) < 1 || stoll(args->argumentsArray[3]) > 36) {
						string msgString = "------\n**Please enter a value between 1 and 36!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					winningNumbers.push_back(getNumberString(args->argumentsArray[3], redNumbers, blackNumbers));

					break;
				}
				case stringToInt("row"):
				{
					payoutAmount = betAmount * 17;
					winningNumbers.push_back(":green_square:0");
					winningNumbers.push_back(":green_square:00");
					break;
				}
				case stringToInt("split"):
				{
					payoutAmount = betAmount * 17;
					if (args->argumentsArray.size() < 4 || args->argumentsArray[3] == "" || !regex_search(args->argumentsArray[3], digitRegExp)) {
						string msgString = "------\n**Please enter a valid starting value for your split!(1 - 35)** \n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					if (stoll(args->argumentsArray[3]) < 1 || stoll(args->argumentsArray[3]) > 35) {
						string msgString = "-------\n**Please enter a value between 1 and 35!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					winningNumbers.push_back(getNumberString(args->argumentsArray[3], redNumbers, blackNumbers));
					stringstream sstream{};
					sstream << stoll(args->argumentsArray[3]) + 1;
					winningNumbers.push_back(getNumberString(sstream.str(), redNumbers, blackNumbers));

					break;
				}
				case stringToInt("street"):
				{
					payoutAmount = betAmount * 11;
					if (args->argumentsArray.size() < 4 || args->argumentsArray[3] == "" || !regex_search(args->argumentsArray[3], digitRegExp)) {
						string msgString = "------\n**Please enter a valid starting value for your street!(1 - 34)** \n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					if (stoll(args->argumentsArray[3]) < 1 || stoll(args->argumentsArray[3]) > 34) {
						string msgString = "-------\n**Please enter a value between 1 and 34!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					winningNumbers.push_back(getNumberString(args->argumentsArray[3], redNumbers, blackNumbers));
					stringstream stream01{};
					stream01 << stoll(args->argumentsArray[3]) + 1;
					winningNumbers.push_back(getNumberString(stream01.str(), redNumbers, blackNumbers));
					stringstream stream02{};
					stream02 << stoll(args->argumentsArray[3]) + 2;
					winningNumbers.push_back(getNumberString(stream02.str(), redNumbers, blackNumbers));

					break;
				}
				case stringToInt("basket"):
				{
					payoutAmount = betAmount * 6;
					winningNumbers.push_back(getNumberString("0", redNumbers, blackNumbers));
					winningNumbers.push_back(getNumberString("1", redNumbers, blackNumbers));
					winningNumbers.push_back(getNumberString("2", redNumbers, blackNumbers));
					winningNumbers.push_back(getNumberString("3", redNumbers, blackNumbers));
					winningNumbers.push_back(":green_square:00");
					break;
				}
				case stringToInt("sixline"):
				{
					payoutAmount = betAmount * 5;
					if (args->argumentsArray.size() < 4 || args->argumentsArray[3] == "" || !regex_search(args->argumentsArray[3], digitRegExp)) {
						string msgString = "------\n**Please enter a valid starting value for your sixline!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					if (stoll(args->argumentsArray[3]) < 1 || stoll(args->argumentsArray[3]) > 31) {
						string msgString = "------\n * *Please enter a value between 1 and 31!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
							DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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

					winningNumbers.push_back(getNumberString(args->argumentsArray[3], redNumbers, blackNumbers));
					stringstream stream01{};
					stream01 << stoll(args->argumentsArray[3]) + 1;
					winningNumbers.push_back(getNumberString(stream01.str(), redNumbers, blackNumbers));
					stringstream stream02{};
					stream02 << stoll(args->argumentsArray[3]) + 2;
					winningNumbers.push_back(getNumberString(stream02.str(), redNumbers, blackNumbers));
					stringstream stream03{};
					stream03 << stoll(args->argumentsArray[3]) + 3;
					winningNumbers.push_back(getNumberString(stream03.str(), redNumbers, blackNumbers));
					stringstream stream04{};
					stream04 << stoll(args->argumentsArray[3]) + 4;
					winningNumbers.push_back(getNumberString(stream04.str(), redNumbers, blackNumbers));
					stringstream stream05{};
					stream05 << stoll(args->argumentsArray[3]) + 5;
					winningNumbers.push_back(getNumberString(stream05.str(), redNumbers, blackNumbers));

					break;
				}
				case stringToInt("1stcolumn"):
				{
					payoutAmount = betAmount * 2;
					winningNumbers = { getNumberString("1", redNumbers, blackNumbers), getNumberString("4", redNumbers, blackNumbers), getNumberString("7", redNumbers, blackNumbers),  getNumberString("10", redNumbers, blackNumbers),
						 getNumberString("13", redNumbers, blackNumbers),  getNumberString("16", redNumbers, blackNumbers),  getNumberString("19", redNumbers, blackNumbers),  getNumberString("22", redNumbers, blackNumbers),
						 getNumberString("25", redNumbers, blackNumbers),  getNumberString("28", redNumbers, blackNumbers),  getNumberString("31", redNumbers, blackNumbers),  getNumberString("34", redNumbers, blackNumbers) };
					break;
				}
				case stringToInt("2ndcolumn"):
				{
					payoutAmount = betAmount * 2;
					winningNumbers = { getNumberString("2", redNumbers, blackNumbers),  getNumberString("5", redNumbers, blackNumbers),  getNumberString("8", redNumbers, blackNumbers),  getNumberString("11", redNumbers, blackNumbers),
						 getNumberString("14", redNumbers, blackNumbers),  getNumberString("17", redNumbers, blackNumbers),  getNumberString("20", redNumbers, blackNumbers),  getNumberString("23", redNumbers, blackNumbers),
						 getNumberString("26", redNumbers, blackNumbers),  getNumberString("29", redNumbers, blackNumbers),  getNumberString("32", redNumbers, blackNumbers),  getNumberString("35", redNumbers, blackNumbers) };
					break;
				}
				case stringToInt("3rdcolumn"):
				{
					payoutAmount = betAmount * 2;
					winningNumbers = { getNumberString("3", redNumbers, blackNumbers),  getNumberString("6", redNumbers, blackNumbers),  getNumberString("9", redNumbers, blackNumbers),  getNumberString("12", redNumbers, blackNumbers),
						 getNumberString("15", redNumbers, blackNumbers),  getNumberString("18", redNumbers, blackNumbers),  getNumberString("21", redNumbers, blackNumbers),  getNumberString("24", redNumbers, blackNumbers),
						 getNumberString("27", redNumbers, blackNumbers),  getNumberString("30", redNumbers, blackNumbers),  getNumberString("33", redNumbers, blackNumbers),  getNumberString("36", redNumbers, blackNumbers) };
					break;
				}
				case stringToInt("1stdozen"):
				{
					payoutAmount = betAmount * 2;
					for (unsigned int x = 1; x <= 12; x += 1) {
						stringstream stream{};
						stream << x;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}
				case stringToInt("2nddozen"):
				{
					payoutAmount = betAmount * 2;
					for (unsigned int x = 13; x <= 24; x += 1) {
						stringstream stream{};
						stream << x;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}	
				case stringToInt("3rddozen"):
				{
					payoutAmount = betAmount * 2;
					for (unsigned int x = 25; x <= 36; x += 1) {
						stringstream stream{};
						stream << x;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("odd"):
				{
					payoutAmount = betAmount;
					for (unsigned int x = 0; x < (36 / 2); x += 1) {
						stringstream stream{};
						stream << (x + 1) * 2 - 1;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("even"):
				{
					payoutAmount = betAmount;
					for (unsigned int x = 0; x < (36 / 2); x += 1) {
						stringstream stream{};
						stream << (x + 1) * 2;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("1to18"):
				{
					payoutAmount = betAmount;
					for (unsigned int x = 0; x < 18; x += 1) {
						stringstream stream{};
						stream << x + 1;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("19to36"):
				{
					payoutAmount = betAmount;
					for (unsigned int x = 0; x < 18; x += 1) {
						stringstream stream{};
						stream << x + 19;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break; 
				}
				default: {
					break;
				}					
				}

				RouletteBet newRouletteBet{ betAmount, betOptions,  betType, payoutAmount,  args->eventData.getAuthorId(),  winningNumbers };

				discordGuild.data.rouletteGame.rouletteBets.push_back(newRouletteBet);
				discordGuild.writeDataToDB();

				DiscordGuild discordGuild2(guild.data);

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				string winningNumbersNew;
				for (unsigned int x = 0; x < winningNumbers.size(); x+=1){
					winningNumbersNew += winningNumbers[x].c_str();
					if (x < winningNumbers.size() - 1) {
						winningNumbersNew += ", ";
					}					
				}
				msgEmbed.setDescription("------\n__**Bet Type:**__ " + betType + "\n__**Your winning numbers are:**__\n" + winningNumbersNew + "\n__**Your winning payout would be:**__\n" + to_string(payoutAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Roulette Bet Placed:**__");
				if (args->eventData.eventType == DiscordCoreAPI::InputEventType::REGULAR_MESSAGE) {
					DiscordCoreAPI::ReplyMessageData dataPackage(args->eventData);
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
			else if (whatAreWeDoing == "start") {
				discordGuild.data.rouletteGame.currentlySpinning = true;
				discordGuild.writeDataToDB();

				DispatcherQueueController queueController = queueController.CreateOnDedicatedThread();
				DispatcherQueue queue = queueController.DispatcherQueue();
				DispatcherQueueTimer queueTimer = queue.CreateTimer();

				int currentIndex = 3;

				InputEventData newEvent = args->eventData;
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("------\n__**" + to_string(currentIndex * 10) + " seconds remaining to place your roulette bets!**__\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Roulette Ball Rolling:**__");
				if (currentIndex == 3) {
					if (newEvent.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData dataPackage(newEvent);
						dataPackage.embeds.push_back(msgEmbed);
						newEvent = InputEventManager::respondToEvent(dataPackage);
					}
					else {
						CreateInteractionResponseData dataPackage(newEvent);
						dataPackage.data.embeds.push_back(msgEmbed);
						newEvent = InputEventManager::respondToEvent(dataPackage);
					}
				}

				currentIndex -= 1;
				queueTimer.Interval(chrono::milliseconds(10000));
				queueTimer.Tick([&](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args2) {
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription("------\n__**" + to_string(currentIndex * 10) + " seconds remaining to place your roulette bets!**__\n------");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Roulette Ball Rolling:**__");
					if (newEvent.eventType == InputEventType::REGULAR_MESSAGE) {
						EditMessageData dataPackage(newEvent);
						dataPackage.embeds.push_back(msgEmbed);
						InputEventManager::respondToEvent(dataPackage);
					}
					else {
						EditInteractionResponseData dataPackage(newEvent);
						dataPackage.embeds.push_back(msgEmbed);
						InputEventManager::respondToEvent(dataPackage);
					}
					currentIndex -= 1;
					if (currentIndex == -1) {
						DiscordCoreAPI::InputEventManager::deleteInputEventResponse(newEvent);
						stringstream stream;
						srand((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
						stream << (unsigned int)trunc(((float)rand() / (float)RAND_MAX) * 38.0f);
						string finalRoll = stream.str();
						calculateResults(finalRoll, args->eventData, *args->eventData.discordCoreClient->discordUser, redNumbers, blackNumbers);
						queueTimer.Stop();
					}
					});
				queueTimer.Start();
				concurrency::wait(31000);
			}
			co_await mainThread;
			co_return;
		}
	};
	RouletteGame roulette;
}

#endif 