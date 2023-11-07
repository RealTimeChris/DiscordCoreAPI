Instantiating/Creating a Button {#creating_a_button}
============ 
- create a data structure of type `discord_core_api::respond_to_input_event_data`, and add either some content or a message embed.- add one or more buttons by using the `discord_core_api::respond_to_input_event_data::addButton` of the `discord_core_api::respond_to_input_event_data` structure.
- call the `discord_core_api::input_events::respondToInputEventAsync()` function, passing it the `discord_core_api::respond_to_input_event_data` structure.
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
			dataPackage.addButton(false, "test_button", "test button", "✅", button_style::danger);
			dataPackage.addContent("test response");
			dataPackage.addMessageEmbed(embed_data {.description = "testing!", .title = "test title"});
			dataPackage.type = input_event_response_type::Interaction_Response;
			auto inputEventData = input_events::respondToInputEventAsync(const& dataPackage);
		}
	};
}
```
