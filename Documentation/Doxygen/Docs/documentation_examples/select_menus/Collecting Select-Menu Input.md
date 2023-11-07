Collecting Select-Menu Input {#collecting_select_menu_input}
============
- after creating a button, create an object of the `discord_core_api::select_menu_collector` class, passing into its constructor the `discord_core_api::input_event_data` that resulted from the call to `discord_core_api::input_events::respondToInputEventAsync`, when the button was created.
- call the `discord_core_api::select_menu_collector::collectSelectMenuData()` function from the instance of the select_menu_collector. note: the arguments for this function are as follows:  
`getSelectMenuDataForAllNew` = whether or not it accepts select-menu entries from everyone or just the individual selected with the `discord_core_api::targetUser` argument.   
`maxWaitTimeInMsNew` = the maximum number of milliseconds that the collector will wait for select-menu entries.   
`maxCollectSelectMenuCountNew` = the maximum number of select-menu entries that the collector will collect.   
`targetUser` = the target user, if `discord_core_api::getSelectMenuDataForAllNew` is disabled.
- collect a result of type `jsonifier::vector<select_menu_response_data>` and deal with the button responses as you see fit! keep in mind that you could set up a voting message by using `discord_core_api::getSelectMenuDataForAllNew` and having multiple entries allowed.
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

			select_option_data selectOption;
			selectOption.label = "select menu";
			selectOption._default = true;
			selectOption.description = "select menu";
			selectOption.emoji.name = "🏁";
			selectOption.value = "select_menu";
			jsonifier::vector<select_option_data> selectMenuOptions {selectOption};
			respond_to_input_event_data& dataPackage {args.eventData};
			dataPackage.addSelectMenu(false, "test_button", selectMenuOptions, "select-menu", 1, 1);
			dataPackage.addContent("test response");
			dataPackage.addMessageEmbed(embed_data {.description = "testing!", .title = "test title"});
			dataPackage.type = input_event_response_type::Interaction_Response;

			auto inputEventData = input_events::respondToInputEventAsync(const& dataPackage);

			select_menu_collector selectMenuCollector {inputEventData};
			auto results = selectMenuCollector.collectSelectMenuData(true, 120000, 3, getBotUser().id).get();
			for (const auto& value: results) {
				std::cout << value.userId << std::endl;
			}
			input_events::deleteInputEventResponseAsync(const inputEventData).get();
		}
	};
}
```
