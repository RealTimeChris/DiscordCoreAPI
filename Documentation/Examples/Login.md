### **Login/Instantiation of the Library:**
---
- Install the dependencies, create your [precompiled header](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Precompiled%20Header.md) and `#include "./DiscordCoreAPI/DiscordCoreClient02.hpp"`.
- Create a string with your bot's token.
- Run the `DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken)` function, using your bot token as an argument, as well as a string to act as a "command prefix" for accepting commands via message input.
- Run the `DiscordCoreAPI::DiscordCoreClient::runBot()` function.

```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/DiscordCoreClient02.hpp"

int main()
{
    init_apartment();
    string botToken = "";
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken, "!");
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}

```
