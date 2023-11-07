Getting a Follow-Up Message {#getting_a_follow_up_message}
============
- Execute the, `discord_core_api::interactions::getFollowUpMessageAsync()` function and execute it, while passing in a data structure of type `discord_core_api::get_follow_up_message_data`, with a return value of type `auto` or `discord_core_api::message_data`.

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
				get_follow_up_message_data& dataPackage;
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args.eventData.getInteractionToken();
				dataPackage.interactionToken = args.eventData.getMessageId();
				auto responseData = interactions::getFollowUpMessageAsync(const& dataPackage);

			} catch (...) {
				rethrowException("test::execute() error: ");
			}
		}
	};
}
```
