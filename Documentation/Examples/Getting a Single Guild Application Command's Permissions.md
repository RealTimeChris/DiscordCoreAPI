
### **Getting a Single Guild Application Command's Permissions:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `getApplicationCommandPermissionsAsync()` function and execute it, while passing in an argument of type `DiscordCoreAPI::GetApplicationCommandPermissionsData`, with a return value of type `auto` or `GuildApplicationCommandPermissionData`.

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

			GetApplicationCommandPermissionsData dataPackage02;
			dataPackage02.guildId = args->eventData.getGuildId();
			dataPackage02.commandName = "botinfo";

			auto returnValue = ApplicationCommands::getApplicationCommandPermissions(dataPackage02);

			cout << returnValue.applicationId << endl;

			
		}
	};
}
```
