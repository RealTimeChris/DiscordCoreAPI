Deleting a Follow-Up Message {#deleting_a_follow_up_message}
============
- Execute the, `discord_core_api::input_events::deleteInputEventResponseAsync()` function, while passing in a data structure of type `discord_core_api::input_event_data`, with a return value of type `void`.
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
				respond_to_input_event_data dataPackage {args.eventData};
				dataPackage.type = input_event_response_type::Follow_Up_Message;
				dataPackage.addContent("test content");
				auto result = input_events::respondToInputEventAsync(const dataPackage);
				auto responseData = input_events::deleteInputEventResponseAsync(const result);

			} catch (...) {
				rethrowException("test::execute() error: ");
			}
		}
	};
}
```
