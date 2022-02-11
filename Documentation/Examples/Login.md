### **Login/Instantiation of the Library:**
---
- Install the dependencies, and `#include Index.hpp"`.
- Create a string with your bot's token.
- Create a pointer of type `DiscordCoreAPI:DiscordCoreClient`, using your bot token as an argument, as well as a string to act as a "command prefix" for accepting commands via message input.
- Run the `DiscordCoreAPI::DiscordCoreClient::runBot()` function.

```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "Index.hpp"

int32_t main()
{
    std::string botToken{ "YOUR_BOT_TOKEN_HERE" };
    auto thePtr = make_unique<DiscordCoreAPI::DiscordCoreClient>(botToken, "!");
    thePtr->runBot();
    return 0;
}

```
