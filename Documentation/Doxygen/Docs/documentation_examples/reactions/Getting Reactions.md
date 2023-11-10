Getting Reactions {#getting_reactions}
============
- Execute the, `discord_core_api::reactions::getReactionsAsync()` function, while passing in a data structure of type `discord_core_api::get_reactions_data`, with a return value of type `auto` or `jsonifier::vector<user>`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for its return value now.

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
				get_reactions_data& dataPackage;
				dataPackage.afterId = args.eventData.getAuthorId();
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.emoji = "😆";
				dataPackage.limit = 50;

				vector<user> usersResponse = reactions::getReactionsAsync(const dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
