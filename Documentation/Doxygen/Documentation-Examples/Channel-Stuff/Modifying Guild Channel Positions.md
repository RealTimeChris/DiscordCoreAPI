Modifying Guild Channel Positions {#modifyingguildchannelpositions}
=============
- Execute the, `Channels::modifyGuildChannelPositionsAsync()` function, while passing in a data structure of type `ModifyGuildChannelPositionsData`, with a return value of type `void`.
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
				vector<Channel> channels = Channels::getGuildChannelsAsync({.guildId = args.eventData.getGuildId()}).get();

				vector<ModifyGuildChannelPositionData> dataPackage00;

				ModifyGuildChannelPositionData dataPackage01;
				dataPackage01.id = channels.at(1).id;
				dataPackage01.lockPermissions = false;
				dataPackage01.parentId = channels.at(1).parentId;
				dataPackage01.position = channels.at(1).position-1;

				dataPackage00.emplace_back(dataPackage01);

				ModifyGuildChannelPositionsData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.modifyChannelData = dataPackage00;

				Channels::modifyGuildChannelPositionsAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
