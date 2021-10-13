// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "Index.hpp"
#include "Commands/CommandsList.hpp"

void onGuildCreation(DiscordCoreAPI::OnGuildCreationData dataPackage) {

}

void onGuildDeletion(DiscordCoreAPI::OnGuildDeletionData dataPackage) {
}

void onBoot01() {
    auto botUser = DiscordCoreAPI::getBotUser();
}

void onBoot02() {
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
    DiscordCoreAPI::RepeatedFunctionData function02;
    function02.function = onBoot02;
    function02.intervalInMs = 5000;
    function02.repeated = false;
    functionVector.push_back(function02);
    DiscordCoreAPI::DiscordCoreClient::thisPointer->setup(botToken, "!", &functionVector);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"test"}, new DiscordCoreAPI::Test);
    DiscordCoreAPI::DiscordCoreClient::thisPointer->runBot();
    return 0;
};
