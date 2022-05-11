Collecting Select-Menu Input {#collectingselectmenuinput}
============
- After creating a button, create an object of the `DiscordCoreAPI::SelectMenuCollector` class, passing into its constructor the `DiscordCoreAPI::InputEventData` that resulted from the call to `DiscordCoreAPI::InputEvents::respondToInputEventAsync`, when the button was created.
- Call the `DiscordCoreAPI::SelectMenuCollector::collectSelectMenuData()` function from the instance of the SelectMenuCollector. NOTE: The arguments for this function are as follows:  
`getSelectMenuDataForAllNew` = Whether or not it accepts select-menu entries from everyone or just the individual selected with the `targetUser` argument.   
`maxWaitTimeInMsNew` = The maximum number of milliseconds that the collector will wait for select-menu entries.   
`maxCollectSelectMenuCountNew` = The maximum number of select-menu entries that the collector will collect.   
`targetUser` = The target user, if `getSelectMenuDataForAllNew` is disabled.
- Collect a result of type `std::vector<DiscordCoreAPI::SelectMenuResponseData>` and deal with the button responses as you see fit! Keep in mind that you could set up a voting message by using `getSelectMenuDataForAllNew` and having multiple entries allowed.
```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			DiscordCoreAPI::SelectOptionData selectOption;
			selectOption.label = "Select Menu";
			selectOption._default = true;
			selectOption.description = "Select Menu";
			selectOption.emoji.name = "🏁";
			selectOption.value = "select_menu";
			std::vector<DiscordCoreAPI::SelectOptionData> selectMenuOptions {selectOption};
			DiscordCoreAPI::RespondToInputEventData dataPackage {args.eventData};
			dataPackage.addSelectMenu(false, "test_button", selectMenuOptions, "Select-Menu", 1, 1);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData {.description = "TESTING!", .title = "Test Title"});
			dataPackage.type = DiscordCoreAPI::InputEventResponseType::Interaction_Response;

			auto inputEventData = DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage);

			DiscordCoreAPI::SelectMenuCollector selectMenuCollector {inputEventData};
			auto results = selectMenuCollector.collectSelectMenuData(true, 120000, 3, getBotUser().id).get();
			for (auto value: results) {
				cout << value.userId << endl;
			}
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(inputEventData).get();
		}
	};
}
```
