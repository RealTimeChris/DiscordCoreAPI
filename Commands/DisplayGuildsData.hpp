// DisplayGuildsData.hpp - Header for the "display guilds data" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISPLAY_GUILDS_DATA_
#define _DISPLAY_GUILDS_DATA_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class DisplayGuildsData : public BaseFunction {
	public:
		DisplayGuildsData() {
			this->commandName = "displayguildsdata";
			this->helpDescription = "__**Display Guilds Data Usage:**__ !displayguildsdata or /displayguildsdata";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

			if (channel.data.type != ChannelType::DM && args->eventData.eventType != InputEventType::SLASH_COMMAND_INTERACTION) {
				InputEventManager::deleteInputEventResponse(args->eventData);
			}

			unsigned int currentCount = 0;
			map<string, Guild> theCache = receive(GuildManagerAgent::cache, 10000);
			InputEventData inputEvent = args->eventData;
			for (auto [key, value] : theCache) {
				string msgString = "__Guild Name:__ " + value.data.name + "\n";
				msgString += "__Guild ID:__ " + value.data.id + "\n";
				msgString += "__Member Count:__ " + to_string(value.data.memberCount) + "\n";

				msgString += "__Joined At:__ " + value.data.joinedAt + "\n";
				User owner = args->eventData.discordCoreClient->users->getUserAsync({ value.data.ownerID }).get();
				msgString += "__Guild Owner:__ <@!" + value.data.ownerID + "> " + owner.data.username + "#" + owner.data.discriminator;

				EmbedData messageEmbed;
				messageEmbed.setColor("FEFEFE");
				messageEmbed.setThumbnail(value.data.icon);
				messageEmbed.setTitle("__**Guild Data " + to_string(currentCount + 1) + " of " + to_string(theCache.size()) + "**__");
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);

				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(inputEvent);
					dataPackage.embeds.push_back(messageEmbed);
					InputEventManager::respondToEvent(dataPackage);
				}
				else if(args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					if (currentCount == 0) {
						CreateInteractionResponseData dataPackage(inputEvent);
						dataPackage.data.embeds.push_back(messageEmbed);
						inputEvent = InputEventManager::respondToEvent(dataPackage);
						EditInteractionResponseData editData(inputEvent);
						editData.embeds.push_back(messageEmbed);
						inputEvent = InputEventManager::respondToEvent(editData);
					}
					else {
						ReplyMessageData dataPackage(inputEvent);
						dataPackage.embeds.push_back(messageEmbed);
						InputEventManager::respondToEvent(dataPackage);
					}
					
				}
				currentCount += 1;
			};
			send(GuildManagerAgent::cache, theCache);
			co_return;
		};
	};
	DisplayGuildsData displayGuildsData{};
}
#endif
