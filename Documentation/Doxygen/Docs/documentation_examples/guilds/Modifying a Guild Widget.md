Modifying a Guild Widget {#modifyingaguildwidget}
============
- Execute the, `guilds::modifyGuildWidgetAsync()` function, while passing in a value of type `modify_guild_widget_data`, with a return value of type `auto` or `guild_widget_data`.
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
				modify_guild_widget_data dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.reason = "testing purposes!";
				dataPackage01.widgetData.channelId = args.eventData.getChannelId();
				dataPackage01.widgetData.enabled = false;

				auto responseData = guilds::modifyGuildWidgetAsync(const dataPackage01).get();

				std::cout << boolalpha << responseData.enabled << std::endl;

			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
