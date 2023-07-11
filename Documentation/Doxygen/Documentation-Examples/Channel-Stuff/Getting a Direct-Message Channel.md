Getting a Direct-Message Channel {#gettingdmchannel}
============
- Execute the `Channels::createDMChannelAsync()` (which collects it from the Discord servers)function, while passing to it a data structure of type `CreateDMChannelData`, or simply `{ .userId = TARGETUSERIDHERE}`, with a return value of `auto` or `DiscordCoreAPI:Channel`.
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

		virtual void execute(std::unique_pt<BarseFunctionArguments> args) {
			Channel channel = Channels::createDMChannelAsync({.userId = args.eventData.getAuthorId()}).get();
		}
	};
}
```
