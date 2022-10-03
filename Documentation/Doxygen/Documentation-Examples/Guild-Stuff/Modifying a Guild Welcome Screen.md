Modifying a Guild Welcome Screen {#modifyingaguildwelcomescreen}
============
- Execute the, `DiscordCoreAPI::Guilds::modifyGuildWelcomeScreenAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildWelcomeScreenData`, with a return value of type `auto` or `DiscordCoreAPI::WelcomeScreenData`.
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
				std::vector<DiscordCoreAPI::WelcomeScreenChannelData> theVector;
				DiscordCoreAPI::WelcomeScreenChannelData channelData;
				channelData.channelId = args.eventData.getChannelId();
				channelData.description = "TESTING CHANNEL!";
				channelData.emojiName = "🏳";
				theVector.emplace_back(channelData);

				DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage01;
				dataPackage01.reason = "TESTING PURPOSES";
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "THE DESCRIPTION!";
				dataPackage01.welcomeChannels = theVector;

				auto responseData = DiscordCoreAPI::Guilds::modifyGuildWelcomeScreenAsync(dataPackage01).get();

				cout << Boolalpha << responseData.welcomeChannels.at(0).channelId << endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
