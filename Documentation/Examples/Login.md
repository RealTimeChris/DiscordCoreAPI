### **Login/Instantiation of the Library:**
---
- Install the dependencies, and `#include Index.hpp"`.
- Create a string with your bot's token.
- Run the `DiscordCoreAPI::DiscordCoreClient::setup()` function, using your bot token as an argument, as well as a string to act as a "command prefix" for accepting commands via message input.
- Run the `DiscordCoreAPI::DiscordCoreClient::runBot()` function.

```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "Index.hpp"

int main()
{
    init_apartment();
    string botToken = "YOUR_TOKEN_HERE";
    DiscordCoreAPI::DiscordCoreClient::setup(botToken, "!");
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}

```
