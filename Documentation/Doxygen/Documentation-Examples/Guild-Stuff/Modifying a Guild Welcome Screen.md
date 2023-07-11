Modifying a Guild Welcome Screen {#modifyingaguildwelcomescreen}
============
- Execute the, `Guilds::modifyGuildWelcomeScreenAsync()` function, while passing in a value of type `ModifyGuildWelcomeScreenData`, with a return value of type `auto` or `WelcomeScreenData`.
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
				std::vector<WelcomeScreenChannelData> vector;
				WelcomeScreenChannelData channelData;
				channelData.channelId = args.eventData.getChannelId();
				channelData.description = "TESTING CHANNEL!";
				channelData.emojiName = "🏳";
				vector.emplace_back(channelData);

				ModifyGuildWelcomeScreenData dataPackage01;
				dataPackage01.reason = "TESTING PURPOSES";
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "THE DESCRIPTION!";
				dataPackage01.welcomeChannels = vector;

				auto responseData = Guilds::modifyGuildWelcomeScreenAsync(dataPackage01).get();

				std::cout << Boolalpha << responseData.welcomeChannels.at(0).channelId << std::endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
