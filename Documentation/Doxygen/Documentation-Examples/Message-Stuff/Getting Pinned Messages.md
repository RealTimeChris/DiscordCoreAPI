Getting Pinned Messages {#gettingpinnedmessages}
============

- Execute the, `Messages::getPinnedMessagesAsync()` function, while passing in a value of type `GetPinnedMessagesData`, with a return value of type `auto` or `vector<Message>`.
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
			EmbedData msgEmbed { };
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
				GetPinnedMessagesData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				vector<Message> messagesVector = Messages::getPinnedMessagesAsync(dataPackage).get();

				for (auto value: messagesVector) {
					std::cout << "THE ID: " << value.id << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
