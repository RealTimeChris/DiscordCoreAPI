### **Modifying Guild Channel Positions:**
---
- Access within the `DiscordCoreAPI` namespace, the `Channels` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildChannelPositionsData`.
- Select, from the `Channels` class, the `modifyGuildChannelPositionsAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

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

		virtual void executeAsync(unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				vector<Channel> channels = Channels::getGuildChannelsAsync({ .guildId = args->eventData.getGuildId() }).get();

				vector<ModifyGuildChannelPositionData> dataPackage00{};

				ModifyGuildChannelPositionData dataPackage01{};
				dataPackage01.id = channels.at(1).id;
				dataPackage01.lockPermissions = false;
				dataPackage01.parentId = channels.at(1).parentId;
				dataPackage01.position = channels.at(1).position - 1;

				dataPackage00.push_back(dataPackage01);

				ModifyGuildChannelPositionsData dataPackage{};
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.modifyChannelData = dataPackage00;

				Channels::modifyGuildChannelPositionsAsync(dataPackage).get();

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
