Set Music Channel {#SetMusicChannel}
============
```cpp
#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

	namespace DiscordCoreAPI {
	class SetMusicChannel : public DiscordCoreAPI::BaseFunction {
	  public:
		SetMusicChannel() {
			this->commandName = "setmusicchannel";
			this->helpDescription = "Sets the channels from which you can issue music commands!";
			EmbedData msgEmbed;
			msgEmbed.setDescription(
				"------\nSimply enter /setmusichannel add in order to add the current channel.\nAlternatively enter /setmusicchannel remove to remove the "
				"current channel.\nAlso, enter /setmusicchannel view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**Set Music Channel Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<SetMusicChannel>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({args.eventData->getChannelId()}).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();
				Guild guild = Guilds::getCachedGuildAsync({args.eventData->getGuildId()}).get();
				DiscordGuild discordGuild(guild);
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({.guildMemberId = args.eventData->getAuthorId(), .guildId = args.eventData->getGuildId()}).get();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(*args, *args.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				if (args.commandData.subCommandName == "add") {
					std::string channelID = channel.id;
					for (uint32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x++) {
						if (channelID == discordGuild.data.musicChannelIds[x]) {
							std::string msgString = "------\n**That channel is already on the list of enabled channels!**\n------";
							EmbedData messageEmbed;
							messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
							messageEmbed.setColor(discordGuild.data.borderColor);
							messageEmbed.setTimeStamp(getTimeAndDate());
							messageEmbed.setDescription(msgString);
							messageEmbed.setTitle("__**Already Listed:**__");
							RespondToInputEventData dataPackage(*args.eventData);
							dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(messageEmbed);
							auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
							InputEvents::deleteInputEventResponseAsync(std::move(newEvent), 20000);
							return;
						}
					}

					discordGuild.data.musicChannelIds.push_back(channelID);
					discordGuild.writeDataToDB();
					EmbedData messageEmbed;
					messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription("------\n**You've succesfully added <#" + channelID + "> to your list of accepted music channels!**\n------");
					messageEmbed.setTitle("__**Music Channel Added:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}
				if (args.commandData.subCommandName == "remove") {
					std::string channelID;
					channelID = channel.id;

					std::string msgString;
					bool isItPresent = false;
					for (uint32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x++) {
						if (channelID == discordGuild.data.musicChannelIds[x]) {
							isItPresent = true;
							discordGuild.data.musicChannelIds.erase(discordGuild.data.musicChannelIds.begin() + x);
							discordGuild.writeDataToDB();
							msgString +=
								"------\n**You've succesfully removed the channel <#" + channelID + "> from the list of enabled music channels!**\n------";
						}
					}

					if (isItPresent == false) {
						std::string msgString2 = "------\n**That channel is not present on the list of enabled music channels!**\n------";
						EmbedData messageEmbed;
						messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setDescription(msgString2);
						messageEmbed.setTitle("__**Missing from List:**__");
						RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
						InputEvents::deleteInputEventResponseAsync(std::move(newEvent), 20000);
						return;
					}

					EmbedData messageEmbed;
					messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Music Channel Removed:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}
				if (args.commandData.subCommandName == "purge") {
					std::string msgString;

					if (discordGuild.data.musicChannelIds.size() > 0) {
						msgString = "__You've removed the following channels from your list of enabled music channels:__\n------\n";

						for (uint32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x++) {
							std::string currentID = discordGuild.data.musicChannelIds[x];

							msgString += "__**Channel #" + std::to_string(x) + "**__<#" + currentID + "> \n";
						}

						msgString += "------\n__**The music commands will now work in ANY CHANNEL!**__";

						discordGuild.data.musicChannelIds = std::vector<std::string>();
						discordGuild.writeDataToDB();
					} else {
						msgString += "------\n**Sorry, but there are no channels to remove!**\n------";
					}

					EmbedData messageEmbed;
					messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Music Channels Removed:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}
				if (args.commandData.subCommandName == "view") {
					std::string msgString = "__You have the following channels enabled for music commands, on this server:__\n------\n";

					for (uint32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x++) {
						std::string currentID = discordGuild.data.musicChannelIds[x];

						msgString += "__**Channel #" + std::to_string(x) + ":**__ <#" + currentID + "> \n";
					}

					msgString += "------\n";

					EmbedData messageEmbed;
					messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Music Channels Enabled:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage);
					return;
				}

				return;
			} catch (...) {
				reportException("SetMusicChannel::execute Error: ");
			}
		}
		virtual ~SetMusicChannel();
	};
}
```