Bulk Deleting Messages {#bulkdeletingmessages}
============
- Execute the, `Messages::deleteMessagesBulkAsync()` function, while passing in a data structure of type `DeleteMessagesBulkData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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
				DeleteMessagesBulkData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				vector<string> messageIds { };
				messageIds.emplace_back(args.eventData.getMessageId());
				dataPackage.messageIds = messageIds;

				Messages::deleteMessagesBulkAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
