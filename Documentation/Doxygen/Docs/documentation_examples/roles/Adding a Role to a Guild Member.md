Adding a Role to a Guild Member {#adding_a_role_to_a_guild_member}
============
- Execute the, `discord_core_api::roles::addGuildMemberRoleAsync()` function, while passing in a value of type `discord_core_api::add_guild_member_role_data`, with a return value of type `void`.
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
			embed_data msgEmbed { };
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
				add_guild_member_role_data& dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.userId = args.eventData.getAuthorId();
				dataPackage01.reason = "testing purposes!";
				dataPackage01.roleId = "866124519303020554";

				roles::addGuildMemberRoleAsync(const dataPackage01).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
