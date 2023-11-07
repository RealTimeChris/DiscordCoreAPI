Editing a Message {#editing_a_message}
============
- Execute the, `discord_core_api::messages::editMessageAsync()` function, while passing in a data structure of type `discord_core_api::edit_message_data`, with a return value of type `discord_core_api::message_data`.
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
				edit_message_data dataPackage {args.eventData};
				dataPackage.addContent("test editing content!");

				message newMessage = messages::editMessageAsync(const dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
