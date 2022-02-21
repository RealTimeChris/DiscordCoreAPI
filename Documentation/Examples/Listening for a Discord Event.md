### **Listening for a Discord Event:**
---
- A comlpete list of possible events is [here](https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-events).
- Create a function that returns `void`, that takes an argument of a type that derives its name from the type of event that is being listened for, where the argument is of type `DiscordCoreAPI::EVENTNAME+Data`. For example, the argument for the `onGuildCreation` event is `DiscordCoreAPI::OnGuildCreationData`.
- The list of events can be found by dereferencing `DiscordCoreAPI::DiscordCoreClient::eventManager->`, where you pass into the function found here, a pointer to your own created function.

```cpp
/// main.cpp - Main entry point.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <pch.h"
#include <./DiscordCoreAPI-main/DiscordCoreClient02.hpp>

static void onGuildCreation(DiscordCoreAPI::OnGuildCreationData dataPackage) {
    cout << "Guild Name: " << dataPackage.guild.data.name << endl;
}

int32_t main()
{
    init_apartment();
    std::string botToken{ "YOUR_BOT_TOKEN_HERE" };
    auto thePtr = make_unique<DiscordCoreAPI::DiscordCoreClient>(botToken, "!");
    thePtr->eventManager->onGuildCreation(&onGuildCreation);
    thePtr->runBot();
    return 0;
}
```
