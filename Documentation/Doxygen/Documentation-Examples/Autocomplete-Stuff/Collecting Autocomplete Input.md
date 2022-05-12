Collecting Autocomplete Input {#collectingautocompleteinput}
============
- After creating an autocomplete - your bot will begin to receive interactions that are the result of inputs coming from the commands for which you enabled autocomplete - and you listen for these using the `DiscordCoreAPI::EventManager::onAutoCompleteEntry` event.

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include <discordcoreapi/Index.hpp>

void theAutoCompleteFunction(DiscordCoreAPI::OnAutoCompleteEntryData dataPackage) {
	DiscordCoreAPI::RespondToInputEventData dataPackageNew{ dataPackage.inputEvent };
	if (dataPackage.inputEvent.getInteractionData().data.applicationCommanddata.options[0].valueString.find("tes") != std::string::npos) {
		dataPackageNew.setAutoCompleteChoice(nullptr, nullptr, std::make_unique<std::string>("The Test Value"), "test_value_name");
		dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Application_Command_AutoComplete_Result);
		DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackageNew).get();
	}
	else {
		dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Application_Command_AutoComplete_Result);
		DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackageNew).get();
	}
}

int32_t main() {
	std::string botToken = "YOUR_BOT_TOKEN_HERE";
	DiscordCoreAPI::DiscordCoreClient thePtr{ botToken };
	thePtr.eventManager.onAutoCompleteEntry(&theAutoCompleteFunction);
	thePtr.registerFunction(std::vector<std::string>{ "test" }, std::make_unique<DiscordCoreAPI::Test>());
	thePtr.runBot();
	return 0;
}

```
