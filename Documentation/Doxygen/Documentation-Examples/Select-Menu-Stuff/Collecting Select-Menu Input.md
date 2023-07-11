Collecting Select-Menu Input {#collectingselectmenuinput}
============
- After creating a button, create an object of the `SelectMenuCollector` class, passing into its constructor the `InputEventData` that resulted from the call to `InputEvents::respondToInputEventAsync`, when the button was created.
- Call the `SelectMenuCollector::collectSelectMenuData()` function from the instance of the SelectMenuCollector. NOTE: The arguments for this function are as follows:  
`getSelectMenuDataForAllNew` = Whether or not it accepts select-menu entries from everyone or just the individual selected with the `targetUser` argument.   
`maxWaitTimeInMsNew` = The maximum number of milliseconds that the collector will wait for select-menu entries.   
`maxCollectSelectMenuCountNew` = The maximum number of select-menu entries that the collector will collect.   
`targetUser` = The target user, if `getSelectMenuDataForAllNew` is disabled.
- Collect a result of type `std::vector<SelectMenuResponseData>` and deal with the button responses as you see fit! Keep in mind that you could set up a voting message by using `getSelectMenuDataForAllNew` and having multiple entries allowed.
```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			SelectOptionData selectOption;
			selectOption.label = "Select Menu";
			selectOption._default = true;
			selectOption.description = "Select Menu";
			selectOption.emoji.name = "🏁";
			selectOption.value = "select_menu";
			std::vector<SelectOptionData> selectMenuOptions {selectOption};
			RespondToInputEventData dataPackage {args.eventData};
			dataPackage.addSelectMenu(false, "test_button", selectMenuOptions, "Select-Menu", 1, 1);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData {.description = "TESTING!", .title = "Test Title"});
			dataPackage.type = InputEventResponseType::Interaction_Response;

			auto inputEventData = InputEvents::respondToInputEventAsync(dataPackage);

			SelectMenuCollector selectMenuCollector {inputEventData};
			auto results = selectMenuCollector.collectSelectMenuData(true, 120000, 3, getBotUser().id).get();
			for (auto value: results) {
				std::cout << value.userId << std::endl;
			}
			InputEvents::deleteInputEventResponseAsync(inputEventData).get();
		}
	};
}
```
