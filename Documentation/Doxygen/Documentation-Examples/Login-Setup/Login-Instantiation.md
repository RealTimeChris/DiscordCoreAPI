Login/Instantiation {#logininstantiation}
============
- <U><B>NOTE FOR SHARDING:</B></U> Make sure to pass in the 5th argument to the `DiscordCoreAPI::DiscordCoreClient` constructor which is a structure of type `DiscordCoreAPI::ShardingOptions`.
- Install the dependencies, and `#include <Index.hpp>`.
- Create a string with your bot's token.
- Create an object of type `DiscordCoreAPI::DiscordCoreClient`, using your bot token as an argument.
- Additionally - there are 4 optional arguments to the constructor which offer various potential configuration options, they are as follows:   
`std::vector<DiscordCoreAPI::RepeatedFunctionData>` - For adding potentailly recurring functions.   
`DiscordCoreAPI::LoggingOptions` - For setting the log-to-cout options.   
`DiscordCoreAPI::CacheOptions` - For setting the caching options.   
`DiscordCoreAPI::ShardingOptions` - For large bots - to set the sharding options.   
- Run the `DiscordCoreAPI::DiscordCoreClient::runBot()` function.

```cpp
/// main.cpp-Main entry point.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <Index.hpp>

int32_t main()
{
	std::string botToken {"YOUR_BOT_TOKEN_HERE"};
	std::unique_ptr<DiscordCoreAPI::DiscordCoreClient> thePtr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(
		botToken, functionVector,
		DiscordCoreAPI::CacheOptions {.cacheGuildMembers = true, .cacheChannels = true, .cacheGuilds = true, .cacheRoles = true, .cacheUsers = true});
	thePtr->runBot();
	return 0;
}
```