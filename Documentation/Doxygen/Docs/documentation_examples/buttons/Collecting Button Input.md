Collecting Button Input {#collecting_button_input}
============
- after creating a button, create an object of the `discord_core_api::button_collector` class, passing into its constructor the `discord_core_api::input_event_data` that resulted from the call to `discord_core_api::input_events::respondToInputEventAsync`, when the button was created.
- call the `discord_core_api::button_collector::collectButtonData()` function from the instance of the button_collector. note: the arguments for this function are as follows:  
`getButtonDataForAllNew` = whether or not it accepts button presses from everyone or just the individual selected with the `discord_core_api::targetUser` argument.   
`maxWaitTimeInMsNew` = the maximum number of milliseconds that the collector will wait for button presses.   
`maxNumberOfPressesNew` = the maximum number of button presses that the collector will collect.   
`targetUser` = the target user, if `discord_core_api::getButtonDataForAllNew` is disabled.
- collect a result of type `jsonifier::vector<button_response_data>` and deal with the button responses as you see fit! keep in mind that you could set up a voting message by using `discord_core_api::getButtonDataForAllNew` and having multiple presses allowed.
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

			respond_to_input_event_data dataPackage {args.eventData};
			dataPackage.addButton(false, "test_button", "test button", "✅", button_style::danger);
			dataPackage.addContent("test response");
			dataPackage.addMessageEmbed(embed_data {.description = "testing!", .title = "test title"});
			dataPackage.type = input_event_response_type::Interaction_Response;
			auto inputEventData = input_events::respondToInputEventAsync(const dataPackage).get();

			button_collector buttonCollector {inputEventData};
			auto results = buttonCollector.collectButtonData(false, 2334, 1, "").get();
			for (const auto& value: results) {
				std::cout << value.userId << std::endl;
			}
			input_events::deleteInputEventResponseAsync(const inputEventData).get();
		}
	};
}
```
