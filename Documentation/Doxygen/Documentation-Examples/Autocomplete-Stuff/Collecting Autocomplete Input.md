Collecting Autocomplete Input {#collectingautocompleteinput}
============
- After creating an autocomplete - your bot will begin to receive interactions that are the result of inputs coming from the commands for which you enabled autocomplete - and you listen for these using the `EventManager::onAutoCompleteEntry` event.
- Create a function which returns void and takes an argument of type `OnAutoCompleteEntryData`, and use it to filter for and provide responses to various inputs.
- The user's current text inputs will come in on the `InputEventData`'s `InteractionData::data`'s, `ApplicationCommandInteractionData::options` array of options.
- Use these inputs in order to construct a `RespondToInputEventData` response, using the `RespondToInputEventData::setAutoCompleteChoice` function - and send it off to the Discord servers in order to provide the user with suggestions for autocomplete.
- Upon receiving a collection of choices, the user may choose one and respond, at which point you will receive it in the form of an incoming `InteractionData` with the respective choices set.

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include <discordcoreapi/Index.hpp>

void theAutoCompleteFunction(OnAutoCompleteEntryData dataPackage) {
	RespondToInputEventData dataPackageNew{ dataPackage.inputEvent };
	if (dataPackage.inputEvent.getInteractionData().data.applicationCommandData.options[0].valueString.find("tes") != std::string::npos) {
		dataPackageNew.setAutoCompleteChoice("The Test Value", "test_value_name");
		dataPackageNew.setResponseType(InputEventResponseType::Application_Command_AutoComplete_Result);
		InputEvents::respondToInputEventAsync(dataPackageNew).get();
	} else {
		dataPackageNew.setResponseType(InputEventResponseType::Application_Command_AutoComplete_Result);
		InputEvents::respondToInputEventAsync(dataPackageNew).get();
	}
}

int32_t main() {
	std::string botToken = "YOUR_BOT_TOKEN_HERE";
	DiscordCoreClient client{ botToken };
	client.eventManager.onAutoCompleteEntry(&theAutoCompleteFunction);
	client.registerFunction(std::vector<std::string>{ "test" }, makeUnique<Test>());
	client.runBot();
	return 0;
}
```
