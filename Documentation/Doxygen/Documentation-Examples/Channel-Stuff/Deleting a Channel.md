Deleting/Closing a Channel {#deletingchannel}
============
- Execute the `DiscordCoreAPI::Channels::deleteOrCloseChannelAsync()` function, while passing in a data structure of type `DiscordCoreAPI::DeleteOrCloseChannelData`, with a return value of type `Void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::DeleteOrCloseChannelData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.reason = "DELETING!";

				Channels::deleteOrCloseChannelAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
