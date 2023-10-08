// main.cpp - Main entry point.
//// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "Commands/BotInfo.hpp"

void onBoot00(discord_core_api::discord_core_client* args) {
	auto botUser = args->getBotUser();
	//DiscordCoreAPI::managerAgent.initialize(botUser.id);
	//DiscordCoreAPI::DiscordUser theUser{ discord_core_api::managerAgent, { botUser.userName }, botUser.id };
	//theUser.writeDataToDB(discord_core_api::managerAgent);
}

discord_core_api::co_routine<void> onGuildCreation(discord_core_api::on_guild_creation_data dataPackage) { 
	co_await discord_core_api::newThreadAwaitable<void>();
	//DiscordCoreAPI::DiscordGuild discordGuild{ discord_core_api::managerAgent, dataPackage.value };
	//discordGuild.getDataFromDB(discord_core_api::managerAgent);
	//discordGuild.writeDataToDB(discord_core_api::managerAgent);
	co_return;
}

int32_t main() {
	jsonifier::string botToken = "";
	jsonifier::vector<discord_core_api::repeated_function_data> functionVector{};
	functionVector.reserve(5);
	discord_core_api::repeated_function_data function01{}; 
	function01.function		= onBoot00;
	function01.intervalInMs = 2500;
	function01.repeated		= false;
	functionVector.emplace_back(function01); 
	discord_core_api::sharding_options shardOptions{};
	shardOptions.numberOfShardsForThisProcess = 1;
	shardOptions.totalNumberOfShards		  = 1;
	shardOptions.startingShard				  = 0;
	discord_core_api::logging_options logOptions{};
	logOptions.logHttpsSuccessMessages	   = true;
	logOptions.logWebSocketErrorMessages   = true;
	logOptions.logGeneralErrorMessages	   = true;
	logOptions.logHttpsErrorMessages	   = true;
	logOptions.logWebSocketSuccessMessages = false;
	logOptions.logGeneralSuccessMessages   = true;
	discord_core_api::discord_core_client_config clientConfig{};
	clientConfig.logOptions						= logOptions;
	clientConfig.botToken						= botToken;
	clientConfig.shardOptions					= shardOptions;
	clientConfig.cacheOptions.cacheGuildMembers = false;
	clientConfig.cacheOptions.cacheChannels		= true;
	clientConfig.cacheOptions.cacheGuilds		= true;
	clientConfig.cacheOptions.cacheUsers		= false;
	clientConfig.cacheOptions.cacheRoles		= false;
	clientConfig.functionsToExecute				= functionVector;
	jsonifier::vector<discord_core_api::activity_data> activities{};
	discord_core_api::activity_data activity{};
	activity.name  = "/help for my commands!";
	activity.type = discord_core_api::activity_type::custom;
	activities.emplace_back(activity);
	clientConfig.presenceData.activities = activities; 
	auto result							 = clientConfig.presenceData.operator discord_core_api::discord_core_internal::etf_serializer(); 
	clientConfig.presenceData.afk		 = false;
	clientConfig.textFormat				 = discord_core_api::text_format::etf;
	clientConfig.presenceData.since		 = 0;
	clientConfig.presenceData.status	 = discord_core_api::presence_update_state::online;
	auto thePtr							 = discord_core_api::makeUnique<discord_core_api::discord_core_client>(clientConfig);
	thePtr->getEventManager().onGuildCreationEvent.add(onGuildCreation);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "botinfo" }, discord_core_api::makeUnique<discord_core_api::bot_info>(), {});
	thePtr->runBot();
	return 0;
};
