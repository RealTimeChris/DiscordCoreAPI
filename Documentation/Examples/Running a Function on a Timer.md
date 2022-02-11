
### **Running a Function on a Timer:**
---
- Create a function who's argument is of type `shared_ptr<DiscordCoreAPI::DiscordCoreClient>`, with a return type of `void`.
- Create a data value of type `vector<DiscordCoreAPI::RepeatedFunctionData>`.
- Create a data value of type `DiscordCoreAPI::RepeatedFunctionData`, and fill out its members, and stuff it inside the vector. Be sure to set `repeated` to true if you would like the function call to be recurring.
- Pass a reference to this vector as a third argument to the `DiscordCoreAPI::DiscordCoreClient::finalSetup()` function - now the functions it contains will run on a schedule defined by the `intervalInMs` member of the data structure.
- Do this with as many functions as you would like!

```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/DiscordCoreClient02.hpp"

static void onBoot(shared_ptr<DiscordCoreAPI::DiscordCoreClient> dataPackage) {
    vector<DiscordCoreAPI::Guild> guilds = DiscordCoreAPI::Guilds::getAllGuildsAsync().get();
    for (auto value : guilds) {
        DiscordCoreAPI::DiscordGuild discordGuild(value.data);
        discordGuild.getDataFromDB();
        for (unsigned int32_t x = 0; x < discordGuild.data.deletionChannels.size(); x += 1) {
            discordGuild.data.deletionChannels[x].currentlyBeingDeleted = false;
        }
        discordGuild.writeDataToDB();
    }
}

int32_t main()
{
    init_apartment();
    string botToken = "YOUR_TOKEN_HERE";
    vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = onBoot;
    function01.repeated = false;
    function01.intervalInMs = 10000;
    functionVector.push_back(function01);
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken, "!", &functionVector);
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}

```
