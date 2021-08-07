### **Login/Instantiation of the Library:**
---
- Install the dependencies and `#include DiscordCoreClient02.hpp`.
- Create a string with your bot's token.
- Run the `DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken)` function, using your bot token as an argument.
- Run the `DiscordCoreAPI::DiscordCoreClient::runBot()` function.

```cpp
// Main.cpp - Main source file, for "the framework".
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/DiscordCoreClient02.hpp"

int main()
{
    init_apartment();
    string botToken = "ODYwMTA1MDY3MzYwMjg4ODA5.YN2ZRA.U8G-Y78hLhFzBfL-VH8v0-zHhzI";
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    DiscordCoreAPI::DiscordCoreClient::runBot();
}

```
