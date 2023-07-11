Getting a Guild Ban {#gettingaguildban}
============
- Execute the, `Guilds::getGuildBanAsync()` function, while passing in a value of type `GetGuildBanData`, with a return value of type `auto` or `BanData`.
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
				GetGuildBanData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.userId = "851629399514415106";

				auto guildBan = Guilds::getGuildBanAsync(dataPackage01).get();

				std::cout << "THE BAN NAME: " << guildBan.user.userName << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
