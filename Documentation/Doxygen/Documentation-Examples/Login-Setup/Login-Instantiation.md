Login/Instantiation {#logininstantiation}
============
- <U><B>NOTE FOR SHARDING:</B></U> Make sure to set the `DiscordCoreAPI::ShardingOptions` options of the `DiscordCoreAPI::DiscordCoreClientConfig` structure.
- Install the dependencies, and `#include <Index.hpp>`.
- Create a string with your bot's token.
- Create an object of type `DiscordCoreAPI::DiscordCoreClient`, using an instance of `DiscordCoreAPI::DiscordCoreClientConfig` as the argument - being sure to at least set the `DiscordCoreAPI::DiscordCoreClientConfig::botToken` member of it.
- Run the `DiscordCoreAPI::DiscordCoreClient::runBot()` function.

```cpp
/// main.cpp-Main entry point.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <Index.hpp>

int main()
{
	DiscordCoreAPI::DiscordCoreClientConfig clientConfig{};
	clientConfig.botToken = "YOUR_BOT_TOKEN_HERE";
	std::vector<DiscordCoreAPI::ActivityData> activities{};
	DiscordCoreAPI::ActivityData activity{};
	activity.name = "/help for my commands!";
	activity.type = DiscordCoreAPI::ActivityType::Game;
	activities.push_back(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk = false;
	clientConfig.presenceData.since = 0;
	clientConfig.presenceData.status = "online";
	auto ptr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
}
```
