Adding a Guild Member {#adding_a_guild_member}
============
- Execute the, `discord_core_api::guild_members::addGuildMemberAsync()` function, while passing in a value of type `discord_core_api::add_guild_member_data`, with a return value of type `auto` or `discord_core_api::guild_member_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

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
				add_guild_member_data dataPackage;
				dataPackage.accessToken = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.nick = "real_time chris";

				auto guildMember = guild_members::addGuildMemberAsync(const dataPackage).get();

				std::cout << "the name: " << guildMember.user.userName << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
