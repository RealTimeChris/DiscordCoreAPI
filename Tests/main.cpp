// main.cpp - Main entry point.
//// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "Commands/BotInfo.hpp"

namespace DiscordCoreAPI {
	extern std::atomic_bool doWeQuit;
}

void onBoot(DiscordCoreAPI::DiscordCoreClient* client) {
	DiscordCoreAPI::doWeQuit.store(true, std::memory_order_release);
}

int32_t main() {
	std::string botToken = "";
	Jsonifier::Vector<DiscordCoreAPI::RepeatedFunctionData> functionVector{};
	functionVector.reserve(5);
	DiscordCoreAPI::RepeatedFunctionData function01{};
	function01.function = onBoot;
	function01.repeated = false;
	function01.intervalInMs = 1500;
	functionVector.emplace_back(function01);
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
	Jsonifier::Vector<DiscordCoreAPI::ActivityData> activities{};
	DiscordCoreAPI::ActivityData activity{};
	activity.name = "/help for my commands!";
	activity.type = DiscordCoreAPI::ActivityType::Game;
	activities.emplace_back(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk = false;
	clientConfig.presenceData.status = DiscordCoreAPI::PresenceUpdateState::Online;
	clientConfig.textFormat = DiscordCoreAPI::TextFormat::Etf;
	clientConfig.presenceData.since = 0;
	auto the = makeUnique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
	DiscordCoreAPI::CreateGlobalApplicationCommandData createBotInfoCommandData{};
	createBotInfoCommandData.dmPermission = true;
	createBotInfoCommandData.type = DiscordCoreAPI::ApplicationCommandType::Chat_Input;
	createBotInfoCommandData.defaultMemberPermissions = DiscordCoreAPI::Permission::Use_Application_Commands;
	createBotInfoCommandData.description = "Displays info about the current bot.";
	createBotInfoCommandData.name = "botinfo";
	the->registerFunction(Jsonifier::Vector<std::string>{ "botinfo" }, DiscordCoreAPI::makeUnique<DiscordCoreAPI::BotInfo>(), createBotInfoCommandData);
	the->runBot();
	return 0;
};
