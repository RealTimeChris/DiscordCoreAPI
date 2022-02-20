
### **Collecting Select-Menu Input:**
---
- After creating a button, create an object of the `DiscordCoreAPI::SelectMenuCollector` class, passing into its constructor the `InputEventData` that resulted from the call to `InputEvents::respondToEvent`, when the button was created.
- Call, from the `DiscordCoreAPI::ButtonCollector` class, the `collectSelectMenuData` function. NOTE: The arguments for this function are as follows: `getSelectMenuDataForAllNew` = Whether or not it accepts select-menu entries from everyone or just the individual selected with the `targetUser` argument. `maxWaitTimeInMsNew` = The maximum number of milliseconds that the collector will wait for select-menu entries. `maxCollectSelectMenuCountNew` = The maximum number of select-menu entries that the collector will collect. `targetUser` = The target user, if `getSelectMenuDataForAllNew` is enabled.
- Collect a result of type `vector<SelectMenuResponseData>` and deal with the button responses as you see fit! Keep in mind that you could set up a voting message by using `getSelectMenuDataForAllNew` and having multiple entries allowed.
```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <Index>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(unique_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			RespondToInputEventData dataPackage{ args->eventData };
			dataPackage.type = InputEventResponseType::InteractionResponse;
			SelectOptionData selectOption{  };
			selectOption.description = "Select Menu";
			selectOption.emoji.name = "🏁";
			selectOption.label = "Select Menu"; 
			selectOption.value = "select_menu";
			selectOption._default = true;
			vector<SelectOptionData> selectMenuOptions{ selectOption };
			dataPackage.addSelectMenu(false, "test_button", selectMenuOptions, "Select-Menu", 1, 1);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData{ .description = "TESTING!",.title = "Test Title" });
			auto inputEventData = InputEvents::respondToEvent(dataPackage);

			SelectMenuCollector buttonCollector(inputEventData);
			auto results = buttonCollector.collectSelectMenuData(true, 120000, 3, getBotUser().id);
			for (auto value : results) {
				cout << value.userId << endl;
			}
			InputEvents::deleteInputEventResponseAsync(inputEventData).get();

			return;
		}
	};
}
#endif
```
