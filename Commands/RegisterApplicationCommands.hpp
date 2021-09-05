// RegisterApplicationCommands.hpp - Registers the slash commands of this bot.
// May 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REGISTER_APPLICATION_COMMANDS_
#define _REGISTER_APPLICATION_COMMANDS_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class RegisterApplicationCommands :public BaseFunction {
	public:
		RegisterApplicationCommands() {
			this->commandName = "registerapplicationcommands";
			this->helpDescription = "Register some application commands.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !RegisterApplicationCommands or /RegisterApplicationCommandsn------");
			msgEmbed.setTitle("__**Register Slash Commands Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		RegisterApplicationCommands* create() {
			return new RegisterApplicationCommands;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			InputEvents::deleteInputEventResponseAsync(args->eventData);
			CreateDeferredInteractionResponseData dataPackage(args->eventData);
			InputEventData newEvent = args->eventData;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				newEvent = InputEvents::respondToEvent(dataPackage);
			}
			/*
			CreateApplicationCommandData RegisterApplicationCommandsCommandData;
			RegisterApplicationCommandsCommandData.type = ApplicationCommandType::CHAT_INPUT;
			RegisterApplicationCommandsCommandData.defaultPermission = true;
			RegisterApplicationCommandsCommandData.description = "Register the programmatically designated slash commands.";
			RegisterApplicationCommandsCommandData.name = "registerapplicationcommands";
			ApplicationCommands::createGlobalApplicationCommandAsync(RegisterApplicationCommandsCommandData);

			CreateApplicationCommandData createTestData;
			createTestData.type = ApplicationCommandType::CHAT_INPUT;
			createTestData.name = "test";
			createTestData.defaultPermission = true;
			createTestData.description = "Test command.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createTestData).get();
			*/

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("------\nNicely done, you've registered some commands!\n------");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Register Slash Commands Complete:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData responseData(newEvent);
				responseData.addMessageEmbed(msgEmbed);
				InputEventData  event01 = InputEvents::respondToEvent(responseData);
			}
			else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				EditInteractionResponseData responseData(newEvent);
				responseData.addMessageEmbed(msgEmbed);
				auto event = InputEvents::respondToEvent(responseData);
			}
			co_return;

		}
	};
}
#endif