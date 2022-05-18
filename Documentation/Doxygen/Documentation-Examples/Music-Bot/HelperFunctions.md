Helper Functions {#HelperFunctions}
============
```cpp
#pragma once

	namespace DiscordCoreAPI {

	bool areWeInADM(InputEventData eventData, Channel channel, bool displayResponse = true) {
		auto currentChannelType = channel.type;
		if (currentChannelType == ChannelType::Dm) {
			if (displayResponse) {
				std::string msgString = "------\n**Sorry, but we can't do that in a direct message!**\n------";
				std::unique_ptr<EmbedData> msgEmbed(new EmbedData());
				msgEmbed->setAuthor(eventData.getMessageData().interaction.user.username, eventData.getMessageData().author.avatar);
				msgEmbed->setColor("FEFEFE");
				msgEmbed->setDescription(msgString);
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**Direct Message Issue:**__");
				std::unique_ptr<RespondToInputEventData> responseData(new RespondToInputEventData(eventData));
				responseData->setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				responseData->addMessageEmbed(*msgEmbed);
				auto event01 = InputEvents::respondToInputEventAsync(*responseData);
			}
			return true;
		}
		return false;
	}

	bool checkIfAllowedPlayingInChannel(InputEventData eventData, DiscordGuild discordGuild) {
		bool isItFound = true;
		if (discordGuild.data.musicChannelIds.size() > 0) {
			isItFound = false;
			std::string msgString = "------\n**Sorry, but please do that in one of the following channels:**\n------\n";
			std::unique_ptr<EmbedData> msgEmbed(new EmbedData());
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
				msgEmbed->setTitle("__**Permissions Issue:**__");
				std::unique_ptr<RespondToInputEventData> replyMessageData(new RespondToInputEventData(eventData));
				replyMessageData->addMessageEmbed(*msgEmbed);
				;
				replyMessageData->setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				InputEvents::respondToInputEventAsync(*replyMessageData);
			}
		}
		return isItFound;
	}

	bool checkIfWeHaveControl(InputEventData eventData, DiscordGuild guildData, GuildMember guildMember) {
		if (guildData.data.djRoleId == "") {
			return true;
		}
		bool doWeHaveControl = false;
		DiscordGuildMember guildMemberData(guildMember);

		auto myRoles = Roles::getGuildMemberRolesAsync({.guildMember = guildMember, .guildId = guildData.data.guildId}).get();

		for (auto value: myRoles) {
			if (value.id == guildData.data.djRoleId) {
				doWeHaveControl = true;
			}
		}

		if (!doWeHaveControl) {
			std::string msgString = "------\n**Sorry, but you lack the permissions to do that!**\n------";
			std::unique_ptr<EmbedData> msgEmbed(new EmbedData());
			msgEmbed->setAuthor(guildMember.user.username, guildMember.user.avatar);
			msgEmbed->setDescription(msgString);
			msgEmbed->setColor(guildData.data.borderColor);
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("Permissions Issue");
			std::unique_ptr<RespondToInputEventData> dataPackage(new RespondToInputEventData(eventData));
			dataPackage->addMessageEmbed(*msgEmbed);
			dataPackage->setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
			InputEvents::respondToInputEventAsync(*dataPackage);
		}
		return doWeHaveControl;
	}

	bool checkForBotCommanderStatus(GuildMember guildMember, DiscordUser discordUser) {
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

	bool doWeHaveAdminPermissions(BaseFunctionArguments args, InputEventData eventData, DiscordGuild discordGuild, Channel channel, GuildMember guildMember,
								  bool displayResponse = true) {
		bool doWeHaveAdmin = guildMember.permissions.checkForPermission(guildMember, channel, Permission::Administrator);

		if (doWeHaveAdmin) {
			return true;
		}

		bool areWeACommander =
			checkForBotCommanderStatus(guildMember, DiscordUser(args.discordCoreClient->getBotUser().username, args.discordCoreClient->getBotUser().id));

		if (areWeACommander) {
			return true;
		}

		if (displayResponse) {
			std::string msgString = "------\n**Sorry, but you don't have the permissions required for that!**\n------";
			std::unique_ptr<EmbedData> msgEmbed(new EmbedData());
			msgEmbed->setAuthor(guildMember.user.username, guildMember.user.avatar);
			msgEmbed->setColor(discordGuild.data.borderColor);
			msgEmbed->setDescription(msgString);
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Permissions Issue:**__");
			std::unique_ptr<RespondToInputEventData> dataPackage(new RespondToInputEventData(eventData));
			dataPackage->addMessageEmbed(*msgEmbed);
			dataPackage->setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
			InputEvents::respondToInputEventAsync(*dataPackage);
		}
		return false;
	}

	float applyAsymptoticTransform(float inputModValue, float horizontalStretch, float ceiling) {
		float finalModValue = 0;
		float newInputModValue = inputModValue;
		if (newInputModValue == 0) {
			newInputModValue += 1;
		}
		if (newInputModValue <= 0) {
			float newInputValue = newInputModValue * -1;

			finalModValue =
				-1 * (float)trunc((ceiling * pow(newInputValue, 3))/((pow(newInputValue, 3) + (int64_t)horizontalStretch * (int64_t)newInputValue)));
			return finalModValue;
		}

		finalModValue =
			(float)trunc((ceiling * pow(newInputModValue, 3))/((pow(newInputModValue, 3) + (int64_t)horizontalStretch * (int64_t)newInputModValue)));

		return finalModValue;
	}
}
```