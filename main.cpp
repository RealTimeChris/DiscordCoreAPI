// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "Index.hpp"
#include "Commands/CommandList.hpp"

void onBoot01() {
    std::vector<DiscordCoreAPI::ActivityData> activities;
    DiscordCoreAPI::ActivityData activity;
    activity.name = "!help for my commands!";
    activity.type = DiscordCoreAPI::ActivityType::Game;
    activities.push_back(activity);
    DiscordCoreAPI::UpdatePresenceData updateData{};
    updateData.activities = activities;
    updateData.afk = false;
    updateData.status = "online";
    DiscordCoreAPI::getBotUser().updatePresence(updateData);
}

int main()
{
    winrt::init_apartment();
    std::string botToken = "YOUR_BOT_TOKEN_HERE";
    std::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = onBoot01;
    function01.intervalInMs = 500;
    function01.repeated = false;
    functionVector.push_back(function01);
    DiscordCoreAPI::CacheOptions cacheOptions{};
    cacheOptions.cacheChannels = true;
    cacheOptions.cacheGuildMembers = true;
    cacheOptions.cacheGuilds = true;
    cacheOptions.cacheRoles = true;
    cacheOptions.cacheUsers = true;
    DiscordCoreAPI::DiscordCoreClient::setup(botToken, "!", functionVector, cacheOptions);
    DiscordCoreAPI::CommandController::registerFunction(std::vector<std::string>{"test"}, std::move(std::make_unique<DiscordCoreAPI::Test>()));

    DiscordCoreAPI::DiscordCoreClient::thisPointer->runBot();
    return 0;
};

