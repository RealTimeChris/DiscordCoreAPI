
### **Getting Guild Application Command Permissions:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `getGuildApplicationCommandPermissionsAsync()` function and execute it, while passing in an argument of type `DiscordCoreAPI::GetGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `vector<GuildApplicationCommandPermissionData>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

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

			GetGuildApplicationCommandPermissionsData dataPackage;
			dataPackage.guildId = args->eventData.getGuildId();

			auto returnVector = ApplicationCommands::getGuildApplicationCommandPermissionsAsync(dataPackage).get();

			for (auto value : returnVector) {
				cout << value.applicationId << endl;
			}

			return;
		}
	};
}
```
