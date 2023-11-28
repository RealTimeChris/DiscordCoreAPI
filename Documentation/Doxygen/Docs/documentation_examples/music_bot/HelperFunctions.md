Helper Functions {#HelperFunctions}
============
```cpp
#pragma once

	namespace discord_core_api {

	bool areWeInADM(input_event_data eventData, channel channel, bool displayResponse = true) {
		auto currentChannelType = channel.type;
		if (currentChannelType == channel_type::dm) {
			if (displayResponse) {
				jsonifier::string msgString = "------\n**sorry, but we can't do that in a direct message!**\n------";
				unique_ptr<embed_data> msgEmbed(makeUnique<embed_data>());
				msgEmbed->setAuthor(eventData.getMessageData().interaction.user.userName, eventData.getMessageData().author.avatar);
				msgEmbed->setColor("fefefe");
				msgEmbed->setDescription(msgString);
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**direct message issue:**__");
				unique_ptr<respond_to_input_event_data> responseData(makeUnique<respond_to_input_event_data>(eventData));
				responseData->setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				responseData->addMessageEmbed(*msgEmbed);
				auto event01 = input_events::respondToInputEventAsync(*responseData);
			}
			return true;
		}
		return false;
	}

	bool checkIfAllowedPlayingInChannel(input_event_data eventData, discord_guild discordGuild) {
		bool isItFound = true;
		if (discordGuild.data.musicChannelIds.size() > 0) {
			isItFound = false;
			jsonifier::string msgString = "------\n**sorry, but please do that in one of the following channels:**\n------\n";
			unique_ptr<embed_data> msgEmbed(makeUnique<embed_data>());
			for (auto& value: discordGuild.data.musicChannelIds) {
				if (eventData.getChannelId() == value) {
					isItFound = true;
					break;
				} else {
					msgString += "<#" + value + ">\n";
				}
			}
			msgString += "------";
			if (isItFound == false) {
				msgEmbed->setAuthor(eventData.getUserName(), eventData.getAvatarUrl());
				msgEmbed->setColor(discordGuild.data.borderColor);
				msgEmbed->setDescription(msgString);
				msgEmbed->setTitle("__**permissions issue:**__");
				unique_ptr<respond_to_input_event_data> replyMessageData(makeUnique<respond_to_input_event_data>(eventData));
				replyMessageData->addMessageEmbed(*msgEmbed);
				;
				replyMessageData->setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				input_events::respondToInputEventAsync(*replyMessageData);
			}
		}
		return isItFound;
	}

	bool checkIfWeHaveControl(input_event_data eventData, discord_guild guildData, guild_member_data guildMember) {
		if (guildData.data.djRoleId == 0) {
			return true;
		}
		bool doWeHaveControl = false;
		discord_guild_member guildMemberData(guildMember);

		auto myRoles = roles::getGuildMemberRolesAsync({.guildMember = guildMember, .guildId = guildData.data.guildId}).get();

		for (const auto& value: myRoles) {
			if (value.id == guildData.data.djRoleId) {
				doWeHaveControl = true;
			}
		}

		if (!doWeHaveControl) {
			jsonifier::string msgString = "------\n**sorry, but you lack the permissions to do that!**\n------";
			unique_ptr<embed_data> msgEmbed(makeUnique<embed_data>());
			msgEmbed->setAuthor(guildMember.user.userName, guildMember.user.avatar);
			msgEmbed->setDescription(msgString);
			msgEmbed->setColor(guildData.data.borderColor);
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("permissions issue");
			unique_ptr<respond_to_input_event_data> dataPackage(makeUnique<respond_to_input_event_data>(eventData));
			dataPackage->addMessageEmbed(*msgEmbed);
			dataPackage->setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
			input_events::respondToInputEventAsync(*dataPackage);
		}
		return doWeHaveControl;
	}

	bool checkForBotCommanderStatus(guild_member_data guildMember, discord_user discordUser) {
		bool areWeACommander;
		for (auto& value: discordUser.data.botCommanders) {
			if (guildMember.user.id == value) {
				areWeACommander = true;
				return areWeACommander;
				break;
			}
		}
		return false;
	}

	bool doWeHaveAdminPermissions(base_function_arguments args, input_event_data eventData, discord_guild discordGuild, channel channel, guild_member_data guildMember,
								  bool displayResponse = true) {
		bool doWeHaveAdmin = guildMember.permissions.checkForPermission(guildMember, channel, permission_types::administrator);

		if (doWeHaveAdmin) {
			return true;
		}

		bool areWeACommander =
			checkForBotCommanderStatus(guildMember, discord_user(discord_core_client::getInstance()->getBotUser().userName, discord_core_client::getInstance()->getBotUser().id));

		if (areWeACommander) {
			return true;
		}

		if (displayResponse) {
			jsonifier::string msgString = "------\n**sorry, but you don't have the permissions required for that!**\n------";
			unique_ptr<embed_data> msgEmbed(makeUnique<embed_data>());
			msgEmbed->setAuthor(guildMember.user.userName, guildMember.user.avatar);
			msgEmbed->setColor(discordGuild.data.borderColor);
			msgEmbed->setDescription(msgString);
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**permissions issue:**__");
			unique_ptr<respond_to_input_event_data> dataPackage(makeUnique<respond_to_input_event_data>(eventData));
			dataPackage->addMessageEmbed(*msgEmbed);
			dataPackage->setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
			input_events::respondToInputEventAsync(*dataPackage);
		}
		return false;
	}

	double applyAsymptoticTransform(double inputModValue, double horizontalStretch, double ceiling) {
		double finalModValue = 0;
		double newInputModValue = inputModValue;
		if (newInputModValue == 0) {
			newInputModValue++;
		}
		if (newInputModValue <= 0) {
			double newInputValue = newInputModValue * -1;

			finalModValue =
				-1 * (double)trunc((ceiling * pow(newInputValue, 3))/((pow(newInputValue, 3) + (int64_t)horizontalStretch * (int64_t)newInputValue)));
			return finalModValue;
		}

		finalModValue =
			(double)trunc((ceiling * pow(newInputModValue, 3))/((pow(newInputModValue, 3) + (int64_t)horizontalStretch * (int64_t)newInputModValue)));

		return finalModValue;
	}
}
```