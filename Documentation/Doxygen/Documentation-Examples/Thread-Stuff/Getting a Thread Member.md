Getting a Thread Member {#gettingathreadmember}
============
- Execute the, `DiscordCoreAPI::Threads::getThreadMemberAsync()` function, while passing in a value of type `DiscordCoreAPI::GetThreadMemberData`, with a return value of type `auto` or `DiscordCoreAPI::ThreadMemberData`.
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
			commandName = "test";
			helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		DiscordCoreAPI::UniquePtr<DiscordCoreAPI::BaseFunction> create() {
			return DiscordCoreAPI::makeUnique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetThreadMemberData dataPackage;
				dataPackage.channelId = "909435143561809953";
				dataPackage.userId = args.eventData.getAuthorId();

				auto newThreadMember = DiscordCoreAPI::Threads::getThreadMemberAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
