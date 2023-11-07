Instantiating/Creating a Modal-Input {#creating_a_modal}
============
- create a data structure of type `discord_core_api::respond_to_input_event_data`.
- add a modal-text-input using the `discord_core_api::respond_to_input_event_data::addModal` function of the `discord_core_api::respond_to_input_event_data` structure.
- call the `discord_core_api::input_events::respondToInputEventAsync` function, passing it the `discord_core_api::respond_to_input_event_data` structure.
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
			try {
				respond_to_input_event_data& dataPackage {args.eventData};
				dataPackage.addModal("test modal", "test_modal", "test modal small", "test_modal", true, 1, 46, text_input_style::paragraph, "test modal",
									 "test_modal");
				dataPackage.type = input_event_response_type::Interaction_Response;
				auto newEvent = input_events::respondToInputEventAsync(const& dataPackage);


			} catch (...) {
				reportException("test::execute()");
			}
		}
		virtual ~test() = default;
	};
}
```