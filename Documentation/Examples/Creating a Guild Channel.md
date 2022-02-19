### **Creating a Guild Channel:**
---
- Access within the `DiscordCoreAPI` namespace, the `Channels` class.
- Populate a data structure of type `DiscordCoreAPI::CreateGuildChannelData`.
- Select, from the `Channels` class, the `createGuildChannelAsync()` function and execute it, with a return value of type `auto` or `Channel`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

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

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				CreateGuildChannelData dataPackage{};
				dataPackage.type = ChannelType::GUILD_TEXT;
				dataPackage.name = "TEST CHANNEL";
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.reason = "TESTING PURPOSES!";

				vector<Channel> channels = Channels::getGuildChannelsAsync({ .guildId = args->eventData.getGuildId() }).get();

				for (auto value : channels) {
					if (value.type == ChannelType::GUILD_CATEGORY) {
						dataPackage.parentId = value.id;
						break;
					}
				}

				Channel channel = Channels::createGuildChannelAsync(dataPackage).get();

				cout << "THE NAME: " << channel.name << endl;

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
