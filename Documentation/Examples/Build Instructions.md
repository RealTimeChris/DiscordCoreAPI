### **Build Instructions:**
---
- Install the [dependencies](https://github.com/RealTimeChris/DiscordCoreAPI#dependencies).
- Create a new "Console Application" project in Visual Studio 2019, and set the project [settings](https://github.com/RealTimeChris/DiscordCoreAPI#dependencies).
- Add, to the project folder the `DiscordCoreAPI-main` folder from the unzipped folder of this repo.
- Create a pch.h header with the following contents: [Precompiled Header](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Precompiled%20Header.md)
- Create a main.cpp with the following include:
```cpp
// main.cpp - Main entry point.
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI-main/DiscordCoreClient02.hpp"

int main()
{
    init_apartment();
    string botToken = "YOUR_BOT_TOKEN_HERE";
    DiscordCoreAPI::DiscordCoreClient::runBot();
    return 0;
}
```
