Modifying a Guild Role {#modifyingaguildrole}
=============
- Execute the, `roles::modifyGuildRoleAsync()` function, while passing in a value of type `modify_guild_role_data`, with a return value of type `auto` or `role`.
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
				modify_guild_role_data dataPackage01;
				dataPackage01.name = "test role";
				dataPackage01.roleId = "886366417316896799";
				dataPackage01.reason = "testing!";
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.hexColorValue = "fefe12";
				dataPackage01.mentionable = false;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto newRole = roles::modifyGuildRoleAsync(const dataPackage01).get();

				std::cout << "role name: " << newRole.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}


```
