
### **Running a Function on a Timer:**
---
- Create a function who's argument is of type `shared_ptr<DiscordCoreAPI::DiscordCoreClient>`.
- Create a data value of type `vector<DiscordCoreAPI::RepeatedFunctionData>*`, and populate it with a new pointer of its own type.
- Create a data structure of type `DiscordCoreAPI::RepeatedFunctionData`, and fill out its members, and stuff it inside the vector.
- Pass this vector as a second argument to the `DiscordCoreAPI::DiscordCoreClient::finalSetup()` function - now it will run on a schedule defined by the `intervalInMS` member of the data structure.

```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/DiscordCoreClient02.hpp"

static void onBoot(shared_ptr<DiscordCoreAPI::DiscordCoreClient> dataPackage) {
    vector<DiscordCoreAPI::Guild> guilds = dataPackage->guilds->getAllGuildsAsync().get();
    for (auto value : guilds) {
        DiscordCoreAPI::DiscordGuild discordGuild(value.data);
        discordGuild.getDataFromDB();
        for (unsigned int x = 0; x < discordGuild.data.deletionChannels.size(); x += 1) {
            discordGuild.data.deletionChannels[x].currentlyBeingDeleted = false;
        }
        discordGuild.writeDataToDB();
    }
}

int main()
{
    init_apartment();
    string botToken = "YOUR_TOKEN_HERE";
    vector<DiscordCoreAPI::RepeatedFunctionData>* functionVector = new vector<DiscordCoreAPI::RepeatedFunctionData>;
    DiscordCoreAPI::RepeatedFunctionData function01;
    function01.function = onBoot;
    function01.repeated = false;
    function01.intervalInMs = 10000;
    functionVector->push_back(function01);
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken, functionVector);
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}

```
