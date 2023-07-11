Getting Guild Bans {#gettingguildbans}
============
- Execute the, `Guilds::getGuildBansAsync()` function, while passing in a value of type `GetGuildBansData`, with a return value of type `auto` or `vector<BanData>`.
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
				GetGuildBansData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto guildBans = Guilds::getGuildBansAsync(dataPackage01).get();

				for (auto value: guildBans) {
					std::cout << "THE BAN NAME: " << value.user.userName << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
