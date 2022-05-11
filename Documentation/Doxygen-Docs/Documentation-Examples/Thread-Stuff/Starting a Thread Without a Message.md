Starting a Thread without a Message {#startingathreadwithoutamessage}
============
- Execute the, from the `DiscordCoreAPI::Threads::startThreadWithoutMessageAsync()` function, while passing in a value of type `DiscordCoreAPI::StartThreadWithoutMessageData`, with a return value of type `auto` or `DiscordCoreAPI::Thread`.
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
			DiscordCoreAPI::EmbedData msgEmbed { };
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
			try {
				DiscordCoreAPI::StartThreadWithoutMessageData dataPackage;
				dataPackage.invitable = true;
				dataPackage.type = ThreadType::GUILD_PUBLIC_THREAD;
				dataPackage.reason = "TESTING REASONS!";
				dataPackage.autoArchiveDuration = DiscordCoreAPI::ThreadAutoArchiveDuration::SHORT;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.threadName = "NEW THREAD";

				DiscordCoreAPI::Channel newThread = DiscordCoreAPI::Threads::startThreadWithoutMessageAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
