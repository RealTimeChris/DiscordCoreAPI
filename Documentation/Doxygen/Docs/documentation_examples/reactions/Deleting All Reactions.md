Deleting All Reactions {#deletingallreactions}
============
- Execute the, `reactions::deleteAllReactionsAsync()` function, while passing in a data structure of type `delete_all_reactions_data`, with a return value of type `void`.
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
				delete_all_reactions_data dataPackage;
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.channelId = args.eventData.getChannelId();

				reactions::deleteAllReactionsAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
