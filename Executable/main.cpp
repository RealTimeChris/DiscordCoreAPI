/// main.cpp - Main entry point.
/// Jun 17, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <../Include/Index.hpp>
#include "Commands/CommandsList.hpp"

void onBoot02(DiscordCoreAPI::DiscordCoreClient* thePtr) {
    std::vector<DiscordCoreAPI::ActivityData> activities{};
    DiscordCoreAPI::ActivityData activity{};
    activity.name = "!help for my commands!";
    activity.type = DiscordCoreAPI::ActivityType::Game;
    activities.push_back(activity);
    thePtr->getBotUser().updatePresence({ .activities = activities, .status = "online",.afk = false });
}

int32_t main()
{
    std::string botToken = "YOUR_BOT_TOKEN_HERE";
    std::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = onBoot02;
    function01.intervalInMs = 500;
    function01.repeated = false;
    functionVector.push_back(function01);
    auto thePtr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(botToken, "!", functionVector, DiscordCoreAPI::CacheOptions{ .cacheGuildMembers = true, .cacheChannels = true, .cacheGuilds = true, .cacheRoles = true, .cacheUsers = true });
    thePtr->registerFunction(std::vector<std::string>{"registerapplicationcommands"}, std::make_unique<DiscordCoreAPI::RegisterApplicationCommands>());
    thePtr->registerFunction(std::vector<std::string>{"playrn"}, std::make_unique<DiscordCoreAPI::PlayRN>());
    thePtr->registerFunction(std::vector<std::string>{"test"}, std::make_unique<DiscordCoreAPI::Test>());
    thePtr->runBot();
    return 0;
}
