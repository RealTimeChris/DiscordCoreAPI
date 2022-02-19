
### **Editing a Guild Application Command's Permissions:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `editApplicationCommandPermissions()` function and execute it, while passing in a data structure of type `EditApplicationCommandPermissionsData`, with a return value of type `auto` or `GuildApplicationCommandPermissionsData`.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

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

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			DiscordCoreAPI::EditApplicationCommandPermissionsData dataPackage;
			ApplicationCommandPermissionData dataPackage02{ .type = ApplicationCommandPermissionType::User,.permission = true, .id = "859853159115259905" };
			dataPackage.commandName = "botinfo";
			dataPackage.permissions.push_back(dataPackage02);
			dataPackage.guildId = args->eventData.getGuildId();

			auto returnValue = ApplicationCommands::editApplicationCommandPermissions(dataPackage);

			cout << returnValue.applicationId << endl;

			co_return;
		}
	};
}
#endif
```
