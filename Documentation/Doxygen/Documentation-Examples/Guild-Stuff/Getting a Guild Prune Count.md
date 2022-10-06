Getting a Guild Prune Count {#gettingaguildprunecount}
============
- Select, from the `DiscordCoreAPI::Guilds::getGuildPruneCountAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildPruneCountData`, with a return value of type `auto` or `DiscordCoreAPI::GuildPruneCountData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetGuildPruneCountData dataPackage01;
				dataPackage01.days = 28;
				dataPackage01.roles.emplace_back("860075141818744853");
				dataPackage01.guildId = args.eventData.getGuildId();

				auto responseData = DiscordCoreAPI::Guilds::getGuildPruneCountAsync(dataPackage01).get();

				cout << responseData.count << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
