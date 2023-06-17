// main.cpp - Main entry point.
//// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef INSTRUCTION_SET_TYPE_AVX2
	#define INSTRUCTION_SET_TYPE_AVX2
#endif

#include "Commands/BotInfo.hpp"

int32_t main() {
	std::string botToken = "";
	DiscordCoreAPI::ShardingOptions shardOptions{};
	shardOptions.numberOfShardsForThisProcess = 1;
	shardOptions.startingShard = 0;
	shardOptions.totalNumberOfShards = 1;
	DiscordCoreAPI::LoggingOptions logOptions{};
	logOptions.logGeneralErrorMessages = true;
	logOptions.logHttpsErrorMessages = true;
	logOptions.logWebSocketErrorMessages = true;
	logOptions.logHttpsSuccessMessages = true;
	logOptions.logGeneralSuccessMessages = true;
	logOptions.logWebSocketSuccessMessages = true;
	DiscordCoreAPI::DiscordCoreClientConfig clientConfig{};
	//clientConfig.connectionAddress = "127.0.0.1";
	clientConfig.botToken = botToken;
	clientConfig.logOptions = logOptions;
	clientConfig.shardOptions = shardOptions;
	clientConfig.cacheOptions.cacheChannels = true;
	clientConfig.cacheOptions.cacheGuilds = true;
	clientConfig.cacheOptions.cacheUsers = true;
	clientConfig.cacheOptions.cacheRoles = true;
	std::vector<DiscordCoreAPI::ActivityData> activities{};
	DiscordCoreAPI::ActivityData activity{};
	activity.name = "/help for my commands!";
	activity.type = DiscordCoreAPI::ActivityType::Game;
	activities.push_back(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk = false;
	clientConfig.presenceData.status = "online";
	clientConfig.textFormat = DiscordCoreAPI::TextFormat::Etf;
	clientConfig.presenceData.since = 0;
	auto the = DiscordCoreAPI::makeUnique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
	DiscordCoreAPI::CreateGlobalApplicationCommandData createBotInfoCommandData{};
	createBotInfoCommandData.dmPermission = true;
	createBotInfoCommandData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
	createBotInfoCommandData.defaultMemberPermissions = DiscordCoreAPI::Permission::Use_Application_Commands;
	createBotInfoCommandData.description = "Displays info about the current bot.";
	createBotInfoCommandData.name = "botinfo";
	the->registerFunction(std::vector<std::string>{ "botinfo" }, DiscordCoreAPI::makeUnique<DiscordCoreAPI::BotInfo>(), createBotInfoCommandData);
	the->runBot();
	return 0;
};