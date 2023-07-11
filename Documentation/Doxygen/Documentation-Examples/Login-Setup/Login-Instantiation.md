Login/Instantiation {#logininstantiation}
============
- <U><B>NOTE FOR SHARDING:</B></U> Make sure to set the `ShardingOptions` options of the `DiscordCoreClientConfig` structure.
- Install the dependencies, and `#include <Index.hpp>`.
- Create a string with your bot's token.
- Create an object of type `DiscordCoreClient`, using an instance of `DiscordCoreClientConfig` as the argument - being sure to at least set the `DiscordCoreClientConfig::botToken` member of it.
- Run the `DiscordCoreClient::runBot()` function.

```cpp
/// main.cpp-Main entry point.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <Index.hpp>

int main()
{
	DiscordCoreClientConfig clientConfig{};
	clientConfig.botToken = "YOUR_BOT_TOKEN_HERE";
	std::vector<ActivityData> activities{};
	ActivityData activity{};
	activity.name = "/help for my commands!";
	activity.type = ActivityType::Game;
	activities.push_back(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk = false;
	clientConfig.presenceData.since = 0;
	clientConfig.presenceData.status = "online";
	auto ptr = makeUnique<DiscordCoreClient>(clientConfig);
}
```
