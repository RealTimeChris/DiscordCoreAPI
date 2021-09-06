// DisplayGuildsData.hpp - Header for the "display guilds data" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISPLAY_GUILDS_DATA_
#define _DISPLAY_GUILDS_DATA_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class DisplayGuildsData : public BaseFunction {
	public:
		DisplayGuildsData() {
			this->commandName = "displayguildsdata";
			this->helpDescription = "Displays some info about the servers that this bot is in.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter !displayguildsdata = musichouse or /displayguildsdata.\n------");
			msgEmbed.setTitle("__**Display Guild's Data Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		DisplayGuildsData* create() {
			return new DisplayGuildsData;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			Channel channel = Channels::getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE && Channels::getChannelAsync({ args->eventData.getChannelId() }).get().type != ChannelType::DM) {
				InputEvents::deleteInputEventResponseAsync(args->eventData);
			}
			else if (Channels::getChannelAsync({ args->eventData.getChannelId() }).get().type == ChannelType::DM || args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				args->argumentsArray.push_back("musichouse");
			}

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			if (args->argumentsArray.size() == 0 || (args->argumentsArray.at(0) != "janny" && args->argumentsArray.at(0) != "musichouse" && args->argumentsArray.at(0) != "gamehouse")) {
				string msgString = "------\n**Please, enter the name of a bot as the first argument! (!displayguildsdata = BOTNAME)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Invalid Or Missing Arguments:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(args->eventData);
					dataPackage.addMessageEmbed(msgEmbed);
					auto eventNew = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(eventNew, 20000);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					CreateEphemeralInteractionResponseData responseData(args->eventData);
					responseData.addMessageEmbed(msgEmbed);
					auto eventNew = InputEvents::respondToEvent(responseData);
					InputEvents::deleteInputEventResponseAsync(eventNew, 20000);
				}
				co_return;
			}
			if (args->argumentsArray.at(0) != "musichouse") {
				co_return;
			}

			unsigned int currentCount = 0;
			vector<Guild> theCache = Guilds::getAllGuildsAsync().get();
			InputEventData inputEvent = args->eventData;
			for (auto value : theCache) {
				string msgString = "__Guild Name:__ " + value.name + "\n";
				msgString += "__Guild ID:__ " + value.id + "\n";
				msgString += "__Member Count:__ " + to_string(value.memberCount) + "\n";

				msgString += "__Joined At:__ " + value.joinedAt + "\n";
				User owner = Users::getUserAsync({ value.ownerID }).get();
				msgString += "__Guild Owner:__ <@!" + value.ownerID + "> " + owner.username + "#" + owner.discriminator + "\n";
				msgString += "__Created At:__ " + value.createdAt;

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor("FEFEFE");
				messageEmbed.setThumbnail(value.icon);
				messageEmbed.setTitle("__**Guild Data " + to_string(currentCount + 1) + " of " + to_string(theCache.size()) + "**__");
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);

				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData dataPackage(inputEvent);
					dataPackage.addMessageEmbed(messageEmbed);
					InputEvents::respondToEvent(dataPackage);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					if (currentCount == 0) {
						CreateInteractionResponseData dataPackage(inputEvent);
						dataPackage.addMessageEmbed(messageEmbed);
						inputEvent = InputEvents::respondToEvent(dataPackage);
						EditInteractionResponseData editData(inputEvent);
						editData.addMessageEmbed(messageEmbed);
						inputEvent = InputEvents::respondToEvent(editData);
					}
					else {
						ReplyMessageData dataPackage(inputEvent);
						dataPackage.addMessageEmbed(messageEmbed);
						InputEvents::respondToEvent(dataPackage);
					}

				}
				currentCount += 1;
			};
			co_return;
		};
	};
}
#endif
