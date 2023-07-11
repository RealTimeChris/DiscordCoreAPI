Getting a Guild Prune Count {#gettingaguildprunecount}
============
- Select, from the `Guilds::getGuildPruneCountAsync()` function, while passing in a value of type `GetGuildPruneCountData`, with a return value of type `auto` or `GuildPruneCountData`.
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
				GetGuildPruneCountData dataPackage01;
				dataPackage01.days = 28;
				dataPackage01.roles.emplace_back("860075141818744853");
				dataPackage01.guildId = args.eventData.getGuildId();

				auto responseData = Guilds::getGuildPruneCountAsync(dataPackage01).get();

				std::cout << responseData.count << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
