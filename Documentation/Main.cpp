// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "Index.hpp"
#include "Commands/CommandsList.hpp"

void onBoot01() {
    vector<DiscordCoreAPI::ActivityData> activities;
    DiscordCoreAPI::ActivityData activity;
    activity.name = "!help for my commands!";
    activity.type = DiscordCoreAPI::ActivityType::Game;
    activities.push_back(activity);
    DiscordCoreAPI::getBotUser().updatePresence({ .activities = activities, .status = "online",.afk = false });
}

int main()
{
    init_apartment();
    string botToken = "YOUR_BOT_TOKEN_HERE";
    vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = onBoot01;
    function01.intervalInMs = 500;
    function01.repeated = false;
    functionVector.push_back(function01);
    DiscordCoreAPI::DiscordCoreClient::thisPointer->setup(botToken, "!", &functionVector);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"test"}, new DiscordCoreAPI::Test);

    DiscordCoreAPI::DiscordCoreClient::thisPointer->runBot();
    return 0;
};
