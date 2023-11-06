Beginning a Guild Prune {#beginningaguildprune}
============
- Execute the,` guilds::beginGuildPruneAsync()` function, while passing in a value of type `begin_guild_prune_data`, with a return value of type `auto` or `guild_prune_count_data`.
- call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				begin_guild_prune_data dataPackage01;
				dataPackage01.days = 30;
				dataPackage01.includeRoles.emplace_back("860075141818744853");
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.computePruneCount = true;
				dataPackage01.reason = "test pruning!";

				auto responseData = guilds::beginGuildPruneAsync(const dataPackage01).get;

				std::cout << responseData.count << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}


```
