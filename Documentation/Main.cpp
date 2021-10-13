// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "Index.hpp"
#include "DatabaseEntities.hpp"
#include "Commands/CommandsList.hpp"

void onGuildCreation(DiscordCoreAPI::OnGuildCreationData dataPackage) {
    DiscordCoreAPI::DiscordUser::guildCount += 1;
}

void onGuildDeletion(DiscordCoreAPI::OnGuildDeletionData dataPackage) {
    DiscordCoreAPI::DiscordUser::guildCount -= 1;
}

void onBoot01() {
    auto botUser = DiscordCoreAPI::getBotUser();
    DiscordCoreAPI::DatabaseManagerAgent::initialize(botUser.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
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
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"botinfo"}, new DiscordCoreAPI::BotInfo);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"clear"}, new DiscordCoreAPI::Clear);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"disconnect"}, new DiscordCoreAPI::Disconnect);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"displayguildsdata"}, new DiscordCoreAPI::DisplayGuildsData);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"help"}, new DiscordCoreAPI::Help);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"loopall"}, new DiscordCoreAPI::LoopAll);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"loopsong"}, new DiscordCoreAPI::LoopSong);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"np"}, new DiscordCoreAPI::Np);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"play"}, new DiscordCoreAPI::Play);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"playq"}, new DiscordCoreAPI::PlayQ);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"playrn"}, new DiscordCoreAPI::PlayRN);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"pause"}, new DiscordCoreAPI::Pause);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"queue"}, new DiscordCoreAPI::Queue);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"registerapplicationcommands"}, new DiscordCoreAPI::RegisterApplicationCommands);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"setbordercolor"}, new DiscordCoreAPI::SetBorderColor);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"setmusicchannel"}, new DiscordCoreAPI::SetMusicChannel);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"skip"}, new DiscordCoreAPI::Skip);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"stop"}, new DiscordCoreAPI::Stop);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"test"}, new DiscordCoreAPI::Test);
    DiscordCoreAPI::DiscordCoreClient::thisPointer->runBot();
    return 0;
};
