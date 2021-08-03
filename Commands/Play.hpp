// Play.hpp - Header for the "play" command.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _PLAY_
#define _PLAY_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Play: public BaseFunction {
	public:
		Play() {
			this->commandName = "play";
			this->helpDescription = "__**Play Usage:**__ !play = SONGNAME  or /play SONGNAME";
		}

		Play* create(){
			return new Play;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm) {
				co_return;
			}

			InputEventManager::deleteInputEventResponseAsync(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);
			
			InputEventData newEvent;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateDeferredInteractionResponseData dataPackage(args->eventData);
				newEvent = InputEventManager::respondToEvent(dataPackage);
			}

			YouTubeAPI youtubeAPI;
			vector<YouTubeSearchResult> searchResults;
			if (args->argumentsArray.size() > 0) {
				searchResults = youtubeAPI.searchForVideo(args->argumentsArray[0]);
			}

			vector<EmbedData> embedsFromSearch;
			unsigned int x = 0;
			for (auto value : searchResults) {
				x += 1;
				EmbedData newEmbed;
				newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				newEmbed.setDescription("__**Title:**__ [" + value.videoTitle + "](" + value.videoURL + ")" + "\n__**Description:**__ " + value.description + "\n__**Duration:**__ " + value.duration);
				newEmbed.setTimeStamp(getTimeAndDate());
				newEmbed.setTitle("__**" + to_string(x) + " of " + to_string(searchResults.size()) + "**__");
				newEmbed.setImage(value.thumbNailURL);
				newEmbed.setColor(discordGuild.data.borderColor);
				embedsFromSearch.push_back(newEmbed);
			}
			unsigned int currentPageIndex = 0;
			RecurseThroughMessagePagesData returnData;
			if (embedsFromSearch.size() > 0) {
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.embeds.push_back(embedsFromSearch[0]);
					newEvent = InputEventManager::respondToEvent(dataPackage);
				}
				else {
					EditInteractionResponseData dataPackage(newEvent);
					dataPackage.embeds.push_back(embedsFromSearch[0]);
					newEvent = InputEventManager::respondToEvent(dataPackage);
				}

				returnData = recurseThroughMessagePages(args->eventData.getAuthorId(), newEvent, currentPageIndex, embedsFromSearch, false, 120000, true);
			}
			else {
				if (args->eventData.eventType != InputEventType::REGULAR_MESSAGE) {
					InputEventManager::deleteInputEventResponseAsync(newEvent);
				}
				co_return;
			}
			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
			if (returnData.inputEventData.discordCoreClient != nullptr) {
				if (guildMember.data.voiceData.channelId != "") {
					auto guildBuffer = args->eventData.discordCoreClient->audioBuffersMap.at(args->eventData.getGuildId());
					//auto searchResults01 = youtubeAPI.searchForVideo("Skrillex workin for it");
					auto voiceConnection = guild.connectToVoice(guildMember.data.voiceData.channelId, args->eventData.discordCoreClient->pWebSocketConnectionAgent, guildBuffer);
					//youtubeAPI.downloadAudio(searchResults01[0], guildBuffer);
					youtubeAPI.downloadAudio(searchResults[returnData.currentPageIndex], guildBuffer).get();
					voiceConnection->play(true);
					agent::wait(voiceConnection.get());
					guild.disconnectFromVoice();
					cout << "WERE HERE WERE HERE WERE HERE" << endl;
					if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
						ReplyMessageData dataPackage(args->eventData);
						EmbedData newEmbed;
						newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						newEmbed.setColor("F3f3ae");
						newEmbed.setDescription("__**TESTING COMPLETE**__");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**TEST STATUS**__");
						dataPackage.embeds.push_back(newEmbed);
						newEvent = InputEventManager::respondToEvent(dataPackage);
					}
					else {
						EditInteractionResponseData dataPackage(newEvent);
						EmbedData newEmbed;
						newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						newEmbed.setColor("F3f3ae");
						newEmbed.setDescription("__**TESTING COMPLETE**__");
						newEmbed.setTimeStamp(getTimeAndDate());
						newEmbed.setTitle("__**TEST STATUS**__");
						dataPackage.embeds.push_back(newEmbed);
						newEvent = InputEventManager::respondToEvent(dataPackage);
					}
				}
			}
			co_return;
		}

	};
	Play play;
}

#endif