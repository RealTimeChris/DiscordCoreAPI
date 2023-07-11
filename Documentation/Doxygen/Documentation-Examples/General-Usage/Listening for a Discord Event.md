Listening for a Discord Event {#listeningforevent}
============
- A complete list of possible events is [here](https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-events).
- Create a function that returns `CoRoutine<void>`, that takes an argument of a type that derives its name from the type of event that is being listened for, where the argument is of type `EVENTNAME+Data`. For example, the argument for the `EventManager::onGuildCreation` event is `OnGuildCreationData`.
- The list of events can be found by dereferencing `DiscordCoreClient::eventManager`, where you pass into the function found here, a pointer to your own created function.

```cpp
/// main.cpp-Main entry point.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <Index.hpp>

CoRoutine<void> onGuildCreation(OnGuildCreationData dataPackage) {
	co_await NewThreadAwaitable<void>();
	std::cout << "Guild Name: " << dataPackage.guild.data.name << std::endl;
	co_return;
}

int32_t main() {
	std::string botToken {"YOUR_BOT_TOKEN_HERE"};
	auto ptr = makeUnique<DiscordCoreClient>(botToken, "!");
	ptr->eventManager->onGuildCreation(&onGuildCreation);
	ptr->runBot();
	return 0;
}
```
