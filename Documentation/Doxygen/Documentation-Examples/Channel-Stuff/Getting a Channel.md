Getting a Channel {#gettingchannel}
============
- Execute the `DiscordCoreAPI::Channels::getCachedChannelAsync()` (which collects it from the cache), or `getChannelAsync()` (which collects it from the Discord servers) function, while passing to it a data structure of type `DiscordCoreAPI::GetChannelData`, with a return type of `auto` or `DiscordCoreAPI::Channel`.
- Call the function with `.get()` added to the end in order to wait for the results now.
```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

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
			Channel channel = Channels::getCachedChannelAsync({args.eventData.getChannelId()}).get();

			Channel channel = Channels::getChannelAsync({args.eventData.getChannelId()}).get();
		}
	};
}
```
