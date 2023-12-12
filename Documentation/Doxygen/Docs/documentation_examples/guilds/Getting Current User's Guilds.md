Getting Current User's Guilds {#getting_current_users_guilds}
============
- Execute the, `discord_core_api::guilds::getCurrentUserGuildsAsync()` function, while passing in a value of type `discord_core_api::get_current_user_guilds_data`, with a return value of type `auto` or `jsonifier::vector<guild_data>`.
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
				get_current_user_guilds_data& dataPackage;
				dataPackage.limit = 2;

				auto currentOutput = guilds::getCurrentUserGuildsAsync(dataPackage).get();

				for (auto& value: currentOutput) {
					std::cout << "guild_data name: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}

		virtual ~test() = default;
	};
}


```
