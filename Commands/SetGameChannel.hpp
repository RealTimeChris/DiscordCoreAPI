// SetGameChannel.hpp - Header for the "set game channel" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_GAME_CHANNEL_
#define _SET_GAME_CHANNEL_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class SetGameChannel :public BaseFunction {
	public:
		SetGameChannel() {
			this->commandName = "setgamechannel";
			this->helpDescription = "__**Set Game Channel Usage:**__ !setgamechannel = ADD or !setgamechannel = REMOVE in the channel you would like to add/remove. Also !setgamechannel = PURGE to remove all channels, or just !setgamechannel to view the currently enabled channels!";
		}

		virtual task<void>execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm == true) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);
			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);
			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ args->eventData.getGuildId() , args->eventData.getAuthorId() }).get();
			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				co_return;
			}

			if (args->argumentsArray.size() == 0) {
				string msgString = "__You have the following channels enabled for gaming, on this server:__\n------\n";

				for (unsigned int x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
					string currentID = discordGuild.data.gameChannelIds[x];

					msgString += "__**Channel #" + to_string(x) + ":**__ <#" + currentID + "> \n";
				}

				msgString += "------\n";

				EmbedData messageEmbed; 
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Game Channels Enabled:**__");
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
				co_return;
			}
			if (args->argumentsArray.at(0).find("add")==string::npos && args->argumentsArray.at(0).find("remove") == string::npos && args->argumentsArray.at(0).find("purge") == string::npos && args->argumentsArray.at(0).find("view") == string::npos) {
				string msgString = "------\n**Please enter either 'add', 'remove', or 'purge' only! (!setgamechannel = ADDorREMOVEorPURGE)**\n------";
				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
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
				co_return;
			}
			if (args->argumentsArray.at(0).find("add") != string::npos) {

				string channelID = channel.data.id;
				for (unsigned int x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
					if (channelID ==  discordGuild.data.gameChannelIds[x]) {
						string msgString = "------\n**That channel is already on the list of enabled channels!**\n------";
						EmbedData messageEmbed;
						messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setDescription(msgString);
						messageEmbed.setTitle("__**Already Listed:**__");
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
						co_return;
					}
				}

				discordGuild.data.gameChannelIds.push_back(channelID);
				discordGuild.writeDataToDB();
				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription("**You've succesfully added <#" + channelID + "> to your list of accepted game channels!**");
				messageEmbed.setTitle("__**Game Channel Added:**__");
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
				co_return;
			}
			if (args->argumentsArray.at(0).find("remove") != string::npos) {
				string channelID;
				channelID = channel.data.id;

				string msgString;
				bool isItPresent = false;
				for (unsigned int x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
					if (channelID == discordGuild.data.gameChannelIds[x]) {
						isItPresent = true;
						discordGuild.data.gameChannelIds.erase(discordGuild.data.gameChannelIds.begin() + x);
						discordGuild.writeDataToDB();
						msgString += "You've succesfully removed the channel <#" + channelID + "> from the list of enabled gaming channels!";
					}
				}

				if (isItPresent == false) {
					string msgString2 = "------\n**That channel is not present on the list of enabled gaming channels!**\n------";
					EmbedData messageEmbed;
					messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString2);
					messageEmbed.setTitle("__**Missing from List:**__");
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
					co_return;
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Game Channel Removed:**__");
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
				co_return;
			}
			if (args->argumentsArray.at(0).find("purge") != string::npos) {
				string msgString;

				if (discordGuild.data.gameChannelIds.size() > 0) {
					msgString = "__You've removed the following channels from your list of 'enabled game channels:__\n------\n";

					for (unsigned int x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						string currentID = discordGuild.data.gameChannelIds[x];

						msgString += "__**Channel #" + to_string(x) + "**__<#" + currentID + "> \n";
					}

					msgString += "------\n__**The games will now work in ANY CHANNEL!**__";

					discordGuild.data.gameChannelIds = vector<string>();
					discordGuild.writeDataToDB();
				}
				else {
					msgString += "**Sorry, but there are no channels to remove!**";
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Game Channels Removed:**__");
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
				co_return;
			}

			co_return;
		}
	};

	SetGameChannel setGameChannel;
}
#endif