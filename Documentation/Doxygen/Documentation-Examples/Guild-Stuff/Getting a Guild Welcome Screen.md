Getting a Guild Welcome Screen {#gettingaguildwelcomescreen}
=============
- Execute the, `Guilds::getGuildWelcomeScreenAsync()` function, while passing in a value of type `GetGuildWelcomeScreenData`, with a return value of type `auto` or `WelcomeScreenData`.
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
				GetGuildWelcomeScreenData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto responseData = Guilds::getGuildWelcomeScreenAsync(dataPackage01).get();

				std::cout << Boolalpha << responseData.welcomeChannels.at(0).channelId << std::endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
