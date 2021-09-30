### **Instantiating/Creating a Select-Menu:**
---
- Create a data structure of type `DiscordCoreAPI::RespondToInputEventData`, and add either some content or a message embed.
- Add one or more buttons by using the `addSelectMenu` function of the `DiscordCoreAPI::RespondToInputEventData` structure.
- Call the `InputEvents::respondToEvent` function, passing it the `DiscordCoreAPI::RespondToInputEventData` structure.
```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

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

		Test* create() {
			return new Test;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			RespondToInputEventData dataPackage{ args->eventData };
			dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
			SelectOptionData selectOption{  };
			selectOption.description = "Select Menu";
			selectOption.emoji.name = "🏁";
			selectOption.label = "Select Menu"; 
			selectOption.value = "select_menu";
			selectOption._default = true;
			vector<SelectOptionData> selectMenu{ selectOption };
			dataPackage.addSelectMenu(false, "test_button", selectMenu, "Select-Menu", 1, 1);
			dataPackage.addContent("Test Response");
			dataPackage.addMessageEmbed(EmbedData{ .description = "TESTING!",.title = "Test Title" });
			auto inputEventData = InputEvents::respondToEvent(dataPackage);

			SelectMenuCollector buttonCollector(inputEventData);
			auto results = buttonCollector.collectSelectMenuData(true, 120000, 3, getBotUser().id);
			for (auto value : results) {
				cout << value.userId << endl;
			}
			InputEvents::deleteInputEventResponseAsync(inputEventData).get();

			co_return;
		}
	};
}
#endif
```
