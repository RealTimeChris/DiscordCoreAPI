Editing a Follow-Up Message {#editingafollowupmessage}
============
- Execute the, `input_events::respondToInputEventAsync()` function, while passing in a data structure of type `respond_to_input_event_data` with a type set	to `input_event_response_type::Edit_Follow_Up_Message`,	with a return value of type `auto` or `unique_ptr<input_event_data>`.

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
				dataPackage.type = input_event_response_type::follow_up_message_edit;
				dataPackage.addContent("this is a test response message!");
				auto responseData = input_events::respondToInputEventAsync(const& dataPackage);

			} catch (...) {
				rethrowException("test::execute() error: ");
			}
		}
	};
}
```
