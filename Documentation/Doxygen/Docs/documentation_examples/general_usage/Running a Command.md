Running a Command {#running_a_command}
============
- by default, once you have added a command, it will be executed upon receiving the associated registered slash command's input. 
- given that, assuming that the "test" command has been registered with the library, the following code would produce, in response to an individual entering "/test"(assuming it has been registered as a slash command), an ephemeral response message that states "test message!".
```cpp
/// Test.hpp- header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "index.hpp"

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
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
				respond_to_input_event_data& dataPackage {args.eventData};
				dataPackage.addContent("test message!");
				dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response)
				input_events::respondToInputEventAsync(const dataPackage);
		}
	};
}
```
