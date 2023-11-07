Instantiating/Creating a Select-Menu {#creating_a_select_menu}
============
- create a data structure of type `discord_core_api::respond_to_input_event_data`, and add either some content or a message embed.
- add one or more select-menus by using the `discord_core_api::respond_to_input_event_data::addSelectMenu` function of the `discord_core_api::respond_to_input_event_data` structure.
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

			select_option_data selectOption;
			selectOption.label = "select menu";
			selectOption._default = true;
			selectOption.description = "select menu";
			selectOption.emoji.name = "🏁";
			selectOption.value = "select_menu";
			jsonifier::vector<select_option_data> selectMenuOptions {selectOption};
			respond_to_input_event_data dataPackage {args.eventData};
			dataPackage.addSelectMenu(false, "test_button", selectMenuOptions, "select-menu", 1, 1);
			dataPackage.addContent("test response");
			dataPackage.addMessageEmbed(embed_data {.description = "testing!", .title = "test title"});
			dataPackage.type = input_event_response_type::Interaction_Response;

			auto inputEventData = input_events::respondToInputEventAsync(const dataPackage);
		}
	};
}
```