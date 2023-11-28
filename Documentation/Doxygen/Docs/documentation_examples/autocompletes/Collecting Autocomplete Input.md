Collecting Autocomplete Input {#collecting_auto_complete_input}
============
- after creating an autocomplete - your bot will begin to receive interactions that are the result of inputs coming from the commands for which you enabled autocomplete - and you listen for these using the `discord_core_api::event_manager::onAutoCompleteEntry` event.
- create a function which returns void and takes an argument of type `discord_core_api::on_auto_complete_entry_data`, and use it to filter for and provide responses to various inputs.
- the user's current text inputs will come in on the `discord_core_api::input_event_data`'s `discord_core_api::interaction_data::data`'s, `discord_core_api::application_command_interaction_data_option` array of options.
- use these inputs in order to construct a `discord_core_api::respond_to_input_event_data` response, using the `discord_core_api::respond_to_input_event_data::setAutoCompleteChoice` function - and send it off to the discord servers in order to provide the user with suggestions for autocomplete.
- upon receiving a collection of choices, the user may choose one and respond, at which point you will receive it in the form of an incoming `discord_core_api::interaction_data` with the respective choices set.

```cpp
// main.cpp - main entry point.
// jun 17, 2021
// chris m.
// https://github.com/RealTimeChris

#include <discordcoreapi/index.hpp>

void theAutoCompleteFunction(on_auto_complete_entry_data dataPackage) {
	respond_to_input_event_data& dataPackageNew{ dataPackage.inputEvent };
	if (dataPackage.inputEvent.getInteractionData().data.applicationCommandData.options[0].valueString.find("tes") != jsonifier::string::npos) {
		dataPackageNew.setAutoCompleteChoice("the test value", "test_value_name");
		dataPackageNew.setResponseType(input_event_response_type::Application_Command_AutoComplete_Result);
		input_events::respondToInputEventAsync(dataPackageNew).get();
	} else {
		dataPackageNew.setResponseType(input_event_response_type::Application_Command_AutoComplete_Result);
		input_events::respondToInputEventAsync(dataPackageNew).get();
	}
}

int32_t main() {
	jsonifier::string botToken = "YOUR_BOT_TOKEN_HERE";
	discord_core_client client{ botToken };
	client.eventManager.onAutoCompleteEntry(&theAutoCompleteFunction);
	client.registerFunction(jsonifier::vector<jsonifier::string>{ "test" }, makeUnique<test>());
	client.runBot();
	return 0;
}
```
