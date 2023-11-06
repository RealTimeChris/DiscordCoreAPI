Bulk Deleting Messages {#bulkdeletingmessages}
============
- Execute the, `messages::deleteMessagesBulkAsync()` function, while passing in a data structure of type `delete_messages_bulk_data`, with a return value of type `void`.
- call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				delete_messages_bulk_data dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				vector<string> messageIds { };
				messageIds.emplace_back(args.eventData.getMessageId());
				dataPackage.messageIds = messageIds;

				messages::deleteMessagesBulkAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
