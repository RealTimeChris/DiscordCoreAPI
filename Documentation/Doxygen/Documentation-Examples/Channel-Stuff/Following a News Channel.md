Following a News Channel {#followingnewschannel}
=============
- Execute, the `Channels::followNewsChannelAsync()` function, while having passed in a data structure of type `FollowNewsChannelData` with a return value of `auto` or `Channel`.
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
				FollowNewsChannelData dataPackage;
				dataPackage.targetChannelId = args.eventData.getChannelId();
				dataPackage.channelId = "34565656332236657533";

				Channel followChannel = Channels::followNewsChannelAsync(dataPackage).get();

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
