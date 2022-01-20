// Main.cpp - Main entry point.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "Index.hpp"

void onBoot01(DiscordCoreAPI::DiscordCoreClient* args) {
    std::vector<DiscordCoreAPI::ActivityData> activities;
    DiscordCoreAPI::ActivityData activity;
    activity.name = "!help for my commands!";
    activity.type = DiscordCoreAPI::ActivityType::Game;
    activities.push_back(activity);
    args->getBotUser().updatePresence({ .activities = activities, .status = "online",.afk = false });
}

int32_t main()
{
    std::string botToken = "YOUR_BOT_TOKEN_HERE";
    std::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = &onBoot01;
    function01.repeated = false;
    function01.intervalInMs = 500;
    functionVector.push_back(function01);
    auto thePtr = make_unique<DiscordCoreAPI::DiscordCoreClient>(botToken, "!", functionVector, DiscordCoreAPI::CacheOptions{ .cacheGuildMembers = true, .cacheChannels = true, .cacheGuilds = true, .cacheRoles = true, .cacheUsers = true });
    thePtr->registerFunction(std::vector<std::string>{"test"}, std::make_unique<DiscordCoreAPI::Test>());
    thePtr->runBot();
    return 0;
}
