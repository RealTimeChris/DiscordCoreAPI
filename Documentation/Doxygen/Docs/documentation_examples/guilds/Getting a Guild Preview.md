Getting a Guild Preview {#gettingaguildpreview}
============
- Execute the, `guilds::getGuildPreviewAsync()` function, while passing in a value of type `get_guild_preview_data`, with a return value of type `auto` or `guild_preview_data`.
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
				get_guild_preview_data dataPackage00;
				dataPackage00.guildId = args.eventData.getGuildId();

				auto guild = guilds::getGuildPreviewAsync(const dataPackage00).get();

				std::cout << "the name: " << guild.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
