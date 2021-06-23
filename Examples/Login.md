### **Login/Instantiation of the Library:**
---
- Install the dependencies and `#include Index.hpp`.
- Create a string with your bots token.
- Create a new shared pointer to an object of the `DiscordCoreAPI::DiscordCoreClient` class by executing the `DiscordCoreClient::finalSetup()` function.
- Execute `agent::wait()` on the `DiscordCoreClient` object.
- Collect and display any possible errors that pop up during the library's execution.
```cpp
// Main.cpp - Main source file, for "the framework".
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/Index.hpp"

int main() {
    winrt::init_apartment();
    string botToken = "YOUR_BOT_TOKEN_HERE";
    shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    agent::wait((agent*)pDiscordCoreClient.get());
    exception error;
    while (pDiscordCoreClient.get()->getError(error)) {
        cout << "DiscordCoreClient() Error: " << error.what() << endl << endl;
    }
    return 0;
}

```
