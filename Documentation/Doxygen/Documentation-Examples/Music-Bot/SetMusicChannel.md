Set Music Channel {#SetMusicChannel}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class SetMusicChannel : public BaseFunction {
	  public:
		SetMusicChannel() {
			this->commandName = "setmusicchannel";
			this->helpDescription = "Sets the channels from which you can issue music commands!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /setmusichannel add in order to add the current channel.\nAlternatively enter /setmusicchannel "
									"remove to remove the current channel.\nAlso, enter "
									"/setmusicchannel view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**Set Music Channel Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<SetMusicChannel>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ newArgs.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ newArgs.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();
				Bool doWeHaveAdminPermission = doWeHaveAdminPermissions(newArgs, newArgs.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				if (newArgs.subCommandName == "add") {
					Uint64 channelID = channel.id;
					for (Uint32 x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.musicChannelIds[x]) {
							String msgString = "------\n**That channel is already on the list of enabled channels!**\n------";
							EmbedData messageEmbed;
							messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
							messageEmbed.setColor(discordGuild.data.borderColor);
							messageEmbed.setTimeStamp(getTimeAndDate());
							messageEmbed.setDescription(msgString);
							messageEmbed.setTitle("__**Already Listed:**__");
							RespondToInputEventData dataPackage(newArgs.eventData);
							dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(messageEmbed);
							auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
							InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
							return;
						}
					}

					discordGuild.data.musicChannelIds.emplace_back(channelID);
					discordGuild.writeDataToDB();
					EmbedData messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription("------\n**You've succesfully added <#" + std::to_string(channelID) + "> to your list of accepted music channels!**\n------");
					messageEmbed.setTitle("__**Music Channel Added:**__");
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (newArgs.subCommandName == "remove") {
					Uint64 channelID;
					channelID = channel.id;

					String msgString;
					Bool isItPresent = false;
					for (Uint32 x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.musicChannelIds[x]) {
							isItPresent = true;
							discordGuild.data.musicChannelIds.erase(discordGuild.data.musicChannelIds.begin() + x);
							discordGuild.writeDataToDB();
							msgString += "------\n**You've succesfully removed the channel <#" + std::to_string(channelID) + "> from the list of enabled music channels!**\n------";
						}
					}

					if (isItPresent == false) {
						String msgString2 = "------\n**That channel is not present on the list of enabled music channels!**\n------";
						EmbedData messageEmbed;
						messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setDescription(msgString2);
						messageEmbed.setTitle("__**Missing from List:**__");
						RespondToInputEventData dataPackage(newArgs.eventData);
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
						InputEvents::deleteInputEventResponseAsync(newEvent, 20000);
						return;
					}

					EmbedData messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Music Channel Removed:**__");
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (newArgs.subCommandName == "purge") {
					String msgString;

					if (discordGuild.data.musicChannelIds.size() > 0) {
						msgString = "__You've removed the following channels from your list of enabled music channels:__\n------\n";

						for (Uint32 x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
							Uint64 currentID = discordGuild.data.musicChannelIds[x];

							msgString += "__**Channel #" + std::to_string(x) + "**__<#" + std::to_string(currentID) + "> \n";
						}

						msgString += "------\n__**The music commands will now work in ANY CHANNEL!**__";

						discordGuild.data.musicChannelIds = std::vector<Uint64>();
						discordGuild.writeDataToDB();
					} else {
						msgString += "------\n**Sorry, but there are no channels to remove!**\n------";
					}

					EmbedData messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Music Channels Removed:**__");
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (newArgs.subCommandName == "view") {
					String msgString = "__You have the following channels enabled for music commands, on this server:__\n------\n";

					for (Uint32 x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						Uint64 currentID = discordGuild.data.musicChannelIds[x];

						msgString += "__**Channel #" + std::to_string(x) + ":**__ <#" + std::to_string(currentID) + "> \n";
					}

					msgString += "------\n";

					EmbedData messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Music Channels Enabled:**__");
					RespondToInputEventData dataPackage(newArgs.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				return;
			} catch (...) {
				reportException("SetMusicChannel::execute()");
			}
		}
		~SetMusicChannel(){};
	};
}```