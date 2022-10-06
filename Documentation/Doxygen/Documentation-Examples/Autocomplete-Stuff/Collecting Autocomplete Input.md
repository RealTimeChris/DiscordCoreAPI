Collecting Autocomplete Input {#collectingautocompleteinput}
============
- After creating an autocomplete - your bot will begin to receive interactions that are the result of inputs coming from the commands for which you enabled autocomplete - and you listen for these using the `DiscordCoreAPI::EventManager::onAutoCompleteEntry` event.
- Create a function which returns void and takes an argument of type `DiscordCoreAPI::OnAutoCompleteEntryData`, and use it to filter for and provide responses to various inputs.
- The user's current text inputs will come in on the `DiscordCoreAPI::InputEventData`'s `DiscordCoreAPI::InteractionData::data`'s, `DiscordCoreAPI::ApplicationCommandInteractionData::options` array of options.
- Use these inputs in order to construct a `DiscordCoreAPI::RespondToInputEventData` response, using the `DiscordCoreAPI::RespondToInputEventData::setAutoCompleteChoice` function - and send it off to the Discord servers in order to provide the user with suggestions for autocomplete.
- Upon receiving a collection of choices, the user may choose one and respond, at which point you will receive it in the form of an incoming `DiscordCoreAPI::InteractionData` with the respective choices set.

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include <discordcoreapi/Index.hpp>

void theAutoCompleteFunction(DiscordCoreAPI::OnAutoCompleteEntryData dataPackage) {
	DiscordCoreAPI::RespondToInputEventData dataPackageNew{ dataPackage.inputEvent };
	if (dataPackage.inputEvent.getInteractionData().data.applicationCommandData.options[0].valueString.find("tes") != String::npos) {
		dataPackageNew.setAutoCompleteChoice("The Test Value", "test_value_name");
		dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Application_Command_AutoComplete_Result);
		DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackageNew).get();
	} else {
		dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Application_Command_AutoComplete_Result);
		DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackageNew).get();
	}
}

Int32 main() {
	String botToken = "YOUR_BOT_TOKEN_HERE";
	DiscordCoreAPI::DiscordCoreClient theClient{ botToken };
	theClient.eventManager.onAutoCompleteEntry(&theAutoCompleteFunction);
	theClient.registerFunction(std::vector<String>{ "test" }, std::make_unique<DiscordCoreAPI::Test>());
	theClient.runBot();
	return 0;
}
```
