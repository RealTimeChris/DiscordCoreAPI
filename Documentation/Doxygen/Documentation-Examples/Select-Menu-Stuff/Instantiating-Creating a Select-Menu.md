Instantiating/Creating a Select-Menu {#instantiatingcreatingselectmenu}
============
- Create a data structure of type `RespondToInputEventData`, and add either some content or a message embed.
- Add one or more select-menus by using the `RespondToInputEventData::addSelectMenu` function of the `RespondToInputEventData` structure.
- Call the `InputEvents::respondToInputEventAsync()` function, passing it the `RespondToInputEventData` structure.
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
		}
	};
}
```