// Leaderboard.hpp - Header for the "leaderboard" command.
// Jun 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _LEADERBOARD_
#define _LEADERBOARD_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Leaderboard :public BaseFunction {
	public:
		Leaderboard() {
			this->commandName = "leaderboard";
			this->helpDescription = "__**Leaderboard Usage:**__ !leaderboard or /leaderboard.";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();
			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm == true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);
			InputEventData newEvent = args->eventData;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateDeferredInteractionResponseData dataPackage(args->eventData);
				dataPackage.requesterId = args->eventData.getAuthorId();
				newEvent = InputEventManager::respondToEvent(dataPackage);
			}

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (areWeAllowed == false) {
				co_return;
			}

			unsigned int minIdx = 0;

			vector<DiscordGuildMember> membersArray;
			for (auto value : guild.data.members) {
				GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId(), value.user.id }).get();
				DiscordGuildMember guildMemberNew(guildMember.data);
				membersArray.push_back(guildMemberNew);
				if (guildMemberNew.data.userName != "") {
					
				}
			}
			
			unsigned int len = (unsigned int)membersArray.size();
			for (unsigned int x = 0; x < len; x += 1) {
				minIdx = x;
				for (unsigned int y = x + 1; y < len; y += 1) {
					if (membersArray[y].data.currency.wallet > membersArray[minIdx].data.currency.wallet) {
						minIdx = y;
					}
				}
				DiscordGuildMember temp = membersArray[x];
				membersArray[x] = membersArray[minIdx];
				membersArray[minIdx] = temp;
			}
			
			unsigned int membersPerPage = 20;
			unsigned int totalPageCount = 0;
			if (membersArray.size() % membersPerPage > 0) {
				totalPageCount = (unsigned int)trunc(membersArray.size() / membersPerPage) + 1;
			}
			else {
				totalPageCount = (unsigned int)trunc(membersArray.size() / membersPerPage);
			}
			unsigned int currentPage = 0;
			vector<EmbedData> pageEmbeds;
			vector<string> pageStrings;

			for (unsigned int x = 0; x < (unsigned int)membersArray.size(); x += 1) {
				if (x% membersPerPage ==  0) {
					pageEmbeds.push_back(EmbedData());
					pageStrings.push_back("");
				}			

				string msgString = "";
				msgString += "__**#" + to_string(currentPage * membersPerPage + ((x % membersPerPage) + 1)) + " | Name:**__ <@!" + membersArray[x].data.guildMemberId + ">** | __" + args->eventData.discordCoreClient->discordUser->data.currencyName +
					":__** " + to_string(membersArray[x].data.currency.wallet) + "\n";
				 
					pageStrings[currentPage] += msgString;
				if (x% membersPerPage ==  membersPerPage - 1 || x ==  (unsigned int)membersArray.size() - 1) {
					pageEmbeds[currentPage].setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					pageEmbeds[currentPage].setDescription(pageStrings[currentPage]);
					pageEmbeds[currentPage].setTimeStamp(getTimeAndDate());
					pageEmbeds[currentPage].setTitle("__**Leaderboard (Wallet) (Page " + to_string(currentPage + 1) + " of " + to_string(totalPageCount) + ")**__");
					pageEmbeds[currentPage].setColor(discordGuild.data.borderColor);
					currentPage += 1;
				}
			}

			unsigned int currentPageIndex = 0;
			string userID = args->eventData.getAuthorId();
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(newEvent);
				dataPackage.embeds.push_back(pageEmbeds[currentPageIndex]);
				newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			else {
				EditInteractionResponseData dataPackage(newEvent);
				dataPackage.embeds.push_back(pageEmbeds[currentPageIndex]);
				newEvent = InputEventManager::respondToEvent(dataPackage);
			}
			recurseThroughMessagePages(userID, newEvent, currentPageIndex, pageEmbeds, true, 120000);
			discordGuild.writeDataToDB();
			co_return;

		}
	};

	Leaderboard leaderboard;
}
#endif