Creating a Global Application Command {#createglobalcommand}
============
- Execute the `ApplicationCommands::createGlobalApplicationCommandAsync()`, while passing in a data structure of type `CreateGlobalApplicationCommandData` (IMPORTANT #1: Notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/application-commands#subcommands-and-subcommand-groups).), with a return value of type `auto` or `ApplicationCommand`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// RegisterApplicationCommands.hpp - Registers the slash commands of this bot.
// May 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class RegisterApplicationCommands : public BaseFunction {
	  public:
		RegisterApplicationCommands() {
			commandName = "registerapplicationcommands";
			helpDescription = "Register some application commands.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /RegisterApplicationCommandsn------");
			msgEmbed.setTitle("__**Register Slash Commands Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<RegisterApplicationCommands>();
		}

		virtual void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannel({ .channelId = argsNew.eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(argsNew.eventData, channel);

				if (areWeInADm) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(argsNew.eventData).get();

				RespondToInputEventData dataPackage(argsNew.eventData);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				
				CreateGlobalApplicationCommandData registerApplicationCommandsCommandData;
				registerApplicationCommandsCommandData.dmPermission = true;
				registerApplicationCommandsCommandData.applicationId = argsNew.discordCoreClient->getBotUser().id;
				registerApplicationCommandsCommandData.type = ApplicationCommandType::Chat_Input;
				registerApplicationCommandsCommandData.description = "Register the programmatically designated slash commands.";
				registerApplicationCommandsCommandData.name = "registerapplicationcommands";
				ApplicationCommands::createGlobalApplicationCommandAsync(registerApplicationCommandsCommandData);

				CreateGlobalApplicationCommandData createTestData;
				createTestData.dmPermission = true;
				createTestData.applicationId = argsNew.discordCoreClient->getBotUser().id;
				createTestData.type = ApplicationCommandType::Chat_Input;
				createTestData.name = "test";
				createTestData.description = "Test command.";
				ApplicationCommands::createGlobalApplicationCommandAsync(createTestData).get();

				EmbedData msgEmbed;
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription("------\nNicely done, you've registered some commands!\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Register Application Commands Complete:**__");
				RespondToInputEventData responseData(newEvent);
				responseData.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				responseData.addMessageEmbed(msgEmbed);
				auto event = InputEvents::respondToInputEventAsync(responseData).get();
				return;
			} catch (...) {
				reportException("RegisterApplicationCommands::execute()");
			}
		}
		virtual ~RegisterApplicationCommands(){};
	};
}// namespace DiscordCoreAPI

```