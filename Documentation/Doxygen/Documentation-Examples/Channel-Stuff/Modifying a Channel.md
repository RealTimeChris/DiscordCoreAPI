Modifying a Channel {#modifyingchannel}
============
- Execute the `Channels::modifyChannelAsync()` function, while passing to it a completed data structure `ModifyChannelData`, with a return value of `auto` or `Channel`.
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
				Channel channel = Channels::getCachedChannel({.channelId = args.eventData.getChannelId()}).get();
				ModifyChannelData dataPackage {channel};
				dataPackage.channelData.name = "TEST UPDATE";
				Channel channelNew = Channels::modifyChannelAsync(dataPackage).get();

			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
