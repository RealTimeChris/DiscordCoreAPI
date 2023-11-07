Adding a Command {#adding_a_command}
============
- First, create an instance of `discord_core_api::Discord
`, and then use from it the `discord_core_api::DiscordCoreClient::registerFunction` function. Passing into it an instance of a `discord_core_api::Jsonifier::Vector` of `jsonifier::string`, which will act as the command names to be triggering the commands, a `discord_core_api::UniquePtr` containing a command function derived from the `discord_core_api::BaseFunction` class, and an instance of either `discord_core_api::CreateGlobalApplicationCommandData` or `discord_core_api::CreateGuildApplicationCommandData`.
- Note that these functions will be registered with the Discord API if any of their properties change.

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// RealTimeChris
// https://github.com/RealTimeChris

#include "Commands/CommandsList.hpp"

int32_t main() {
	DiscordCoreClientConfig clientConfig{};
	clientConfig.botToken = "YOUR_BOT_TOKEN_HERE";
	Jsonifier::Vector<ActivityData> activities{};
	ActivityData activity{};
	activity.name = "/help for my commands!";
	activity.type = ActivityType::Game;
	activities.pushBack(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk = false;
	clientConfig.presenceData.since = 0;
	clientConfig.presenceData.status = "online";
	auto ptr = makeUnique<DiscordCoreClient>(clientConfig);
	CreateGlobalApplicationCommandData createBotInfoCommandData{};
	createBotInfoCommandData.dmPermission = true;
	createBotInfoCommandData.applicationId = ptr->getBotUser().id;
	createBotInfoCommandData.type = ApplicationCommandType::Chat_Input;
	createBotInfoCommandData.defaultMemberPermissions = Permission::Use_Application_Commands;
	createBotInfoCommandData.description = "Displays info about the current bot.";
	createBotInfoCommandData.name = "botinfo";
	CreateGlobalApplicationCommandData playCommandData{};
	playCommandData.defaultMemberPermissions = Permission::Use_Application_Commands;
	playCommandData.dmPermission = false;
	playCommandData.applicationId = ptr->getBotUser().id;
	playCommandData.type = ApplicationCommandType::Chat_Input;
	playCommandData.description = "Search for and play a song.";
	playCommandData.name = "play";
	ApplicationCommandOptionData playCommandDataOptionOne;
	CreateGlobalApplicationCommandData createHelpData{};
	createHelpData.dmPermission = true;
	createHelpData.defaultMemberPermissions = Permission::Use_Application_Commands;
	createHelpData.applicationId = ptr->getBotUser().id;
	createHelpData.type = ApplicationCommandType::Chat_Input;
	createHelpData.name = "help";
	createHelpData.description = "A help command for this bot.";
	playCommandDataOptionOne.name = "songname";
	playCommandDataOptionOne.type = ApplicationCommandOptionType::jsonifier::string;
	playCommandDataOptionOne.description = "The name of the song that you would like to search.";
	playCommandDataOptionOne.required = false;
	playCommandData.options.pushBack(playCommandDataOptionOne);
	ptr->registerFunction(Jsonifier::Vector<jsonifier::string>{ "play" }, makeUnique<Play>(), playCommandData);
	ptr->registerFunction(Jsonifier::Vector<jsonifier::string>{ "botinfo" }, makeUnique<bot_info>(), createBotInfoCommandData);
	ptr->registerFunction(Jsonifier::Vector<jsonifier::string>{ "help" }, makeUnique<Help>(), createHelpData);
	ptr->runBot();
	return 0;
};

```
