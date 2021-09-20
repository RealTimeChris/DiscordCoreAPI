// SetMusicChannel.hpp - Header for the "set music channel" command.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_MUSIC_CHANNEL_
#define _SET_MUSIC_CHANNEL_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {
	class SetMusicChannel :public BaseFunction {
	public:
		SetMusicChannel() {
			this->commandName = "setmusicchannel";
			this->helpDescription = "Sets the channels from which you can issue music commands!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !setmusicchannel = add or /setmusichannel add in order to add the current channel.\nAlternatively enter !setmusicchannel = remove or /setmusicchannel remove to remove the current channel.\nAlso, enter !setmusicchannel = view or = purge OR /setmusicchannel view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**Set Music Channel Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		SetMusicChannel* create() {
			return new SetMusicChannel;
		}

		virtual task<void>execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = Channels::getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm == true) {
				co_return;
			}

			InputEvents::deleteInputEventResponseAsync(args->eventData);
			Guild guild = Guilds::getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);
			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ args->eventData.getGuildId() , args->eventData.getAuthorId() }).get();
			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				co_return;
			}

			if (args->argumentsArray.size() == 0) {
				string msgString = "__You have the following channels enabled for gaming, on this server:__\n------\n";

				for (unsigned int x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
					string currentID = discordGuild.data.musicChannelIds[x];

					msgString += "__**Channel #" + to_string(x) + ":**__ <#" + currentID + "> \n";
				}

				msgString += "------\n";

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Music Channels Enabled:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				co_return;
			}
			if (args->argumentsArray.at(0).find("add") == string::npos && args->argumentsArray.at(0).find("remove") == string::npos && args->argumentsArray.at(0).find("purge") == string::npos && args->argumentsArray.at(0).find("view") == string::npos) {
				string msgString = "------\n**Please enter either 'add', 'remove', or 'purge' only! (!SetMusicChannel = ADDorREMOVEorPURGE)**\n------";
				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
				}
				else {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
				}
				co_return;
			}
			if (args->argumentsArray.at(0).find("add") != string::npos) {

				string channelID = channel.id;
				for (unsigned int x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
					if (channelID == discordGuild.data.musicChannelIds[x]) {
						string msgString = "------\n**That channel is already on the list of enabled channels!**\n------";
						EmbedData messageEmbed;
						messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setDescription(msgString);
						messageEmbed.setTitle("__**Already Listed:**__");
						if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
							RespondToInputEventData dataPackage(args->eventData);
							dataPackage.type = DesiredInputEventResponseType::RegularMessage;
							dataPackage.addMessageEmbed(messageEmbed);
							auto newEvent = InputEvents::respondToEvent(dataPackage);
							InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
						}
						else {
							RespondToInputEventData dataPackage(args->eventData);
							dataPackage.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
							dataPackage.addMessageEmbed(messageEmbed);
							auto newEvent = InputEvents::respondToEvent(dataPackage);
							InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
						}
						co_return;
					}
				}

				discordGuild.data.musicChannelIds.push_back(channelID);
				discordGuild.writeDataToDB();
				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription("------\n**You've succesfully added <#" + channelID + "> to your list of accepted music channels!**\n------");
				messageEmbed.setTitle("__**Music Channel Added:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				co_return;
			}
			if (args->argumentsArray.at(0).find("remove") != string::npos) {
				string channelID;
				channelID = channel.id;

				string msgString;
				bool isItPresent = false;
				for (unsigned int x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
					if (channelID == discordGuild.data.musicChannelIds[x]) {
						isItPresent = true;
						discordGuild.data.musicChannelIds.erase(discordGuild.data.musicChannelIds.begin() + x);
						discordGuild.writeDataToDB();
						msgString += "------\n**You've succesfully removed the channel <#" + channelID + "> from the list of enabled gaming channels!**\n------";
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
						RespondToInputEventData dataPackage(args->eventData);
						dataPackage.type = DesiredInputEventResponseType::RegularMessage;
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
					}
					else {
						RespondToInputEventData dataPackage(args->eventData);
						dataPackage.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
					}
					co_return;
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Music Channel Removed:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				co_return;
			}
			if (args->argumentsArray.at(0).find("purge") != string::npos) {
				string msgString;

				if (discordGuild.data.musicChannelIds.size() > 0) {
					msgString = "__You've removed the following channels from your list of 'enabled music channels:__\n------\n";

					for (unsigned int x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						string currentID = discordGuild.data.musicChannelIds[x];

						msgString += "__**Channel #" + to_string(x) + "**__<#" + currentID + "> \n";
					}

					msgString += "------\n__**The music commands will now work in ANY CHANNEL!**__";

					discordGuild.data.musicChannelIds = vector<string>();
					discordGuild.writeDataToDB();
				}
				else {
					msgString += "------\n**Sorry, but there are no channels to remove!**\n------";
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Music Channels Removed:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::RegularMessage;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					RespondToInputEventData dataPackage(args->eventData);
					dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				co_return;
			}

			co_return;
		}
	};
}
#endif