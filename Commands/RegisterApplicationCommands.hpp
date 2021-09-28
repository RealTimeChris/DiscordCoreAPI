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

			InputEventData newEvent = args->eventData;

			RespondToInputEventData dataPackage(args->eventData);
			dataPackage.type = DesiredInputEventResponseType::DeferredResponse;
			if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				newEvent = InputEvents::respondToEvent(dataPackage);
			}
			/*
			DiscordCoreAPI::CreateApplicationCommandData createBotInfoCommandData;
			createBotInfoCommandData.type = ApplicationCommandType::CHAT_INPUT;
			createBotInfoCommandData.defaultPermission = true;
			createBotInfoCommandData.description = "Displays info about the current bot.";
			createBotInfoCommandData.name = "botinfo";
			ApplicationCommands::createGlobalApplicationCommandAsync(createBotInfoCommandData);
			
			DiscordCoreAPI::CreateApplicationCommandData createDisplayGuildsDataCommandData;
			createDisplayGuildsDataCommandData.type = ApplicationCommandType::CHAT_INPUT;
			createDisplayGuildsDataCommandData.defaultPermission = true;
			createDisplayGuildsDataCommandData.description = "View the list of servers that this bot is in.";
			createDisplayGuildsDataCommandData.name = "displayguildsdata";
			ApplicationCommands::createGlobalApplicationCommandAsync(createDisplayGuildsDataCommandData);

			CreateApplicationCommandData playCommandData;
			playCommandData.type = ApplicationCommandType::CHAT_INPUT;
			playCommandData.defaultPermission = true;
			playCommandData.description = "Search for and play a song.";
			playCommandData.name = "play";
			ApplicationCommandOptionData playCommandDataOptionOne;
			playCommandDataOptionOne.name = "songname";
			playCommandDataOptionOne.type = ApplicationCommandOptionType::STRING;
			playCommandDataOptionOne.description = "The name of the song that you would like to search.";
			playCommandDataOptionOne.required = false;
			playCommandData.options.push_back(playCommandDataOptionOne);
			ApplicationCommands::createGlobalApplicationCommandAsync(playCommandData);
			
			CreateApplicationCommandData RegisterApplicationCommandsCommandData;
			RegisterApplicationCommandsCommandData.type = ApplicationCommandType::CHAT_INPUT;
			RegisterApplicationCommandsCommandData.defaultPermission = true;
			RegisterApplicationCommandsCommandData.description = "Register the programmatically designated slash commands.";
			RegisterApplicationCommandsCommandData.name = "registerapplicationcommands";
			ApplicationCommands::createGlobalApplicationCommandAsync(RegisterApplicationCommandsCommandData);
			
			DiscordCoreAPI::CreateApplicationCommandData createSetGameChannelCommandData;
			createSetGameChannelCommandData.type = ApplicationCommandType::CHAT_INPUT;
			createSetGameChannelCommandData.defaultPermission = true;
			createSetGameChannelCommandData.description = "Sets the channels which you can issue music commands in.";
			createSetGameChannelCommandData.name = "setmusicchannel";
			DiscordCoreAPI::ApplicationCommandOptionData createSetGameChannelOptionOne;
			createSetGameChannelOptionOne.type = ApplicationCommandOptionType::SUB_COMMAND;
			createSetGameChannelOptionOne.name = "view";
			createSetGameChannelOptionOne.description = "View the currently enabled channels.";
			createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionOne);
			DiscordCoreAPI::ApplicationCommandOptionData createSetGameChannelOptionTwo;
			createSetGameChannelOptionTwo.type = ApplicationCommandOptionType::SUB_COMMAND;
			createSetGameChannelOptionTwo.name = "add";
			createSetGameChannelOptionTwo.description = "Add a channel to the list of enabled channels.";
			ApplicationCommandOptionData createSetGameChannelOptionTwoOne;
			createSetGameChannelOptionTwoOne.name = "add";
			createSetGameChannelOptionTwoOne.description = "Add a channel to the list of enabled channels";
			createSetGameChannelOptionTwoOne.required = true;
			createSetGameChannelOptionTwoOne.type = ApplicationCommandOptionType::STRING;
			ApplicationCommandOptionChoiceData choiceOne;
			choiceOne.name = "add";
			choiceOne.valueString = "add";
			createSetGameChannelOptionTwoOne.choices.push_back(choiceOne);
			createSetGameChannelOptionTwo.options.push_back(createSetGameChannelOptionTwoOne);
			createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionTwo);
			DiscordCoreAPI::ApplicationCommandOptionData createSetGameChannelOptionThree;
			createSetGameChannelOptionThree.type = ApplicationCommandOptionType::SUB_COMMAND;
			createSetGameChannelOptionThree.name = "remove";
			createSetGameChannelOptionThree.description = "Remove a channel from the list of enabled channels.";
			ApplicationCommandOptionData createSetGameChannelOptionThreeOne;
			createSetGameChannelOptionThreeOne.name = "remove";
			createSetGameChannelOptionThreeOne.description = "Remove a channel from the list of enabled channels";
			createSetGameChannelOptionThreeOne.required = true;
			createSetGameChannelOptionThreeOne.type = ApplicationCommandOptionType::STRING;
			ApplicationCommandOptionChoiceData choiceTwo;
			choiceTwo.name = "remove";
			choiceTwo.valueString = "remove";
			createSetGameChannelOptionThreeOne.choices.push_back(choiceTwo);
			createSetGameChannelOptionThree.options.push_back(createSetGameChannelOptionThreeOne);
			createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionThree);
			DiscordCoreAPI::ApplicationCommandOptionData createSetGameChannelOptionFour;
			createSetGameChannelOptionFour.type = ApplicationCommandOptionType::SUB_COMMAND;
			createSetGameChannelOptionFour.name = "purge";
			createSetGameChannelOptionFour.description = "Purges the list of channels for the server.";
			ApplicationCommandOptionData createSetGameChannelOptionFourOne;
			createSetGameChannelOptionFourOne.name = "purge";
			createSetGameChannelOptionFourOne.description = "Purges the list of channels for the server.";
			createSetGameChannelOptionFourOne.required = true;
			createSetGameChannelOptionFourOne.type = ApplicationCommandOptionType::STRING;
			ApplicationCommandOptionChoiceData choiceThree;
			choiceThree.name = "purge";
			choiceThree.valueString = "purge";
			createSetGameChannelOptionFourOne.choices.push_back(choiceThree);
			createSetGameChannelOptionFour.options.push_back(createSetGameChannelOptionFourOne);
			createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionFour);
			ApplicationCommands::createGlobalApplicationCommandAsync(createSetGameChannelCommandData).get();
			
			CreateApplicationCommandData createSetBorderColorCommandData;
			createSetBorderColorCommandData.type = ApplicationCommandType::CHAT_INPUT;
			createSetBorderColorCommandData.defaultPermission = true;
			createSetBorderColorCommandData.description = "Set the default color of borders.";
			createSetBorderColorCommandData.name = "setbordercolor";
			createSetBorderColorCommandData.type = ApplicationCommandType::CHAT_INPUT;
			ApplicationCommandOptionData createSetBorderColoreOptionOne;
			createSetBorderColoreOptionOne.type = ApplicationCommandOptionType::STRING;
			createSetBorderColoreOptionOne.name = "botname";
			createSetBorderColoreOptionOne.description = "Which of the bots to change the setting on.";
			createSetBorderColoreOptionOne.required = true;
			ApplicationCommandOptionChoiceData setBorderColorchoiceOne;
			setBorderColorchoiceOne.name = "musichouse";
			setBorderColorchoiceOne.valueString = "musichouse";
			createSetBorderColoreOptionOne.choices.push_back(setBorderColorchoiceOne);
			createSetBorderColorCommandData.options.push_back(createSetBorderColoreOptionOne);
			ApplicationCommandOptionData createSetBorderColorOptionTwo;
			createSetBorderColorOptionTwo.type = ApplicationCommandOptionType::STRING;
			createSetBorderColorOptionTwo.name = "hexcolorvalue";
			createSetBorderColorOptionTwo.required = true;
			createSetBorderColorOptionTwo.description = "The hex-color-value to set the borders to.";
			createSetBorderColorCommandData.options.push_back(createSetBorderColorOptionTwo);
			ApplicationCommands::createGlobalApplicationCommandAsync(createSetBorderColorCommandData).get();

			CreateApplicationCommandData createSkipData;
			createSkipData.type = ApplicationCommandType::CHAT_INPUT;
			createSkipData.name = "skip";
			createSkipData.defaultPermission = true;
			createSkipData.description = "Skip to the next song in the queue.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createSkipData).get();
			
			CreateApplicationCommandData createStopData;
			createStopData.type = ApplicationCommandType::CHAT_INPUT;
			createStopData.name = "stop";
			createStopData.defaultPermission = true;
			createStopData.description = "Stops the currently playing music.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createStopData).get();

			CreateApplicationCommandData createLoopSongData;
			createLoopSongData.type = ApplicationCommandType::CHAT_INPUT;
			createLoopSongData.name = "loopsong";
			createLoopSongData.defaultPermission = true;
			createLoopSongData.description = "Set the current song to be looped over.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createLoopSongData).get();

			CreateApplicationCommandData createLoopAllData;
			createLoopAllData.type = ApplicationCommandType::CHAT_INPUT;
			createLoopAllData.name = "loopall";
			createLoopAllData.defaultPermission = true;
			createLoopAllData.description = "Set the current playlist to be looped over.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createLoopAllData).get();
			
			CreateApplicationCommandData createSeekCommandData;
			createSeekCommandData.defaultPermission = true;
			createSeekCommandData.description = "Seek into a song.";
			createSeekCommandData.name = "seek";
			createSeekCommandData.type = ApplicationCommandType::CHAT_INPUT;
			ApplicationCommandOptionData createSeekOptionOne;
			createSeekOptionOne.type = ApplicationCommandOptionType::STRING;
			createSeekOptionOne.name = "hours";
			createSeekOptionOne.description = "The number of hours into the song to seek.";
			createSeekOptionOne.required = false;
			ApplicationCommandOptionData createSeekOptionTwo;
			createSeekOptionTwo.type = ApplicationCommandOptionType::STRING;
			createSeekOptionTwo.name = "minutes";
			createSeekOptionTwo.description = "The number of minutes into the song to seek.";
			createSeekOptionTwo.required = false;
			ApplicationCommandOptionData createSeekOptionThree;
			createSeekOptionThree.type = ApplicationCommandOptionType::STRING;
			createSeekOptionThree.name = "seconds";
			createSeekOptionThree.description = "The number of seconds into the song to seek.";
			createSeekOptionThree.required = false;
			createSeekCommandData.options.push_back(createSeekOptionOne);
			createSeekCommandData.options.push_back(createSeekOptionTwo);
			createSeekCommandData.options.push_back(createSeekOptionThree);
			ApplicationCommands::createGlobalApplicationCommandAsync(createSeekCommandData).get();
			
			CreateApplicationCommandData createClearData;
			createClearData.type = ApplicationCommandType::CHAT_INPUT;
			createClearData.name = "clear";
			createClearData.defaultPermission = true;
			createClearData.description = "Clears the current song queue.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createClearData).get();
			
			CreateApplicationCommandData createEditQueueData;
			createEditQueueData.type = ApplicationCommandType::CHAT_INPUT;
			createEditQueueData.name = "editqueue";
			createEditQueueData.defaultPermission = true;
			createEditQueueData.description = "Edit the current server's queue of songs.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createEditQueueData).get();

			CreateApplicationCommandData createDisconnectData;
			createDisconnectData.type = ApplicationCommandType::CHAT_INPUT;
			createDisconnectData.name = "disconnect";
			createDisconnectData.defaultPermission = true;
			createDisconnectData.description = "Disconnect from the current voice channel.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createDisconnectData).get();
			
			CreateApplicationCommandData createTestData;
			createTestData.type = ApplicationCommandType::CHAT_INPUT;
			createTestData.name = "test";
			createTestData.defaultPermission = true;
			createTestData.description = "Test command.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createTestData).get();
			
			CreateApplicationCommandData createPauseData;
			createPauseData.type = ApplicationCommandType::CHAT_INPUT;
			createPauseData.name = "pause";
			createPauseData.defaultPermission = true;
			createPauseData.description = "Pause the music.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createPauseData).get();
			
			CreateApplicationCommandData createQueueData;
			createQueueData.type = ApplicationCommandType::CHAT_INPUT;
			createQueueData.name = "queue";
			createQueueData.defaultPermission = true;
			createQueueData.description = "Look at the queue and possibly edit it.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createQueueData).get();

			CreateApplicationCommandData createHelpData;
			createHelpData.type = ApplicationCommandType::CHAT_INPUT;
			createHelpData.name = "help";
			createHelpData.defaultPermission = true;
			createHelpData.description = "A help command for this bot.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createHelpData).get();
			
			CreateApplicationCommandData createNpData;
			createNpData.type = ApplicationCommandType::CHAT_INPUT;
			createNpData.name = "np";
			createNpData.defaultPermission = true;
			createNpData.description = "Displays the currently playing song.";
			ApplicationCommands::createGlobalApplicationCommandAsync(createNpData).get();
			
			CreateApplicationCommandData playRNCommandData;
			playRNCommandData.type = ApplicationCommandType::CHAT_INPUT;
			playRNCommandData.defaultPermission = true;
			playRNCommandData.description = "Plays a song immediately.";
			playRNCommandData.name = "playrn";
			ApplicationCommandOptionData playRNCommandDataOptionOne;
			playRNCommandDataOptionOne.name = "songname";
			playRNCommandDataOptionOne.type = ApplicationCommandOptionType::STRING;
			playRNCommandDataOptionOne.description = "The name of the song that you would like to search.";
			playRNCommandDataOptionOne.required = false;
			playRNCommandData.options.push_back(playRNCommandDataOptionOne);
			ApplicationCommands::createGlobalApplicationCommandAsync(playRNCommandData);
			*/
			CreateApplicationCommandData playQCommandData;
			playQCommandData.type = ApplicationCommandType::CHAT_INPUT;
			playQCommandData.defaultPermission = true;
			playQCommandData.description = "Plays a song from the queue immediately.";
			playQCommandData.name = "playq";
			ApplicationCommandOptionData playQCommandDataOptionOne;
			playQCommandDataOptionOne.name = "tracknumber";
			playQCommandDataOptionOne.type = ApplicationCommandOptionType::INTEGER;
			playQCommandDataOptionOne.description = "The number of the track in the current queue.";
			playQCommandDataOptionOne.required = true;
			playQCommandData.options.push_back(playQCommandDataOptionOne);
			ApplicationCommands::createGlobalApplicationCommandAsync(playQCommandData);

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("------\nNicely done, you've registered some commands!\n------");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Register Application Commands Complete:**__");
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				RespondToInputEventData responseData(args->eventData);
				responseData.type = DesiredInputEventResponseType::RegularMessage;
				responseData.addMessageEmbed(msgEmbed);
				InputEventData  event01 = InputEvents::respondToEvent(responseData);
			}
			else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				RespondToInputEventData responseData(args->eventData);
				responseData.type = DesiredInputEventResponseType::InteractionResponseEdit;
				responseData.addMessageEmbed(msgEmbed);
				auto event = InputEvents::respondToEvent(responseData);
			}
			co_return;
			
		}
	};
}
#endif