// main.cpp - Main entry point.
//// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "Commands/BotInfo.hpp"

void onBoot00(DiscordCoreAPI::DiscordCoreClient* args) {
	auto botUser = args->getBotUser();
	//DiscordCoreAPI::managerAgent.initialize(botUser.id);
	//DiscordCoreAPI::DiscordUser theUser{ DiscordCoreAPI::managerAgent, { botUser.userName }, botUser.id };
	//theUser.writeDataToDB(DiscordCoreAPI::managerAgent);
}

DiscordCoreAPI::CoRoutine<void> onGuildCreation(DiscordCoreAPI::OnGuildCreationData dataPackage) {
	co_await DiscordCoreAPI::NewThreadAwaitable<void>();
	//DiscordCoreAPI::DiscordGuild discordGuild{ DiscordCoreAPI::managerAgent, dataPackage.value };
	//discordGuild.getDataFromDB(DiscordCoreAPI::managerAgent);
	//discordGuild.writeDataToDB(DiscordCoreAPI::managerAgent);
	co_return;
}

int32_t main() {
	jsonifier::string botToken = "";
	jsonifier::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector{};
	functionVector.reserve(5);
	DiscordCoreAPI::RepeatedFunctionData function01{};
	function01.function		= onBoot00;
	function01.intervalInMs = 2500;
	function01.repeated		= false;
	functionVector.emplace_back(function01);
	DiscordCoreAPI::ShardingOptions shardOptions{};
	shardOptions.numberOfShardsForThisProcess = 1;
	shardOptions.totalNumberOfShards		  = 1;
	shardOptions.startingShard				  = 0;
	DiscordCoreAPI::LoggingOptions logOptions{};
	logOptions.logHttpsSuccessMessages	   = true;
	logOptions.logWebSocketErrorMessages   = true;
	logOptions.logGeneralErrorMessages	   = true;
	logOptions.logHttpsErrorMessages	   = true;
	logOptions.logWebSocketSuccessMessages = false;
	logOptions.logGeneralSuccessMessages   = true;
	DiscordCoreAPI::DiscordCoreClientConfig clientConfig{};
	clientConfig.logOptions						= logOptions;
	clientConfig.botToken						= botToken;
	clientConfig.shardOptions					= shardOptions;
	clientConfig.cacheOptions.cacheGuildMembers = false;
	clientConfig.cacheOptions.cacheChannels		= true;
	clientConfig.cacheOptions.cacheGuilds		= true;
	clientConfig.cacheOptions.cacheUsers		= false;
	clientConfig.cacheOptions.cacheRoles		= false;
	clientConfig.functionsToExecute				= functionVector;
	jsonifier::vector<DiscordCoreAPI::ActivityData> activities{};
	DiscordCoreAPI::ActivityData activity{};
	activity.name  = "/help for my commands!";
	activity.type  = DiscordCoreAPI::ActivityType::Custom;
	activities.emplace_back(activity);
	clientConfig.presenceData.activities = activities;
	auto result							 = clientConfig.presenceData.operator DiscordCoreAPI::DiscordCoreInternal::EtfSerializer();
	clientConfig.presenceData.afk		 = false;
	clientConfig.textFormat				 = DiscordCoreAPI::TextFormat::Etf;
	clientConfig.presenceData.since		 = 0;
	clientConfig.presenceData.status	 = DiscordCoreAPI::PresenceUpdateState::Online;
	auto thePtr							 = DiscordCoreAPI::makeUnique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
	thePtr->getEventManager().onGuildCreationEvent.add(onGuildCreation);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "botinfo" }, DiscordCoreAPI::makeUnique<DiscordCoreAPI::BotInfo>(), {});
	thePtr->runBot();
	return 0;
};
