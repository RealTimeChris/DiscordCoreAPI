Creating a Global Application Command {#create_global_command}
============
- Execute the `discord_core_api::application_commands::createGlobalApplicationCommandAsync()`, while passing in a data structure of type `discord_core_api::create_global_application_command_data` (important #1: notes on which kind of types to set can be found [here](https://discord.com/developers/docs/interactions/application-commands#subcommands-and-subcommand-groups).), with a return value of type `auto` or `discord_core_api::application_command_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
// register_application_commands.hpp- registers the slash commands of this bot.
// may 27, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class register_application_commands : public base_function {
	  public:
		register_application_commands() {
			commandName = "registerapplicationcommands";
			helpDescription = "register some application commands.";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /register_application_commandsn------");
			msgEmbed.setTitle("__**register slash commands usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<register_application_commands>();
		}

		virtual void execute(base_function_arguments& argsNew) {
			try {
				channel channel = discord_core_api::channels::getCachedChannel({ .channelId = argsNew.eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(argsNew.eventData, channel);

				if (areWeInADm) {
					return;
				}

				input_events::deleteInputEventResponseAsync(const argsNew.eventData).get();

				respond_to_input_event_data& dataPackage(argsNew.eventData);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				auto newEvent = input_events::respondToInputEventAsync(const& dataPackage).get();
				
				create_global_application_command_data registerApplicationCommandsCommandData;
				registerApplicationCommandsCommandData.dmPermission = true;
				registerApplicationCommandsCommandData.applicationId = discord_core_client::getInstance()->getBotUser().id;
				registerApplicationCommandsCommandData.type = application_command_type::Chat_Input;
				registerApplicationCommandsCommandData.description = "register the programmatically designated slash commands.";
				registerApplicationCommandsCommandData.name = "registerapplicationcommands";
				application_commands::createGlobalApplicationCommandAsync(const registerApplicationCommandsCommandData);

				create_global_application_command_data createTestData;
				createTestData.dmPermission = true;
				createTestData.applicationId = discord_core_client::getInstance()->getBotUser().id;
				createTestData.type = application_command_type::Chat_Input;
				createTestData.name = "test";
				createTestData.description = "test command.";
				application_commands::createGlobalApplicationCommandAsync(const createTestData).get();

				embed_data msgEmbed;
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription("------\nNicely done, you've registered some commands!\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**register application commands complete:**__");
				respond_to_input_event_data responseData(newEvent);
				responseData.setResponseType(input_event_response_type::Edit_Interaction_Response);
				responseData.addMessageEmbed(msgEmbed);
				auto event = input_events::respondToInputEventAsync(const responseData).get();
				return;
			} catch (...) {
				reportException("register_application_commands::execute()");
			}
		}
		virtual ~register_application_commands(){};
	};
}// namespace discord_core_api

```