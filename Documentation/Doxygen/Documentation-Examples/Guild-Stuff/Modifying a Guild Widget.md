Modifying a Guild Widget {#modifyingaguildwidget}
============
- Execute the, `Guilds::modifyGuildWidgetAsync()` function, while passing in a value of type `ModifyGuildWidgetData`, with a return value of type `auto` or `GuildWidgetData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				ModifyGuildWidgetData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.reason = "TESTING PURPOSES!";
				dataPackage01.widgetData.channelId = args.eventData.getChannelId();
				dataPackage01.widgetData.enabled = false;

				auto responseData = Guilds::modifyGuildWidgetAsync(dataPackage01).get();

				std::cout << Boolalpha << responseData.enabled << std::endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
