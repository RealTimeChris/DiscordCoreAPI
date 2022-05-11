Pinning a Message {#pinningamessage}
============
- Execute the, `DiscordCoreAPI::Messages::pinMessageAsync()` function, while passing in a value of type `DiscordCoreAPI::PinMessageData`, with a return value of type `void` while passing it the created data structure.
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
				DiscordCoreAPI::PinMessageData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.messageId = args.eventData.getMessageId();

				DiscordCoreAPI::Messages::pinMessageAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
