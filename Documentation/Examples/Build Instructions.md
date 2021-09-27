### **Build Instructions:**
---
- Install the dependencies.
- Create a new "Console Application" project in Visual Studio 2019.
- Add, to the project folder the `DiscordCoreAPI-main` folder from the unzipped folder of this repo.
- Create a pch.h header with the following contents: [Precompiled Header](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Precompiled%20Header.md)
- Create a main.cpp with the following include:
```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI-main/DiscordCoreClient02.hpp"

void onBoot(shared_ptr<DiscordCoreAPI::DiscordCoreClient> dataPackage) {
    vector<DiscordCoreAPI::ActivityData> activities;
    DiscordCoreAPI::ActivityData activity;
    activity.name = "!help for my commands!";
    activity.type = DiscordCoreAPI::ActivityType::Game;
    activities.push_back(activity);
    dataPackage->currentUser.updatePresence({ .activities = activities, .status = "online",.afk = false });
}

int main()
{
    init_apartment();
    string botToken = "ODcxOTQzMDgyODA1OTczMDE1.YQiqRg.dyOU_Jp2NvpSpE2_8aFZeAi-R-s";
    vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = onBoot;
    function01.intervalInMs = 5000;
    function01.repeated = false;
    functionVector.push_back(function01);
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken, "!", &functionVector);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"botinfo"}, new DiscordCoreAPI::BotInfo);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"clear"}, new DiscordCoreAPI::Clear);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"disconnect"}, new DiscordCoreAPI::Disconnect);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"displayguildsdata"}, new DiscordCoreAPI::DisplayGuildsData);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"help"}, new DiscordCoreAPI::Help);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"loopall"}, new DiscordCoreAPI::LoopAll);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"loopsong"}, new DiscordCoreAPI::LoopSong);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"np"}, new DiscordCoreAPI::Np);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"play"}, new DiscordCoreAPI::Play);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"playq"}, new DiscordCoreAPI::PlayQ);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"playrn"}, new DiscordCoreAPI::PlayRN);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"pause"}, new DiscordCoreAPI::Pause);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"queue"}, new DiscordCoreAPI::Queue);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"registerapplicationcommands"}, new DiscordCoreAPI::RegisterApplicationCommands);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"setbordercolor"}, new DiscordCoreAPI::SetBorderColor);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"setmusicchannel"}, new DiscordCoreAPI::SetMusicChannel);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"skip"}, new DiscordCoreAPI::Skip);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"stop"}, new DiscordCoreAPI::Stop);
    DiscordCoreAPI::CommandCenter::registerFunction(vector<string>{"test"}, new DiscordCoreAPI::Test);
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}
```
