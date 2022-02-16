### **Modifying a Guild Welcome Screen:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildWelcomeScreenData`.
- Select, from the `Guilds` class, the `mnodifyGuildWelcomeScreenAsync()` function and execute it, with a return value of type `auto` or `GuildWelcomeScreenData`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {

				vector<WelcomeScreenChannelData> theVector{};
				WelcomeScreenChannelData channelData{};
				channelData.channelId = args.eventData.getChannelId();
				channelData.description = "TESTING CHANNEL!";
				channelData.emojiName = "🏳";
				theVector.push_back(channelData);

				ModifyGuildWelcomeScreenData dataPackage01{};
				dataPackage01.reason = "TESTING PURPOSES";
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "THE DESCRIPTION!";
				dataPackage01.welcomeChannels = theVector;

				auto responseData = Guilds::modifyGuildWelcomeScreenAsync(dataPackage01).get();

				cout << boolalpha << responseData.welcomeChannels.at(0).channelId << endl;
				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
