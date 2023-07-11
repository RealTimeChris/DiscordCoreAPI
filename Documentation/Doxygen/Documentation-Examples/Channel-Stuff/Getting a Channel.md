Getting a Channel {#gettingchannel}
============
- Execute the `Channels::getCachedChannel()` (which collects it from the cache), or `getChannelAsync()` (which collects it from the Discord servers) function, while passing to it a data structure of type `GetChannelData`, with a return type of `auto` or `Channel`.
- Call the function with `.get()` added to the end in order to wait for the results now.
```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

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
			Channel channel = Channels::getCachedChannel({args.eventData.getChannelId()}).get();

			Channel channel = Channels::getChannelAsync({args.eventData.getChannelId()}).get();
		}
	};
}
```
