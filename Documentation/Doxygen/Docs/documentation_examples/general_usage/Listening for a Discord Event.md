Listening for a Discord Event {#listeningforevent}
============
- a complete list of possible events is [here](https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-events).
- create a function that returns `co_routine<void>`, that takes an argument of a type that derives its name from the type of event that is being listened for, where the argument is of type `eventname+data`. for example, the argument for the `event_manager::onGuildCreation` event is `on_guild_creation_data`.
- the list of events can be found by dereferencing `discord_core_client::eventManager`, where you pass into the function found here, a pointer to your own created function.

```cpp
/// main.cpp-main entry point.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <index.hpp>

co_routine<void> onGuildCreation(on_guild_creation_data& dataPackage) {
	co_await newThreadAwaitable<void>();
	std::cout << "guild name: " << dataPackage.guild.data.name << std::endl;
	co_return;
}

int32_t main() {
	jsonifier::string botToken {"YOUR_BOT_TOKEN_HERE"};
	auto ptr = makeUnique<discord_core_client>(botToken, "!");
	ptr->eventManager->onGuildCreation(&onGuildCreation);
	ptr->runBot();
	return 0;
}
```
