Creating a Guild Template {#creating_a_guild_template}
=============
- Execute the, `discord_core_api::guilds::createGuildTemplateAsync()` function, while passing in a value of type `discord_core_api::create_guild_template_data`, with a return value of type `auto` or `discord_core_api::guild_template_data`.
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
				create_guild_template_data& dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "testing template!";
				dataPackage01.name = "test template!";

				auto responseData = guilds::createGuildTemplateAsync(dataPackage01).get();

				std::cout << "the name: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
