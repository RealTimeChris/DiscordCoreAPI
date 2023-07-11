Beginning a Guild Prune {#beginningaguildprune}
============
- Execute the,` Guilds::beginGuildPruneAsync()` function, while passing in a value of type `BeginGuildPruneData`, with a return value of type `auto` or `GuildPruneCountData`.
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
				BeginGuildPruneData dataPackage01;
				dataPackage01.days = 30;
				dataPackage01.includeRoles.emplace_back("860075141818744853");
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.computePruneCount = true;
				dataPackage01.reason = "TEST PRUNING!";

				auto responseData = Guilds::beginGuildPruneAsync(dataPackage01).get;

				std::cout << responseData.count << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
