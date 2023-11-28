Removing a Guild Role {#removing_a_guild_role}
=============
- Execute the, `discord_core_api::roles::removeGuildRoleAsync()` function, while passing in a value of type `discord_core_api::remove_guild_role_data`, with a return value of type `void`.
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
				remove_guild_role_data& dataPackage01;
				dataPackage01.roleId = "886366417316896799";
				dataPackage01.reason = "testing!";
				dataPackage01.guildId = args.eventData.getGuildId();

				roles::removeGuildRoleAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}


```
