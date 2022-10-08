Following a News Channel {#followingnewschannel}
=============
- Execute, the `DiscordCoreAPI::Channels::followNewsChannelAsync()` function, while having passed in a data structure of type `DiscordCoreAPI::FollowNewsChannelData` with a return value of `auto` or `DiscordCoreAPI::Channel`.
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

		UniquePtr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::FollowNewsChannelData dataPackage;
				dataPackage.targetChannelId = args.eventData.getChannelId();
				dataPackage.channelId = "34565656332236657533";

				DiscordCoreAPI::Channel followChannel = Channels::followNewsChannelAsync(dataPackage).get();

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
