Removing a User from a Thread {#removing_a_user_from_a_thread}
============
- Execute the, `discord_core_api::threads::removeThreadMemberAsync()` function, while passing in a value of type `discord_core_api::remove_thread_member_data`, with a return value of type `void`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

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
				remove_thread_member_data dataPackage;
				dataPackage.channelId = "909407162776834069";
				dataPackage.userId = args.eventData.getAuthorId();

				threads::removeThreadMemberAsync(const dataPackage).get();

			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
