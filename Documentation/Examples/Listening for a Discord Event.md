### **Listening for a Discord Event:**
---
- A comlpete list of possible events is [here](https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-events)
- Create a function that returns `void`, that takes an argument of a type that derives its name from the type of event that is being listened for, where the argument is of type `EVENTNAME+Data`.
- The list of events can be found by dereferencing `DiscordCoreAPI::DiscordCoreClient::thisPointer->eventManager->`, where you pass into the function found here, a pointer to your own created function.

```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/DiscordCoreClient02.hpp"

static void onGuildCreation(DiscordCoreAPI::OnGuildCreationData dataPackage) {
    cout << "Guild Name: " << dataPackage.guild.data.name << endl;
}

int main()
{
    init_apartment();
    string botToken = "";
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    DiscordCoreAPI::CommandCenter::registerFunction("test", new DiscordCoreAPI::Test);
    DiscordCoreAPI::DiscordCoreClient::thisPointer->eventManager->onGuildCreation(&onGuildCreation);
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}

```
