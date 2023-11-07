Set Music Channel {#SetMusicChannel}
============
```cpp
#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class set_music_channel : public base_function {
	  public:
		set_music_channel() {
			commandName = "setmusicchannel";
			helpDescription = "sets the channels from which you can issue music commands!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /setmusichannel add in order to add the current channel.\nAlternatively enter /setmusicchannel "
									"remove to remove the current channel.\nAlso, enter "
									"/setmusicchannel view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**set music channel usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<set_music_channel>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ newArgs.eventData.getChannelId() }).get();

				guild_data guild_data = guilds::getCachedGuild({ newArgs.eventData.getGuildId() }).get();
				discord_guild discordGuild(guild_data);
				guild_member_data guildMember =
					guild_members::getCachedGuildMember({ .guildMemberId = newArgs.eventData.getAuthorId(), .guildId = newArgs.eventData.getGuildId() }).get();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(newArgs, newArgs.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				if (newArgs.subCommandName == "add") {
					uint64_t channelID = channel.id;
					for (int32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.musicChannelIds[x]) {
							jsonifier::string msgString = "------\n**that channel is already on the list of enabled channels!**\n------";
							embed_data messageEmbed;
							messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
							messageEmbed.setColor(discordGuild.data.borderColor);
							messageEmbed.setTimeStamp(getTimeAndDate());
							messageEmbed.setDescription(msgString);
							messageEmbed.setTitle("__**already listed:**__");
							respond_to_input_event_data dataPackage(newArgs.eventData);
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(messageEmbed);
							auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
							input_events::deleteInputEventResponseAsync(const newEvent, 20000);
							return;
						}
					}

					discordGuild.data.musicChannelIds.emplace_back(channelID);
					discordGuild.writeDataToDB();
					embed_data messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription("------\n**you've succesfully added <#" + jsonifier::toString(channelID) + "> to your list of accepted music channels!**\n------");
					messageEmbed.setTitle("__**music channel added:**__");
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					return;
				}
				if (newArgs.subCommandName == "remove") {
					uint64_t channelID;
					channelID = channel.id;

					jsonifier::string msgString;
					bool isItPresent = false;
					for (int32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.musicChannelIds[x]) {
							isItPresent = true;
							discordGuild.data.musicChannelIds.erase(discordGuild.data.musicChannelIds.begin() + x);
							discordGuild.writeDataToDB();
							msgString += "------\n**you've succesfully removed the channel <#" + jsonifier::toString(channelID) + "> from the list of enabled music channels!**\n------";
						}
					}

					if (isItPresent == false) {
						jsonifier::string msgString2 = "------\n**that channel is not present on the list of enabled music channels!**\n------";
						embed_data messageEmbed;
						messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setDescription(msgString2);
						messageEmbed.setTitle("__**missing from list:**__");
						respond_to_input_event_data dataPackage(newArgs.eventData);
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
						input_events::deleteInputEventResponseAsync(const newEvent, 20000);
						return;
					}

					embed_data messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**music channel removed:**__");
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					return;
				}
				if (newArgs.subCommandName == "purge") {
					jsonifier::string msgString;

					if (discordGuild.data.musicChannelIds.size() > 0) {
						msgString = "__You've removed the following channels from your list of enabled music channels:__\n------\n";

						for (int32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
							uint64_t currentID = discordGuild.data.musicChannelIds[x];

							msgString += "__**channel #" + jsonifier::toString(x) + "**__<#" + jsonifier::toString(currentID) + "> \n";
						}

						msgString += "------\n__**the music commands will now work in any channel!**__";

						discordGuild.data.musicChannelIds = jsonifier::vector<uint64_t>();
						discordGuild.writeDataToDB();
					} else {
						msgString += "------\n**sorry, but there are no channels to remove!**\n------";
					}

					embed_data messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**music channels removed:**__");
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					return;
				}
				if (newArgs.subCommandName == "view") {
					jsonifier::string msgString = "__You have the following channels enabled for music commands, on this server:__\n------\n";

					for (int32_t x = 0; x < discordGuild.data.musicChannelIds.size(); x += 1) {
						uint64_t currentID = discordGuild.data.musicChannelIds[x];

						msgString += "__**channel #" + jsonifier::toString(x) + ":**__ <#" + jsonifier::toString(currentID) + "> \n";
					}

					msgString += "------\n";

					embed_data messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**music channels enabled:**__");
					respond_to_input_event_data dataPackage(newArgs.eventData);
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = input_events::respondToInputEventAsync(const dataPackage).get();
					return;
				}

				return;
			} catch (...) {
				reportException("set_music_channel::execute()");
			}
		}
		~set_music_channel(){};
	};
}```