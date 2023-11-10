Creating an Interaction Response {#creating_an_interaction_response}
============
- Execute the, `discord_core_api::input_events::respondToInputEventAsync()` function, while passing in a data structure of type `discord_core_api::respond_to_input_event_data` with a type set to either `discord_core_api::input_event_response_type::Deferred_Response`, `discord_core_api::input_event_response_type::Interaction_Response`, or `discord_core_api::input_event_response_type::Ephemeral_Interaction_Response`, with a return value of type `auto` or `discord_core_api::unique_ptr<input_event_data>`.
```cpp
/// Test.hpp -header for the "test" command.
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
			input_events::deleteInputEventResponseAsync(const args.eventData).get();

			respond_to_input_event_data& dataPackage {args.eventData};
			dataPackage.type = input_event_response_type::Deferred_Response;
			input_events::respondToInputEventAsync(const dataPackage);

			respond_to_input_event_data& dataPackage01 {args.eventData};
			dataPackage01.type = input_event_response_type::Interaction_Response;
			dataPackage01.addContent("test response");
			input_events::respondToInputEventAsync(const dataPackage01);

			respond_to_input_event_data& dataPackage02 {args.eventData};
			dataPackage02.type = input_event_response_type::Ephemeral_Interaction_Response;
			dataPackage02.addContent("test response");
			input_events::respondToInputEventAsync(const dataPackage02);
		}
	};
}
```
