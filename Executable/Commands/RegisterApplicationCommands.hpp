/// RegisterApplicationCommands.hpp - Registers the slash commands of this bot.
/// May 27, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#ifndef REGISTER_APPLICATION_COMMANDS
#define REGISTER_APPLICATION_COMMANDS

#include "../../Include/Index.hpp"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class RegisterApplicationCommands : public BaseFunction {
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

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<RegisterApplicationCommands>();
		}

		virtual void execute(std::unique_ptr<BaseFunctionArguments> args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

				bool areWeInADm = areWeInADM(args->eventData, channel);

				if (areWeInADm) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(args->eventData).get();

				InputEventData newEvent = args->eventData;

				RespondToInputEventData dataPackage(args->eventData);
				dataPackage.type = InputEventResponseType::Deferred_Response;
				if (args->eventData.eventType == InputEventType::Application_Command_Interaction) {
					newEvent = InputEvents::respondToEvent(dataPackage);
				}

				Guild guild = Guilds::getCachedGuildAsync({ args->eventData.getGuildId() }).get();
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId() ,.guildId = args->eventData.getGuildId() }).get();

				CreateGlobalApplicationCommandData testCommandData{};
				testCommandData.applicationId = args->discordCoreClient->getBotUser().id;
				testCommandData.type = ApplicationCommandType::Chat_Input;
				testCommandData.defaultPermission = true;
				testCommandData.description = "Reports a test message!";
				testCommandData.name = "test";
				ApplicationCommandOptionData testCommandDataOptionOne{};
				testCommandDataOptionOne.name = "outputtext";
				testCommandDataOptionOne.type = ApplicationCommandOptionType::String;
				testCommandDataOptionOne.description = "The test message you would like to have relayed.";
				testCommandDataOptionOne.required = true;
				testCommandData.options.push_back(testCommandDataOptionOne);
				ApplicationCommands::createGlobalApplicationCommandAsync(testCommandData);

				CreateGlobalApplicationCommandData RegisterApplicationCommandsCommandData{};
				RegisterApplicationCommandsCommandData.applicationId = args->discordCoreClient->getBotUser().id;
				RegisterApplicationCommandsCommandData.type = ApplicationCommandType::Chat_Input;
				RegisterApplicationCommandsCommandData.defaultPermission = true;
				RegisterApplicationCommandsCommandData.description = "Register the programmatically designated slash commands.";
				RegisterApplicationCommandsCommandData.name = "registerapplicationcommands";
				ApplicationCommands::createGlobalApplicationCommandAsync(RegisterApplicationCommandsCommandData);

				CreateGlobalApplicationCommandData playRNCommandData{};
				playRNCommandData.applicationId = args->discordCoreClient->getBotUser().id;
				playRNCommandData.type = ApplicationCommandType::Chat_Input;
				playRNCommandData.defaultPermission = true;
				playRNCommandData.description = "Plays a song immediately.";
				playRNCommandData.name = "playrn";
				ApplicationCommandOptionData playRNCommandDataOptionOne{};
				playRNCommandDataOptionOne.name = "songname";
				playRNCommandDataOptionOne.type = ApplicationCommandOptionType::String;
				playRNCommandDataOptionOne.description = "The name of the song that you would like to search.";
				playRNCommandDataOptionOne.required = false;
				playRNCommandData.options.push_back(playRNCommandDataOptionOne);
				ApplicationCommands::createGlobalApplicationCommandAsync(playRNCommandData);

				EmbedData msgEmbed{};
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarUrl());
				msgEmbed.setColor("FeFeFe");
				msgEmbed.setDescription("------\nNicely done, you've registered some commands!\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Register Application Commands Complete:**__");
				if (args->eventData.eventType == InputEventType::Regular_Message) {
					RespondToInputEventData responseData(args->eventData);
					responseData.type = InputEventResponseType::Regular_Message;
					responseData.addMessageEmbed(msgEmbed);
					InputEventData  event01 = InputEvents::respondToEvent(responseData);
				}
				else if (args->eventData.eventType == InputEventType::Application_Command_Interaction) {
					RespondToInputEventData responseData(args->eventData);
					responseData.type = InputEventResponseType::Interaction_Response_Edit;
					responseData.addMessageEmbed(msgEmbed);
					auto event = InputEvents::respondToEvent(responseData);
				}
				return;
			}
			catch (...) {
				reportException("RegisterApplicationCommands::executeAsync Error: ");
			}


		}
		virtual ~RegisterApplicationCommands() = default;
	};
}
#endif