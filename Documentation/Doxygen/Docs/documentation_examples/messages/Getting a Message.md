Getting a Message {#gettingamessage}
============
- Execute the, `messages::getMessageAsync()` function, while passing in a data structure of type `get_message_data`, with a return value of type `auto`, or, `message`.
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
				get_message_data dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.id = args.eventData.getMessageId();

				message responseMessages = messages::getMessageAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
