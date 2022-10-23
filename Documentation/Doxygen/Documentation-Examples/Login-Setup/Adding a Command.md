Adding a Command {#addingcommand}
============
- First, create an instance of `DiscordCoreAPI::DiscordCoreClient`, and then use from it the `DiscordCoreAPI::DiscordCoreClient::registerFunction` function. Passing into it an instance of a `std::vector` of `std::string`, which will act as the command names to be triggering the commands, a `std::unique_ptr` containing a command function derived from the `DiscordCoreAPI::BaseFunction` class, and an instance of either `DiscordCoreAPI::CreateGlobalApplicationCommandData` or `DiscordCoreAPI::CreateGuildApplicationCommandData`.
- Note that these functions will be registered with the Discord API if any of their properties change.

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// RealTimeChris
// https://github.com/RealTimeChris

#include "Commands/CommandsList.hpp"

int32_t main() {
	DiscordCoreAPI::DiscordCoreClientConfig clientConfig{};
	clientConfig.botToken = "YOUR_BOT_TOKEN_HERE";
	std::vector<DiscordCoreAPI::ActivityData> activities{};
	DiscordCoreAPI::ActivityData activity{};
	activity.name = "/help for my commands!";
	activity.type = DiscordCoreAPI::ActivityType::Game;
	activities.push_back(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk = false;
	clientConfig.presenceData.since = 0;
	clientConfig.presenceData.status = "online";
	auto ptr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
	DiscordCoreAPI::CreateGlobalApplicationCommandData createBotInfoCommandData{};
	createBotInfoCommandData.dmPermission = true;
	createBotInfoCommandData.applicationId = ptr->getBotUser().id;
	createBotInfoCommandData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
	createBotInfoCommandData.defaultMemberPermissions = DiscordCoreAPI::Permission::Use_Application_Commands;
	createBotInfoCommandData.description = "Displays info about the current bot.";
	createBotInfoCommandData.name = "botinfo";
	DiscordCoreAPI::CreateGlobalApplicationCommandData playCommandData{};
	playCommandData.defaultMemberPermissions = DiscordCoreAPI::Permission::Use_Application_Commands;
	playCommandData.dmPermission = false;
	playCommandData.applicationId = ptr->getBotUser().id;
	playCommandData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
	playCommandData.description = "Search for and play a song.";
	playCommandData.name = "play";
	DiscordCoreAPI::ApplicationCommandOptionData playCommandDataOptionOne;
	DiscordCoreAPI::CreateGlobalApplicationCommandData createHelpData{};
	createHelpData.dmPermission = true;
	createHelpData.defaultMemberPermissions = DiscordCoreAPI::Permission::Use_Application_Commands;
	createHelpData.applicationId = ptr->getBotUser().id;
	createHelpData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
	createHelpData.name = "help";
	createHelpData.description = "A help command for this bot.";
	playCommandDataOptionOne.name = "songname";
	playCommandDataOptionOne.type = DiscordCoreAPI::ApplicationCommandOptionType::std::string;
	playCommandDataOptionOne.description = "The name of the song that you would like to search.";
	playCommandDataOptionOne.required = false;
	playCommandData.options.push_back(playCommandDataOptionOne);
	ptr->registerFunction(std::vector<std::string>{ "play" }, std::make_unique<DiscordCoreAPI::Play>(), playCommandData);
	ptr->registerFunction(std::vector<std::string>{ "botinfo" }, std::make_unique<DiscordCoreAPI::BotInfo>(), createBotInfoCommandData);
	ptr->registerFunction(std::vector<std::string>{ "help" }, std::make_unique<DiscordCoreAPI::Help>(), createHelpData);
	ptr->runBot();
	return 0;
};

```
