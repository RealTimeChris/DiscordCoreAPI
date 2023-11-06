Searching Guild Members {#searchingguildmembers}
============
- Execute the, `guild_members::searchGuildMembersAsync()` function, while passing in a value of type `search_guild_members_data`, with a return value of type `auto` or `vector<guild_member>`.
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
				search_guild_members_data dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.limit = 20;
				dataPackage.query = "real";

				auto guildMembers = guild_members::searchGuildMembersAsync(const& dataPackage).get();

				for (const auto& value: guildMembers) {
					std::cout << "the name: " << value.user.userName << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
